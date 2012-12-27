/*
 * =====================================================================================
 *
 *       Filename:  dispatch.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月11日 21时05分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */

extern "C"
{
#include <libtaomee/log.h>
}
#include "switch.h"
#include "online.h"
#include "shop.h"
#include "everyday_fight.h"

#include "../../common/message.h"
#include "../../common/constant.h"
#include "../../common/data_structure.h"

extern c_online g_online;
extern c_shop g_shop;
extern c_everyday_fight g_ed_fight;

int dispatch(void *p_data, int len, fdsession_t *fdsess)
{
    svr_msg_header_t *p_recv_msg = (svr_msg_header_t*)p_data;


    uint16_t msg_type = p_recv_msg->msg_type;
    if((int)p_recv_msg->len > len)
    {
        ERROR_LOG("msg_len error,type:%u, len in header:%u , recved_len:%d.", msg_type, p_recv_msg->len, len);
        return -1;
    }

    int body_len = p_recv_msg->len - sizeof(svr_msg_header_t);
    switch(msg_type)
    {
        case svr_msg_acquire_online:
            return g_online.acquire_available_online(p_recv_msg, body_len, fdsess);
        case svr_msg_online_register:
            return g_online.register_online(p_recv_msg, body_len, fdsess);
        case svr_msg_login:
            return g_online.login_online(p_recv_msg, body_len, fdsess);
        case svr_msg_logout:
            return g_online.logout_online(p_recv_msg, body_len, fdsess);
        case svr_msg_switch_friend_apply:
            return g_online.add_friend_request(p_recv_msg, body_len, fdsess);
        case svr_msg_query_switch_status:
            return g_online.query_switch_status(p_recv_msg, body_len, fdsess);
        case svr_msg_query_ip:
            return g_online.query_ip_info(p_recv_msg, body_len, fdsess);
        case svr_msg_query_user:
            return g_online.query_user_info(p_recv_msg, body_len, fdsess);
        case svr_msg_get_active_user:
            return g_online.get_active_user(p_recv_msg, body_len, fdsess);
        case svr_msg_get_shop_item:
            return g_shop.get_shop_item(p_recv_msg, body_len, fdsess);
        case svr_msg_online_exit:
            return g_online.clear_register_online(p_recv_msg, body_len, fdsess);
        case svr_msg_online_keepalive:
            return g_online.make_online_keepalive(p_recv_msg, body_len, fdsess);
        case svr_msg_online_encourage:
            return g_ed_fight.encourage_guide(p_recv_msg, body_len, fdsess);
        default:
            ERROR_LOG("invalid msg_type=%u from fd=%u.", msg_type, fdsess->fd);
            return -1;
    }

    return 0;
}
