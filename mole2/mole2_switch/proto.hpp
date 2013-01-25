#ifndef SWITCH_PROTO_HPP_
#define SWITCH_PROTO_HPP_

extern "C" {
#include <stdint.h>
}

enum {
	pkg_size	= 8192
};

enum {
	begin_user_id	= 50000,
};

enum {
	beast_val_id	= 1000000,
	beast_count_max = 5000,
};



/**
 * @brief 定义命令号
 */
enum cmd_id_t {
	proto_test_alive            = 30000,
	// Adminer proto
	adm_proto_kick_user_offline	= 63000,
	adm_proto_broadcast_msg		= 63001,
	// 63002 not used anymore
	adm_proto_create_npc		= 63003,

	// Online proto
	proto_kick_usr_offline		= 60001,
	proto_broadcast_msg			= 60002,
	proto_syn_vip_flag			= 2516,
	proto_syn_vip_exp			= 0x09D7,

	proto_create_npc			= 60101,

	proto_online_svrinfo		= 62001,
	proto_user_onoff			= 62002,
	proto_users_online_status	= 62003,
	proto_chat_across_svr		= 62004,
	// 62005 used by login server
	proto_chk_user_location		= 62007,
	proto_online_keepalive		= 62008,

	proto_onli_challenge_battle	= 62009,
	proto_challenge_battle_onli = 62010,
	proto_noti_across_svr		= 62011,
	proto_onli_apply_pk			= 62012,
	proto_onli_auto_challenge	= 62013,
	proto_onli_auto_accept		= 62014,
	proto_onli_auto_fight		= 62015,
	proto_onli_cancel_pk_apply	= 62016,
	proto_onli_clear_btr_info	= 62017,
	proto_do_proto_across_svr	= 62018,
	proto_onli_sync_beast		= 62019,

	proto_syn_info_to_user		= 62100,
	// Login proto
	login_proto_get_recommended_svrlist_new	= 62006, 
	login_proto_get_ranged_svr_list_new		= 64002,
};

typedef uint32_t userid_t;
typedef uint64_t battle_id_t;



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
