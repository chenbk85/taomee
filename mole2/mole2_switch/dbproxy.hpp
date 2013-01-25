#ifndef SWITCH_DBPROXY_HPP_
#define SWITCH_DBPROXY_HPP_

#include "proto.hpp"

/**
  * @brief command id for db proxy
  */
enum db_cmd_t {
	dbcmd_offline_msg	= 0x0920,
	dbcmd_add_val		= 0xD431,
	dbcmd_get_val		= 0xD432,
};

int send_request_to_db(userid_t id, uint16_t cmd, const void* body_buf, int body_len);
void handle_db_return(svr_proto_t* dbpkg, uint32_t pkglen);
int init_db_proto_handles();

extern int proxysvr_fd;

#endif // SWITCH_DBPROXY_HPP_

