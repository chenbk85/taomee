/*
 * write_file.h
 *
 * Created on:	2011-8-4
 * Author:		Singku
 * Paltform:		Linux Fedora Core 8 x86-32
 *	Compiler:		GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef WRITE_FILE_H_
#define WRITE_FILE_H_

#include <libtaomee/list.h>

typedef struct tmr{
	list_head_t timer_list;
} tmr_t;

extern tmr_t g_timer;		//定时将内存数据写到磁盘
extern uint32_t current_version;//当前版本
extern uint32_t data_store_time;	//默认的数据存储时间 分钟
extern char data_store_path[1024];//数据存放路径
extern int write_file();
#endif /* WRITE_FILE_H_ */
