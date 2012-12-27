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

#ifndef  DINING_ROOM_INC
#define  DINING_ROOM_INC

extern "C" {
#include <stdint.h>
}


/*设置餐厅的请求包*/
struct dining_room_in {
	uint32_t exp;
	int32_t xiaomi;
}__attribute((packed));

/*获取餐厅的经验值和收益*/
struct dining_room_out {
	uint32_t userid;
	uint32_t dining_room_exp;
	int32_t dining_room_xiaomi;
}__attribute((packed));

/*更新餐厅的经验值和收益*/
int dining_room_update(char *recbuf, int rcvlen, char *sendbuf, int *sndlen);
/*得到餐厅的经验值和收益*/
int dining_room_obtain(char *recbuf, int rcvlen, char *sendbuf, int *sndlen);

#endif   /* ----- #ifndef TIME_INC  ----- */

