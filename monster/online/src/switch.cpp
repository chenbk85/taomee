/*
 * =====================================================================================
 *
 *       Filename:  switch_henry.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年12月26日 17时06分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */

#include <assert.h>
#include <libtaomee++/inet/pdumanip.hpp>
#include "data.h"
#include "function.h"
#include "message.h"
#include "cli_proto.h"
#include "utility.h"
#include "switch.h"


void handle_switch_return(svr_msg_header_t *p_data, int len)
{
    assert(p_data != NULL);
    uint16_t msg_type = p_data->msg_type;

#ifdef DEBUG
    KDEBUG_LOG(p_data->user_id, "recv msg %u from switch.", msg_type);
#endif

    fdsession_t *fdsess = g_user_mng.get_fd(p_data->user_id);
    if(fdsess == NULL)
    {
        KCRIT_LOG(p_data->user_id, "User is not online now.");
        return;
    }

    usr_info_t *p_user = g_user_mng.get_user(fdsess->fd);
    if(p_user == NULL)
    {
        KCRIT_LOG(p_data->user_id, "User is not cache now.");
        return;
    }

    if((int)p_data->len != len)
    {
        KCRIT_LOG(p_data->user_id, "msg_len from switch is not correct, p_data->len:%u, len:%u", p_data->len, len);
        return;
    }

    if(p_data->seq_id != 0)
    {//seq_id == 0为switch主动推送过来的
         uint32_t counter = p_data->seq_id & 0xFFFF;
         if(counter != p_user->counter)
         {
             KCRIT_LOG(p_data->user_id, "It should not happen this case(switch return).");
             return;
         }
        REMOVE_TIMERS(p_user);
    }

    g_errno = ERR_NO_ERR;
    bool clear_wait_cmd = true;

    switch(msg_type)
    {
        case svr_msg_kick_user://服务器主动推送过来的
            {
                int idx = 0;
                uint8_t reason = 0;
                taomee::unpack_h(p_data->body, reason, idx);
                pack_as_pkg_header(p_user->uid, as_msg_level_up, 0, ERR_REPEAT_LOGIN);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), false);
                KINFO_LOG(p_user->uid, "user is kicked for reason:%u", reason);
                close_client_conn(fdsess->fd);
            }
            return;
        case svr_msg_online_friend_apply://服务器主动推送过来的
            process_notify_apply(p_user, p_data, len);
            clear_wait_cmd = false;
            return;
        case svr_msg_get_active_user:
            process_active_user_return(p_user, p_data, len);
            break;
        case svr_msg_get_shop_item:
            process_get_shop_item_return(p_user, p_data, len);
            break;
        case svr_msg_online_encourage:
            process_encourage_guide_return(p_user, p_data, len);
            break;
        default:
            KCRIT_LOG(p_data->user_id, "recv unkonwn [msg:%u] from switch_svr.", msg_type);
	    return;
    }

    if(g_errno != ERR_NO_ERR)
    {
        send_header_to_user(p_user, SYSTEM_ERR_NUM, g_errno);
    }

    return;
}


int process_notify_apply(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len)
{
    assert(p_user != NULL && p_pkg != NULL);

    if(p_pkg->result != 0)
    {
        KCRIT_LOG(p_pkg->user_id, "notify apply failed(errno:%u).", p_pkg->result);
        g_errno = p_pkg->result;
        return 0;
    }

    int idx = 0;
    uint32_t peer_id = 0;
    taomee::unpack_h(p_pkg->body, peer_id, idx);
    uint8_t type = 0;
    taomee::unpack_h(p_pkg->body, type, idx);
    KINFO_LOG(p_pkg->user_id, "notify from switch, peer_id:%u, type:%u", peer_id, type);

    switch(type)
    {
        case MESSAGE_NOTIFY:    // no break
            {
                p_user->user_cache.del_pinboard();
                pack_as_pkg_header(p_user->uid, as_msg_friend_apply, 0, ERR_NO_ERR);
                g_send_msg.pack(type);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), false);
#ifdef DEBUG
                KINFO_LOG(p_user->uid, "notify user new message");
#endif
                break;
            }
        case FRIEND_NOTIFY:
            {
                role_cache_t *p_role = p_user->user_cache.get_role();
                if (p_role != NULL)
                {
                    p_role->pending_req_num += 1;
                }
                pack_as_pkg_header(p_user->uid, as_msg_friend_apply, 0, ERR_NO_ERR);
                g_send_msg.pack(type);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), false);
#ifdef DEBUG
                KINFO_LOG(p_user->uid, "new friend req by switch");
#endif
                break;
            }
        case MESSAGE_APPROVED_FRIEND:
            {
                pack_as_pkg_header(p_user->uid, switch_msg_add_friend, 0, ERR_NO_ERR);
                g_send_msg.pack(peer_id);
                g_send_msg.end();

                cache_a_pkg(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
#ifdef DEBUG
                KINFO_LOG(p_user->uid, "friend apply is approved by %u", peer_id);
#endif
                break;
            }
         case MESSAGE_DEL_FRIEND:
            {
                p_user->friend_tree.del_friend(peer_id);
                role_cache_t *p_role = p_user->user_cache.get_role();
                if (NULL == p_role)
                {
                    KCRIT_LOG(p_user->uid, "role is not in cache");
                    return 0;
                }
                p_role->role.friend_num -= 1;

                break;

            }
         case NEW_VISIT:
            {
                pack_as_pkg_header(p_user->uid, as_msg_friend_apply, 0, ERR_NO_ERR);
                g_send_msg.pack(type);
                g_send_msg.end();
                send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), false);
#ifdef DEBUG
                KINFO_LOG(p_user->uid, "notify user new visit message");
#endif

                break;
            }
         default:
                KCRIT_LOG(p_user->uid, "notify type:%u invalid, peer:%u", type, peer_id);
                return 0;
    }

    return 0;
}

int process_active_user_return(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len)
{
    assert(p_user != NULL && p_pkg != NULL);
    if(p_pkg->result != 0)
    {
        KCRIT_LOG(p_pkg->user_id, "Get active user from switch failed(%u).", p_pkg->result);
        g_errno = p_pkg->result;
    }

    switch_msg_active_user_t *p_active_usr = (switch_msg_active_user_t*)p_pkg->body;
    if(p_active_usr->count == 0)
    {
        KINFO_LOG(p_user->uid, "get no active user");
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack((uint8_t)0);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        return 0;
    }


    if (!p_user->friend_tree.friend_list_exist())   //用户好友列表不存在
    {
        KCRIT_LOG(p_user->uid, "it cann't be here, user friend list not exists!");
        return -1;
    }

    stranger_t *p_stranger_info = (stranger_t *)p_user->buffer;
    if (get_stranger_id(p_user, p_stranger_info, p_active_usr) != 0)
    {
        KCRIT_LOG(p_user->uid, "get stranger id from active user");
        return -1;
    }

    p_stranger_info->cur_num = 0;

    KINFO_LOG(p_user->uid, "get %u users, %u strangers.", p_active_usr->count, p_stranger_info->count);
    if (0 == p_stranger_info->count)
    {
        pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
        g_send_msg.pack((uint8_t)0);
        g_send_msg.end();
        send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
        return 0;
    }

    //获得每个用户的profile信息
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_get_profile, p_stranger_info->stranger[0].user_id, GET_SVR_SN(p_user), 0);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        KCRIT_LOG(p_user->uid, "send to db proxy server failed.");
        return 0;

    }

    return 0;
}

int process_encourage_guide_return(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len)
{
    assert(p_user != NULL && p_pkg != NULL);
    if(p_pkg->result != 0)
    {
        KCRIT_LOG(p_pkg->user_id, "Get shop item from switch failed(%u).", p_pkg->result);
        g_errno = p_pkg->result;
    }
	db_return_encourage_t *p_db_ret = (db_return_encourage_t *)(p_pkg->body);
	KINFO_LOG(p_user->uid, "send encourage to AS:win:%d total:%d", p_db_ret->win_num,p_db_ret->total_num);

	pack_as_pkg_header(p_user->uid, as_msg_encourage_guide, 0, ERR_NO_ERR);
	g_send_msg.pack(p_db_ret->type);
	g_send_msg.pack(p_db_ret->score);
	g_send_msg.pack(p_db_ret->win_num);
	g_send_msg.pack(p_db_ret->total_num);
	g_send_msg.pack(p_db_ret->user_id);
	g_send_msg.end();
	send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len(), false);
    return 0;
}
int process_get_shop_item_return(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len)
{
    assert(p_user != NULL && p_pkg != NULL);
    if(p_pkg->result != 0)
    {
        KCRIT_LOG(p_pkg->user_id, "Get shop item from switch failed(%u).", p_pkg->result);
        g_errno = p_pkg->result;
    }

    shop_t *p_shop_item = (shop_t *)p_pkg->body;
    KINFO_LOG(p_user->uid, "get %u shop item", p_shop_item->count);
    pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack(p_shop_item->remain_time);
    g_send_msg.pack(p_shop_item->count);
    for (int i = 0; i != p_shop_item->count; ++i)
    {
       // KINFO_LOG(p_user->uid, "get item:%u", p_shop_item->item_id[i]);
        g_send_msg.pack(p_shop_item->item_id[i]);
    }

    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}
