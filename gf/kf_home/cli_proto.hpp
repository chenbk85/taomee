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

#ifndef KF_CLI_PROTO_HPP_
#define KF_CLI_PROTO_HPP_

#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/project/types.h>
}

#include "fwd_decl.hpp"
#include "Player.hpp"

/**
  * @brief command id for client protocol
  */
enum cli_cmd_t {
	// for home notifications
	cli_proto_cmd_start		= 10001,

	cli_proto_login_home    = 10002,
	cli_proto_logout_home   = 10003,
	cli_proto_enter_map     = 10004,
	cli_proto_leave_map     = 10005,
	cli_proto_list_players  = 10006,
	
	cli_proto_cmd_end,
	cli_proto_probe				= 30000,
	cli_proto_cmd_max			= 30001
};

/**
  * @brief command id for online-home_server communication
  */
/*enum home_cmd_t {
	home_cmd_start			= 10001,
	home_cmd_end,

	home_cmd_max				= 13000
};*/

/**
  * @brief severe home server error
  */
/*enum home_err_t {
	home_err_player_not_found	= 1001,
	home_err_no_home_grp		= 1002,

	home_err_max
};*/

/**
  * @brief errno for client protocol
  */
enum cli_err_t {
	// errnos from 10001 to 50000 are reserved for common errors
	cli_err_pkglen_error	= 20001,
	cli_err_system_error	= 20002,
	cli_err_system_busy		= 20003,
	cli_err_multi_login		= 20004,
	cli_err_user_not_found	= 20006,

	cli_err_still_in_map	= 21006,
	cli_err_not_in_map	 	= 21007,
	cli_err_not_in_home 	= 21008,
	cli_err_no_mapid	 	= 21009,
	/*! we return errno from dbproxy by plusing 100000 to dberr (100000 + dberr) */
	cli_err_base_dberr		= 100000,
};

/**
  * @brief some constants
  */
enum {
	/*! max acceptable length in bytes for each client package */
	cli_proto_max_len	= 8 * 1024
};

#pragma pack(1)

/**
  * @brief home protocol type
  */
/*struct home_proto_t {
	uint32_t	len;
	uint32_t	seq;
	uint16_t	cmd;
	uint32_t	ret;
	userid_t	id;
	uint8_t		body[];
};*/

/**
  * @brief client protocol type
  */
struct cli_proto_t {
	/*! package length */
	uint32_t	len;
	/*! protocol command id */
	uint16_t	cmd;
	/*! user id */
	userid_t	id;
	/*! used as an auto-increment sequence number for checking GameCheater */
	uint32_t	seq;
	/*! error number: 0 for OK */
	uint32_t	ret;
	/*! body of the package */
	uint8_t		body[];
};

/**
  * @brief cache a package
  */
struct cached_pkg_t {
	uint16_t	len;
	uint8_t		pkg[];
};

#pragma pack()


/**
  * @brief send a package to player p
  * @param p player to send a package to
  * @param pkg package to send
  * @param len length of the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
//int send_to_player(player_t* p, void* pkgbuf, uint32_t len, int completed);
/**
  * @brief send a package header to player p
  * @param p player to send a package to
  * @param cmd command id of the package
  * @param err errno to be set into the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
//int send_header_to_player(player_t* p, uint32_t cmd, uint32_t err, int completed);
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
/*inline void init_home_proto_head_full(void* header, uint32_t cmd, uint32_t len, uint32_t ret)
{
	home_proto_t* p = reinterpret_cast<home_proto_t*>(header);

	p->len = len;
	p->cmd = cmd;
	p->ret = ret;
}*/

/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  */
/*inline void init_home_proto_head(void* header, uint32_t cmd, uint32_t len)
{
	init_home_proto_head_full(header, cmd, len, 0);
}*/

/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  */
inline void init_cli_proto_head(void* header, uint16_t cmd, uint32_t len)
{
	cli_proto_t* p = reinterpret_cast<cli_proto_t*>(header);

	p->id  = 0;
	p->len = taomee::bswap(len);
	p->cmd = taomee::bswap(cmd);
	p->ret = 0;
}

#endif // KF_CLI_PROTO_HPP_

