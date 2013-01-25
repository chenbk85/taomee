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

/*
 * @typedef file_t
 * @brief 当目标目录存在多个文件时，将这些文件名全部读入，循环处理
 */
typedef struct file{
	char		filename[FILE_NAME_LEN];		//N0000.year-month-day-hour-min==>N0000.2011-05-25-13-14'\0'
	char		fullpath[FULL_PATH_LEN];		//文件全路径名
	struct file *next;							//下一个文件地址
}__attribute__((packed)) file_t;



extern server_ip_t	*analyze_server_set;		//服务器列表
extern char			analyze_server_domainname[DOMAIN_NAME_LEN];//服务器域名
extern uint16_t		server_conn_port;			//服务器通信端口
extern uint16_t		server_analyze_port;		//服务器分析端口
extern char			des_pass_phrase[PASSWD_LEN];		//DES密码
extern uint8_t		if_ddns;					//是否解析域名
extern int			epoll_time_out;				//连接服务器的超时

//发送数据线程函数
extern void* data_send();

#endif /* SEND_H_ */
