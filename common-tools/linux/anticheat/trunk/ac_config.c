#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "struct.h"
#include "shm_keys.h"
#include "shm.h"
#include "util.h"
#include "mysql_wrapper.h"

#include "ac_config.h"



#define DP(_fmt, _args...) \
	do { \
		printf(_fmt"\n", ##_args); \
	} while(0)



struct shm_mgr_t ac_config_shm_mgr, *ac_cfg_mgr = &ac_config_shm_mgr;
struct anticheat_config_t *ac_cfg;
struct dbmgr_t db_manager, *dbmgr = &db_manager;
struct ac_cfgd_config_t ac_cfgd_config, *ac_cfgd_cfg = &ac_cfgd_config;

int init_ac_cfgd_config(char *conf_path, struct ac_cfgd_config_t *config)
{
	int ret;

	cfg_opt_t opts[] = {
		CFG_BOOL("background", cfg_false, CFGF_NONE),
		CFG_BOOL("debug_mode", cfg_false, CFGF_NONE),

		CFG_STR("svc_name", SVC_TEST_NAME, CFGF_NONE),
		CFG_INT("svc_gameid", SVC_TEST_GAMEID, CFGF_NONE),
		CFG_STR("cc_ip", "localhost", CFGF_NONE),
		CFG_INT("cc_port", 3306, CFGF_NONE),

		CFG_STR("cc_my_user", "root", CFGF_NONE),
		CFG_STR("cc_my_passwd", "ta0mee", CFGF_NONE),
		CFG_STR("cc_my_dbname", "anticheat", CFGF_NONE),
		CFG_STR("cc_my_tw_tab_basename", "tw_config", CFGF_NONE),
		CFG_STR("cc_my_sw_tab_basename", "sw_config", CFGF_NONE),

		CFG_END()
	};

	cfg_t *cfg = cfg_init(opts, CFGF_NONE);
	ret = cfg_parse(cfg, conf_path ? conf_path : DEF_CFG_PATH);
	if(ret == CFG_FILE_ERROR) {
		DP("Access error, conf: %s, err: %s",
				conf_path ? conf_path : DEF_CFG_PATH, strerror(errno));
		return -1;
	} else if(ret == CFG_PARSE_ERROR) {
		DP("Parse error, conf: %s", conf_path ? conf_path : DEF_CFG_PATH);
		return -1;
	}

	memset(config, 0, sizeof(*config));

	config->background = cfg_getbool(cfg, "background");
	config->debug_mode = cfg_getbool(cfg, "debug_mode");
	snprintf(config->svc_name, sizeof(config->svc_name), "%s", cfg_getstr(cfg, "svc_name"));
	config->svc_gameid = cfg_getint(cfg, "svc_gameid");
	snprintf(config->cc_ip, sizeof(config->cc_ip), "%s", cfg_getstr(cfg, "cc_ip"));
	config->cc_port = cfg_getint(cfg, "cc_port");

	snprintf(config->cc_my_user, sizeof(config->cc_my_user),
			"%s", cfg_getstr(cfg, "cc_my_user"));
	snprintf(config->cc_my_passwd, sizeof(config->cc_my_passwd),
			"%s", cfg_getstr(cfg, "cc_my_passwd"));
	snprintf(config->cc_my_dbname, sizeof(config->cc_my_dbname),
			"%s", cfg_getstr(cfg, "cc_my_dbname"));
	snprintf(config->cc_my_tw_tab_basename, sizeof(config->cc_my_tw_tab_basename),
			"%s", cfg_getstr(cfg, "cc_my_tw_tab_basename"));
	snprintf(config->cc_my_sw_tab_basename, sizeof(config->cc_my_sw_tab_basename),
			"%s", cfg_getstr(cfg, "cc_my_sw_tab_basename"));

	if (config->svc_gameid > MAX_SVC_GAMEID) {
		DP("Invalid svc_gameid: %u for svc: %s", config->svc_gameid, config->svc_name);
		return -1;
	}


	cfg_free(cfg);

	return 0;
}


int init_anticheat_config_shm(uint32_t gameid)
{
	size_t size = sizeof(struct anticheat_config_t);
	key_t key = SHM_KEY_AC_CONFIG + gameid * 1000;
	init_shm_mgr(ac_cfg_mgr, key);
	if (get_shm_create_noinitexist(key, size, NULL, 0) == -1) {
		DP("Failed to get ac_cfg_mgr shm, key=%#x", key);
		return -1;
	}
	if (update_shm_attach(ac_cfg_mgr) == -1) {
		DP("Failed to update ac_cfg_mgr, key=%#x", key);
		return -1;
	}
	ac_cfg = (struct anticheat_config_t *)(ac_cfg_mgr->shm);

	return 0;
}

int init_db_connection(void)
{
	struct dbconn_t *dbconn = &(dbmgr->dbconn);

	my_init_db(dbmgr);
	sprintf(dbconn->host, "%s", ac_cfgd_cfg->cc_ip);
	dbconn->port = ac_cfgd_cfg->cc_port;
	sprintf(dbconn->user, "%s", ac_cfgd_cfg->cc_my_user);
	sprintf(dbconn->passwd, "%s", ac_cfgd_cfg->cc_my_passwd);
	sprintf(dbconn->db, "%s", ac_cfgd_cfg->cc_my_dbname);
	if (!dbmgr->conned && my_connect_db(dbmgr) == -1) {
		DP("Cannot connect to db, err(%d): %s",
				mysql_errno(&dbmgr->my), mysql_error(&dbmgr->my));
		return -1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	int i;
	char *conf_path = (argc == 2) ? argv[1] : NULL;
	if (init_ac_cfgd_config(conf_path, ac_cfgd_cfg) == -1) {
		DP("Failed to init_ac_cfgd_config");
		return -1;
	}

	if (init_anticheat_config_shm(ac_cfgd_cfg->svc_gameid)) {
		DP("Failed to init_anticheat_config_shm, svc_gameid=%d",
				ac_cfgd_cfg->svc_gameid);
		return -1;
	}

	if (ac_cfg->svc_name[0] != '\0'
		&& strncmp(ac_cfg->svc_name, ac_cfgd_cfg->svc_name, MAX_SVC_NAME_LEN)) {
		DP("Prev svc(%s)'s cfg exists and not for cur_svc(%s)",
				ac_cfg->svc_name, ac_cfgd_cfg->svc_name);
		return -1;
	}

	if (init_db_connection() == -1) {
		DP("Failed to init_db_connection");
		return -1;
	}

	char table[MAX_SVC_NAME_LEN + 128];
	sprintf(table, "%s.%s_%s", ac_cfgd_cfg->cc_my_dbname,
			ac_cfgd_cfg->svc_name, ac_cfgd_cfg->cc_my_tw_tab_basename);

	int sqllen;
	sqllen = snprintf(dbmgr->sql, MAX_SQL_LEN, "SELECT "
			"db_addr_ip,"
			"db_addr_port,"
			"min_tw_interval,"
			"max_tw_freq,"
			"update_tw_start_interval"
			" FROM %s", table);
	if (sqllen >= MAX_SQL_LEN) {
		DP("tw: Sql is Too long, sqllen: %d", sqllen);
		return -1;
	}

//DP("exex_sql: %s", dbmgr->sql);
	if (my_exec_sql(dbmgr, 0) == -1) {
		DP("tw: Failed exec sql, err(%d): %s",
				mysql_errno(&dbmgr->my), mysql_error(&dbmgr->my));
		return -1;
	}

	if (my_num_rows(dbmgr, 0) == 0) {
		DP("tw: Not any tw_config when my_num_rows");
		return -1;
	}

	if (my_fetch_row(dbmgr, 0) == -1) {
		DP("tw: Failed to fetch row, err(%d): %s",
				mysql_errno(&dbmgr->my), mysql_error(&dbmgr->my));
		return -1;
	}

	ac_cfg->status = AC_CFG_STATUS_DISABLE;

	if (ac_cfg->svc_name[0] == '\0') {
		snprintf(ac_cfg->svc_name, sizeof(ac_cfg->svc_name), "%s", ac_cfgd_cfg->svc_name);
	}
	snprintf(ac_cfg->db_addr_ip, sizeof(ac_cfg->db_addr_ip), "%s", dbmgr->my_row[0]);
	ac_cfg->db_addr_port = atoi(dbmgr->my_row[1]);
	ac_cfg->min_tw_interval = atoll(dbmgr->my_row[2]);
	ac_cfg->max_tw_freq = atof(dbmgr->my_row[3]);
	ac_cfg->update_tw_start_interval = atol(dbmgr->my_row[4]);

	ac_cfg->status = AC_CFG_STATUS_ENABLE;

	sprintf(table, "%s.%s_%s", ac_cfgd_cfg->cc_my_dbname,
			ac_cfgd_cfg->svc_name, ac_cfgd_cfg->cc_my_sw_tab_basename);
	sqllen = snprintf(dbmgr->sql, MAX_SQL_LEN, "SELECT "
			"cmd,"
			"min_sw_interval,"
			"max_sw_freq,"
			"update_sw_start_interval,"
			"label"
			" FROM %s ORDER BY cmd ASC", table);
	if (sqllen >= MAX_SQL_LEN) {
		DP("sw: Sql is Too long, sqllen: %d", sqllen);
		return -1;
	}

//DP("exex_sql: %s", dbmgr->sql);
	if (my_exec_sql(dbmgr, 0) == -1) {
		DP("sw: Failed exec sql, err(%d): %s",
				mysql_errno(&dbmgr->my), mysql_error(&dbmgr->my));
		return -1;
	}

	int32_t sw_config_real_count = 0;
	int32_t sw_config_count = my_num_rows(dbmgr, 0);
	struct sw_config_t *sw_cfg = NULL;
	if (sw_config_count > MAX_CONFIG_SW_CMD) {
		DP("Warning: too many sw_config_count: %d, cut it to: %d",
				sw_config_count, MAX_CONFIG_SW_CMD);
		sw_config_count = MAX_CONFIG_SW_CMD;
	}

	ac_cfg->status = AC_CFG_STATUS_DISABLE;
	for (i = 0; i < sw_config_count; i++) {
		if (my_fetch_row(dbmgr, 0) == -1) {
			DP("sw: Failed to fetch row, err(%d): %s",
					mysql_errno(&dbmgr->my), mysql_error(&dbmgr->my));
			ac_cfg->sw_config_count = sw_config_real_count;
			ac_cfg->status = AC_CFG_STATUS_ENABLE;
			return -1;
		}

		sw_cfg = &(ac_cfg->sw_config[i]);
		sw_cfg->cmd = atoi(dbmgr->my_row[0]);
		sw_cfg->min_sw_interval = atoll(dbmgr->my_row[1]);
		sw_cfg->max_sw_freq = atof(dbmgr->my_row[2]);
		sw_cfg->update_sw_start_interval = atoi(dbmgr->my_row[3]);
		snprintf(sw_cfg->label, sizeof(sw_cfg->label), "%s", dbmgr->my_row[4]);
		sw_config_real_count++;
	}
	ac_cfg->sw_config_count = sw_config_real_count;
	ac_cfg->status = AC_CFG_STATUS_ENABLE;


	my_close_db(dbmgr);

	return 0;
}
