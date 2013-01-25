/*
 * send.h
 *
 *  Created on:	2011-7-5
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef SEND_H_
#define SEND_H_

#include "common.h"

extern uint32_t accept_addr;	//转发目的地址 10.1.1.141
extern uint16_t	accept_port;		//转发目的端口
extern char		des_pass_phrase[PASSWD_LEN];	//DES密码

//发送数据线程函数
extern void data_send();

#endif /* SEND_H_ */
