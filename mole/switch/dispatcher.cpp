extern "C" {
#include <stdint.h>

#include <libtaomee/log.h>

#include <async_serv/dll.h>
}

#include "online.hpp"
#include "proto.hpp"

#include "dispatcher.hpp"

typedef int (*func_t)(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

func_t funcs[5500];

#define PROTO_BEGIN	60001

void init_funcs()
{
	// For Online Server
	funcs[proto_online_svrinfo - PROTO_BEGIN]       = Online::report_online_info;
	funcs[proto_user_onoff - PROTO_BEGIN]           = Online::report_user_onoff;
	funcs[proto_users_online_status - PROTO_BEGIN]  = Online::users_online_status;
	funcs[proto_chat_across_svr - PROTO_BEGIN]      = Online::chat_across_svr;
	funcs[proto_user_attr_upd_noti - PROTO_BEGIN]   = Online::user_attr_upd_noti;
	funcs[proto_chk_user_location - PROTO_BEGIN]    = Online::chk_user_location;
	funcs[proto_online_keepalive - PROTO_BEGIN]     = Online::online_keepalive;
	funcs[proto_del_uid_friend_list - PROTO_BEGIN]  = Online::del_from_friend_list;
	funcs[proto_notify_use_skill_prize - PROTO_BEGIN]     = Online::broadcast_use_skill_prize_pkt;
	funcs[proto_tell_cp_beauty_prize - PROTO_BEGIN] = Online::broadcast_get_cp_beauty_prize;
	funcs[proto_tell_flash_some_msg - PROTO_BEGIN] 	= Online::broadcast_tell_flash_some_msg;

	// For Login Server
	funcs[login_proto_get_recommended_svrlist - PROTO_BEGIN] = Online::get_recommeded_svrlist;
	funcs[login_proto_get_ranged_svr_list - PROTO_BEGIN]     = Online::get_ranged_svrlist;
	// For Adminer Server
	funcs[adm_proto_kick_user_offline - PROTO_BEGIN]= Online::kick_user_offline;
	funcs[adm_proto_broadcast_msg - PROTO_BEGIN]    = Online::broadcast_msg;
	funcs[adm_proto_broadcast_bubble_msg - PROTO_BEGIN]      = Online::broadcast_bubble_msg;
	
	// For XHX online
	funcs[proto_xhx_user_set_value - PROTO_BEGIN] = Online::xhx_user_set_value;

	funcs[adm_proto_create_npc - PROTO_BEGIN]       = Online::create_npc;
	funcs[adm_proto_syn_to_user_cmd - PROTO_BEGIN]	= Online::syn_to_user_cmd;
	funcs[adm_proto_syn_vip_flag - PROTO_BEGIN]     = Online::syn_vip_flag;

	// For chat monitor
	funcs[proto_chat_monitor_warning_user - PROTO_BEGIN] = Online::chat_monitor_warning_user;
	
}

int dispatch(void* data, fdsession_t* fdsess)
{
	svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(data);
	uint16_t cmd = pkg->cmd - PROTO_BEGIN;
    if(pkg->cmd == proto_test_alive) {
        static uint8_t alive_buffer[1024];
        int idx = sizeof(svr_proto_t);
	    init_proto_head(alive_buffer, pkg->id, idx, pkg->seq, pkg->cmd, 0);
	    return send_pkg_to_client(fdsess, alive_buffer, idx);

    }
	if ((cmd < 1) || (cmd > 5500) || (funcs[cmd] == 0)) {
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg->cmd, fdsess->fd);
		return -1;
	}
    //uint32_t waitcmd = pkg->seq & 0xFFFF;
    //int      connfd  = pkg->seq >> 16;
	return funcs[cmd](pkg, pkg->len - sizeof(svr_proto_t), fdsess);
}

