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
#include <sys/time.h>
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
#include "puzzle.h"
#include "../role-info/role_info.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t get_puzzle_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }


    uint16_t count = 0;
    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT type_id, last_playtime, max_score, total_score, total_num FROM db_monster_%d.t_puzzle_%d WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    db_msg_puzzle_rsp_t puzzle_info = {0};

    while (row != NULL)
    {
        if(str2uint(&puzzle_info.puzzle[count].type, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert type_id:%s to uint8 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }
        if(str2uint(&puzzle_info.puzzle[count].last_playtime, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert last_playtime:%s to uint32 failed(%s).", msg_type, row[1], g_sql_str);
            return ERR_SQL_ERR;
        }
        if(str2uint(&puzzle_info.puzzle[count].max_score, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert max_score:%s to uint16 failed(%s).", msg_type, row[2], g_sql_str);
            return ERR_SQL_ERR;
        }
        if(str2uint(&puzzle_info.puzzle[count].score, row[3]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert total_score:%s to uint32 failed(%s).", msg_type, row[3], g_sql_str);
            return ERR_SQL_ERR;
        }
        if(str2uint(&puzzle_info.puzzle[count].num, row[4]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert total_num:%s to uint32 failed(%s).", msg_type, row[4], g_sql_str);
            return ERR_SQL_ERR;
        }

        ++count;
        row = p_mysql_conn->select_next_row(true);
    }
    puzzle_info.num = count;
    g_pack.pack((char *)&puzzle_info, sizeof(uint16_t) + count * sizeof(puzzle_t));
    KINFO_LOG(user_id, "get %u puzzle info.", count);

    return 0;
}

uint32_t commit_puzzle(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_puzzle_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_puzzle_req_t *p_puzzle = (db_msg_puzzle_req_t *)p_request_body;
    KINFO_LOG(user_id, "[commit puzzle]type:%u, last_playtime:%u, max_score:%u, score:%u, num:%u, coins:%u, exp:%u, happy:%u",
            p_puzzle->puzzle.type, p_puzzle->puzzle.last_playtime, p_puzzle->puzzle.max_score, p_puzzle->puzzle.score, p_puzzle->puzzle.num, p_puzzle->reward.coins, p_puzzle->reward.exp, p_puzzle->reward.happy);

    snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_puzzle_%d(user_id, type_id, last_playtime, max_score, total_score, total_num) VALUES(%u, %u, %u, %u, %u, %u) ON DUPLICATE KEY UPDATE total_score = total_score + %u,total_num = total_num + %u,",
            DB_ID(user_id), TABLE_ID(user_id), user_id, p_puzzle->puzzle.type, p_puzzle->puzzle.last_playtime, p_puzzle->puzzle.max_score, p_puzzle->puzzle.score, p_puzzle->puzzle.num, p_puzzle->puzzle.score, p_puzzle->puzzle.num);

    char buffer[100] = {0};
    if (p_puzzle->puzzle.last_playtime != 0)
    {
        sprintf(buffer, "last_playtime=%u,", p_puzzle->puzzle.last_playtime);
        strcat(g_sql_str, buffer);
    }
    if (p_puzzle->puzzle.max_score != 0)
    {
        sprintf(buffer, "max_score=%u,", p_puzzle->puzzle.max_score);
        strcat(g_sql_str, buffer);
    }
    g_sql_str[strlen(g_sql_str) - 1] = 0;

    //修改答题信息和增加答题奖励放一个事务里面
    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get msyql conn failed.");
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

    int max_score_add = 0;
    if (p_puzzle->puzzle.max_score != 0)
    {
        role_t role = {{0}};
        if (get_user_role(user_id, &role, p_mysql_conn, p_memcached) <= 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]get role info failed", msg_type);
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
        if (p_puzzle->puzzle.max_score > role.max_puzzle_score)
        {
            max_score_add = p_puzzle->puzzle.max_score - role.max_puzzle_score;
        }
    }

    db_msg_update_role_req_t *p_add_role = (db_msg_update_role_req_t *)buffer;
    p_add_role->type = DB_ADD_ROLE;
    p_add_role->count = 5;
    p_add_role->field[0].type = FIELD_COIN;
    p_add_role->field[0].value = p_puzzle->reward.coins;
    p_add_role->field[1].type = FIELD_EXP;
    p_add_role->field[1].value = p_puzzle->reward.exp;
    p_add_role->field[2].type = FIELD_LEVEL;
    p_add_role->field[2].value = p_puzzle->reward.level;
    p_add_role->field[3].type = FIELD_HAPPY;
    p_add_role->field[3].value = p_puzzle->reward.happy;
    p_add_role->field[4].type = FIELD_MAX_PUZZLE_SCORE;
    p_add_role->field[4].value = max_score_add;

    if (update_role_info(p_mysql_conn, p_memcached, user_id, p_add_role) != 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]add role info failed.", msg_type);
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

    return 0;
}
