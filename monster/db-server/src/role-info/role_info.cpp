/*
 * =====================================================================================
 *
 *       Filename:  role_info.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月21日 13时26分45秒
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
#include <assert.h>
extern "C"
{
#include <libtaomee/log.h>
}
#include "../../../common/pack/c_pack.h"
#include "../../../common/message.h"
#include "../../../common/data_structure.h"
#include "../util.h"
#include "role_info.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};


extern int add_default_room(i_mysql_iface *p_mysql_conn, uint32_t user_id, uint32_t room_id, char *room_buf, uint32_t door_id, uint32_t wallpaper_id, uint32_t floor_id, uint32_t window_id);

/**
 * @brief add_new_role 添加一个新的角色 对应53408(0xD0A0)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t add_new_role(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    db_msg_add_role_req_t *p_role = (db_msg_add_role_req_t *)p_request_body;

    if (check_val_len(msg_type, request_len, sizeof(db_msg_add_role_req_t) + p_role->stuff_count * sizeof(uint32_t)) != 0)
    {
        return ERR_MSG_LEN;
    }


    KINFO_LOG(user_id, "[ADD ROLE]gender:%u, birthday:%u, monster_id:%u, main color:%u, ex color:%u, eye color:%u, time:%u",
            p_role->gender, p_role->birthday, p_role->monster_id, p_role->monster_main_color, p_role->monster_ex_color, p_role->monster_eye_color, p_role->register_time);

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    int initial_friend_num = 0;
    if(p_role->default_friend != 0)
    {
        initial_friend_num++;

    }

    if(p_role->invitor_id != 0 && p_role->default_friend != p_role->invitor_id)
    {
        initial_friend_num++;
    }

    char name_esp[2 * sizeof(p_role->name) + 1] = {0};
    char name[sizeof(p_role->name) + 1] = {0};
    memcpy(name, p_role->name, sizeof(p_role->name));
    mysql_real_escape_string(p_conn, name_esp, name, strlen(name));

    char mon_name_esp[2 * sizeof(p_role->monster_name) + 1] = {0};
    char mon_name[sizeof(p_role->monster_name) + 1] = {0};
    memcpy(mon_name, p_role->monster_name, sizeof(p_role->monster_name));
    mysql_real_escape_string(p_conn, mon_name_esp, mon_name, strlen(mon_name));

    sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_role_%d(user_id, name, gender, country_id, "
            "birthday, register_time, coins, last_login_time, mon_id, mon_name, mon_main_color, mon_exp_color, mon_eye_color, "
            "mon_exp, mon_level, mon_health, mon_happy, invitor_id, unapproved_msg_num, friend_num) VALUES(%u, \'%s\', %u, %u, %u, "
            "%u, %u, %u, %u, \'%s\', %u, %u, %u, %u, %u, %u, %u, %u, 1, %d);",
            DB_ID(user_id), TABLE_ID(user_id), user_id, name_esp, p_role->gender, p_role->country_id,
            p_role->birthday, p_role->register_time, p_role->coins, p_role->last_login_time, p_role->monster_id,
            mon_name_esp, p_role->monster_main_color, p_role->monster_ex_color, p_role->monster_eye_color, p_role->monster_exp,
            p_role->monster_level, p_role->monster_health, p_role->monster_happy, p_role->invitor_id, initial_friend_num);

    //增加用户信息与增加默认房屋和物品在一个事务里面
    if (mysql_autocommit(p_conn, false) != 0)
    {
        KCRIT_LOG(user_id, "close mysql_autocommit failed.");
        return ERR_SQL_ERR;
    }

    if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }

    char buf[2 * sizeof(user_room_buf_t) + 1] = {0};
    mysql_real_escape_string(p_conn, buf, (char *)&p_role->room_buf, sizeof(user_room_buf_t));

    if (add_default_room(p_mysql_conn, user_id, p_role->room_id, buf, p_role->door_id, p_role->wallpaper_id, p_role->floor_id, p_role->window_id) != 0)
    {
        KCRIT_LOG(user_id, "add user default failed.");
        mysql_rollback(p_conn);
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }

    //增加种子
    if(p_role->seed1 != 0)
    {
        sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) VALUES(%u, %u, 1)",
                DB_ID(user_id), TABLE_ID(user_id), user_id, p_role->seed1);

        if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
    }

    if(p_role->seed2 != 0)
    {
        sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) VALUES(%u, %u, 1) ON DUPLICATE KEY UPDATE stuff_num = stuff_num + 1",
                DB_ID(user_id), TABLE_ID(user_id), user_id, p_role->seed2);

        if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
    }

    if(p_role->seed3 != 0)
    {
        sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) VALUES(%u, %u, 1) ON DUPLICATE KEY UPDATE stuff_num = stuff_num + 1",
                DB_ID(user_id), TABLE_ID(user_id), user_id, p_role->seed3);

        if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
    }
    if (p_role->default_friend != 0)
    {
        sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_friend_%d(user_id, friend_id, create_time, type, is_best_friend) VALUES(%u, %u, %zu, %u, 1)",
                DB_ID(user_id), TABLE_ID(user_id), user_id, p_role->default_friend, time(NULL), FRIEND_DEFAULT);

        if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }

    }

    if (p_role->invitor_id!= 0 && p_role->invitor_id != p_role->default_friend)
    {
        sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_friend_%d(user_id, friend_id, create_time, type, is_best_friend) VALUES(%u, %u, %zu, %u, 1)",
                DB_ID(user_id), TABLE_ID(user_id), user_id, p_role->invitor_id, time(NULL), FRIEND_DEFAULT);

        if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }

    }
    //增加默认赠送的物品
    if (p_role->stuff_count != 0)
    {
        memset(g_sql_str, 0, sizeof(g_sql_str));
        sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num, used_num) VALUES", DB_ID(user_id), TABLE_ID(user_id));
        for (int i = 0; i != p_role->stuff_count; ++i)
        {
            char buffer[100] = {0};
            sprintf(buffer, "(%u, %u, %u, %u),", user_id, p_role->stuff_id[i], 1, 0);
            strcat(g_sql_str, buffer);
        }
        g_sql_str[strlen(g_sql_str) - 1] = 0;
        if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, g_sql_str);
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
    }

    //增加默认的一条留言
    sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_pinboard_%d(user_id, peer_id, message, icon, color, status, create_time) values(%u, 12345, '欢迎来到怪兽国,赶快点击麦咭的头像,去麦咭家参观一下吧', 2, 1,  1, %u)", DB_ID(user_id), TABLE_ID(user_id), user_id, (uint32_t)time(NULL));
    if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql:%s exec failed(%s).", msg_type, g_sql_str, p_mysql_conn->get_last_errstr());
        mysql_rollback(p_conn);
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }


    //增加种植园中的三个坑
    sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_plant_%d(user_id, hole_id) values(%u, 1), (%u, 2), (%u, 3);", DB_ID(user_id), TABLE_ID(user_id), user_id, user_id, user_id);
    if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql:%s exec failed(%s).", msg_type, g_sql_str, p_mysql_conn->get_last_errstr());
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

/**
 * @brief get_user_role 获得用户role表的信息
 *
 * @return -1:失败, 1:成功获得用户的信息, 0:用户没有注册
 */
int get_user_role(uint32_t user_id, role_t *p_role, i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, message_num_t *p_msg_num)
{
    //memcache里找到，直接返回,否则从数据库里查找
    if (p_memcached->get_role(user_id, p_role) == 0)
    {
        KINFO_LOG(user_id, "user from cache,friend num:%u,room_num:%u,pet_num:%u,happy:%u,health:%u, coins:%u.", p_role->friend_num, p_role->room_num, p_role->pet_num, p_role->monster_happy, p_role->monster_health, p_role->coins);
        return 1;
    }

    sprintf(g_sql_str, "SELECT name, gender, country_id, vip, birthday, register_time, coins, last_login_time, mon_id, "
            "mon_name, mon_main_color, mon_exp_color, mon_eye_color, mon_exp, mon_level, mon_health, mon_happy, thumb, visits, recent_badge, last_visit_plantation_time, "
            "mood, fav_color, fav_pet, fav_fruit, personal_sign, max_puzzle_score, mon_eye_color, flag1, last_logout_time, drawing_id, compose_time, compose_id, "
            "npc_score_daytime, last_paper_reward, last_paper_read, npc_score, online_time, offline_time, guide_flag, limit_reward, invitor_id, approved_msg_num, unapproved_msg_num, room_num, friend_num, last_show_id FROM db_monster_%d.t_role_%d WHERE user_id = %u",
            DB_ID(user_id), TABLE_ID(user_id), user_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (0 == result_count)          //没有记录，用户未注册
    {
        return 0;
    }
    else if(result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return -1;
    }

    strncpy(p_role->name, row[0], sizeof(p_role->name));

    if(str2uint(&p_role->gender, row[1]) != 0)
    {
        KCRIT_LOG(user_id, "convert gender:%s to uint8 failed(%s).", row[1], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->country_id, row[2]) != 0)
    {
        KCRIT_LOG(user_id, "convert country_id:%s to uint8 failed(%s).", row[2], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->user_type, row[3]) != 0)
    {
        KCRIT_LOG(user_id, "convert vip:%s to uint8 failed(%s).", row[3], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->birthday, row[4]) != 0)
    {
        KCRIT_LOG(user_id, "convert birthday:%s to uint32 failed(%s).", row[4], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->register_time, row[5]) != 0)
    {
        KCRIT_LOG(user_id, "convert register_time:%s to uint32 failed(%s).", row[5], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->coins, row[6]) != 0)
    {
        KCRIT_LOG(user_id, "convert coins:%s to uint32 failed(%s).", row[6], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->last_login_time, row[7]) != 0)
    {
        KCRIT_LOG(user_id, "convert last_login_time:%s to uint32 failed(%s).", row[7], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->monster_id, row[8]) != 0)
    {
        KCRIT_LOG(user_id, "convert monster_id:%s to uint32 failed(%s).", row[8], g_sql_str);
        return -1;
    }

    strncpy(p_role->monster_name, row[9], sizeof(p_role->monster_name));

    if(str2uint(&p_role->monster_main_color, row[10]) != 0)
    {
        KCRIT_LOG(user_id, "convert monster_main_color:%s to uint8 failed(%s).", row[10], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->monster_ex_color, row[11]) != 0)
    {
        KCRIT_LOG(user_id, "convert monster_ex_color:%s to uint8 failed(%s).", row[11], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->monster_eye_color, row[12]) != 0)
    {
        KCRIT_LOG(user_id, "convert monster_eye_color:%s to uint8 failed(%s).", row[12], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->monster_exp, row[13]) != 0)
    {
        KCRIT_LOG(user_id, "convert monster_exp:%s to uint32 failed(%s).", row[13], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->monster_level, row[14]) != 0)
    {
        KCRIT_LOG(user_id, "convert monster_level:%s to uint8 failed(%s).", row[14], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->monster_health, row[15]) != 0)
    {
        KCRIT_LOG(user_id, "convert monster_health:%s to uint32 failed(%s).", row[15], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->monster_happy, row[16]) != 0)
    {
        KCRIT_LOG(user_id, "convert monster_happy:%s to uint32 failed(%s).", row[16], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->thumb, row[17]) != 0)
    {
        KCRIT_LOG(user_id, "convert thumb:%s to uint32 failed(%s).", row[17], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->visits, row[18]) != 0)
    {
        KCRIT_LOG(user_id, "convert visits:%s to uint32 failed(%s).", row[18], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->recent_unread_badge, row[19]) != 0)
    {
        KCRIT_LOG(user_id, "convert recent_badge:%s to uint32 failed(%s).", row[19], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->last_visit_plantation_time, row[20]) != 0)
    {
        KCRIT_LOG(user_id, "convert last_visit_plantation_time:%s to uint32 failed(%s).", row[20], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->mood, row[21]) != 0)
    {
        KCRIT_LOG(user_id, "convert mood:%s to uint8 failed(%s).", row[21], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->fav_color, row[22]) != 0)
    {
        KCRIT_LOG(user_id, "convert fav_color:%s to uint8 failed(%s).", row[22], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->fav_pet, row[23]) != 0)
    {
        KCRIT_LOG(user_id, "convert fav_pet:%s to uint8 failed(%s).", row[23], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->fav_fruit, row[24]) != 0)
    {
        KCRIT_LOG(user_id, "convert fav_fruit:%s to uint8 failed(%s).", row[24], g_sql_str);
        return -1;
    }

    strcpy(p_role->personal_sign, row[25]);

    if(str2uint(&p_role->max_puzzle_score, row[26]) != 0)
    {
        KCRIT_LOG(user_id, "convert max_puzzle_score:%s to uint8 failed(%s).", row[26], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->monster_eye_color, row[27]) != 0)
    {
        KCRIT_LOG(user_id, "convert mon_eye_color:%s to uint8 failed(%s).", row[27], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->flag1, row[28]) != 0)
    {
        KCRIT_LOG(user_id, "convert flag1:%s to uint8 failed(%s).", row[28], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->last_logout_time, row[29]) != 0)
    {
        KCRIT_LOG(user_id, "convert last logout time:%s to uint32 failed(%s).", row[29], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->compose_id, row[30]) != 0)
    {
        KCRIT_LOG(user_id, "convert compose id:%s to uint32 failed(%s).", row[30], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->compose_time, row[31]) != 0)
    {
        KCRIT_LOG(user_id, "convert compose time:%s to uint32 failed(%s).", row[31], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->map_id, row[32]) != 0)
    {
        KCRIT_LOG(user_id, "convert compose id:%s to uint32 failed(%s).", row[32], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->npc_score_daytime, row[33]) != 0)
    {
        KCRIT_LOG(user_id, "convert npc_score_daytime:%s to uint32 failed(%s).", row[33], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->last_paper_reward, row[34]) != 0)
    {
        KCRIT_LOG(user_id, "convert last_paper_reward:%s to uint16 failed(%s).", row[34], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->last_paper_read, row[35]) != 0)
    {
        KCRIT_LOG(user_id, "convert last_paper_read:%s to uint16 failed(%s).", row[35], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->npc_score, row[36]) != 0)
    {
        KCRIT_LOG(user_id, "convert last_paper_read:%s to uint16 failed(%s).", row[35], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->online_time, row[37]) != 0)
    {
        KCRIT_LOG(user_id, "convert online time:%s to uint32 failed(%s).", row[37], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->offline_time, row[38]) != 0)
    {
        KCRIT_LOG(user_id, "convert offline time:%s to uint32 failed(%s).", row[38], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->guide_flag, row[39]) != 0)
    {
        KCRIT_LOG(user_id, "convert guide_flag:%s to uint32 failed(%s).", row[39], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->limit_reward, row[40]) != 0)
    {
        KCRIT_LOG(user_id, "convert limit-reward:%s to uint32 failed(%s).", row[40], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->invitor_id, row[41]) != 0)
    {
        KCRIT_LOG(user_id, "convert invitor_id:%s to uint32 failed(%s).", row[41], g_sql_str);
        return -1;
    }


        if (str2uint(&p_role->approved_message_num, row[42]) != 0)
        {
            KCRIT_LOG(user_id, "convert approved message num:%s to uint16 failed(%s).", row[42], g_sql_str);
            return -1;
        }

        else if (str2uint(&p_role->unapproved_message_num, row[43]) != 0)
        {
            KCRIT_LOG(user_id, "convert unapproved message num:%s to uint16 failed(%s).", row[43], g_sql_str);
            return -1;
        }


   // if(p_msg_num)
   // {
   //     if (str2uint(&p_msg_num->approved_message_num, row[42]) != 0)
   //     {
   //         KCRIT_LOG(user_id, "convert approved message num:%s to uint16 failed(%s).", row[42], g_sql_str);
   //         return -1;
   //     }

   //     else if (str2uint(&p_msg_num->unapproved_message_num, row[43]) != 0)
   //     {
   //         KCRIT_LOG(user_id, "convert unapproved message num:%s to uint16 failed(%s).", row[43], g_sql_str);
   //         return -1;
   //     }
   // }

    if(str2uint(&p_role->room_num, row[44]) != 0)
    {
            KCRIT_LOG(user_id, "convert room num:%s to uint8 failed(%s).", row[44], g_sql_str);
            return -1;
    }

    if(str2uint(&p_role->friend_num, row[45]) != 0)
    {
        KCRIT_LOG(user_id, "convert friend num:%s to uint8 failed(%s).", row[45], g_sql_str);
        return -1;
    }

    if(str2uint(&p_role->last_show_id, row[46]) != 0)
    {
        KCRIT_LOG(user_id, "convert last_show_id:%s to uint32 failed(%s).", row[46], g_sql_str);
        return -1;
    }


    //用户的好友数量
   // sprintf(g_sql_str, "SELECT count(*) FROM db_monster_%u.t_friend_%u WHERE user_id = %u AND type = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, FRIEND_DEFAULT);
   // result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
   // if(result_count <= 0)
   // {
   //     KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
   //     return -1;
   // }

   // if(str2uint(&p_role->friend_num, row[0]) != 0)
   // {
   //     KCRIT_LOG(user_id, "convert friend num:%s to uint8 failed(%s).", row[0], g_sql_str);
   //     return -1;
   // }

//    //获得用户的房间数量
//    sprintf(g_sql_str, "SELECT count(*) FROM db_monster_%u.t_room_%u WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);
//    result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
//    if(result_count <= 0)
//    {
//        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
//        return -1;
//    }
//
//    if(str2uint(&p_role->room_num, row[0]) != 0)
//    {
//        KCRIT_LOG(user_id, "convert room num:%s to uint8 failed(%s).", row[0], g_sql_str);
//        return -1;
//    }
//
    //用户精灵种类数量
    sprintf(g_sql_str, "SELECT count(*) FROM db_monster_%u.t_pet_%u WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);
    result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if(result_count <= 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return -1;
    }

    if(str2uint(&p_role->pet_num, row[0]) != 0)
    {
        KCRIT_LOG(user_id, "convert pet num:%s to uint8 failed(%s).", row[0], g_sql_str);
        return -1;
    }
//    KINFO_LOG(user_id, "user from db,friend num:%u,room_num:%u,pet_num:%u,happy:%u,health:%u, coins:%u.", p_role->friend_num, p_role->room_num, p_role->pet_num, p_role->monster_happy, p_role->monster_health, p_role->coins);

    //将数据库获得的role信息写入memcache
    p_memcached->set_role(user_id, p_role);

    return 1;
}


/**
 * @brief get_follow_pet 获得用户跟随的精灵
 *
 * @return -1:失败, 0:成功
 */
static int get_following_pet(uint32_t user_id, following_pet_t *p_pet, i_mysql_iface *p_mysql_conn)
{
    sprintf(g_sql_str, "SELECT pet_id, follow_num FROM db_monster_%d.t_pet_%d WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return -1;
    }

    p_pet->count = 0;
    while (row != NULL)
    {
        uint32_t pet_id = 0;
        if(str2uint(&pet_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert pet_id:%s to uint32 failed(%s).", row[0], g_sql_str);
            return -1;
        }
        uint16_t follow_num = 0;
        if(str2uint(&follow_num, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "convert follow_num:%s to uint8 failed(%s).", row[2], g_sql_str);
            return -1;
        }

        if (follow_num != 0)
        {
            for (int i = 0; i != follow_num; ++i)
            {
                p_pet->pet_id[p_pet->count] = pet_id;
                ++p_pet->count;
            }
        }
        row = p_mysql_conn->select_next_row(true);
    }

    return 0;
}

/**
 * @brief get_pet 获得用户跟随的精灵
 *
 * @return -1:失败, 0:成功
 */
static int get_pet(uint32_t user_id, pet_t *p_pet, i_mysql_iface *p_mysql_conn)
{
    sprintf(g_sql_str, "SELECT pet_id, total_num, follow_num FROM db_monster_%d.t_pet_%d WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return -1;
    }

    p_pet->count = 0;
    while (row != NULL)
    {
        if(str2uint(&p_pet->pet[p_pet->count].pet_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert pet_id:%s to uint32 failed(%s).", row[0], g_sql_str);
            return -1;
        }
        if(str2uint(&p_pet->pet[p_pet->count].total_num, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "convert total_num:%s to uint32 failed(%s).", row[1], g_sql_str);
            return -1;
        }
        if(str2uint(&p_pet->pet[p_pet->count].follow_num, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "convert follow_num:%s to uint8 failed(%s).", row[2], g_sql_str);
            return -1;
        }

        ++p_pet->count;
        row = p_mysql_conn->select_next_row(true);
    }

    return 0;
}

/**
 * @brief get_user_plant 获得用户种植园的植物
 *
 * @return -1:失败, 0:成功
 */
static int get_user_plant(uint32_t user_id, all_hole_info_t *p_all_hole, i_mysql_iface *p_mysql_conn)
{
    sprintf(g_sql_str, "SELECT hole_id, plant_id, color, last_growth_time, growth, last_extra_growth_time, last_growth_value, maintain, next_maintain_time, last_reward_id,maintain_count FROM db_monster_%d.t_plant_%d WHERE user_id = %u order by hole_id",
                       DB_ID(user_id), TABLE_ID(user_id), user_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return -1;
    }

    p_all_hole->hole_count = 0;
    int idx = 0;
    while (row != NULL)
    {
        if(str2uint(&p_all_hole->hole[idx].hole_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert hole_id:%s to uint8 failed(%s).", row[0], g_sql_str);
            return -1;
        }

        if(str2uint(&p_all_hole->hole[idx].plant_id, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "convert plant_id:%s to uint32 failed(%s).", row[1], g_sql_str);
            return -1;
        }

        if(str2uint(&p_all_hole->hole[idx].color, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "convert color:%s to uint16 failed(%s).", row[2], g_sql_str);
            return -1;
        }

        if(str2uint(&p_all_hole->hole[idx].last_growth_time, row[3]) != 0)
        {
            KCRIT_LOG(user_id, "convert last_growth_time:%s to uint32 failed(%s).", row[3], g_sql_str);
            return -1;
        }


        if(str2uint(&p_all_hole->hole[idx].growth, row[4]) != 0)
        {
            KCRIT_LOG(user_id, "convert growth:%s to uint16 failed(%s).", row[4], g_sql_str);
            return -1;
        }

        if(str2uint(&p_all_hole->hole[idx].last_add_extra_growth_time, row[5]) != 0)
        {
            KCRIT_LOG(user_id, "convert last extra growth time:%s to uint16 failed(%s).", row[5], g_sql_str);
            return -1;
        }

        if(str2uint(&p_all_hole->hole[idx].last_growth_value, row[6]) != 0)
        {
            KCRIT_LOG(user_id, "convert last growth value:%s to uint16 failed(%s).", row[6], g_sql_str);
            return -1;
        }

        if(str2uint(&p_all_hole->hole[idx].maintain, row[7]) != 0)
        {
            KCRIT_LOG(user_id, "convert maintain:%s to uint8 failed(%s).", row[7], g_sql_str);
            return -1;
        }
        if(str2uint(&p_all_hole->hole[idx].next_maintain_time, row[8]) != 0)
        {
            KCRIT_LOG(user_id, "convert next_maintain_time:%s to uint32 failed(%s).", row[8], g_sql_str);
            return -1;
        }

        if(str2uint(&p_all_hole->hole[idx].last_reward_id, row[9]) != 0)
        {
            KCRIT_LOG(user_id, "convert last_reward_id:%s to uint32 failed(%s).", row[9], g_sql_str);
            return -1;
        }

        if(str2uint(&p_all_hole->hole[idx].maintain_count, row[10]) != 0)
        {
            KCRIT_LOG(user_id, "convert maintain_count:%s to uint32 failed(%s).", row[10], g_sql_str);
            return -1;
        }
        ++idx;
        row = p_mysql_conn->select_next_row(true);
    }

    p_all_hole->hole_count = idx;
    p_all_hole->plant_count = 0;
    return 0;
}

/**
 * @brief query_role_info 查询用户的role信息,对应50439(0xD0A1)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t query_role_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    char buffer[4096] = {0};
    db_msg_get_role_rsp_t *p_role_rsp = (db_msg_get_role_rsp_t *)buffer;
    int buf_len = sizeof(db_msg_get_role_rsp_t);

    message_num_t message_num = {0};
    //获得用户的role信息
    int ret = get_user_role(user_id, &p_role_rsp->register_rsp.role, p_mysql_conn, p_memcached, &message_num);
    if (ret < 0)
    {
        KCRIT_LOG(user_id, "get user role info failed");
        return ERR_SQL_ERR;
    }
    else if (0 == ret) //用户没有注册
    {
        g_pack.pack((uint8_t)0);
        return 0;
    }
    else
    {
        p_role_rsp->is_register = 1;
    }

   // p_role_rsp->register_rsp.approved_message_num = message_num.approved_message_num;
   // p_role_rsp->register_rsp.unapproved_message_num = message_num.unapproved_message_num;

    //获得用户的好友申请数量
    sprintf(g_sql_str, "SELECT count(*) FROM db_monster_%u.t_friend_%u WHERE user_id = %u AND type = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, FRIEND_PENDING);
    MYSQL_ROW row = NULL;
    ret = p_mysql_conn->select_first_row(&row, g_sql_str);
    if(ret <= 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    if(str2uint(&p_role_rsp->register_rsp.pending_req_num, row[0]) != 0)
    {
        KCRIT_LOG(user_id, "convert pending req num:%s to uint8 failed(%s).", row[0], g_sql_str);
        return ERR_SQL_ERR;
    }

    pet_t *p_pet = (pet_t *)(p_role_rsp->register_rsp.buf);
    if (get_pet(user_id, p_pet, p_mysql_conn) < 0)
    {
        KCRIT_LOG(user_id, "get user pet failed");
        return ERR_SQL_ERR;
    }

    int pet_len = sizeof(pet_t) + p_pet->count * sizeof(pet_info_t);
    buf_len += pet_len;
    all_hole_info_t *p_hole = (all_hole_info_t*)(p_role_rsp->register_rsp.buf + pet_len);
    //all_plant_t *p_plant = (all_plant_t *)(p_role_rsp->register_rsp.buf + pet_len);

    if (get_user_plant(user_id, p_hole, p_mysql_conn) < 0)
    {
        KCRIT_LOG(user_id, "get user plant failed");
        return ERR_SQL_ERR;
    }

    //buf_len += sizeof(all_plant_t) + p_plant->count * sizeof(plant_t);
    buf_len += sizeof(all_hole_info_t) + p_hole->hole_count * sizeof(hole_info_t);
    KINFO_LOG(user_id, "max score:%u,plant:%u,approved:%u,unapproved:%u,friend req:%u, follow pet:%u", p_role_rsp->register_rsp.role.max_puzzle_score, p_hole->hole_count, p_role_rsp->register_rsp.role.approved_message_num, p_role_rsp->register_rsp.role.unapproved_message_num, p_role_rsp->register_rsp.pending_req_num, p_pet->count);

    g_pack.pack(buffer, buf_len);
    return 0;
}

/**
 * @brief query_other_role_info 查询其他用户的role信息,对应50440协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t query_other_role_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    char buffer[4096] = {0};
    as_msg_register_rsp_t *p_role_rsp = (as_msg_register_rsp_t *)buffer;
    int buf_len = sizeof(as_msg_register_rsp_t);

    message_num_t message_num = {0};
    //获得用户的role信息
    int ret = get_user_role(user_id, &p_role_rsp->role, p_mysql_conn, p_memcached, &message_num);
    if (ret < 0)
    {
        KCRIT_LOG(user_id, "get user role info failed");
        return ERR_SQL_ERR;
    }
    else if (0 == ret) //用户没有注册
    {
        KCRIT_LOG(user_id, "query user role info that is not exist");
        return ERR_SQL_NO_RECORD;
    }

    //p_role_rsp->approved_message_num = message_num.approved_message_num;
    //p_role_rsp->unapproved_message_num = message_num.unapproved_message_num;

    //获得用户的好友申请数量
   // sprintf(g_sql_str, "SELECT count(*) FROM db_monster_%u.t_friend_%u WHERE user_id = %u AND type = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, FRIEND_PENDING);
   // MYSQL_ROW row = NULL;
   // ret = p_mysql_conn->select_first_row(&row, g_sql_str);
   // if(ret <= 0)
   // {
   //     KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
   //     return ERR_SQL_ERR;
   // }

   // if(str2uint(&p_role_rsp->pending_req_num, row[0]) != 0)
   // {
   //     KCRIT_LOG(user_id, "convert pending req num:%s to uint8 failed(%s).", row[0], g_sql_str);
   //     return ERR_SQL_ERR;
   // }

    p_role_rsp->pending_req_num = 0;//进入别人家不需要知道好友申请数量
    following_pet_t *p_pet = (following_pet_t *)(p_role_rsp->buf);
    if (get_following_pet(user_id, p_pet, p_mysql_conn) < 0)
    {
        KCRIT_LOG(user_id, "get user pet failed");
        return ERR_SQL_ERR;
    }

    int pet_len = sizeof(following_pet_t) + p_pet->count * sizeof(uint32_t);
    buf_len += pet_len;

    g_pack.pack(buffer, buf_len);
    return 0;
}

int update_role_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, db_msg_update_role_req_t *p_role)
{
    assert(p_mysql_conn != NULL && p_role != NULL);

    if (0 == p_role->count)
    {
        //没有更新
        return 0;
    }

    role_t cached_role = {{0}};
    int mem_ret = p_memcached->get_role(user_id, &cached_role);

    char buf[100] = {0};
    sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d SET ", DB_ID(user_id), TABLE_ID(user_id));

    for (int i = 0; i != (int)p_role->count; ++i)
    {
        switch (p_role->field[i].type)
        {
            case FIELD_COIN:
                sprintf(buf, "coins = coins %s %u,", p_role->type == DB_ADD_ROLE ? "+" : "-", p_role->field[i].value);
                strcat(g_sql_str, buf);
                //if (p_role->type == DB_ADD_ROLE)
                //{
                //    KINFO_LOG(user_id, "add coins:%u", p_role->field[i].value);
                //}
                //else
                //{
                //    KINFO_LOG(user_id, "desc coins:%u", p_role->field[i].value);
                //}
                if (0 == mem_ret)
                {
                    if (p_role->type == DB_ADD_ROLE)
                    {
                        cached_role.coins += p_role->field[i].value;
                    }
                    else
                    {
                        cached_role.coins -= p_role->field[i].value;
                    }
                }
                break;
            case FIELD_EXP:
                sprintf(buf, "mon_exp = mon_exp %s %u,", p_role->type == DB_ADD_ROLE ? "+" : "-", p_role->field[i].value);
                strcat(g_sql_str, buf);
                //if (p_role->type == DB_ADD_ROLE)
                //{
                //    KINFO_LOG(user_id, "add exp:%u", p_role->field[i].value);
                //}
                //else
                //{
                //    KINFO_LOG(user_id, "desc exp:%u", p_role->field[i].value);
                //}
                if (0 == mem_ret)
                {
                    if (p_role->type == DB_ADD_ROLE)
                    {
                        cached_role.monster_exp += p_role->field[i].value;
                    }
                    else
                    {
                        cached_role.monster_exp -= p_role->field[i].value;
                    }
                }
                break;
            case FIELD_LEVEL:
                sprintf(buf, "mon_level = mon_level %s %u,", p_role->type == DB_ADD_ROLE ? "+" : "-", p_role->field[i].value);
                strcat(g_sql_str, buf);
                //if (p_role->type == DB_ADD_ROLE)
                //{
                //    KINFO_LOG(user_id, "add mon_level:%u", p_role->field[i].value);
                //}
                //else
                //{
                //    KINFO_LOG(user_id, "desc mon_level:%u", p_role->field[i].value);
                //}

                if (0 == mem_ret)
                {
                    if (p_role->type == DB_ADD_ROLE)
                    {
                        cached_role.monster_level += p_role->field[i].value;
                    }
                    else
                    {
                        cached_role.monster_level -= p_role->field[i].value;
                    }
                }
                break;
            case FIELD_HAPPY:
                sprintf(buf, "mon_happy = mon_happy %s %u,", p_role->type == DB_ADD_ROLE ? "+" : "-", p_role->field[i].value);
                strcat(g_sql_str, buf);
                //if (p_role->type == DB_ADD_ROLE)
                //{
                //    KINFO_LOG(user_id, "add mon_happy:%u", p_role->field[i].value);
                //}
                //else
                //{
                //    KINFO_LOG(user_id, "desc mon_happy:%u", p_role->field[i].value);
                //}
                if (0 == mem_ret)
                {
                    if (p_role->type == DB_ADD_ROLE)
                    {
                        cached_role.monster_happy += p_role->field[i].value;
                    }
                    else
                    {
                        cached_role.monster_happy -= p_role->field[i].value;
                    }
                }
                break;
            case FIELD_HEALTH:
                sprintf(buf, "mon_health = mon_health %s %u,", p_role->type == DB_ADD_ROLE ? "+" : "-", p_role->field[i].value);
                strcat(g_sql_str, buf);
                //if (p_role->type == DB_ADD_ROLE)
                //{
                //    KINFO_LOG(user_id, "add mon_health:%u", p_role->field[i].value);
                //}
                //else
                //{
                //    KINFO_LOG(user_id, "desc mon_health:%u", p_role->field[i].value);
                //}

                if (0 == mem_ret)
                {
                    if (p_role->type == DB_ADD_ROLE)
                    {
                        cached_role.monster_health += p_role->field[i].value;
                    }
                    else
                    {
                        cached_role.monster_health -= p_role->field[i].value;
                    }
                }
                break;
            case FIELD_THUMB:
                sprintf(buf, "thumb = thumb %s %u,", p_role->type == DB_ADD_ROLE ? "+" : "-", p_role->field[i].value);
                strcat(g_sql_str, buf);
                //if (p_role->type == DB_ADD_ROLE)
                //{
                //    KINFO_LOG(user_id, "add thumb:%u", p_role->field[i].value);
                //}
                //else
                //{
                //    KINFO_LOG(user_id, "desc thumb:%u", p_role->field[i].value);
                //}

                if (0 == mem_ret)
                {
                    if (p_role->type == DB_ADD_ROLE)
                    {
                        cached_role.thumb += p_role->field[i].value;
                    }
                    else
                    {
                        cached_role.thumb -= p_role->field[i].value;
                    }
                }
                break;
            case FIELD_VISITS:
                sprintf(buf, "visits = visits %s %u,", p_role->type == DB_ADD_ROLE ? "+" : "-", p_role->field[i].value);
                strcat(g_sql_str, buf);
                //if (p_role->type == DB_ADD_ROLE)
                //{
                //    KINFO_LOG(user_id, "add visit:%u", p_role->field[i].value);
                //}
                //else
                //{
                //    KINFO_LOG(user_id, "desc visit:%u", p_role->field[i].value);
                //}

                if (0 == mem_ret)
                {
                    if (p_role->type == DB_ADD_ROLE)
                    {
                        cached_role.visits += p_role->field[i].value;
                    }
                    else
                    {
                        cached_role.visits -= p_role->field[i].value;
                    }
                }
                break;
            case FIELD_MAX_PUZZLE_SCORE:
                sprintf(buf, "max_puzzle_score = max_puzzle_score %s %u,", p_role->type == DB_ADD_ROLE ? "+" : "-", p_role->field[i].value);
                strcat(g_sql_str, buf);
                //if (p_role->type == DB_ADD_ROLE)
                //{
                //    KINFO_LOG(user_id, "add max_puzzle_score:%u", p_role->field[i].value);
                //}
                //else
                //{
                //    KINFO_LOG(user_id, "desc max_puzzle_score:%u", p_role->field[i].value);
                //}

                if (0 == mem_ret)
                {
                    if (p_role->type == DB_ADD_ROLE)
                    {
                        cached_role.max_puzzle_score += p_role->field[i].value;
                    }
                    else
                    {
                        cached_role.max_puzzle_score -= p_role->field[i].value;
                    }
                }
                break;
            case FIELD_BOBO_REWARD:
                sprintf(buf, "last_paper_reward= last_paper_reward %s %u,", p_role->type == DB_ADD_ROLE ? "+" : "-", p_role->field[i].value);
                strcat(g_sql_str, buf);
                //if (p_role->type == DB_ADD_ROLE)
                //{
                //    KINFO_LOG(user_id, "add last_paper_reward:%u", p_role->field[i].value);
                //}
                //else
                //{
                //    KINFO_LOG(user_id, "desc last_paper_reward:%u", p_role->field[i].value);
                //}

                if (0 == mem_ret)
                {
                    if (p_role->type == DB_ADD_ROLE)
                    {
                        cached_role.last_paper_reward += p_role->field[i].value;
                    }
                    else
                    {
                        cached_role.last_paper_reward -= p_role->field[i].value;
                    }
                }
                break;
            default:
                break;
        }
    }

    g_sql_str[strlen(g_sql_str) - 1] = 0;

    sprintf(buf, " WHERE user_id = %u", user_id);
    strcat(g_sql_str, buf);

    KINFO_LOG(user_id, "update_role_value:%s", g_sql_str);
    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KERROR_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return -1;
    }

    if (0 == mem_ret)
    {
        p_memcached->set_role(user_id, &cached_role);
    }
    return 0;
}

/**
 * @brief update_role 更新role信息，目前只更新last_login_time和mon_happy,对应53416(0xD0A8)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t update_role(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    db_msg_update_login_t *p_update = (db_msg_update_login_t *)p_request_body;
    if (check_val_len(msg_type, request_len, sizeof(db_msg_update_login_t) + p_update->count * sizeof(field_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    role_t cached_role = {{0}};
    int mem_ret = p_memcached->get_role(user_id, &cached_role);

    sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d SET ", DB_ID(user_id), TABLE_ID(user_id));

    if (0 == p_update->count)
    {
        return 0;
    }
    char temp[100] = {0};
    for (int i = 0; i != p_update->count; ++i)
    {
        switch (p_update->field[i].type)
        {
            case FIELD_LOGIN_TIME:
                sprintf(temp, " last_login_time = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update last login time to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.last_login_time = p_update->field[i].value;
                }
                break;
            case FIELD_HAPPY:
                sprintf(temp, " mon_happy = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update mon_happy to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.monster_happy = p_update->field[i].value;
                }
                break;
            case FIELD_HEALTH:
                sprintf(temp, " mon_health = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update mon_health to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.monster_health = p_update->field[i].value;
                }
                break;
            case FIELD_LAST_VISIT_PLANTATION:
                sprintf(temp, " last_visit_plantation_time = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update last visit plantation time to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.last_visit_plantation_time = p_update->field[i].value;
                }
                break;
            case FIELD_PUZZLE_MAX_SCORE:
                sprintf(temp, " max_puzzle_score = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update max puzzle score to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.max_puzzle_score = p_update->field[i].value;
                }
                break;
            case FIELD_LOGOUT_TIME:
                sprintf(temp, " last_logout_time = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update logout time to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.last_logout_time = p_update->field[i].value;
                }
                break;
            case FIELD_FAV_COLOR:
                sprintf(temp, " fav_color = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update fav color to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.fav_color = p_update->field[i].value;
                }
                break;
            case FIELD_FAV_FRUIT:
                sprintf(temp, " fav_fruit = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update fav fruit to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.fav_fruit = p_update->field[i].value;
                }
                break;
            case FIELD_FAV_PET:
                sprintf(temp, " fav_pet = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update fav pet to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.fav_pet = p_update->field[i].value;
                }
                break;
            case FIELD_MOOD:
                sprintf(temp, " mood = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update mood to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.mood = p_update->field[i].value;
                }
                break;
            case FIELD_FLAG1:
                sprintf(temp, " flag1 = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update flag1 to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.flag1 = p_update->field[i].value;
                }
                break;
			case FIELD_GUIDE_FLAG:
                sprintf(temp, " guide_flag = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update guide_flag to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.guide_flag = p_update->field[i].value;
                }
                break;
            case FIELD_COMPOSE_TIME:
                sprintf(temp, " compose_time = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update compose time to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.compose_time = p_update->field[i].value;
                }
                break;
            case FIELD_COMPOSE_ID:
                sprintf(temp, " drawing_id = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update drawing id to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.compose_id = p_update->field[i].value;
                }
                break;
            case FIELD_ONLINE_TIME:
                sprintf(temp, " online_time = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update online time to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.online_time = p_update->field[i].value;
                }
                break;
            case FIELD_BOBO_READ:
                sprintf(temp, "last_paper_read = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update last paper read to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.last_paper_read = p_update->field[i].value;
                }
                break;
            case FIELD_NPC_SCORE:
                sprintf(temp, "npc_score = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update npc score to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.npc_score = p_update->field[i].value;
                }
                break;
            case FIELD_NPC_SCORE_TIME:
                sprintf(temp, "npc_score_daytime = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update npc score time to:%u.", p_update->field[i].value);
                if (0 == mem_ret)
                {
                    cached_role.npc_score_daytime = p_update->field[i].value;
                }
                break;
            case FIELD_LAST_SHOW_ID:
                sprintf(temp, "last_show_id = %u,", p_update->field[i].value);
                strcat(g_sql_str, temp);
                KINFO_LOG(user_id, "update last_show_id to %u", p_update->field[i].value);
                if(0 == mem_ret)
                {
                    cached_role.last_show_id = p_update->field[i].value;
                }
            default:
                break;
        }
    }
    g_sql_str[strlen(g_sql_str) - 1] = 0;

    sprintf(temp, " WHERE user_id = %u", user_id);
    strcat(g_sql_str, temp);

    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    if (0 == mem_ret)
    {
        p_memcached->set_role(user_id, &cached_role);
    }

    return 0;
}

uint32_t update_role_value(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    db_msg_update_role_req_t *p_role = (db_msg_update_role_req_t *)p_request_body;
    if (check_val_len(msg_type, request_len, sizeof(db_msg_update_role_req_t) + p_role->count * sizeof(field_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    if (update_role_info(p_mysql_conn, p_memcached, user_id, p_role) != 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    return 0;
}

uint32_t insert_name(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_insert_name_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_insert_name_req_t *p_req = (db_msg_insert_name_req_t *)p_request_body;

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql connection failed.");
        return ERR_SQL_ERR;
    }

    char buf[2 * sizeof(p_req->name) + 1] = {0};
    char name[sizeof(p_req->name) + 1] = {0};
    memcpy(name, p_req->name, sizeof(p_req->name));
    mysql_real_escape_string(p_conn, buf, name, strlen(name));

    KINFO_LOG(user_id, "user:[%s, %u] register game.", buf, user_id);

    sprintf(g_sql_str, "INSERT INTO db_monster_config.t_name(name, user_id) VALUES(\"%s\", %u)", buf, user_id);
    if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", mysql_error(p_conn));
        return ERR_SQL_ERR;
    }

    return 0;
}

uint32_t search_by_name(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_search_name_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_search_name_req_t *p_req = (db_msg_search_name_req_t *)p_request_body;
    char buffer[2 * sizeof(p_req->name) + 1] = {0};
    char name[sizeof(p_req->name) + 1] = {0};
    memcpy(name, p_req->name, sizeof(p_req->name));
    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }
    mysql_real_escape_string(p_conn, buffer, name, strlen(name));

    char rsp_buffer[1024] = {0};
    db_msg_search_name_rsp_t *p_rsp = (db_msg_search_name_rsp_t *)rsp_buffer;
    MYSQL_ROW row = NULL;
    //屏蔽名字搜索功能，会导致压力郭大
//    int result_count = p_mysql_conn->select_first_row(&row, "SELECT user_id FROM db_monster_config.t_name WHERE name like \"\%\%%s\%\%\" LIMIT %u", buffer, MAX_SEARCH_FRIEND_NUM);
//
   int result_count = 0;
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    while (row != NULL)
    {
        uint32_t friend_id = 0;
        if(str2uint(&friend_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert user_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }
        if (friend_id == user_id)
        {
            row = p_mysql_conn->select_next_row(true);
            continue;
        }

        KINFO_LOG(user_id, "get user_id:%u by name", friend_id);
        p_rsp->user_id[p_rsp->count] = friend_id;
        ++p_rsp->count;
        if (p_rsp->count > MAX_SEARCH_FRIEND_NUM)
        {
            break;
        }
        row = p_mysql_conn->select_next_row(true);
    }

    g_pack.pack((char *)p_rsp, sizeof(db_msg_search_name_rsp_t) + p_rsp->count * sizeof(uint32_t));

    return 0;
}


uint32_t get_profile_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }


    role_t role = {{0}};
    //获得用户的role信息
    int ret = get_user_role(user_id, &role, p_mysql_conn, p_memcached);
    if (ret <= 0)
    {
        KCRIT_LOG(user_id, "get user role info failed:%u", ret);
        return ERR_SQL_ERR;
    }

    db_msg_street_user_rsp_t rsp = {{{0}}};
    memcpy(rsp.profile.name, role.name, sizeof(role.name));
    rsp.profile.gender = role.gender;
    rsp.profile.country_id = role.country_id;
    rsp.profile.user_type = role.user_type;
    rsp.profile.birthday = role.birthday;
    rsp.profile.monster_id = role.monster_id;
    memcpy(rsp.profile.monster_name, role.monster_name, sizeof(role.monster_name));
    rsp.profile.monster_main_color = role.monster_main_color;
    rsp.profile.monster_ex_color = role.monster_ex_color;
    rsp.profile.monster_eye_color = role.monster_eye_color;
    rsp.profile.monster_exp = role.monster_exp;
    rsp.profile.monster_level = role.monster_level;
    rsp.profile.monster_health = role.monster_health;
    rsp.profile.monster_happy = role.monster_happy;
    rsp.profile.friend_num = role.friend_num;
    rsp.profile.visits = role.visits;
    //这里还需要添加最近未读访客数
    //rsp.profile.recent_unread_badge = role.recent_unread_badge;

    rsp.profile.mood = role.mood;
    rsp.profile.fav_color = role.fav_color;
    rsp.profile.fav_pet = role.fav_pet;
    rsp.profile.fav_fruit = role.fav_fruit;
    memcpy(rsp.profile.personal_sign, role.personal_sign, sizeof(role.personal_sign));
    rsp.profile.room_num = role.room_num;
    rsp.profile.pet_num = role.pet_num;
    rsp.profile.max_puzzle_score = role.max_puzzle_score;

    if (role.pet_num > 0)
    {
        int index = uniform_rand(0, role.pet_num - 1);
        snprintf(g_sql_str, sizeof(g_sql_str), "SELECT pet_id FROM db_monster_%d.t_pet_%d WHERE user_id = %u LIMIT %u, 1", DB_ID(user_id), TABLE_ID(user_id), user_id, index);

        MYSQL_ROW row = NULL;
        int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
        if(result_count < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            return -1;
        }
        else if (0 == result_count)
        {
            rsp.pet_id = 0;
        }
        else if(str2uint(&rsp.pet_id, row[0]) < 0)
        {
            KCRIT_LOG(user_id, "convert pet_id:%s to uint32 failed(%s).", row[1], g_sql_str);
            return -1;
        }
        else
        {
            //do nothing
        }
    }
    else
    {
        rsp.pet_id = 0;
    }
//    KINFO_LOG(user_id, "get profile, happy:%u, health:%u", rsp.profile.monster_happy, rsp.profile.monster_health);
    g_pack.pack((char *)&rsp, sizeof(rsp));

    return 0;
}

uint32_t eat_food(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_eat_food_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_eat_food_req_t *p_req = (db_msg_eat_food_req_t *)p_request_body;

    KINFO_LOG(user_id, "[eat food]food id:%u, reward happy:%u, reward health:%u, reward_coins:%u", p_req->item_id, p_req->reward_happy, p_req->reward_health, p_req->reward_coins);

    snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_stuff_%d SET stuff_num = stuff_num - 1 WHERE user_id = %u AND stuff_id = %u and stuff_num > 0", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->item_id);

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
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
    	mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }

    //增加怪兽的愉悦值和健康值
    snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_role_%d SET mon_happy = mon_happy + %u, mon_health = mon_health + %u, coins = coins + %u WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), p_req->reward_happy, p_req->reward_health, p_req->reward_coins, user_id);

    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
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

    //更改缓存里的值
    role_t cached_role = {{0}};
    if (p_memcached->get_role(user_id, &cached_role) == 0)
    {
        cached_role.monster_happy += p_req->reward_happy;
        cached_role.monster_health += p_req->reward_health;
        cached_role.coins += p_req->reward_coins;
        p_memcached->set_role(user_id, &cached_role);
    }

    return 0;
}

uint32_t modify_personal_sign(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (request_len > MAX_SIGN_BYTE)
    {
        KCRIT_LOG(user_id, "personal sign is too long:%u.", request_len);
        return ERR_MSG_LEN;
    }

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    char buf[2 * MAX_SIGN_BYTE + 1] = {0};
    mysql_real_escape_string(p_conn, buf, p_request_body, request_len);

    sprintf(g_sql_str, "UPDATE db_monster_%u.t_role_%u SET personal_sign = \"%s\" WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), buf, user_id);
    if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", mysql_error(p_conn));
        return ERR_SQL_ERR;
    }

    //更改缓存里的值
    role_t cached_role = {{0}};
    if (p_memcached->get_role(user_id, &cached_role) == 0)
    {
        memset(cached_role.personal_sign, 0, sizeof(cached_role.personal_sign));
        memcpy(cached_role.personal_sign, p_request_body, request_len);
        p_memcached->set_role(user_id, &cached_role);
    }

    return 0;
}

uint32_t update_login_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    db_msg_update_login_req_t *p_req = (db_msg_update_login_req_t *)p_request_body;
    if (check_val_len(msg_type, request_len, sizeof(db_msg_update_login_req_t) + p_req->plant_count * sizeof(plant_growth_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

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

    if(p_req->limit_reward == 1)
    {
        sprintf(g_sql_str, "INSERT INTO db_monster_%u.t_stuff_%u(user_id, stuff_id, stuff_num) values(%u, %u, 1);", DB_ID(user_id), TABLE_ID(user_id), user_id, LIMIT_PAPER_ID);
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s) limit_reward 1.", p_mysql_conn->get_last_errstr());
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
    	sprintf(g_sql_str, "UPDATE db_monster_%u.t_role_%u SET last_login_time = %u, mon_happy = %u, offline_time = %u, limit_reward = 1 WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), p_req->login_time, p_req->monster_happy, p_req->offline_time, user_id);
    }
    else if(p_req->limit_reward == 2)
    {
        sprintf(g_sql_str, "INSERT INTO db_monster_%u.t_stuff_%u(user_id, stuff_id, stuff_num) values(%u, %u, 1), (%u, %u, 1);", DB_ID(user_id), TABLE_ID(user_id), user_id, LIMIT_PAPER_ID, user_id, LIMIT_STATUE_ID);
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s) repeat limit_reward 2.", p_mysql_conn->get_last_errstr());
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
    	sprintf(g_sql_str, "UPDATE db_monster_%u.t_role_%u SET last_login_time = %u, mon_happy = %u, offline_time = %u, limit_reward = 1 WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), p_req->login_time, p_req->monster_happy, p_req->offline_time, user_id);
    }
	else
	{

    //将封测大礼字段置为已领取
    	sprintf(g_sql_str, "UPDATE db_monster_%u.t_role_%u SET last_login_time = %u, mon_happy = %u, offline_time = %u  WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), p_req->login_time, p_req->monster_happy, p_req->offline_time, user_id);
	}


	if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        mysql_rollback(p_conn);
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }


    time_t now = time(NULL);
    for (int i = 0; i != (int)p_req->plant_count; ++i)
    {
        snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_plant_%d  SET growth = growth + %u, last_growth_time = %zu, maintain = %u WHERE user_id = %u AND hole_id = %u", DB_ID(user_id), TABLE_ID(user_id), p_req->plant_growth[i].growth, now, p_req->plant_growth[i].new_maintain_type, user_id, p_req->plant_growth[i].hole_id);

        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
        KINFO_LOG(user_id, "login update_plant_growth[%s]", g_sql_str);
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
        cached_role.last_login_time = p_req->login_time;
        cached_role.monster_happy = p_req->monster_happy;
        cached_role.offline_time = p_req->offline_time;
		if(p_req->limit_reward != 0)
		{
        	cached_role.limit_reward = p_req->limit_reward;
		}
		p_memcached->set_role(user_id, &cached_role);
    }

    //拉取用户的成就列表

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


uint32_t update_coins_from_interactive(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    db_msg_interactive_req_t *p_req = (db_msg_interactive_req_t *)p_request_body;
    if (check_val_len(msg_type, request_len, sizeof(db_msg_interactive_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

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

    KDEBUG_LOG(user_id, "interactive request(day_time:%u id:%u reward_coins:%u)", p_req->day_time, p_req->interactive_id, p_req->reward_coins);

    snprintf(g_sql_str, sizeof(g_sql_str), "SELECT value FROM db_monster_%d.t_day_restrict_%d WHERE user_id = %u AND time = %u AND type = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->day_time, p_req->interactive_id);
    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
     if (result_count < 0)
     {
          KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
          return ERR_SQL_ERR;
     }
     else if (0 == result_count)
     {//当天第一次抽奖
         //增加限制
        sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_day_restrict_%d(user_id, time, type, value) values(%u, %u, %u, 1)", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->day_time, p_req->interactive_id);

        if(p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }

         //增加用户的咕咚果数
         snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_role_%d SET coins = coins + %u WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), p_req->reward_coins, user_id);

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

        //更改memcache里的金币
        role_t cached_role = {{0}};
        if (p_memcached->get_role(user_id, &cached_role) == 0)
        {
            cached_role.coins += p_req->reward_coins;
            p_memcached->set_role(user_id, &cached_role);
        }

        g_pack.pack((uint8_t)0);
        g_pack.pack((uint16_t)p_req->reward_coins);
     }
     else
     {//当天已经抽过奖，达到每日限制
         g_pack.pack((uint8_t)1);
         g_pack.pack((uint16_t)0);
     }

    mysql_autocommit(p_conn, true);

    return 0;
}


uint32_t update_recent_badge(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    uint16_t *p_unread_badge_num = (uint16_t*)p_request_body;
    if (check_val_len(msg_type, request_len, sizeof(uint16_t)) != 0)
    {
        return ERR_MSG_LEN;
    }
    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    KINFO_LOG(user_id, "set unread_badge_num to :%u", *p_unread_badge_num);

    if(*p_unread_badge_num == 0)
    {
        if(mysql_autocommit(p_conn, false) != 0)
        {
            KCRIT_LOG(user_id, "close mysql_autocommit failed.");
            return ERR_SQL_ERR;
        }
    }
    snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_role_%d SET recent_badge = %u  WHERE user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), *p_unread_badge_num, user_id);

    KINFO_LOG(user_id, "update_unread_badge:%s", g_sql_str);

    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KERROR_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        if(*p_unread_badge_num == 0)
        {
            mysql_autocommit(p_conn, true);
        }
        return -1;
    }

    if(*p_unread_badge_num == 0)
    {
        sprintf(g_sql_str, "UPDATE db_monster_%d.t_badge_%d SET status = %d  WHERE user_id = %u and status = %d;", DB_ID(user_id), TABLE_ID(user_id), badge_acquired_no_reward, user_id, badge_acquired_no_screen);

        KINFO_LOG(user_id, "update_badge_status:%s", g_sql_str);

        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KERROR_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            if(*p_unread_badge_num == 0)
            {
                mysql_rollback(p_conn);
                mysql_autocommit(p_conn, true);
            }
            return -1;
        }

    }

    if(*p_unread_badge_num == 0)
    {
        // 结束事务
        if (mysql_commit(p_conn) != 0)
        {
            KCRIT_LOG(user_id, "mysql_commit() failed.");
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }

        mysql_autocommit(p_conn, true);

    }

    g_pack.pack((*p_unread_badge_num));

    role_t cached_role = {{0}};
    if (p_memcached->get_role(user_id, &cached_role) == 0)
    {
        cached_role.recent_unread_badge = *p_unread_badge_num;
        p_memcached->set_role(user_id, &cached_role);
    }

    return 0;
}


uint32_t compose_stuff(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    db_msg_compose_stuff_req_t *p_req = (db_msg_compose_stuff_req_t *)p_request_body;
    if (check_val_len(msg_type, request_len, sizeof(db_msg_compose_stuff_req_t) + sizeof(item_cost_t) * p_req->num) != 0)
    {
        return ERR_MSG_LEN;
    }

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }

    sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d SET drawing_id = %u, compose_time = %u, compose_id = %u WHERE user_id = %u",
            DB_ID(user_id), TABLE_ID(user_id), p_req->stuff_id, p_req->finish_time, p_req->map_id, user_id);

    //更改用户的role信息
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

    //减少用户的物品
    for (int i = 0; i != p_req->num; ++i)
    {
        sprintf(g_sql_str, "UPDATE db_monster_%d.t_stuff_%d SET stuff_num = stuff_num - %u WHERE user_id = %u AND stuff_id = %u",
                DB_ID(user_id), TABLE_ID(user_id), p_req->item_cost[i].item_num, user_id, p_req->item_cost[i].item_id);

        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
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

    //更改memcached缓存
    role_t cached_role = {{0}};
    if (p_memcached->get_role(user_id, &cached_role) == 0)
    {
        cached_role.compose_time = p_req->finish_time;
        cached_role.compose_id = p_req->stuff_id;
        cached_role.map_id = p_req->map_id;
        p_memcached->set_role(user_id, &cached_role);
    }

    return 0;
}

uint32_t modify_monster_name(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 16) != 0)
    {
        return ERR_MSG_LEN;
    }

    char name_esp[2 * 16 + 1] = {0};
    char name[16 + 1] = {0};
    memcpy(name, p_request_body, 16);

    MYSQL *p_conn = p_mysql_conn->get_conn();
    if (NULL == p_conn)
    {
        KCRIT_LOG(user_id, "get mysql conn failed.");
        return ERR_SQL_ERR;
    }
    mysql_real_escape_string(p_conn, name_esp, name, strlen(name));

    sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d SET mon_name = \'%s\' WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), name_esp, user_id);

    if (mysql_real_query(p_conn, g_sql_str, strlen(g_sql_str)) != 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        mysql_autocommit(p_conn, true);
        return ERR_SQL_ERR;
    }

    //更改缓存里的值
    role_t cached_role = {{0}};
    if (p_memcached->get_role(user_id, &cached_role) == 0)
    {
        memcpy(cached_role.monster_name, p_request_body, 16);
        p_memcached->set_role(user_id, &cached_role);
    }

    return 0;
}
