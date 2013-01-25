/*
 * common.h 全局结构体定义
 *
 *  Created on:	2011-7-4
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#ifndef COMMON_H_
#define COMMON_H_

#define	FILE_HEAD_SYMBOL	"TAOMEEV5"				//文件头部标识
#define	F_SYMBOL_LEN		8 * sizeof(char)		//文件头部标识的长度

// needed for uintxx_t
#include <stdint.h>

/*
 * @enum number_related
 * @brief 将需要用的宏定义的数值用枚举元素来代替
 */
enum number_related{
	REALLY_UPDATE_FOLDER	= 0,
	TEST_FOLDER_EXISTENCE	= 1,
	ALIGN_LEN				= 8,
	PASSWD_LEN				= 16,
	FOLDER_NAME_LEN			= 17,
	FILE_NAME_LEN			= 24,
	DOMAIN_NAME_LEN			= 256,
	FULL_PATH_LEN			= 1024,
	TEXT_LINE_LEN			= 4096,
	NET_BUFFER_LEN			= 8192,
};

/*
 * @typedef time_string_t
 * @brief 公共的时间字符串结构 每轮循环时用get_time_string更新 打印日志时打印这个时间
 */
typedef struct time_string{
	char	year[5];
	char	month[3];
	char	day[3];
	char	hour[3];
	char	min[3];
	char	secs[3];
	char	result_string[24];	//存储分钟文件名
	char	fullstring[24];		//错误日志中的详细时间
}__attribute__((packed)) time_string_t;

/*
 * @typedef server_ip_t
 * @brief 存储待发送数据的服务器IP链.每次在这些IP中选择一个负载较低的服务器发送
 */
typedef struct server_ip{
	uint32_t			ipvalue;
	struct server_ip* next;
}__attribute__((packed)) server_ip_t;

/*
 * @typedef ip_addr_t
 * @brief 待探测的IP地址结构体
 */
typedef struct ip_addr{
	uint32_t		ipvalue;	//整型32位地址
	uint8_t			ip[4];		//ip的4个字节
	struct ip_addr *next;
}__attribute__((packed)) ip_addr_t;

/*
 * @typedef probe_result_t
 * @brief 探测结果结构体,保存每个ip的探测结果
 */
typedef struct probe_result{
	uint32_t			machine_number;		//探测机器编号
	uint32_t			probe_time;			//探测时间
	uint32_t			probed_ip;			//探测的IP
	uint32_t			reachable;			//能否Ping通，//只需要true or false即可 但是为了内存对齐定义为4B
	uint32_t			avg_latency;		//平均延时,即ping值
	uint32_t			hop;				//跳数
	uint32_t			loss_percentage;	//丢包率
	uint32_t			previous_hop;		//上一跳路由IP
	struct	probe_result	*next;
	uint32_t			end_ip;				//终止IP地址 如果ping通 则end_ip=probed_ip
} __attribute__((packed)) probe_result_t;

//存储探测结果的结构体 比探测结构体少了一个end_ip存储的时候不需要这个成员
typedef struct store_result{
	uint32_t			processed;			//是否处理标记
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

/*
 * @typedef file_header_t
 * @brief 存储的文件头部结构定义
 */
typedef struct file_head{
	char			symbol[8];				//读取8字节长度标识
	uint32_t		create_time;			//文件创建时间
	uint32_t		last_modify;			//最后修改时间
	uint32_t		total_record;			//文件中的总记录数
	uint32_t		processed_record;		//文件中总共处理了多少记录
}__attribute__((packed)) file_header_t;


extern uint32_t			machine_number;			//机器编号
extern time_string_t 	timestring;				//打印日志的时间，每次循环中更新
extern char 			data_store_path[FULL_PATH_LEN];	//数据存储路径

#endif /* COMMON_H_ */
