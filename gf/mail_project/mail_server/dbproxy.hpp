#ifndef SWITCH_DBPROXY_HPP_
#define SWITCH_DBPROXY_HPP_

#include "mail_server.hpp"

/**
  * @brief command id for db proxy
  */
enum db_cmd_t 
{
	dbproto_mail_head_list     = 0x0633,
	dbproto_mail_body          = 0x0636,
};

enum cmp_t 
{
	cmp_must_eq     = 1,
	cmp_must_ge     = 2 
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
	uint32_t    id;
	/*! role create time */
	uint32_t    role_tm;
	/*! package body */
	uint8_t     body[];
};

#pragma pack()

int send_request_to_db(int online_fd, uint32_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, int body_len);

void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen);

bool init_db_proto_handles();

extern int proxysvr_fd;

#endif // SWITCH_DBPROXY_HPP_

