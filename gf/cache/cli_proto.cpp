/*
 * =====================================================================================
 *
 *       Filename:  cli_proto.cpp
 *
 *
 *        Version:  1.0
 *        Created:  05/09/2012 03:05:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
extern "C" {
#include <assert.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
}
#include "cli_proto.hpp"
#include "cache.hpp"
#include "common_val.hpp"

typedef int (*func_t)(svr_proto_t* pkg, fdsession_t* fdsess);

func_t funcs[5500];

int dispatch(void *buf, fdsession_t * fdsess)
{
	svr_proto_t * pkg = reinterpret_cast<svr_proto_t*>(buf);
	
	assert(pkg->len < max_pkg_size);

	if (funcs[pkg->cmd - 10001] != NULL) {
		return funcs[pkg->cmd - 10001](pkg, fdsess);
	}
	return -1;
}

void init_funcs() 
{
	final_funcs();
	funcs[report_user_basic_info - 10001] = CacheSvr::report_user_basic_info;
	funcs[player_save_team_active_score - 10001] = CacheSvr::save_team_active_info;
	funcs[player_list_team_active_ranker - 10001] = CacheSvr::list_team_ranker_info;


	funcs[set_team_contest_server_info - 10001] = CacheSvr::set_team_contest_server;
	funcs[get_team_contest_server_info - 10001] = CacheSvr::get_team_contest_server;
	funcs[player_get_team_contest_server - 10001] = CacheSvr::player_get_team_contest_server;
	funcs[player_save_active_score - 10001 ] = CacheSvr::player_save_active_info;
	funcs[player_list_active_ranker - 10001] = CacheSvr::player_list_active_ranker_info;
	funcs[player_reset_player_achieve - 10001 ] = CacheSvr::reset_player_achieve;

	funcs[player_generate_magic_number - 10001]  = CacheSvr::player_generate_magic_number;
	funcs[player_bind_magic_number - 10001] = CacheSvr::player_bind_magic_number;
	funcs[player_get_magic_number_creator - 10001] = CacheSvr::get_player_maigc_invitee; 

}

void final_funcs()
{
	for (uint32_t i = 0; i < 5500; i++) {
		funcs[i] = NULL;
	}
}

char pkgbuf[20000];

void init_pkg_form_pkg(void * buf,
		               svr_proto_t * pkg, 
					   uint32_t len)
{
	svr_proto_t * new_pkg = reinterpret_cast<svr_proto_t*>(buf);
	new_pkg->len = len;
	new_pkg->cmd = pkg->cmd;
	new_pkg->ret = 0;
	new_pkg->seq = pkg->seq;
	new_pkg->uid = pkg->uid;
	new_pkg->role_tm = pkg->role_tm;

}

void init_pkg_head_full(void * buf,
						uint32_t uid,
	                	uint32_t role_tm,
					   	uint32_t cmd,
						uint32_t len,
						uint32_t seq, 
						uint32_t ret) 
{
	svr_proto_t * new_pkg = reinterpret_cast<svr_proto_t*>(buf);
	new_pkg->len = len;
	new_pkg->cmd = cmd;
	new_pkg->ret = ret;
	new_pkg->seq = seq;
	new_pkg->uid = uid;
	new_pkg->role_tm = role_tm;

}

int send_pkg(fdsession_t * fdsss, void * buf, uint32_t len)
{
	assert(len <= max_pkg_size);
	return send_pkg_to_client(fdsss, buf, len);
}



