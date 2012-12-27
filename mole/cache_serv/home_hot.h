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

#ifndef  HOME_HOT_INC
#define  HOME_HOT_INC

extern "C" {
#include <stdint.h>
}

struct home_hot {
	uint32_t userid;
	uint32_t hot;
}__attribute__((packed));
/*更新家园时间和拉姆状态*/
int home_hot_update(char *recbuf, int rcvlen, char *sendbuf, int *sndlen);
/*得到家园上次更新时间和拉姆的状态*/
int home_hot_obtain(char *recbuf, int rcvlen, char *sendbuf, int *sndlen);

#endif   /* ----- #ifndef TIME_INC  ----- */

