/*
 * =====================================================================================
 *
 *       Filename:  team_contest.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2012 04:57:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
}
#include <map>
#include <libtaomee++/inet/pdumanip.hpp>
using namespace taomee;
#include "team_contest.hpp"
#include "cache.hpp"

struct ServerInfo {
	uint32_t server_id;
    uint32_t team_cnt;	
	ServerInfo(uint32_t svr_id)
		: server_id(svr_id),
		  team_cnt(0)
	{

	}
};

std::map<uint32_t, uint32_t> * TeamServerInfo = NULL;

std::map<uint32_t, ServerInfo*> * ServerTeamCntInfo = NULL;

void reset_server_team_cnt()
{
	for(std::map<uint32_t, ServerInfo*>::iterator it = ServerTeamCntInfo->begin();
			it != ServerTeamCntInfo->end(); ++it) {
		ServerInfo * svr_info = it->second;
		svr_info->team_cnt = 0;
	}
}


uint32_t get_team_server_id(uint32_t team_id)
{
	std::map<uint32_t, uint32_t>::iterator it = TeamServerInfo->find(team_id);
	if (it != TeamServerInfo->end()) {
		return it->second;
	}
	return 0;
}

void save_team_server_info(uint32_t team_id, uint32_t svr_id)
{
	TeamServerInfo->insert(std::map<uint32_t, uint32_t>::value_type(team_id, svr_id));
	int idx = 0;
	uint8_t buf[8] = {0};
	pack_h(buf, team_id, idx);
	pack_h(buf, svr_id, idx);
	send_request_to_cache_svr(0, 0, cache_save_team_server_info, buf, idx);
}

uint32_t auto_find_team_server_id(uint32_t team_id)
{
	for (std::map<uint32_t, ServerInfo*>::iterator it = ServerTeamCntInfo->begin();
			it != ServerTeamCntInfo->end(); ++it) {
		ServerInfo * svr = it->second;
		if (svr->team_cnt < 20) {
			svr->team_cnt += 1;
			save_team_server_info(team_id, svr->server_id);
			return svr->server_id;
		}
	}
	return 0;
}

int  get_server_team_info_from_cachesvr()
{
	int idx = 0;
	char buf[8] = {0};
	pack_h(buf, 100, idx);
	pack_h(buf, 150, idx);
	if (send_request_to_cache_svr(0, 0, cache_get_team_server_info, buf, idx)) {
		return -1;
	}
	return 0;
}

struct basic_team_server_info {
	uint32_t server_id;
	uint32_t team_id;
}__attribute__((packed));


void handle_server_info_form_cache_svr(cachesvr_proto_t * pkg)
{
	int idx = 0;
	uint32_t info_cnt = 0;
	unpack_h(pkg->body, info_cnt, idx);
	if (pkg->len - sizeof(cachesvr_proto_t) >= 4 + 8 * info_cnt) {
		for (uint32_t i = 0; i < info_cnt; i++) {
			basic_team_server_info * info = reinterpret_cast<basic_team_server_info*>(pkg->body
					+ 4 + i * sizeof(basic_team_server_info));
			TeamServerInfo->insert(std::map<uint32_t, uint32_t>::value_type(info->team_id, info->server_id));
			DEBUG_LOG("Team %u SERVER IS %u", info->team_id, info->server_id);
			std::map<uint32_t, ServerInfo*>::iterator it = ServerTeamCntInfo->find(info->server_id);
			if (it != ServerTeamCntInfo->end()) {
				ServerInfo * svr = it->second;
				svr->team_cnt ++;
				DEBUG_LOG("SVR %u Team Cnt Now %u", svr->server_id, svr->team_cnt);
			}
		}
	} else {
		ERROR_LOG("GET TEAM SERVER INFO FORM CACHE SVR CALLBACK FATAL ERROR!");
	}
}

void init_team_contest_server_info()
{

	if (TeamServerInfo == NULL) {
		TeamServerInfo = new std::map<uint32_t, uint32_t>;
	}
	if (ServerTeamCntInfo == NULL) {
		ServerTeamCntInfo = new std::map<uint32_t, ServerInfo*>;
		for (uint32_t i = 100; i <= 150; i++) {
			ServerInfo * svr_info = new ServerInfo(i);
			ServerTeamCntInfo->insert(std::map<uint32_t, ServerInfo*>::value_type(i, svr_info));
		}
	}
}

void clear_out_date_team_contest_info()
{
	time_t now_tv = get_now_tv()->tv_sec;
	struct tm * now = localtime(&now_tv);
	if (now->tm_hour == 0 && now->tm_hour == 0 && now->tm_sec == 0) {
		TeamServerInfo->clear();
	 	reset_server_team_cnt();
		DEBUG_LOG("CLEAR ALL OLD INFO >>>>>>>>>>");
	}
}

