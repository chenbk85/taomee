
extern "C" 
{
#include <stdint.h>
#include <libtaomee/log.h>
#include <async_serv/dll.h>
}

#include "online.hpp"
#include "switch.hpp"
#include "dispatcher.hpp"

typedef int (*func_t)(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

func_t funcs[5500];

/**
  * @brief initialize processing functions
  */
void init_funcs()
{
	//For admin
	funcs[adm_proto_post_msg - 60001]	= Online::adm_post_msg;
	funcs[adm_proto_kick_user_offline - 60001]	= Online::adm_kick_user_offline;
	funcs[adm_proto_syn_vip_flag - 60001]	= Online::adm_syn_vip;
	funcs[adm_proto_syn_vip_value - 60001]	= Online::adm_syn_vip_base_value;
	funcs[adm_proto_syn_user_gcoins - 60001]	= Online::adm_syn_user_gcoins;
				
	// For Online Server
	funcs[proto_online_svrinfo - 60001]       = Online::report_online_info;
	funcs[proto_home_online_info - 60001]       = Online::report_home_online_info;
	funcs[proto_user_onoff - 60001]           = Online::report_user_onoff;
	funcs[proto_users_online_status - 60001]  = Online::friends_online_status;
	funcs[proto_chat_across_svr - 60001]      = Online::chat_across_svr;
	funcs[proto_user_attr_upd_noti - 60001]   = Online::user_attr_upd_noti;
	funcs[proto_chk_user_location - 60001]    = Online::chk_user_location;
	funcs[proto_online_keepalive - 60001]    = Online::online_keepalive;
	funcs[proto_user_map_upd_noti - 60001]   = Online::user_map_upd_noti;
	funcs[proto_chat_across_svr_2 - 60001]      = Online::chat_across_svr_2;
	funcs[proto_chat_across_svr_mentor_sys - 60001]	= Online::chat_across_svr_mentor_sys;
	funcs[proto_upd_new_invitee_cnt - 60001]	= Online::upd_new_invitee_cnt;
	funcs[proto_send_btl_inv - 60001]	= Online::send_btl_inv;
	funcs[proto_chat_across_svr_team_sys - 60001]	= Online::chat_across_svr_team_sys;
	funcs[proto_chat_across_svr_rltm - 60001]	= Online::chat_across_svr_rltm;
	
	funcs[proto_new_mail_notify - 60001] = Online::notify_player_new_mail;
	funcs[mcast_official_world_notice - 60001] = Online::mcast_official_world_notice;
	// For Login Server
	funcs[login_proto_get_recommended_svrlist - 60001] = Online::get_recommeded_svrlist;
	funcs[login_proto_get_ranged_svr_list - 60001]     = Online::get_ranged_svrlist;
}

/**
  * @brief dispatch package to the corresponding processing functions
  * @param data package data
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int dispatch(void* data, fdsession_t* fdsess)
{
	svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(data);
	if (pkg->cmd == 30000) {
		send_pkg_to_client(fdsess, pkg, pkg->len);
	}
    if (pkg->cmd == 1888 || pkg->cmd == 1889 || pkg->cmd == 1890) {
        send_pkg_to_client(fdsess, pkg, 22);
    }

	uint16_t cmd = pkg->cmd - 60001;
	if ((cmd < 0) || (cmd > 5499) || (funcs[cmd] == 0)) 
	{
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg->cmd, fdsess->fd);
		return -1;
	}
	return funcs[cmd](pkg, pkg->len - sizeof(svr_proto_t), fdsess);
}

