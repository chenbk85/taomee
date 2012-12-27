/**
 * ============================================================================
 * @file    user_time.cpp
 *
 * @brief   实现跟时间戳有关的函数。
 *
 * ompiler gcc version 4.1.2
 *
 * platfomr Debian 4.1.1-12
 *
 * copyright TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ============================================================================
 */



#include <cstring>
#include <cstdlib>

extern "C"{
#include <arpa/inet.h>
#include <stdint.h>
#include <time.h>

#include <libtaomee/interprocess/filelock.h>
#include <libtaomee/crypt/qdes.h>
#include <serverbench/benchapi.h>
}

#include "./pkg/Cclientproto.h"

#include "gamescore.h"
#include "packet.h"
#include "globalvar.h"
#include "base.h"
#include "user_time.h"




/**
 * @brief  更新用户的时间
 * @param  recbuf  接收到的字符指针
 * @param  rcvlen  接收到的字符的长度
 * @param  sendbuf 发送的字符串指针
 * @param  sndlen  保存发送的字符串的长度
 * @return 0,成功
 */
int time_stamp_update(char *recbuf, int rcvlen, char *sendbuf, int *sndlen)
{

	proto_head_ptr_t proto_data;
	uint32_t user_id;
	*sndlen = 0;
	if (ntohl(*(uint32_t *)recbuf) != PROTO_HEAD_SIZE ) {
		ERROR_LOG("the length is wrong, %u", ntohl(*(uint32_t *)recbuf));
		return REG_SIZE_ERR;
	}

	proto_data = (proto_head_ptr_t)recbuf;
	/*get user id*/
	user_id = ntohl(proto_data->user_id);
	if (user_id >= max_user) {
		ERROR_LOG("the user id is too big, %u", user_id);
		return USER_ID_ERR;
	}
	/*set time*/
	time_mem[user_id].nick_time = htonl(time(0));
	DEBUG_LOG("commid 7 userid %u", user_id);
	return 0;
}



/**
 * @brief 根据用户号得到相应的时间
 * @param recbuf  接收到的字符串的指针
 * @param rcvlen  接收到的字符串的长度
 * @param sendbuf 存放发送的字符串指针
 * @param sndlen  保存发送字符串的长度
 * @return 0,成功；其他错误
 */
int time_stamp_obtain(char *recbuf, int rcvlen, char *sendbuf, int *sndlen)
{
	uint32_t count;
	uint32_t *p_user_id;
	obtain_time_resp_t *time_info;
	uint32_t len = ntohl(*(uint32_t *)recbuf);
	uint32_t i;
	if (len >= PROTO_HEAD_SIZE + USER_ID_CNT_SIZE * 2) {
		/*get user total number*/
		count = ntohl(*(uint32_t *)(recbuf + PROTO_HEAD_SIZE));
		if (len != (PROTO_HEAD_SIZE + USER_ID_CNT_SIZE + count * USER_ID_SIZE)) {
			ERROR_LOG("the length is wrong, %u", len);
			return CPROTO_LEN_ERR;
		}
	} else {
		ERROR_LOG("receive is wrong, %u", len);
		return CPROTO_LEN_ERR;
	}
	
	if (set_sndbuf_head(sendbuf, sndlen, (proto_head_ptr_t)recbuf,
	    count * OBTAIN_TIME_RESP_SIZE + USER_ID_CNT_SIZE, SUCCESS) != SUCCESS) {
		ERROR_LOG("the set sendbuf is wrong.");
		return CSYS_ERR;
	}
	*(uint32_t *)(sendbuf + PROTO_HEAD_SIZE) = htonl(count);
	p_user_id = (uint32_t *)(recbuf + PROTO_HEAD_SIZE + USER_ID_SIZE);
	DEBUG_LOG("len:%u, count:%u", len, count);	
	time_info = (obtain_time_resp_t *)(sendbuf + PROTO_HEAD_SIZE + USER_ID_CNT_SIZE);
	for (i = 0; i < count; ++i, ++p_user_id, ++time_info) {
		uint32_t user_id = ntohl(*p_user_id);
		if (user_id >= max_user) {
			ERROR_LOG("invalid user id %u", user_id);
			return USER_ID_ERR;
		}
		time_info->user_id = *p_user_id;
		time_info->time = time_mem[user_id].nick_time;
	}
	
	return 0;
}
