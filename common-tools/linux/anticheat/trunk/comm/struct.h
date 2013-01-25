/**
 * @brief common(shared) structers for the comm
 */

#ifndef __STRUCT_H__
#define __STRUCT_H__


#include <stdint.h>
#include <netinet/in.h>

#include "common.h"


/* net-communication packages-structers */
#pragma pack(1)


/* 网络序 */
struct cli_proto_t {
	/*! package length */
	uint32_t    len;
	/*! protocol version */
	uint8_t     ver;
	/*! protocol command id */
	uint32_t    cmd;
	/*! user id */
	uint32_t    uid;
	/*! seqno in client's pkg */
	uint32_t    seqno;
	/*! body of the package */
	uint8_t     body[];
};

struct client_info_t {
	/* 玩家的IP地址 (由cs包的接收者赋值) */
	struct sockaddr_in		addr;
	/* cs包被recv的时间 (由cs包的接收者赋值) */
	struct timeval			recv_time;
};

struct ac_one_cdata_t {
	/* client 信息 */
	struct client_info_t	cinfo;
	/* client pkg head 原始数据 */
	struct cli_proto_t		chead;
};

struct ac_body_t {
	uint32_t				count;
	struct ac_one_cdata_t	cdata[];
};

struct svr_pkg_t {
	/*! package length */
	uint32_t				len;
	/*! protocol version */
	uint32_t				ver;
	/*! protocol command id */
	uint32_t				cmd;
	/*! user id */
	uint32_t				uid;
	/*! seqno in client's pkg */
	uint32_t				seqno;
	/*! body of the package */
	uint8_t     			body[];
};

struct ac_chl_head_t {
	/* 整个 channel pkg 的长度 (本机序) */
	uint32_t				len;
	/* interface 从这个地址 recv 数据 */
	struct sockaddr_in		recv_from;
};

struct ac_chl_pkg_t {
	struct ac_chl_head_t	head;
	struct svr_pkg_t		svr_pkg;
};

struct ill_report_rcd_t {
	/* 业务名 (同时也决定表名) */
	char		svc_name[MAX_SVC_NAME_LEN];
	/* tw 的违规错误码 */
	uint32_t	tw_ill_code;
	/* sw 的违规错误码 */
	uint32_t	sw_ill_code;
	/* 米米号 */
	uint32_t	uid;
	/* 违规命令号 */
	uint32_t	cmd;
	/* 收到违规命令号的时间戳 (单位: 秒) */
	int32_t		cmd_recv_sec;
	/* 网络序 */
	uint32_t	ip;
	/* 网络序 */
	uint16_t	port;
	/* 网络序 */
	uint32_t	reporter_ip;
	/* 网络序 */
	uint16_t	reporter_port;
	/* tw 的上一次间隔 (单位: us) */
	int64_t		tw_last_int;
	/* tw 的间隔监控标准 (单位: us) */
	int64_t		tw_int_limit;
	/* tw 的与时间起点的间隔 (单位: us) */
	int64_t		tw_start_int;
	/* tw 的命令总数 */
	uint64_t	tw_count;
	/* tw 的频率 */
	double		tw_freq;
	/* tw 的频率监控标准 (单位: us) */
	int64_t		tw_freq_limit;
	/* sw 的上一次间隔 (单位: us) */
	int64_t		sw_last_int;
	/* sw 的间隔监控标准 (单位: us) */
	int64_t		sw_int_limit;
	/* sw 的与时间起点的间隔 (单位: us) */
	int64_t		sw_start_int;
	/* sw 的命令总数 */
	uint64_t	sw_count;
	/* sw 的频率 */
	double		sw_freq;
	/* sw 的频率监控标准 (单位: us) */
	int64_t		sw_freq_limit;
};

struct ill_report_t {
	/* 本次同步记录条数 */
	uint32_t					count;
	/* 记录 (共 count 条记录) */
	struct ill_report_rcd_t		record[];
};

#pragma pack()


/* local memory-structers */

struct sw_config_t {
	uint32_t				cmd;
	/* single_watch 两个 cmd 的最小时间间隔 (单位: us) */
	int64_t					min_sw_interval;
	/* single_watch 允许的频率最大值 (单位: 次/秒) */
	double					max_sw_freq;
	/* sw: 当 now - cmd_node->last_tv 大于该值, 就更新该tw的start_tv (单位: 秒), 无数据窗口 */
	int32_t					update_sw_start_interval;
	/* 对cmd的描述 ('\0'结尾) */
	char					label[255];
	/* 保留 */
	char					reserved[128];
};

struct anticheat_config_t {
	/* 配置的状态: 0: 不可用(可能刚建立还没拉到配置 或者 正在更新), 1: 可用;
	 *
	 * 注意: 更新/建立程序有一定的顺序:
	 * 1. 先把 status 改成 DISABLE;
	 * 2. 把除 status 的字段全部更新;
	 * 3. 最后把 status 改成 ENABLE;
	 */
	uint32_t				status;
	/* 所属的业务名, 用于简单的校验; TODO: 加上安全更新机制 */
	char					svc_name[MAX_SVC_NAME_LEN];
	/* 违规信息发送给的db_server的地址 */
	char					db_addr_ip[INET_ADDRSTRLEN];
	/* 违规信息发送给的db_server的端口 */
	uint16_t				db_addr_port;
	/* total_watch 两个 cmd 的最小时间间隔 (单位: us) */
	int64_t					min_tw_interval;
	/* total_watch 允许的频率最大值 (单位: 次/秒) */
	double					max_tw_freq;
	/* tw: 当 now - uid_node->tw.last_tv 大于该值, 就更新该tw的start_tv (单位: 秒), 无数据窗口 */
	int32_t					update_tw_start_interval;
	/* 保留 */
	char					reserved[128];
	/* 已配置的 sw_config 数量 */
	int32_t					sw_config_count;
	/* single_watch 的相关配置 (其中的cmd是按从小到大的顺序排列, 搜索时用二分查找) */
	struct sw_config_t		sw_config[MAX_CONFIG_SW_CMD];
};



#define MAX_PKG_LEN		(64*1024)
#define RCV_BUF_LEN		(2*1024*1024)
#define SND_BUF_LEN		(2*1024*1024)

#endif /* __STRUCT_H__ */
