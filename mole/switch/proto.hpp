#ifndef SWITCH_PROTO_HPP_
#define SWITCH_PROTO_HPP_

extern "C" {
#include <stdint.h>
}

enum {
	pkg_size	= 8192
};

/**
 * @brief 定义命令号
 */
enum cmd_id_t {
	proto_test_alive            	= 30000,
	// Adminer proto
	adm_proto_syn_to_user_cmd		= 61696,
	adm_proto_syn_vip_flag			= 61697,
	adm_proto_kick_user_offline		= 63000,
	adm_proto_broadcast_msg			= 63001,
	adm_proto_broadcast_bubble_msg 	= 63002,
	adm_proto_create_npc			= 63003,

	// Online proto
	proto_kick_usr_offline			= 60001,
	proto_broadcast_msg				= 60002,
	proto_syn_vip_flag				= 60003,
	proto_broadcast_bubble_msg		= 60004,

	proto_guess_start				= 60005,
	proto_guess_end					= 60006,

	proto_create_npc				= 60101,

	proto_online_svrinfo			= 62001,
	proto_user_onoff				= 62002,
	proto_users_online_status		= 62003,
	proto_chat_across_svr			= 62004,
	// 62005 used by login server
	proto_user_attr_upd_noti		= 62006,
	proto_chk_user_location			= 62007,
	proto_online_keepalive			= 62008,
	proto_del_uid_friend_list   	= 62009,
	proto_notify_use_skill_prize 	= 62010,

	//62014 used to recv msg from xhx
	proto_xhx_user_set_value		= 62014,
	proto_chat_monitor_warning_user	= 62015,
	proto_tell_cp_beauty_prize		= 62016,
	proto_tell_flash_some_msg 		= 62017,
	
	// Login proto
	login_proto_get_recommended_svrlist	= 62005,
	login_proto_get_ranged_svr_list		= 64001
};

typedef uint32_t userid_t;



/* Define the communication protocol between the switch and online 
   server(or adminer server, login server) */
struct svr_proto_t {
        /* package head */
        uint32_t	len; // package length, no more than 4k bytes
        uint32_t	seq; // must be 0
        uint16_t	cmd; // cmd id, >OxEA60
        uint32_t	ret; // Result, must be 0
        userid_t	id; // Userid
        /* package body, variable length */
        uint8_t		body[];
} __attribute__((packed));

inline void
init_proto_head(void* buf, userid_t uid, uint32_t len, uint32_t seq, uint16_t cmd, uint32_t ret)
{
	svr_proto_t* hdr = reinterpret_cast<svr_proto_t*>(buf);
	hdr->len = len;
	hdr->seq = seq;
	hdr->cmd = cmd;
	hdr->ret = ret;
	hdr->id  = uid;
}

#endif // SWITCH_PROTO_HPP_
