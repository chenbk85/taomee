#include <cstring>
#include <libtaomee++/inet/pdumanip.hpp>
#include <arpa/inet.h>

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
}

#include "cache_switch.hpp"

using namespace taomee;

int cache_switch_fd = -1;
/*! for packing protocol data and send to battle switch. 2M */  
uint8_t cachepkgbuf[1 << 21];


void connect_to_cache_switch()
{
	cache_switch_fd = connect_to_service(config_get_strval("cache_switch_name"),  0, 65535, 1);
	if( cache_switch_fd != -1)
	{
		DEBUG_LOG("BEGIN CONNECTING TO SWITCH AND SEND BATTLE_SERVER ID");
	}
}

int  send_to_cache_switch(Player* p, uint16_t cmd, uint32_t body_len, void* body_buf, userid_t sender_id)
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
		DEBUG_LOG("SEND REQ TO  BATTLE SWITCH\t[uid=%u cmd=%u waitcmd=%u]", p->id, cmd, p->waitcmd);
	}
	return net_send(cache_switch_fd, cache_pkg, len);
}

void handle_cache_switch_return()
{

}


int notify_finish_tower_info(Player* p)
{
	uint32_t interval = get_now_tv()->tv_sec - p->score.start_tm;
	TRACE_LOG("uid=%u, %u %u", p->id, p->cur_tower_layer, interval);
    int idx = 0;
	pack_h(cachepkgbuf, 0, idx);
    pack_h(cachepkgbuf, p->id, idx);
    pack_h(cachepkgbuf, p->role_tm, idx);
    pack_h(cachepkgbuf, p->role_type, idx);
    pack_h(cachepkgbuf, p->cur_tower_layer, idx);
    pack_h(cachepkgbuf, interval, idx);
    pack_h(cachepkgbuf, static_cast<uint32_t>(p->lv), idx);
	pack(cachepkgbuf, p->nick, sizeof(p->nick), idx);

	return send_to_cache_switch(0, cache_cmd_notify_finish_tower_info, idx, cachepkgbuf, p->id);
}

