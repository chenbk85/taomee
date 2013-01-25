#include <stdio.h>
#include <stdlib.h>
#include <confuse.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"
#include "struct.h"
#include "shm_keys.h"
#include "shm.h"
#include "channel.h"
#include "util.h"
#include "hash_table.h"
#include "daemon.h"
#include "log.h"
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


struct hash_table_t uid_nodes_hash_table, *uid_nodes_htab = &uid_nodes_hash_table;
struct shm_mgr_t uid_node_hash_table_shm_mgr, *uid_nodes_mgr = &uid_node_hash_table_shm_mgr;
int empty_key = 0;


/* return: 0: node 的 key 与 key 相同; 1: node 的 key 与 key 不相同 */
static int uid_node_compare(const void *key, const void *node)
{
	uint32_t _key = *((uint32_t *)key);
	struct uid_node_t *_node = (struct uid_node_t *)node;
	return ((_key == _node->uid) ? 0 : 1);
}


int init_uid_nodes_htab(uint32_t svc_gameid,
		uint32_t *row_num, uint32_t **nodes_num, uint32_t **mods)
{
	void *table = NULL;
	size_t node_size = sizeof(struct uid_node_t);
	size_t size = get_svc_hash_table_size(svc_gameid, node_size, row_num, nodes_num, mods);
	struct hash_table_t *htab = uid_nodes_htab;
	key_t key = SHM_KEY_UID_NODE + svc_gameid * 1000;

	init_shm_mgr(uid_nodes_mgr, key);
	if (get_shm_nocreate_noinitexist(key, size) == -1) {
		DP("Failed to get_shm, gameid=%u, key=%#x", svc_gameid, key);
		return -1;
	}
	if (update_shm_attach(uid_nodes_mgr) == -1) {
		DP("Failed to update uid_nodes_mgr shm, gameid=%u, key=%#x",
				svc_gameid, key);
		return -1;
	}
	table = uid_nodes_mgr->shm;
	if (hash_table_init(htab, table, size, node_size,
				*row_num, *nodes_num, *mods, uid_node_compare) == -1) {
		DP("Failed to hash_table_init, gameid=%u, key=%#x", svc_gameid, key);
		return -1;
	}
	if (htab->total_node_num > MAX_UID_NODE_NUM) {
		DP("Too many hash_table nodes (%u over %d)", htab->total_node_num, MAX_UID_NODE_NUM);
		return -1;
	}

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

int init_system(const char *svc_conf, const char *svc_name,
		uint32_t *row_num, uint32_t **nodes_num, uint32_t **mods)
{
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

	if (init_uid_nodes_htab(svc_gameid, row_num, nodes_num, mods) == -1) {
		DP("Failed to init_uid_nodes_htab");
		return -1;
	}

	if (init_single_watch_shm(svc_gameid) == -1) {
		DP("Failed to init_single_watch_shm");
		return -1;
	}

	return 0;
}

void show_one_uid_node(struct uid_node_t *uid_node, uint32_t row, uint32_t col)
{
	int i = 0;
	int len;
	char fmt_time[128];
	time_t last_access = uid_node->last_access;

	struct cmd_t *cmd_node;
	struct single_watch_t *sw;

	DP("===================[UID_NODE INFO]===================");
	DP("\n-----------------[uid-hash-table]-----------------");
	DP("\tuid: %u\trow: %u\tcol: %u", uid_node->uid, row, col);

	DP("\ttw.count: %u", uid_node->tw.count);
	DP("\tcli-addr: %s:%hu (%u:%hu)", inet_ntoa(*(struct in_addr *)&uid_node->ip),
			uid_node->port, uid_node->ip, uid_node->port);

	DP("\n-------------[uid_node-detail-info]-----------");
	len = sprintf(fmt_time, "%s", ctime((const time_t *)&last_access));
	fmt_time[len-1] = '\0';
	DP("\tlast_access: %d (%s)", uid_node->last_access, fmt_time);

	len = sprintf(fmt_time, "%s", ctime(&uid_node->tw.last_tv.tv_sec));
	fmt_time[len-1] = '\0';
	DP("\ttw.last_tv: [%ld, %ld] (%s)",
			uid_node->tw.last_tv.tv_sec, uid_node->tw.last_tv.tv_usec, fmt_time);

	len = sprintf(fmt_time, "%s", ctime(&uid_node->tw.start_tv.tv_sec));
	fmt_time[len-1] = '\0';
	DP("\ttw.start_tv: [%ld, %ld] (%s)",
			uid_node->tw.start_tv.tv_sec, uid_node->tw.start_tv.tv_usec, fmt_time);

	DP("\n-------------[uid_node-sw-detail-info]-----------");
	DP("\tsw_cmd_count: %u", uid_node->sw_cmd_count);
	DP("\tsw_level: %u, sw_index: %u", uid_node->sw_level, uid_node->sw_index);

	sw = GET_SW_NODE(uid_node->sw_level, uid_node->sw_index);
	for (i = 0; i < uid_node->sw_cmd_count; i++) {
		DP("\tNo.%d, sw->uid: %u", i, sw->uid);
		cmd_node = &(sw->cmd_list[i]);
		DP("\t\tcmd_node->cmd: %u", cmd_node->cmd);
		DP("\t\tcmd_node->count: %u", cmd_node->count);

		len = sprintf(fmt_time, "%s", ctime(&cmd_node->last_tv.tv_sec));
		fmt_time[len-1] = '\0';
		DP("\t\tcmd_node->last_tv: [%ld, %ld] (%s)",
				cmd_node->last_tv.tv_sec, cmd_node->last_tv.tv_usec, fmt_time);

		len = sprintf(fmt_time, "%s", ctime(&cmd_node->start_tv.tv_sec));
		fmt_time[len-1] = '\0';
		DP("\t\tcmd_node->start_tv: [%ld, %ld] (%s)",
				cmd_node->start_tv.tv_sec, cmd_node->start_tv.tv_usec, fmt_time);
		DP("----------------------------");
	}

	DP("\n==================[UID_NODE END]===================");
}

int main(int argc, char **argv)
{
	if (argc != 4 && argc != 5 && argc != 6) {
		DP("Usage: %s <svc_conf> <svc_name> <uid> [row] [col]", argv[0]);
		return -1;
	}
	int show_one_row = 0;
	char *svc_conf = argv[1];
	char *svc_name = argv[2];
	uint32_t uid = atoi(argv[3]);

	if (uid == 0 && argc != 5 && argc != 6) {
		DP("Usage: %s <svc_conf> <svc_name> <uid> <row> [col]", argv[0]);
		return -1;
	}

	uint32_t row = 0, col = 0;
	if (argc >= 5) {
		row = atoi(argv[4]);
		show_one_row = 1;
		if (argc == 6) {
			col = atoi(argv[5]);
			show_one_row = 0;
		}
	}

	uint32_t row_num, *nodes_num, *mods;
	if (init_system(svc_conf, svc_name, &row_num, &nodes_num, &mods) == -1) {
		DP("Failed to init_system");
		return -1;
	}

	if (row >= row_num || col >= nodes_num[row]) {
		DP("Invalid row: %u(max: %u), or col: %u(max: %u)",
				row, col, row_num, (row < row_num) ? nodes_num[row] : 0);
		return -1;
	}

	struct uid_node_t *uid_node = NULL;

	if (show_one_row) { /* show one row */
		int c = 0;
		for (; c < nodes_num[row]; c++) {
			uid_node = hash_table_get_node(uid_nodes_htab, row, c);
			if (uid_node->uid == 0) {
				//DP("row: %u, col: %u svc: %s is empty", row, c, svc_name);
				continue;
			}
			show_one_uid_node(uid_node, row, c);
		}
		return 0;
	}

	if (uid == 0) {
		uid_node = hash_table_get_node(uid_nodes_htab, row, col);
		if (uid_node->uid == 0) {
			DP("row: %u, col: %u svc: %s is empty", row, col, svc_name);
			return 0;
		}
	} else {
		uid_node = HTAB_LOOKUP_POSINFO(uid_nodes_htab, uid, row, col);
		if (!uid_node) {
			DP("uid: %u is not coming in svc: %s", uid, svc_name);
			return 0;
		}
	}

	/* show uid_node info */
	show_one_uid_node(uid_node, row, col);

	return 0;
}
