/*
 * =====================================================================================
 *
 *       Filename:  friend.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年08月05日 13时26分45秒
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
#include "../../../common/pack/c_pack.h"
#include "../../../common/message.h"
#include "../../../common/data_structure.h"
#include "../../../common/constant.h"
#include "../util.h"
#include "friend.h"
#include "../role-info/role_info.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t get_friend_id(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_friend_list_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_friend_list_req_t *p_req = (db_msg_friend_list_req_t *)p_request_body;
   // KINFO_LOG(user_id, "[get friend id list:%u].", p_req->begin);

    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT friend_id, type, is_best_friend FROM db_monster_%d.t_friend_%d WHERE user_id = %u AND type not IN(%u, %u) LIMIT %u, 1024", DB_ID(user_id), TABLE_ID(user_id), user_id, FRIEND_PENDING, FRIEND_BLOCK, p_req->begin);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    uint16_t num = 0;
    char buffer[4096] = {0};
    db_msg_friend_list_rsp_t *p_friend = (db_msg_friend_list_rsp_t *)buffer;

    p_friend->is_end = true;
    while (row != NULL)
    {
	if (sizeof(buffer) <= (num + 1) * sizeof(friend_info_t) + sizeof(db_msg_friend_list_rsp_t))
        {
            p_friend->is_end = false;
            break;
        }

        if(str2uint(&p_friend->friend_info[num].friend_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert friend_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_friend->friend_info[num].type, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert type:%s to uint8 failed(%s).", msg_type, row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_friend->friend_info[num].is_bestfriend, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert is_bestfriend:%s to uint8 failed(%s).", msg_type, row[2], g_sql_str);
            return ERR_SQL_ERR;
        }
//	KINFO_LOG(user_id, "get friend:%u, type:%u, is_best_friend:%u", p_friend->friend_info[num].friend_id, p_friend->friend_info[num].type, p_friend->friend_info[num].is_bestfriend);

       ++num;
       row = p_mysql_conn->select_next_row(true);
    }
    p_friend->count = num;
    KINFO_LOG(user_id, "get %u friends", num);

    g_pack.pack((char *)p_friend, sizeof(db_msg_friend_list_rsp_t) + num * sizeof(friend_info_t));

    return 0;
}

uint32_t apply_for_friend(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    db_msg_friend_apply_req_t *p_apply = (db_msg_friend_apply_req_t *)p_request_body;
    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed");
        return ERR_SQL_ERR;
    }
    if (p_apply->type != FRIEND_PENDING)
    {
    		/*
        if (check_val_len(msg_type, request_len, sizeof(db_msg_friend_apply_req_t)) != 0)
        {
            return ERR_MSG_LEN;
        }
        */

        if (mysql_autocommit(p_conn, false) != 0)
        {
            KCRIT_LOG(user_id, "close mysql_autocommit failed.");
            return ERR_SQL_ERR;
        }

        snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_friend_%d(user_id, friend_id, create_time, type) VALUES(%u, %u, %zu, %u) ON DUPLICATE KEY UPDATE type = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_apply->peer_id, time(NULL), FRIEND_DEFAULT, FRIEND_DEFAULT);
        if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
        {
            KCRIT_LOG(user_id, "make friend failed(%s).", g_sql_str);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }

        sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set friend_num = friend_num + 1 where user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);
        if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", g_sql_str);
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
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
    }
    else
    {
        if (request_len > (int)(sizeof(db_msg_friend_apply_req_t) + MAX_REMARK_COUNT))
        {
            return ERR_MSG_LEN;
        }

        char buffer[2 * MAX_REMARK_COUNT + 1] = {0};

        mysql_real_escape_string(p_conn, buffer, p_apply->remark, p_apply->remark_count);

        snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_friend_%d(user_id, friend_id, create_time, type, remark) VALUES(%u, %u, %zu, %u, \"%s\")", DB_ID(user_id), TABLE_ID(user_id), user_id, p_apply->peer_id, time(NULL), FRIEND_PENDING, buffer);
        if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
        {
            int errno = mysql_errno(p_conn);
            if (1062 == errno)  //插入冲突
            {
                return ERR_APPLY_FOR_FRIEND_AGAIN;
            }
            else
            {
                return ERR_SQL_ERR;
            }
        }
    }


	if(!strncmp(p_apply->remark, INVITE_FRIEND_REMARK, strlen(INVITE_FRIEND_REMARK)))
	{
		snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_invite_%d(user_id, invite_num, qualified_num) VALUES(%u, 1, 0) ON DUPLICATE KEY UPDATE invite_num=invite_num+1;", DB_ID(user_id), TABLE_ID(user_id), user_id);
		if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
		{
			 int errno = mysql_errno(p_conn);
			 KCRIT_LOG(user_id, "insert apply for friend to db failed(%s), errno:%u.", mysql_error(p_conn), errno);
			 if (1062 == errno)	//插入冲突
			 {
				  return ERR_APPLY_FOR_FRIEND_AGAIN;
			 }
			 else
			 {
				  return ERR_SQL_ERR;
			 }
		}
	}

    role_t cached_role = {{0}};
    if (p_apply->type != FRIEND_PENDING && p_memcached->get_role(user_id, &cached_role) == 0)
    {
        cached_role.friend_num += 1;
        p_memcached->set_role(user_id, &cached_role);
    }

    return 0;
}

uint32_t get_friend_apply(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }


    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT friend_id  FROM db_monster_%d.t_friend_%d WHERE user_id = %u AND type = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, FRIEND_PENDING);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    uint8_t num = 0;
    g_pack.pack(num);   //先占位，确定个数后再更改大小
    while (row != NULL)
    {
        uint32_t peer_id = 0;
        if(str2uint(&peer_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert peer_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }
        g_pack.pack(peer_id);


        ++num;
        if (num == MAX_FRIEND_REQ)
        {
            break;
        }
        row = p_mysql_conn->select_next_row(true);
    }
    g_pack.pack(num, sizeof(svr_msg_header_t));   //更新申请的个数

    return 0;
}

uint32_t set_friend_status(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_set_friend_status_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    const static uint8_t no_update = -1;

    db_msg_set_friend_status_req_t *p_friend = (db_msg_set_friend_status_req_t *)p_request_body;
    KINFO_LOG(user_id, "set friend:%u status, type:%u, is_best_friend:%u", p_friend->friend_info.friend_id, p_friend->friend_info.type, p_friend->friend_info.is_bestfriend);
    if (p_friend->friend_info.type == no_update && p_friend->friend_info.is_bestfriend == no_update)
    {
        return 0;
    }

    char buf[100] = {0};
    sprintf(g_sql_str, "UPDATE db_monster_%d.t_friend_%d SET ", DB_ID(user_id), TABLE_ID(user_id));

    if (p_friend->friend_info.type != no_update)
    {
        sprintf(buf, "type = %u,", p_friend->friend_info.type);
        strcat(g_sql_str, buf);
    }
    if (p_friend->friend_info.is_bestfriend != no_update)
    {
        sprintf(buf, "is_best_friend = %u,", p_friend->friend_info.is_bestfriend);
        strcat(g_sql_str, buf);
    }
    g_sql_str[strlen(g_sql_str) - 1] = 0;

    sprintf(buf, " WHERE user_id = %u and friend_id = %u", user_id, p_friend->friend_info.friend_id);
    strcat(g_sql_str, buf);

    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "update friend status sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return -1;
    }

    if (p_friend->friend_info.type == FRIEND_DEFAULT) //设为默认好友
    {

        sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set friend_num = friend_num + 1 where user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "update friend num sql exec failed(%s).", g_sql_str);
            return -1;
        }

        role_t cached_role = {{0}};
        if (p_memcached->get_role(user_id, &cached_role) == 0)
        {
            cached_role.friend_num += 1;
            p_memcached->set_role(user_id, &cached_role);
        }
    }
    if (p_friend->friend_info.type == FRIEND_BLOCK) //设为默认好友
    {
        role_t cached_role = {{0}};
        if (p_memcached->get_role(user_id, &cached_role) == 0 && cached_role.friend_num > 0)
        {
            cached_role.friend_num -= 1;
            p_memcached->set_role(user_id, &cached_role);
        }
    }


    return 0;
}

uint32_t get_friend_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }


    role_t role = {{0}};
    int ret = get_user_role(user_id, &role, p_mysql_conn, p_memcached);
    if (ret < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }
    else if (0 == ret)
    {
        KCRIT_LOG(user_id, "user is not in role table.");
        return ERR_ROLE_NOT_EXISTS;
    }

    g_pack.pack(role.name, sizeof(role.name));
    g_pack.pack(role.gender);
    g_pack.pack(role.country_id);
    g_pack.pack(role.user_type);
    g_pack.pack(role.birthday);
    g_pack.pack(role.last_login_time);
    g_pack.pack(role.monster_id);
    g_pack.pack(role.monster_main_color);
    g_pack.pack(role.monster_ex_color);
    g_pack.pack(role.monster_eye_color);
    g_pack.pack(role.monster_level);
   // KINFO_LOG(user_id, "[get friend info finished]");

    return 0;
}

uint32_t del_friend(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_del_friend_req_t)) != 0)
    {
        KCRIT_LOG(user_id, "msg len is invalid, recv len:%u", request_len);
        return ERR_MSG_LEN;
    }

    db_msg_del_friend_req_t req = {0};
    req.friend_id = *(uint32_t *)p_request_body;
    KINFO_LOG(user_id, "[del friend:%u].", req.friend_id);
    sprintf(g_sql_str,
            "DELETE FROM db_monster_%d.t_friend_%d WHERE user_id = %u AND friend_id = %u;",
            DB_ID(user_id), TABLE_ID(user_id), user_id, req.friend_id);

    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "del friend sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }


        sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set friend_num = friend_num - 1 where user_id = %u and friend_num > 0", DB_ID(user_id), TABLE_ID(user_id), user_id);
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "update friend num sql exec failed(%s).", g_sql_str);
            return -1;
        }


    role_t cached_role = {{0}};
    if (p_memcached->get_role(user_id, &cached_role) == 0)
    {
        if (cached_role.friend_num > 0)
        {
            cached_role.friend_num -= 1;
            p_memcached->set_role(user_id, &cached_role);
        }
    }

    return 0;
}

