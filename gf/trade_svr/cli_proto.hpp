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
	cli_proto_probe					= 30000,
	cli_get_version					= 30001,
};


/**
  * @brief command id for online-battle_server communication
  */
enum trd_cmd_t {
	/*! starting command id */
	trd_cmd_start			= 10001,

	trd_enter_market	    = 10001,
	trd_leave_market        = 10002,
	trd_change_market       = 10003,
	trd_setup_shop			= 10004,
	trd_drop_shop			= 10005,
	trd_update_shop			= 10006,
	trd_buy_goods           = 10007,
	trd_decorate_shop       = 10008,

	trd_walk				= 10009,
	trd_walk_keyboard		= 10010,
	trd_stand				= 10011,
	trd_jump				= 10012,
    trd_sell_goods          = 10013,
    trd_list_user			= 10014,

	trd_list_shop           = 10015,

	trd_get_shop_detail     = 10016,
	trd_change_shop_name    = 10017,
	trd_pause_shop          = 10018,
	trd_talk				= 10019,
	trd_transmit_only       = 10020,

	trd_wear_clothes        = 10021,

	trd_change_summon       = 10022,
	trd_call_summon         = 10023,
	trd_cmd_max				= 10024,

    trd_safe_trade_create_room  = 10030,
    trd_safe_trade_join_room    = 10031,
    trd_safe_trade_cancel_room  = 10032,
    trd_safe_trade_set_item     = 10033,
    trd_safe_trade_action_agree = 10034,

    trd_safe_trade_cmd_max  = 10041,
};

enum err_t {
	cli_err_system_error     = 10002,
	cli_err_system_busy      = 10003,
	cli_err_wrong_market_id  = 13001,
	cli_err_wrong_shop_id	 = 13002,
	cli_err_market_full      = 13003,
	cli_err_shop_in_tmlimit  = 13004,
	cli_err_have_no_shop	 = 13005,
	//cli_err_shop_constructing= 13006,
	cli_err_shop_not_open    = 13007,
	cli_err_item_in_tradeing = 13008,
	cli_err_shop_outdated    = 13009,


	tr_err_player_not_found  = 10031,

	cli_err_base_dberr		= 100000,
};

enum {
	/*! max acceptable length in bytes for each client package */
	max_pkg_size = 16 * 1024
};

#pragma pack(1)

/**
  * @brief battle protocol type
  */
struct tr_proto_t {
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

int send_to_player_by_uid(uint32_t uid, fdsession_t* fdsess, void* pkgbuf, uint32_t len, int completed);

int send_header_to_player_2(uint32_t uid, fdsession_t* fdsess, uint32_t cmd, uint32_t err, int completed);
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
inline void init_tr_proto_head_full(void* header, uint32_t cmd, uint32_t len, uint32_t ret)
{
	tr_proto_t* p = reinterpret_cast<tr_proto_t*>(header);

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
inline void init_tr_proto_head(void* header, uint32_t cmd, uint32_t len)
{
	init_tr_proto_head_full(header, cmd, len, 0);
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

