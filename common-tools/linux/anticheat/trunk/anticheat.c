#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <time.h>

#include <event2/event.h>

#include "common.h"
#include "struct.h"
#include "shm_keys.h"
#include "shm.h"
#include "channel.h"
#include "util.h"
#include "hash_table.h"
#include "daemon.h"
#include "log.h"
#include "svc_hash_table.h"

#include "anticheat.h"


struct ac_local_config_t ac_local_config, *ac_local_cfg = &ac_local_config;

time_t sys_clock;
int udp_sock_fd = -1;
uint32_t empty_key = 0;
struct ac_statistic_t ac_statistic, *stat = &ac_statistic;

struct timeval sys_clock_tv;

struct shm_mgr_t svc_channel_shm_mgr, *svc_chl_mgr = &svc_channel_shm_mgr;
struct shm_mgr_t uid_node_hash_table_shm_mgr, *uid_nodes_mgr = &uid_node_hash_table_shm_mgr;
struct shm_mgr_t ac_config_shm_mgr, *ac_cfg_mgr = &ac_config_shm_mgr;
struct anticheat_config_t *ac_cfg;

struct shm_mgr_t single_watch_lvl0, *sw_lvl0_mgr = &single_watch_lvl0;
struct shm_mgr_t single_watch_lvl1, *sw_lvl1_mgr = &single_watch_lvl1;
struct shm_mgr_t single_watch_lvl2, *sw_lvl2_mgr = &single_watch_lvl2;
struct shm_mgr_t single_watch_lvl3, *sw_lvl3_mgr = &single_watch_lvl3;

uint32_t sw_lvl_nnode_list[SW_MAX_LVL] = {
	SW_LVL0_NNODE, SW_LVL1_NNODE, SW_LVL2_NNODE, SW_LVL3_NNODE
};
uint32_t sw_lvl_size_list[] = {
	SW_LVL0_NNODE * sizeof(struct single_watch_lvl0_t),
	SW_LVL1_NNODE * sizeof(struct single_watch_lvl1_t),
	SW_LVL2_NNODE * sizeof(struct single_watch_lvl2_t),
	SW_LVL3_NNODE * sizeof(struct single_watch_lvl3_t)
};
uint32_t sw_lvl_node_size_list[] = {
	sizeof(struct single_watch_lvl0_t),
	sizeof(struct single_watch_lvl1_t),
	sizeof(struct single_watch_lvl2_t),
	sizeof(struct single_watch_lvl3_t)
};
uint32_t sw_lvl_spec_list[SW_MAX_LVL] = {
	SW_LVL0_SPEC, SW_LVL1_SPEC, SW_LVL2_SPEC, SW_LVL3_SPEC
};
void *sw_lvl_start[SW_MAX_LVL] = { 0 };
struct shm_mgr_t *sw_lvl_mgr_list[SW_MAX_LVL] = { 0 };

struct hash_table_t uid_nodes_hash_table, *uid_nodes_htab = &uid_nodes_hash_table;

char recvbuf[RCV_BUF_LEN];
char sendbuf[SND_BUF_LEN];


/* libevent's guys start */

/* priority: 0 */
struct event *tmrev_renew_sys_clock;

/* priority: 1 */
struct event *tmrev_worker;

/* priority: 3 */
struct event *tmrev_clean_uid_node;
struct event *tmrev_update_start;
struct event *tmrev_statistics;

/* signal */
struct event *quit_signal_event;

/* libevent's guys end */



struct sw_config_t *find_sw_config(struct anticheat_config_t *ac_config, uint32_t cmd)
{
	int left = 0;
	int right = ac_config->sw_config_count - 1;
	int mid = 0;
	struct sw_config_t *sw_cfg;

	if (ac_config->status != AC_CFG_STATUS_ENABLE) { return NULL; }

	while (left <= right) {
		mid = (left + right) / 2;
		sw_cfg = &(ac_config->sw_config[mid]);
		if (sw_cfg->cmd == cmd) {
			return sw_cfg;
		} else if (cmd < sw_cfg->cmd) {
			right = mid - 1;
		} else { /* cmd > sw_cfg->cmd */
			left = mid + 1;
		}
	}
	return NULL;
}


/* return: 0: node 的 key 与 key 相同; 1: node 的 key 与 key 不相同 */
static int uid_node_compare(const void *key, const void *node)
{
	uint32_t _key = *((uint32_t *)key);
	struct uid_node_t *_node = (struct uid_node_t *)node;
	return ((_key == _node->uid) ? 0 : 1);
}

/**
 * @brief 初始化 与 interface 之间的 channel
 * @return -1: failed, 0: success
 */
int init_anticheat_channels(void)
{
	/*
	 * (1) 用 svcid + 0x10000000 做key 避免重复;
	 * (2) key分区间, 统一规划, 分类管理(config, dbaddr, ...)
	 */
	key_t key = SHM_KEY_ANTICHEAT_CHANNEL + ac_local_cfg->svc_gameid * 1000;
	init_shm_mgr(svc_chl_mgr, key);
	if (get_channel_shm(svc_chl_mgr, create_noexist_yes) == -1) {
		BOOT_LOG(-1, "Failed to get_channel_shm");
	}

	return 0;
}

int init_uid_nodes_htab(void)
{
	uint32_t row_num;
	uint32_t *nodes_num;
	uint32_t *mods;

	void *table = NULL;
	size_t node_size = sizeof(struct uid_node_t);
	uint32_t svc_gameid = ac_local_cfg->svc_gameid;
	//size_t size = calc_hash_table_size(node_size, row_num, nodes_num);
	size_t size = get_svc_hash_table_size(svc_gameid, node_size, &row_num, &nodes_num, &mods);
	struct hash_table_t *htab = uid_nodes_htab;
	key_t key = SHM_KEY_UID_NODE + ac_local_cfg->svc_gameid * 1000;

	init_shm_mgr(uid_nodes_mgr, key);
	get_shm_create_noinitexist(key, size, NULL, 0);
	if (update_shm_attach(uid_nodes_mgr) == -1) {
		BOOT_LOG(-1, "Failed to update uid_nodes_mgr shm");
	}
	table = uid_nodes_mgr->shm;
	if (hash_table_init(htab, table, size, node_size, row_num, nodes_num, mods, uid_node_compare) == -1) {
		BOOT_LOG(-1, "Failed to hash_table_init");
	}
	if (htab->total_node_num > MAX_UID_NODE_NUM) {
		BOOT_LOG(-1, "Too many hash_table nodes (%u over %d)",
				htab->total_node_num, MAX_UID_NODE_NUM);
	}

	return 0;
}

int init_single_watch_shm(uint32_t gameid)
{
#define INIT_SW_WATCH(__lvl, __mgr, __key) \
	do { \
		size_t __size = sw_lvl_size_list[__lvl]; \
		init_shm_mgr(__mgr, __key); \
		get_shm_create_noinitexist(__key, __size, NULL, 0); \
		if (update_shm_attach(__mgr) == -1) { \
			BOOT_LOG(-1, "Failed to update uid_nodes_mgr, key=%#x", __key); \
		} \
		sw_lvl_mgr_list[__lvl] = __mgr; \
		sw_lvl_start[__lvl] = __mgr->shm; \
	} while (0)

	INIT_SW_WATCH(0, sw_lvl0_mgr, SHM_KEY_SINGLE_WATCH_LVL0 + gameid * 1000);
	INIT_SW_WATCH(1, sw_lvl1_mgr, SHM_KEY_SINGLE_WATCH_LVL1 + gameid * 1000);
	INIT_SW_WATCH(2, sw_lvl2_mgr, SHM_KEY_SINGLE_WATCH_LVL2 + gameid * 1000);
	INIT_SW_WATCH(3, sw_lvl3_mgr, SHM_KEY_SINGLE_WATCH_LVL3 + gameid * 1000);
#undef INIT_SW_WATCH

	return 0;
}

int init_anticheat_config_shm(uint32_t gameid)
{
	size_t size = sizeof(struct anticheat_config_t);
	key_t key = SHM_KEY_AC_CONFIG + gameid * 1000;
	init_shm_mgr(ac_cfg_mgr, key);
	if (get_shm_nocreate_noinitexist(key, size) == -1) {
		ERROR_LOG("Failed to get ac_cfg_mgr shm, key=%#x", key);
		return -1;
	}
	if (update_shm_attach(ac_cfg_mgr) == -1) {
		ERROR_LOG("Failed to update ac_cfg_mgr, key=%#x", key);
		return -1;
	}
	ac_cfg = (struct anticheat_config_t *)(ac_cfg_mgr->shm);
	if (strncmp(ac_cfg->svc_name, ac_local_cfg->svc_name, MAX_SVC_NAME_LEN)) {
		ERROR_LOG("svc mismatch: my-svc: %s, cfg->svc=%s",
				ac_local_cfg->svc_name, ac_cfg->svc_name);
		return -1;
	}

	return 0;
}

int create_udp_socket_nobind(void)
{
	int sock;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		return -1;
	}
	set_socket_nonblock(sock);
	set_socket_reuseaddr(sock);
	set_socket_rwmem(sock, SET_SOCK_RWMEM_SIZE);

	return sock;
}

int init_ac_local_config(char *conf_path, struct ac_local_config_t *config)
{
	int ret;

	cfg_opt_t opts[] = {
		CFG_BOOL("background", cfg_false, CFGF_NONE),
		CFG_BOOL("debug_mode", cfg_false, CFGF_NONE),

		CFG_STR("svc_name", "test", CFGF_NONE),
		CFG_INT("svc_gameid", 0, CFGF_NONE),
		CFG_STR("db_agent_ip", "0.0.0.0", CFGF_NONE),
		CFG_INT("db_agent_port", 0, CFGF_NONE),

		CFG_STR("log_dir", "./log", CFGF_NONE),
		CFG_INT("max_log_lvl", log_lvl_debug, CFGF_NONE),
		CFG_INT("max_log_size", MAX_LOG_SIZE, CFGF_NONE),
		CFG_INT("max_log_file", 0, CFGF_NONE),
		CFG_STR("log_prefix", "ac_", CFGF_NONE),

		CFG_END()
	};

	cfg_t *cfg = cfg_init(opts, CFGF_NONE);
	ret = cfg_parse(cfg, conf_path ? conf_path : DEF_CFG_PATH);
	if(ret == CFG_FILE_ERROR) {
		BOOT_LOG(-1, "Access error, conf: %s, err: %s",
				conf_path ? conf_path : DEF_CFG_PATH, strerror(errno));
	} else if(ret == CFG_PARSE_ERROR) {
		BOOT_LOG(-1, "Parse error, conf: %s", conf_path ? conf_path : DEF_CFG_PATH);
	}

	memset(config, 0, sizeof(*config));

	config->background = cfg_getbool(cfg, "background");
	config->debug_mode = cfg_getbool(cfg, "debug_mode");
	snprintf(config->svc_name, sizeof(config->svc_name), "%s", cfg_getstr(cfg, "svc_name"));
	config->svc_gameid = cfg_getint(cfg, "svc_gameid");
	snprintf(config->db_agent_ip, sizeof(config->db_agent_ip), "%s", cfg_getstr(cfg, "db_agent_ip"));
	config->db_agent_port = cfg_getint(cfg, "db_agent_port");

	snprintf(config->log_dir, sizeof(config->log_dir), "%s", cfg_getstr(cfg, "log_dir"));
	config->max_log_lvl = cfg_getint(cfg, "max_log_lvl");
	config->max_log_size = cfg_getint(cfg, "max_log_size");
	config->max_log_file = cfg_getint(cfg, "max_log_file");
	snprintf(config->log_prefix, sizeof(config->log_prefix), "%s", cfg_getstr(cfg, "log_prefix"));

	if (config->svc_gameid > MAX_SVC_GAMEID) {
		BOOT_LOG(-1, "Invalid svc_gameid: %u for svc: %s",
				config->svc_gameid, config->svc_name);
	}

	config->db_agent_addr.sin_family = AF_INET;
	inet_pton(AF_INET, config->db_agent_ip, &(config->db_agent_addr.sin_addr));
	config->db_agent_addr.sin_port = htons(config->db_agent_port);

	cfg_free(cfg);

	return 0;
}

int init_system(int argc, char **argv)
{
	int ret;
	char *conf_path = (argc == 2) ? argv[1] : NULL;
	ret = init_ac_local_config(conf_path, ac_local_cfg);
	if (ret == -1) {
		BOOT_LOG(-1, "Failed to init_ac_local_config");
	}

	ret = log_init(ac_local_cfg->log_dir, ac_local_cfg->max_log_lvl,
			ac_local_cfg->max_log_size, ac_local_cfg->max_log_file,
			ac_local_cfg->log_prefix);
	if (ret == -1) {
		BOOT_LOG(-1, "Failed to log_init");
	}

	if (init_anticheat_config_shm(ac_local_cfg->svc_gameid) == -1) {
		BOOT_LOG(-1, "Failed to init_anticheat_config_shm");
	}

	if (init_anticheat_channels() == -1) {
		BOOT_LOG(-1, "Failed to init_anticheat_channels");
	}

	if (init_uid_nodes_htab() == -1) {
		BOOT_LOG(-1, "Failed to init_uid_nodes_htab");
	}

	if (init_single_watch_shm(ac_local_cfg->svc_gameid) == -1) {
		BOOT_LOG(-1, "Failed to init_single_watch_shm");
	}

	set_rlimit(DEFAULT_MAX_OPEN_FILES);

	return 0;
}

/* return -1: fail, 0: success */
struct single_watch_t *get_sw_empty_node(uint32_t uid, uint32_t from_lvl, uint32_t to_lvl, uint32_t *lvl, uint32_t *idx)
{
	if (from_lvl >= SW_MAX_LVL || to_lvl >= SW_MAX_LVL) {
		return NULL;
	}

	int cur_lvl, cur_idx;
	struct single_watch_t *sw = NULL;
	static uint32_t lvl_free_cache[SW_MAX_LVL] = { 0 };

	cur_lvl = from_lvl;
	for (; cur_lvl <= to_lvl; cur_lvl++) {
		cur_idx = lvl_free_cache[cur_lvl];

		for (; cur_idx < sw_lvl_nnode_list[cur_lvl]; cur_idx++) {
			sw = GET_SW_NODE(cur_lvl, cur_idx);
			if (sw->uid == 0) {
				goto find_out;
			}
		}

		for (cur_idx = 0; cur_idx < lvl_free_cache[cur_lvl]; cur_idx++) {
			sw = GET_SW_NODE(cur_lvl, cur_idx);
			if (sw->uid == 0) {
				goto find_out;
			}
		}
	}

	return NULL;

find_out:
	*lvl = cur_lvl;
	*idx = cur_idx;
	lvl_free_cache[cur_lvl] = (cur_idx + 1) % sw_lvl_nnode_list[cur_lvl];
	memset(sw, 0, sw_lvl_node_size_list[cur_lvl]);
	sw->uid = uid;
	return sw;
}

int upgrade_sw_cmd_list_lvl(struct uid_node_t *uid_node, uint32_t lvl, uint32_t idx)
{
	uint32_t nlvl, nidx;
	struct single_watch_t *o_sw, *n_sw;

	if (lvl >= SW_MAX_LVL - 1) {
		/*
		 * TODO:
		 * 已经超过最高级别的命令号列表, 再也无法升级, 此时:
		 * (1) send warning (报警 single watch shm 级别不足);
		 * (2) 不能认为是非法, 因为可能长时间稳定的发各种包 (只好放弃判断);
		 */
		return -1;
	}

	n_sw = get_sw_empty_node(uid_node->uid, lvl+1, lvl+1, &nlvl, &nidx);
	if (n_sw == NULL) {
		return -1;
	}
	TRACE_LOG("get_sw_empty_node - 2: found, uid=%u, ori_lvl=%u, ori_idx=%u, "
			"new_lvl=%u, new_idx=%u", uid_node->uid, lvl, idx, nlvl, nidx);

	o_sw = GET_SW_NODE(lvl, idx);
	n_sw = GET_SW_NODE(nlvl, nidx);
	memcpy(n_sw, o_sw, sw_lvl_node_size_list[lvl]);

	uid_node->sw_level = nlvl;
	uid_node->sw_index = nidx;

	memset(o_sw, 0, sw_lvl_node_size_list[lvl]);

	return 0;
}


/* return 0: success; -1: fail */
int add_new_cmd_to_uid_node(struct uid_node_t *uid_node, struct client_info_t *cinfo, uint32_t cmd)
{
	/* single watch */
	uint32_t lvl = uid_node->sw_level;
	uint32_t idx = uid_node->sw_index;
	struct single_watch_t *sw = NULL;

	sw = GET_SW_NODE(lvl, idx);
	if (uid_node->uid != sw->uid) {
		sw = get_sw_empty_node(uid_node->uid, 0, 0, &lvl, &idx);
		if (sw == NULL) {
			/* TODO: send warning:
			 * 		没有更多的同级别的空sw_node可用了,
			 * 		由于不允许还不够级别时的跳级,
			 * 		因此在本级别中找不到空位就必须返回失败;
			 */
			/* 注意:
			 * 		升级的理由是因为 sw->cmd_list[lvl] 装不下更多的命令了;
			 * 		而不是因为在这一级别找不到空的 sw 节点;
			 * 		故此处返回失败
			 */
			CRIT_LOG("FATAL: Failed to get_sw_empty_node, uid=%u, cmd=%u, "
					"lvl=0, idx=0", uid_node->uid, cmd);
			return -1;
		}

		uid_node->sw_level = lvl;
		uid_node->sw_index = idx;
		uid_node->tw.start_tv = cinfo->recv_time;
		uid_node->tw.last_tv = cinfo->recv_time;

		TRACE_LOG("get_sw_empty_node - 1: found, uid=%u, cmd=%u, "
				"lvl=%u, idx=%u", uid_node->uid, cmd, lvl, idx);
	} else if (uid_node->sw_cmd_count >= sw_lvl_spec_list[uid_node->sw_level]) {
		/* 把分配的 sw_node 放满了, 之前升级又没有成功, 只好放弃判断 */
		/* TODO: send warning */
		CRIT_LOG("FATAL: Too many diff_cmd, uid=%u, cmd=%u, "
				"sw_cmd_count=%u, lvl=%u, idx=%u", uid_node->uid, cmd,
				uid_node->sw_cmd_count, uid_node->sw_level, uid_node->sw_index);
		return -1;
	} else {
		lvl = uid_node->sw_level;
		idx = uid_node->sw_index;
	}

	uint32_t sw_cmd_count = uid_node->sw_cmd_count;
	sw->cmd_list[sw_cmd_count].cmd = cmd;
	sw->cmd_list[sw_cmd_count].count++;
	sw->cmd_list[sw_cmd_count].start_tv = cinfo->recv_time;
	sw->cmd_list[sw_cmd_count].last_tv = cinfo->recv_time;

	uid_node->sw_cmd_count++;
	if (uid_node->sw_cmd_count >= sw_lvl_spec_list[lvl]) {
		if (upgrade_sw_cmd_list_lvl(uid_node, lvl, idx) == -1) {
			/* 升级失败 */
			/* TODO: send waring (too bad) */
			CRIT_LOG("FATAL: Failed to upgrade_sw_cmd_list_lvl, uid=%u, cmd=%u, "
					"olvl=%u, oidx=%u", uid_node->uid, cmd, lvl, idx);
			return -1;
		}
	}

	return 0;
}

void init_uid_node_base(struct svr_pkg_t *svr_pkg, struct client_info_t *cinfo, struct cli_proto_t *cpkg, struct uid_node_t *uid_node)
{
	memset(uid_node, 0, sizeof(*uid_node));

	uid_node->last_access = sys_clock; /* 一定要用本机时间, 防止 recv_time 出错导致永远无法释放 */
	uid_node->uid = cpkg->uid;
	memcpy(&(uid_node->ip), &(cinfo->addr.sin_addr), sizeof(uid_node->ip));
	uid_node->port = cinfo->addr.sin_port;
}

struct cmd_t *find_cmd_list_node_by_single_watch(struct single_watch_t *sw, uint32_t sw_cmd_count, uint32_t cmd)
{
	int i = 0;
	for (; i < sw_cmd_count; i++) {
		if (cmd == sw->cmd_list[i].cmd) {
			return &(sw->cmd_list[i]);
		}
	}

	return NULL;
}

struct cmd_t *find_cmd_list_node_by_uid_node(struct uid_node_t *uid_node, uint32_t cmd)
{
	uint32_t lvl = uid_node->sw_level;
	uint32_t idx = uid_node->sw_index;
	uint32_t sw_cmd_count = uid_node->sw_cmd_count;
	struct single_watch_t *sw = GET_SW_NODE(lvl, idx);
	return find_cmd_list_node_by_single_watch(sw, sw_cmd_count, cmd);
}

void total_cmd_check_interval(uint32_t *ill_code, int64_t diff, struct client_info_t *cinfo, struct uid_node_t *uid_node)
{
	if (diff < ac_cfg->min_tw_interval) {
		*ill_code |= ILL_CODE_TW_INT;

		TRACE_LOG("1 total_cmd(%u), ill-interval, uid=%u, last_tv[%ld, %ld], "
				"cmd_tv[%ld, %ld], interval=%ld", uid_node->tw.count, uid_node->uid,
				uid_node->tw.last_tv.tv_sec, uid_node->tw.last_tv.tv_usec,
				cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec, diff);
	}
}

double total_cmd_check_frequence(uint32_t *ill_code, int64_t diff, struct client_info_t *cinfo, struct uid_node_t *uid_node)
{
	double freq = 0.0;
	struct total_watch_t *tw = &(uid_node->tw);

	if (diff <= 0) {
		*ill_code |= ILL_CODE_TW_FREQ;

		TRACE_LOG("2 total_cmd, ill-freq, uid=%u, cmd_tw_count=%u, diff_to_st=%ld, "
				"freq=INF, last_tv[%ld, %ld], cmd_tv[%ld, %ld]",
				uid_node->uid, uid_node->tw.count, diff,
				tw->last_tv.tv_sec, tw->last_tv.tv_usec,
				cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec);
	} else {
		freq = (tw->count * 1.0 / diff) * 1000000;
		/* 注意: == 对于浮点数不可使用, 这里无论如何也不能用 >= */
		if (freq > ac_cfg->max_tw_freq) {
			*ill_code |= ILL_CODE_TW_FREQ;

			TRACE_LOG("3 total_cmd, ill-freq, uid=%u, cmd_tw_count=%u, "
					"diff_to_st=%ld, freq=%lu, last_tv[%ld, %ld], cmd_tv[%ld, %ld]",
					uid_node->uid, uid_node->tw.count, diff, freq,
					tw->last_tv.tv_sec, tw->last_tv.tv_usec,
					cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec);
		}
	}

	return freq;
}

void single_cmd_check_interval(uint32_t *ill_code, int64_t diff, struct client_info_t *cinfo, struct uid_node_t *uid_node, struct cmd_t *cmd_node, struct sw_config_t *sw_cfg)
{
	if (diff < sw_cfg->min_sw_interval) {
		*ill_code |= ILL_CODE_SW_INT;

		TRACE_LOG("4 single_cmd(%u), ill-interval, uid=%u, cmd=%u, "
				"last_tv[%ld, %ld], cmd_tv[%ld, %ld], diff=%ld",
				cmd_node->count, uid_node->uid, cmd_node->cmd,
				cmd_node->last_tv.tv_sec, cmd_node->last_tv.tv_usec,
				cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec, diff);
	}
}

double single_cmd_check_frequence(uint32_t *ill_code, int64_t diff, struct client_info_t *cinfo, struct uid_node_t *uid_node, struct cmd_t *cmd_node, struct sw_config_t *sw_cfg)
{
	double freq = 0;

	if (diff <= 0) {
		*ill_code |= ILL_CODE_SW_FREQ;

		TRACE_LOG("5 single_cmd, ill-freq, uid=%u, cmd=%u, cmd_sw_count=%u, "
				"diff_to_st=%ld, freq=INF, last_tv[%ld, %ld], cmd_tv[%ld, %ld]",
				uid_node->uid, cmd_node->cmd, cmd_node->count, diff,
				cmd_node->last_tv.tv_sec, cmd_node->last_tv.tv_usec,
				cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec);
	} else {
		freq = (cmd_node->count * 1.0 / diff) * 1000000;
		/* 注意: == 对于浮点数不可使用, 这里无论如何也不能用 >= */
		if (freq > sw_cfg->max_sw_freq) {
			*ill_code |= ILL_CODE_SW_FREQ;

			TRACE_LOG("6 single_cmd, ill-freq, uid=%u, cmd=%u, sw_cmd_count=%u, "
					"diff_to_st=%ld, freq=%f, last_tv[%ld, %ld], cmd_tv[%ld, %ld]",
					uid_node->uid, cmd_node->cmd, cmd_node->count, diff, freq,
					cmd_node->last_tv.tv_sec, cmd_node->last_tv.tv_usec,
					cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec);
		}
	}

	return freq;
}

int proc_one_client_request(struct ac_chl_pkg_t *chl_pkg, struct svr_pkg_t *svr_pkg, struct ac_one_cdata_t *one_cdata)
{
	struct client_info_t *cinfo = (struct client_info_t *)(&one_cdata->cinfo);
	struct cli_proto_t *cpkg = (struct cli_proto_t *)(&one_cdata->chead);

	//cpkg->len = ntohl(cpkg->len); /* 注意: cpkg->len 是原始 cs包的长度 */
	//cpkg->seqno = ntohl(cpkg->seqno);
	cpkg->cmd = ntohl(cpkg->cmd);
	cpkg->uid = ntohl(cpkg->uid);
	if (cpkg->uid == 0) return 0;

	int exist = 0;
	struct uid_node_t *uid_node = HTAB_LOOKUP_EX(uid_nodes_htab, cpkg->uid, exist);
#if 0 /* JUST FOR DEBUG */
	uint32_t chs_row = 0, chs_col = 0;
	struct uid_node_t *uid_node = HTAB_LOOKUP_EX_POSINFO(uid_nodes_htab, cpkg->uid, exist, chs_row, chs_col);
	DEBUG_LOG("HTAB_LOOKUP_EX_POSINFO: uid: %u, exist: %d, chs_row: %u, chs_col: %u",
			cpkg->uid, exist, chs_row, chs_col);
#endif

	if (!uid_node) { /* hash_table full */
		/* TODO: fatal error, send warning */
		ERROR_LOG("Cannot find or getnew uid_node for uid: %u", cpkg->uid);
		return -1;
	}

	if (!exist) { /* new coming uid */
		stat->tot_coming_uid++; /* tot_ill_uid 不好统计, 否则要记录ill_uid_list */

		init_uid_node_base(svr_pkg, cinfo, cpkg, uid_node);
		if (add_new_cmd_to_uid_node(uid_node, cinfo, cpkg->cmd) == 0) {
			uid_node->tw.count++;
		}
		return 0;
	}
	uid_node->last_access = sys_clock;
	uid_node->tw.count++;

	/* 确认配置可用, 否则放弃本次检查 */
	static int log_ac_cfg_count = 0;
	if (!ac_cfg || ac_cfg->status != AC_CFG_STATUS_ENABLE) {
		/* TODO: send waring, 由于配置信息不可用, 本次无法检测 */
		if (log_ac_cfg_count++ < 100) {
			ERROR_LOG("ac_config is not avaliable!");
		}
		return -1;
	}
	struct sw_config_t *sw_cfg = find_sw_config(ac_cfg, cpkg->cmd);

	/* exist uid */
	uint32_t tw_ill_code = 0, sw_ill_code = 0;
	struct timeval sub_tv = { 0 };
	int64_t tw_diff_to_start = 0, tw_diff_to_last = 0;
	int64_t sw_diff_to_start = 0, sw_diff_to_last = 0;
	double tw_freq = 0.0, sw_freq = 0.0;
	struct cmd_t *cmd_node = NULL;
	if (sw_cfg) { /* 找到了 sw_config, 是需要监控的 cmd */
		cmd_node = find_cmd_list_node_by_uid_node(uid_node, cpkg->cmd);
		if (!cmd_node) { /* new cmd */
			if (add_new_cmd_to_uid_node(uid_node, cinfo, cpkg->cmd) == -1) {
				return -1;
			}
		} else {
			cmd_node->count++;
		}
	}

	tw_ill_code = sw_ill_code = 0;

	timersub(&(cinfo->recv_time), &(uid_node->tw.last_tv), &sub_tv);
	tw_diff_to_last = sub_tv.tv_sec * 1000000 + sub_tv.tv_usec;
	timersub(&(cinfo->recv_time), &(uid_node->tw.start_tv), &sub_tv);
	tw_diff_to_start = sub_tv.tv_sec * 1000000 + sub_tv.tv_usec;

#if 0
	DEBUG_LOG("tw_diff_to_last = %ld, recv_time: [%ld, %ld], last: [%ld, %ld]",
			tw_diff_to_last, cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec,
			uid_node->tw.last_tv.tv_sec, uid_node->tw.last_tv.tv_usec);
	DEBUG_LOG("tw_diff_to_start = %ld, recv_time: [%ld, %ld], start: [%ld, %ld]",
			tw_diff_to_start, cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec,
			uid_node->tw.start_tv.tv_sec, uid_node->tw.start_tv.tv_usec);
#endif

	/* 1. 不分命令号的检查: 非法间隔 */
	if (uid_node->tw.count > 1) {
		total_cmd_check_interval(&tw_ill_code, tw_diff_to_last, cinfo, uid_node);
	}

	/* 2. 不分命令号的检查: 非法频率 */
	if (uid_node->tw.count > CHECK_TW_FREQ_MIN_COUNT) {
		tw_freq = total_cmd_check_frequence(&tw_ill_code, tw_diff_to_start, cinfo, uid_node);
	}

	uid_node->tw.last_tv = cinfo->recv_time;

#if 0
	DEBUG_LOG("renewed uid_node->tw.last_tv: [%ld, %ld]",
			uid_node->tw.last_tv.tv_sec, uid_node->tw.last_tv.tv_usec);
#endif

	if (cmd_node) {
		timersub(&(cinfo->recv_time), &(cmd_node->last_tv), &sub_tv);
		sw_diff_to_last = sub_tv.tv_sec * 1000000 + sub_tv.tv_usec;
		timersub(&(cinfo->recv_time), &(cmd_node->start_tv), &sub_tv);
		sw_diff_to_start = sub_tv.tv_sec * 1000000 + sub_tv.tv_usec;

#if 0
	DEBUG_LOG("sw_diff_to_last = %ld, recv_time: [%ld, %ld], last: [%ld, %ld]",
			sw_diff_to_last, cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec,
			cmd_node->last_tv.tv_sec, cmd_node->last_tv.tv_usec);
	DEBUG_LOG("tw_diff_to_start = %ld, recv_time: [%ld, %ld], start: [%ld, %ld]",
			tw_diff_to_start, cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec,
			cmd_node->start_tv.tv_sec, cmd_node->start_tv.tv_usec);
#endif

		/* 3. 区分命令号的检查: 非法间隔 */
		if (cmd_node->count > 1) {
			single_cmd_check_interval(&sw_ill_code, sw_diff_to_last, cinfo, uid_node, cmd_node, sw_cfg);
		}

		/* 4. 区分命令号的检查: 非法频率 */
		if (cmd_node->count > CHECK_SW_FREQ_MIN_COUNT) {
			sw_freq = single_cmd_check_frequence(&sw_ill_code, sw_diff_to_start, cinfo, uid_node, cmd_node, sw_cfg);
		}

		cmd_node->last_tv = cinfo->recv_time;
	}

	/* SEND ILL */
	//DEBUG_LOG("tw_ill_code: %u, sw_ill_code: %u", tw_ill_code, sw_ill_code);
	if (tw_ill_code || sw_ill_code) {
		stat->tot_ill++;
		if (tw_ill_code) stat->tw_ill_cnt++;

		struct svr_pkg_t *svr_pkg = (struct svr_pkg_t *)sendbuf;
		struct ill_report_t *report = (struct ill_report_t *)(svr_pkg->body);
		struct ill_report_rcd_t *record = NULL;
		int bodylen = 0, pkglen = 0;
		const struct sockaddr *dbaddr = (const struct sockaddr *)&(ac_local_cfg->db_agent_addr);
		socklen_t dbaddrlen = sizeof(*dbaddr);

		report->count = 1;
		record = (struct ill_report_rcd_t *)&(report->record);
		snprintf(record->svc_name, MAX_SVC_NAME_LEN, "%s", ac_local_cfg->svc_name);
		record->tw_ill_code = tw_ill_code;
		record->sw_ill_code = sw_ill_code;
		record->uid = cpkg->uid;
		record->cmd = cpkg->cmd;
		record->cmd_recv_sec = cinfo->recv_time.tv_sec;
		record->ip = cinfo->addr.sin_addr.s_addr;
		record->port = cinfo->addr.sin_port;
		record->reporter_ip = chl_pkg->head.recv_from.sin_addr.s_addr;
		record->reporter_port = chl_pkg->head.recv_from.sin_port;
		record->tw_last_int = tw_diff_to_last;
		record->tw_int_limit = ac_cfg->min_tw_interval;
		record->tw_start_int = tw_diff_to_start;
		record->tw_count = uid_node->tw.count;
		record->tw_freq = tw_freq;
		record->tw_freq_limit = ac_cfg->max_tw_freq;
		if (sw_ill_code) {
			record->sw_last_int = sw_diff_to_last;
			record->sw_int_limit = sw_cfg->min_sw_interval;
			record->sw_start_int = sw_diff_to_start;
			record->sw_count = cmd_node->count;
			record->sw_freq = sw_freq;
			record->sw_freq_limit = sw_cfg->max_sw_freq;
		}
		bodylen = sizeof(struct ill_report_t) + report->count * sizeof(struct ill_report_rcd_t);
		pkglen = sizeof(struct svr_pkg_t) + bodylen;

		svr_pkg->ver = htonl(0);
		svr_pkg->cmd = htonl(1001);
		svr_pkg->uid = htonl(cpkg->uid);
		svr_pkg->seqno = htonl(0);
		svr_pkg->len = htonl(pkglen);

		if (sendto(udp_sock_fd, (const void *)svr_pkg, pkglen, 0, dbaddr, dbaddrlen) == -1) {
			ERROR_LOG("Failed to sendto: s=%d, err(%d): %s",
					udp_sock_fd, errno, strerror(errno));
		}
	}

	if (tw_ill_code & ILL_CODE_TW_FREQ) {
		/* tw 有任何违规, 就更新其 start_tv */
		uid_node->tw.count = 0;
		uid_node->tw.start_tv = cinfo->recv_time;
	}

	if (sw_ill_code & ILL_CODE_SW_FREQ) {
		/* sw 有任何违规, 就更新其 start_tv */
		cmd_node->count = 0;
		cmd_node->start_tv = cinfo->recv_time;
	}

	if (tw_diff_to_last / 1000000 > ac_cfg->update_tw_start_interval) {
		TRACE_LOG("UPDATE_TW_START-2, uid=%u, from:[%ld, %ld], to:[%ld, %ld]",
				uid_node->uid,
				uid_node->tw.start_tv.tv_sec, uid_node->tw.start_tv.tv_usec,
				cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec);

		uid_node->tw.count = 0;
		/* 之前肯定会更新 uid_node->tw.last_tv, 这里就不重复了 */
		uid_node->tw.start_tv = cinfo->recv_time;
	}

	if (cmd_node && (sw_diff_to_last / 1000000 > sw_cfg->update_sw_start_interval)) {
		TRACE_LOG("UPDATE_SW_START-2, uid=%u, cmd=%u, from:[%ld, %ld], to:[%ld, %ld]",
				uid_node->uid, cmd_node->cmd,
				cmd_node->start_tv.tv_sec, cmd_node->start_tv.tv_usec,
				cinfo->recv_time.tv_sec, cinfo->recv_time.tv_usec);

		cmd_node->count = 0;
		/* 之前肯定会更新 cmd_node->last_tv, 这里就不重复了 */
		cmd_node->start_tv = cinfo->recv_time;
	}

	return 0;
}

int proc_client_request(struct ac_chl_pkg_t *chl_pkg, struct svr_pkg_t *svr_pkg)
{
	uint32_t pad = sizeof(struct svr_pkg_t) + sizeof(struct ac_body_t);
	if (svr_pkg->len < pad) {
		ERROR_LOG("invalid svr_pkg, too short pkg, pad: %u, svr_pkg->len: %u",
				pad, svr_pkg->len);
		return -1;
	}
	struct ac_body_t *ac_body = (struct ac_body_t *)(svr_pkg->body);
	ac_body->count = ntohl(ac_body->count);
	if (ac_body->count == 0 || ac_body->count > MAX_AC_ONE_DATA_COUNT) {
		ERROR_LOG("invalid svr_pkg, ac_body->count: %u (not in [%u, %u])",
				ac_body->count, 1u, MAX_AC_ONE_DATA_COUNT);
		return -1;
	}
	uint32_t explen = pad + ac_body->count * sizeof(struct ac_one_cdata_t);
	if (explen != svr_pkg->len) {
		ERROR_LOG("invalid svr_pkg, explen(%u) mismatch with svr_pkg->len: %u",
				explen, svr_pkg->len);
		return -1;
	}

	uint32_t i = 0;
	struct ac_one_cdata_t *one_cdata;
	for (; i < ac_body->count; i++) {
		one_cdata = (struct ac_one_cdata_t *)(&ac_body->cdata[i]);
		proc_one_client_request(chl_pkg, svr_pkg, one_cdata);
		stat->tot_cs_recved++;
	}

	return 0;
}

/**
 * @empty 0: channel在本次已经被处理完成了, 1: channel 里还有没处理的数据
 * @return 0: success, -1: fail
 */
int recv_interface(int *empty)
{
	int count = 0;
	int max_count = 100; /* TODO: config? */

	struct channel_t *chl = (struct channel_t *)(svc_chl_mgr->shm);
	struct ac_chl_pkg_t *chl_pkg = (struct ac_chl_pkg_t *)recvbuf;
	uint32_t rcvlen;

	for (; count < max_count; count++) {
		rcvlen = sizeof(recvbuf);
		if (get_from_channel(chl, &rcvlen, recvbuf, REQUEST_CHANNEL) == -1) {
			ERROR_LOG("Failed to get_from_channel");
			return -1;
		}

		if (rcvlen == 0) {
			*empty = 0;
			break;
		}

		if (rcvlen < sizeof(struct ac_chl_pkg_t)
			|| rcvlen != chl_pkg->head.len) {
			ERROR_LOG("invliad chl_pkg rcvlen: %u, chl_pkg->head.len: %u, "
					"sizeof(ac_chl_pkg_t): %zd",
					rcvlen, chl_pkg->head.len, sizeof(struct ac_chl_pkg_t));
			return -1;
		}

		uint32_t rcv_svr_pkg_len = rcvlen - sizeof(struct ac_chl_head_t);
		struct svr_pkg_t *svr_pkg = &(chl_pkg->svr_pkg);
		svr_pkg->len = ntohl(svr_pkg->len);
		svr_pkg->cmd = ntohl(svr_pkg->cmd);
		if (svr_pkg->len != rcv_svr_pkg_len) {
			ERROR_LOG("invalid svr_pkg: svr_pkg->len(%u) != rcv_svr_pkg_len(%u)",
					svr_pkg->len, rcv_svr_pkg_len);
			return -1;
		}

		stat->tot_recved++;
		proc_client_request(chl_pkg, svr_pkg);
	}

	return 0;
}

void tmr_renew_sys_clock(evutil_socket_t pad, short events, void *user_data)
{
	gettimeofday(&sys_clock_tv, 0);
	sys_clock = sys_clock_tv.tv_sec;
}

void add_sys_clock_timer(struct event_base *base)
{
	/* timer event */
    struct timeval tmr_tv;
	tmr_tv.tv_sec = 0;
	tmr_tv.tv_usec = 500000;
	tmrev_renew_sys_clock = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, tmr_renew_sys_clock, NULL);
	/* 更新系统时钟: 最高优先级: 保证在循环的一开始执行 */
	event_priority_set(tmrev_renew_sys_clock, 0);
	event_add(tmrev_renew_sys_clock, &tmr_tv);
}

void tmr_do_statistics(evutil_socket_t pad, short events, void *user_data)
{
	/* TODO: 上报 */

	DEBUG_LOG("AC_STAT(per min): %lu %lu %lu %u", stat->tot_recved,
			stat->tot_cs_recved, stat->tot_ill, stat->tot_coming_uid);
	memset(stat, 0, sizeof(*stat));
}

int add_statistics_timer_event(struct event_base *base)
{
	struct timeval tmr_tv;

	tmr_tv.tv_sec = AC_STATISTICS_INTERVAL;
	tmr_tv.tv_usec = 0;
	tmrev_statistics = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, tmr_do_statistics, NULL);
	evtimer_add(tmrev_statistics, &tmr_tv);

	return 0;
}

void release_uid_node(struct uid_node_t *uid_node)
{
	uint32_t lvl = uid_node->sw_level;
	uint32_t idx = uid_node->sw_index;
	struct single_watch_t *sw = GET_SW_NODE(lvl, idx);
	memset(sw, 0, sw_lvl_node_size_list[lvl]);
	memset(uid_node, 0, sizeof(*uid_node));
}

void tmr_clean_uid_node(evutil_socket_t pad, short events, void *user_data)
{
	static uint32_t row = 0;
	static uint32_t col = 0;
	static uint32_t occupy_cur_row = 0;
	static uint32_t last_row = 0;

	int scan = 0;
	int handle = 0;
	struct uid_node_t *uid_node;
	for (scan = 0; scan < AC_CLEAN_UID_NODE_SCAN_LIMIT; scan++) {
		uid_node = (struct uid_node_t *)hash_table_walk(uid_nodes_htab, &row, &col);
		if (!uid_node) {
			row = col = 0;
			occupy_cur_row = 0;
			last_row = 0;
			continue;
		}

		if (last_row != row) { /* switch row */
			if (occupy_cur_row == 0) {
				DEBUG_LOG("tmr_clean_uid_node - scaned row: %u, occupied: %u",
						last_row, occupy_cur_row);
				/* 本次轮询这行没有一个被占据的uid_node,
				 * 此时认为已经轮转到了最后有数据的一行,
				 * 于是恢复到初始状态, 开始下一趟扫描 */
				row = col = 0;
				occupy_cur_row = 0;
				last_row = 0;
				break;
			}
			last_row = row;
			occupy_cur_row = 0;
		}

		if (uid_node->uid == empty_key) continue;

		occupy_cur_row++;

		if (sys_clock - uid_node->last_access > MAX_UID_KEEPALIVE_TIME) {
			release_uid_node(uid_node);

			if (++handle >= AC_CLEAN_UID_NODE_HDL_LIMIT) {
				break;
			}
		}
	}
}

int add_clean_uid_node_timer_event(struct event_base *base)
{
	struct timeval tmr_tv;

	tmr_tv.tv_sec = AC_CLEAN_UID_NODE_INTERVAL;
	tmr_tv.tv_usec = 0;
	tmrev_clean_uid_node = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, tmr_clean_uid_node, NULL);
	evtimer_add(tmrev_clean_uid_node, &tmr_tv);
	return 0;
}

void tmr_update_start(evutil_socket_t pad, short events, void *user_data)
{
	static uint32_t row = 0;
	static uint32_t col = 0;
	static uint32_t occupy_cur_row = 0;
	static uint32_t last_row = 0;

	static struct cmd_t cmd_list_buf[CMD_LIST_BUF_SIZE];

	uint32_t count = 0;
	int clean = 0;
	uint32_t clean_idx = 0;

	int c;
	uint32_t lvl, idx;
	struct single_watch_t *sw = NULL;
	struct cmd_t *cmd_node = NULL, *cmd_node_last = NULL;

	int scan = 0;
	int handle = 0;
	struct uid_node_t *uid_node;
	for (scan = 0; scan < AC_UPDATE_START_SCAN_LIMIT; scan++) {
		/* 注意: 对 handle 的判断要放在最前面 */
		if (handle >= AC_UPDATE_START_HDL_LIMIT) {
			break;
		}

		uid_node = (struct uid_node_t *)hash_table_walk(uid_nodes_htab, &row, &col);
		if (!uid_node) {
			row = col = 0;
			occupy_cur_row = 0;
			last_row = 0;
			continue;
		}

		if (last_row != row) { /* switch row */
			if (occupy_cur_row == 0) {
				TRACE_LOG("tmr_update_start - scaned row: %u, occupied: %u",
						last_row, occupy_cur_row);
				/* 本次轮询这行没有一个被占据的uid_node,
				 * 此时认为已经轮转到了最后有数据的一行,
				 * 于是恢复到初始状态, 开始下一趟扫描 */
				row = col = 0;
				occupy_cur_row = 0;
				last_row = 0;
				break;
			}
			last_row = row;
			occupy_cur_row = 0;
		}

		if (uid_node->uid == empty_key) continue;

		occupy_cur_row++;

		lvl = uid_node->sw_level;
		idx = uid_node->sw_index;

		int32_t update_tw_interval = MAX_UPDATE_START_INTERVAL;
		if (ac_cfg && ac_cfg->status == AC_CFG_STATUS_ENABLE
			&& ac_cfg->update_tw_start_interval < MAX_UPDATE_START_INTERVAL) {
			update_tw_interval = ac_cfg->update_tw_start_interval;
		}
		if (sys_clock - uid_node->tw.last_tv.tv_sec > update_tw_interval) {
			TRACE_LOG("UPDATE_TW_START-1, uid=%u, from: [%ld, %ld], to: [%ld, %ld]",
					uid_node->uid,
					uid_node->tw.start_tv.tv_sec, uid_node->tw.start_tv.tv_usec,
					sys_clock_tv.tv_sec, sys_clock_tv.tv_usec);

			uid_node->tw.count = 0;
			uid_node->tw.last_tv = sys_clock_tv;
			uid_node->tw.start_tv = sys_clock_tv;
			handle++;
		}

		if (uid_node->sw_cmd_count <= 0) {
			uid_node->sw_cmd_count = 0;
			continue;
		}

		clean = 0;
		sw = GET_SW_NODE(lvl, idx);
		for (c = 0; c < uid_node->sw_cmd_count; c++) {
			cmd_node = &(sw->cmd_list[c]);
			struct sw_config_t *sw_cfg = NULL;
			int32_t update_sw_interval = MAX_UPDATE_START_INTERVAL;
			if (ac_cfg && ac_cfg->status == AC_CFG_STATUS_ENABLE) {
				sw_cfg = find_sw_config(ac_cfg, cmd_node->cmd);
				if (sw_cfg
					&& sw_cfg->update_sw_start_interval < MAX_UPDATE_START_INTERVAL) {
					update_sw_interval = sw_cfg->update_sw_start_interval;
				}
			}
			if (sys_clock - cmd_node->last_tv.tv_sec > SW_CMD_KEEPALIVE_TIME) {
				/* 该cmd很久没发了, 清除它 */
				cmd_node->cmd = 0;
				clean++;
				clean_idx = c;
			} else if (sys_clock - cmd_node->last_tv.tv_sec > update_sw_interval) {
				TRACE_LOG("UPDATE_SW_START-1, uid=%u, cmd=%u, "
						"from: [%ld, %ld], to: [%ld, %ld]", uid_node->uid,
						cmd_node->cmd, cmd_node->start_tv.tv_sec,
						cmd_node->start_tv.tv_usec, sys_clock_tv.tv_sec,
						sys_clock_tv.tv_usec);
				cmd_node->count = 0;
				cmd_node->last_tv = sys_clock_tv;
				cmd_node->start_tv = sys_clock_tv;
				handle++;
			}
		}

		if (clean) {
			if (clean == 1) {
				cmd_node_last = &(sw->cmd_list[uid_node->sw_cmd_count - 1]);
				if (uid_node->sw_cmd_count == 1) {
					TRACE_LOG("CLEAN_ONLY_CMD: uid=%u, cmd=%u",
							uid_node->uid, cmd_node_last->cmd);
					memset(cmd_node_last, 0, sizeof(*cmd_node_last));
				} else {
					TRACE_LOG("CLEAN_ONE_CMD: uid=%u, cmd=%u, clean_idx=%u",
							uid_node->uid, cmd_node_last->cmd, clean_idx);
					cmd_node = &(sw->cmd_list[clean_idx]);
					memcpy(cmd_node, cmd_node_last, sizeof(*cmd_node));
				}
				uid_node->sw_cmd_count--;
				handle++;
			} else { /* clean > 1 */
				count = 0;
				for (c = 0; c < uid_node->sw_cmd_count; c++) {
					cmd_node = &(sw->cmd_list[c]);
					if (cmd_node->cmd) {
						TRACE_LOG("CLEAN_MULT_CMD: uid=%u, cmd=%u, clean=%u/%d",
								uid_node->uid, cmd_node_last->cmd, c, clean);
						memcpy(&cmd_list_buf[count], cmd_node, sizeof(*cmd_node));
						count++;
					}
				}
				uid_node->sw_cmd_count = count;
				if (count) {
					memcpy(sw->cmd_list, cmd_list_buf, count * sizeof(*cmd_node));
				}

				handle += clean * 2;
			}
		}
	}
}

int add_step_start_timer_event(struct event_base *base)
{
	struct timeval tmr_tv;

	tmr_tv.tv_sec = AC_UPDATE_START_INTERVAL;
	tmr_tv.tv_usec = 0;
	tmrev_update_start = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, tmr_update_start, NULL);
	evtimer_add(tmrev_update_start, &tmr_tv);
	return 0;
}

void add_timer_events(struct event_base *base)
{
	add_statistics_timer_event(base);
	add_clean_uid_node_timer_event(base);
	add_step_start_timer_event(base);
}

static void quit_signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = user_data;
	struct timeval delay = { 0, 400000 };
	DEBUG_LOG("Done, anticheat_svc=%s\n", ac_local_cfg->svc_name);
	event_base_loopexit(base, &delay);
}

int add_quit_signal_event(struct event_base *base)
{
	quit_signal_event = evsignal_new(base, SIGINT, quit_signal_cb, (void *)base);
	if (!quit_signal_event || event_add(quit_signal_event, NULL) < 0) {
		BOOT_LOG(-1, "Could not create/add a signal event!");
	}

	return 0;
}

void add_signal_events(struct event_base *base)
{
	add_quit_signal_event(base);
}

void tmr_worker(evutil_socket_t pad, short events, void *user_data)
{
	int empty;
	recv_interface(&empty);
}

void add_worker_events(struct event_base *base)
{
    struct timeval tmr_tv;
	tmr_tv.tv_sec = 0;
	tmr_tv.tv_usec = 30; /* 设置成一个很小的间隔, 使得每次主循环都能执行它 */
	tmrev_worker = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, tmr_worker, NULL);
	/* 更新系统时钟: 最高优先级: 保证在循环的一开始执行 */
	event_priority_set(tmrev_worker, 1);
	event_add(tmrev_worker, &tmr_tv);
}

int main(int argc, char **argv)
{
	struct event_base *base;

	sys_clock = time(NULL);

	if (init_system(argc, argv) == -1) {
		BOOT_LOG(-1, "Failed to init_system");
	}

	init_daemon(ac_local_cfg->background, ac_local_cfg->debug_mode);

	udp_sock_fd = create_udp_socket_nobind();
	if (udp_sock_fd == -1) {
		BOOT_LOG(-1, "Failed to create_udp_socket_nobind");
	}

	base = event_base_new();
	if (!base) {
		BOOT_LOG(-1, "Could not initialize libevent!");
	}
	event_base_priority_init(base, 3);

	add_sys_clock_timer(base);
	add_timer_events(base);
	add_signal_events(base);
	add_worker_events(base);

	event_base_dispatch(base);

	return 0;
}
