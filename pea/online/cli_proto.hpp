/**
 *============================================================
 *  @file      cli_proto.hpp
 *  @brief    client protocol definations and package sending functions
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_CLI_PROTO_HPP_
#define KF_CLI_PROTO_HPP_

#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/project/types.h>
}

#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include <libtaomee++/utils/strings.hpp>

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "player.hpp"
#include "proto.hpp"



extern uint8_t pkgbuf[1 << 21];

extern list_head_t awaiting_playerlist;

void proc_cached_pkgs();

int dispatch(void* data, fdsession_t* fdsess, bool first_tm = true);

void cache_a_pkg(player_t *p, char* buf, uint32_t buflen);

int send_to_player(player_t * p, char* pkg, uint32_t len, int completed);

int send_to_player(player_t * p, Cmessage * p_out,  uint16_t cmd, uint8_t completed);

int send_header_to_player(player_t* p, uint16_t cmd, uint32_t err, int completed);

int send_error_to_player(player_t * p, uint32_t err);

void init_cli_proto_head_full(void* header, userid_t uid, uint32_t seqno, uint16_t cmd, uint32_t len, uint32_t ret);

void init_cli_proto_head(void* header, const player_t* p, uint16_t cmd, uint32_t len);


#endif // KF_CLI_PROTO_HPP_



