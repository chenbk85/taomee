extern "C" {
#include <stdint.h>

#include <libtaomee/log.h>

#include <async_serv/dll.h>
}

#include "online.hpp"
#include "proto.hpp"

#include "dispatcher.hpp"

/********新的命令分发 start*******/
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include <libtaomee++/utils/strings.hpp>
#include "./proto/mole2_switch.h"
#include "./proto/mole2_switch_enum.h"

extern int g_log_send_buf_hex_flag;
//-------------------------------------------------------
//函数定义
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    int proto_name(fdsession_t* fdsess,Cmessage* c_in ) ;
#include "./proto/mole2_switch_bind_for_cli_request.h"
//-------------------------------------------------------------
//对应的结构体
#include "./proto/mole2_switch.h"
//-------------------------------------------------------------
//命令绑定
typedef   int(*P_DEALFUN_T)(fdsession_t* fdsess,Cmessage* p_in);

//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_cli_cmd_map;
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    {cmdid, new (c_in), md5_tag,bind_bitmap ,Online::proto_name },

Ccmd< P_DEALFUN_T> g_cli_cmd_list[]={
#include "./proto/mole2_switch_bind_for_cli_request.h"
};
/********新的命令分发 end*******/

typedef int (*func_t)(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
typedef int (*udp_func_t)(svr_proto_t* pkg, uint32_t bodylen, int fd);

func_t funcs[5500];
udp_func_t udp_funcs[5500];

void init_funcs()
{
	// For Online Server
	funcs[proto_online_svrinfo - 60001]       = Online::report_online_info;
	funcs[proto_user_onoff - 60001]           = Online::report_user_onoff;
	funcs[proto_users_online_status - 60001]  = Online::users_online_status;
	funcs[proto_chat_across_svr - 60001]      = Online::chat_across_svr;
	funcs[proto_noti_across_svr - 60001]      = Online::noti_across_svr;
	funcs[proto_chk_user_location - 60001]    = Online::chk_user_location;
	funcs[proto_online_keepalive - 60001]     = Online::online_keepalive;
	funcs[proto_onli_challenge_battle - 60001]     = Online::challenge_battle;
	funcs[proto_onli_apply_pk - 60001]  	  = Online::onli_apply_pk;
	funcs[proto_onli_auto_accept - 60001]  	  = Online::onli_auto_accept;
	funcs[proto_onli_auto_fight - 60001] 	  = Online::onli_auto_fight;
	funcs[proto_onli_cancel_pk_apply - 60001] = Online::onli_cancel_pk_apply;
	funcs[proto_do_proto_across_svr - 60001]  = Online::do_proto_across_svr;
	funcs[proto_onli_sync_beast - 60001]	  = Online::onli_sync_beast;
	funcs[proto_broadcast_msg  - 60001]	  = Online::broadcast_msg;
	
	// For Login Server
	funcs[login_proto_get_recommended_svrlist_new - 60001]	= Online::get_recommeded_svrlist_new;
	funcs[login_proto_get_ranged_svr_list_new- 60001]     = Online::get_ranged_svrlist_new;
	// For Adminer Server
	funcs[adm_proto_kick_user_offline - 60001]  = Online::kick_user_offline;
	funcs[adm_proto_broadcast_msg - 60001]      = Online::broadcast_msg;
	funcs[adm_proto_create_npc - 60001]         = Online::create_npc;
	funcs[proto_syn_info_to_user-60001]       = Online::syn_info_to_user;

	udp_funcs[proto_syn_vip_flag]       	= Online::syn_vip_flag;
	udp_funcs[proto_syn_vip_exp]       		= Online::syn_vip_exp;
	g_cli_cmd_map.initlist(g_cli_cmd_list, sizeof(g_cli_cmd_list)/sizeof(g_cli_cmd_list[0]));
}

int dispatch(void* data, fdsession_t* fdsess)
{
	svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(data);
	uint16_t cmd = pkg->cmd - 60001;
    if(pkg->cmd == proto_test_alive) {
        static uint8_t alive_buffer[1024];
        int idx = sizeof(svr_proto_t);
	    init_proto_head(alive_buffer, pkg->id, idx, pkg->seq, pkg->cmd, 0);
	    return send_pkg_to_client(fdsess, alive_buffer, idx);

    }
	if ((cmd < 1) || (cmd > 5500)) {
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg->cmd, fdsess->fd);
		return -1;
	}
	//DEBUG_LOG("userid:%u,cmdid:%u",pkg->id,cmd );
    //uint32_t waitcmd = pkg->seq & 0xFFFF;
    //int      connfd  = pkg->seq >> 16;
    Ccmd< P_DEALFUN_T> * p_cmd_item =g_cli_cmd_map.getitem(pkg->cmd);
	if (p_cmd_item ) {//是新的命令
        KDEBUG_LOG(pkg->id,"cli dispatch cmd %u", pkg->cmd);
        //还原对象
        p_cmd_item->proto_pri_msg->init( );
        byte_array_t in_ba ( ((char*)data)+sizeof(*pkg),
            pkg->len -sizeof(*pkg) );
        //失败
        if (!p_cmd_item->proto_pri_msg->read_from_buf(in_ba)) {
            DEBUG_LOG("client recover object fail");
            return -1;
        }
        //客户端多上传报文
        if (!in_ba.is_end()) {
            DEBUG_LOG("client !in_ba.is_end()");
            return  -1;
        }
        return p_cmd_item->func(fdsess,p_cmd_item->proto_pri_msg);
    }else{
		return funcs[cmd](pkg, pkg->len - sizeof(svr_proto_t), fdsess);
	}	
}

int dispatch_udp(void* data, int fd)
{
	svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(data);
	if ((pkg->cmd < 1) || (pkg->cmd > 5500) || (udp_funcs[pkg->cmd] == 0)) {
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg->cmd, fd);
		return -1;
	}
	DEBUG_LOG("userid:%u,cmdid:%u",pkg->id, pkg->cmd );
	return udp_funcs[pkg->cmd](pkg, pkg->len - sizeof(svr_proto_t), fd);
}
