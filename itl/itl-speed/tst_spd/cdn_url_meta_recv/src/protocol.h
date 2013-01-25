/*
 * proto.h
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef PROTO_H_
#define PROTO_H_

#define	FILE_HEAD_LEN	8*sizeof(char)			//文件头部长度
#define	SYMBOL_LEN		8*sizeof(char)			//数据包标识符长度
#define	COMMAND_LEN		8*sizeof(char)			//命令长度
#define	DATA_LEN		sizeof(uint32_t)		//数据长度4字节
#define	FILL_LEN		sizeof(uint32_t)		//填补长度 DES加密需要8字节的整数倍 有时候可能需要填补0
#define	DATA_END_LEN	8*sizeof(char)			//数据结束标识符长度

#define	SYMBOL				"TAOMEEV5"			//数据包标识符
#define	C_SEND_DATA		"SENDDATA"			//发送数据包命令
#define	C_DATA_OK			"DATA__OK"			//服务器响应"数据存储完毕"
#define	C_WITH_NAME		"WITHNAME"			//文件名数据包
#define	CONTINUE			"CONTINUE"			//此包后还有数据，数据是否结束标识符
#define	DATA_END			"DATA_END"			//标志一个数据包的结束

#include <stdint.h>

/*
 * @enum number_related
 * @brief 常用的宏定义数值用enum中的元素来表示
 */
enum number_related{
	ALIGN_LEN					= 8,
	PASSWD_LEN				= 16,
	FILE_NAME_LEN			= 256,
	FULL_PATH_LEN			= 1024,
	TEXT_LINE_LEN			= 4096,
	NET_BUFFER_LEN			= 1024000,
};

/*
 * @typedef store_result_t
 * @brief 存储探测结果的结构体
 */
typedef struct store_result{
	uint32_t			processed	;		//是否已经处理
	uint32_t			machine_number;		//探测机器编号
	uint32_t			probe_time;			//探测时间
	uint32_t			probed_ip;			//探测的IP
	uint32_t			reachable;			//能否Ping通，//只需要true or false即可 但是为了内存对齐定义为4B
	uint32_t			avg_latency;		//平均延时,即ping值
	uint32_t			hop;				//跳数
	uint32_t			loss_percentage;	//丢包率
	uint32_t			previous_hop;		//上一跳路由IP
	struct	store_result	*next;
}__attribute__((packed)) store_result_t;

#endif /* PROTO_H_ */
