#include <cstring>
#include <libtaomee++/inet/pdumanip.hpp>
#include <arpa/inet.h>

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
}
#include "utils.hpp"
#include "cache_switch.hpp"
#include "common_op.hpp"
#include "global_data.hpp"
using namespace taomee;


void connect_to_cache_switch()
{
	cache_switch_fd = connect_to_service(config_get_strval("cache_switch_name"),  0, 65535, 1);
	if( cache_switch_fd != -1)
	{
		KDEBUG_LOG(0, "BEGIN CONNECTING TO SWITCH AND SEND BATTLE_SERVER ID");
	}
}

int  send_to_cache_switch(player_t* p, uint16_t cmd, uint32_t body_len, void* body_buf, userid_t sender_id)
{
	static uint8_t cache_pkg[ cache_proto_max_len ] = { 0 };
	if( cache_switch_fd == -1){
		connect_to_cache_switch();
	}
	int len = sizeof(cache_switch_proto_t) + body_len;
	if(  cache_switch_fd == -1 || body_len > int(sizeof(cache_pkg) - sizeof(cache_switch_proto_t))){
		ERROR_LOG("FAILED SENDING PACKAGE TO CACHE SWITCH\t[switch_fd=%d]", cache_switch_fd);
		return -1;
	}

	cache_switch_proto_t* pkg = reinterpret_cast<cache_switch_proto_t*>(cache_pkg);
	pkg->len = len;
	pkg->seq = (p ? ((p->fd) << 16) | (p->waitcmd) : 0); // set higher 16 bytes fd and lower 16 bytes waitcmd
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->sender_id  = sender_id;

	memcpy(pkg->body, body_buf, body_len);
	if (p) {
		KDEBUG_LOG(p->id, "SEND REQ TO  BATTLE SWITCH\t[uid=%u cmd=%u waitcmd=%u]", p->id, cmd, p->waitcmd);
	}
	return net_send(cache_switch_fd, cache_pkg, len);
}

void handle_cache_switch_return(cache_switch_proto_t* data, uint32_t len)
{
	uint16_t waitcmd = data->seq & 0xFFFF; // lower 16 bytes
	int   connfd  = data->seq >> 16; //higher 16 bytes
	
	player_t* p = get_player_by_fd(connfd);
	if (!p || p->waitcmd != waitcmd) {
		ERROR_LOG("process connection to switch error\t[uid=%u cmd=%u seq=%X]",
			data->sender_id, waitcmd, data->seq);
		return; 							
	}
//	uint32_t bodylen = data->len - sizeof(battle_switch_proto_t);
	TRACE_LOG("%u %u %u %u", p->id, p->waitcmd, data->cmd, data->ret);
	switch (data->cmd) {
		case cache_cmd_get_hunter_top_info:
			cache_get_hunter_top_info_callback(p, data, len);
			break;
		default:
			WARN_LOG("btl sw cmdid not existed: %u", data->cmd);
			break;
	}

}

int sent_get_hunter_top_rsp(player_t* p)
{
	int idx = 0;
	pack_h(cachepkgbuf, p->role_tm, idx);
	return send_to_cache_switch(p, cache_cmd_get_hunter_top_info, idx, cachepkgbuf, p->id);
}


int cache_get_hunter_top_info_callback(player_t* p, cache_switch_proto_t* bpkg, uint32_t len)
{
	hunter_top_header_t* hunter_header = reinterpret_cast<hunter_top_header_t*>(bpkg->body);
	
	 CHECK_VAL_EQ(len, sizeof(cache_switch_proto_t) + sizeof(hunter_top_header_t) + hunter_header->cnt * sizeof(hunter_top_info));

	 CHECK_VAL_GE(len, sizeof(cache_switch_proto_t) + sizeof(hunter_top_header_t) + 1 * sizeof(hunter_top_info));
	
	 hunter_top_info * hunter = reinterpret_cast<hunter_top_info*>(hunter_header->data);
	 int idx = sizeof(cli_proto_t);
	 if (hunter[0].layer) {
		 pack(pkgbuf, hunter[0].layer, idx);
		 pack(pkgbuf, hunter[0].interval, idx);
	 } else {
	 	pack(pkgbuf, p->fumo_tower_top, idx);
		pack(pkgbuf, p->fumo_tower_used_tm, idx);
	 }
	 pack(pkgbuf, static_cast<uint32_t>(hunter[0].order), idx);
	 pack(pkgbuf, hunter_header->cnt - 1, idx);

	 TRACE_LOG("%u self: %u %u %u %u ", hunter_header->cnt, p->id, hunter[0].layer, hunter[0].interval, hunter[0].order);
	
	 for (uint32_t i = 1; i < hunter_header->cnt; i++) {
		 pack(pkgbuf, hunter[i].userid, idx);
		 pack(pkgbuf, hunter[i].roletm, idx);
		 pack(pkgbuf, hunter[i].role_type, idx);
		 pack(pkgbuf, hunter[i].nick, sizeof(hunter->nick), idx);
		 pack(pkgbuf, hunter[i].layer, idx);
		 pack(pkgbuf, hunter[i].interval, idx);
		 pack(pkgbuf, hunter[i].userlv, idx);
		 TRACE_LOG("other: %u %u %u %s | %u %u %u ",  hunter[i].userid, hunter[i].roletm, hunter[i].role_type, 
		 	hunter[i].nick, hunter[i].layer, hunter[i].interval, hunter[i].userlv);
	 }
	 init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	 return send_to_player(p, pkgbuf, idx, 1);
}

