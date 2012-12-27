/*
 * =====================================================================================
 *
 *       Filename:  donate.cpp
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
#include "donate.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t request_donate_history(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    sprintf(g_sql_str, "SELECT donate_id, donate, helped_pet, donor_count, reach_time FROM db_monster_config.t_donate order by donate_id desc;");


    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    KINFO_LOG(user_id, "sql:%s", g_sql_str);

    char buffer[4096] = {0};
    msg_history_donate_t *res = (msg_history_donate_t*)buffer;

    uint16_t idx = 0;

    while(row != NULL)
    {
		 uint32_t db_donate_id, db_donate, db_donor_count, db_reach_time;
		 uint16_t db_helped_pet;
        if(str2uint(&db_donate_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert donate_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&db_donate, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert donor_count:%s to uint32 failed(%s).", msg_type, row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&db_helped_pet, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert cur_donate:%s to uint32 failed(%s).", msg_type, row[2], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&db_donor_count, row[3]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert reach_time:%s to uint32 failed(%s).", msg_type, row[3], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&db_reach_time, row[4]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert reach_time:%s to uint32 failed(%s).", msg_type, row[3], g_sql_str);
            return ERR_SQL_ERR;
        }

		res->donate[idx].donate_id = db_donate_id;
		res->donate[idx].end_time = db_reach_time;
		res->donate_total += db_donate;
		res->helped_pet += db_helped_pet;
		res->total_participater += db_donor_count;
        idx++;

        row = p_mysql_conn->select_next_row(true);
    }

    res->history_num = idx;
    g_pack.pack(buffer, sizeof(msg_history_donate_t) + idx * sizeof(history_donate_t));

    return 0;
}

uint32_t request_cur_donate(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    MYSQL_ROW row = NULL;
 	sprintf(g_sql_str, "SELECT donate_id, reach_time, donate, donor_count FROM db_monster_config.t_donate ORDER BY donate_id desc limit 1;");
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    KINFO_LOG(user_id, "sql:%s", g_sql_str);

    db_return_cur_donate_t res = {0};

    if(row == NULL)
    {//没有记录，不应该，创建表时就应该插入一条记录
        KCRIT_LOG(user_id, "system error, must have init data int db");
        return ERR_SQL_ERR;
    }
    else // row != NULL
    {
        uint32_t db_donate_id, db_reach_time, reward_donate_id, db_donate, db_donor_count;

        if(str2uint(&db_donate_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert donate_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&db_reach_time, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert reach_time:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&db_donate, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert db_donate:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&db_donor_count, row[3]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert db_donor_count:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(db_reach_time)
        {//已经达成
            res.donate_id = db_donate_id + 1;
            res.donate_num = 0;
            res.donor_count = 0;
            res.money = 0;

            //查看是否有未领取的奖品
            sprintf(g_sql_str, "SELECT DISTINCT donate_id FROM db_monster_config.t_donate_detail WHERE user_id=%u AND is_reward=%u AND donate_id<=%u;", user_id, HAVENOT_GET_REWARD, db_donate_id);

        }
        else
        {//未达成
            res.donate_id = db_donate_id;
            res.donate_num = db_donate;
            res.donor_count = db_donor_count;
            res.money = 0;

            //查看是否有未领取的奖品
            sprintf(g_sql_str, "SELECT DISTINCT donate_id FROM db_monster_config.t_donate_detail WHERE user_id=%u AND is_reward=%u AND donate_id < %u;", user_id, HAVENOT_GET_REWARD, db_donate_id);
        }


        row = NULL;
        result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
        if (result_count < 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            return ERR_SQL_ERR;
        }

        if(row != NULL)
        {
            if(str2uint(&reward_donate_id, row[0]) != 0)
            {
                KCRIT_LOG(user_id, "[msg:%u]convert donor_count:%s to uint32 failed(%s).", msg_type, row[1], g_sql_str);
                return ERR_SQL_ERR;
            }
            sprintf(g_sql_str, "UPDATE  db_monster_config.t_donate_detail SET  is_reward=%u WHERE donate_id=%u AND user_id=%u", HAVE_GET_REWARD, reward_donate_id, user_id);

            if (p_mysql_conn->execsql(g_sql_str) < 0)
            {
                KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                return ERR_SQL_ERR;
            }

            res.reward_donate_id = reward_donate_id;
        }
        else
        {
            res.reward_donate_id = 0;
        }
    }

    g_pack.pack((char *)(&res), sizeof(db_return_cur_donate_t));
    return 0;
}

uint32_t request_update_wealth(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_request_update_wealth_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

	db_request_update_wealth_t *req = (db_request_update_wealth_t *)p_request_body;

	MYSQL *p_conn = p_mysql_conn->get_conn();
	if (NULL == p_conn)
	{
		KCRIT_LOG(user_id, "get mysql conn failed.");
		return ERR_SQL_ERR;
	}

	if(req->stuff_id)
	{
		sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) values(%u, %u, %u) ON DUPLICATE KEY UPDATE stuff_num = stuff_num + %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, req->stuff_id, req->stuff_num, req->stuff_num);

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

		mysql_autocommit(p_conn, true);
	}

	if(req->money != 0)
	{
		sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set coins = coins - %u where user_id = %u and coins > 0;", DB_ID(user_id), TABLE_ID(user_id), req->money, user_id);

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


		role_t cached_role = {{0}};
		if(p_memcached->get_role(user_id, &cached_role) == 0)
		{
			cached_role.coins -= req->money;
			p_memcached->set_role(user_id, &cached_role);
		}
		mysql_autocommit(p_conn, true);
	}

	g_pack.pack((char *)req, sizeof(db_request_update_wealth_t));
    return 0;
}

uint32_t request_donate_coins(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_request_donate_coins_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    uint32_t db_donate_id = 0,
             db_donate = 0,
             db_donate_person = 0,
             db_donor_count = 0,
             db_reach_time = 0;
    uint16_t db_helped_pet = 0;

    db_request_donate_coins_t *req = (db_request_donate_coins_t *)p_request_body;
    db_return_donate_coins_t res={0};
    res.donate_id = res.next_donate_id = req->req_donate_id;

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

    MYSQL_ROW row = NULL;

	sprintf(g_sql_str, "SELECT donate_id, donate, donor_count, reach_time FROM db_monster_config.t_donate ORDER BY donate_id desc limit 1;");
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);

    if(result_count == 1)
    {
        if(str2uint(&db_donate_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert donate_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&db_donate, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert donate:%s to uint32 failed(%s).", msg_type, row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&db_donor_count, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert db_donor_count:%s to uint32 failed(%s).", msg_type, row[2], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&db_reach_time, row[3]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert db_reach_time:%s to uint32 failed(%s).", msg_type, row[3], g_sql_str);
            return ERR_SQL_ERR;
        }

        if((req->req_donate_id == db_donate_id && db_reach_time)|| (req->req_donate_id < db_donate_id))
        {//本期已经达成
            res.flag = DENOTE_FAIL_AND_REACHED;
            res.next_donate_id = ++db_donate_id ;
        }
        else
        {
            sprintf(g_sql_str, "SELECT SUM(donate) FROM db_monster_config.t_donate_detail WHERE donate_id=%u AND user_id=%u;", req->req_donate_id, user_id);

		    row = NULL;
            result_count = p_mysql_conn->select_first_row(&row, g_sql_str);

            if (result_count < 0)
            {
                KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                return ERR_SQL_ERR;
            }
            else if(row[0])
            {
                if(str2uint(&db_donate_person, row[0]) != 0)
                {
                    KCRIT_LOG(user_id, "[msg:%u]convert db_donate_person:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
                    return ERR_SQL_ERR;
                }

                if((db_donate_person + req->req_donate_coins) > req->donate_info.donate_limit)
                {
                    res.flag = DENOTE_REACHED_LIMIT;
                }
            }
            else if(db_reach_time && req->req_donate_id>db_donate_id)
            {
            	   db_donate_id = req->req_donate_id;
			   db_donate = 0;
			   db_reach_time = 0;
            }
			else
			{
				db_donor_count++;
			}
        }

	    if(res.flag != DENOTE_FAIL_AND_REACHED && res.flag != DENOTE_REACHED_LIMIT)
	    {
	        res.money = req->req_donate_coins;
	        db_donate += req->req_donate_coins;
	        if(db_donate >= req->donate_info.donate_total)
	        {
	            res.flag = DENOTE_SUCCESS_AND_REACHED;
	            res.next_donate_id++;
	            db_reach_time = req->req_donate_timestamp;
	            db_helped_pet = req->donate_info.helped_pet;
	        }
	        else
	        {
	            res.flag = DENOTE_SUCCESS_BUT_NOT_REACHED;
	        }

	        sprintf(g_sql_str, "INSERT INTO db_monster_config.t_donate(donate_id, donate, helped_pet, donor_count, reach_time) VALUES(%u, %u, 0, 1, 0) ON DUPLICATE KEY UPDATE donate=%u, helped_pet=%u, donor_count=%u, reach_time=%u;", db_donate_id, db_donate, db_donate, db_helped_pet, db_donor_count, db_reach_time);
	        if (p_mysql_conn->execsql(g_sql_str) < 0)
	        {
	            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
	            mysql_rollback(p_conn);
	            mysql_autocommit(p_conn, true);
	            return ERR_SQL_ERR;
	        }


	        sprintf(g_sql_str, "INSERT INTO db_monster_config.t_donate_detail(donate_id, user_id, donate, timestamp, is_reward) VALUES(%u, %u, %u, %u, %u);", req->req_donate_id, user_id, req->req_donate_coins, req->req_donate_timestamp, HAVENOT_GET_REWARD);
	        if (p_mysql_conn->execsql(g_sql_str) < 0)
	        {
	            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
	            mysql_rollback(p_conn);
	            mysql_autocommit(p_conn, true);
	            return ERR_SQL_ERR;
	        }

	    }

	    if(res.flag == DENOTE_SUCCESS_AND_REACHED || res.flag == DENOTE_FAIL_AND_REACHED)
	    {
	        //res.reward_donate_id = req->req_donate_id;
	        res.reward_id = req->donate_info.reward_id;
	        sprintf(g_sql_str, "UPDATE db_monster_config.t_donate_detail SET is_reward=%u WHERE donate_id=%u AND user_id=%u;", HAVE_GET_REWARD, req->req_donate_id, user_id);
	        int ret = p_mysql_conn->execsql(g_sql_str);
	        if (ret < 0)
	        {
	            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
	            mysql_rollback(p_conn);
	            mysql_autocommit(p_conn, true);
	            return ERR_SQL_ERR;
	        }
	        res.reward_donate_id = ret?req->req_donate_id:0;
	    }
	    mysql_autocommit(p_conn, true);

	    g_pack.pack((char *)(&res), sizeof(res));
    }
	else
	{
		KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}

    return 0;
}

