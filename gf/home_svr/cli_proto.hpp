/**
 *============================================================
 *  @file      cli_proto.hpp
 *  @brief    client protocol definations and package sending functions
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFTRD_CLI_PROTO_HPP_
#define KFTRD_CLI_PROTO_HPP_

#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/project/types.h>
}

#include "player.hpp"


/**
  * @brief command id for client protocol
  */
enum cli_cmd_t {
	cli_proto_enter_map             = 2001,
	cli_proto_leave_map 			= 2002,
	cli_proto_list_user		        = 2003,


	cli_proto_list_home_pet         = 2005,

	cli_proto_walk					= 2101,
	cli_proto_talk					= 2102,
	cli_proto_walk_keyboard 		= 2103,
	cli_proto_stand 				= 2104,
	cli_proto_jump					= 2105,

    cli_proto_trade_shop_change      = 4013,
	cli_proto_sync_process          = 2408,
	cli_proto_sync_process_noti     = 2409,
	
	cli_proto_player_move           = 2451,
	cli_proto_npc_move              = 2452,
	cli_proto_summon_change         = 2309,
	cli_proto_call_summon           = 2306,

	cli_proto_wear_clothes          = 2604,

	cli_proto_enter_friend_home     = 3002,
	cli_proto_pet_move              = 3004,
	cli_proto_pet_home_call_mcast   = 3006,

	cli_proto_syn_home_player_info  = 3007,
	cli_proto_home_owner_off_line   = 3008,
	cli_proto_home_kick_off_guest   = 3009,

	cli_proto_get_plants_list		= 3010,
	cli_proto_set_plant_status		= 3011,
	cli_proto_notify_plant_status	= 3016,
	
	cli_proto_get_home_attr         = 3020,
	cli_proto_add_home_exp          = 3022,

    cli_proto_get_decorate_list     = 3030,
    cli_proto_set_decorate          = 3031,
    cli_proto_add_decorate_lv       = 3032,
    cli_proto_query_player_count    = 8007,

	cli_proto_probe					= 30000,
	cli_get_version					= 30001
};


/**
  * @brief command id for online-battle_server communication
  */
enum home_cmd_t {
	/*! starting command id */
	home_cmd_start			= 10001,

	home_enter_home          = 10001,
	home_enter_friend_home   = 10002,
	home_leave_home       = 10003,

	home_pet_move         = 10004,
	home_pet_call         = 10005,
	home_pet_callback     = 10006,

	home_list_user        = 10007,
	home_list_pet         = 10008,
	
	home_syn_player_info  = 10009,

	home_get_plants_list  = 10030,
	home_set_plant_status  = 10031,
	home_pick_fruit		  = 10032,
	home_sow_plant_seed   = 10033,

    home_get_decorate_list  = 10040,
    home_set_decorate       = 10041,
    home_add_decorate_lv    = 10042,
    home_get_decorate_reward= 10043,

	home_player_walk      = 10050,
	home_walk_keyboard    = 10051,
	home_player_stand     = 10052,
	home_player_jump      = 10053,
	home_player_talk      = 10054,
    home_query_player_count = 10055,

	
	home_player_enter_battle = 10070,
	home_player_leave_battle = 10071,

	home_player_off_line     = 10072,

	home_kick_off_player     = 10073,
	home_get_home_attr       = 10074,
	home_level_up            = 10075,

	home_cmd_max             = 11000,

    home_transmit_only    = 12001,

};

enum err_t {
	cli_err_system_error     = 10002,
	cli_err_system_busy      = 10003,
	cli_err_cannot_pick		 = 14001,
	cli_err_cannot_sow_seed  = 14002,
	cli_err_cannot_water  	 = 14003,
	cli_err_home_leveless  	 = 14004,
	cli_err_cannot_heap  	 = 14005,
	
	cli_err_home_full        = 10195,
	cli_err_not_in_home      = 10196,
	cli_err_not_home_owner   = 10197,
	cli_err_not_level_up     = 10198,
	cli_err_not_fumo_point   = 10199,

	tr_err_player_not_found  = 10031,

	cli_err_base_dberr		= 100000,
};

enum {
	/*! max acceptable length in bytes for each client package */
	max_pkg_size = 8 * 1024
};

#pragma pack(1)

/**
  * @brief battle protocol type
  */
struct home_proto_t {
	/*! package length */
	uint32_t	len;
	/*! battle group id */
	uint32_t	seq;
	/*! command id */
	uint16_t	cmd;
	/*! errno */
	uint32_t	ret;
	/*! user id */
	userid_t	id;
	/*! package body */
	uint8_t		body[];
};

struct cli_proto_t {
	uint32_t len;
	uint16_t cmd;
	userid_t id;
	uint32_t seqno;
	uint32_t ret;
	uint8_t  body[];
};


/**
  * @brief cache a package
  */
struct cached_pkg_t {
	uint16_t	len;
	uint8_t		pkg[];
};

#pragma pack()

/*! for packing protocol data and send to client. 2M */
extern uint8_t pkgbuf[1 << 21];

/**
  * @brief send a package to player p
  * @param p player to send a package to
  * @param pkg package to send
  * @param len length of the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_to_player(Player* p, void* pkgbuf, uint32_t len, int completed);


int send_to_player_by_uid(fdsession_t* fdsess, uint32_t uid, void* pkgbuf, uint32_t len, int completed);
/**
  * @brief send a package header to player p
  * @param p player to send a package to
  * @param cmd command id of the package
  * @param err errno to be set into the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_header_to_player(Player* p, uint32_t cmd, uint32_t err, int completed);
/**
  * @brief send a package header to online server
  * @param uid user who will receive the package
  * @param fdsess
  * @param cmd
  * @param seqno
  * @param err errno to be set into the package
  * @return 0 on success, -1 on error
  */
int send_header_to_online(userid_t uid, fdsession_t* fdsess, uint32_t cmd, uint32_t seqno, uint32_t err);

//--------------------------------------------------------------------

/**
  * @brief init handlers to handle protocol packages from client
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_cli_proto_handles();
/**
  * @brief dispatches protocol packages from client
  * @param data package from client
  * @param fdsess fd session
  * @param first_tm true if we are processing a package from a client,
  *              false if from the pending-processing queue
  * @return value that was returned by the corresponding protocol handler
  */
int dispatch(void* data, fdsession_t* fdsess, bool first_tm = true);
/**
  * @brief scan the 'awaiting_playerlist' and process cached packages of those players whose waitcmd is 0
  */
void proc_cached_pkgs();

//-----------------------------------------------------------
// inline function definations
//-----------------------------------------------------------
/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  * @param ret return code
  */
inline void init_home_proto_head_full(void* header, uint32_t cmd, uint32_t len, uint32_t ret)
{
	home_proto_t* p = reinterpret_cast<home_proto_t*>(header);

	p->len = len;
	p->cmd = cmd;
	p->ret = ret;
}

/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  */
inline void init_home_proto_head(void* header, uint32_t cmd, uint32_t len)
{
	init_home_proto_head_full(header, cmd, len, 0);
}

/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  */
inline void init_cli_proto_head_full(void* header, userid_t id, uint16_t cmd, uint32_t ret, uint32_t len)
{
	cli_proto_t* p = reinterpret_cast<cli_proto_t*>(header);

	p->id  = taomee::bswap(id);
	p->len = taomee::bswap(len);
	p->cmd = taomee::bswap(cmd);
	p->ret = taomee::bswap(ret);
}

/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  */
inline void init_cli_proto_head(void* header, uint16_t cmd, uint32_t len)
{
	init_cli_proto_head_full(header, 0, cmd, 0, len);
}

//--------------------

#endif // KF_CLI_PROTO_HPP_

