/*
 * =====================================================================================
 *
 *       Filename:  pinboard.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月21日 13时26分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luis, luis@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
extern "C"
{
#include <libtaomee/log.h>
}
#include <benchapi.h>
#include "../../../common/message.h"
#include "../../../common/pack/c_pack.h"
#include "../../../common/data_structure.h"
#include "../../../common/constant.h"
#include "../util.h"
#include "pinboard.h"
#include "../db_constant.h"

extern c_pack g_pack;
static char g_sql_str[4096] = {0};


uint32_t get_pinboard_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_pinboard_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_pinboard_req_t *p_req = (db_msg_pinboard_req_t *)p_request_body;

    uint8_t reverse_flag = 0;

    int begin = (p_req->page - 1) * p_req->page_num;

    if(p_req->real_num == -1)
    {
        reverse_flag  = 1;
        snprintf(g_sql_str, sizeof(g_sql_str), "SELECT id, peer_id, icon, color, status, create_time, message FROM db_monster_%d.t_pinboard_%d WHERE (user_id = %u AND status & %u != 0) OR (peer_id = %u AND user_id = %u) order by create_time desc limit %d, %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->status, p_req->peer_id, user_id, begin, p_req->page_num);
    }
    else if(p_req->real_num - begin < 8)
    {
            snprintf(g_sql_str, sizeof(g_sql_str), "SELECT id, peer_id, icon, color, status, create_time, message FROM db_monster_%d.t_pinboard_%d WHERE (user_id = %u AND status & %u != 0) OR (peer_id = %u AND user_id = %u) limit 0, %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->status, p_req->peer_id, user_id, p_req->real_num - begin);
    }
    else
    {
            snprintf(g_sql_str, sizeof(g_sql_str), "SELECT id, peer_id, icon, color, status, create_time, message FROM db_monster_%d.t_pinboard_%d WHERE (user_id = %u AND status & %u != 0) OR (peer_id = %u AND user_id = %u) limit %d, %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->status, p_req->peer_id, user_id, p_req->real_num - begin - p_req->page_num, p_req->page_num);

    }

    KINFO_LOG(user_id, "get pinbaord %s", g_sql_str);
    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    g_pack.pack(reverse_flag);   //标识online收到返回包后是否需要逆序处理
    uint8_t num = 0;
    g_pack.pack(num);   //先占位，等确定num数量后再更新
    while (row != NULL)
    {
        uint32_t id = 0;
        if(str2uint(&id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        uint32_t peer_id = 0;
        if(str2uint(&peer_id, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert peer_id:%s to uint32 failed(%s).", msg_type, row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        uint8_t icon = 0;
        if(str2uint(&icon, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert icon:%s to uint32 failed(%s).", msg_type, row[2], g_sql_str);
            return ERR_SQL_ERR;
        }

        uint8_t color = 0;
        if(str2uint(&color, row[3]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert color:%s to uint32 failed(%s).", msg_type, row[3], g_sql_str);
            return ERR_SQL_ERR;
        }

        uint8_t status = 0;
        if(str2uint(&status, row[4]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert status:%s to uint32 failed(%s).", msg_type, row[4], g_sql_str);
            return ERR_SQL_ERR;
        }

        uint32_t create_time = 0;
        if(str2uint(&create_time, row[5]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert status:%s to uint32 failed(%s).", msg_type, row[5], g_sql_str);
            return ERR_SQL_ERR;
        }

        if (row[6] == NULL)
        {
            KCRIT_LOG(user_id, "message cann't be null.");
            return ERR_SQL_ERR;
        }
        g_pack.pack(id);
        g_pack.pack(peer_id);
        g_pack.pack(icon);
        g_pack.pack(color);
        g_pack.pack(status);
        g_pack.pack(create_time);
        uint16_t len = strlen(row[6]);
        g_pack.pack(len);
        if (len != 0)
        {
            g_pack.pack(row[6], len);
        }

        ++num;
        row = p_mysql_conn->select_next_row(true);
    }
    if (num != 0)
    {
    	g_pack.pack(num, sizeof(svr_msg_header_t) + sizeof(uint8_t));   //更新拉取的留言的数量
    }
    KINFO_LOG(user_id, "get pinbaord finished, num:%u", num);
    return 0;
}

uint32_t add_message(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (request_len > sizeof(db_msg_add_message_req_t) + MAX_MESSAGE_BYTE)
    {
        return ERR_MSG_LEN;
    }
    int message_len = request_len - sizeof(db_msg_add_message_req_t);

    db_msg_add_message_req_t *p_message = (db_msg_add_message_req_t *)p_request_body;
    char buffer[2 * MAX_MESSAGE_BYTE + 1] = {0};

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysq conn failed.");
        return ERR_SQL_ERR;
    }

    if (mysql_autocommit(p_conn, false) != 0)
    {
        KCRIT_LOG(user_id, "close mysql_autocommit failed.");
        return ERR_SQL_ERR;
    }

    mysql_real_escape_string(p_conn, buffer, p_message->message, message_len);

    snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_pinboard_%d(user_id, peer_id, message, icon, color, status, create_time) VALUES(%u, %u, \"%s\", %u, %u, %u, %u)", DB_ID(user_id), TABLE_ID(user_id), user_id, p_message->peer_id, buffer, p_message->icon, p_message->color, p_message->type, p_message->create_time);
    KINFO_LOG(user_id, "add message:%s, peer_id:%u create_time:%u", g_sql_str, p_message->peer_id, p_message->create_time);

    if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, mysql_error(p_conn));
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, "SELECT last_insert_id()");
    if (result_count <= 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]get last insert id failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        mysql_rollback(p_conn);
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }
    uint32_t id = 0;
    if(str2uint(&id, row[0]) != 0)
    {
        KCRIT_LOG(user_id, "get last insert id failed.");
        mysql_rollback(p_conn);
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }

    uint8_t exec_flag = 0;
    if(p_message->type == MESSAGE_APPROVED)
    {
        sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set approved_msg_num = approved_msg_num + 1 where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id);
        exec_flag = 1;
    }
    else if(p_message->type == MESSAGE_UNAPPROVED)
    {
        sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set unapproved_msg_num = unapproved_msg_num + 1 where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id);
        exec_flag = 1;
    }

    if(exec_flag)
    {
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }

            role_t cached_role = {{0}};
            if(p_memcached->get_role(user_id, &cached_role) == 0)
            {
                if(p_message->type == MESSAGE_APPROVED)
                {
                    cached_role.approved_message_num += 1;
                }
                else if(p_message->type == MESSAGE_UNAPPROVED)
                {
                    cached_role.unapproved_message_num += 1;
                }

                p_memcached->set_role(user_id, &cached_role);
            }


    }
    // 结束事务
    if (mysql_commit(p_conn) != 0)
    {
        KCRIT_LOG(user_id, "mysql_commit() failed.");
        mysql_rollback(p_conn);
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }
    mysql_autocommit(p_conn, true);
    //KINFO_LOG(user_id, "last insert id:%u", id);
    g_pack.pack(id);

    return 0;
}

uint32_t update_message_status(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(uint32_t) + sizeof(uint8_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    uint8_t update_role_flag = 0;
    uint32_t id = *(uint32_t *)p_request_body;
    uint8_t type = *(uint8_t *)(p_request_body + sizeof(uint32_t));
    if (MESSAGE_DELETE == type)
    {
        sprintf(g_sql_str, "select status from db_monster_%d.t_pinboard_%d where id = %u", DB_ID(user_id), TABLE_ID(user_id), id);
        MYSQL_ROW row = NULL;
        int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
        if (result_count <= 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            return ERR_SQL_ERR;
        }

        uint8_t msg_status = 0;
        if(str2uint(&msg_status, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "get real count failed.");
            return ERR_SQL_ERR;
        }

        if(msg_status == MESSAGE_APPROVED)
        {
            update_role_flag = 1;//删除一条已经同意的留言
        }
        else if(msg_status == MESSAGE_UNAPPROVED)
        {
            update_role_flag = 2; //删除一条尚未同意的留言
        }

        snprintf(g_sql_str, sizeof(g_sql_str), "DELETE FROM db_monster_%d.t_pinboard_%d WHERE id = %u", DB_ID(user_id), TABLE_ID(user_id), id);
    }
    else
    {
        snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_pinboard_%d SET status = %u WHERE id = %u", DB_ID(user_id), TABLE_ID(user_id), type, id);
        update_role_flag = 3;//审核一条留言，即增加同意留言的数量，减少未同意留言的数量
    }

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysq conn failed.");
        return ERR_SQL_ERR;
    }
    if (mysql_autocommit(p_conn, false) != 0)
    {
        KCRIT_LOG(user_id, "close mysql_autocommit failed.");
        return ERR_SQL_ERR;
    }

    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }

    role_t cached_role = {{0}};
    int ret_mem = p_memcached->get_role(user_id, &cached_role);


    if(update_role_flag == 1)
    {
        sprintf(g_sql_str, "update db_monster_%d.t_role_%d set approved_msg_num = approved_msg_num - 1 where user_id = %u and approved_msg_num > 0", DB_ID(user_id), TABLE_ID(user_id), user_id);
        cached_role.approved_message_num -= 1;
    }
    else if(update_role_flag == 2)
    {
        sprintf(g_sql_str, "update db_monster_%d.t_role_%d set unapproved_msg_num = unapproved_msg_num - 1 where user_id = %u and unapproved_msg_num > 0", DB_ID(user_id), TABLE_ID(user_id), user_id);
        cached_role.unapproved_message_num -= 1;
    }
    else if(update_role_flag == 3)
    {
        sprintf(g_sql_str, "update db_monster_%d.t_role_%d set approved_msg_num = approved_msg_num + 1, unapproved_msg_num = unapproved_msg_num - 1 where user_id = %u and unapproved_msg_num > 0", DB_ID(user_id), TABLE_ID(user_id), user_id);
        cached_role.approved_message_num += 1;
        cached_role.unapproved_message_num -= 1;
    }

    if(update_role_flag)
    {
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
    }
    // 结束事务
    if (mysql_commit(p_conn) != 0)
    {
        KCRIT_LOG(user_id, "mysql_commit() failed.");
        mysql_rollback(p_conn);
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }
    mysql_autocommit(p_conn, true);
    if(ret_mem == 0)
    {
        p_memcached->set_role(user_id, &cached_role);
    }

    return 0;
}

uint32_t get_real_message_count(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(uint32_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    uint32_t *p_peer_id = (uint32_t *)p_request_body;
    KINFO_LOG(user_id, "get user:%u pinbaord num", *p_peer_id);

    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT COUNT(*) FROM db_monster_%d.t_pinboard_%d WHERE user_id = %u AND (status & %u != 0 OR peer_id = %u)", DB_ID(user_id), TABLE_ID(user_id), user_id, MESSAGE_APPROVED, *p_peer_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count <= 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    uint32_t real_count = 0;
    if(str2uint(&real_count, row[0]) != 0)
    {
        KCRIT_LOG(user_id, "get real count failed.");
        return ERR_SQL_ERR;
    }

    g_pack.pack(real_count);

    return 0;
}
