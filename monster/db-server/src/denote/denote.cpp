/*
 * =====================================================================================
 *
 *       Filename:  denote.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年04月17日 15时11分46秒
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
#include "../../../common/pack/c_pack.h"
#include "../../../common/message.h"
#include "../../../common/data_structure.h"
#include "../../../common/constant.h"
#include "../util.h"
#include "../db_constant.h"
#include "denote.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};
/*
uint32_t select_cur_denote(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    //查询当前是哪一期的援助计划
    sprintf(g_sql_str, "SELECT denote_id, cur_denote_num,  cur_denoter,  end_flag FROM db_monster_config.t_denote order by denote_id desc limit 1;");

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    KINFO_LOG(user_id, "sql:%s", g_sql_str);

    char buffer[4096] = {0};
    msg_db_select_denote_t *p_rsp = (msg_db_select_denote_t*)buffer;

    uint32_t denote_id = 0;
    uint8_t flag = 0;
    uint32_t reward_id = 0;
    if(row != NULL)
    {
        if(str2uint(&flag, row[3]) != 0)
        {//最近一期是否结束
            KCRIT_LOG(user_id, "[msg:%u]convert flag:%s to uint8 failed(%s).", msg_type, row[3], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&denote_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert denote_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(flag == 0)
        {//未结束
            if(str2uint(&p_rsp->denoter_num, row[2]) != 0)
            {
                KCRIT_LOG(user_id, "[msg:%u]convert cur_denoter_num:%s to uint32 failed(%s).", msg_type, row[2], g_sql_str);
                return ERR_SQL_ERR;
            }

            if(str2uint(&p_rsp->cur_denote_num, row[1]) != 0)
            {
                KCRIT_LOG(user_id, "[msg:%u]convert cur_denote_num:%s to uint32 failed(%s).", msg_type, row[1], g_sql_str);
                return ERR_SQL_ERR;
            }

            p_rsp->denote_id = denote_id;
//查看用户上一期是否有未领取的奖励
            sprintf(g_sql_str, "SELECT denote_id FROM db_monster_%d.t_denote_%d where user_id = %u and reward_flag = 0 and denote_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, denote_id - 1);
        }
        else
        {//都结束
            p_rsp->denote_id  = denote_id + 1;
            p_rsp->cur_denote_num = 0;
            p_rsp->denoter_num = 0;

            //查询用户是否有未领取的奖励
            sprintf(g_sql_str, "SELECT denote_id FROM db_monster_%d.t_denote_%d where user_id = %u and reward_flag = 0 limit 1;", DB_ID(user_id), TABLE_ID(user_id), user_id);
	}

            result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
            if (result_count < 0)
            {
                KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                return ERR_SQL_ERR;
            }

            KINFO_LOG(user_id, "sql:%s", g_sql_str);

            if(row != NULL)
            {//有未领取的
                if(str2uint(&denote_id, row[0]) != 0)
                {
                    KCRIT_LOG(user_id, "2:[msg:%u]convert denote_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
                    return ERR_SQL_ERR;
                }

                //查看未领取的奖励id
                sprintf(g_sql_str, "SELECT reward_id FROM db_monster_config.t_denote where denote_id = %u;", denote_id);
                result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
                if (result_count < 0)
                {
                    KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                    return ERR_SQL_ERR;
                }

                KINFO_LOG(user_id, "sql:%s", g_sql_str);
                if(row != NULL)
                {
                    if(str2uint(&reward_id, row[0]) != 0)
                    {
                        KCRIT_LOG(user_id, "2:[msg:%u]convert reward_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
                        return ERR_SQL_ERR;
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

                    sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) values(%u, %u, 1) ON DUPLICATE KEY UPDATE stuff_num = stuff_num + 1;", DB_ID(user_id), TABLE_ID(user_id), user_id, reward_id);
                    if (p_mysql_conn->execsql(g_sql_str) < 0)
                    {
                        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                        mysql_autocommit(p_conn, true);
                        return ERR_SQL_ERR;
                    }

                    sprintf(g_sql_str, "UPDATE db_monster_%d.t_denote_%d set reward_flag = 1 where user_id = %u and denote_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, denote_id);
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

                }
                else
                {
                    KCRIT_LOG(user_id, "system error");
                    return ERR_SQL_ERR;
                }
                p_rsp->last_unreward_denote = denote_id;
                p_rsp->last_reward_id = reward_id;
            }
            else
            {//没有未领取的
		KINFO_LOG(user_id, "no reward now");
                p_rsp->last_unreward_denote = 0;
                p_rsp->last_reward_id = 0;
            }
       // }
    }
    else
    {//row == NULL
        KCRIT_LOG(user_id, "system error, must have init data int db");
        return ERR_SQL_ERR;
    }

    g_pack.pack(buffer, sizeof(msg_db_select_denote_t));

    return 0;
}


uint32_t select_history_denote(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    sprintf(g_sql_str, "SELECT denote_id, cur_denote_num, cur_denoter, helped_pet, end_flag FROM db_monster_config.t_denote order by denote_id desc;");

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    KINFO_LOG(user_id, "sql:%s", g_sql_str);

    char buffer[4096] = {0};
    msg_history_denote_t *p_rsp = (msg_history_denote_t*)buffer;

    uint16_t idx = 0;
    uint32_t num = 0;

    while(row != NULL)
    {

        if(str2uint(&p_rsp->denote[idx].denote_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert denote_id:%s to uint32 failed(%s).", msg_type, row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&num, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert cur_denote_num:%s to uint32 failed(%s).", msg_type, row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        p_rsp->denote_total += num;
        num = 0;

        if(str2uint(&num, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert cur_denoter:%s to uint32 failed(%s).", msg_type, row[2], g_sql_str);
            return ERR_SQL_ERR;
        }

        p_rsp->total_participater += num;
        num = 0;


        if(str2uint(&num, row[3]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert helped_pet:%s to uint32 failed(%s).", msg_type, row[3], g_sql_str);
            return ERR_SQL_ERR;
        }

        p_rsp->helped_pet += num;
        num = 0;

        if(str2uint(&p_rsp->denote[idx].end_time, row[4]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert end_flag:%s to uint32 failed(%s).", msg_type, row[4], g_sql_str);
            return ERR_SQL_ERR;
        }

        idx++;

        row = p_mysql_conn->select_next_row(true);
    }

    p_rsp->history_num = idx;
    g_pack.pack(buffer, sizeof(msg_history_denote_t) + idx * sizeof(history_denote_t));

    return 0;
}


uint32_t denote_coins(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, sizeof(msg_db_denote_request_t)) != 0)
    {
        return ERR_MSG_LEN;
    }

    msg_db_denote_request_t *p_request = (msg_db_denote_request_t*)p_request_body;


    KINFO_LOG(user_id, "denote_id:%u denote_num:%u", p_request->denote_id, p_request->denote_num);

    //仙查看有没有达到募捐上限
    sprintf(g_sql_str, "SELECT sum(denote_num) from db_monster_%d.t_denote_%d where user_id = %u and denote_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_request->denote_id);
    MYSQL_ROW row = NULL;
    KINFO_LOG(user_id, "查看用户已募捐数额:%s", g_sql_str);
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    uint32_t have_denoted = 0;
    uint8_t denote_flag = 0;
    db_msg_denote_response_t denote_res;
    if(row != NULL)
    {
        if(row[0] != NULL)
        {
            if(str2uint(&have_denoted, row[0]) != 0)
            {
                KCRIT_LOG(user_id, "[msg:%u]convert denote_sum %s to uint32 failed(%s).", msg_type, row[0],  p_mysql_conn->get_last_errstr());
                return ERR_SQL_ERR;
            }
        }
    }

    if(have_denoted >= p_request->denote_limit)
    {//已达到募捐上限
        KINFO_LOG(user_id, "user have reached denote limit");
        denote_flag = DENOTE_REACHED_LIMIT;
        denote_res.denote_id = p_request->denote_id;
        denote_res.flag = denote_flag;
        denote_res.denote_num = 0;
        denote_res.reward_id = 0;
        denote_res.new_denote_id = 0;
        g_pack.pack((char*)&denote_res, sizeof(db_msg_denote_response_t));
        return 0;
    }


    sprintf(g_sql_str, "SELECT denote_id, cur_denote_num,  cur_denoter,  end_flag FROM db_monster_config.t_denote order by denote_id desc limit 1;");
    row = NULL;
    result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    KINFO_LOG(user_id, "查看数据库中最近的一个募捐计划:%s", g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    uint32_t denote_id = 0;
    uint32_t cur_denote_num = 0;//当前已经募捐的数额
    uint32_t cur_denoter = 0;//当前参与的人数
    uint32_t end_flag = 0;//是否结束标志
    if(row != NULL)
    {
        if(str2uint(&denote_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert denote_id %s to uint32 failed(%s).", msg_type, row[0],  p_mysql_conn->get_last_errstr());
            return ERR_SQL_ERR;
        }

        if(str2uint(&cur_denote_num, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert cur_denote_num %s to uint32 failed(%s).", msg_type, row[1],  p_mysql_conn->get_last_errstr());
            return ERR_SQL_ERR;
         }

        if(str2uint(&cur_denoter, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert cur_denoter %s to uint32 failed(%s).", msg_type, row[2],  p_mysql_conn->get_last_errstr());
            return ERR_SQL_ERR;
         }

        if(str2uint(&end_flag, row[3]) != 0)
        {
            KCRIT_LOG(user_id, "[msg:%u]convert end_flag %s to uint32 failed(%s).", msg_type, row[3],  p_mysql_conn->get_last_errstr());
            return ERR_SQL_ERR;
       }

        KINFO_LOG(user_id, "最近一期募捐计划：id:%u, 已募捐数额:%u, 参与人数:%u, 结束标志:%u", denote_id, cur_denote_num, cur_denoter, end_flag);
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

        if(denote_id == p_request->denote_id)
        {
            if(end_flag != 0)
            {//募捐已有其他人达成
                KINFO_LOG(user_id, "募捐%u由其他人完成", denote_id);
                denote_flag = DENOTE_FAIL_AND_REACHED;
            }
            else if(cur_denote_num + p_request->denote_num >= p_request->denote_total)
            {//募捐由该用户的本次捐款达成
                KINFO_LOG(user_id, "募捐%u由用户达成", denote_id);
                denote_flag = DENOTE_SUCCESS_AND_REACHED;
                if(have_denoted == 0)
                {//该用户之前没有捐助过，增加总捐助人数
                    sprintf(g_sql_str, "UPDATE db_monster_config.t_denote set cur_denote_num = cur_denote_num + %u, helped_pet = %u, end_flag = %u, cur_denoter = cur_denoter +1 where denote_id = %u;", p_request->denote_num, p_request->helped_pet,  (uint32_t)time(NULL), p_request->denote_id);
                }
                else
                {
                    sprintf(g_sql_str, "UPDATE db_monster_config.t_denote set cur_denote_num = cur_denote_num + %u, helped_pet = %u,  end_flag = %u where denote_id = %u;", p_request->denote_num, p_request->helped_pet, (uint32_t)time(NULL), p_request->denote_id);
                }
            }
            else
            {
                denote_flag = DENOTE_SUCCESS_BUT_NOT_REACHED;
                KINFO_LOG(user_id, "募捐%u尚未达成", denote_id);
                if(have_denoted == 0)
                {//该用户之前没有捐助过，增加总捐助人数
                    sprintf(g_sql_str, "UPDATE db_monster_config.t_denote set cur_denote_num = cur_denote_num + %u,  cur_denoter = cur_denoter +1 where denote_id = %u;", p_request->denote_num, p_request->denote_id);
                }
                else
                {
                    sprintf(g_sql_str, "UPDATE db_monster_config.t_denote set cur_denote_num = cur_denote_num + %u  where denote_id = %u;", p_request->denote_num, p_request->denote_id);
                }

            }

            if(denote_flag != DENOTE_FAIL_AND_REACHED)
            {
                KINFO_LOG(user_id, "更新募捐记录表:%s", g_sql_str);
                if (p_mysql_conn->execsql(g_sql_str) < 0)
                {
                    KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                    mysql_rollback(p_conn);
                    mysql_autocommit(p_conn, true);
                    return ERR_SQL_ERR;
                }

                //添加一条捐助记录
                sprintf(g_sql_str, "insert into db_monster_%d.t_denote_%d(user_id, denote_id, denote_time, denote_num, reward_flag) values(%u, %u, %u, %u,0);", DB_ID(user_id), TABLE_ID(user_id), user_id, p_request->denote_id, (uint32_t)time(NULL), p_request->denote_num);
                KINFO_LOG(user_id, "添加一条捐款记录:%s", g_sql_str);
                if (p_mysql_conn->execsql(g_sql_str) < 0)
                {
                    KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                    mysql_rollback(p_conn);

                    mysql_autocommit(p_conn, true);
                    return ERR_SQL_ERR;
                }

                //扣减金币
                sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set coins = coins - %u where user_id = %u and coins > 0;", DB_ID(user_id), TABLE_ID(user_id), p_request->denote_num, user_id);
                KINFO_LOG(user_id, "扣减金币:%s", g_sql_str);
                if (p_mysql_conn->execsql(g_sql_str) < 0)
                {
                    KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                    mysql_rollback(p_conn);

                    mysql_autocommit(p_conn, true);
                    return ERR_SQL_ERR;
                }

            }

            if(denote_flag != DENOTE_SUCCESS_BUT_NOT_REACHED)
            {//添加奖励
                sprintf(g_sql_str, "INSERT INTO db_monster_%d.t_stuff_%d(user_id, stuff_id, stuff_num) values(%u, %u, 1) ON DUPLICATE KEY UPDATE stuff_num = stuff_num + 1;", DB_ID(user_id), TABLE_ID(user_id), user_id, p_request->reward_id);
                KINFO_LOG(user_id, "添加奖励:%s", g_sql_str);
                if (p_mysql_conn->execsql(g_sql_str) < 0)
                {
                    KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                    mysql_rollback(p_conn);
                    mysql_autocommit(p_conn, true);
                    return ERR_SQL_ERR;
                }

                sprintf(g_sql_str, "UPDATE db_monster_%d.t_denote_%d set reward_flag = 1 where user_id = %u and denote_id = %u;", DB_ID(user_id), TABLE_ID(user_id), user_id, denote_id);
                KINFO_LOG(user_id, "更新奖励:%s", g_sql_str);
                if (p_mysql_conn->execsql(g_sql_str) < 0)
                {
                    KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                    mysql_rollback(p_conn);

                    mysql_autocommit(p_conn, true);
                    return ERR_SQL_ERR;
                }
            }

        }
        else if(p_request->denote_id == denote_id + 1)
        {//新的一期捐助计划
                denote_flag = DENOTE_SUCCESS_BUT_NOT_REACHED;
                sprintf(g_sql_str, "INSERT INTO db_monster_config.t_denote(denote_id, cur_denote_num, total_denote_num, cur_denoter, helped_pet, reward_id, end_flag) values(%u, %u, %u, 1, 0, %u, 0) ON DUPLICATE KEY UPDATE cur_denote_num = cur_denote_num + %u, cur_denoter = cur_denoter + 1;", p_request->denote_id, p_request->denote_num, p_request->denote_total, p_request->reward_id, p_request->denote_num);
                KINFO_LOG(user_id, "新一期捐助:%s", g_sql_str);
                if (p_mysql_conn->execsql(g_sql_str) < 0)
                {
                    KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                    mysql_autocommit(p_conn, true);
                    return ERR_SQL_ERR;
                }
                //添加一条捐助记录
                sprintf(g_sql_str, "insert into db_monster_%d.t_denote_%d(user_id, denote_id, denote_time, denote_num, reward_flag) values(%u, %u, %u, %u,0);", DB_ID(user_id), TABLE_ID(user_id), user_id, p_request->denote_id, (uint32_t)time(NULL), p_request->denote_num);
                KINFO_LOG(user_id, "新一期捐助, 添加捐助记录:%s", g_sql_str);
                if (p_mysql_conn->execsql(g_sql_str) < 0)
                {
                    KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                    mysql_rollback(p_conn);

                    mysql_autocommit(p_conn, true);
                    return ERR_SQL_ERR;
                }
                //扣减金币
                sprintf(g_sql_str, "UPDATE db_monster_%d.t_role_%d set coins = coins - %u where user_id = %u and coins > 0;", DB_ID(user_id), TABLE_ID(user_id), p_request->denote_num, user_id);
                KINFO_LOG(user_id, "扣减金币:%s", g_sql_str);
                if (p_mysql_conn->execsql(g_sql_str) < 0)
                {
                    KCRIT_LOG(user_id, "[msg:%u]sql exec failed(%s).", msg_type, p_mysql_conn->get_last_errstr());
                    mysql_rollback(p_conn);

                    mysql_autocommit(p_conn, true);
                    return ERR_SQL_ERR;
                }

        }
        else
        {
            KCRIT_LOG(user_id, "db_denote_id:%u, client_denote_id:%u", denote_id, p_request->denote_id);
                mysql_autocommit(p_conn, true);
            return ERR_DENOTE_ID;
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

        if(denote_flag == DENOTE_SUCCESS_BUT_NOT_REACHED)
        {
            denote_res.denote_id = p_request->denote_id;
            denote_res.flag = denote_flag;
            denote_res.denote_num = p_request->denote_num;
            denote_res.reward_id = 0;
            denote_res.new_denote_id = 0;
        }
        else if(denote_flag == DENOTE_SUCCESS_AND_REACHED)
        {
            denote_res.denote_id = p_request->denote_id;
            denote_res.flag = denote_flag;
            denote_res.denote_num = p_request->denote_num;
            denote_res.reward_id = p_request->reward_id;
            denote_res.new_denote_id = denote_id + 1;
        }
        else if(denote_flag == DENOTE_FAIL_AND_REACHED)
        {
            denote_res.denote_id = p_request->denote_id;
            denote_res.flag = denote_flag;
            denote_res.denote_num = 0;
            denote_res.reward_id = p_request->reward_id;
            denote_res.new_denote_id = denote_id + 1;
        }

        g_pack.pack((char*)&denote_res, sizeof(db_msg_denote_response_t));
    }
    else//row == NULL
    {
        KCRIT_LOG(user_id, "system error ,no no no, %s", g_sql_str);
        return ERR_SQL_ERR;
    }

        return 0;
}
*/
