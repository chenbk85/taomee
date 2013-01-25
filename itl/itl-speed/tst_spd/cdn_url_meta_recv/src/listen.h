/*
 * listen.h
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef LISTEN_H_
#define LISTEN_H_

#include <stdint.h>
// 收数据端监听端口
extern uint16_t	server_conn_port;

// 收数据时同时开启的线程数
extern volatile int thread_count;

// 服务器绑定的IP地址
extern char bind_ip[20];
/*
 * @brief 监听线程函数
 */
extern void start_listen();

#endif /* LISTEN_H_ */
