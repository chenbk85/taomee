/**
 * =====================================================================================
 *       @file  task.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  12/04/2011 06:25:44 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  henry
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
#include "task.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t get_finished_task_list(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT task_id, last_reward_time FROM db_monster_%d.t_task_%d WHERE user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id);


    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    KINFO_LOG(user_id, "sql:%s", g_sql_str);

    char buffer[4096] = {0};
    db_msg_finished_task_rsp_t *p_rsp = (db_msg_finished_task_rsp_t*)buffer;

    p_rsp->finished_task_num = 0;
    uint16_t idx = 0;
    while(row != NULL)
    {
        if(str2uint(&p_rsp->finished_task[idx].task_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert task_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_rsp->finished_task[idx].last_reward_time, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert last_reward_time:%s to uint32 failed(%s).", msg_type, row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        idx++;
        row = p_mysql_conn->select_next_row(true);
    }


    p_rsp->finished_task_num = idx;
    g_pack.pack(buffer, sizeof(db_msg_finished_task_rsp_t) + idx * sizeof(finished_task_t));

    return 0;
}

uint32_t finished_a_task(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{

    db_msg_task_reward_t *p_req = (db_msg_task_reward_t*)p_request_body;
    if (check_val_len(msg_type, request_len, sizeof(db_msg_task_reward_t) + p_req->reward_num * sizeof(task_reward_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    KINFO_LOG(user_id, "[finish a task: task_id:%u level_up:%u reward_time:%u reward_num:%u].", p_req->task_id, p_req->level_up,  p_req->reward_time, p_req->reward_num);


    sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_task_%d(user_id, task_id, last_reward_time) VALUES(%u, %u, %u) ON DUPLICATE KEY UPDATE last_reward_time = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->task_id, p_req->reward_time, p_req->reward_time);

    KINFO_LOG(user_id, "finish a task:%s", g_sql_str);
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


    role_t cached_role = {{0}};
    int ret_mem = p_memcached->get_role(user_id, &cached_role);

    for(uint8_t idx = 0; idx < p_req->reward_num; idx++)
    {
        if(p_req->reward[idx].reward_id >= ITEM_ID_BEGIN && p_req->reward[idx].reward_id <= ITEM_ID_END)
        {
            sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) values(%u, %u, 1) ON DUPLICATE KEY UPDATE stuff_num = stuff_num + %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->reward[idx].reward_id, p_req->reward[idx].reward_num);

        }
        else if(p_req->reward[idx].reward_id == TASK_REWARD_COIN)
        {
            sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set coins = coins + %u where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), p_req->reward[idx].reward_num, user_id);
            cached_role.coins += p_req->reward[idx].reward_num;
        }
        else if(p_req->reward[idx].reward_id == TASK_REWARD_EXP)
        {//奖励经验值
            sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set mon_exp = mon_exp + %u, mon_level = mon_level + %u where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), p_req->reward[idx].reward_num, p_req->level_up, user_id);
            cached_role.monster_exp += p_req->reward[idx].reward_num;
            cached_role.monster_level += p_req->level_up;

        }

        KINFO_LOG(user_id, "task_reward:%s", g_sql_str);
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

    if(ret_mem == 0)
    {
        p_memcached->set_role(user_id, &cached_role);
    }
    g_pack.pack(p_request_body, request_len);

    return 0;
}

