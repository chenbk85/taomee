#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "common.h"
#include "struct.h"
#include "shm_keys.h"
#include "shm.h"
#include "svc.h"


#include "ac_config.h"



#define DP(_fmt, _args...) \
	do { \
		printf(_fmt"\n", ##_args); \
	} while(0)



struct shm_mgr_t ac_config_shm_mgr, *ac_cfg_mgr = &ac_config_shm_mgr;
struct anticheat_config_t *ac_cfg;

static char *svc_name_map[MAX_SVC_NUM];

int init_svc_name_map(void)
{
#define ADD_SVC_NAME(_gameid, _svc_name) \
	do { \
		if (_gameid >= MAX_SVC_NUM) { \
			fprintf(stderr, "Too big svc_gameid: %d\n", _gameid); \
		} \
		svc_name_map[_gameid] = _svc_name; \
	} while(0)

	int i = 0;
	for (; i < MAX_SVC_NUM; i++) {
		svc_name_map[i] = NA_SVC_NAME;
	}

	ADD_SVC_NAME(1, "mole");
	ADD_SVC_NAME(2, "seer");
	ADD_SVC_NAME(3, "2125");
	ADD_SVC_NAME(4, "DUDU");
	ADD_SVC_NAME(5, "xhx");
	ADD_SVC_NAME(6, "gf");
	ADD_SVC_NAME(7, "hero");
	ADD_SVC_NAME(8, "mole2");
	ADD_SVC_NAME(9, "bus");
	ADD_SVC_NAME(10, "seer2");
	ADD_SVC_NAME(12, "POP");
	ADD_SVC_NAME(13, "xgs");
	ADD_SVC_NAME(21, "haqi");

#undef ADD_SVC_NAME
	return 0;
}

int init_anticheat_config_shm(uint32_t svc_gameid)
{
	size_t size = sizeof(struct anticheat_config_t);
	key_t key = SHM_KEY_AC_CONFIG + svc_gameid * 1000;
	init_shm_mgr(ac_cfg_mgr, key);
	if (get_shm_create_noinitexist(key, size, NULL, 0) == -1) {
		fprintf(stderr, "Failed to get ac_cfg_mgr shm, key=%#x\n", key); /* TODO: log */
		return -1;
	}
	if (update_shm_attach(ac_cfg_mgr) == -1) {
		fprintf(stderr, "Failed to update ac_cfg_mgr, key=%#x\n", key); /* TODO: log */
		return -1;
	}
	ac_cfg = (struct anticheat_config_t *)(ac_cfg_mgr->shm);

	return 0;
}

int main(int argc, char **argv)
{
	int i;
	struct sw_config_t *sw_cfg;
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <svc_conf> <svc_name>\n", argv[0]);
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

	if (init_anticheat_config_shm(svc_gameid)) {
		fprintf(stderr, "ERROR: Failed to init_anticheat_config_shm, "
				"svc_name: %s, svc_gameid: %d\n", svc_name, svc_gameid);
		return -1;
	}

	DP("==============[AC-CONFIG for SVC: %s(%d) START]==============",
			svc_name, svc_gameid);
	DP("--------------[tw-configs]--------------");
	DP("status(%u): %s", ac_cfg->status, ac_cfg->status ? "ENABLE" : "DISABLE");
	DP("svc_name: %s", ac_cfg->svc_name);
	DP("db_addr_ip: %s", ac_cfg->db_addr_ip);
	DP("db_addr_port: %hu", ac_cfg->db_addr_port);
	DP("min_tw_interval: %ld", ac_cfg->min_tw_interval);
	DP("max_tw_freq: %f", ac_cfg->max_tw_freq);
	DP("update_tw_start_interval: %d", ac_cfg->update_tw_start_interval);
	DP("----------------------------------------");
	DP("--------------[sw-configs]--------------");
	DP("sw_config_count: %d", ac_cfg->sw_config_count);
	for (i = 0; i < ac_cfg->sw_config_count; i++) {
		sw_cfg = &(ac_cfg->sw_config[i]);
		DP("-----sw_config[%d]-----", i);
		DP("\tcmd: %u", sw_cfg->cmd);
		DP("\tcmd_label: %s", sw_cfg->label);
		DP("\tmin_sw_interval: %ld", sw_cfg->min_sw_interval);
		DP("\tmax_sw_freq: %f", sw_cfg->max_sw_freq);
		DP("\tupdate_sw_start_interval: %d", sw_cfg->update_sw_start_interval);
	}
	DP("----------------------------------------");
	DP("==============[AC-CONFIG for SVC: %s END]==============", svc_name);

	return 0;
}
