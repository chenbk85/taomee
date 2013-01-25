/** 
 * ========================================================================
 * @file itl_proto.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-04
 * Modify $Date: 2012-10-17 10:29:55 +0800 (三, 17 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_PROTO_H_2012_07_04
#define H_PROTO_H_2012_07_04



#include <stdint.h>
#include <libtaomee++/proto/proto_base.h>

#include "async_server.h"
#include "define.h"


#pragma pack(1)


#define PROTO_MAX_LEN   (PKG_BUF_SIZE)

struct itl_proto_t
{
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
	/*! package body */
    char        body[];
};


typedef itl_proto_t node_proto_t;
typedef itl_proto_t head_proto_t;
typedef itl_proto_t rrd_proto_t;
typedef itl_proto_t db_proto_t;
typedef itl_proto_t alarm_proto_t;
typedef itl_proto_t update_proto_t;
typedef itl_proto_t control_proto_t;


struct cached_pkg_t 
{
	uint32_t    len;
    char        pkg[];
};


#pragma pack()







int net_send_ser_msg(int fd, const void * head_buf, Cmessage * msg);


int net_send_cli_msg(int fd, const void * head_buf, Cmessage * msg);


#define INIT_PROTO_HEADER(proto_name)   \
    int init_ ## proto_name ## _proto_header(void * buf, uint32_t len, uint32_t seq, uint16_t cmd, uint32_t ret, uint32_t id)

INIT_PROTO_HEADER(head);
INIT_PROTO_HEADER(node);
INIT_PROTO_HEADER(db);
INIT_PROTO_HEADER(rrd);
INIT_PROTO_HEADER(alarm);
INIT_PROTO_HEADER(update);
INIT_PROTO_HEADER(control);

#undef INIT_PROTO_HEADER

#endif
