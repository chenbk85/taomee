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
#include <new>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "work_proc.h"

using namespace std;

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
    m_errno = ERR_NO_ERR;
    m_max_map_file = 0;
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

    char *ucount_file = config_get_strval("ucount_prefix");
    if(ucount_file == NULL)
    {
        ERROR_LOG("read config(ucount_prefix failed.)");
        return -1;
    }

    m_ucount_prefix_len = strlen(ucount_file);
    if(m_ucount_prefix_len >= PATH_MAX)
    {
        ERROR_LOG("error config ucount_prefix, too length(%d).", m_ucount_prefix_len);
        return -1;
    }
    memcpy(m_ucount_file, ucount_file, m_ucount_prefix_len);
    m_ucount_file[m_ucount_prefix_len] = '\0';

    char *visit_file = config_get_strval("visit_prefix");
    if(visit_file == NULL)
    {
        ERROR_LOG("read config(visit_prefix failed.)");
        return -1;
    }

    m_visit_prefix_len = strlen(visit_file);
    if(m_visit_prefix_len >= PATH_MAX)
    {
        ERROR_LOG("error config visit_prefix, too length(%d).", m_visit_prefix_len);
        return -1;
    }
    memcpy(m_visit_file, visit_file, m_visit_prefix_len);
    m_visit_file[m_visit_prefix_len] = '\0';

    m_p_ucount = NULL;
    if(create_ucount_instance(&m_p_ucount) != 0)
    {
        ERROR_LOG("create ucount instance failed.");
        return -1;
    }

    m_visit_count = config_get_intval("visit_count", 100);
    if (m_visit_count <= 0 || m_visit_count > 100)
    {
        ERROR_LOG("visit count:%u is invalid.", m_visit_count);
        return -1;
    }
    INFO_LOG("max visit count:%u", m_visit_count);

    m_max_map_file = config_get_intval("max_file_num", 10000);
    if (m_max_map_file <= 0 || m_max_map_file > 10000000)
    {
        ERROR_LOG("visit count:%u is invalid.", m_max_map_file);
        return -1;
    }
    INFO_LOG("max file num:%u", m_max_map_file);

    //if (load_all_visit_file() != 0)
    //{
    //    ERROR_LOG("load visit file failed.");
    //    return -1;
    //}

    m_inited = 1;
    return 0;
}


int c_work_proc::uninit()
{
    if (!m_inited)
    {
        return -1;
    }

    if(m_p_ucount)
    {
        m_p_ucount->release();
        m_p_ucount = NULL;
    }

    for (map<uint32_t, char *>::iterator iter = m_visit_file_map.begin(); iter != m_visit_file_map.end(); ++iter)
    {
        KDEBUG_LOG(iter->first, "msync file");
        msync(iter->second, sizeof(latest_visit_t) + sizeof(visit_info_t) * m_visit_count, MS_SYNC);
    	munmap(iter->second, sizeof(latest_visit_t) + sizeof(visit_info_t) * m_visit_count);
    }

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
            case svr_msg_ucount_visit:
                save_latest_visit(m_user_id, *(uint32_t *)p_request_body);  //保存最近访问和订的用户
                //no break
            case svr_msg_ucount_thumb:
	    {
                uint8_t is_visit = 0;
                m_errno = process_msg_visit_and_thumb(p_request_body, &is_visit);
                if (m_msg_type == svr_msg_ucount_thumb && m_errno == 0 && is_visit == 0)
                {
                    save_latest_visit(m_user_id, *(uint32_t *)p_request_body);  //保存最近访问和订的用户
                }
                break;
	    }
            case svr_msg_ucount_latest_visit:
                m_errno = get_latest_visit();
                break;
            case svr_msg_get_unread_count:                                  //获取用户未读的信息条数
                m_errno = get_unread_count();
                break;
            default:
                m_errno = ERR_MSG_TYPE;
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

int c_work_proc::process_msg_visit_and_thumb(char *p_body, uint8_t *p_is_visit)
{
    int ret_value = 0;
    uint32_t peer_id = *(uint32_t*)p_body;
    m_last_two_id = LAST_TWO_ID(m_user_id);
    m_before_last_two_id = BEFORE_LAST_TWO_ID(m_user_id);
    m_cur_day = get_cur_day();
    if(m_cur_day == 0)
    {
        ret_value = ERR_SYSTEM_ERR;
        goto end;
    }

    sprintf(m_ucount_file + m_ucount_prefix_len, "/%d/%u/%d/%d/", m_cur_day, m_msg_type, m_before_last_two_id, m_last_two_id);
    DEBUG_LOG("m_ucount_file=%s", m_ucount_file);
    if(access(m_ucount_file, F_OK) != 0)
    {
        if(0 != mkdir_p(m_ucount_file))
        {
            ERROR_LOG("create dir %s failed.", m_ucount_file);
            ret_value = ERR_SYSTEM_ERR;
            goto end;
        }
    }
    sprintf(m_ucount_file + strlen(m_ucount_file), "/%u", m_user_id);
    if(0 != m_p_ucount->init(m_ucount_file, i_ucount::CREATE, 0600))
    {
        ERROR_LOG("ucount %s init failed.", m_ucount_file);
        ret_value =  ERR_SYSTEM_ERR;
        goto end;
    }

    ret_value = m_p_ucount->set_on_noucount(peer_id, i_ucount::SET);
    DEBUG_LOG("ret_value = %d", ret_value);
    if(ret_value == -1)
    {
        ret_value = ERR_SYSTEM_ERR;
    }
    else
    {
        g_pack.pack((uint8_t)ret_value);
        *p_is_visit = ret_value;
        ret_value = 0;
    }

    m_p_ucount->uninit();

end:
    return ret_value;
}

int c_work_proc::load_all_visit_file()
{
    DIR *dir = opendir(m_visit_file);
    if (!dir)
    {
        ERROR_LOG("open dir:%s failed", m_visit_file);
        return -1;
    }

    struct dirent *de = NULL;
    int i = 0;
    while ((de = readdir(dir)) != NULL)
    {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
        {
            continue;
        }
        uint32_t user_id = atoi(de->d_name);
        char file_name[PATH_MAX] = {'\0'};
        sprintf(file_name, "%s%s", m_visit_file, de->d_name);
        int fd = open(file_name, O_RDWR);
        if (fd < 0)
        {
            ERROR_LOG("open file:%s failed", file_name);
            continue;
        }
        char *p_file = (char *)mmap(NULL, sizeof(latest_visit_t) + sizeof(visit_info_t) * m_visit_count, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);
        if (p_file == MAP_FAILED)
        {
            ERROR_LOG("mmap file:%s failed", file_name);
            return -1;
        }

        latest_visit_t *p_visit = (latest_visit_t *)p_file;
        INFO_LOG("mmap file:%u, unread:%u, count:%u, index:%u", user_id, p_visit->unread_count, p_visit->count, p_visit->index);
        p_visit->last_time = time(NULL);
        m_visit_file_map.insert(pair<uint32_t, char *>(user_id, p_file));
        file_cmp_t file_cmp = {time(NULL), user_id};
        m_file_time_set.insert(file_cmp);
        if (++i >= m_max_map_file)
        {
            break;
        }
    }

    closedir(dir);
    return 0;
}

void c_work_proc::save_latest_visit(uint32_t user_id, uint32_t peer_id)
{
    if (m_msg_type == svr_msg_ucount_visit)
    {
        KINFO_LOG(user_id, "add latest visit:%u", peer_id);
    }
    else
    {
	uint32_t temp = peer_id;
	peer_id = user_id;
        user_id = temp;
        KINFO_LOG(user_id, "add latest thumb:%u", peer_id);
    }

    time_t now = time(NULL);
    latest_visit_t *p_visit = NULL;
    map<uint32_t, char *>::iterator iter =  m_visit_file_map.find(user_id);
    if (iter != m_visit_file_map.end()) //用户的最近访问文件存在
    {
        p_visit = (latest_visit_t *)iter->second;
    }
    else    //最近访问文件不存在
    {
        //将最长时间没访问的文件换出去
        unmap_oldest_file();

        //将新的文件映射进来
        char *p_file = map_new_file(user_id);
        if (NULL == p_file)
        {
            KCRIT_LOG(user_id, "mmap file failed.");
            return;
        }

        p_visit = (latest_visit_t *)p_file;
    }

    KINFO_LOG(user_id, "p_visit:%x", p_visit);
    p_visit->visit_info[p_visit->index].user_id = peer_id;
    p_visit->visit_info[p_visit->index].timestamp = now;
    p_visit->visit_info[p_visit->index].type = m_msg_type == svr_msg_ucount_visit ? visit_type : thumb_type;
    p_visit->visit_info[p_visit->index].status = MESSAGE_UNREAD;
    if (++p_visit->unread_count > m_visit_count)
    {
        p_visit->unread_count = m_visit_count;
    }
    if (++p_visit->count > m_visit_count)
    {
        p_visit->count = m_visit_count;
    }
    KINFO_LOG(user_id, "index:%u, unread count:%u, count:%u, user_id:%u", p_visit->index, p_visit->unread_count, p_visit->count, peer_id);
    if (++p_visit->index >= m_visit_count)
    {
        p_visit->index = 0;
    }

    for (int i = 0; i != p_visit->count; ++i)
    {
        KINFO_LOG(user_id, "current visit:%u", p_visit->visit_info[i].user_id);
    }
}

int c_work_proc::get_latest_visit()
{
    KINFO_LOG(m_user_id, "get latest");
    map<uint32_t, char *>::iterator iter =  m_visit_file_map.find(m_user_id);
    latest_visit_t *p_visit = NULL;
    if (iter == m_visit_file_map.end()) //没有map进来
    {
        //将最长时间没访问的文件换出去
        unmap_oldest_file();

        //将新的文件映射进来
        char *p_file = map_new_file(m_user_id);
        if (NULL == p_file)
        {
            KCRIT_LOG(m_user_id, "mmap file failed.");
            g_pack.pack((uint8_t)0);
            return 0;
        }
        p_visit = (latest_visit_t *)p_file;
    }
    else
    {
        p_visit = (latest_visit_t *)iter->second;
    }

    g_pack.pack(p_visit->count);
    for (int i = 0; i != p_visit->count; ++i)
    {
        g_pack.pack(p_visit->visit_info[i].user_id);
        g_pack.pack(p_visit->visit_info[i].timestamp);
        g_pack.pack(p_visit->visit_info[i].type);
        g_pack.pack(p_visit->visit_info[i].status);
        if (p_visit->visit_info[i].status != MESSAGE_READ)
        {
            p_visit->visit_info[i].status = MESSAGE_READ;
        }
        KINFO_LOG(m_user_id, "get latest:%u", p_visit->visit_info[i].user_id);
    }
    p_visit->unread_count = 0;
    KINFO_LOG(m_user_id, "get latest end");
    return 0;
}

void c_work_proc::unmap_oldest_file()
{
    if ((int)m_file_time_set.size() < m_max_map_file)
    {
        INFO_LOG("no need to unmap,now size:%zu", m_file_time_set.size());
        return;
    }
    set<file_cmp_t>::iterator iter = m_file_time_set.begin(); //时间最早的一个文件
    if (iter == m_file_time_set.end())
    {
        return;
    }

    uint32_t user_id = (*iter).user_id;
    INFO_LOG("unmap file:%u", user_id);
    m_file_time_set.erase(iter);

    map<uint32_t, char *>::iterator mmap_iter = m_visit_file_map.find(user_id);
    if (mmap_iter == m_visit_file_map.end())
    {
        return;
    }
    char *map_addr = mmap_iter->second;
    munmap(map_addr, sizeof(latest_visit_t) + sizeof(visit_info_t) * m_visit_count);
    m_visit_file_map.erase(mmap_iter);
}

char *c_work_proc::map_new_file(uint32_t user_id)
{
    //先判断文件存不存在
    char file_name[PATH_MAX] = {0};
    sprintf(file_name, "%s%u/%u/%u", m_visit_file, user_id % 100, (user_id / 100) % 100, user_id);
    int fd = open(file_name, O_RDWR);
    if (fd == -1)   //文件不存在
    {
        char path_name[PATH_MAX] = {0};
        sprintf(path_name, "%s%u/%u/", m_visit_file, user_id % 100, (user_id / 100) % 100);
        if(access(path_name, F_OK) != 0)
        {
            if(0 != mkdir_p(path_name))
            {
                ERROR_LOG("create dir %s failed.", path_name);
                return NULL;
            }
        }

        KINFO_LOG(user_id, "create file:%s", file_name);
        fd = open(file_name, O_RDWR | O_CREAT, 0755);
        if (fd < 0)
        {
            ERROR_LOG("create file:%s failed", file_name);
            return NULL;
        }

        char buffer[1024] = {0};
        memset(buffer, 0, sizeof(buffer));
        latest_visit_t *p_visit = (latest_visit_t *)buffer;
        p_visit->index = 0;
        p_visit->count = 0;
        write(fd, buffer, sizeof(latest_visit_t) + sizeof(visit_info_t) * m_visit_count);
    }

    KINFO_LOG(user_id, "mmap file:%s", file_name);
    char *p_file = (char *)mmap(NULL, sizeof(latest_visit_t) + sizeof(visit_info_t) * m_visit_count, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (p_file == MAP_FAILED)
    {
        ERROR_LOG("mmap file:%s failed(%s)", file_name, strerror(errno));
        return NULL;
    }

    KINFO_LOG(user_id, "mmap:%x", p_file);
    m_visit_file_map.insert(pair<uint32_t, char *>(user_id, p_file));
    //更新文件的访问时间
    latest_visit_t *p_visit = (latest_visit_t *)p_file;
    p_visit->last_time = time(NULL);
    file_cmp_t file_cmp = {p_visit->last_time, user_id};
    set<file_cmp_t>::iterator time_iter = m_file_time_set.find(file_cmp);
    if (time_iter != m_file_time_set.end())
    {
        m_file_time_set.erase(time_iter);
    }
    file_cmp.timestamp = time(NULL);
    m_file_time_set.insert(file_cmp);

    return p_file;
}

int c_work_proc::get_unread_count()
{
    KINFO_LOG(m_user_id, "get unread count");
    map<uint32_t, char *>::iterator iter =  m_visit_file_map.find(m_user_id);
    latest_visit_t *p_visit = NULL;
    if (iter == m_visit_file_map.end()) //没有map进来
    {
        //将最长时间没访问的文件换出去
        unmap_oldest_file();

        //将新的文件映射进来
        char *p_file = map_new_file(m_user_id);
        if (NULL == p_file)
        {
            KCRIT_LOG(m_user_id, "mmap file failed.");
            g_pack.pack((uint8_t)0);
            return 0;
        }
        p_visit = (latest_visit_t *)p_file;
    }
    else
    {
        p_visit = (latest_visit_t *)iter->second;
    }

    g_pack.pack(p_visit->unread_count);
    KINFO_LOG(m_user_id, "get unread count:%u", p_visit->unread_count);
    return 0;
}
