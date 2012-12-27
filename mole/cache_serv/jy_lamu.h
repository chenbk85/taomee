/**
 * ====================================================================
 *  @file       user_time.h
 *
 *  @brief      定义时间戳命令相关的结构体和错误码
 *
 *  platform   Debian 4.1.1-12 
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ====================================================================
 */

#ifndef  JY_LAMU_INC
#define  JY_LAMU_INC

extern "C" {
#include <stdint.h>
}

/*返回给ONLINE的数据包头*/
struct online_header {
	uint32_t len;
	uint32_t seq;
	uint16_t commdid;
	int result;
	uint32_t userid;
}__attribute((packed));


/*设置家园和拉姆的请求包*/
struct jy_lamu_in {
	uint32_t type;
	uint32_t state;
}__attribute((packed));

/*获取家园上次更新时间和拉姆状态的返回包*/
struct jy_lamu_out {
	uint32_t userid;
	uint32_t jy_time;
	uint32_t lamu_state;
}__attribute((packed));
/* online包头的长度 */
const uint32_t online_header_len = sizeof(online_header);
/*更新家园时间和拉姆状态*/
int jy_lamu_update(char *recbuf, int rcvlen, char *sendbuf, int *sndlen);
/*得到家园上次更新时间和拉姆的状态*/
int jy_lamu_obtain(char *recbuf, int rcvlen, char *sendbuf, int *sndlen);
/*设置返回给online的包头*/
void init_proto_head(void* buf, int *sndlen, int pri_len, uint32_t cmd, uint32_t seq, userid_t uid, uint32_t result);

#endif   /* ----- #ifndef TIME_INC  ----- */

