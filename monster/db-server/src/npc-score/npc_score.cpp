/**
 * =====================================================================================
 *       @file  pet.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/02/2011 06:25:44 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
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
#include "npc_score.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t get_npc_score(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    sprintf(g_sql_str, "SELECT npc_id, npc_score FROM db_monster_%d.t_npc_score_%d WHERE user_id = %u order by day_time desc limit 3;", DB_ID(user_id), TABLE_ID(user_id), user_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    char buffer[8192] = {0};
    db_return_npc_score_t *p_npc = (db_return_npc_score_t *)(buffer);
    p_npc->npc_num = NPC_SCORE_NUM;
    int idx = 0;
    while (row != NULL)
    {
        if(str2uint(&p_npc->npc[idx].npc_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert npc_id:%s to uint32 failed(%s).", row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_npc->npc[idx].npc_score, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "convert npc_score:%s to uint32 failed(%s).", row[1], g_sql_str);
            return ERR_SQL_ERR;
        }
        ++idx;
        if(idx >= NPC_SCORE_NUM)
        {
            break;
        }


        row = p_mysql_conn->select_next_row(true);
    }

    g_pack.pack(buffer, sizeof(db_return_npc_score_t));

    return 0;
}

uint32_t set_npc_score(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{

    db_msg_set_npc_score_t *p_req = (db_msg_set_npc_score_t *)p_request_body;
    if (check_val_len(msg_type, request_len, sizeof(db_msg_set_npc_score_t)) != 0)
    {
        return ERR_MSG_LEN;
    }


    snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_role_%d SET npc_score_daytime = %u, npc_score = %u WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), p_req->day_time, p_req->total_score, user_id);

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
            //更改memcache里的值
            role_t cached_role = {{0}};
            if (p_memcached->get_role(user_id, &cached_role) == 0)
            {
                cached_role.npc_score_daytime = p_req->day_time;
                cached_role.npc_score = p_req->total_score;
                p_memcached->set_role(user_id, &cached_role);
            }

    for (int i = 0; i != (int)p_req->npc_num; ++i)
    {
    snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_npc_score_%d(user_id, npc_id, day_time, npc_score) VALUES(%u, %u, %u, %u) ON DUPLICATE KEY UPDATE npc_id = %u , npc_score = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->npc[i].npc_id, p_req->day_time,  p_req->npc[i].npc_score, p_req->npc[i].npc_id, p_req->npc[i].npc_score);
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

    g_pack.pack(p_req->day_time);

    return 0;
}

uint32_t encourage_guide(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_request_encourage_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    db_request_encourage_t *req = (db_request_encourage_t *)p_request_body;
    sprintf(g_sql_str, "INSERT INTO db_monster_config.t_encourage_guide(type, user_id, timestamp, score) VALUES(%u, %u, %u, %u) ON DUPLICATE KEY UPDATE timestamp = %u, score = %u;", req->type, user_id,  req->timestamp, req->score, req->timestamp, req->score);

	if (p_mysql_conn->execsql(g_sql_str) < 0)
	{
		KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}

    sprintf(g_sql_str, "SELECT(SELECT COUNT(*) FROM db_monster_config.t_encourage_guide WHERE type=%u AND timestamp=%u), (SELECT COUNT(*) FROM db_monster_config.t_encourage_guide WHERE type=%u AND timestamp=%u AND score<%u), (SELECT user_id FROM db_monster_config.t_encourage_guide WHERE type=%u AND timestamp=%u AND score>%u limit 1);", req->type, req->timestamp,req->type, req->timestamp, req->score, req->type, req->timestamp, req->score);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    KINFO_LOG(user_id, "sql:%s", g_sql_str);

    char buffer[4096] = {0};
    db_return_encourage_t *p_rsp = (db_return_encourage_t *)buffer;
    p_rsp->type = req->type;
    p_rsp->score = req->score;
	if(str2uint(&p_rsp->total_num, row[0]) != 0)
	{
		KCRIT_LOG(user_id, "[msg:%u]convert total_num:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&p_rsp->win_num, row[1]) != 0)
	{
		KCRIT_LOG(user_id, "[msg:%u]convert win_num:%s to uint32 failed(%s).", msg_type, row[1], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(row[2])
	{
		if(str2uint(&p_rsp->user_id, row[2]) != 0)
		{
			KCRIT_LOG(user_id, "[msg:%u]convert user_id:%s to uint32 failed(%s).", msg_type, row[2], g_sql_str);
			return ERR_SQL_ERR;
		}
	}
	else
	{
		p_rsp->user_id = 12345;
	}

	KINFO_LOG(user_id, "win:%d total:%d uid:%d", p_rsp->win_num,p_rsp->total_num,p_rsp->user_id);

    g_pack.pack(buffer, sizeof(db_return_encourage_t));

    return 0;
}

