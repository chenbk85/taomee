/*
 * protocol.h
 *
 *  Created on:	2011-7-14
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef	PROTOCOL_H_
#define	PROTOCOL_H_

//下面一部分内容用在choose server中，用来和分析服务器的conn端口通讯，查询系统负载,以选择服务器.
#define	SYMBOL_LEN		8*sizeof(char)			//数据包标识符长度
#define	COMMAND_LEN		8*sizeof(char)			//命令长度
#define	DATA_LEN		sizeof(uint32_t)		//数据长度4字节
#define	FILL_LEN		sizeof(uint32_t)		//填补长度 DES加密需要8字节的整数倍 有时候可能需要填补0
#define	DATA_END_LEN	8*sizeof(char)			//数据结束标识符长度

#define	SYMBOL			"TAOMEEV5"				//数据包标识符
#define	C_GET_LOAD		"GET_LOAD"				//获取服务器负载命令
#define	C_SHOW_LOAD		"SHOWLOAD"				//服务器返回负载的命令
#define	C_SEND_DATA		"SENDDATA"				//发送数据包命令
#define	CONTINUE		"CONTINUE"				//此包后还有数据，数据是否结束标识符
#define	DATA_END		"DATA_END"				//标志一个数据包的结束

//下面部分内容，用在data_manipulate中，用以和服务器的server端口通讯,提交fcgi数据.

typedef struct protocol {
	uint32_t	len;
	uint32_t	seq;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	uint8_t	body[];
} __attribute__((packed)) protocol_t;

enum _proto_fcgi_cmd {
	proto_fcgi_cdn_report						= 0x2001,
	proto_fcgi_url_report						= 0x2002,
};

static inline void
init_proto_head(void* buf, int len, int cmd, uint32_t id, int seq, int ret)
{
#ifdef __cplusplus
	protocol_t* p = reinterpret_cast<protocol_t*>(buf);
#else
	protocol_t* p = buf;
#endif
	p->len = (len);
	p->cmd = (cmd);
	p->id  = (id);
	p->seq = (seq);
	p->ret = (ret);
}
#endif		//PROTOCOL_H_
