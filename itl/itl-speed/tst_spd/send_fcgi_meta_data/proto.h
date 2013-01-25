#ifndef _PROTO_H
#define _PROTO_H


#include <arpa/inet.h>
#include <stdint.h>

#define MAX_POINT		20

enum _proto_fcgi_cmd {
	proto_fcgi_cdn_report						= 0x2001,
	proto_fcgi_url_report						= 0x2002,
};

typedef struct protocol {
	uint32_t	len;
	uint32_t	seq;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	uint8_t	body[];
} __attribute__((packed)) protocol_t;


typedef struct fcgi_cdn{
	uint32_t	cdn_ip;			//cdn_ip
	uint32_t	speed;				//下载速率					0x2001
}__attribute__((packed)) fcgi_cdn_t;

typedef struct fcgi_url{
	uint32_t	page_id;				//页面编号				0x2002
	uint32_t count;					//断点数
	uint32_t	vlist[MAX_POINT];	//断点耗时
}__attribute__((packed)) fcgi_url_t;

typedef union union_result{
	fcgi_cdn_t	m_cdn;
	fcgi_url_t	m_url;
}__attribute__((packed)) union_result_t;

typedef struct fcgi_common{
	uint32_t	project_number;	//项目编号
	uint32_t	user_id;			//用户编号
	uint32_t	client_ip;		//client_ip
	uint32_t	test_time;		//测试时间
}__attribute__((packed)) fcgi_common_t;

typedef struct fcgi_store{
	uint32_t type;				//在内存中标识是CDN还是URL
	uint32_t special_flag;	//说明这个数据是特殊数据，是没有往分析服务器发送成功的，需要另外专门存储
	uint32_t	project_number;	//项目编号
	uint32_t	user_id;			//用户编号
	uint32_t	client_ip;		//client_ip
	uint32_t	test_time;		//测试时间
	union_result_t record;	//结果
}__attribute__((packed)) fcgi_store_t;


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


#endif
