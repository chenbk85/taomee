#ifndef SWITCH_DBPROXY_HPP_
#define SWITCH_DBPROXY_HPP_

#include "switch.hpp"

/**
  * @brief command id for db proxy
  */
enum db_cmd_t 
{
	dbcmd_offline_msg	= 0x0704,
	dbcmd_send_mail		= 0x199F,
};

#pragma pack(1)

/**
 * @brief db protocol type
 */
struct db_proto_t {
	/*! package length */
	uint32_t    len;
	/*! sequence number ((p->fd << 16) | p->waitcmd) */
	uint32_t    seq;
	/*! command id */
	uint16_t    cmd;
	/*! errno */
	uint32_t    ret;
	/*! user id */
	userid_t    id;
	/*! role create time */
	uint32_t    role_tm;
	/*! package body */
	uint8_t     body[];
};

#pragma pack()

int send_request_to_db(userid_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, int body_len);

extern int proxysvr_fd;

#endif // SWITCH_DBPROXY_HPP_

