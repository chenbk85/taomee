/**
 * ============================================================================
 * @file   base.cpp
 *
 * @brief  提供字符串处理，判断用户是否存在等基本函数 
 *
 * compiler : gcc version 4.1.2
 *
 * platfomr : Debian 4.1.1-12
 *
 * copyright : TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ============================================================================
 */

#include <cstring>
#include <cstdio>
#include <cstdlib>

extern "C" {
#include <arpa/inet.h>

#include <libtaomee/interprocess/filelock.h>
#include <serverbench/benchapi.h>
}

#include "./pkg/Cclientproto.h"

#include "globalvar.h"
#include "packet.h"
#include "base.h"



/**
 * @brief  去除字符串的前后空格，TAB键和回车键
 * @param  s 指向字符串的指针
 * @return 0
 */
int trim(char *s)
{
	int i, l, r, len;

	/*get the length*/
	for (len = 0; s[len]; len++) {
        ;
    }   
	/*get the numbers of ' ' or '\t' or '\n' at the begin of this string*/
	for (l = 0; s[l] == '\t' || s[l] == ' ' || s[l] == '\n'; l++) {
		;
	}
	if (l == len) {
		s[0] = '\0';
		return 0;
	}
	
	/*get the ' ' or '\t' or '\n' at the end of this string*/
	for (r = len - 1; s[r] == ' ' || s[r] == '\t' || s[r] == '\n'; r--) {
		;
	}
	/*delete ' ' or '\t' or '\n' at the begin and end*/
	for (i = 1; i <= r; i++) {
		s[r - l + 1] = '\0';
	}
	return 0;
}



/**
 * @brief  初始化sendbuf指向的空间
 * @param  sendbuf 发送字符串的指针
 * @param  sndlen  保存发送字符串长度
 * @param  rcvph  接收的字符串指针
 * @param  private_size 在包的头信息基础上加的字符长度
 * @param  ret, 返回给客户端的结果；-1, 失败；0，成功
 */

int set_sndbuf_head(char *sendbuf, int *sndlen, proto_head_t *rcvph, int private_size, int ret)
{
	proto_head_t *proto_data;
	*sndlen = PROTO_HEAD_SIZE + private_size;
	
	proto_data = (proto_head_t *)sendbuf;
	/*initialize memory*/
	memcpy(proto_data, rcvph, PROTO_HEAD_SIZE);
	//proto_data->len = htonl(*sndlen);
    //proto_data->result = htonl(ret);
	proto_data->len = *sndlen;
	proto_data->result = ret;

	return 0;
}



/**
 * @brief   得到用户在排行榜中的位置
 * @param   gm_id, 游戏的ID号
 * @param   user_id, 用户的ID号
 * @param   score, 用户的游戏分数
 * @param   opy_pos, 保存返回的位置
 * @return  0，用户不在排行榜中
 * @return  -1，用户在排行榜中，但新的分数少于原来的分数;
 *          1，用户在排行榜中，位置信息存储在opy_pos中
 */

int is_user_exist(uint32_t gm_id, uint32_t user_id, uint32_t score, int *opy_pos)
{
	return 0;
}
