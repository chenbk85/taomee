#ifndef _PROTO_H
#define _PROTO_H


#include <arpa/inet.h>
#include <stdint.h>

#define proto_max_len   3*1024 * 1024

#define	MAX_POINT			20

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

typedef struct fcgi_packet{
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


#define UNPKG_UINT(pkg, val, idx) \
		do { \
			switch ( sizeof(val) ) { \
			case 1: (val) = *(uint8_t*)((pkg) + (idx)); (idx) += 1; break; \
			case 2: (val) = ntohs( *(uint16_t*)((pkg) + (idx)) ); (idx) += 2; break; \
			case 4: (val) = ntohl( *(uint32_t*)((pkg) + (idx)) ); (idx) += 4; break; \
			} \
		} while (0)

#define UNPKG_UINT8(b, v, j) \
		do { \
			(v) = *(uint8_t*)((b)+(j)); (j) += 1; \
		} while (0)

#define UNPKG_UINT16(b_, v_, j_) \
		do { \
			(v_) = ntohs(*(uint16_t*)((b_)+(j_))); (j_) += 2; \
		} while (0)

#define UNPKG_UINT32(b, v, j) \
		do { \
			(v) = ntohl( *(uint32_t*)((b)+(j)) ); (j) += 4; \
		} while (0)

#define UNPKG_UINT64(b, v, j) \
		do { \
			(v) = bswap_64( *(uint64_t*)((b)+(j)) ); (j) += 8; \
		} while (0)

#define UNPKG_STR(b, v, j, l) \
		do { \
			memcpy((v), (b)+(j), (l)); (j) += (l); \
		} while (0)

#define UNPKG_H_UINT32(b, v, j) \
		do { \
			(v) = *(uint32_t*)((b)+(j)); (j) += 4; \
		} while (0)

#define UNPKG_H_UINT16(b, v, j) \
		do { \
			(v) = *(uint16_t*)((b)+(j)); (j) += 2; \
		} while (0)

#define UNPKG_H_UINT8(buf_, val_, idx_) UNPKG_UINT8((buf_), (val_), (idx_))

#define PKG_UINT(pkg, val, idx) \
		do { \
			switch ( sizeof(val) ) { \
			case 1: *(uint8_t*)((pkg) + (idx)) = (val); (idx) += 1; break; \
			case 2: *(uint16_t*)((pkg) + (idx)) = htons(val); (idx) += 2; break; \
			case 4: *(uint32_t*)((pkg) + (idx)) = htonl(val); (idx) += 4; break; \
			} \
		} while (0)

#define PKG_UINT8(b, v, j) \
		do { \
			*(uint8_t*)((b)+(j)) = (v); (j) += 1; \
		} while (0)

#define PKG_UINT16(b, v, j) \
		do { \
			*(uint16_t*)((b)+(j)) = htons(v); (j) += 2; \
		} while (0)

#define PKG_UINT32(b, v, j) \
		do { \
			*(uint32_t*)((b)+(j)) = htonl(v); (j) += 4; \
		} while (0)

#define PKG_UINT64(b, v, j) \
		do { \
			*(uint64_t*)( (b)+(j) ) = bswap_64( (v) ); (j) += 8; \
		} while (0)

#define PKG_H_UINT8(b, v, j) \
		do { \
			*(uint8_t*)((b)+(j)) = (v); (j) += 1; \
		} while (0)
		
#define PKG_H_UINT16(b, v, j) \
		do { \
			*(uint16_t*)((b)+(j)) = (v); (j) += 2; \
		} while (0)

#define PKG_H_UINT32(b, v, j) \
		do { \
			*(uint32_t*)((b)+(j)) = (v); (j) += 4; \
		} while (0)

#define PKG_H_UINT64(b, v, j) \
		do { \
			*(uint64_t*)((b)+(j)) = (v); (j) += 8; \
		} while (0)

#define PKG_STR(b, v, j, l) \
		do { \
			memcpy((b)+(j), (v), (l)); (j) += (l); \
		} while (0)

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
