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

#include <serverbench/benchapi.h>
#include "globalvar.h"
#include "base.h"
#include "jy_lamu.h"
#include "dining_room.h"



/**
 * @brief  更新餐厅的经验值和收益 
 * @param  recbuf  接收到的字符指针
 * @param  rcvlen  接收到的字符的长度
 * @param  sendbuf 发送的字符串指针
 * @param  sndlen  保存发送的字符串的长度
 * @return 0,成功
 */
int dining_room_update(char *recbuf, int rcvlen, char *sendbuf, int *sndlen)
{
	proto_head_ptr_t proto_data = (proto_head_ptr_t)recbuf;
	uint32_t user_id = proto_data->user_id;
	uint32_t seq = proto_data->result;
	dining_room_in pri_data = *(reinterpret_cast<dining_room_in *>(recbuf + PROTO_HEAD_SIZE));
	*sndlen = 0;
	if (ntohl(*(reinterpret_cast<uint32_t *>(recbuf))) != PROTO_HEAD_SIZE + sizeof(dining_room_in)) {
		ERROR_LOG("the length is wrong %u, userid %u", ntohl(*(uint32_t *)recbuf), user_id);
		init_proto_head(sendbuf, sndlen, 0, 13, seq, user_id, REG_SIZE_ERR);
		return 0;
	}

	if (user_id >= max_user) {
		ERROR_LOG("the user id is too big, %u", user_id);
		init_proto_head(sendbuf, sndlen, 0, 13, seq, user_id, USER_ID_ERR);
		return 0;
	}
	time_mem[user_id].dining_room_exp = pri_data.exp;
	time_mem[user_id].dining_room_xiaomi = pri_data.xiaomi;
	return 0;
}


/**
 * @brief 获取上次家园更新的时间和拉姆的状态
 * @param recbuf  接收到的字符串的指针
 * @param rcvlen  接收到的字符串的长度
 * @param sendbuf 存放发送的字符串指针
 * @param sndlen  保存发送字符串的长度
 * @return 0,成功；其他错误
 */
int dining_room_obtain(char *recbuf, int rcvlen, char *sendbuf, int *sndlen)
{
	uint32_t len = ntohl(*reinterpret_cast<uint32_t *>(recbuf));
	proto_head_ptr_t proto_data;
	proto_data = (proto_head_ptr_t)recbuf;
	uint32_t seq = proto_data->result;
	uint32_t user_id = proto_data->user_id;
	uint32_t user_count = *(reinterpret_cast<uint32_t *>(recbuf + PROTO_HEAD_SIZE));
	if (len != PROTO_HEAD_SIZE + 4 + user_count * 4) {
		ERROR_LOG("receive is wrong, %u userid:%u",len, user_id);
		init_proto_head(sendbuf, sndlen, 0, 14, seq, user_id, REG_SIZE_ERR);
		return 0;
	}
	
	if (user_id > max_user) {
		init_proto_head(sendbuf, sndlen, 0, 14, seq, user_id, USER_ID_ERR);
		return 0;
	}
	//DEBUG_LOG("userid:%u, len:%u, user_count:%u",user_id, len, user_count);
	init_proto_head(sendbuf, sndlen, sizeof(dining_room_out) * user_count + 4, 14, seq, user_id, 0);
	dining_room_out *temp = (dining_room_out *)(sendbuf + sizeof(online_header) + 4);
	*reinterpret_cast<uint32_t *>(sendbuf + online_header_len) = user_count;
	uint32_t *p_userid = reinterpret_cast<uint32_t *>(recbuf + PROTO_HEAD_SIZE + 4);
	for (uint32_t i = 0; i < user_count; i++, p_userid++) {
		if (*p_userid > max_user) {
			init_proto_head(sendbuf, sndlen, 0, 10, seq, user_id, USER_ID_ERR);
			return 0;
		}
		temp->userid = *p_userid;
		temp->dining_room_exp = time_mem[*p_userid].dining_room_exp;
		temp->dining_room_xiaomi = time_mem[*p_userid].dining_room_xiaomi;
		temp++;
	}

	return 0;
}

