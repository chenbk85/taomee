#ifndef CENTRAL_ONLINE_H_
#define CENTRAL_ONLINE_H_

// headers since C99
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/timer.h>
#include "libtaomee/project/stat_agent/msglog.h"
#include <libtaomee/conf_parser/config.h>
#ifdef __cplusplus
}
#endif


#include "benchapi.h"
#include "dbproxy.h"
#include "cli_proto.h"
#include "svr_proto.h"
#include "sprite.h"
#include "event.h"
struct syn_info_t {
        uint32_t src_userid;
        uint32_t obj_userid;
        uint32_t opt_type;
        uint32_t v1;
        uint32_t v2;   
        uint32_t v3;
        uint32_t v4; 
} __attribute__((packed));


enum CentralOnlineCmd {
	COCMD_kick_user 			= 60001,
	COCMD_broadcast_msg			= 60002,
	COCMD_sync_vip				= 2516,
	COCMD_sync_vip_exp			= 0x09D7,

	COCMD_create_npc			= 60101,
	
	COCMD_init_pkg				= 62001,
	COCMD_user_login_noti		= 62002,
	COCMD_chk_if_online			= 62003,
	COCMD_chat_across_svr		= 62004,
	// 62005 Used by DirSer
	COCMD_attr_update_noti		= 62006,
	COCMD_keep_alive			= 62008,
	COCMD_onli_challenge_battle	= 62009,
	COCMD_challenge_battle_onli	= 62010,
	COCMD_notify_across			= 62011,
	COCMD_onli_apply_pk			= 62012,
	COCMD_onli_auto_challenge	= 62013,
	COCMD_onli_auto_accept		= 62014,
	COCMD_onli_auto_fight		= 62015,
	COCMD_onli_cancel_pk_apply	= 62016,
	COCMD_onli_clear_btr_info	= 62017,
	COCMD_add_cmd_across_svr	= 62018,
	COCMD_onli_sync_beast_cnt	= 62019,
	COCMD_switch_to_user		= 62020,
	//同步用户信息
	COCMD_sync_info_to_user		= 62100,
};

enum CentralOnlineErrNo {
	COERR_user_offline	= 1
};


int sync_vip_op(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, int len, int ret);
int chk_online_across_svr_callback(sprite_t* initor, uint32_t rcverid, const uint8_t* buf, uint32_t len, uint32_t ret);
int handle_switch_return(switch_proto_t* dbpkg, int len);


/**
  * @brief connect to switch server every 30 senconds in case of disconnection
  * @return 0 on success and delete the original timer, -1 on error
  */
static inline int connect_to_switch_timely(void* owner, void* data)
{
	if (!data) {
		ADD_ONLINE_TIMER(&g_events, n_connect_to_switch_timely, (void*)1, 30);
	} else if (switch_fd == -1) {
		connect_to_switch();
		if (switch_fd == -1) {
			ADD_ONLINE_TIMER(&g_events, n_connect_to_switch_timely, (void*)1, 30);
		}
	}
	return 0;
}

// check if `p's` friends is online in some other servers
static inline void chk_online_across_svr(sprite_t* p, void* pkg, int len)
{
	if (send_to_switch(p, COCMD_chk_if_online, len, pkg, p->id) == -1) {
		chk_online_across_svr_callback(p, p->id, 0, 0, 0);
	}
}

static inline void notify_user_login(sprite_t* p, uint8_t login)
{
	uint8_t buf[2] = {login, p->btr_info ? 1 : 0};
	send_to_switch(NULL, COCMD_user_login_noti, 2, buf, p->id);
}

static inline void notify_across_switch(sprite_t* p, uint8_t* buf, uint32_t len)
{
	send_to_switch(p, COCMD_notify_across, len , buf, p->id);
}

static inline void report_global_beast_kill(uint32_t kill_beast)
{
	send_to_switch(NULL, COCMD_onli_sync_beast_cnt, 4, &kill_beast, 0);
}


static inline void syn_info_to_user( sprite_t* p, syn_info_t & syn_info)
{
	send_to_switch( p, COCMD_sync_info_to_user, sizeof(syn_info ) , &syn_info, 0);
}


int init_switch_handle_funs();
int get_data_from_switch();
#endif // CENTRAL_ONLINE_H_
