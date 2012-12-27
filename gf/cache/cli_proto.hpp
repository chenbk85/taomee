/*
 * =====================================================================================
 *
 *       Filename:  cli_proto.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/21/2012 04:38:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */

#ifndef CLI_PROTO_HPP
#define CLI_PROTO_HPP
extern "C" {
#include <stdint.h>
}

#include <async_serv/net_if.h>

enum cli_cmd_t {
	report_user_basic_info = 10001,

	//for team active
	player_save_team_active_score =  10011,
	player_list_team_active_ranker = 10012,

	player_save_active_score = 10013,
	player_list_active_ranker = 10014,


	//for switch save team contest info
	set_team_contest_server_info = 11002,
	get_team_contest_server_info = 11003,
	player_get_team_contest_server = 11004,

	player_reset_player_achieve = 11010,

	player_generate_magic_number = 11011,
	player_bind_magic_number = 11012,
	player_get_magic_number_creator = 11013,

	cache_max_cmd = 12000,
};

struct svr_proto_t {
	uint32_t len;
	uint32_t seq;
	uint32_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint32_t role_tm;
	uint8_t  body[];
}__attribute__((packed));


int dispatch(void *buf, fdsession_t * fdsess);

void init_funcs();

void final_funcs();

extern char pkgbuf[20000];

void init_pkg_form_pkg(void * buf,
		               svr_proto_t * pkg, 
					   uint32_t len);

void init_pkg_head_full(void * buf,
						uint32_t uid,
	                	uint32_t role_tm,
					   	uint32_t cmd,
						uint32_t len,
						uint32_t seq = 0, 
						uint32_t ret = 0); 

int send_pkg(fdsession_t * fdsss, void * buf, uint32_t len);

#endif


