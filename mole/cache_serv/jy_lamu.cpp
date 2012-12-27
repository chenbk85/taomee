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



/**
 * @brief  更新家园的时间和更新拉姆的状态
 * @param  recbuf  接收到的字符指针
 * @param  rcvlen  接收到的字符的长度
 * @param  sendbuf 发送的字符串指针
 * @param  sndlen  保存发送的字符串的长度
 * @return 0,成功
 */
int jy_lamu_update(char *recbuf, int rcvlen, char *sendbuf, int *sndlen)
{
	proto_head_ptr_t proto_data = (proto_head_ptr_t)recbuf;
	uint32_t user_id = proto_data->user_id;
	uint32_t seq = proto_data->result;
	jy_lamu_in pri_data = *(reinterpret_cast<jy_lamu_in *>(recbuf + PROTO_HEAD_SIZE));
	*sndlen = 0;
	if (ntohl(*(reinterpret_cast<uint32_t *>(recbuf))) != PROTO_HEAD_SIZE + sizeof(jy_lamu_in)) {
		ERROR_LOG("the length is wrong %u, userid %u", ntohl(*(uint32_t *)recbuf), user_id);
		init_proto_head(sendbuf, sndlen, 0, 9, seq, user_id, REG_SIZE_ERR);
		return 0;
	}

	if (user_id >= max_user) {
		ERROR_LOG("the user id is too big, %u", user_id);
		init_proto_head(sendbuf, sndlen, 0, 9, seq, user_id, USER_ID_ERR);
		return 0;
	}
	//DEBUG_LOG("USERID:%u, type:%u, state:%u",user_id, pri_data.type, pri_data.state);
	/*set time*/
	switch (pri_data.type) {
		case 1:
			time_mem[user_id].jy_time = time(0);
			break;
		case 2:
			time_mem[user_id].lamu_state = pri_data.state;
			break;
		default:
			ERROR_LOG("wrong type.userid %u, type %u", user_id, pri_data.type);
			init_proto_head(sendbuf, sndlen, 0, 10, seq, user_id, OUT_LAMU_STATE);
			break;
	}
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
int jy_lamu_obtain(char *recbuf, int rcvlen, char *sendbuf, int *sndlen)
{
	uint32_t len = ntohl(*reinterpret_cast<uint32_t *>(recbuf));
	proto_head_ptr_t proto_data;
	proto_data = (proto_head_ptr_t)recbuf;
	uint32_t seq = proto_data->result;
	uint32_t user_id = proto_data->user_id;
	uint32_t user_count = *(reinterpret_cast<uint32_t *>(recbuf + PROTO_HEAD_SIZE));
	if (len != PROTO_HEAD_SIZE + 4 + user_count * 4) {
		ERROR_LOG("receive is wrong, %u userid:%u",len, user_id);
		init_proto_head(sendbuf, sndlen, 0, 10, seq, user_id, REG_SIZE_ERR);
		return 0;
	}
	
	if (user_id > max_user) {
		init_proto_head(sendbuf, sndlen, 0, 10, seq, user_id, USER_ID_ERR);
		return 0;
	}
	//DEBUG_LOG("userid:%u, len:%u, user_count:%u",user_id, len, user_count);
	init_proto_head(sendbuf, sndlen, sizeof(jy_lamu_out) * user_count + 4, 10, seq, user_id, 0);
	jy_lamu_out *temp = (jy_lamu_out *) (sendbuf + sizeof(online_header) + 4);
	*reinterpret_cast<uint32_t *>(sendbuf + online_header_len) = user_count;
	uint32_t *p_userid = reinterpret_cast<uint32_t *>(recbuf + PROTO_HEAD_SIZE + 4);
	for (uint32_t i = 0; i < user_count; i++, p_userid++) {
		if (*p_userid > max_user) {
			init_proto_head(sendbuf, sndlen, 0, 10, seq, user_id, USER_ID_ERR);
			return 0;
		}
		temp->userid = *p_userid;
		temp->jy_time = time_mem[*p_userid].jy_time;
		temp->lamu_state = time_mem[*p_userid].lamu_state;
		temp++;
	}

	return 0;
}

/**
 * @brief 设置ONLINE的包头格式
 * @param buf 返回数据保存的空间
 * @sndlen 返回数据的长度
 * @pri_len 返回数据包体的长度
 * @cmd 返回的命令号
 * @uid 返回的用户ID
 * @result 返回的错误码
 */
void init_proto_head(void* buf, int *sndlen, int pri_len, uint32_t cmd, uint32_t seq, userid_t uid, uint32_t result)
{
	online_header *p = (online_header *)(buf);
	*sndlen = online_header_len + pri_len;
	p->len = online_header_len + pri_len;
	p->seq = seq;
	p->commdid = cmd;
	p->userid = uid;
	p->result = result;
}
