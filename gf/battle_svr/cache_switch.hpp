#ifndef _CACHE_SWITCH_H
#define _CACHE_SWITCH_H

extern "C" {
#include <libtaomee/project/types.h>
#include <stdint.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>
#include <libtaomee/log.h>
#include <libtaomee/project/stat_agent/msglog.h>
#include <async_serv/net_if.h>
}

#include "fwd_decl.hpp"
#include "player.hpp"

enum cache_cmd_t
{
	cache_cmd_init_pkg = 50001,
	cache_cmd_notify_finish_tower_info = 65002,
};


enum {
	cache_proto_max_len = 8 * 1024,
};

#pragma pack(1)
struct cache_switch_proto_t 
{
	uint32_t    len;
	uint32_t    seq;
	uint16_t    cmd;
	uint32_t    ret;
	uint32_t    sender_id;
	uint8_t     body[];
};
#pragma pack()


extern int cache_switch_fd;

void connect_to_cache_switch();

int  send_to_cache_switch(Player* p, uint16_t cmd, uint32_t body_len, void* body_buf, userid_t sender_id);

void handle_cache_switch_return();

int notify_finish_tower_info(Player* p);

#endif
