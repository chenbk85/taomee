/*
 * =====================================================================================
 *
 *       Filename:  activity.cpp
 *
 *    Description:  zR
 *
 *        Version:  1.0
 *        Created:  2012年05月30日 13时34分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
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
#include "../../../common/pack/c_pack.h"
#include "../../../common/message.h"
#include "../../../common/data_structure.h"
#include "../../../common/constant.h"
#include "../util.h"
#include "../db_constant.h"
#include "activity.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t get_open_act_reward(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if(check_val_len(msg_type, request_len, sizeof(db_msg_get_open_reward_t)))
    {
        return ERR_MSG_LEN;
    }

    db_msg_get_open_reward_t *p_body = (db_msg_get_open_reward_t*)p_request_body;

    sprintf(g_sql_str, "update db_monster_%d.t_activity_%d set flag = 1 where user_id = %u and activity_id = %u and reward_id = %u;", DB_ID(user_id), TABLE_ID(user_id),  user_id, p_body->act_id, p_body->reward_id);

    KINFO_LOG(user_id, "get open reward:%s", g_sql_str);
    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    if (mysql_autocommit(p_conn, false) != 0)
    {
        KCRIT_LOG(user_id, "close mysql_autocommit failed.");
        return ERR_SQL_ERR;
    }

    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }


    sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) values(%u, %u, %u) ON DUPLICATE KEY UPDATE stuff_num = stuff_num + %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_body->reward_id, p_body->reward_num, p_body->reward_num);


        KINFO_LOG(user_id, "open act_reward:%s", g_sql_str);
        if (p_mysql_conn->execsql(g_sql_str) < 0)
            {
                KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
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

    g_pack.pack(p_request_body, request_len);

    return 0;
}

uint32_t get_activity_reward(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if(check_val_len(msg_type, request_len, sizeof(uint32_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    uint32_t act_id = *(uint32_t*)p_request_body;

    sprintf(g_sql_str, "select reward_id, reward_num,  flag from db_monster_%d.t_activity_%d where activity_id = %u and user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), act_id, user_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if(result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exev failed(%s)", g_sql_str);
        return ERR_SQL_ERR;
    }

    char buffer[1024] = {0};
    single_activity_t *p_ret = (single_activity_t*)buffer;
    p_ret->activity_id = act_id;
    p_ret->kind_num = 0;
    uint16_t idx = 0;
    while(row != NULL)
    {
        if(str2uint(&p_ret->reward[idx].reward_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert reward_id %s to uint32 failed(%s).", row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_ret->reward[idx].reward_num, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "convert reward_num %s to uint32 failed(%s).", row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_ret->reward[idx].reward_status, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "convert reward_status %s to uint32 failed(%s).", row[2], g_sql_str);
            return ERR_SQL_ERR;
        }
        idx++;
        row = p_mysql_conn->select_next_row(true);
    }
    p_ret->kind_num = idx;
    KINFO_LOG(user_id, "get act :%s", g_sql_str);
    g_pack.pack(buffer, sizeof(single_activity_t) + idx * sizeof(single_reward_t));
    return 0;
}
uint32_t activity_update_invite_friend(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if(check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    sprintf(g_sql_str, "UPDATE db_monster_%d.t_invite_%d SET qualified_num = qualified_num + 1 where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id),  user_id);
    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    if (mysql_autocommit(p_conn, false) != 0)
    {
        KCRIT_LOG(user_id, "close mysql_autocommit failed.");
        return ERR_SQL_ERR;
    }

    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }

    if (mysql_commit(p_conn) != 0)
    {
        KCRIT_LOG(user_id, "mysql_commit() failed.");
        mysql_rollback(p_conn);
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }

    mysql_autocommit(p_conn, true);

    return 0;
}

uint32_t activity_get_invite_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if(check_val_len(msg_type, request_len, sizeof(uint32_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

	uint32_t activity_id = (uint32_t)(*p_request_body);
	db_return_get_invite_info_t res = {0};
	uint16_t idx = 0;
	uint8_t status = NOT_FINISH;
    MYSQL_ROW row = NULL;
	MYSQL *p_conn = p_mysql_conn->get_conn();

	if (NULL == p_conn)
	{
		 KCRIT_LOG(user_id, "get mysql conn failed.");
		 return ERR_SQL_ERR;
	}
	if (mysql_autocommit(p_conn, false) != 0)
	{
		 KCRIT_LOG(user_id, "close mysql_autocommit failed.");
		 return ERR_SQL_ERR;
	}

	sprintf(g_sql_str, "SELECT invite_num, qualified_num FROM db_monster_%d.t_invite_%d WHERE user_id = %u;", DB_ID(user_id), TABLE_ID(user_id),  user_id);
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if(result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exev failed(%s)", g_sql_str);
        return ERR_SQL_ERR;
    }
    if(row != NULL)
    {
        if(str2uint(&res.invite_num, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert invite_num %s to uint16 failed(%s).", row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&res.qualified_num, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "convert qualified_num %s to uint16 failed(%s).", row[1], g_sql_str);
            return ERR_SQL_ERR;
        }
    }

	sprintf(g_sql_str, "SELECT flag FROM db_monster_%d.t_activity_%d WHERE user_id = %u AND activity_id = %u ORDER BY reward_id;", DB_ID(user_id), TABLE_ID(user_id),  user_id, activity_id);
	result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
	if(result_count < 0)
	{
		 KCRIT_LOG(user_id, "sql exev failed(%s)", g_sql_str);
		 return ERR_SQL_ERR;
	}

	for(idx=0; idx<3; idx++, row = p_mysql_conn->select_next_row(true))
	{
		switch(idx+1)
		{
			case 1:
				status = res.invite_num >= 1?FINISH_HAVENOT_REWARD:NOT_FINISH;
				break;
			case 2:
				status = res.invite_num >= 3?FINISH_HAVENOT_REWARD:NOT_FINISH;
				break;
			case 3:
				status = res.qualified_num >= 3 && res.invite_num >= 5?FINISH_HAVENOT_REWARD:NOT_FINISH;
				break;
		}

		if(row)
		{
			if(str2uint(&res.level_status[idx], row[0]) != 0)
			{
				 KCRIT_LOG(user_id, "convert level_status %s to uint8 failed(%s).", row[0], g_sql_str);
				 return ERR_SQL_ERR;
			}
			if(res.level_status[idx] != NOT_FINISH || status == NOT_FINISH)
			{
				continue;
			}
		}
		res.level_status[idx] = status;

		sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_activity_%d(user_id, activity_id, reward_id, flag) VALUES(%u, %u, %u, %u) ON DUPLICATE KEY UPDATE flag = %u;", DB_ID(user_id), TABLE_ID(user_id),  user_id, activity_id, idx+1, status, status);
		if (p_mysql_conn->execsql(g_sql_str) < 0)
		{
			 KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
			 mysql_rollback(p_conn);
			 mysql_autocommit(p_conn, true);
			 return ERR_SQL_ERR;
		}
	}

    if (mysql_commit(p_conn) != 0)
    {
        KCRIT_LOG(user_id, "mysql_commit() failed.");
        mysql_rollback(p_conn);
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }

    mysql_autocommit(p_conn, true);

	g_pack.pack((char *)&res, sizeof(res));
    return 0;
}

uint32_t activity_get_dragon_boat_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if(check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

	db_return_get_dragon_boat_t res = {0};
	MYSQL_ROW row = NULL;

	sprintf(g_sql_str, "SELECT reward_id, flag FROM db_monster_%d.t_activity_%d WHERE user_id = %u AND activity_id = %u;", DB_ID(user_id), TABLE_ID(user_id),  user_id, DRAGON_BOAT_ACTIVITY_ID);
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if(result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exev failed(%s)", g_sql_str);
        return ERR_SQL_ERR;
    }

	uint8_t count = 0, level_id, flag[3];
	while(row)
	{

		if(str2uint(&level_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert status %s to uint8 failed(%s).", row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

		if(str2uint(&flag[level_id -1], row[1]) != 0)
        {
            KCRIT_LOG(user_id, "convert status %s to uint8 failed(%s).", row[0], g_sql_str);
            return ERR_SQL_ERR;
        }
		res.status[level_id-1] = 1;
		count++;
		row = p_mysql_conn->select_next_row(true);
	}

	if(count == 3)
	{
		res.is_reward = flag[0] == FINISH_HAVENOT_REWARD?0:1;
	}

	g_pack.pack((char *)&res, sizeof(res));
	return 0;
}

uint32_t activity_update_dragon_boat_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if(check_val_len(msg_type, request_len, sizeof(uint8_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

	uint8_t level_id = *(uint8_t *)p_request_body;
 	uint8_t status = 0;
    MYSQL_ROW row = NULL;
	MYSQL *p_conn = NULL;

	sprintf(g_sql_str, "SELECT flag FROM db_monster_%d.t_activity_%d WHERE user_id = %u AND activity_id = %u AND reward_id = %u limit 1;", DB_ID(user_id), TABLE_ID(user_id),  user_id, DRAGON_BOAT_ACTIVITY_ID, level_id);
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if(result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exev failed(%s)", g_sql_str);
        return ERR_SQL_ERR;
    }

	if(row)
	{
		status = 1;
	}
	else
	{
		p_conn = p_mysql_conn->get_conn();
		if (NULL == p_conn)
		{
			 KCRIT_LOG(user_id, "get mysql conn failed.");
			 return ERR_SQL_ERR;
		}

		if (mysql_autocommit(p_conn, false) != 0)
		{
			 KCRIT_LOG(user_id, "close mysql_autocommit failed.");
			 return ERR_SQL_ERR;
		}

		sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_activity_%d(user_id, activity_id, reward_id, flag) VALUES(%u, %u, %u, 2);", DB_ID(user_id), TABLE_ID(user_id),	user_id, DRAGON_BOAT_ACTIVITY_ID, level_id);
		if (p_mysql_conn->execsql(g_sql_str) < 0)
		{
			 KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
			 mysql_rollback(p_conn);
			 mysql_autocommit(p_conn, true);
			 return ERR_SQL_ERR;
		}
		mysql_autocommit(p_conn, true);
	}

	db_return_update_dragon_boat_t res = {level_id, status};
	g_pack.pack((char *)&res, sizeof(res));
    return 0;
}

uint32_t activity_get_activity_reward_ex(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if(check_val_len(msg_type, request_len, sizeof(db_request_get_activity_reward_ex_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

	db_request_get_activity_reward_ex_t *req = (db_request_get_activity_reward_ex_t *)p_request_body;
	db_return_get_activity_reward_ex_t  res = {0};

	uint8_t status = NOT_FINISH;
    MYSQL_ROW row = NULL;
	MYSQL *p_conn = NULL;

	sprintf(g_sql_str, "SELECT flag FROM db_monster_%d.t_activity_%d WHERE user_id = %u AND activity_id = %u AND reward_id = %u limit 1;", DB_ID(user_id), TABLE_ID(user_id),  user_id, req->activity_id, req->level_id);
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if(result_count <= 0)
    {
        KCRIT_LOG(user_id, "sql exev failed(%s)", g_sql_str);
        return ERR_SQL_ERR;
    }
    if(row != NULL)
    {
        if(str2uint(&status, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert status %s to uint8 failed(%s).", row[0], g_sql_str);
            return ERR_SQL_ERR;
        }
		if(status != FINISH_HAVENOT_REWARD)
		{
			KCRIT_LOG(user_id, "this activity level have rewarded or not finished");
			return ERR_SQL_ERR;
		}

		if(req->reward_id == TASK_REWARD_COIN)
		{
			sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set coins = coins + %u where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), req->reward_num, user_id);
		}
		else if(req->reward_id == TASK_REWARD_EXP)
		{
		}
		else if(req->reward_id >= ITEM_ID_BEGIN && req->reward_id <= ITEM_ID_END)
		{
			sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) values(%u, %u, %u) ON DUPLICATE KEY UPDATE stuff_num = stuff_num + %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, req->reward_id, req->reward_num, req->reward_num);
		}
		else if(req->reward_id >= PET_ID_BEGIN && req->reward_id <= PET_ID_END)
		{
			sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_pet_%d(user_id, pet_id, total_num, follow_num) values(%u, %u, 1, 1) ON DUPLICATE KEY UPDATE total_num = total_num + 1, follow_num = follow_num + 1;", DB_ID(user_id), TABLE_ID(user_id), user_id, req->reward_id);
		}
		else if(req->reward_id == TASK_REWARD_NONE)
		{
			goto end;
		}
		else
		{
			KCRIT_LOG(user_id, "reward id error.");
			return ERR_ITEM_NOT_EXIST;
		}

		p_conn = p_mysql_conn->get_conn();
		if (NULL == p_conn)
		{
			 KCRIT_LOG(user_id, "get mysql conn failed.");
			 return ERR_SQL_ERR;
		}

		if (mysql_autocommit(p_conn, false) != 0)
		{
			 KCRIT_LOG(user_id, "close mysql_autocommit failed.");
			 return ERR_SQL_ERR;
		}

		if (p_mysql_conn->execsql(g_sql_str) < 0)
		{
			 KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
			 mysql_rollback(p_conn);
			 mysql_autocommit(p_conn, true);
			 return ERR_SQL_ERR;
		}

		sprintf(g_sql_str, "UPDATE db_monster_%d.t_activity_%d SET flag = %u WHERE user_id = %u AND activity_id = %u AND reward_id = %u;", DB_ID(user_id), TABLE_ID(user_id), FINISH_HAVE_REWARD, user_id, req->activity_id, req->level_id);
		if (p_mysql_conn->execsql(g_sql_str) < 0)
		{
			 KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
			 mysql_rollback(p_conn);
			 mysql_autocommit(p_conn, true);
			 return ERR_SQL_ERR;
		}

		if (mysql_commit(p_conn) != 0)
	    {
	        KCRIT_LOG(user_id, "mysql_commit() failed.");
	        mysql_rollback(p_conn);
	        mysql_autocommit(p_conn, true);
	        return ERR_SQL_ERR;
	    }

	    mysql_autocommit(p_conn, true);

        if(req->reward_id == TASK_REWARD_COIN)
        {
            role_t cached_role = {{0}};
            if(p_memcached->get_role(user_id, &cached_role) == 0)
            {
                cached_role.coins += req->reward_num;
                p_memcached->set_role(user_id, &cached_role);
            }
        }
    }

end:
	res.reward_id = req->reward_id;
	res.reward_num = req->reward_num;
	g_pack.pack((char *)&res, sizeof(res));
	return 0;
}

uint32_t update_activity_status(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    single_activity_t *p_sa = (single_activity_t*)p_request_body;
    if(check_val_len(msg_type, request_len, sizeof(single_activity_t) + p_sa->kind_num * sizeof(single_reward_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    if (mysql_autocommit(p_conn, false) != 0)
    {
        KCRIT_LOG(user_id, "close mysql_autocommit failed.");
        return ERR_SQL_ERR;
    }

    for(uint16_t idx = 0; idx < p_sa->kind_num; idx++)
    {
		sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_activity_%d(user_id, activity_id, reward_id, reward_num,  flag) values(%u, %u, %u, %u, %u) ON DUPLICATE KEY UPDATE flag = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_sa->activity_id, p_sa->reward[idx].reward_id, p_sa->reward[idx].reward_num, p_sa->reward[idx].reward_status, p_sa->reward[idx].reward_status);
		KINFO_LOG(user_id, "update open act_reward:%s", g_sql_str);
		if (p_mysql_conn->execsql(g_sql_str) < 0)
		{
			KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
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

    g_pack.pack(p_request_body, request_len);
    return 0;
}
