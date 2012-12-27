/*
 * =====================================================================================
 *
 *       Filename:  badge.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年02月20日 16时45分12秒
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
#include <benchapi.h>
#include "../../../common/pack/c_pack.h"
#include "../../../common/message.h"
#include "../../../common/data_structure.h"
#include "../../../common/constant.h"
#include "../util.h"
#include "../db_constant.h"
#include "badge.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t get_all_badge(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    sprintf(g_sql_str, "SELECT badge_id, status, progress FROM db_monster_%d.t_badge_%d WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    char buffer[8192] = {0};
    all_badge_info_t *p_badge = (all_badge_info_t *)(buffer);
    p_badge->badge_num = 0;
    while (row != NULL)
    {
        if(str2uint(&p_badge->badge[p_badge->badge_num].badge_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert badge_id:%s to uint32 failed(%s).", row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_badge->badge[p_badge->badge_num].badge_status, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "convert badge_status:%s to uint8 failed(%s).", row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_badge->badge[p_badge->badge_num].badge_progress, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "convert badge_num:%s to uint8 failed(%s).", row[2], g_sql_str);
            return ERR_SQL_ERR;
        }

        ++p_badge->badge_num;

        row = p_mysql_conn->select_next_row(true);
    }

    KINFO_LOG(user_id, "get_all_badge:%s", g_sql_str);

    g_pack.pack(buffer, sizeof(all_badge_info_t) + p_badge->badge_num * sizeof(badge_info_t));

    return 0;
}

uint32_t update_a_badge(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(badge_info_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    badge_info_t *p_body = (badge_info_t*)p_request_body;

    sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_badge_%d(user_id, badge_id, status, progress) values(%u, %u, %u, %u) ON DUPLICATE KEY UPDATE status = %u, progress = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_body->badge_id, p_body->badge_status, p_body->badge_progress, p_body->badge_status, p_body->badge_progress);

    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KERROR_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    KINFO_LOG(user_id, "update_badge:%s", g_sql_str);

    g_pack.pack((char*)p_body, sizeof(badge_info_t));

    return 0;
}
