/*
 * data_manipulate.h
 *
 *  Created on:	2011-7-14
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef DATA_MANIPULATE_H_
#define DATA_MANIPULATE_H_
/*
 * 数据操纵
 * 包括
 * 	1连接服务器
 * 	2组织客户端提交的数据
 * 	3将数据发送到服务器
 */

typedef enum type_and_max{
		TYPE_CDN					= 1,
		TYPE_URL					= 2,
		MAX_POINT					= 20,
		MAX_POST_STRING_LEN	= 32,
}type_and_max_t;

typedef struct cdn_speed{
	uint32_t	cdn_ip;			//cdn_ip
	uint32_t	speed;				//下载速率
}__attribute__((packed)) cdn_speed_t;

typedef struct url_probe{
	uint32_t	page_id;				//页面编号
	uint32_t count;					//断点数
	uint32_t	vlist[MAX_POINT];	//断点耗时
}__attribute__((packed)) url_probe_t;

typedef union union_result{
	cdn_speed_t	m_cdn;
	url_probe_t	m_url;
}__attribute__((packed)) union_result_t;

typedef struct fcgi_packet{
	uint32_t	project_number;	//项目编号
	uint32_t	user_id;			//用户编号
	uint32_t	client_ip;		//client_ip
	uint32_t	test_time;		//测试时间
}__attribute__((packed)) fcgi_common_t;

typedef struct store_result{
	uint32_t	type;					//决定发送的协议 cdn 0 0x2000 url 1 0x2001
	uint32_t	project_number;		//项目编号
	uint32_t	user_id;				//用户编号
	uint32_t	client_ip;			//client_ip
	uint32_t	test_time;			//测试时间
	union_result_t record;		//结果
}__attribute__((packed)) store_result_t;


extern int connect_2_analyze_server(int *sockfd);
extern int connect_2_backup_server(int *sockfd);
extern int prepare_data(store_result_t *result);
extern int send_2_server(store_result_t *result, int *sockfd, char *str, uint32_t flag);
//extern int send_2_backup_server(store_result_t *result, int *sockfd);

#endif /* DATA_MANIPULATE_H_ */
