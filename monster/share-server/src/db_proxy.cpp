/**
 * =====================================================================================
 *       @file  db_proxy.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/30/2011 02:27:31 PM
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
#include <stdio.h>
#include <map>

#include <libtaomee++/inet/pdumanip.hpp>
#include "message.h"
#include "stat.h"

#include "data.h"
#include "function.h"
#include "db_proxy.h"
#include "utility.h"

using std::map;

void init_db_handle_funs()
{
    g_db_cmd_map.clear();

    g_db_cmd_map[svr_msg_db_room_info] = process_db_get_room_info_return;                       //32774---0x8006
    g_db_cmd_map[svr_msg_db_other_role_info] = process_db_other_role_info_return;               //32800---0x8020

    return;
}

void handle_db_return(svr_msg_header_t *p_db_pkg, int len)
{
    g_errno = ERR_NO_ERR;

    uint32_t counter = p_db_pkg->seq_id & 0xFFFF;
    uint32_t conn_fd  = p_db_pkg->seq_id >> 16;
    usr_info_t *p_user = g_user_mng.get_user(conn_fd);
    if (NULL == p_user || counter != p_user->counter || (int)p_db_pkg->len != len)
    {
        ERROR_LOG("CONNECTION HAD BEEN CLOSED OR DB TIMEOUT\t[cmd=%X %u cnt=%d %d uid=%u]", p_db_pkg->msg_type, p_user ? p_user->waitcmd : 0, counter, p_user ? p_user->counter : 0, p_user ? p_user->uid : 0);
        return;
    }

    DEBUG_LOG("DB R\t[uid=%u fd=%d cmd=0x%X ret=%u]", p_db_pkg->user_id, conn_fd, p_db_pkg->msg_type, p_db_pkg->result);

    REMOVE_TIMERS(p_user);

    map<uint16_t, DB_PROCESS_FUNC>::iterator cmd_iter = g_db_cmd_map.find(p_db_pkg->msg_type);
    if (cmd_iter == g_db_cmd_map.end())    //未知的协议
    {
        ERROR_LOG("[DB MSG:%u] invalid, seq:%u", p_db_pkg->msg_type, p_db_pkg->seq_id);
        close_client_conn(conn_fd);
    }
    else if (cmd_iter->second(p_user, p_db_pkg) != 0)
    {
        CRIT_LOG("[DB MSG:%u] process failed, seq:%u", p_db_pkg->msg_type, p_db_pkg->seq_id);
        close_client_conn(p_user->session->fd);
        return;
    }
    else
    {
        // do nothing
    }

    if (g_errno != ERR_NO_ERR)
    {
        send_header_to_user(p_user, SYSTEM_ERR_NUM, g_errno);
    }

    return;
}

int process_db_get_room_info_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        CRIT_LOG("db get room failed:%u", p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    as_msg_get_room_req_t *p_req = &p_user->buffer;
    if (p_req->user_id != p_db_pkg->user_id)
    {
        CRIT_LOG("ret user:%u is not %u", p_db_pkg->user_id, p_req->user_id);
        return -1;
    }
    int len = p_db_pkg->len - sizeof(svr_msg_header_t);

    INFO_LOG("get user:%u room:%u from db", p_req->user_id, p_req->room_id);

    pack_as_pkg_header(0, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_db_pkg->body, len);
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}

int process_db_other_role_info_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg)
{
    assert(p_user != NULL && p_db_pkg != NULL);
    if (p_db_pkg->result != 0)
    {
        CRIT_LOG("get role:%u info failed:%u", p_db_pkg->user_id, p_db_pkg->result);
        g_errno = p_db_pkg->result;
        return 0;
    }

    const as_msg_register_rsp_t *p_db_rsp = (const as_msg_register_rsp_t *)p_db_pkg->body;
    pack_as_pkg_header(0, p_user->waitcmd, 0, ERR_NO_ERR);
    time_t now = time(NULL);
    uint32_t mon_happy = get_new_happy(now, p_db_rsp->role.last_login_time, p_db_rsp->role.monster_happy);
    uint32_t mon_health = get_other_monster_health(now, p_db_rsp->role.last_login_time, p_db_rsp->role.last_logout_time, p_db_rsp->role.monster_health);


    g_send_msg.pack((uint8_t)1);
    g_send_msg.pack(p_db_rsp->role.name, sizeof(p_db_rsp->role.name));
    g_send_msg.pack(p_db_rsp->role.gender);
    g_send_msg.pack(p_db_rsp->role.country_id);
    g_send_msg.pack(p_db_rsp->role.user_type);
    g_send_msg.pack(p_db_rsp->role.birthday);
    g_send_msg.pack(p_db_rsp->role.mood);
    g_send_msg.pack(p_db_rsp->role.fav_color);
    g_send_msg.pack(p_db_rsp->role.fav_pet);
    g_send_msg.pack(p_db_rsp->role.fav_fruit);
    g_send_msg.pack(p_db_rsp->role.personal_sign, sizeof(p_db_rsp->role.personal_sign));
    g_send_msg.pack(p_db_rsp->role.room_num);
    g_send_msg.pack(p_db_rsp->role.pet_num);
    g_send_msg.pack(p_db_rsp->role.max_puzzle_score);
    g_send_msg.pack(p_db_rsp->role.register_time);
    g_send_msg.pack(p_db_rsp->role.coins);
    g_send_msg.pack(p_db_rsp->role.last_login_time);
    g_send_msg.pack(p_db_rsp->role.monster_id);
    g_send_msg.pack(p_db_rsp->role.monster_name, sizeof(p_db_rsp->role.monster_name));
    g_send_msg.pack(p_db_rsp->role.monster_main_color);
    g_send_msg.pack(p_db_rsp->role.monster_ex_color);
    g_send_msg.pack(p_db_rsp->role.monster_eye_color);
    g_send_msg.pack(p_db_rsp->role.monster_exp);
    g_send_msg.pack(p_db_rsp->role.monster_level);
    g_send_msg.pack(mon_health);
    g_send_msg.pack(mon_happy);
    g_send_msg.pack(p_db_rsp->role.approved_message_num);
    g_send_msg.pack(p_db_rsp->role.unapproved_message_num);
    g_send_msg.pack(p_db_rsp->role.friend_num);
    g_send_msg.pack((uint8_t)(p_db_rsp->pending_req_num != 0 ? 1 : 0));
    g_send_msg.pack(p_db_rsp->role.thumb);
    g_send_msg.pack(p_db_rsp->role.visits);
    g_send_msg.pack(p_db_rsp->role.recent_unread_badge);
    g_send_msg.pack(p_db_rsp->role.npc_score);//打包npc评分


    int idx = sizeof(as_msg_register_rsp_t);
    uint16_t pet_count = 0;
    taomee::unpack_h(p_db_rsp, pet_count, idx);
    g_send_msg.pack((uint8_t)pet_count);
    for (int i = 0; i != pet_count; ++i)
    {
       // uint32_t id = 0;
       // taomee::unpack_h(p_db_rsp, id, idx);
        uint32_t pet_id = 0;
        taomee::unpack_h(p_db_rsp, pet_id, idx);
        g_send_msg.pack(pet_id);
	KINFO_LOG(p_user->uid, "pet_id:%u", pet_id);
    }
    g_send_msg.pack((uint16_t)0);//最近访客数
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    return 0;
}
