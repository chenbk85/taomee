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
#include "game_changed.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t get_game_changed(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_get_changed_stuff_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_get_changed_stuff_t *p_changed_stuff = (db_msg_get_changed_stuff_t *)p_request_body;
    KINFO_LOG(user_id, "[get_game_chagned:game%u period%u].", p_changed_stuff->game_id, p_changed_stuff->period_id);


    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT stuff_id FROM db_monster_%d.t_game_changed_%d WHERE user_id = %u AND game_id = %u and period_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_changed_stuff->game_id, p_changed_stuff->period_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    char buffer[4096] = {0};
    db_msg_changed_stuff_t *p_changed = (db_msg_changed_stuff_t*)buffer;

    p_changed->game_id = p_changed_stuff->game_id;
    p_changed->changed_stuff_num = 0;
    uint16_t idx = 0;
    while(row != NULL)
    {
        if(str2uint(&p_changed->changed_stuff[idx], row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert stuff_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        idx++;
        row = p_mysql_conn->select_next_row(true);
    }

    p_changed->changed_stuff_num = idx;

    g_pack.pack(buffer, sizeof(db_msg_changed_stuff_t) + idx * sizeof(uint32_t));

    return 0;
}

uint32_t set_game_changed(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_get_changed_stuff_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_get_changed_stuff_t *p_changed_stuff = (db_msg_get_changed_stuff_t *)p_request_body;
    KINFO_LOG(user_id, "[get_game_chagned:game%u period%u stuff:%u, repeat:%u].", p_changed_stuff->game_id, p_changed_stuff->period_id, p_changed_stuff->stuff_id, p_changed_stuff->repeat);

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

    if(p_changed_stuff->repeat == 0)
    {
        sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_game_changed_%d(user_id, game_id, period_id, stuff_id) VALUES(%u, %u, %u, %u);", DB_ID(user_id), TABLE_ID(user_id), user_id, p_changed_stuff->game_id, p_changed_stuff->period_id, p_changed_stuff->stuff_id);

        KINFO_LOG(user_id, "changed_stuff:%s", g_sql_str);

        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
    }
    sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) values(%u, %u, 1) ON DUPLICATE KEY UPDATE stuff_num = stuff_num + 1;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_changed_stuff->stuff_id);

    KINFO_LOG(user_id, "changed_stuff:%s", g_sql_str);
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

    g_pack.pack(p_request_body, sizeof(db_msg_get_changed_stuff_t));

    return 0;
}

