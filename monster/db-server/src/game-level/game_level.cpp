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
#include "game_level.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t get_game_level(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(uint32_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    uint32_t *p_game_id = (uint32_t *)p_request_body;
    KINFO_LOG(user_id, "[get_game_level:%u].", *p_game_id);


    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT level_id, max_score, max_star, is_passed FROM db_monster_%d.t_game_%d WHERE user_id = %u AND game_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, *p_game_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    char buffer[4096] = {0};
    game_level_info_t *p_lvl = (game_level_info_t*)buffer;

    p_lvl->level_num = 0;
    int idx = 0;
    while(row != NULL)
    {
        if(str2uint(&p_lvl->level[idx].level_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert level_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_lvl->level[idx].max_score, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert max_score:%s to uint32 failed(%s).", msg_type, row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_lvl->level[idx].star_num, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert max_star:%s to uint32 failed(%s).", msg_type, row[2], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_lvl->level[idx].is_passed, row[3]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert is_passed:%s to uint8 failed(%s).", msg_type, row[3], g_sql_str);
            return ERR_SQL_ERR;
        }
        idx++;
        row = p_mysql_conn->select_next_row(true);
    }

    p_lvl->level_num = idx;

    g_pack.pack(buffer, sizeof(game_level_info_t) + idx * sizeof(level_info_t));

    return 0;
}

uint32_t add_game_level(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    db_msg_update_game_level_req_t *p_req = (db_msg_update_game_level_req_t *)p_request_body;
    if (check_val_len(msg_type, request_len, sizeof(db_msg_update_game_level_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }


    KINFO_LOG(user_id, "[add_game_level:game%u,level:%u,score:%u,star:%u,is_passed:%u].", p_req->game_id, p_req->level_id, p_req->score, p_req->star_num, p_req->is_passed);

    snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_game_%d(user_id, game_id, level_id, max_score, max_star, is_passed) VALUES(%u, %u, %u, %u, %u, %u) ON DUPLICATE KEY UPDATE max_score = %u, max_star = %u, is_passed = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->game_id, p_req->level_id, p_req->score, p_req->star_num, p_req->is_passed, p_req->score, p_req->star_num, p_req->is_passed);

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    return 0;
}

