/**
 * =====================================================================================
 *       @file  function.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/21/2011 01:47:48 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <assert.h>
#include <map>
#include <vector>
#include <set>
#include <stdio.h>

#include <libtaomee++/inet/pdumanip.hpp>
#include "message.h"
#include "stat.h"

#include "function.h"
#include "data.h"
#include "utility.h"

using std::map;
using std::vector;
using std::pair;
using std::set;

void pack_as_pkg_header(uint32_t user_id, uint16_t msg_type, uint32_t result, uint32_t err_no)
{
    g_send_msg.begin(SEND_TO_AS);
    g_send_msg.pack(msg_type);
    g_send_msg.pack(user_id);
    g_send_msg.pack(result);
    g_send_msg.pack(err_no);
};

void send_header_to_user(usr_info_t *p_user, uint32_t result, uint32_t err_no, bool clear_waitcmd)  
{
    if (NULL == p_user)
    {
        return;
    }
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, result, err_no);
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), clear_waitcmd);
}

void send_data_to_user(usr_info_t *p_user, const char *p_buf, uint32_t buf_len, bool clear_waitcmd)
{
    if (NULL == p_user)
    {
        return; 
    }
    if (clear_waitcmd)
    {
        p_user->waitcmd = 0;
    }
    p_user->inc_counter();
    int ret = send_pkg_to_client(p_user->session, p_buf, buf_len);
    if (ret != 0)
    {
        close_client_conn(p_user->session->fd);
    }
}

void pack_svr_pkg_head(svr_msg_header_t *p_head, uint16_t msg_type, uint32_t user_id, uint32_t seq_id, uint32_t body_len)
{
    p_head->len = body_len + sizeof(svr_msg_header_t); 
    p_head->seq_id = seq_id;
    p_head->user_id = user_id;
    p_head->msg_type = msg_type;
    p_head->result = 0; 
}

int timeout_cb(void *p_owner, void *p_data)
{
    assert(p_owner != NULL);
    usr_info_t *p_user  = (usr_info_t *)p_owner;  

    send_header_to_user(p_user, SYSTEM_ERR_NUM, ERR_MSG_TIMEOUT);  

    return 0;
}

uint32_t get_new_happy(uint32_t now_time, uint32_t last_time, uint32_t last_happy)
{
    int past_time = now_time - last_time;
    if (past_time < 0)
    {
        CRIT_LOG("it cann't be here");
        past_time = 0; 
    }
    uint32_t happy_desc = past_time / (g_happy_desc_time * 60);

    return happy_desc >= last_happy ? 0 : last_happy - happy_desc;
}

/**
 * @brief 计算现在实际的健康值
 *
 * @return 无
 */
static int get_now_health(int health, uint32_t last_update_health_time, uint32_t now)
{
    uint32_t interval = now - last_update_health_time;
    if (health <= HEALTH_LOW)
    {
        health = health - interval / (g_health_desc_slow * 60);
        if (health < HEALTH_MIN)
        {
            health = HEALTH_MIN;
        }
    }
    else if (health <= HEALTH_MIDDLE)
    {
        int temp_health = health -  interval / (g_health_desc_middle * 60);
        if (temp_health >= HEALTH_LOW)
        {
            health = temp_health;
        }
        else
        {
            uint32_t left_time = interval - (health - HEALTH_LOW) / (g_health_desc_middle * 60);
            health = HEALTH_LOW - left_time / 60;
            if (health < HEALTH_MIN)
            {
                health = HEALTH_MIN;
            }
        }
    }
    else
    {
        int temp_health = health - interval / (g_health_desc_high * 60);
        if (temp_health >= HEALTH_MIDDLE)
        {
            health = temp_health;
        }
        else
        {
            uint32_t left_time = interval - (health - HEALTH_MIDDLE) / (g_health_desc_high * 60);
            health = HEALTH_MIDDLE - left_time / (60 * g_health_desc_middle);
            if (health < HEALTH_LOW)
            {
                left_time = left_time - (HEALTH_MIDDLE - HEALTH_LOW) / (60 * g_health_desc_middle);
                health = health - left_time / (60 * g_health_desc_slow);
                if (health < HEALTH_MIN)
                {
                    health = HEALTH_MIN;
                }
            }
        }
    }

    return health;
}

int get_other_monster_health(uint32_t now, uint32_t last_logout_time, uint32_t last_login_time, int last_health)
{
    if (last_logout_time > last_login_time)
    {   
        last_health = last_health - (now - last_logout_time) / (g_health_desc_offline * 60);
    }   
    else
    {   
        if (last_logout_time != 0)
        {   
            last_health = last_health - (last_login_time - last_logout_time) / (g_health_desc_offline * 60);
        }   

        last_health = get_now_health(last_health, last_login_time, now);
    }   

    if (last_health < HEALTH_MIN)
    {   
        last_health = HEALTH_MIN;
    }   

    return last_health;
}



