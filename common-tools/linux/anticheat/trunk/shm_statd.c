#include <stdio.h>
#include <stdlib.h>
#include <confuse.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#include "common.h"
#include "struct.h"
#include "shm_keys.h"
#include "shm.h"
#include "channel.h"
#include "util.h"
#include "hash_table.h"
#include "daemon.h"
#include "svc.h"
#include "svc_hash_table.h"

#include "anticheat.h"

#define GET_SW_NODE(_lvl, _idx) \
	((struct single_watch_t*)(sw_lvl_start[_lvl] + _idx * sw_lvl_node_size_list[_lvl]))


#define DP(_fmt, _args...) \
	do { \
		printf(_fmt"\n", ##_args); \
	} while(0)


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


uint32_t g_row_num;


struct sw_cmd_stat_t {
	/* 命令号 */
	uint32_t	cmd;
	/* 收到cmd的数量 */
	uint64_t	recv;
	/* cmd违规的数量 */
	uint64_t	tot_ill;
	/* cmd间隔违规的数量 */
	uint64_t	int_ill;
	/* cmd频率违规的数量 */
	uint64_t	freq_ill;
};

struct row_stat_t {
	/* 当前row的空节点数量 (应该有: empty + used = row_nnode) */
	int32_t		empty_cnt;
	/* 当前row的未超时点数量 */
	int32_t		used_cnt;
};

struct uid_htab_stat_t {
	/* 当前占据的最后一级 uid_node hash表的行数 (从0开始计数) */
	uint32_t	max_row;
	/* 当前hash表的空节点数量 (应该有: empty + used = total_nnode) */
	int32_t		empty_cnt;
	/* 当前hash表的未超时点数量 */
	int32_t		used_cnt;
	/* hash表每行的统计 */
	struct row_stat_t row_stat[MAX_NODES_NUM_HTAB_SIZE];
};

struct stat_t {
	/* uid hash表的相关统计 */
	struct uid_htab_stat_t		uid_htab;
	/* sw lvln 的已用节点数量 */
	int32_t						lvln_used[SW_MAX_LVL];
};


struct hash_table_t uid_nodes_hash_table, *uid_nodes_htab = &uid_nodes_hash_table;
struct shm_mgr_t uid_node_hash_table_shm_mgr, *uid_nodes_mgr = &uid_node_hash_table_shm_mgr;

struct stat_t stat_st,  *stat = &stat_st;

uint32_t empty_key = 0;


/* return: 0: node 的 key 与 key 相同; 1: node 的 key 与 key 不相同 */
static int uid_node_compare(const void *key, const void *node)
{
	uint32_t _key = *((uint32_t *)key);
	struct uid_node_t *_node = (struct uid_node_t *)node;
	return ((_key == _node->uid) ? 0 : 1);
}


int init_uid_nodes_htab(uint32_t svc_gameid)
{
	uint32_t row_num;
	uint32_t *nodes_num;
	uint32_t *mods;

	void *table = NULL;
	size_t node_size = sizeof(struct uid_node_t);
	size_t size = get_svc_hash_table_size(svc_gameid, node_size, &row_num, &nodes_num, &mods);
	struct hash_table_t *htab = uid_nodes_htab;
	key_t key = SHM_KEY_UID_NODE + svc_gameid * 1000;

	init_shm_mgr(uid_nodes_mgr, key);
	if (get_shm_nocreate_noinitexist(key, size) == -1) {
		DP("Failed to get_shm, gameid=%u, key=%#x", svc_gameid, key);
		return -1;
	}
	if (update_shm_attach(uid_nodes_mgr) == -1) {
		DP("Failed to update uid_nodes_mgr shm, gameid=%u, key=%#x", svc_gameid, key);
		return -1;
	}
	table = uid_nodes_mgr->shm;
	if (hash_table_init(htab, table, size, node_size, row_num, nodes_num, mods, uid_node_compare) == -1) {
		DP("Failed to hash_table_init, gameid=%u, key=%#x", svc_gameid, key);
		return -1;
	}
	if (htab->total_node_num > MAX_UID_NODE_NUM) {
		DP("Too many hash_table nodes (%u over %d)", htab->total_node_num, MAX_UID_NODE_NUM);
		return -1;
	}

	g_row_num = row_num;
	return 0;
}

int init_single_watch_shm(uint32_t gameid)
{
#define INIT_SW_WATCH(__lvl, __mgr, __key) \
	do { \
		size_t __size = sw_lvl_size_list[__lvl]; \
		init_shm_mgr(__mgr, __key); \
		get_shm_nocreate_noinitexist(__key, __size); \
		if (update_shm_attach(__mgr) == -1) { \
			DP("Failed to update uid_nodes_mgr, key=%#x", __key); \
			return -1; \
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

int main(int argc, char **argv)
{
	if (argc != 3) {
		DP("Usage: %s <svc_conf> <svc_name>", argv[0]);
		return -1;
	}
	
	char *svc_conf = argv[1];
	char *svc_name = argv[2];

	uint32_t load_count;
	uint32_t svc_gameid;
	if (load_svc_gameid_map(svc_conf, &load_count) == -1) {
		DP("Failed to load_svc_gameid_map");
		return -1;
	}

	if (load_count == 0) {
		DP("Not any svcinfo in svc.conf");
		return -1;
	}
	svc_gameid = get_svc_gameid(svc_name);
	if (svc_gameid == 0) {
		DP("Unsupported svc(%s) in svc_conf: %s", svc_name, svc_conf);
		return -1;
	}

	if (init_uid_nodes_htab(svc_gameid) == -1) {
		DP("Failed to init_uid_nodes_htab");
		return -1;
	}

	if (init_single_watch_shm(svc_gameid) == -1) {
		DP("Failed to init_single_watch_shm");
		return -1;
	}

	uint32_t row = 0;
	uint32_t col = 0;

	int scan = 0;
	struct uid_node_t *uid_node;
	while (1) {
		if (scan++ >= AC_CLEAN_UID_NODE_SCAN_LIMIT) {
			scan = 0;
			usleep(8000);
		}

		struct row_stat_t *row_stat = &(stat->uid_htab.row_stat[row]);

		uid_node = (struct uid_node_t *)hash_table_walk(uid_nodes_htab, &row, &col);
		if (!uid_node) {
			/* 完成一遍扫描, 退出循环 */
			break;
		}

		if (uid_node->uid == empty_key) {
			stat->uid_htab.empty_cnt++;
			row_stat->empty_cnt++;
		} else {
			if (row > stat->uid_htab.max_row) {
				stat->uid_htab.max_row = row;
			}
			stat->uid_htab.used_cnt++;
			row_stat->used_cnt++;
		}
	}

	int lvl, idx;
	int sleep_count = 0;
	struct single_watch_t *sw = NULL;
	for (lvl = 0; lvl < SW_MAX_LVL; lvl++) {
		for (idx = 0; idx < sw_lvl_nnode_list[lvl]; idx++) {
			if (sleep_count++ >= AC_CLEAN_UID_NODE_SCAN_LIMIT) {
				sleep_count = 0;
				usleep(10000);
			}

			sw = GET_SW_NODE(lvl, idx);
			if (sw->uid) {
				stat->lvln_used[lvl]++;
			}
		}
	}

	/* show stat info */
	int i = 0;
	DP("==================[SHM STAT INFO]==================");
	DP("\n------------------[uid-hash-table]-----------------");
	DP("\tmax_row: %u\n\tempty_cnt: %d\n\tused_cnt: %d",
			stat->uid_htab.max_row,
			stat->uid_htab.empty_cnt,
			stat->uid_htab.used_cnt);

	DP("\n-------------[uid-hash-table-row-info]-----------");
	for (i = 0; i < g_row_num; i++) {
		DP("\trow: %u, empty: %d, used: %d", i,
				stat->uid_htab.row_stat[i].empty_cnt,
				stat->uid_htab.row_stat[i].used_cnt);
	}

	DP("\n----------------[sw-lvln-table]------------------");
	for (i = 0; i < SW_MAX_LVL; i++) {
		DP("\tlvl: %d, used: %d (total: %d, spec: %d, lvl_node_size: %u, lvl_size: %u)", i,
				stat->lvln_used[i], sw_lvl_nnode_list[i], sw_lvl_spec_list[i],
				sw_lvl_node_size_list[i], sw_lvl_size_list[i]);
	}
	DP("\n==================[SHM STAT END]===================");

	return 0;
}
