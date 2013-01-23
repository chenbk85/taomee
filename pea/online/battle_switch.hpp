#ifndef BATTLE_SWITCH_HPP_
#define BATTLE_SWITCH_HPP_

#include <libtaomee++/inet/pdumanip.hpp>
extern "C" 
{	
#include <stdint.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/stat_agent/msglog.h>
#include <async_serv/net_if.h>	
}

#include <libtaomee++/proto/proto_util.h>

using namespace taomee;

#include "pea_common.hpp"
#include "fwd_decl.hpp"
#include "proto.hpp"


class player_t;

typedef int (*btlswitch_handle)(player_t *p, btlsw_proto_t *pkg);


void connect_to_battle_switch();
int connect_to_battle_switch_timely(void *owner, void *data);
void init_connect_to_battle_switch();
bool is_battle_switch_fd(int fd);
void close_battle_switch_fd();
int send_to_battle_switch(const player_t* p, uint16_t cmd,
		uint32_t body_len, void* body_buf);
int send_to_battle_switch(const player_t* p, uint16_t cmd, Cmessage * p_out);
void handle_battle_switch_return(btlsw_proto_t* data, uint32_t len);


#endif

