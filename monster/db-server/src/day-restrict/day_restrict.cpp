/**
 * =====================================================================================
 *       @file  puzzle.cpp
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
#include "../../../common/pack/c_pack.h"
#include "../../../common/message.h"
#include "../../../common/data_structure.h"
#include "../../../common/constant.h"
#include "../util.h"
#include "../db_constant.h"
#include "day_restrict.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t get_day_restrict(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_get_day_restrict_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_get_day_restrict_req_t *p_req = (db_msg_get_day_restrict_req_t *)p_request_body;
    KINFO_LOG(user_id, "[get day restrict,time:%u, type:%u].", p_req->time, p_req->type);

    uint32_t value = 0;
    if (p_memcached->get_day_restrict(user_id, p_req->type, p_req->time, &value) == 0)
    {
        g_pack.pack(value);
        return 0;
    }

    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT value FROM db_monster_%d.t_day_restrict_%d WHERE user_id = %u AND time = %u AND type = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->time, p_req->type);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }
    else if (0 == result_count)
    {
        value = 0;
    }
    else if(str2uint(&value, row[0]) != 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]convert value:%s to uint8 failed(%s).", msg_type, row[0], g_sql_str);
        return ERR_SQL_ERR;
    }

    g_pack.pack(value);

    p_memcached->set_day_restrict(user_id, value, p_req->type, p_req->time);

    return 0;
}

uint32_t add_day_restrict(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    db_msg_add_day_restrict_req_t *p_req = (db_msg_add_day_restrict_req_t *)p_request_body;
    if (check_val_len(msg_type, request_len, sizeof(db_msg_add_day_restrict_req_t) + p_req->count * sizeof(restrict_value_t)) != 0)
    {
        return ERR_MSG_LEN;
    }


    KINFO_LOG(user_id, "[add day restrict,time:%u, type:%u, value:%u, count:%u].", p_req->time, p_req->type, p_req->value, p_req->count);

    snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_day_restrict_%d(user_id, time, type, value) VALUES(%u, %u, %u, %u) ON DUPLICATE KEY UPDATE value = value + %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->time, p_req->type, p_req->value, p_req->value);

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
    uint32_t value = 0;
    if (p_memcached->get_day_restrict(user_id, p_req->type, p_req->time, &value) == 0)
    {
        value += p_req->value;
        p_memcached->set_day_restrict(user_id, value, p_req->type, p_req->time);
    }

    for (int i = 0; i != (int)p_req->count; ++i)
    {
        //增加对应的value值
        if (p_req->type == COIN_ID) //增加的金币
        {
            snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_role_%d SET coins = coins + %u WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), p_req->restrict_stuff[i].value, user_id);
            if (p_mysql_conn->execsql(g_sql_str) < 0)
            {
                KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                mysql_rollback(p_conn);
                mysql_autocommit(p_conn, true);
                return ERR_SQL_ERR;
            }
            //更改memcache里的金币
            role_t cached_role = {{0}};
            if (p_memcached->get_role(user_id, &cached_role) == 0)
            {
                cached_role.coins += p_req->restrict_stuff[i].value;
                p_memcached->set_role(user_id, &cached_role);
            }
        }
	else if(p_req->type >= STRICT_STUFF_EXP)
	{//经验值限制的
			//nothing to do
	}
        else    //增加物品
        {
            snprintf(g_sql_str, sizeof(g_sql_str),
                    "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) VALUES(%u, %u, %u) "
                    "ON DUPLICATE KEY UPDATE stuff_num = stuff_num + %u",
                    DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->restrict_stuff[i].value_id, p_req->restrict_stuff[i].value, p_req->restrict_stuff[i].value);

            if (p_mysql_conn->execsql(g_sql_str) < 0)
            {
                KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                mysql_rollback(p_conn);
                mysql_autocommit(p_conn, true);
                return ERR_SQL_ERR;
            }
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

    return 0;
}

uint32_t add_game_day_restrict(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    db_msg_add_game_day_restrict_req_t *p_req = (db_msg_add_game_day_restrict_req_t *)p_request_body;
    if (check_val_len(msg_type, request_len, sizeof(db_msg_add_game_day_restrict_req_t) + p_req->count * sizeof(restrict_value_t)) != 0)
    {
        return ERR_MSG_LEN;
    }


    KINFO_LOG(user_id, "[add game day restrict,time:%u, type:%u, reward_coins:%u count:%u].", p_req->time, p_req->type, p_req->reward_coins, p_req->count);

    snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_day_restrict_%d(user_id, time, type, value) VALUES(%u, %u, %u, %u) ON DUPLICATE KEY UPDATE value = value + %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->time, p_req->type, p_req->count, p_req->count);

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

    if(p_req->reward_coins != 0)
    {
        snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_day_restrict_%d(user_id, time, type, value) VALUES(%u, %u, %u, %u) ON DUPLICATE KEY UPDATE value = value + %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->time, STRICT_COIN, p_req->reward_coins, p_req->reward_coins);
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }

        snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_role_%d SET coins = coins + %u WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), p_req->reward_coins, user_id);
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
        //更改memcache里的金币
        role_t cached_role = {{0}};
        if (p_memcached->get_role(user_id, &cached_role) == 0)
        {
            cached_role.coins += p_req->reward_coins;
            p_memcached->set_role(user_id, &cached_role);
        }
    }

    for (int i = 0; i != (int)p_req->count; ++i)
    {
            snprintf(g_sql_str, sizeof(g_sql_str),
                    "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) VALUES(%u, %u, %u) "
                    "ON DUPLICATE KEY UPDATE stuff_num = stuff_num + %u",
                    DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->restrict_stuff[i].value_id, p_req->restrict_stuff[i].value, p_req->restrict_stuff[i].value);

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

    return 0;
}

uint32_t get_game_day_restrict(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_get_day_restrict_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_get_day_restrict_req_t *p_req = (db_msg_get_day_restrict_req_t *)p_request_body;
    KINFO_LOG(user_id, "[get day restrict,time:%u, type:%u].", p_req->time, p_req->type);

    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT type, value FROM db_monster_%d.t_day_restrict_%d WHERE user_id = %u AND time = %u AND (type = %u or type = %u) ", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->time, p_req->type, STRICT_COIN);

    uint32_t restrict_value = 0;
    uint32_t restrict_coins = 0;
    uint8_t type = 0;
    uint32_t value = 0;
    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    while(row != NULL)
    {
       if(str2uint(&type, row[0]) != 0)
       {
            KCRIT_LOG(user_id, "[msg:%u]convert type:%s to uint8 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
       }

       if(str2uint(&value, row[1]) != 0)
       {
            KCRIT_LOG(user_id, "[msg:%u]convert value:%s to uint8 failed(%s).", msg_type, row[1], g_sql_str);
            return ERR_SQL_ERR;
       }

        if(type == STRICT_COIN)
        {
            restrict_coins = value;
        }
        else
        {
            restrict_value = value;
        }
        row = p_mysql_conn->select_next_row(true);
    }
    g_pack.pack(restrict_coins);
    g_pack.pack(restrict_value);

    return 0;
}
