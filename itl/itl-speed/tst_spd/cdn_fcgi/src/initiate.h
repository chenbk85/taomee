/*
 * initiate.h
 *
 *  Created on:	2011-7-14
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef		INITIATE_H_
#define		INITIATE_H_

#include <stdint.h>
//时间字符串结构
typedef struct time_string{
	char	year[5];
	char	month[3];
	char	day[3];
	char	hour[3];
	char	min[3];
	char	secs[3];
	char	result_string[24];	//存储分钟文件名
	char	fullstring[24];		//错误日志中的详细时间
}time_string_t;

//存储待发送数据的服务器IP链.
typedef struct server_ip{
	uint32_t		ipvalue;
	struct server_ip* next;
}server_ip_t;

extern server_ip_t		*analyze_server_set;
extern char				analyze_server_domainname[256];
extern uint16_t			server_analyze_port;
extern uint16_t			server_conn_port;
extern uint32_t			data_backup_server;
extern uint16_t			data_backup_port;
extern char				des_pass_phrase[16];
extern uint8_t			if_ddns;
extern uint8_t			if_choose_server;
extern uint8_t			if_log_data;
extern int				time_out;
extern time_string_t	timestring;


/*
 * @brief 得到当前运行时时间，以便打印错误日志时记录时间
 *
 */
extern void get_time_string();

/*
 *
 *  @brief 初始化函数，包括得到时间串，路径前缀，以及调用load_config加载配置文件
 */
extern int initiate(char *path);

/*
 * 
 *  @brief 关闭日志 释放server_set
 */
extern void destroy();

#endif			//INITIATE_H_
