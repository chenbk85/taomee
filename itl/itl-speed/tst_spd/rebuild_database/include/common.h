#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>
#include "proto.h"

#define	MAX_LINK	256			//最大支持16个IDC 16个IDC 就有16*15=240个link
#define	PROTO_MAX_TAG		0xFFFF0000	//tag的高22位作为mmap标记，低10位作为项目编号
#define	TAG_MASK				0x0000FFFF	//tag掩码

#ifndef	MAX_AREA_RECORD
#define	MAX_AREA_RECORD		8192
#endif

#define	CDN_SAVE_TIME		5
#define	CDN_TIME_INTER		(CDN_SAVE_TIME)

typedef struct cdn_speed_area{
	uint32_t	project_number;		//项目编号
	uint32_t	t_time;			//测试时间
	uint32_t	t_speed;				//下载速率和				0x1002
	uint32_t	t_count;				//测试次数
	uint32_t	time[CDN_TIME_INTER];		//5分钟时间
	uint32_t	speed[CDN_TIME_INTER];		//5分钟速度和
	uint32_t	count[CDN_TIME_INTER];		//5分钟次数
	uint32_t	step;
	uint32_t	isp_key;				//运营商key,根据key可从ipmap找到province_code,city_code,isp ispid
}__attribute__((packed)) cdn_speed_area_t;

typedef struct cdn_speed_ip_area_detail {
	uint32_t	value;
	uint32_t	count;
}__attribute__((packed)) cdn_speed_ip_area_detail_t;

typedef struct cdn_speed_ip{
	uint32_t	offset;				//为hash表记录偏移
	uint32_t	project_number;		//项目编号
	uint32_t	ip;						//cdn_ip
	uint32_t	t_time;			//测试时间
	uint32_t	t_speed;					//下载速率和				0x1001
	uint32_t	t_count;					//测试次数
	uint32_t	time[CDN_TIME_INTER];		//5分钟时间
	uint32_t	speed[CDN_TIME_INTER];		//5分钟速度和
	uint32_t	count[CDN_TIME_INTER];		//5分钟次数
	uint32_t	step;
	uint32_t	isp_key;				//isp key
}__attribute__((packed)) cdn_speed_ip_t;

typedef struct recv_flag{
	uint32_t	idc_flag;
	uint32_t	all_idc_flag;
}__attribute__((packed)) recv_flag_t;

typedef struct link{
	uint32_t	link_no;
	uint32_t	count;
	uint32_t	test_time;
	uint32_t	delay;
	uint32_t	loss;
	uint32_t	hop;
}__attribute__((packed)) link_t;

typedef struct idc_test_area{
	uint32_t	idcid;					//idc编号 也即machine number;
	uint32_t	isp_key;				//isp_key
	uint32_t	test_time;			//测试时间
	uint32_t	delay;					//延迟和					0x1004
	uint32_t	loss;					//丢包率和 却掉百分号		0x1006
	uint32_t	hop;					//跳数和					0x1008
	uint32_t	flag;					//是否发送
	uint32_t	count;					//测试次数
}__attribute__((packed)) idc_test_area_t;

typedef struct idc_test_ip{
	uint32_t	idcid;					//idc编号 也即machine number;
	uint32_t	offset;				//为hash表记录偏移,mmap中只有1024个IP
	uint32_t	idc_master;			//默认值0xFF,不是link ip
	uint32_t	count_link;			//如果是link ip，则表示有多少个其他idc向idc_master发送探测数据
	uint32_t	ip;						//ip数值
	uint32_t	isp_key;				//ispkey
	uint32_t	test_time;			//测试时间
	uint32_t	delay;					//延迟和 				0x1003
	uint32_t	loss;					//丢包率和 却掉百分号	0x1005
	uint32_t	hop;					//跳数和				0x1007
	uint32_t	flag;					//是否发送
	recv_flag_t	ext_flag;			//如果是该ip是link ip,则收包要判断该结构。
	link_t		link_result[MAX_LINK];//如果是linkip 则存link ip的结果
	uint32_t	count;					//测试次数
}__attribute__((packed)) idc_test_ip_t;

typedef struct link_test{
	uint32_t	number;				//链路编号
	uint32_t	test_time;			//测试时间
	uint32_t	delay;					//延迟和 				0x1009
	uint32_t	loss;					//丢包率和 却掉百分号	0x1010
	uint32_t	hop;					//跳数和				0x1011
	uint32_t	count;					//测试次数
}__attribute__((packed)) link_test_t;

typedef struct idc_between_test{
	uint32_t	number;				//idc编号
	uint32_t	test_time;			//测试时间
	uint32_t	delay;					//延迟和 				0x1012
	uint32_t	loss;					//丢包率和 却掉百分号	0x1013
	uint32_t	hop;					//跳数和				0x1014
	uint32_t	count;					//测试次数
}__attribute__((packed)) idc_between_test_t;

typedef struct all_idc_test{
	uint32_t	number;				//idc编号
	uint32_t	test_time;			//测试时间
	uint32_t	delay;					//延迟和 				0x1015
	uint32_t	loss;					//丢包率和 却掉百分号	0x1016
	uint32_t	hop;					//跳数和				0x1017
	uint32_t	count;					//测试次数
}__attribute__((packed)) all_idc_t;

typedef struct url_test{
	uint32_t	project_number;		//项目号
	uint32_t	page_id;				//页面编号
	uint32_t	isp_key;				//ispkey
	uint32_t	test_time;			//测试时间
	uint32_t	count;					//测试次数
	uint32_t	v_count;				//断点数
	uint32_t	vlist[20];			//断点耗时 			0x1018
}__attribute__((packed)) url_test_t;



typedef struct fcgi_cdn{
	uint32_t	cdn_ip;			//cdn_ip
	uint32_t	speed;				//下载速率
}__attribute__((packed)) fcgi_cdn_t;

typedef struct fcgi_url{
	uint32_t	page_id;				//页面编号
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


static inline uint32_t max(uint32_t x, uint32_t y)
{
	return (x > y) ?x :y;
}

static inline uint32_t min(uint32_t x, uint32_t y)
{
	return (x < y) ?x :y;
}

#endif
