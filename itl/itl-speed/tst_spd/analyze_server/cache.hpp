/*
 * cache.hpp
 *
 * Created on:	2011-7-26
 * Author:		Singku
 * Paltform:		Linux Fedora Core 8 x86-32
 *	Compiler:		GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef CACHE_HPP_
#define CACHE_HPP_

#define CACHE_TIME		30
#define MAX_PROVINCE	100
#define MAX_CDN_NODE	512
#define MAX_IDC			16


typedef struct idc_test_area_data{
	uint32_t	test_time;			//测试时间
	uint32_t	delay;					//分钟延迟
	uint32_t	loss;					//分钟丢包率
	uint32_t	hop;					//分钟跳数
	uint32_t	count;					//累计次数
}__attribute__((packed)) idc_test_area_data_t;

typedef struct idc_test_area_avg{
	uint32_t	isp_key;				//isp_key
	uint32_t	machine_number;		//IDC编号
	uint32_t	avg_delay;			//分钟avg延迟
	uint32_t	avg_loss;				//分钟avg丢包率
	uint32_t	avg_hop;				//分钟avg跳数
	idc_test_area_data_t data[CACHE_TIME];	//保留近30分钟的数据做缓存
}__attribute__((packed)) idc_test_area_avg_t;


typedef struct idc_test_ip_avg{
	uint32_t	isp_key;				//
	uint32_t	machine_number;		//IDC编号
	uint32_t ipvalue;				//ip
	uint32_t	avg_delay;			//分钟avg延迟
	uint32_t	avg_loss;				//分钟avg丢包率
	uint32_t	avg_hop;				//分钟avg跳数
	idc_test_area_data_t data[CACHE_TIME];	//保留近30分钟的数据做缓存
}__attribute__((packed)) idc_test_ip_avg_t;


typedef struct cdn_speed_area_data{
	uint32_t	test_time;			//测试时间
	uint32_t	speed;					//分钟下载速率
	uint32_t	count;					//累计次数
}__attribute__((packed)) cdn_speed_area_data_t;

typedef struct cdn_speed_area_avg{
	uint32_t project_number;		//项目编号
	uint32_t	isp_key;				//ispkey
	uint32_t	avg_speed;			//分钟avg速率
	cdn_speed_area_data_t data[CACHE_TIME];	//保留近30分钟的数据做缓存
	uint32_t	speed_distribution[CACHE_TIME][6];
}__attribute__((packed)) cdn_speed_area_avg_t;

typedef struct cdn_node_data{
	uint32_t	test_time;			//测试时间
	uint32_t	speed;					//分钟下载速率
	uint32_t	count;					//累计次数
}__attribute__((packed)) cdn_node_data_t;

typedef struct cdn_node{
	uint32_t	project_number;		//项目编号
	uint32_t	node_value;
	uint32_t	isp_key;
	uint32_t	avg_speed;			//分钟avg速率
	cdn_node_data_t data[CACHE_TIME];
	uint32_t	speed_distribution[CACHE_TIME][6];
	struct cdn_node *next;
}__attribute__((packed)) cdn_node_area_avg_t;

typedef struct url_test_area_data{
	uint32_t	test_time;			//测试时间
	uint32_t	count;					//累计次数
	uint32_t	vlist[MAX_POINT];	//断点耗时
}__attribute__((packed)) url_test_area_data_t;

typedef struct url_test_area_avg{
	uint32_t	project_number;		//项目号
	uint32_t	page_id;				//断点编号
	uint32_t	isp_key;
	uint32_t	avg_spent[MAX_POINT];	//分钟avg耗时
	uint32_t	v_count;				//断点数
	url_test_area_data_t data[CACHE_TIME];//保留近30分钟的数据
}__attribute__((packed)) url_test_area_avg_t;


typedef struct cache_cmd{
	int sockfd;
	uint16_t cmd;
	uint32_t project_number;
	uint32_t idcid;
	uint32_t granularity;
	uint32_t pageid;
}cache_cmd_t;

extern pthread_spinlock_t lock_cdn_node;
extern cdn_speed_area_avg_t *cache_cdn;
extern idc_test_ip_avg_t *cache_idc_ip;
extern "C" int update_cache(uint16_t type, void *data);
extern "C" int init_cache();
extern "C" int destroy_cache();
extern "C" int echo_cdn_area(int sockfd, uint16_t cmd, uint32_t project_number, uint32_t granularity);
extern "C" int echo_cdn_node(int sockfd, uint16_t cmd, uint32_t project_number, uint32_t granularity);
extern "C" int echo_idc_area(int sockfd, uint16_t cmd, uint32_t idcid, uint32_t granularity);
extern "C" int echo_idc_ip(int sockfd, uint16_t cmd, uint32_t idcid, uint32_t granularity);
extern "C" int echo_url_area(int sockfd, uint16_t cmd, uint32_t project_number, uint32_t page_id, uint32_t granularity);

#endif /* CACHE_HPP_ */
