#ifndef SWITCH_DBPROXY_HPP_
#define SWITCH_DBPROXY_HPP_

#include "proto.hpp"

/**
  * @brief command id for db proxy
  */
enum db_cmd_t {
	dbcmd_offline_msg	= 0x112A
};

int send_request_to_db(userid_t id, uint16_t cmd, const void* body_buf, int body_len);

extern int proxysvr_fd;

#endif // SWITCH_DBPROXY_HPP_

