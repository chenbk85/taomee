/**
 *====================================================================
 *  @file        		mole_bank.c
 *  @brief     		This file is to define the functions that handle the request of the client
 * 				and the respond from the dbproxy.
 *
 *  compiler   		gcc  4.1.2
 *  platform   		Debian GNU/LINUX
 *  author      		Kal, Develop Group 1
 *  create date	2009.2.26
 *  copyright  		TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *====================================================================
*/

#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <statistic_agent/msglog.h>

#include "util.h"
#include "sprite.h"
#include "protoheaders.h"
#include "proto.h"
#include "dbproxy.h"
#include "mole_bank.h"

/**
  * @brierf	handle the client's get all infomation request.
  * @param	p. sprite infomation, uid is valid in this struct anyway.
  * @param 	body. req	uest body
  * @param	len. request body length
  * @return  	0 on success. -1 on error
  */
int get_deposit_info_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	//check the data
	CHECK_BODY_LEN(bodylen, 0);
	CHECK_VALID_ID(p->id);
	return send_request_to_db(SVR_PROTO_GET_DEPOSIT_INFO, p, 0, NULL, p->id);
}

/**
  * @brierf	handle the client's deposit bean request.
  * @param	p. sprite infomation, uid is valid in this struct anyway.
  * @param 	body. req	uest body
  * @param	len. request body length
  * @return  	0 on success. -1 on error
  */
int deposit_bean_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	//check the data
	CHECK_BODY_LEN(bodylen, 12);
	CHECK_VALID_ID(p->id);

	deposit_t dep;
	int idx = 0;
	UNPKG_UINT32(body, dep.deposit_bean, idx);
	UNPKG_UINT32(body, dep.drawout, idx);
	UNPKG_UINT32(body, dep.deposit_interval, idx);
	dep.deposit_time = 0;

	//filter the error condition
	if ( dep.deposit_bean < 1000 ) {
		return send_to_self_error(p, p->waitcmd, -ERR_XIAOMEE_LESS_THRESHOLD_ERR, 1);
	}
	if ( dep.deposit_interval != 1 && dep.deposit_interval != 2 ) {
		return send_to_self_error(p, p->waitcmd, -ERR_INVALID_TIME_LIMIT_ERR, 1);
	}

	return send_request_to_db(SVR_PROTO_DEPOSIT_BEAN, p, sizeof(deposit_t),
								&dep, p->id);
}

/**
  * @brierf	handle the client's draw bean request.
  * @param	p. sprite infomation, uid is valid in this struct anyway.
  * @param 	body. req	uest body
  * @param	len. request body length
  * @return  	0 on success. -1 on error
  */
int draw_bean_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	//check the data
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID(p->id);

	uint32_t dep_time;
	int idx = 0;
	UNPKG_UINT32(body, dep_time, idx);
	return send_request_to_db(SVR_PROTO_DRAW_BEAN, p, 4,
								&dep_time, p->id);
}


/**
  * @brierf	handle the database respond data.
  * @param	p. sprite infomation, uid is valid in this struct anyway.
  * @param	id. user id
  * @param 	buf. request body
  * @param	len. request body length
  * @return  	0 on success. -1 on error
  */
int get_deposit_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	//check the data
	CHECK_BODY_LEN_GE(len, 8);

	uint32_t cur_time = *(uint32_t*)buf;
	uint32_t count = *(uint32_t*)(buf + 4);
	CHECK_BODY_LEN(len, 8 + count * sizeof(deposit_t));

	//prepare the respond pkg to client
	deposit_t* dep = (deposit_t*)(buf + 8);
	int idx = sizeof(protocol_t);
	uint32_t i;

	PKG_UINT32(msg, cur_time, idx);
	PKG_UINT32(msg, count, idx);
	for ( i = 0; i < count; i++, dep++ ) {
		PKG_UINT32(msg, dep->deposit_bean, idx);
		PKG_UINT32(msg, dep->drawout, idx);
		PKG_UINT32(msg, dep->deposit_time, idx);
		PKG_UINT32(msg, dep->deposit_interval, idx);
	}
	init_proto_head(msg, p->waitcmd, idx);
	return send_to_self(p, msg, idx, 1);

}

/**
  * @brierf	handle the database respond data.
  * @param	p. sprite infomation, uid is valid in this struct anyway.
  * @param	id. user id
  * @param 	buf. request body
  * @param	len. request body length
  * @return  	0 on success. -1 on error
  */
int deposit_bean_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	//check the data
	CHECK_BODY_LEN(len, sizeof(deposit_t));

	deposit_t* dep = (deposit_t*)buf;
	//update the sprite structure's yxb attr, at this point, deposit MUST be successed
	p->yxb -= dep->deposit_bean;

	//prepare the respond pkg to client
	int idx = sizeof(protocol_t);

	PKG_UINT32(msg, dep->deposit_bean, idx);
	PKG_UINT32(msg, dep->drawout, idx);
	PKG_UINT32(msg, dep->deposit_time, idx);
	PKG_UINT32(msg, dep->deposit_interval, idx);
	init_proto_head(msg, p->waitcmd, idx);
	return send_to_self(p, msg, idx, 1);
}

/**
  * @brierf handle the database respond data.
  * @param	p. sprite infomation, uid is valid in this struct anyway.
  * @param	id. user id
  * @param	buf. request body
  * @param	len. request body length
  * @return 	0 on success. -1 on error
  */
int draw_bean_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	//check the data
	CHECK_BODY_LEN(len, 16);


	//update the sprite structure's yxb attr, at this point, draw MUST be successed
	p->yxb += *(uint32_t*)(buf + 4);

	//prepare the respond pkg to client
	int idx = sizeof(protocol_t);

	PKG_UINT32(msg, *(uint32_t*)buf, idx);
	PKG_UINT32(msg, *(uint32_t*)(buf + 4), idx);
	PKG_UINT32(msg, *(uint32_t*)(buf + 8), idx);
	PKG_UINT32(msg, *(uint32_t*)(buf + 12), idx);
	init_proto_head(msg, p->waitcmd, idx);
	uint32_t interest = *(uint32_t *)buf;
    msglog(statistic_logfile, 0x020B1101, get_now_tv()->tv_sec, &interest, sizeof(uint32_t));
	return send_to_self(p, msg, idx, 1);
}



