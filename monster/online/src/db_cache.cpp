/**
 * =====================================================================================
 *       @file  ucount.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/30/2011 01:42:31 PM
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
#include "message.h"
#include <libtaomee++/inet/pdumanip.hpp>
#include "db_cache.h"
#include "function.h"
#include "data.h"
#include "stat.h"
#include "utility.h"

void handle_cache_svr_return(svr_msg_header_t *p_cache_pkg, int len)
{
    if (0 == p_cache_pkg->seq_id)
    {
        ERROR_LOG("err cache pkg, seq:0");
        return;
    }

    int ret = 0;
    g_errno = ERR_NO_ERR;

    uint32_t counter = p_cache_pkg->seq_id & 0xFFFF;
    uint32_t conn_fd  = p_cache_pkg->seq_id >> 16;
    usr_info_t *p_user = g_user_mng.get_user(conn_fd);
    if(NULL == p_user || counter != p_user->counter || (int)p_cache_pkg->len != len)
    {
        ERROR_LOG("CONNECTION HAD BEEN CLOSED OR cache_svr TIMEOUT\t[uid=%u cmd=%X %u cnt=%d %d]",
                p_user ? p_user->uid : 0, p_cache_pkg->msg_type, p_user ? p_user->waitcmd : 0, counter, p_user ? p_user->counter : 0);
        return;
    }

    KDEBUG_LOG(p_user->uid, "CACHE R\t[uid=%u fd=%d cmd=0x%X ret=%u]", p_cache_pkg->user_id, conn_fd, p_cache_pkg->msg_type, p_cache_pkg->result);

    REMOVE_TIMERS(p_user);

    switch (p_cache_pkg->msg_type)
    {
        case svr_msg_db_get_pinboard_info:
            ret = handle_get_pinboard_return(p_user, p_cache_pkg);
            break;
        case svr_msg_join_show:
            ret = handle_join_show_return(p_user, p_cache_pkg);
            break;
        case svr_msg_vote_show:
            ret = handle_vote_show_return(p_user, p_cache_pkg);
            break;
        case svr_msg_history_show:
            ret = handle_history_show_return(p_user, p_cache_pkg);
            break;
        case svr_msg_enter_show:
            ret = handle_enter_show_return(p_user, p_cache_pkg);
            break;
        default:
            close_client_conn(conn_fd);
            return;
    }

    if (ret != 0)
    {
        close_client_conn(p_user->session->fd);
    }
    if (g_errno != ERR_NO_ERR)
    {
        send_header_to_user(p_user, SYSTEM_ERR_NUM, g_errno);
    }

    return;
}


int handle_enter_show_return(usr_info_t *p_user, svr_msg_header_t *p_cache_pkg)
{
    assert(p_user != NULL && p_cache_pkg != NULL);

    if (p_cache_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "enter show failed:%u", p_cache_pkg->result);
        g_errno = p_cache_pkg->result;
        return 0;
    }

    msg_enter_show_return_t *p_mesrt = (msg_enter_show_return_t*)p_cache_pkg->body;
    msg_next_of_enter_t *p_mnoet = (msg_next_of_enter_t*)(p_cache_pkg->body + sizeof(msg_enter_show_return_t) + p_mesrt->user_num.user_num * sizeof(show_user_info_t));
    if(p_mnoet->is_notice == 0)
    {//未通知
       // 告知db给用户加金币
        show_reward_t *p_srt = (show_reward_t*)(p_mnoet + 1);
        svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
        db_msg_update_role_req_t *p_body = (db_msg_update_role_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

        pack_svr_pkg_head(p_header, svr_msg_db_add_role_value, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_update_role_req_t) + sizeof(field_t));

        memset(p_body, 0, sizeof(db_msg_update_role_req_t));
         p_body->type = DB_ADD_ROLE;
        p_body->count = 1;
        p_body->field[0].type = FIELD_COIN;
         p_body->field[0].value = get_current_gains(p_srt->reward_coins, p_user);
         p_srt->reward_coins = p_body->field[0].value;


        if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
        {
            KCRIT_LOG(p_user->uid, "send to db proxy failed.");
            g_errno = ERR_MSG_DB_PROXY;
            return 0;
        }
        memcpy(p_user->buffer, p_cache_pkg->body, p_cache_pkg->len - sizeof(svr_msg_header_t));

    }
    else
    {//已通知

        KINFO_LOG(p_user->uid, "have noticed: show_id:%u show_status:%u user_num:%u total_num:%u is_join:%u", p_mesrt->show_id, p_mesrt->show_status, p_mesrt->user_num.user_num, p_mesrt->user_num.total_num,  p_mnoet->is_join);
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(p_mesrt->show_id);
        g_send_msg.pack(p_mesrt->show_status);
        g_send_msg.pack(p_mesrt->user_num.total_num);
        g_send_msg.pack(p_mesrt->user_num.user_num);
        for(uint8_t idx = 0; idx < p_mesrt->user_num.user_num; idx++)
        {
        g_send_msg.pack(p_mesrt->user_info[idx].user_id);
        g_send_msg.pack(p_mesrt->user_info[idx].user_name, 16);
        g_send_msg.pack(p_mesrt->user_info[idx].mon_id);
        g_send_msg.pack(p_mesrt->user_info[idx].mon_main_color);
        g_send_msg.pack(p_mesrt->user_info[idx].mon_exp_color);
        g_send_msg.pack(p_mesrt->user_info[idx].mon_eye_color);
        g_send_msg.pack(p_mesrt->user_info[idx].npc_score);
        g_send_msg.pack(p_mesrt->user_info[idx].npc_timestamp);
        g_send_msg.pack(p_mesrt->user_info[idx].votes);
        g_send_msg.pack(p_mesrt->user_info[idx].is_top);
        }
        g_send_msg.pack(p_mnoet->is_join);
        g_send_msg.pack((uint8_t)1);



        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    }

    return 0;

}

int handle_history_show_return(usr_info_t *p_user, svr_msg_header_t *p_cache_pkg)
{
    assert(p_user != NULL && p_cache_pkg != NULL);

    if (p_cache_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "join show failed:%u", p_cache_pkg->result);
        g_errno = p_cache_pkg->result;
        return 0;
    }


    msg_history_show_t *p_mhst = (msg_history_show_t*)p_cache_pkg->body;
    KINFO_LOG(p_user->uid, "history show id %u user_num %u total_num %u", p_mhst->show_id, p_mhst->user_num.user_num, p_mhst->user_num.total_num);
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_mhst->show_id);
    g_send_msg.pack(p_mhst->user_num.total_num);
    g_send_msg.pack(p_mhst->user_num.user_num);
    for(uint8_t idx = 0; idx < p_mhst->user_num.user_num; idx++)
    {
        g_send_msg.pack(p_mhst->user_info[idx].user_id);
        g_send_msg.pack(p_mhst->user_info[idx].user_name, 16);
        g_send_msg.pack(p_mhst->user_info[idx].mon_id);
        g_send_msg.pack(p_mhst->user_info[idx].mon_main_color);
        g_send_msg.pack(p_mhst->user_info[idx].mon_exp_color);
        g_send_msg.pack(p_mhst->user_info[idx].mon_eye_color);
        g_send_msg.pack(p_mhst->user_info[idx].npc_score);
        g_send_msg.pack(p_mhst->user_info[idx].npc_timestamp);
        g_send_msg.pack(p_mhst->user_info[idx].votes);
        g_send_msg.pack(p_mhst->user_info[idx].is_top);
        KINFO_LOG(p_user->uid, "idx:%u user_id:%u user_name:%s, npc_score:%u votes:%u is_top:%u", idx, p_mhst->user_info[idx].user_id, p_mhst->user_info[idx].user_name, p_mhst->user_info[idx].npc_score, p_mhst->user_info[idx].votes, p_mhst->user_info[idx].is_top);
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    return 0;
}

int handle_join_show_return(usr_info_t *p_user, svr_msg_header_t *p_cache_pkg)
{
    assert(p_user != NULL && p_cache_pkg != NULL);

    if (p_cache_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "join show failed:%u", p_cache_pkg->result);
        g_errno = p_cache_pkg->result;
        return 0;
    }

    uint32_t show_id = *(uint32_t*)p_cache_pkg->body;
    svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_update_role, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_update_login_t) + sizeof(field_t));

    db_msg_update_login_t *p_body = (db_msg_update_login_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->count = 1;
    p_body->field[0].type = FIELD_LAST_SHOW_ID;
    p_body->field[0].value = show_id;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    memcpy(p_user->buffer, p_body, sizeof(db_msg_update_login_t) + sizeof(field_t));

    return 0;
}

int handle_vote_show_return(usr_info_t *p_user, svr_msg_header_t *p_cache_pkg)
{
    assert(p_user != NULL && p_cache_pkg != NULL);

    if (p_cache_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "vote show failed:%u", p_cache_pkg->result);
        g_errno = p_cache_pkg->result;
    	return 0;
	}

    uint32_t show_id = *(uint32_t*)p_cache_pkg->body;

    svr_msg_header_t *p_head = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_head, svr_msg_db_update_role, p_user->uid, GET_SVR_SN(p_user), sizeof(db_msg_update_login_t) + sizeof(field_t));

    db_msg_update_login_t *p_body = (db_msg_update_login_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->count = 1;
    p_body->field[0].type = FIELD_LAST_SHOW_ID;
    p_body->field[0].value = show_id;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_head->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    memcpy(p_user->buffer, p_body, sizeof(db_msg_update_login_t) + sizeof(field_t));

    return 0;
}

int handle_get_pinboard_return(usr_info_t *p_user, svr_msg_header_t *p_cache_pkg)
{
    assert(p_user != NULL && p_cache_pkg != NULL);

    if (p_cache_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "get pinboard failed:%u", p_cache_pkg->result);
		g_errno = p_cache_pkg->result;
		return 0;
    }

    uint8_t msg_count = 0;
    int idx = 0;
    taomee::unpack_h(p_cache_pkg->body, msg_count, idx);
    pinboard_t *p_pinboard = (pinboard_t *)(p_user->buffer + sizeof(pinboard_count_t));
    p_pinboard->count = msg_count;

    for (int i = 0; i < (int)msg_count; ++i)
    {
        taomee::unpack_h(p_cache_pkg->body, p_pinboard->message[i].message.id, idx);
        taomee::unpack_h(p_cache_pkg->body, p_pinboard->message[i].message.peer_id, idx);
        taomee::unpack_h(p_cache_pkg->body, p_pinboard->message[i].message.icon, idx);
        taomee::unpack_h(p_cache_pkg->body, p_pinboard->message[i].message.color, idx);
        taomee::unpack_h(p_cache_pkg->body, p_pinboard->message[i].message.status, idx);
        taomee::unpack_h(p_cache_pkg->body, p_pinboard->message[i].message.create_time, idx);
        taomee::unpack_h(p_cache_pkg->body, p_pinboard->message[i].message.message_count, idx);
        taomee::unpack(p_cache_pkg->body, p_pinboard->message[i].message.message, p_pinboard->message[i].message.message_count, idx);
    }

    KINFO_LOG(p_user->uid, "get %u message from db cache svr", msg_count);
    if(msg_count == 0)
    {
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack(msg_count);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        return 0;
    }

    //向db请求第一个用户的信息
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_pinboard->message[0].message.peer_id, GET_SVR_SN(p_user), 0);
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    return 0;
}
