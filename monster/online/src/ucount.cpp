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
#include "ucount.h"
#include "function.h"
#include "data.h"
#include "stat.h"
#include "utility.h"

void handle_ucount_return(svr_msg_header_t *p_ucount_pkg, int len)
{
    if (0 == p_ucount_pkg->seq_id)
    {
        ERROR_LOG("err ucount pkg, seq:0");
        return;
    }

    int ret = 0;
    g_errno = ERR_NO_ERR;

    uint32_t counter = p_ucount_pkg->seq_id & 0xFFFF;
    uint32_t conn_fd  = p_ucount_pkg->seq_id >> 16;
    usr_info_t *p_user = g_user_mng.get_user(conn_fd);
    if(NULL == p_user || counter != p_user->counter || (int)p_ucount_pkg->len != len)
    {
        ERROR_LOG("CONNECTION HAD BEEN CLOSED OR UCOUNT TIMEOUT\t[uid=%u cmd=%X %u cnt=%d %d]",
                p_user ? p_user->uid : 0, p_ucount_pkg->msg_type, p_user ? p_user->waitcmd : 0, counter, p_user ? p_user->counter : 0);
        return;
    }

    KDEBUG_LOG(p_user->uid, "UCOUNT R\t[uid=%u fd=%d cmd=0x%X ret=%u]", p_ucount_pkg->user_id, conn_fd, p_ucount_pkg->msg_type, p_ucount_pkg->result);

    REMOVE_TIMERS(p_user);

    switch (p_ucount_pkg->msg_type)
    {
        case svr_msg_ucount_visit:
            ret = handle_visit_user_room_return(p_user, p_ucount_pkg);
            break;
        case svr_msg_ucount_thumb:
            ret = handle_thumb_user_room_return(p_user, p_ucount_pkg);
            break;
        case svr_msg_ucount_latest_visit:
            ret = handle_latest_visit_return(p_user, p_ucount_pkg);
            break;
        case svr_msg_get_unread_count:
            ret = handle_unread_count_return(p_user, p_ucount_pkg);
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

int handle_thumb_user_room_return(usr_info_t *p_user, svr_msg_header_t *p_ucount_pkg)
{
    assert(p_user != NULL && p_ucount_pkg != NULL);
    if (p_ucount_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "add user thumb failed:%u", p_ucount_pkg->result);
        g_errno = p_ucount_pkg->result;
        return 0;
    }

    as_msg_thumb_user_req_t *p_req = (as_msg_thumb_user_req_t *)p_user->buffer;

    int idx = 0;
    uint8_t flag = 0;
    taomee::unpack_h(p_ucount_pkg->body, flag, idx);

    if (flag)
    {
        KDEBUG_LOG(p_user->uid, "thumb user:%u before today.", p_req->user_id);
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack((uint8_t)THUMB_ROOM_BEFORE);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        return 0;
    }

    //统计赞的人数人次
    stat_two_param_t stat_data = {p_user->uid, 1};
    msg_log(stat_thumb_count, &stat_data, sizeof(stat_data));

    //通知switch用户有新的最近访问记录
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_switch_friend_apply, p_user->uid, GET_SVR_SN(p_user), sizeof(online_notify_t));

    online_notify_t *p_body = (online_notify_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->user_id = p_req->user_id;
    p_body->type = NEW_VISIT;
    KINFO_LOG(p_user->uid, "%u has new thumb, notify switch", p_ucount_pkg->user_id);

    g_p_switch_svr->send_data(NULL, g_send_buffer, p_header->len, NULL);

    //增加db里面的顶的次数
    pack_svr_pkg_head(p_header, svr_msg_db_add_role_value, p_req->user_id, GET_SVR_SN(p_user), sizeof(db_msg_update_role_req_t) + sizeof(field_t));

    db_msg_update_role_req_t *p_new_body = (db_msg_update_role_req_t *)p_body;
    memset(p_body, 0, sizeof(db_msg_update_role_req_t));
    p_new_body->type = DB_ADD_ROLE;
    p_new_body->count = 1;
    p_new_body->field[0].type = FIELD_THUMB;
    p_new_body->field[0].value = 1;

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        g_errno = ERR_MSG_DB_PROXY;
        return 0;
    }

    return 0;
}

int handle_visit_user_room_return(usr_info_t *p_user, svr_msg_header_t *p_ucount_pkg)
{
    assert(p_user != NULL && p_ucount_pkg != NULL);
    if (p_ucount_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "get visit user:%u room failed:%u", p_ucount_pkg->user_id, p_ucount_pkg->result);
        return 0;
    }

    //通知switch有新的访客记录
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_switch_friend_apply, p_user->uid, GET_SVR_SN(p_user), sizeof(online_notify_t));

    online_notify_t *p_body = (online_notify_t *)(g_send_buffer + sizeof(svr_msg_header_t));
    p_body->user_id = p_ucount_pkg->user_id;
    p_body->type = NEW_VISIT;
    KINFO_LOG(p_user->uid, "%u has new visit, notify switch", p_ucount_pkg->user_id);

    g_p_switch_svr->send_data(NULL, g_send_buffer, p_header->len, NULL);

    int idx = 0;
    uint8_t flag = 0;
    taomee::unpack_h(p_ucount_pkg->body, flag, idx);

    if (flag)
    {
        p_user->waitcmd = 0;
        return 0;
    }

    pack_svr_pkg_head(p_header, svr_msg_db_add_role_value, p_ucount_pkg->user_id, GET_SVR_SN(p_user), sizeof(db_msg_update_role_req_t) + sizeof(field_t));

    db_msg_update_role_req_t *p_new_body = (db_msg_update_role_req_t *)p_body;
    memset(p_body, 0, sizeof(db_msg_update_role_req_t));
    p_new_body->type = DB_ADD_ROLE;
    p_new_body->count = 1;
    p_new_body->field[0].type = FIELD_VISITS;
    p_new_body->field[0].value = 1;

    if (g_p_db_proxy_svr->send_data(NULL, g_send_buffer, p_header->len, NULL) != 0)
    {
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }
    KINFO_LOG(p_user->uid, "add user:%u visit", p_ucount_pkg->user_id);

    return 0;
}

int handle_latest_visit_return(usr_info_t *p_user, svr_msg_header_t *p_ucount_pkg)
{
    assert(p_user != NULL && p_ucount_pkg != NULL);
    if (p_ucount_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "get visit user:%u room failed:%u", p_ucount_pkg->user_id, p_ucount_pkg->result);
        g_errno = p_ucount_pkg->result;
        return 0;
    }

    visit_id_t *p_visit = (visit_id_t *)p_ucount_pkg->body;

    if (p_visit->count == 0)
    {
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack((uint16_t)p_visit->count);
        KINFO_LOG(p_user->uid, "get 0 latest visit");
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

        return 0;
    }

    latest_visit_t *p_latest_visit = (latest_visit_t *)p_user->buffer;
    p_latest_visit->count = p_visit->count;
    p_latest_visit->cur_index = 0;
    for (int i = 0; i != p_visit->count; ++i)
    {
        p_latest_visit->visit_info[i].user_id = p_visit->visit_info[i].user_id;
        p_latest_visit->visit_info[i].timestamp = p_visit->visit_info[i].timestamp;
        p_latest_visit->visit_info[i].type = p_visit->visit_info[i].type;
        p_latest_visit->visit_info[i].status = p_visit->visit_info[i].status;
    }

    //向db请求第一个用户的信息
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_friend_info, p_latest_visit->visit_info[0].user_id, GET_SVR_SN(p_user), 0);
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy failed.");
        return 0;
    }

    return 0;
}

int handle_unread_count_return(usr_info_t *p_user, svr_msg_header_t *p_ucount_pkg)
{
    assert(p_user != NULL && p_ucount_pkg != NULL);

    uint16_t unread_count = 0;
    if (p_ucount_pkg->result != 0)
    {
        KCRIT_LOG(p_user->uid, "get unread count failed:%u", p_ucount_pkg->result);
        unread_count = 0;
    }
    else
    {
        KINFO_LOG(p_user->uid, "get unread count:%u", *(uint8_t *)p_ucount_pkg->body);
        unread_count = *(uint8_t *)p_ucount_pkg->body;
    }

    switch (p_user->waitcmd)
    {
        case as_msg_get_unread_visit:   //获取用户未读访客数量
            {
                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                g_send_msg.pack(unread_count);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                break;
            }

        case as_msg_login_online:
            {
                //通知分享服务
                role_cache_t *p_role = p_user->user_cache.get_role();
                if (NULL == p_role)
                {
                    KCRIT_LOG(p_user->uid, "get user role");
                    return -1;
                }
                p_role->unread_visits = unread_count;

                if(g_user_index++ > 100)
                {
                    pack_as_pkg_header(p_user->uid, online_msg_login_user, 0, ERR_NO_ERR);
                    g_send_msg.pack(p_role->role.gender);
                    g_send_msg.pack(p_role->role.name, sizeof(p_role->role.name));
                    g_send_msg.end();
                    g_p_share_svr->send_data(NULL, g_send_msg.get_msg(), g_send_msg.get_msg_len(), NULL);
                    g_user_index = 0;
                }

                //通知switch有用户登录
                svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
                pack_svr_pkg_head(p_header, svr_msg_login, p_user->uid, GET_SVR_SN(p_user), 0);
                //没有返回包，不需要加定时器
                g_p_switch_svr->send_data(NULL, g_send_buffer, p_header->len, NULL);

                pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
                uint32_t seq = uniform_rand(1, 10000);
                g_send_msg.pack(seq);
                p_user->seq = seq;
                g_send_msg.pack((uint8_t)1);
				g_send_msg.pack(p_role->role.guide_flag);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
                KINFO_LOG(p_user->uid, "login success");

                break;
            }
        default:
            KCRIT_LOG(p_user->uid, "waitcmd:%u is invalid", p_user->waitcmd);
            return -1;

    }

    return 0;
}
