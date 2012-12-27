/*
 * =====================================================================================
 *
 *       Filename:  room.cpp
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
extern "C"
{
#include <libtaomee/log.h>
}
#include "../../../common/pack/c_pack.h"
#include "../../../common/message.h"
#include "../../../common/data_structure.h"
#include "../../../common/constant.h"
#include "../util.h"
#include "room.h"
#include "../db_constant.h"

extern c_pack g_pack;
static char g_sql_str[8192] = {0};


/**
 * @brief add_default_room 添加默认房间,需要在一个事务里面
 *
 * @param user_id
 * @param room_id
 * @param room_buf
 * @param door_id
 * @param wallpaper_id
 * @param floor_id
 * @param window_id
 *
 * @return
 */
int add_default_room(i_mysql_iface *p_mysql_conn, uint32_t user_id, uint32_t room_id, char *room_buf, uint32_t door_id, uint32_t wallpaper_id, uint32_t floor_id, uint32_t window_id)
{
    snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_room_%d(user_id, room_id, buf) VALUES(%u, %u, \'%s\')", DB_ID(user_id), TABLE_ID(user_id), user_id, room_id, room_buf);
    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed");
        return -1;
    }
    if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
    {
        KCRIT_LOG(user_id, "add room sql exec failed(%s).", mysql_error(p_conn));
        return -1;
    }

    //增加默认的物品
    snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num, used_num) VALUES(%u, %u, 1, 1), (%u, %u, 1, 1), (%u, %u, 1, 1), (%u, %u, 1, 1)", DB_ID(user_id), TABLE_ID(user_id), user_id, door_id, user_id, wallpaper_id, user_id, floor_id, user_id, window_id);
    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "add default room stuff failed(%s).", p_mysql_conn->get_last_errstr());
        return -1;
    }

    return 0;
}

/**
 * @brief get_room_num 拉取用户的房间数量: 对应53413(0XC0A5)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t get_room_num(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    KINFO_LOG(user_id, "[get room num].");

    uint16_t count = 0;

    snprintf(g_sql_str, sizeof(g_sql_str), "SELEC room_id FROM db_monster_%d.t_room_%d WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    char buffer[MAX_SEND_PKG_SIZE] = {0};
    as_msg_room_num_rsp_t *p_room = (as_msg_room_num_rsp_t *)buffer;

    while (row != NULL)
    {
        if(str2uint(&p_room->room_id[count], row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert room_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        ++count;
        row = p_mysql_conn->select_next_row(true);
    }
    p_room->room_num = count;

    g_pack.pack((char *)p_room, sizeof(as_msg_room_num_rsp_t) + count * sizeof(uint32_t));

    return 0;
}

/**
 * @brief get_room_info 拉取单个房间信息: 53414(0XD0A6)
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t get_room_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_get_room_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_get_room_req_t *p_req = (db_msg_get_room_req_t *)p_request_body;

    uint32_t len = 0;
    char room_buf[MAX_ROOM_STUFF * 32 + sizeof(uint16_t)] = {0};
    if (p_memcached->get_room(user_id, p_req->room_id, room_buf, &len) == 0)
    {
        KINFO_LOG(user_id, "[get room info]room_id:%u from cache.", p_req->room_id);
        g_pack.pack(room_buf, len);
        return 0;
    }

    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT buf, bit_length(buf) FROM db_monster_%d.t_room_%d WHERE user_id = %u AND room_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->room_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }
    else if (0 == result_count)
    {
        KCRIT_LOG(user_id, "no record, room_id:%u.", p_req->room_id);
        return ERR_SQL_NO_RECORD;
    }

    if(str2uint(&len, row[1]) != 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]convert buf len:%s to uint16 failed.", msg_type, row[1]);
        return ERR_SQL_ERR;
    }
    g_pack.pack(row[0], len / 8);

    return 0;
}

/**
 * @brief add_room 新增一个房间: 53415(0XD0A7)
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t add_room(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_add_room_req_t) + sizeof(user_room_buf_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_add_room_req_t *p_room = (db_msg_add_room_req_t *)p_request_body;

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    char buf[2 * sizeof(user_room_buf_t) + 1] = {0};
    mysql_real_escape_string(p_conn, buf, p_room->room_buf, sizeof(user_room_buf_t));

    KINFO_LOG(user_id, "[add room]room_id:%u.", p_room->room_id);

    //增加房屋与增加房屋默认的物品在一个事务里面
    if (mysql_autocommit(p_conn, false) != 0)
    {
        KCRIT_LOG(user_id, "close mysql_autocommit failed.");
        return ERR_SQL_ERR;
    }

    if (add_default_room(p_mysql_conn, user_id, p_room->room_id, buf, p_room->door_id, p_room->wallpaper_id, p_room->floor_id, p_room->windows_id) != 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed.", msg_type);
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

    //将房间写入memcache
    p_memcached->set_room(user_id, p_room->room_id, p_room->room_buf);

    role_t cached_role = {{0}};
    if (p_memcached->get_role(user_id, &cached_role) == 0)
    {
        cached_role.room_num += 1;
        p_memcached->set_role(user_id, &cached_role);
    }


    return 0;
}

/**
 * @brief update_room 更新房间room_buf: 53417(0XD0A9)
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t update_room(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    uint16_t num = *(uint16_t *)p_request_body;
    uint16_t stuff_len = num * (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t));
    uint32_t room_id = *(uint32_t *)(p_request_body + sizeof(uint16_t) + stuff_len);
    uint16_t room_buf_len = *(uint16_t *)(p_request_body + sizeof(uint16_t) + stuff_len + sizeof(uint32_t));
    if (check_val_len(msg_type, request_len, 2 * sizeof(uint16_t) + sizeof(uint32_t) + stuff_len + room_buf_len) != 0)
    {
	    KCRIT_LOG(user_id, "msg:%u len:%u err.", msg_type, request_len);
        return ERR_MSG_LEN;
    }

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    char *p_room_buf = p_request_body + 2 * sizeof(uint16_t) + sizeof(uint32_t) + stuff_len;
    char buf[MAX_ROOM_STUFF * 32 * 2 + 1] = {0};
    mysql_real_escape_string(p_conn, buf, p_room_buf, room_buf_len);

    snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_room_%d SET buf = \"%s\" WHERE user_id = %u AND room_id = %u", DB_ID(user_id), TABLE_ID(user_id), buf, user_id, room_id);

    //更新房屋与更改物品在一个事务里面
    if (mysql_autocommit(p_conn, false) != 0)
    {
        KCRIT_LOG(user_id, "close mysql_autocommit failed.");
        return ERR_SQL_ERR;
    }
    if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
    {
        KCRIT_LOG(user_id, "update room buf sql exec failed(%s).", mysql_error(p_conn));
        return ERR_SQL_ERR;
    }

    stuff_change_t *p_stuff_change = (stuff_change_t *)p_request_body;
    uint32_t stuff_id = 0;
    uint16_t stuff_num = 0;
    uint8_t stuff_flow = 0;

    for (int i = 0; i != (int)p_stuff_change->num; ++i)
    {
        stuff_id = p_stuff_change->stuff_flow[i].stuff_id;
        stuff_num = p_stuff_change->stuff_flow[i].stuff_num;
        stuff_flow = p_stuff_change->stuff_flow[i].stuff_flow;

        //物品的使用次数加1
        if (flow_bag_to_room == stuff_flow)
        {
            snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_stuff_%d SET used_num = used_num + %u WHERE user_id = %u AND stuff_id = %u;", DB_ID(user_id), TABLE_ID(user_id), stuff_num, user_id, stuff_id);
            KINFO_LOG(user_id, "stuff:%u add used", stuff_id);
        }
        else
        {
            snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_stuff_%d SET used_num = used_num - %u WHERE user_id = %u AND stuff_id = %u AND used_num >= %u;", DB_ID(user_id), TABLE_ID(user_id), stuff_num, user_id, stuff_id, stuff_num);
            KINFO_LOG(user_id, "stuff:%u desc used", stuff_id);
        }

        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "update stuff sql exec failed(%s).", p_mysql_conn->get_last_errstr());
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

    p_memcached->set_room(user_id, room_id, p_room_buf);

    return 0;
}
