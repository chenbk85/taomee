/**
 * =====================================================================================
 *       @file  plantation.cpp
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
#include "plantation.h"
#include "../role-info/role_info.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t pet_op(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_pet_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_pet_req_t *p_req = (db_msg_pet_req_t *)p_request_body;

    KINFO_LOG(user_id, "[pet op]type:%u, id:%u", p_req->type, p_req->id);
    if (PET_ADD == p_req->type) //增加小怪兽
    {
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

        //判断现在是否有这个类别的小怪兽
        snprintf(g_sql_str, sizeof(g_sql_str), "SELECT COUNT(*) FROM db_monster_%d.t_pet_%d WHERE user_id = %u AND pet_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->id);
        MYSQL_ROW row = NULL;
        int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
        if (result_count <= 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]mysql select failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
        uint8_t num = 0;
        if(str2uint(&num, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "get pet:%u count failed.", p_req->id);
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }

        //不存在就新增一条记录，存在就直接更新当前数量
        if (0 == num)
        {
            snprintf(g_sql_str, sizeof(g_sql_str), "INSERT INTO db_monster_%d.t_pet_%d(user_id, pet_id, total_num, follow_num) VALUES(%u, %u, 1, 1)", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->id);
        }
        else
        {
            snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_pet_%d SET total_num = total_num + 1, follow_num = follow_num + %u WHERE user_id = %u AND pet_id = %u", DB_ID(user_id), TABLE_ID(user_id), p_req->status == PET_FOLLOWING ?  1 : 0, user_id, p_req->id);
        }

        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }

        //删除植物
        sprintf(g_sql_str, "UPDATE db_monster_%d.t_plant_%d  SET plant_id = 0, color = 0, growth = 0, last_extra_growth_time = 0, last_growth_value = 0, last_growth_time = 0, maintain = 0, next_maintain_time = 0,  maintain_count = 0 where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id);

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


        //如果新增的精灵，更新缓存里面的精灵类别数量
        if (0 == num)
        {
            role_t cached_role = {{0}};
            int mem_ret = p_memcached->get_role(user_id, &cached_role);
            if (0 == mem_ret)
            {
                cached_role.pet_num += 1;
                p_memcached->set_role(user_id, &cached_role);
            }
        }

        g_pack.pack(num);
    }
    else if(p_req->type == PET_DROP)
    {//删除
        //判断现在是否有这个类别的小怪兽
        snprintf(g_sql_str, sizeof(g_sql_str), "SELECT total_num FROM db_monster_%d.t_pet_%d WHERE user_id = %u AND pet_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->id);
        MYSQL_ROW row = NULL;
        int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
        if (result_count <= 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]mysql select failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
            return ERR_SQL_ERR;
        }
        uint8_t num = 0;
        if(str2uint(&num, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "get pet:%u count failed.", p_req->id);
            return ERR_SQL_ERR;
        }

        //不存在就新增一条记录，存在就直接更新当前数量
        if (1 == num)
        {
            snprintf(g_sql_str, sizeof(g_sql_str), "delete from  db_monster_%d.t_pet_%d where user_id = %u and pet_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->id);
        }
        else
        {
            snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_pet_%d SET total_num = total_num - 1, follow_num = follow_num - 1 WHERE user_id = %u AND pet_id = %u", DB_ID(user_id), TABLE_ID(user_id),user_id, p_req->id);
        }

        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            return ERR_SQL_ERR;
        }
        //如果新增的精灵，更新缓存里面的精灵类别数量
        if (1 == num)
        {
            role_t cached_role = {{0}};
            int mem_ret = p_memcached->get_role(user_id, &cached_role);
            if (0 == mem_ret)
            {
                cached_role.pet_num -= 1;
                p_memcached->set_role(user_id, &cached_role);
            }
        }

        g_pack.pack(num);

    }

    g_pack.pack(p_req->type);
    g_pack.pack(p_req->id);
    return 0;
}

uint32_t add_plant(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_add_plant_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_add_plant_req_t *p_req = (db_msg_add_plant_req_t *)p_request_body;

    KINFO_LOG(user_id, "[add plant]plant id:%u, hole id:%u, color:%u", p_req->plant_id, p_req->hole_id, p_req->color);

    snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_plant_%d set plant_id = %u, color = %u, last_extra_growth_time = %u, last_growth_time= %u, maintain = %u, next_maintain_time = %u where user_id = %u and hole_id = %u;", DB_ID(user_id), TABLE_ID(user_id), p_req->plant_id, p_req->color, p_req->last_extra_growth_time, p_req->last_grown_time, p_req->maintain, p_req->maintain_time,  user_id, p_req->hole_id);

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

    //将种子数量减1
    snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_stuff_%d SET stuff_num = stuff_num - 1 WHERE user_id = %u AND stuff_id = %u and stuff_num > 0", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->plant_id);
    KINFO_LOG(user_id, "desc seed:%s", g_sql_str);

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

    return 0;
}

uint32_t del_plant(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_del_plant_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_del_plant_req_t *p_req = (db_msg_del_plant_req_t *)p_request_body;
    if(p_req->hole_id)
    {
        sprintf(g_sql_str,"UPDATE db_monster_%d.t_plant_%d  SET plant_id = 0, color = 0, growth = 0, last_extra_growth_time = 0, last_growth_value = 0, last_growth_time = 0, maintain = 0,next_maintain_time = 0, maintain_count = 0 where user_id = %u and hole_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->hole_id);
    }
    else
    {
        sprintf(g_sql_str, "UPDATE db_monster_%d.t_plant_%d  SET plant_id = 0, color = 0, growth = 0, last_extra_growth_time = 0, last_growth_value = 0, last_growth_time = 0, maintain = 0, next_maintain_time = 0,  maintain_count = 0 where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id);
    }

     KINFO_LOG(user_id, "%s", g_sql_str);
    if (p_mysql_conn->execsql(g_sql_str) < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    return 0;
}

uint32_t update_plant(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (request_len < sizeof(db_msg_update_plant_req_t))
    {
        KCRIT_LOG(user_id, "req len:%u is invalid", request_len);
        return ERR_MSG_LEN;
    }
    db_msg_update_plant_req_t *p_req = (db_msg_update_plant_req_t *)p_request_body;

    if (check_val_len(msg_type, request_len, sizeof(db_msg_update_plant_req_t) + p_req->count * sizeof(plant_growth_t)) != 0)
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

  //  char temp[1024] = {0};
    for (int i = 0; i != (int)p_req->count; ++i)
    {
        snprintf(g_sql_str, sizeof(g_sql_str), "UPDATE db_monster_%d.t_plant_%d  SET growth = growth + %u,", DB_ID(user_id), TABLE_ID(user_id), p_req->plant_growth[i].growth);

        if (p_req->plant_growth[i].growth_time)
        {
	    sprintf(g_sql_str + strlen(g_sql_str), "last_growth_time = %u,", p_req->plant_growth[i].growth_time);
        }
        if (p_req->plant_growth[i].last_add_extra_growth_time)
        {
	    sprintf(g_sql_str + strlen(g_sql_str), "last_extra_growth_time = %u,", p_req->plant_growth[i].last_add_extra_growth_time);
        }
        if (p_req->plant_growth[i].last_growth_value)
        {
	        sprintf(g_sql_str + strlen(g_sql_str), "last_growth_value = %u,", p_req->plant_growth[i].last_growth_value);
        }
        if(p_req->plant_growth[i].last_reward_id)
        {
	        sprintf(g_sql_str + strlen(g_sql_str), "last_reward_id = %u,", p_req->plant_growth[i].last_reward_id);
        }

        sprintf(g_sql_str + strlen(g_sql_str), "maintain = %u, next_maintain_time = %u, maintain_count= %u", p_req->plant_growth[i].new_maintain_type, p_req->plant_growth[i].next_maintain_time, p_req->plant_growth[i].maintain_count);

    	sprintf(g_sql_str + strlen(g_sql_str), " WHERE user_id = %u AND hole_id = %u", user_id, p_req->plant_growth[i].hole_id);

        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
        KINFO_LOG(user_id, "[update plant]:%s", g_sql_str);
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


uint32_t get_all_hole_reward(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    sprintf(g_sql_str, "SELECT hole_id, reward_id, reward_happy, reward_exp, reward_coins FROM db_monster_%d.t_plant_reward_%d WHERE user_id = %u limit 600", DB_ID(user_id), TABLE_ID(user_id), user_id);
    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if(result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exex failed(%s)", g_sql_str);
        return -1;
    }

    char buffer[4096] = {0};
    db_all_hole_reward_rsp_t *p_all_reward = (db_all_hole_reward_rsp_t*)buffer;
    uint32_t idx = 0;
    while(row != NULL)
    {
        if(str2uint(&p_all_reward->hole_reward[idx].hole_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert hole_id:%s to uint8 failed(%s).", row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_all_reward->hole_reward[idx].reward_id, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "convert reward_id:%s to uint32 failed(%s).", row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_all_reward->hole_reward[idx].sun_reward.reward_happy, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "convert reward_happy:%s to uint16 failed(%s).", row[2], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_all_reward->hole_reward[idx].sun_reward.reward_exp, row[3]) != 0)
        {
            KCRIT_LOG(user_id, "convert reward_exp:%s to uint16 failed(%s).", row[3], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_all_reward->hole_reward[idx].sun_reward.reward_coins, row[4]) != 0)
        {
            KCRIT_LOG(user_id, "convert reward_coins:%s to uint16 failed(%s).", row[4], g_sql_str);
            return ERR_SQL_ERR;
        }
        idx++;
        row = p_mysql_conn->select_next_row(true);
    }

    p_all_reward->reward_num = idx;

    g_pack.pack(buffer, sizeof(db_all_hole_reward_rsp_t) + idx*sizeof(all_hole_reward_t));

    return 0;
}

uint32_t maintain_plant(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_maintain_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_maintain_req_t *p_req = (db_msg_maintain_req_t*)p_request_body;

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


     //sprintf(g_sql_str,"UPDATE db_monster_%d.t_plant_%d  SET growth = growth + %u, last_growth_value = growth + %u, last_growth_time = %u, maintain = %u, maintain_count = %u, last_reward_id = %u where user_id = %u and hole_id = %u;", DB_ID(user_id), TABLE_ID(user_id), p_req->add_growth, p_req->add_growth, p_req->add_growth_time, p_req->new_maintain_type, p_req->maintain_count, p_req->last_reward_id, user_id, p_req->hole_id);

    sprintf(g_sql_str,"UPDATE db_monster_%d.t_plant_%d  SET growth = growth + %u, last_growth_value = growth, last_growth_time = %u, maintain = %u, maintain_count = %u, last_reward_id = %u where user_id = %u and hole_id = %u;", DB_ID(user_id), TABLE_ID(user_id), p_req->add_growth, p_req->add_growth_time, p_req->new_maintain_type, p_req->maintain_count, p_req->last_reward_id, user_id, p_req->hole_id);
     KINFO_LOG(user_id, "%s", g_sql_str);


        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }

    sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_plant_reward_%d(user_id, hole_id, reward_id, reward_happy, reward_exp, reward_coins) VALUES(%u, %u, %u, %u, %u, %u) ON DUPLICATE KEY UPDATE reward_happy = %u, reward_exp = %u, reward_coins = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->hole_id, p_req->last_reward_id, p_req->reward_happy, p_req->reward_exp, p_req->reward_coins, p_req->reward_happy, p_req->reward_exp, p_req->reward_coins);

    KINFO_LOG(user_id, "%s", g_sql_str);
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
    return 0;
}

uint32_t msg_get_plant(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_get_plant_req_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_get_plant_req_t *p_req = (db_msg_get_plant_req_t*)p_request_body;

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


     sprintf(g_sql_str, "UPDATE db_monster_%d.t_plant_%d  SET plant_id = 0, color = 0, growth = 0, last_extra_growth_time = 0, last_growth_value = 0, last_growth_time = 0, maintain = 0, maintain_count = 0 where user_id = %u and hole_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->hole_id);

     KINFO_LOG(user_id, "%s", g_sql_str);


        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }

        sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set coins = coins + %u where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), p_req->reward_coins, user_id);
    KINFO_LOG(user_id, "%s", g_sql_str);
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
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

uint32_t sun_reward_happy(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(db_msg_sun_reward_happy_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    db_msg_sun_reward_happy_t *p_req = (db_msg_sun_reward_happy_t*)p_request_body;

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


    if(p_req->reward_happy != 0)
    {
        sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d  SET  mon_happy = mon_happy + %u where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), p_req->reward_happy, user_id);

        KINFO_LOG(user_id, "%s", g_sql_str);
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
        //更改memcache里的金币
        role_t cached_role = {{0}};
        if (p_memcached->get_role(user_id, &cached_role) == 0)
        {
            cached_role.monster_happy += p_req->reward_happy;
            p_memcached->set_role(user_id, &cached_role);
        }
    }

    sprintf(g_sql_str, "DELETE FROM db_monster_%d.t_plant_reward_%d WHERE user_id= %u and hole_id = %u and reward_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->sun_reward.hole_id, p_req->sun_reward.reward_id);

    KINFO_LOG(user_id, "%s", g_sql_str);
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
    return 0;
}

uint32_t sun_reward_exp(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(sun_reward_exp_cache_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    sun_reward_exp_cache_t *p_req = (sun_reward_exp_cache_t*)p_request_body;

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


    if(p_req->reward_exp != 0)
    {
        sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d  SET  mon_exp = mon_exp + %u, mon_level = mon_level + %u where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), p_req->reward_exp, p_req->level_up, user_id);

        KINFO_LOG(user_id, "%s", g_sql_str);
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
            mysql_rollback(p_conn);
            mysql_autocommit(p_conn, true);
            return ERR_SQL_ERR;
        }
        //更改memcache里的金币
        role_t cached_role = {{0}};
        if (p_memcached->get_role(user_id, &cached_role) == 0)
        {
            cached_role.monster_level += p_req->level_up;
            cached_role.monster_exp += p_req->reward_exp;
            p_memcached->set_role(user_id, &cached_role);
        }
    }

    sprintf(g_sql_str, "DELETE FROM db_monster_%d.t_plant_reward_%d WHERE user_id= %u and hole_id = %u and reward_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->sun_reward.hole_id, p_req->sun_reward.reward_id);

    KINFO_LOG(user_id, "%s", g_sql_str);
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
    return 0;
}

uint32_t sun_reward_coins(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(sun_reward_coins_cache_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    sun_reward_coins_cache_t *p_req = (sun_reward_coins_cache_t*)p_request_body;

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


    if(p_req->reward_coins != 0)
    {
        sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d  SET  coins = coins + %u where user_id = %u;", DB_ID(user_id), TABLE_ID(user_id), p_req->reward_coins, user_id);

        KINFO_LOG(user_id, "%s", g_sql_str);
        if (p_mysql_conn->execsql(g_sql_str) < 0)
        {
            KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
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

    sprintf(g_sql_str, "DELETE FROM db_monster_%d.t_plant_reward_%d WHERE user_id= %u and hole_id = %u and reward_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_req->sun_reward.hole_id, p_req->sun_reward.reward_id);

    KINFO_LOG(user_id, "%s", g_sql_str);
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
    return 0;
}
