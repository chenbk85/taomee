/**
 *============================================================
 *  @file      mcast_proto.hpp
 *  @brief    multi-cast proto
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef GF_MCAST_PROTO_HPP_
#define GF_MCAST_PROTO_HPP_

extern "C" {
#include <stdint.h>
#include <async_serv/dll.h>
}

/**
  * @brief multi-cast proto command type
  */
enum mcast_cmd_t {
	/*! reload config */
	mcast_reload_conf	= 60001,
	/*! team info */
	mcast_team_info		= 60002,
	/*frined login*/
	mcast_friend_login  = 60003,
	/*for trade mcast */
	mcast_trade_message = 60004,
    /*for world notice */
    mcast_world_notice = 60005,
    /*for limit data */
    mcast_limit_data = 60006,
	
	mcast_achieve_mcast = 60007,
};

#pragma pack(1)

/**
  * @brief multi-cast package type
  */
struct mcast_pkg_t {
	uint32_t	server_id;
	uint32_t	main_cmd;
	uint32_t	minor_cmd;
	uint8_t		body[];	
};

#pragma pack()

inline void init_mcast_pkg_head(void* buf, uint32_t major_cmd, uint32_t minor_cmd)
{
	mcast_pkg_t* pkg = reinterpret_cast<mcast_pkg_t *>(buf);
	pkg->server_id = get_server_id(); /* mcast pkg_head id */
	pkg->main_cmd  = major_cmd;
	pkg->minor_cmd = minor_cmd;
}


#endif // GF_MCAST_PROTO_HPP_

