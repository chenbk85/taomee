/** 
 *============================================================
 *  @file      cachesvr.hpp
 *  @brief     cache server related functions are declared here
 *  
 *  compiler   gcc4.3.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef  KF_CACHESVR_HPP_
#define  KF_CACHESVR_HPP_

#include <libtaomee++/inet/pdumanip.hpp> 

extern "C" {
#include <stdint.h>
#include <libtaomee/project/types.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/log.h>
#include <async_serv/net_if.h>
}   
    
#include "player.hpp"

#pragma pack(1)

/** 
  * @brief cache server protocol definition
  */
struct cachesvr_proto_t { 
	/*! package length */
	uint32_t	len;
	/*! sequence number ((p->fd << 16) | p->role_tm | login_tm)*/
	uint32_t	seq;
	/*! command id */
	uint32_t    cmd;
	/*! errno */
	uint32_t	ret;
	/*! user id */
	userid_t	uid;

	uint32_t    role_tm;

	/*! package body */
	uint8_t		body[];
};

#pragma pack()

enum {
	cache_basic_info_report = 10001,

	cache_save_team_active_info = 10011,
	cache_list_team_active_ranker = 10012,
	cache_save_single_active_info = 10013,
	cache_list_single_active_ranker = 10014,
	cache_reset_player_achieve = 11010,

	cache_generate_magic_number = 11011,
	cache_bind_magic_number = 11012,
	cache_get_magic_number_creator = 11013,

};


/*! file descriptor for connection with cache server */
extern int cachesvr_fd;

void report_user_basic_info_to_cachesvr(player_t * p);

int send_request_to_cachesvr(player_t * p, uint32_t uid, uint32_t role_tm,  uint32_t cmd, const void* body, uint32_t bodylen);

void handle_cachesvr_return(cachesvr_proto_t* data, int len);

int load_cache_config(xmlNodePtr xml);

#endif //KF_CACHESVR_HPP  ----- 

