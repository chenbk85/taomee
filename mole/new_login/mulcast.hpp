// =====================================================================================
// 
//       Filename:  mulcast.hpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  06/12/2010 04:50:04 PM CST
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  xcwen (xcwen), jim@taomee.com
//        Company:  TAOMEE
// 
// =====================================================================================

#ifndef  MULCAST_INC
#define  MULCAST_INC

extern "C" {
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/inet/ipaddr_cat.h>

#include <libtaomee/project/utilities.h>

#include <async_serv/net_if.h>
}

#include <libtaomee++/inet/byteswap.hpp>

#include <main_login/common.hpp>
#include <main_login/id_counter.hpp>
#include <main_login/user_manager.hpp>

enum {

	MULTICAST_CMD = 61003,
};

//login-onlineServer multicast package header define
typedef struct login_online_pk_header {
	uint32_t	len;
	uint32_t	online_id;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	uint64_t	mapid;
	uint32_t	opid;
} __attribute__((packed)) login_online_pk_header_t;

int multicast_init();

int  multicast_player_login(uint8_t* body_buf, int body_len);
#endif   // ----- #ifndef MULCAST_INC  ----- 

