/*
 * proto.h 分钟文件存储的协议，文件头定义、向服务器发送文件的协议定义
 *
 *  Created on:	2011-7-4
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef PROTO_H_
#define PROTO_H_

#define	P_SYMBOL_LEN	8*sizeof(char)		//数据包标识符长度
#define	COMMAND_LEN		8*sizeof(char)		//命令长度
#define	DATA_LEN		sizeof(uint32_t)	//数据长度4字节
#define	FILL_LEN		sizeof(uint32_t)	//填补长度 DES加密需要8字节的整数倍 有时候可能需要填补0
#define	FILE_HEAD_LEN	8*sizeof(char)		//文件头的长度
#define	DATA_END_LEN	8*sizeof(char)		//数据结束标识符长度

#define	SYMBOL			"TAOMEEV5"			//数据包标识符
#define	C_GET_LOAD		"GET_LOAD"			//获取服务器负载命令
#define	C_SHOW_LOAD		"SHOWLOAD"			//服务器返回负载的命令
#define	C_SEND_DATA		"SENDDATA"			//发送数据包命令
#define C_DATA_OK		"DATA__OK"			//服务响应数据已经存储
#define	CONTINUE		"CONTINUE"			//此包后还有数据，数据是否结束标识符
#define	DATA_END		"DATA_END"			//标志一个数据包的结束


#endif /* PROTO_H_ */
