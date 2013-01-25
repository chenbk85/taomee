#ifndef _PROTO_H
#define _PROTO_H


#include <arpa/inet.h>

#define proto_max_len   3*1024 * 1024
#define	MAX_POINT		20

#ifndef	ISP_LEN
#define	ISP_LEN		64
#endif

enum _proto_as_cmd {
	proto_as_cdn_report_ip						= 0x1001,
	proto_as_cdn_report_cityisp					= 0x1002,
	proto_as_net_stat_report_ip_delay			= 0x1003,
	proto_as_net_stat_report_cityisp_delay	= 0x1004,
	proto_as_net_stat_report_ip_loss			= 0x1005,
	proto_as_net_stat_report_cityisp_loss		= 0x1006,
	proto_as_net_stat_report_ip_hop				= 0x1007,
	proto_as_net_stat_report_cityisp_hop		= 0x1008,
	proto_as_net_stat_report_link_delay		= 0x1009,
	proto_as_net_stat_report_link_loss			= 0x1010,
	proto_as_net_stat_report_link_hop			= 0x1011,
	proto_as_net_stat_report_idc_delay			= 0x1012,
	proto_as_net_stat_report_idc_loss			= 0x1013,
	proto_as_net_stat_report_idc_hop			= 0x1014,
	proto_as_net_stat_report_all_idc_delay	= 0x1015,
	proto_as_net_stat_report_all_idc_loss		= 0x1016,
	proto_as_net_stat_report_all_idc_hop		= 0x1017,
	proto_as_url_stat_page_time					= 0x1018,
	proto_as_url_stat_forward					= 0x1019,
	proto_as_cdn_report_ip_isp					= 0x1201,
	proto_as_net_stat_report_ip					= 0x1211,
	proto_as_net_stat_report_cityisp			= 0x1212,
	proto_as_net_stat_report_link				= 0x1213,
	proto_as_net_stat_report_idc				= 0x1214,
	proto_as_net_stat_report_all_idc			= 0x1215,
};

enum _proto_fcgi_cmd {
	proto_fcgi_cdn_report						= 0x2001,
	proto_fcgi_url_report						= 0x2002,
};

enum _proto_cache_cmd {
	proto_cache_idc_area						= 0x3001,
	proto_cache_idc_ip							= 0x3002,
	proto_cache_cdn_area						= 0x4001,
	proto_cache_cdn_node						= 0x4002,
	proto_cache_url_area						= 0x5001,
};

typedef struct protocol {
	uint32_t	len;
	uint32_t	seq;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	id;
	uint8_t	body[];
} __attribute__((packed)) protocol_t;

typedef struct p_cdn_speed_ip{
	uint32_t	project_number;		//项目编号
	uint32_t	ip;						//cdn_ip
	uint32_t	test_time;			//测试时间
	uint32_t	speed;					//下载速率和			0x1001
	uint32_t	count;					//测试次数
} __attribute__((packed)) proto_cdn_speed_ip_t;

typedef struct p_cdn_speed_ip_isp{
	uint32_t	project_number;		//项目编号
	uint32_t	test_time;			//测试时间
	uint32_t	ip;						//cdn_ip
	uint32_t	province_id;			//省
	uint32_t	city_id;				//市
	uint8_t	isp[ISP_LEN];		//运营商
	uint32_t	speed;					//下载速率和			0x1201
	uint32_t	count;					//测试次数
} __attribute__((packed)) proto_cdn_speed_ip_isp_t;

typedef struct p_cdn_speed_area{
	uint32_t	project_number;		//项目编号
	uint32_t	province_code;		//省编号
	uint32_t	city_code;			//城市编号
	uint8_t	isp[ISP_LEN];				//运营商名称
	uint32_t	test_time;			//测试时间
	uint32_t	speed;					//下载速率和			0x1002
	uint32_t	count;					//测试次数
} __attribute__((packed)) proto_cdn_speed_area_t;

typedef struct p_idc_test_ip_delay{
	uint32_t	idcid;				//idc编号 也即machine number;
	uint32_t	ip;					//ip数值
	uint32_t	test_time;		//测试时间
	uint32_t	delay;				//延迟和 					0x1003
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_test_ip_delay_t;

typedef struct p_idc_test_ip_loss{
	uint32_t	idcid;				//idc编号 也即machine number;
	uint32_t	ip;					//ip数值
	uint32_t	test_time;		//测试时间
	uint32_t	loss;				//丢包率和 却掉百分号		0x1005
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_test_ip_loss_t;

typedef struct p_idc_test_ip_hop{
	uint32_t	idcid;				//idc编号 也即machine number;
	uint32_t	ip;					//ip数值
	uint32_t	test_time;		//测试时间
	uint32_t	hop;				//跳数和					0x1007
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_test_ip_hop_t;

typedef struct p_idc_test_ip{
	uint32_t	idcid;				//idc编号 也即machine number;
	uint32_t	ip;					//ip数值
	uint32_t	test_time;		//测试时间
	uint32_t	delay_v;			//延迟和					0x1211==代替03/05/07
	uint32_t	loss_v;			//丢包和
	uint32_t	hop_v;				//跳数和
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_test_ip_t;

typedef struct p_idc_test_area_delay{
	uint32_t	idcid;				//idc编号 也即machine number;
	uint32_t	province_code;	//省编号
	uint32_t	city_code;		//城市编号
	uint8_t	isp[ISP_LEN];			//运营商名称
	uint32_t	test_time;		//测试时间
	uint32_t	delay;				//延迟和					0x1004
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_test_area_delay_t;

typedef struct p_idc_test_area_loss{
	uint32_t	idcid;				//idc编号 也即machine number;
	uint32_t	province_code;	//省编号
	uint32_t	city_code;		//城市编号
	uint8_t	isp[ISP_LEN];			//运营商名称
	uint32_t	test_time;		//测试时间
	uint32_t	loss;				//丢包和					0x1006
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_test_area_loss_t;

typedef struct p_idc_test_area_hop{
	uint32_t	idcid;				//idc编号 也即machine number;
	uint32_t	province_code;	//省编号
	uint32_t	city_code;		//城市编号
	uint8_t	isp[ISP_LEN];			//运营商名称
	uint32_t	test_time;		//测试时间
	uint32_t	hop;				//跳数和					0x1008
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_test_area_hop_t;

typedef struct p_idc_test_area{
	uint32_t	idcid;				//idc编号 也即machine number;
	uint32_t	province_code;	//省编号
	uint32_t	city_code;		//城市编号
	uint8_t	isp[ISP_LEN];			//运营商名称
	uint32_t	test_time;		//测试时间
	uint32_t	delay_v;			//延迟和
	uint32_t	loss_v;			//丢包和
	uint32_t	hop_v;				//跳数和					0x1212==代替04/06/08
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_test_area_t;


typedef struct p_link_test_delay{
	uint32_t	number;			//链路编号
	uint32_t	test_time;		//测试时间
	uint32_t	delay;				//延迟和 					0x1009
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_link_test_delay_t;

typedef struct p_link_test_loss{
	uint32_t	number;			//链路编号
	uint32_t	test_time;		//测试时间
	uint32_t	loss;				//丢包率和 去掉百分号		0x1010
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_link_test_loss_t;

typedef struct p_link_test_hop{
	uint32_t	number;			//链路编号
	uint32_t	test_time;		//测试时间
	uint32_t	hop;				//跳数和					0x1011
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_link_test_hop_t;

typedef struct p_link_test{
	uint32_t	number;			//链路编号
	uint32_t	test_time;		//测试时间
	uint32_t	delay_v;			//延迟和
	uint32_t	loss_v;			//丢包和
	uint32_t	hop_v;				//跳数和					0x1213==代替09/10/11
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_link_test_t;

typedef struct p_idc_delay{
	uint32_t	number;			//IDC编号
	uint32_t	test_time;		//测试时间
	uint32_t	delay;				//延迟和 					0x1012
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_delay_t;

typedef struct p_idc_loss{
	uint32_t	number;			//IDC编号
	uint32_t	test_time;		//测试时间
	uint32_t	loss;				//丢包率和 却掉百分号		0x1013
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_loss_t;

typedef struct p_idc_hop{
	uint32_t	number;			//IDC编号
	uint32_t	test_time;		//测试时间
	uint32_t	hop;				//跳数和					0x1014
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_hop_t;

typedef struct p_idc{
	uint32_t	number;			//IDC编号
	uint32_t	test_time;		//测试时间
	uint32_t	delay_v;			//延迟和
	uint32_t	loss_v;			//丢包和
	uint32_t	hop_v;				//跳数和					0xxxxx
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_idc_t;


typedef struct p_all_idc_delay{
	uint32_t	test_time;		//测试时间
	uint32_t	delay;				//延迟和 					x1015
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_all_idc_delay_t;

typedef struct p_all_idc_loss{
	uint32_t	test_time;		//测试时间
	uint32_t	loss;				//丢包率和 却掉百分号		0x1016
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_all_idc_loss_t;

typedef struct p_all_idc_hop{
	uint32_t	test_time;		//测试时间
	uint32_t	hop;				//跳数和					0x1017
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_all_idc_hop_t;

typedef struct p_all_idc{
	uint32_t	test_time;		//测试时间
	uint32_t	delay_v;			//延迟和
	uint32_t	loss_v;			//丢包和
	uint32_t	hop_v;				//跳数和					0xxxxx
	uint32_t	count;				//测试次数
} __attribute__((packed)) proto_all_idc_t;

typedef struct p_url_test_fixed{
	uint32_t	project_number;	//项目编号
	uint32_t	page_id;			//页面编号
	uint32_t	provice_code;	//省编号
	uint32_t	city_code;		//市编号
	uint32_t	isp_id;			//isp编号
	uint8_t	isp[ISP_LEN];			//运营商名称
	uint32_t	test_time;		//测试时间
	uint32_t	count;				//记录数
	uint32_t	v_count;			//断点数
} __attribute__((packed)) proto_url_test_fixed_t;

typedef struct p_url_test{
	uint32_t	project_number;	//项目编号
	uint32_t	page_id;			//页面编号
	uint32_t	provice_code;	//省编号
	uint32_t	city_code;		//市编号
	uint32_t	isp_id;			//isp编号
	uint8_t	isp[ISP_LEN];			//运营商名称
	uint32_t	test_time;		//测试时间
	uint32_t	count;				//记录数
	uint32_t	v_count;			//断点数
	uint32_t	vlist[20];		//断点耗时 				0x1018
} __attribute__((packed)) proto_url_test_t;

typedef struct p_url_forward{
	uint32_t	project_number;	//项目编号
	uint32_t	user_id;			//用户编号
	uint32_t	client_ip;		//客户端ip
	uint32_t	test_time;		//测试时间
	uint32_t	page_id;			//页面编号
	uint32_t	v_count;			//断点数
	uint32_t	vlist[20];		//断点耗时 				0x1019
} __attribute__((packed)) proto_url_forward_t;


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
init_proto_head(void* buf, uint32_t len, uint16_t cmd, uint32_t id, uint32_t seq, uint32_t ret)
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
