/**
 * ========================================================================
 * @file work_proc.cpp
 * @brief
 * @author henry（韩林），henry@taomee.com
 * @version 1.0.0
 * @date 2011-06-30
 * Modify $Date: $
 * Modify $Author: $
 * copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <new>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/mman.h>
#include "work_proc.h"
#include "../util.h"

using namespace std;

extern c_pack g_pack;

/**
 * @brief  创建work_proc进程的一个实例
 * @param  pp_instance
 * @return  0success -1 failed
 */
int create_work_proc_instance(i_work_proc **pp_instance)
{
    if(pp_instance == NULL)
    {
        return -1;
    }

    c_work_proc *p_instance = new (std::nothrow)c_work_proc();
    if(p_instance == NULL)
    {
        return -1;
    }
    else
    {
        *pp_instance = dynamic_cast<i_work_proc*>(p_instance);
        return 0;
    }
}

c_work_proc::c_work_proc()
{
    m_inited = 0;
    m_p_mysql_conn = NULL;
    m_errno = ERR_NO_ERR;
}

c_work_proc::~c_work_proc()
{
    uninit();
}

int c_work_proc::release()
{
    delete this;
    return 0;
}

int c_work_proc::init(int argc, char ** argv)
{
    if (m_inited)
    {
        ERROR_LOG("work proc inited");
        return -1;
    }

    //读取配置
    char *db_host = config_get_strval("db_host");
    if(db_host == NULL)
    {
        ERROR_LOG("read config(db_host) failed.");
        return -1;
    }

    char *db_name = config_get_strval("db_name");
    if(db_name == NULL)
    {
        ERROR_LOG("read config(db_name) failed.");
        return -1;
    }

    char *db_user = config_get_strval("db_user");
    if(db_user == NULL)
    {
        ERROR_LOG("read config(db_user) failed.");
        return -1;
    }

    char *db_pass = config_get_strval("db_pass");
    if(db_pass == NULL)
    {
        ERROR_LOG("read config(db_pass) failed.");
        return -1;
    }

    int db_port = config_get_intval("db_port", 3306);
    if (db_port <= 0 || db_port >= 65535)
    {
        ERROR_LOG("db_port must between 0 and 65535");
        return -1;
    }

    // 连接到数据库
    if (0 != create_mysql_iface_instance(&m_p_mysql_conn))
    {
        ERROR_LOG("create mysql iface failed.");
        return -1;
    }
	if (0 != m_p_mysql_conn->init(db_host, db_port, db_name, db_user, db_pass, "utf8"))
	{
		ERROR_LOG("mysql init failed (%s)", m_p_mysql_conn->get_last_errstr());
        m_p_mysql_conn->release();
        m_p_mysql_conn = NULL;
		return -1;
	}

    m_oldest_idx = -1;
    init_maiji_pinboard();

    m_p_rc = NULL;
    if(connect_to_redis_svr() != 0)
    {
        ERROR_LOG("connect to redis svr failed.");
        return -1;
    }
    memset(m_redis_command, 0, sizeof(m_redis_command));


    const char *show_file_prefix = config_get_strval("show_file_prefix");
    if(show_file_prefix == NULL)
    {
        ERROR_LOG("Not found conf show_file_prefix");
        return -1;
    }
    sprintf(m_show_file_prefix, "%s", show_file_prefix);
    m_show_file_size = sizeof(show_user_num_t) + SHOW_USER_NUM * sizeof(show_user_info_t);
    if(init_show_file() != 0)
    {
        return -1;
    }

    m_inited = 1;
    return 0;
}


int c_work_proc::uninit()
{
    if (!m_inited)
    {
        return -1;
    }


    if(m_p_cur_top_start)
    {
        munmap(m_p_cur_top_start, m_show_file_size);
        m_p_cur_top_start = NULL;
    }

    if(m_p_last_top_start)
    {
        munmap(m_p_last_top_start, m_show_file_size);
        m_p_last_top_start = NULL;
    }

    memset(m_redis_command, 0, sizeof(m_redis_command));
    if(m_p_rc)
    {
        redisFree(m_p_rc);
        m_p_rc = NULL;
    }

    if(m_p_mysql_conn)
    {
        m_p_mysql_conn->uninit();
        m_p_mysql_conn->release();
        m_p_mysql_conn = NULL;
    }

    m_oldest_idx = -1;

    m_inited = 0;
    return 0;
}

int c_work_proc::process(char * req, int req_len , char ** ack, int * ack_len)
{
    svr_msg_header_t * p_msg_header = (svr_msg_header_t *)req;
    uint32_t len = p_msg_header->len;
    m_seq_id = p_msg_header->seq_id;
    m_msg_type = p_msg_header->msg_type;
    m_user_id = p_msg_header->user_id;

    *ack = m_response_buffer;

    g_pack.begin(SEND_TO_SVR, (char *)*ack);
    g_pack.pack(m_seq_id);
    g_pack.pack(m_msg_type);
    g_pack.pack((uint32_t)0);
    g_pack.pack(m_user_id);

    KDEBUG_LOG(m_user_id, "[%u:%u] recv.len = %u", m_msg_type, m_seq_id, len);

    if(len != (uint32_t)req_len)
    {
        KCRIT_LOG(m_user_id, "msg_len:%u, req_len:%u", len, req_len);
        m_errno = ERR_MSG_LEN;
    }
    else
    {

        char *p_request_body = req + sizeof(svr_msg_header_t);

        m_errno = ERR_NO_ERR;

        switch(m_msg_type)
        {
            case svr_msg_db_get_pinboard_info:
                m_errno = process_get_maiji_pinboard(p_request_body, len - sizeof(svr_msg_header_t));
                break;
            case svr_msg_db_add_message:
                process_add_maiji_pinboard(p_request_body, len - sizeof(svr_msg_header_t));
                return 0;//无需返回
            case svr_msg_enter_show:
                m_errno = process_enter_show(p_request_body, len - sizeof(svr_msg_header_t));
                break;
            case svr_msg_join_show://参与大众点评
                m_errno = process_join_show(p_request_body, len - sizeof(svr_msg_header_t));
                break;
            case svr_msg_vote_show://投票大众点评
                m_errno = process_vote_show(p_request_body, len - sizeof(svr_msg_header_t));
                break;
            case svr_msg_history_show://往届投票大众点评
                m_errno = process_history_show(p_request_body, len - sizeof(svr_msg_header_t));
                break;
            default:
                KCRIT_LOG(m_user_id,"Unknown msg type %u", m_msg_type);
                break;
        }
    }

    if (ERR_NO_ERR != m_errno)
    {
        g_pack.pack(m_errno, 10);
        g_pack.end(sizeof(svr_msg_header_t));
    }
    else
    {
        g_pack.end();
    }

    *ack_len = g_pack.get_msg_len();

    KDEBUG_LOG(m_user_id, "[%u:%u] send.len = %u\terrno = %u", m_msg_type, m_seq_id, *ack_len, m_errno);
    return 0;
}

int c_work_proc::init_show_file()
{
    if(m_p_cur_top_start)
    {
        munmap(m_p_cur_top_start, m_show_file_size);
        m_p_cur_top_start = NULL;
    }
    if(m_p_last_top_start)
    {
        munmap(m_p_last_top_start, m_show_file_size);
        m_p_last_top_start = NULL;
    }

    m_cur_show_id = GET_SHOW_PERIOD(time(NULL));
    char show_file_name[1024] = {0};
    sprintf(show_file_name, "%s%u", m_show_file_prefix, m_cur_show_id);
    int cur_file_fd = open(show_file_name, O_CREAT|O_RDWR, 0644);
    if(cur_file_fd < 0)
    {
        ERROR_LOG("open file %s failed.", show_file_name);
        return -1;
    }

    struct stat st = {0};
    if(fstat(cur_file_fd, &st) == 0)
    {
        if(st.st_size == 0)
        {//新文件
            ftruncate(cur_file_fd, m_show_file_size);
            if((m_p_cur_top_start = mmap(NULL, m_show_file_size, PROT_READ|PROT_WRITE, MAP_SHARED, cur_file_fd, 0)) != MAP_FAILED)
            {
                m_p_user_num = (show_user_num_t*)m_p_cur_top_start;
                m_p_show = (show_user_info_t *)(m_p_user_num + 1);
                m_p_user_num->user_num = 0;
                m_p_user_num->total_num = 0;
            }
            else
            {
                ERROR_LOG("mmap new file %s faield.", show_file_name);
                close(cur_file_fd);
                return -1;
            }
        }
        else
        {//已有的文件
            if((m_p_cur_top_start = mmap(NULL, m_show_file_size, PROT_READ|PROT_WRITE, MAP_SHARED, cur_file_fd, 0)) != MAP_FAILED)
            {
                m_p_user_num = (show_user_num_t*)m_p_cur_top_start;
                m_p_show = (show_user_info_t *)(m_p_user_num + 1);
            }
            else
            {
                ERROR_LOG("mmap old file %s faield.", show_file_name);
                close(cur_file_fd);
                return -1;
            }

        }
    }
    else
    {
        ERROR_LOG("fstat file %s failed.", show_file_name);
        return -1;
    }

    close(cur_file_fd);

    if(m_cur_show_id > 1)
    {
        uint32_t cur_show_id = m_cur_show_id - 1;
        sprintf(show_file_name, "%s%u", m_show_file_prefix, cur_show_id);
        cur_file_fd = open(show_file_name, O_CREAT|O_RDWR, 0644);
        if(cur_file_fd < 0)
        {
            ERROR_LOG("open file %s failed.", show_file_name);
            munmap(m_p_cur_top_start, m_show_file_size);
            m_p_cur_top_start = NULL;
            return -1;
        }
        KINFO_LOG(0, "last_top_start mmap file %s", show_file_name);
        if((m_p_last_top_start = mmap(NULL, m_show_file_size, PROT_READ|PROT_WRITE, MAP_SHARED, cur_file_fd, 0)) != MAP_FAILED)
        {
            m_p_last_user_num = (show_user_num_t*)m_p_last_top_start;
            m_p_last_show = (show_user_info_t *)(m_p_last_user_num + 1);
        }
        else
        {
            ERROR_LOG("mmap old file %s faield.", show_file_name);
            munmap(m_p_cur_top_start, m_show_file_size);
            m_p_cur_top_start = NULL;
            close(cur_file_fd);
            return -1;
        }
        close(cur_file_fd);
    }
    return 0;
}

int c_work_proc::ping_redis()
{
    static uint32_t last_update_time = 0;
    if(last_update_time  != (uint32_t)time(NULL))
    {
        last_update_time = (uint32_t)time(NULL);
        redisReply *p_rp =(redisReply*)redisCommand(m_p_rc, "PING");
        if(p_rp)
        {
            freeReplyObject(p_rp);
        }
        else
        {
            KWARN_LOG(0, "reconnect to redis svr");
            connect_to_redis_svr();
        }

    }
    return 0;
}

int c_work_proc::connect_to_redis_svr()
{
    if(m_p_rc != NULL)
    {
        return 0;
    }

    const char *redis_svr_ip = config_get_strval("redis_host");
    if(NULL == redis_svr_ip)
    {
        ERROR_LOG("Not found redis_host in conf");
        return -1;
    }
    int redis_svr_port = config_get_intval("redis_port", 6379);

    m_p_rc = redisConnect(redis_svr_ip, redis_svr_port);
    if(m_p_rc->err)
    {
        ERROR_LOG("connect to redis svr faield(%s)", m_p_rc->errstr);
        redisFree(m_p_rc);
        m_p_rc = NULL;
        return -1;
    }
    return 0;
}

int c_work_proc::init_maiji_pinboard()
{
    char sql_str[4096] = {0};
    sprintf(sql_str, "select id, peer_id, icon, color, status, create_time, message from db_monster_5.t_pinboard_23 where user_id = 12345 order by create_time desc  limit %d",  MAIJI_PINBOARD_COUNT);

    MYSQL_ROW row = NULL;
    int result_count = m_p_mysql_conn->select_first_row(&row, sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(m_user_id, "[msg:%u]sql exec failed(%s).", m_msg_type, m_p_mysql_conn->get_last_errstr());
        m_errno = ERR_SQL_ERR;
        return -1;
    }

    while (row != NULL)
	{
		uint32_t id = 0;
		if(str2uint(&id, row[0]) != 0)
		{
			KCRIT_LOG(m_user_id, "[msg:%u]convert id:%s to uint32 failed(%s).", m_msg_type, row[0], sql_str);
			m_errno = ERR_SQL_ERR;
			return -1;
		}

		uint32_t peer_id = 0;
		if(str2uint(&peer_id, row[1]) != 0)
		{
			KCRIT_LOG(m_user_id, "[msg:%u]convert peer_id:%s to uint32 failed(%s).", m_msg_type, row[1], sql_str);
			m_errno = ERR_SQL_ERR;
			return -1;
		}

		uint8_t icon = 0;
		if(str2uint(&icon, row[2]) != 0)
		{
			KCRIT_LOG(m_user_id, "[msg:%u]convert icon:%s to uint32 failed(%s).", m_msg_type, row[2], sql_str);
			m_errno = ERR_SQL_ERR;
			return -1;
		}

		uint8_t color = 0;
		if(str2uint(&color, row[3]) != 0)
		{
			KCRIT_LOG(m_user_id, "[msg:%u]convert color:%s to uint32 failed(%s).", m_msg_type, row[3], sql_str);
			m_errno = ERR_SQL_ERR;
			return -1;
		}

		uint8_t status = 0;
		if(str2uint(&status, row[4]) != 0)
		{
			KCRIT_LOG(m_user_id, "[msg:%u]convert status:%s to uint32 failed(%s).", m_msg_type, row[4], sql_str);
			m_errno = ERR_SQL_ERR;
			return -1;
		}

		uint32_t create_time = 0;
		if(str2uint(&create_time, row[5]) != 0)
		{
			KCRIT_LOG(m_user_id, "[msg:%u]convert status:%s to uint32 failed(%s).", m_msg_type, row[5], sql_str);
			m_errno = ERR_SQL_ERR;
			return -1;
		}

		if (row[6] == NULL)
		{
			KCRIT_LOG(m_user_id, "message cann't be null.");
			m_errno = ERR_SQL_ERR;
			return -1;
		}

		m_oldest_idx++;
		if(m_oldest_idx > MAIJI_PINBOARD_COUNT)
		{
			break;
		}
		m_maiji_pinboard[m_oldest_idx].id = id;
		m_maiji_pinboard[m_oldest_idx].peer_id = peer_id;
		m_maiji_pinboard[m_oldest_idx].icon = icon;
		m_maiji_pinboard[m_oldest_idx].color = color;
		m_maiji_pinboard[m_oldest_idx].status = status;
		m_maiji_pinboard[m_oldest_idx].create_time = create_time;
		uint16_t len = strlen(row[6]);
		m_maiji_pinboard[m_oldest_idx].message_count = len;
		if (len != 0 && len < MAX_MESSAGE_BYTE)
		{
			strncpy(m_maiji_pinboard[m_oldest_idx].message, row[6], len);
		}

		row = m_p_mysql_conn->select_next_row(true);
	}
    return 0;
}

int c_work_proc::process_add_maiji_pinboard(const char *p_request_body, uint32_t request_len)
{

    pinboard_message_t *p_req = (pinboard_message_t*)p_request_body;
    m_maiji_pinboard[m_oldest_idx].id = p_req->id;
    m_maiji_pinboard[m_oldest_idx].peer_id = p_req->peer_id;
    m_maiji_pinboard[m_oldest_idx].icon = p_req->icon;
    m_maiji_pinboard[m_oldest_idx].color = p_req->color;
    m_maiji_pinboard[m_oldest_idx].status = p_req->status;
    m_maiji_pinboard[m_oldest_idx].create_time = p_req->create_time;
    m_maiji_pinboard[m_oldest_idx].message_count = p_req->message_count;
    memcpy(m_maiji_pinboard[m_oldest_idx].message, p_req->message, p_req->message_count);

    if(m_oldest_idx == 0)
    {
        m_oldest_idx = MAIJI_PINBOARD_COUNT - 1;
    }
    else
    {
        m_oldest_idx--;
    }

    return 0;
}

int c_work_proc::process_get_maiji_pinboard(const char *p_request_body, uint32_t request_len)
{

    if (check_val_len(m_msg_type, request_len, sizeof(db_msg_pinboard_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_pinboard_req_t *p_req = (db_msg_pinboard_req_t *)p_request_body;


    if(p_req->page > MAX_PAGE_NUM)
    {//最多取999页
        KCRIT_LOG(m_user_id, "req->page(%u) > 999", p_req->page);
        return ERR_PAGE_ZERO;
    }
    int newest_idx = (m_oldest_idx + 1) % MAIJI_PINBOARD_COUNT;

    int begin = (p_req->page - 1) * p_req->page_num;

    int data_idx = (newest_idx + begin) % MAIJI_PINBOARD_COUNT;

    uint8_t num = PER_PAGE_NUM;//每页取8条
    g_pack.pack(num);
    for(uint8_t i = 0; i < num; i++)
    {
        int j = (data_idx + i) % MAIJI_PINBOARD_COUNT;
        g_pack.pack(m_maiji_pinboard[j].id);
        g_pack.pack(m_maiji_pinboard[j].peer_id);
        g_pack.pack(m_maiji_pinboard[j].icon);
        g_pack.pack(m_maiji_pinboard[j].color);
        g_pack.pack(m_maiji_pinboard[j].status);
        g_pack.pack(m_maiji_pinboard[j].create_time);
        g_pack.pack(m_maiji_pinboard[j].message_count);
        g_pack.pack(m_maiji_pinboard[j].message, m_maiji_pinboard[j].message_count);
    }

    return 0;
}

int c_work_proc::process_join_show(const char *p_request_body, uint32_t request_len)
{
    if (check_val_len(m_msg_type, request_len, sizeof(online_msg_join_show_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    online_msg_join_show_t *p_omjs = (online_msg_join_show_t*)p_request_body;
    uint32_t cur_time = (uint32_t)time(NULL);
    uint32_t svr_show_id = GET_SHOW_PERIOD(cur_time);
    if(svr_show_id != p_omjs->show_id)
    {//请求的大众点评届数不正确
        KCRIT_LOG(m_user_id, "cli_show_id(%u) not equal to svr_show_id(%u)", p_omjs->show_id, svr_show_id);
        return ERR_REQ_SHOW_ID;
    }

    if(get_phase_of_show() != PHASE_ENTER)
    {//非报名阶段
        KCRIT_LOG(m_user_id, "Now is not PHASE_ENTER, show_id:%u", svr_show_id);
        return ERR_NOT_PHASE_ENTER;
    }

    if(timestamp_begin_day(cur_time) != p_omjs->user_info.npc_timestamp)
    {//今天小屋未参与评分
        KCRIT_LOG(m_user_id, "npc_timestamp %u not today %u", p_omjs->user_info.npc_timestamp, timestamp_begin_day(cur_time));
        return ERR_NPC_NOT_SCORED;
    }

    ping_redis();

    //判断是否已报过名
    sprintf(m_redis_command, "sismember %s_%u %u", KEY_JOIN_PREFIX, svr_show_id, m_user_id);
    redisReply *p_rp = (redisReply*)redisCommand(m_p_rc, m_redis_command);
    if(p_rp == NULL || p_rp->type != REDIS_REPLY_INTEGER)
    {
        KCRIT_LOG(m_user_id, "exec redis  command %s failed.", m_redis_command);
        if(p_rp == NULL)
        {
            redisFree(m_p_rc);
            m_p_rc = NULL;
        }
        return ERR_SYSTEM_ERR;
    }

    if(p_rp->integer == 1)
    {//已经报过名
        KCRIT_LOG(m_user_id, "have joined show_id(%u)", svr_show_id);
        return ERR_REPEAT_JOIN;
    }

	freeReplyObject(p_rp);

    sprintf(m_redis_command, "sadd %s_%u %u", KEY_JOIN_PREFIX, svr_show_id, m_user_id);
    p_rp = (redisReply*)redisCommand(m_p_rc, m_redis_command);
    if(p_rp == NULL || p_rp->type != REDIS_REPLY_INTEGER)
    {
        KCRIT_LOG(m_user_id, "exec redis  command %s failed.", m_redis_command);
        if(p_rp == NULL)
        {
            redisFree(m_p_rc);
            m_p_rc = NULL;
        }
        return ERR_SYSTEM_ERR;
    }

	freeReplyObject(p_rp);

    uint8_t user_num = m_p_user_num->user_num;

    for(uint8_t xdi = 0; xdi < user_num; xdi++)
    {
        if(m_p_show[xdi].user_id == m_user_id)
        {
            KCRIT_LOG(m_user_id, "repeat join and redis failed.");
            return ERR_REPEAT_JOIN;
        }
    }

    if(user_num < SHOW_USER_NUM)
    {
        memcpy(&(m_p_show[user_num]), &(p_omjs->user_info), sizeof(show_user_info_t));
        if(user_num == 0)
        {//第一个人
            m_p_show[user_num].is_top = 1;
            m_top_index[0] = user_num;
        }
        else
        {
            m_top_index[user_num] = user_num;
            m_p_show[user_num].is_top = user_num + 1;//名次按照1 2 3 4 5 6来

            for(uint8_t i = 0; i < user_num ; i++)
            {
                uint8_t top_idx = m_top_index[i];
                if(p_omjs->user_info.npc_score > m_p_show[top_idx].npc_score)
                {
                    m_top_index[i] = user_num;
                    m_p_show[user_num].is_top = i + 1;
					uint8_t j = user_num;
                    for(j = user_num; j > i + 1; j--)
                    {
                        m_top_index[j] = m_top_index[j - 1];
                        m_p_show[m_top_index[j - 1]].is_top = j + 1;
                    }
                    m_top_index[j] = top_idx;
                    m_p_show[top_idx].is_top = j + 1;
                    break;
                }
            }
        }
        m_p_user_num->user_num += 1;
    }
    else
    {
        uint8_t min_score_idx = m_top_index[SHOW_USER_NUM - 1];
        if(p_omjs->user_info.npc_score > m_p_show[min_score_idx].npc_score)
        {//比最低分的还要高
            memcpy(&m_p_show[min_score_idx], &(p_omjs->user_info), sizeof(show_user_info_t));
            for(uint8_t i = 0; i < user_num; i++)
            {
                uint8_t top_idx = m_top_index[i];
                if(p_omjs->user_info.npc_score > m_p_show[top_idx].npc_score)
                {
                    m_top_index[i] = min_score_idx;
                    m_p_show[min_score_idx].is_top = i + 1;
					uint8_t j = user_num - 1;
                    for(; j > i + 1; j--)
                    {
                        m_top_index[j] = m_top_index[j - 1];
                        m_p_show[m_top_index[j - 1]].is_top = j + 1;
                    }
                    m_top_index[j] = top_idx;
                    m_p_show[top_idx].is_top = j + 1;
                    break;
                }

            }
        }
    }

        m_p_user_num->total_num += 1;

    g_pack.pack(p_omjs->show_id);
    return 0;
}


/**
 * @brief process_vote_show 投票
 * 需要在redis中记录两个set,第一个时针对每个被投票的用户， key为vote_user_id_show_id  用户记录投中冠军的用户,当用户领完奖后从该set中删除
 *                          第二个仅仅时记录一个用户是否投过票
 *
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
int c_work_proc::process_vote_show(const char *p_request_body, uint32_t request_len)
{
    if (check_val_len(m_msg_type, request_len, sizeof(msg_vote_show_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    msg_vote_show_t *p_mvs = (msg_vote_show_t*)p_request_body;
    uint32_t cur_time = (uint32_t)time(NULL);
    uint32_t svr_show_id = GET_SHOW_PERIOD(cur_time);
    if(svr_show_id != p_mvs->show_id)
    {//请求的大众点评届数不正确
        KCRIT_LOG(m_user_id, "cli_show_id(%u) not equal to svr_show_id(%u)", p_mvs->show_id, svr_show_id);
        return ERR_REQ_SHOW_ID;
    }

    if(get_phase_of_show() != PHASE_VOTE)
    {//非报名阶段
        KCRIT_LOG(m_user_id, "Now is not PHASE_VOTE, show_id:%u", svr_show_id);
        return ERR_NOT_PHASE_VOTE;
    }

    ping_redis();
    //判断是否已报过名
    sprintf(m_redis_command, "sismember %s_%u %u", KEY_VOTE_PREFIX, svr_show_id, m_user_id);
    redisReply *p_rp = (redisReply*)redisCommand(m_p_rc, m_redis_command);
    if(p_rp == NULL || p_rp->type != REDIS_REPLY_INTEGER)
    {
        KCRIT_LOG(m_user_id, "exec redis  command %s failed.", m_redis_command);
        if(p_rp == NULL)
        {
            redisFree(m_p_rc);
            m_p_rc = NULL;
        }
        return ERR_SYSTEM_ERR;
    }

    if(p_rp->integer == 1)
    {//已经投过票
        KCRIT_LOG(m_user_id, "have vote show_id(%u)", svr_show_id);
        return ERR_REPEAT_VOTE;
    }

	freeReplyObject(p_rp);
    //先判断被投票人的米米号是否正确
	uint8_t idx = 0;
    for(idx = 0; idx < m_p_user_num->user_num; idx++)
    {
        if(m_p_show[idx].user_id == p_mvs->peer_id)
        {
            m_p_show[idx].votes++;
            break;
        }
    }
    if(idx == m_p_user_num->user_num)
    {//被投票人不在候选人列表中
        KCRIT_LOG(m_user_id, "candidata_id %u not in show_id %u list", p_mvs->peer_id, p_mvs->show_id);
        return ERR_NOT_CANDIDATE;
    }


    KINFO_LOG(m_user_id, "vote show %u user %u", p_mvs->show_id,  p_mvs->peer_id);

    sprintf(m_redis_command, "sadd %s_%u_%u %u", KEY_VOTE_PREFIX, svr_show_id, p_mvs->peer_id, m_user_id);
    p_rp = (redisReply*)redisCommand(m_p_rc, m_redis_command);
    if(p_rp == NULL || p_rp->type != REDIS_REPLY_INTEGER)
    {
        KCRIT_LOG(m_user_id, "exec redis  command %s failed.", m_redis_command);
        if(p_rp == NULL)
        {
            redisFree(m_p_rc);
            m_p_rc = NULL;
        }
        return ERR_SYSTEM_ERR;
    }

	freeReplyObject(p_rp);

    sprintf(m_redis_command, "sadd %s_%u %u", KEY_VOTE_PREFIX, svr_show_id, m_user_id);
    p_rp = (redisReply*)redisCommand(m_p_rc, m_redis_command);
    if(p_rp == NULL || p_rp->type != REDIS_REPLY_INTEGER)
    {
        KCRIT_LOG(m_user_id, "exec redis  command %s failed.", m_redis_command);
        if(p_rp == NULL)
        {
            redisFree(m_p_rc);
            m_p_rc = NULL;
        }
        return ERR_SYSTEM_ERR;
    }

	freeReplyObject(p_rp);

    g_pack.pack(p_mvs->show_id);
    return 0;
}

int c_work_proc::process_enter_show(const char *p_request_body, uint32_t request_len)
{
    if (check_val_len(m_msg_type, request_len, sizeof(uint32_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    uint32_t last_show_id = *(uint32_t*)p_request_body;

    //先获取当前的届数 以及阶段，根据阶段拉取用户的信息
    uint32_t cur_time = (uint32_t)time(NULL);
    uint32_t svr_show_id = GET_SHOW_PERIOD(cur_time);
    uint8_t phase_of_show = get_phase_of_show();

    show_user_num_t *p_user_num = NULL;
    show_user_info_t *p_user_info = NULL;


    if(phase_of_show == PHASE_VOTE)
    {//投票阶段
        if(m_p_cur_top_start)
        {
            p_user_num = (show_user_num_t*)m_p_cur_top_start;
            p_user_info = (show_user_info_t*)(p_user_num + 1);
        }

        sprintf(m_redis_command, "sismember %s_%u %u", KEY_VOTE_PREFIX, svr_show_id, m_user_id);

    }
    else if(phase_of_show == PHASE_ENTER)
    {//报名阶段

        if(m_cur_show_id != svr_show_id)
        {//新的一届开始报名
            KINFO_LOG(m_user_id, "new show id %u old %u", svr_show_id, m_cur_show_id);
                init_show_file();
            KINFO_LOG(m_user_id, "now svr_show id %u m_cur_show_id %u", svr_show_id, m_cur_show_id);
        }

        if(m_p_last_top_start == NULL)
        {
            if(svr_show_id > 1)
            {
                char show_file_name[1024] = {0};
                sprintf(show_file_name, "%s%u", m_show_file_prefix, svr_show_id - 1);
                int show_file_fd = open(show_file_name, O_RDWR);
                if(show_file_fd < 0)
                {
                    ERROR_LOG("open file %s failed.", show_file_name);
                    m_p_last_top_start = NULL;
                }
                else
                {
                    if((m_p_last_top_start = mmap(NULL, m_show_file_size, PROT_READ|PROT_WRITE, MAP_SHARED, show_file_fd, 0)) == MAP_FAILED)
                    {
                        ERROR_LOG("mmap old file %s faield.", show_file_name);
                        m_p_last_top_start = NULL;
                    }

                    close(show_file_fd);
                }
            }
        }

        if(m_p_last_top_start)
        {
            p_user_num = (show_user_num_t*)m_p_last_top_start;
            p_user_info = (show_user_info_t*)(p_user_num + 1);
        }

        sprintf(m_redis_command, "sismember %s_%u %u", KEY_JOIN_PREFIX, svr_show_id, m_user_id);
    }
    else
    {//错误的阶段
        KCRIT_LOG(m_user_id, "invalid phase of show %u", phase_of_show);
        return ERR_SYSTEM_ERR;
    }

    uint32_t total_join_num = 0;
    if(m_p_cur_top_start)
    {
        total_join_num =  m_p_user_num->total_num;
    }


    g_pack.pack(svr_show_id);
    g_pack.pack(phase_of_show);
    g_pack.pack(total_join_num);
    KINFO_LOG(m_user_id, "svr_show_id:%u phase_of_show:%u total_join_num:%u", svr_show_id, phase_of_show, total_join_num);

    if(p_user_num)
    {
        g_pack.pack(p_user_num->user_num);
        for(uint8_t idx = 0; idx < p_user_num->user_num; idx++)
        {
            g_pack.pack(p_user_info[idx].user_id);
            g_pack.pack(p_user_info[idx].user_name, 16);
            g_pack.pack(p_user_info[idx].mon_id);
            g_pack.pack(p_user_info[idx].mon_main_color);
            g_pack.pack(p_user_info[idx].mon_exp_color);
            g_pack.pack(p_user_info[idx].mon_eye_color);
            g_pack.pack(p_user_info[idx].npc_score);
            g_pack.pack(p_user_info[idx].npc_timestamp);
            g_pack.pack(p_user_info[idx].votes);
            g_pack.pack(p_user_info[idx].is_top);
            g_pack.pack(p_user_info[idx].is_notice);
        }
    }
    else
    {
        g_pack.pack((uint8_t)0);
    }
    ping_redis();
    //之后判断该用户是否已报名或已投票
    redisReply *p_rp = (redisReply*)redisCommand(m_p_rc, m_redis_command);
    if(p_rp == NULL || p_rp->type != REDIS_REPLY_INTEGER)
    {
        KCRIT_LOG(m_user_id, "exec redis  command %s failed.", m_redis_command);
        if(p_rp == NULL)
        {
            redisFree(m_p_rc);
            m_p_rc = NULL;
        }
        return ERR_SYSTEM_ERR;
    }

    if(p_rp->integer == 1)
    {//已经报过名或已经投过票
		KINFO_LOG(m_user_id, "Have joined");
        g_pack.pack((uint8_t)1);//已经投过票
    }
    else
    {
		KINFO_LOG(m_user_id, "Not joined");
        g_pack.pack((uint8_t)0);//未投过票
    }

	freeReplyObject(p_rp);

    //接下来需要判断用户是否有往届奖励未领取:先看是否冠军，在看是否在冠军列表中
    void *p_last_top_start = NULL;

    if(last_show_id == svr_show_id - 1 && m_p_last_top_start)
    {//请求的是上一届的信息
        KINFO_LOG(m_user_id, "request last show info %u", last_show_id);
            p_last_top_start = m_p_last_top_start;
    }
	else if(last_show_id == 0 || last_show_id == svr_show_id)
	{
        KINFO_LOG(m_user_id, "last show info failed %u, cur_show_id %u", last_show_id, svr_show_id);
            p_last_top_start = NULL;
	}
    else
    {
        char show_file_name[1024] = {0};
        sprintf(show_file_name, "%s%u", m_show_file_prefix, last_show_id);
        int show_file_fd = open(show_file_name, O_RDWR);
        if(show_file_fd < 0)
        {
            ERROR_LOG("open file %s failed.", show_file_name);
            p_last_top_start = NULL;
        }
        else
        {
            if((p_last_top_start = mmap(NULL, m_show_file_size, PROT_READ|PROT_WRITE, MAP_SHARED, show_file_fd, 0)) == MAP_FAILED)
            {
                ERROR_LOG("mmap old file %s faield.", show_file_name);
                p_last_top_start = NULL;
            }

            close(show_file_fd);
        }
    }

    if(p_last_top_start)
    {
            p_user_num = (show_user_num_t*)p_last_top_start;
            p_user_info = (show_user_info_t*)(p_user_num + 1);
            uint32_t last_top1_user = 0;
            uint8_t idx = 0;
            for(; idx < p_user_num->user_num; idx++)
            {
                KINFO_LOG(m_user_id, "idx:%u user_id:%u top:%u", idx, p_user_info[idx].user_id, p_user_info[idx].is_top);
                if(p_user_info[idx].is_top == 1)
                {
                    last_top1_user = p_user_info[idx].user_id;
                    break;
                }
            }

            if(idx < p_user_num->user_num)
            {
                if(m_user_id == last_top1_user)
                {//是冠军
                    if(p_user_info[idx].is_notice == 0)
                    {//未通知
                            KINFO_LOG(m_user_id, "top1  not notcied.");
                        g_pack.pack((uint8_t)0);
                        p_user_info[idx].is_notice = 1;
                        g_pack.pack(last_show_id);
                        g_pack.pack((uint8_t)0);//冠军奖
                        g_pack.pack((uint32_t)TOP1_REWARD_COIN);
                    }
                    else
                    {
                            KINFO_LOG(m_user_id, "top1 have notcied.");
                        g_pack.pack((uint8_t)1);//已通知
                    }
                }
                else
                {//不是冠军
                    sprintf(m_redis_command, "sismember %s_%u_%u %u", KEY_VOTE_PREFIX, last_show_id, last_top1_user, m_user_id);
                    p_rp = (redisReply*)redisCommand(m_p_rc, m_redis_command);
                    if(p_rp == NULL || p_rp->type != REDIS_REPLY_INTEGER)
                    {
                        KCRIT_LOG(m_user_id, "exec redis  command %s failed.", m_redis_command);
                        if(p_rp == NULL)
                        {
                            redisFree(m_p_rc);
                            m_p_rc = NULL;
                        }
                            KINFO_LOG(m_user_id, "redis failed.");
                        g_pack.pack((uint8_t)1);
                    }
                    else
                    {
                        if(p_rp->integer == 1)
                        {//给冠军投过票
                            KINFO_LOG(m_user_id, "vote top1");
                            g_pack.pack((uint8_t)0);
                            sprintf(m_redis_command, "srem %s_%u_%u %u", KEY_VOTE_PREFIX, last_show_id, last_top1_user, m_user_id);
                            p_rp = (redisReply*)redisCommand(m_p_rc, m_redis_command);
                            g_pack.pack(last_show_id);
                            g_pack.pack((uint8_t)1);//猜中冠军奖
                            g_pack.pack((uint32_t)VOTE_TOP1_COIN);
                        }
                        else
                        {
                            KINFO_LOG(m_user_id, "Not vote top1");
                            g_pack.pack((uint8_t)1);
                        }
                        freeReplyObject(p_rp);
                    }
                }
            }
            else
            {//未找到冠军，不应该走到这里
        		KINFO_LOG(m_user_id, "Not found top1 idx :%u user_num:%u", idx, p_user_num->user_num);
                g_pack.pack((uint8_t)1);//已通知
            }

    }
    else
    {
        KINFO_LOG(m_user_id, "p_last_top_start is NULL");
        g_pack.pack((uint8_t)1);//已通知
    }


    return 0;
}

int c_work_proc::process_history_show(const char *p_request_body, uint32_t request_len)
{
    if (check_val_len(m_msg_type, request_len, sizeof(uint32_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    uint32_t p_show_id = *(uint32_t*)p_request_body;
    uint32_t cur_time = (uint32_t)time(NULL);
    uint32_t svr_show_id = GET_SHOW_PERIOD(cur_time);
    if(p_show_id < 1 || p_show_id >= svr_show_id)
    {
        KCRIT_LOG(m_user_id, "cli_show_id(%u) between 0 and svr_show_id(%u)", p_show_id, svr_show_id);
        return ERR_REQ_SHOW_ID;
    }

    void *p_history_top_start = NULL;
    show_user_num_t *p_history_user_num;
    show_user_info_t *p_history_show;

    if(p_show_id == svr_show_id - 1 && m_p_last_top_start)
    {//请求的是上一届的信息
            p_history_top_start = m_p_last_top_start;
    }
    else
    {
		char show_file_name[1024] = {0};
        sprintf(show_file_name, "%s%u", m_show_file_prefix, p_show_id);
        int show_file_fd = open(show_file_name, O_RDWR);
        if(show_file_fd < 0)
        {
            ERROR_LOG("open file %s failed.", show_file_name);
            return ERR_SYSTEM_ERR;
        }

        if((p_history_top_start = mmap(NULL, m_show_file_size, PROT_READ|PROT_WRITE, MAP_SHARED, show_file_fd, 0)) == MAP_FAILED)
        {
            ERROR_LOG("mmap old file %s faield.", show_file_name);
            return ERR_SYSTEM_ERR;
        }

        close(show_file_fd);
    }

    p_history_user_num = (show_user_num_t*)p_history_top_start;
    p_history_show = (show_user_info_t*)(p_history_user_num + 1);

    g_pack.pack(p_show_id);
    g_pack.pack(p_history_user_num->total_num);
    g_pack.pack(p_history_user_num->user_num);
    for(uint8_t idx = 0; idx < p_history_user_num->user_num; idx++)
    {
        g_pack.pack(p_history_show[idx].user_id);
        g_pack.pack(p_history_show[idx].user_name, 16);
        g_pack.pack(p_history_show[idx].mon_id);
        g_pack.pack(p_history_show[idx].mon_main_color);
        g_pack.pack(p_history_show[idx].mon_exp_color);
        g_pack.pack(p_history_show[idx].mon_eye_color);
        g_pack.pack(p_history_show[idx].npc_score);
        g_pack.pack(p_history_show[idx].npc_timestamp);
        g_pack.pack(p_history_show[idx].votes);
        g_pack.pack(p_history_show[idx].is_top);
        g_pack.pack(p_history_show[idx].is_notice);
    }


    if(p_show_id != svr_show_id - 1)
    {
        munmap(p_history_top_start, m_show_file_size);
        p_history_top_start = NULL;
    }

    return 0;
}

