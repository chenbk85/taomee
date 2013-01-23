#ifndef DBPROXY_HPP_
#define DBPROXY_HPP_

extern "C" 
{
#include <libtaomee/project/types.h>
}

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "proto.hpp"
#include <libtaomee++/proto/proto_util.h>

class player_t;

// extern uint8_t dbpkgbuf[dbproto_max_len];


void connect_to_dbproxy();

int connect_to_dbproxy_timely(void* owner, void* data);


void init_connect_to_dbproxy();

bool is_dbproxy_fd(int fd);

void close_dbproxy_fd();

int  send_to_db(player_t* p, uint32_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, uint32_t body_len);

int send_to_db(const player_t * p, uint32_t id, uint32_t role_tm, uint16_t cmd, Cmessage * p_out);

int send_to_db(const player_t * p, uint16_t cmd, Cmessage * p_out);

void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen);

bool init_db_proto_handles();

bool final_db_proto_handles();






















#endif
