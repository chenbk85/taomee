#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <stdint.h>
#include <sys/time.h>
#include "common.h"


#define DEF_CFG_PATH					"./conf/test.ac.conf"

#define ILL_CODE_TW_INT					(0x1)
#define ILL_CODE_TW_FREQ				(0x2)

#define ILL_CODE_SW_INT					(0x1)
#define ILL_CODE_SW_FREQ				(0x2)


/* 记录/上报统计间隔, 单位: 秒 */
#define AC_STATISTICS_INTERVAL			(60)
/* 清理已下线 uid 的 hash_table 节点的间隔, 单位: 秒 */
#define AC_CLEAN_UID_NODE_INTERVAL		(1)
/* 检查更新时间起点的间隔, 单位: 秒 (注意: 它同时是tw/sw的cfg-update_start的最小值) */
#define AC_UPDATE_START_INTERVAL		(1)
/* cmd 超过这个时间不发包, 就被清除记录, 单位: 秒 (注意: 它同时是sw的cfg-interval的最大值) */
#define SW_CMD_KEEPALIVE_TIME			(30)

/* 多长时间不发包, 就认为下线了 (单位: 秒) */
#define MAX_UID_KEEPALIVE_TIME			(60)

/* 只有累积多少个 tw 包才开始检查频率 */
#define CHECK_TW_FREQ_MIN_COUNT			(10)
/* 只有累积多少个 sw 包才开始检查频率 */
#define CHECK_SW_FREQ_MIN_COUNT			(3)


/* 清理下线 uid时, 最多一次扫描多少个 hash_table 节点 */
#define AC_CLEAN_UID_NODE_SCAN_LIMIT	(2000)
/* 清理下线 uid时, 最多一次真正处理多少个 hash_table 节点 */
#define AC_CLEAN_UID_NODE_HDL_LIMIT		(1000)

/* 清理教长时间未捕获cmd时, 最多一次扫描多少个 hash_table 节点 */
#define AC_UPDATE_START_SCAN_LIMIT		(2000)
/* 清理教长时间未捕获cmd时, 最多一次处理多少个 tw或sw->cmd_list[i] 节点;
 * 由于cmd的更新时间起点工作较清, 因此每次可执行的数量较多;
 * 但要注意: 每次一定要完整的清理掉一个 uid 才能退出循环, 不能达到这个设定就立刻退出 */
#define AC_UPDATE_START_HDL_LIMIT		(2000)


/* uid hash_table 相关 */
#define UID_NODE_SIZE					(1*1024)
#define MAX_UID_NODE_NUM				(1000000)

/* level single_watch 相关 */
#define SW_LVL0_SPEC					(16)
#define SW_LVL1_SPEC					(32)
#define SW_LVL2_SPEC					(128)
#define SW_LVL3_SPEC					(512)
#define SW_LVL_MAX_SPEC					(SW_LVL3_SPEC)

#if  (SW_LVL_MAX_SPEC < MAX_CONFIG_SW_CMD)
#error "MAX_CONFIG_SW_CMD is too big (over SW_LVL_MAX_SPEC)"
#endif

/* 注意: 该值是在删除cmd_node时的缓存大小, 必须保证这个值不小于最大的 LVL_SPEC */
#define CMD_LIST_BUF_SIZE				(SW_LVL_MAX_SPEC)

#define SW_LVL0_NNODE					(500000) /* 50w * 16 = 8000000 */
#define SW_LVL1_NNODE					(100000) /* 10w * 32 = 3200000 */
#define SW_LVL2_NNODE					(20000) /* 2w * 128 = 2560000 */
#define SW_LVL3_NNODE					(10000) /* 1w * 512 = 5120000 */

#define SW_MAX_LVL						(4)
#define TOTAL_SW_LVLn_NNODE				(SW_LVL0_NNODE \
										+ SW_LVL1_NNODE \
										+ SW_LVL2_NNODE \
										+ SW_LVL3_NNODE)

#if (TOTAL_SW_LVLn_NNODE > MAX_UID_NODE_NUM)
#error "Too many SW_LVLn_NNODE (sum > MAX_UID_NODE_NUM)"
#endif


/* 注意: 这个设定必须大于等于所有 svc_ac_api 的接口中的 max_batch_num */
#define MAX_AC_ONE_DATA_COUNT			(1000)


/* 如果ac_config不可用, 则按照最大的cmd不发包存在时间来更新cmd的 start_tv;
 * 同时, 在配置时, 无论是 tw 还是 sw, 它的 update_start_interval 都不允许超过这个值 */
#define MAX_UPDATE_START_INTERVAL	(SW_CMD_KEEPALIVE_TIME)

#define HTAB_LOOKUP_POSINFO(_htab, _uid, _row, _col) \
	hash_table_lookup_node_posinfo(_htab, (void*)&_uid, _uid, &_row, &_col)

#define HTAB_LOOKUP(_htab, _uid) \
	hash_table_lookup_node(_htab, (void*)&_uid, _uid)

#define HTAB_LOOKUP_EX_POSINFO(_htab, _uid, _exist, _row, _col) \
	hash_table_lookup_node_ex_posinfo(_htab, (void*)&empty_key, (void*)(&_uid), _uid, &_exist, &_row, &_col)

#define HTAB_LOOKUP_EX(_htab, _uid, _exist) \
	hash_table_lookup_node_ex(_htab, (void*)&empty_key, (void*)(&_uid), _uid, &_exist)

#define GET_SW_NODE(_lvl, _idx) \
	((struct single_watch_t*)(sw_lvl_start[_lvl] + _idx * sw_lvl_node_size_list[_lvl]))


struct cmd_t {
	/* 命令号 */
	uint32_t		cmd;
	/* 该cmd收取的总命令数 */
	uint32_t		count;
	/* 上次发cmd过来的时间 */
	struct timeval	last_tv;
	/*! 此番监视的开始时间 */
	struct timeval	start_tv;
};

/*! 本次在线的统计 */
struct total_watch_t {
	/* 收取的总命令数 */
	uint32_t		count;
	/* 上次发包过来的时间 */
	struct timeval	last_tv;
	/*! 此番监视的开始时间 */
	struct timeval	start_tv;
};

struct single_watch_t {
	/*! 
	 * 注意:
	 * (1) single_watch 的节点一旦被 uid_node 占据,
	 *	就必须当 uid_node 被释放时, 才被连带的释放
	 *	因此, 这里直接用 uid 表示状态: 0: 没被占据, 非0: 被uid占据;
	 * (2) 鉴于基于firrt-zero算法的empty-sw节点的获取方式,
	 * 不可能出现: uid_node->uid != sw->uid 的情况,
	 *   但有可能由于释放 uid_node 时, 还没来得及清除相关的 sw 而产生孤儿sw;
	 * 这种情况可用 旁路进程 或 本进程 不断扫描各级sw, 并且用 sw 中不为 0 的 uid
	 * 到 uid_node_htab 反向查找, 如果找不到, 则说明产生了孤儿 sw,
	 * 此时, 可以安全的把该 sw 清空;
	 */
	/*! 米米号 */
	uint32_t		uid;
	/* 命令列表 (sizeof(struct cmd_t) * 16 = 320) */
	struct cmd_t	cmd_list[];
};

struct single_watch_lvl0_t {
	/*! 米米号 (see to single_watch_t) */
	uint32_t		uid;
	/* 命令列表 (sizeof(struct cmd_t) * 16 = 320) */
	struct cmd_t	cmd_list[SW_LVL0_SPEC];
};

struct single_watch_lvl1_t {
	/* 米米号 */
	uint32_t		uid;
	/* 命令列表 (sizeof(struct cmd_t) * 32 = 640) */
	struct cmd_t	cmd_list[SW_LVL1_SPEC];
};

struct single_watch_lvl2_t {
	/* 米米号 */
	uint32_t		uid;
	/* 命令列表 (sizeof(struct cmd_t) * 128 = 2560) */
	struct cmd_t	cmd_list[SW_LVL2_SPEC];
};

struct single_watch_lvl3_t {
	/* 米米号 */
	uint32_t		uid;
	/* 命令列表 (sizeof(struct cmd_t) * 512 = 10240) */
	struct cmd_t	cmd_list[SW_LVL3_SPEC];
};

/**
 * @briel anticheat的node结构, 一个node管理一个uid的信息, 该节点被hash_table管理;
 * 注意: 1KB 一个node, 意味着, 2GB内存最多承担 200w 个在线结构;
 */
struct uid_node_t {
	/*! 上一次访问的时间, 单位: 秒 */
	int32_t						last_access;
	/*! 米米号 */
	uint32_t					uid;
	/*! 不区分命令号的监视 */
	struct total_watch_t		tw;
	/*! 区分命令号的监视: 不同命令号的数量 */
	uint32_t					sw_cmd_count;
	/*! 区分命令号的监视: 命令号信息缓存等级 */
	uint32_t					sw_level;
	/*! 区分命令号的监视: 命令号信息在相应缓存等级中的buffer索引 */
	uint32_t					sw_index;
	/* 客户端ip (网络序: 没有就是0) */
	uint32_t					ip;
	/* 客户端port (网络序: 没有就是0) */
	uint16_t					port;
	/* reserved (let each uid_node to 1KB) */
	uint8_t						reserved[954];
};
COMPILE_ASSERT(sizeof(struct uid_node_t) == UID_NODE_SIZE);


struct ac_local_config_t {
    /** 是否是后台进程 */
    cfg_bool_t  background;
    /** 是否是 debug 模式(debug不关闭0,1,2) */
    cfg_bool_t  debug_mode;

    /** 负责的业务 */
    char        svc_name[MAX_SVC_NAME_LEN];
    /** 业务id */
	uint32_t	svc_gameid;
	/* 违规信息发送给的db_agent的ip */
	char		db_agent_ip[INET_ADDRSTRLEN];
	/* 违规信息发送给的db_agent的端口 */
	uint16_t	db_agent_port;
	/* 违规信息发送给的db_agent的地址, 由 agent_ip 和 agent_port 计算得出 */
	struct sockaddr_in	db_agent_addr;


	/* for log */
	char		log_dir[4096];    
	int			max_log_lvl; 
	int			max_log_size;
	int			max_log_file;
	char		log_prefix[128]; 
};
extern struct ac_local_config_t ac_local_config;


/* 统计量 (TODO: 上报) */
struct ac_statistic_t {
	/* 总共收到的业务包量 (业务机器发给ac_svr的包量) */
	uint64_t					tot_recved;
	/* 总共收到的客户端包量 (也可以作为人次数量) */
	uint64_t					tot_cs_recved;
	/* 总共被判定成违规的数量 (也可以作为人次数量) */
	uint64_t					tot_ill;
	/* 总共接触到的新上线uid数量 */
	uint32_t					tot_coming_uid;
	/* 不区分cmd的情况下, 被判定成违规的数量 */
	uint64_t					tw_ill_cnt;
	/* 区分cmd的情况下, 被判定成违规的数量 */
	uint64_t					sw_ill_cnt;
};


#endif /* __INTERFACE_H__ */
