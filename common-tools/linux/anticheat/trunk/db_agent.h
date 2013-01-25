#ifndef __DB_AGENT_H__
#define __DB_AGENT_H__


#include "common.h"

#define DEF_CFG_PATH					"./conf/dbagt.conf"

#define DB_AGENT_STATISTICS_INTERVAL	(60)
#define DB_AGENT_STORE_TO_DB_INTERVAL	(1)

#define MAX_ONCE_REPORT_NUM				(100)

/* TODO: config start */

#define MY_IP_ETHX						"eth1"
#define MY_IP							"localhost"
#define MY_PORT							(3306)
#define MY_USER							"db"
#define MY_PASSWD						"fsd23rg8@"
#define MY_DB_NAME						"anticheat"
#define MY_TABLE_BASENAME				"today_anticheat"

/* TODO: config end */


struct dbagt_config_t {
    /** 是否是后台进程 */
    cfg_bool_t  background;
    /** 是否是 debug 模式(debug不关闭0,1,2) */
    cfg_bool_t  debug_mode;

	/* configs for mysql */
	char		my_host[INET_ADDRSTRLEN];
	uint16_t	my_port;

	char		my_user[MAX_DB_USER_LEN];
	char		my_passwd[MAX_DB_PASSWD_LEN];
	char		my_dbname[MAX_DB_NAME_LEN];
	char		my_tab_basename[MAX_TAB_NAME_LEN];

	/* for log */
	char		log_dir[4096];    
	int			max_log_lvl; 
	int			max_log_size;
	int			max_log_file;
	char		log_prefix[128]; 
};
extern struct dbagt_config_t dbagt_config;


struct ill_report_record_t {
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
	double sw_freq;
	/* sw 的频率监控标准 (单位: us) */
	int64_t		sw_freq_limit;
};




#endif /* __DB_AGENT_H__ */
