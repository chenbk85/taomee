/*
 * =====================================================================================
 *
 *       Filename:  museum.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年04月17日 15时11分46秒
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
#include "../../../common/pack/c_pack.h"
#include "../../../common/message.h"
#include "../../../common/data_structure.h"
#include "../../../common/constant.h"
#include "../util.h"
#include "../db_constant.h"
#include "museum.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t enter_museum(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
	if (check_val_len(msg_type, request_len, sizeof(uint32_t)) != 0)
	{
		return ERR_MSG_LEN;
	}

	museum_info_t museum_item={0};
	museum_item.museum_id = *(uint32_t *)p_request_body;

	sprintf(g_sql_str, "SELECT cur_level, reward_flag, timestamp FROM db_monster_%d.t_museum_%d WHERE user_id = %u AND museum_id = %u ORDER BY timestamp desc limit 1;", DB_ID(user_id), TABLE_ID(user_id), user_id, museum_item.museum_id);

	MYSQL_ROW row = NULL;
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
	if (result_count < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}
	else if(result_count == 1 && row != NULL)
	{
		if(str2uint(&museum_item.level_id, row[0]) != 0)
		{
			KCRIT_LOG(user_id, "convert level:%s to uint32 failed.", row[0]);
			return ERR_SQL_ERR;
		}

		if(str2uint(&museum_item.reward_flag, row[1]) != 0)
		{
			KCRIT_LOG(user_id, "convert flag:%s to uint32 failed.", row[1]);
			return ERR_SQL_ERR;
		}

		if(str2uint(&museum_item.timestamp, row[2]) != 0)
		{
			KCRIT_LOG(user_id, "convert timestamp:%s to uint32 failed.", row[2]);
			return ERR_SQL_ERR;
		}
	}

	g_pack.pack((char *)&museum_item, sizeof(museum_item));

	return 0;
}

uint32_t commit_museum_game(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_request_commit_museum_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_request_commit_museum_t *p_req = (db_request_commit_museum_t *)p_request_body;

    sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_museum_%d(user_id, museum_id, cur_level, reward_flag, timestamp) values(%u, %u, %u, 0, %u) ON duplicate key UPDATE cur_level = %u, reward_flag = 0;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->museum_id, p_req->level_id, p_req->timestamp, p_req->level_id);

    KINFO_LOG(user_id, "commit_museum_anserr:%s", g_sql_str);
	if(p_mysql_conn->execsql(g_sql_str) < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}

	g_pack.pack(p_request_body, request_len);

	return 0;
}

uint32_t get_museum_reward(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_add_museum_reward_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

	uint8_t reward_flag = 0;
    uint8_t cur_level = 0;
    db_msg_add_museum_reward_t *p_req = (db_msg_add_museum_reward_t *)p_request_body;


    sprintf(g_sql_str, "SELECT reward_flag, cur_level FROM db_monster_%d.t_museum_%d WHERE user_id=%u AND museum_id=%u AND timestamp=%u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->museum_id,p_req->timestamp);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);

	KINFO_LOG(user_id, "get_museum_reward:%s, result_count = %u", g_sql_str, result_count);
	if(result_count == 1 && row != NULL)
    {
        if(str2uint(&reward_flag, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert flag:%s to uint32 failed.", row[0]);
            return ERR_SQL_ERR;
        }

        if(str2uint(&cur_level, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "convert cur_level:%s to uint32 failed.", row[1]);
        }

        if(cur_level != p_req->level_id || reward_flag == HAVE_GET_REWARD)
        {
            KCRIT_LOG(user_id, "This user has get the reward  or level not consistent.");
            return ERR_SQL_ERR;
        }

        sprintf(g_sql_str, "UPDATE db_monster_%d.t_museum_%d SET reward_flag = 1 WHERE user_id= %u AND museum_id = %u AND timestamp=%u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->museum_id, p_req->timestamp);

        MYSQL *p_conn = p_mysql_conn->get_conn();
        if(p_conn == NULL)
        {
            KCRIT_LOG(user_id, "get mysql conn failed.");
            return ERR_SQL_ERR;
        }
        if(mysql_autocommit(p_conn, false) != 0)
        {
            KCRIT_LOG(user_id, "close mysql_autocommit failed.");
            return ERR_SQL_ERR;
        }
        if(p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn,true);
            return ERR_SQL_ERR;
        }

		switch(p_req->reward_id)
		{
			case TASK_REWARD_NONE:
				break;
			case TASK_REWARD_COIN:
				sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set coins = coins + %u where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), p_req->reward_num, user_id);
				break;
			case TASK_REWARD_EXP:
				break;
			default:
				sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) values(%u, %u, %u) ON DUPLICATE KEY UPDATE stuff_num = stuff_num + %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->reward_id, p_req->reward_num, p_req->reward_num);
		}

		  if(p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn,true);
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

        if(p_req->reward_id == TASK_REWARD_COIN)
        {
            role_t cached_role = {{0}};
            if(p_memcached->get_role(user_id, &cached_role) == 0)
            {
                cached_role.coins += p_req->reward_num;
                p_memcached->set_role(user_id, &cached_role);
            }

        }

        g_pack.pack(p_request_body, request_len);
        return 0;
    }
	else
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}

}

uint32_t prize_lottery(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_request_prize_lottery_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_request_prize_lottery_t *p_req = (db_request_prize_lottery_t *)p_request_body;

    MYSQL_ROW row = NULL;
	MYSQL *p_conn = p_mysql_conn->get_conn();
     if(p_conn == NULL)
     {
         KCRIT_LOG(user_id, "get mysql conn failed.");
         return ERR_SQL_ERR;
     }
     if(mysql_autocommit(p_conn, false) != 0)
     {
         KCRIT_LOG(user_id, "close mysql_autocommit failed.");
         return ERR_SQL_ERR;
     }

	sprintf(g_sql_str, "UPDATE db_monster_%d.t_stuff_%d SET stuff_num = stuff_num-%u WHERE user_id= %u AND stuff_id=%u;", DB_ID(user_id), TABLE_ID(user_id), p_req->cost, user_id, p_req->stuff_a_id);
	if(p_mysql_conn->execsql(g_sql_str) < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		mysql_rollback(p_conn);
		mysql_autocommit(p_conn,true);
		return ERR_SQL_ERR;
	}

	sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) VALUES(%u, %u, 1) ON DUPLICATE KEY UPDATE stuff_num = stuff_num+1;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->stuff_b_id);
	if(p_mysql_conn->execsql(g_sql_str) < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		mysql_rollback(p_conn);
		mysql_autocommit(p_conn,true);
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

	g_pack.pack(p_request_body, request_len);
	return 0;
}

