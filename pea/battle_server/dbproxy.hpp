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

class Player;

void connect_to_dbproxy();

void init_connect_to_dbproxy();

bool is_dbproxy_fd(int fd);

void close_dbproxy_fd();

int  send_request_to_db(Player* p, uint32_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, uint32_t body_len);

int send_request_to_db(const Player * p, uint32_t id, uint32_t role_tm, uint16_t cmd, Cmessage * p_out);

void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen);

bool init_db_proto_handles();

bool final_db_proto_handles();






















#endif
