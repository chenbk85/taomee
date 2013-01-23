#ifndef CLI_PROTO_HPP_
#define CLI_PROTO_HPP_

#include <libtaomee++/inet/pdumanip.hpp>
extern "C" 
{
#include <libtaomee/project/types.h>
#include <libtaomee/project/stat_agent/msglog.h>	
}

#include "pea_common.hpp"
#include "player.hpp"

#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include <libtaomee++/utils/strings.hpp>

#include "proto.hpp"


extern uint8_t pkgbuf[1 << 21];

bool init_cli_proto_handles();

bool final_cli_proto_handles();

int dispatch(void* data, fdsession_t* fdsess, bool cache_cmd_flag = true);

bool is_alloc_player_cmd(uint32_t cmd);

int send_header_to_online(userid_t uid, fdsession_t* fdsess, uint32_t cmd, uint32_t seqno, uint32_t err);

int send_to_player(Player* p, void* pkgbuf, uint32_t len, int completed);

int send_header_to_player(Player* p, uint32_t cmd, uint32_t err, int completed);

void init_btl_proto_head_full(void* header, uint32_t cmd, uint32_t len, uint32_t ret);

void init_btl_proto_head(void* header, uint32_t cmd, uint32_t len);

void init_cli_proto_head_full(void* header, userid_t id, uint16_t cmd, uint32_t ret, uint32_t len);

void init_cli_proto_head(void* header, uint16_t cmd, uint32_t len);

void cache_a_pkg(Player* p, const void* buf, uint32_t buflen);

void proc_cached_pkgs();
#endif
