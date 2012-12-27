/*
 * =====================================================================================
 *
 *       Filename:  bag.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月21日 13时26分45秒
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
#include "bag.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

/**
 * @brief get_bag_stuff 拉取背包中的物品,对应53410(0xD0A2)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t get_bag_stuff(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT stuff_id, stuff_num, used_num FROM db_monster_%d.t_stuff_%d WHERE user_id = %u and stuff_num > 0", DB_ID(user_id), TABLE_ID(user_id), user_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    char buffer[MAX_SEND_PKG_SIZE] = {0};
    db_msg_get_bag_rsp_t *p_bag = (db_msg_get_bag_rsp_t *)buffer;

    uint16_t count = 0;
    while (row != NULL)
    {
        if(str2uint(&p_bag->stuff[count].stuff_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert stuff_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_bag->stuff[count].stuff_num, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert stuff_num:%s to uint32 failed(%s).", msg_type, row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_bag->stuff[count].used_num, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert used_num:%s to uint32 failed(%s).", msg_type, row[2], g_sql_str);
            return ERR_SQL_ERR;
        }

        ++count;
        row = p_mysql_conn->select_next_row(true);
    }
    p_bag->num = count;

    g_pack.pack((char *)p_bag, sizeof(db_msg_get_bag_rsp_t) + count * sizeof(stuff_t));

    return 0;
}

 /**
 * @brief add_bag_stuff 在背包中增加物品,对应53411(0xD0A3)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t add_bag_stuff(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    db_msg_add_stuff_req_t *p_bag_stuff = (db_msg_add_stuff_req_t *)p_request_body;
    if ((check_val_len(msg_type, request_len, sizeof(db_msg_add_stuff_req_t) + p_bag_stuff->count * sizeof(as_msg_buy_stuff_req_t))) != 0)
    {
        return ERR_MSG_LEN;
    }

    if (0 == p_bag_stuff->count)
    {
        return 0;
    }

    //增加物品在一个事务里面
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

    for (int i = 0; i != p_bag_stuff->count; ++i)
    {
        snprintf(g_sql_str, sizeof(g_sql_str),
                "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) VALUES(%u, %u, %u) "
                "ON DUPLICATE KEY UPDATE stuff_num = stuff_num + %u",
                DB_ID(user_id), TABLE_ID(user_id), user_id, p_bag_stuff->stuff[i].stuff_id, p_bag_stuff->stuff[i].stuff_num, p_bag_stuff->stuff[i].stuff_num);
        KINFO_LOG(user_id, "buy_stuff sql:%s", g_sql_str);

        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
    }

    if (p_bag_stuff->happy != 0 || p_bag_stuff->coins != 0  || p_bag_stuff->reward_exp != 0 )
    {
        //减去用户的金币数量,增加用户的愉悦度 经验值 等级
        snprintf(g_sql_str, sizeof(g_sql_str),
                    "UPDATE db_monster_%d.t_role_%d SET coins = coins - %u, mon_happy = mon_happy + %u, mon_exp = mon_exp + %u, mon_level = mon_level + %u WHERE user_id = %u",
                    DB_ID(user_id), TABLE_ID(user_id), p_bag_stuff->coins, p_bag_stuff->happy, p_bag_stuff->reward_exp, p_bag_stuff->level_up, user_id);

        KINFO_LOG(user_id, "buy_stuff reward:%s", g_sql_str);

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

    role_t cached_role = {{0}};
    if (p_memcached->get_role(user_id, &cached_role) == 0)
    {
        cached_role.coins -= p_bag_stuff->coins;
        cached_role.monster_happy += p_bag_stuff->happy;
        cached_role.monster_exp += p_bag_stuff->reward_exp;
        cached_role.monster_level += p_bag_stuff->level_up;
        p_memcached->set_role(user_id, &cached_role);
    }


    return 0;
}

/**
 * @brief update_bag_stuff 更新背包中物品的数量,对应53412(0XD0A4)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param check_val_len(msg_type, request_len, sizeof(stuff_t
 *
 * @return
 */
uint32_t update_bag_stuff(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(stuff_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    stuff_t *p_stuff_info = (stuff_t *)p_request_body;
    if (0 == p_stuff_info->stuff_num)
    {
        snprintf(g_sql_str, sizeof(g_sql_str),
                 "DELETE FROM db_monster_%d.t_bag_stuff_%d WHERE user_id = %u AND stuff_id = %u",
                 DB_ID(user_id), TABLE_ID(user_id), user_id, p_stuff_info->stuff_id
                );
    }
    else
    {
        snprintf(g_sql_str, sizeof(g_sql_str),
                 "UPDATE db_monster_%d.t_bag_stuff_%d SET stuff_num = %u, used_num = %u WHERE user_id = %u AND stuff_id = %u",
                 DB_ID(user_id), TABLE_ID(user_id), p_stuff_info->stuff_num, p_stuff_info->used_num, user_id, p_stuff_info->stuff_id
                );
    }

    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    return 0;
}

uint32_t factory_op(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    factory_op_get_stuff_t *p_bag_stuff = (factory_op_get_stuff_t *)p_request_body;
    if ((check_val_len(msg_type, request_len, sizeof(factory_op_get_stuff_t) + p_bag_stuff->count * sizeof(as_msg_buy_stuff_req_t))) != 0)
    {
        return ERR_MSG_LEN;
    }

    if (0 == p_bag_stuff->count)
    {
        return 0;
    }

    //增加物品在一个事务里面
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

    for (int i = 0; i != p_bag_stuff->count; ++i)
    {
        snprintf(g_sql_str, sizeof(g_sql_str),
                "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) VALUES(%u, %u, %u) "
                "ON DUPLICATE KEY UPDATE stuff_num = stuff_num + %u",
                DB_ID(user_id), TABLE_ID(user_id), user_id, p_bag_stuff->stuff[i].stuff_id, p_bag_stuff->stuff[i].stuff_num, p_bag_stuff->stuff[i].stuff_num);
        KINFO_LOG(user_id, "factory op sql:%s", g_sql_str);

        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
    }

    sprintf(g_sql_str,
            "UPDATE db_monster_%d.t_role_%d SET compose_time = 0, compose_id = 0 WHERE user_id = %u",
            DB_ID(user_id), TABLE_ID(user_id), user_id);

    KINFO_LOG(user_id, "factory update compose time and id:%s", g_sql_str);

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

    role_t cached_role = {{0}};
    if (p_memcached->get_role(user_id, &cached_role) == 0)
    {
        cached_role.compose_time = 0;
        cached_role.compose_id = 0;
        p_memcached->set_role(user_id, &cached_role);
    }

    return 0;
}
