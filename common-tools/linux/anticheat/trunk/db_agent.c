#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <error.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

#include <event2/util.h>
#include <event2/event.h>


#include "common.h"
#include "struct.h"
#include "shm_keys.h"
#include "shm.h"
#include "channel.h"
#include "util.h"
#include "mysql_wrapper.h"
#include "log.h"
#include "daemon.h"


#include "db_agent.h"


struct dbagt_config_t dbagt_config, *dbagt_cfg = &dbagt_config;

time_t sys_clock;
struct timeval sys_clock_tv;

struct dbmgr_t db_manager, *dbmgr = &db_manager;

/* libevent events starts */
/* priority: 0 */
struct event *tmrev_renew_sys_clock;
/* priority: 1 */
struct event *tmrev_worker;
/* priority: 3 */
struct event *tmrev_statistics;

/* signal events */
struct event *quit_signal_event;
/* libevent events end */

struct shm_mgr_t dbagent_channel_shm_mgr, *dbagent_chl_mgr = &dbagent_channel_shm_mgr;

char recvbuf[RCV_BUF_LEN];

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
}

int add_statistics_timer_event(struct event_base *base)
{
	struct timeval tmr_tv;

	tmr_tv.tv_sec = DB_AGENT_STATISTICS_INTERVAL;
	tmr_tv.tv_usec = 0;
	tmrev_statistics = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, tmr_do_statistics, NULL);
	evtimer_add(tmrev_statistics, &tmr_tv);

	return 0;
}

void add_timer_events(struct event_base *base)
{
	add_statistics_timer_event(base);
}

static void quit_signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = user_data;
	struct timeval delay = { 0, 400000 };
	DEBUG_LOG("Done, db_agent");
	event_base_loopexit(base, &delay);
}

int add_quit_signal_event(struct event_base *base)
{
	quit_signal_event = evsignal_new(base, SIGINT, quit_signal_cb, (void *)base);
	if (!quit_signal_event || event_add(quit_signal_event, NULL) < 0) {
		ERROR_LOG("Could not create/add a signal event!");
		return 1;
	}

	return 0;
}

void add_signal_events(struct event_base *base)
{
	add_quit_signal_event(base);
}

int store_ill_record_to_db(struct ill_report_rcd_t *record)
{
	int sqllen;
	char *sql = dbmgr->sql;
	char table[MAX_SVC_NAME_LEN + 128];
	struct dbagt_config_t *cfg = dbagt_cfg;
	sprintf(table, "%s.%s_%s", cfg->my_dbname, record->svc_name, cfg->my_tab_basename);

	sqllen = snprintf(sql, MAX_SQL_LEN, "INSERT INTO %s ("
			"tw_ill_code,"
			"sw_ill_code,"
			"uid,"
			"cmd,"
			"cmd_recv_sec,"
			"ip,"
			"port,"
			"rpt_ip,"
			"rpt_port,"
			"tw_last_int,"
			"tw_int_limit,"
			"tw_start_int,"
			"tw_count,"
			"tw_freq,"
			"tw_freq_limit,"
			"sw_last_int,"
			"sw_int_limit,"
			"sw_start_int,"
			"sw_count,"
			"sw_freq,"
			"sw_freq_limit"
			") "
			"VALUES ("
			"'%u'," /* tw_ill_code */
			"'%u'," /* sw_ill_code */
			"'%u'," /* uid */
			"'%u'," /* cmd */
			"FROM_UNIXTIME('%u')," /* cmd_recv_sec */
			"'%u'," /* ip */
			"'%hu'," /* port */
			"'%u'," /* reporter_ip */
			"'%hu'," /* reporter_port */
			"'%ld'," /* tw_last_int */
			"'%ld'," /* tw_int_limit */
			"'%ld'," /* tw_start_int */
			"'%lu'," /* tw_count */
			"'%f'," /* tw_freq */
			"'%ld'," /* tw_freq_limit */
			"'%ld'," /* sw_last_int */
			"'%ld'," /* sw_int_limit */
			"'%ld'," /* sw_start_int */
			"'%lu'," /* sw_count */
			"'%f'," /* sw_freq */
			"'%ld'" /* sw_freq_limit */
			")",
			table,
			record->tw_ill_code,
			record->sw_ill_code,
			record->uid,
			record->cmd,
			record->cmd_recv_sec,
			record->ip,
			record->port,
			record->reporter_ip,
			record->reporter_port,
			record->tw_last_int,
			record->tw_int_limit,
			record->tw_start_int,
			record->tw_count,
			record->tw_freq,
			record->tw_freq_limit,
			record->sw_last_int,
			record->sw_int_limit,
			record->sw_start_int,
			record->sw_count,
			record->sw_freq,
			record->sw_freq_limit
			);
	if (sqllen >= MAX_SQL_LEN) {
		ERROR_LOG("Sql is Too long, sqllen: %d", sqllen);
		return -1;
	}

//DEBUG_LOG("exex_sql: %s", sql);
	if (my_exec_sql(dbmgr, 0) == -1) {
		ERROR_LOG("Failed exec sql, err(%d): %s",
				mysql_errno(&dbmgr->my), mysql_error(&dbmgr->my));
		return -1;
	}

	return 0;
}

int handle_one_ill_record(struct ill_report_rcd_t *report)
{
	struct ill_report_rcd_t record_struct;
	struct ill_report_rcd_t *record = &record_struct;

	memset(record, 0, sizeof(*record));
	snprintf(record->svc_name, MAX_SVC_NAME_LEN, "%s", report->svc_name);
	record->tw_ill_code = report->tw_ill_code;
	record->sw_ill_code = report->sw_ill_code;
	record->uid = report->uid;
	record->cmd = report->cmd;
	record->cmd_recv_sec = report->cmd_recv_sec;
	record->ip = report->ip;
	record->port = report->port;
	record->reporter_ip = report->reporter_ip;
	record->reporter_port = report->reporter_port;

	if (record->tw_ill_code) {
		record->tw_ill_code = report->tw_ill_code;
		record->tw_last_int = report->tw_last_int;
		record->tw_start_int = report->tw_start_int;
		record->tw_int_limit = report->tw_int_limit;
		record->tw_count = report->tw_count;
		record->tw_freq = report->tw_freq;
		record->tw_freq_limit = report->tw_freq_limit;
	}

	if (record->sw_ill_code) {
		record->sw_ill_code = report->sw_ill_code;
		record->sw_last_int = report->sw_last_int;
		record->sw_int_limit = report->sw_int_limit;
		record->sw_start_int = report->sw_start_int;
		record->sw_count = report->sw_count;
		record->sw_freq = report->sw_freq;
		record->sw_freq_limit = report->sw_freq_limit;
	}

	return store_ill_record_to_db(record);
}

int proc_conn_pkg(struct svr_pkg_t *conn_pkg, uint32_t bodylen)
{
	struct ill_report_rcd_t *record = NULL;

	if (bodylen < sizeof(struct ill_report_t)) {
		ERROR_LOG("bodylen(%u) < sizeof(struct ill_report_t), uid=%u, cmd=%u",
				bodylen, conn_pkg->uid, conn_pkg->cmd);
		return -1;
	}

	struct ill_report_t *ill_report = (struct ill_report_t *)(conn_pkg->body);
	if (ill_report->count > MAX_ONCE_REPORT_NUM) {
		ERROR_LOG("ill_report->count(%u) is too many, uid=%u, cmd=%u",
				ill_report->count, conn_pkg->uid, conn_pkg->cmd);
		return -1;
	}
	if (ill_report->count == 0) {
		ERROR_LOG("ill_report->count is 0, uid=%u, cmd=%u",
				conn_pkg->uid, conn_pkg->cmd);
		return 0;
	}
	uint32_t explen = sizeof(struct ill_report_t) + ill_report->count * sizeof(struct ill_report_rcd_t);
	if (explen != bodylen) {
		ERROR_LOG("ill_report record data mismatch: explen=%u, bodylen=%u, "
				"uid=%u, cmd==%u", explen, bodylen, conn_pkg->uid, conn_pkg->cmd);
		return -1;
	}
	int i = 0;
	for (; i < ill_report->count; i++) {
		record = &(ill_report->record[i]);
		if (handle_one_ill_record(record) == -1) {
			/* TODO: 改成一次report的所有违规项目一次insert
			 * (这里要考虑不同的违规有不同导入, 可以根据cmd来区分)
			 */
			return -1;
		}
	}

	return 0;
}

/**
 * @return 0: success, -1: fail
 */
int recv_interface(void)
{
	int count = 0;
	int max_count = 100; /* TODO: config? */

	struct channel_t *chl = (struct channel_t *)(dbagent_chl_mgr->shm);
	struct ac_chl_pkg_t *chl_pkg = (struct ac_chl_pkg_t *)recvbuf;
	uint32_t rcvlen;

	for (; count < max_count; count++) {
		rcvlen = sizeof(recvbuf);
		if (get_from_channel(chl, &rcvlen, recvbuf, REQUEST_CHANNEL) == -1) {
			ERROR_LOG("Failed to get_from_channel");
			return -1;
		}

		if (rcvlen == 0) {
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
		//svr_pkg->ver = ntohl(svr_pkg->ver);
		//svr_pkg->cmd = ntohl(svr_pkg->cmd);
		//svr_pkg->uid = ntohl(svr_pkg->uid);
		//svr_pkg->seqno = ntohl(svr_pkg->seqno);
		if (svr_pkg->len != rcv_svr_pkg_len
			|| svr_pkg->len > MAX_PKG_LEN) {
			ERROR_LOG("invalid svr_pkg: svr_pkg->len(%u) != rcv_svr_pkg_len(%u) "
					"or svr_pkg->len > MAX_PKG_LEN(%u)",
					svr_pkg->len, rcv_svr_pkg_len, MAX_PKG_LEN);
			return -1;
		}

		uint32_t bodylen = svr_pkg->len - sizeof(struct svr_pkg_t);
		proc_conn_pkg(svr_pkg, bodylen);
	}

	return 0;
}

void tmr_worker(evutil_socket_t pad, short events, void *user_data)
{
	recv_interface();
}

void add_worker_events(struct event_base *base)
{
    struct timeval tmr_tv;
	tmr_tv.tv_sec = 0;
	tmr_tv.tv_usec = 100; /* 设置成一个很小的间隔, 使得每次主循环都能执行它 */
	tmrev_worker = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, tmr_worker, NULL);
	/* 更新系统时钟: 最高优先级: 保证在循环的一开始执行 */
	event_priority_set(tmrev_worker, 1);
	event_add(tmrev_worker, &tmr_tv);
}

/**
 * @brief 初始化 与 interface 之间的 channel
 * @return -1: failed, 0: success
 */
int init_dbagent_channels(void)
{
	/* TODO: config-shm related;
	 * (1) 不区分业务, 所以直接用 SHM_KEY_DB_AGENT_CHANNEL;
	 */
	key_t key = SHM_KEY_DB_INTERFACE_CHANNEL;
	init_shm_mgr(dbagent_chl_mgr, key);
	if (get_channel_shm(dbagent_chl_mgr, create_noexist_yes) == -1) {
		ERROR_LOG("Failed to get_channel_shm for db_agent");
		return -1;
	}

	return 0;
}

int init_db_connection(void)
{
	struct dbagt_config_t *cfg = dbagt_cfg;
	struct dbconn_t *dbconn = &(dbmgr->dbconn);

	my_init_db(dbmgr);

	snprintf(dbconn->host, sizeof(dbconn->host), "%s", cfg->my_host);
	dbconn->port = cfg->my_port;
	sprintf(dbconn->user, "%s", cfg->my_user);
	sprintf(dbconn->passwd, "%s", cfg->my_passwd);
	sprintf(dbconn->db, "%s", cfg->my_dbname);
	if (!dbmgr->conned && my_connect_db(dbmgr) == -1) {
		ERROR_LOG("Cannot connect to db, err(%d): %s",
				mysql_errno(&dbmgr->my), mysql_error(&dbmgr->my));
		return -1;
	}

	return 0;
}

int init_dbagt_config(char *conf_path, struct dbagt_config_t *config)
{
	int ret;

	cfg_opt_t opts[] = {
		CFG_BOOL("background", cfg_false, CFGF_NONE),
		CFG_BOOL("debug_mode", cfg_false, CFGF_NONE),

		CFG_STR("my_host", "localhost", CFGF_NONE),
		CFG_INT("my_port", 3306, CFGF_NONE),

		CFG_STR("my_user", "root", CFGF_NONE),
		CFG_STR("my_passwd", "ta0mee", CFGF_NONE),
		CFG_STR("my_dbname", "anticheat", CFGF_NONE),
		CFG_STR("my_tab_basename", "today_anticheat", CFGF_NONE),

		CFG_STR("log_dir", "./log", CFGF_NONE),
		CFG_INT("max_log_lvl", log_lvl_debug, CFGF_NONE),
		CFG_INT("max_log_size", MAX_LOG_SIZE, CFGF_NONE),
		CFG_INT("max_log_file", 0, CFGF_NONE),
		CFG_STR("log_prefix", "dbagt_", CFGF_NONE),

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

	snprintf(config->my_host, sizeof(config->my_host),
			"%s", cfg_getstr(cfg, "my_host"));
	config->my_port = cfg_getint(cfg, "my_port");

	snprintf(config->my_user, sizeof(config->my_user),
			"%s", cfg_getstr(cfg, "my_user"));
	snprintf(config->my_passwd, sizeof(config->my_passwd),
			"%s", cfg_getstr(cfg, "my_passwd"));
	snprintf(config->my_dbname, sizeof(config->my_dbname),
			"%s", cfg_getstr(cfg, "my_dbname"));
	snprintf(config->my_tab_basename, sizeof(config->my_tab_basename),
			"%s", cfg_getstr(cfg, "my_tab_basename"));

	snprintf(config->log_dir, sizeof(config->log_dir), "%s", cfg_getstr(cfg, "log_dir"));
	config->max_log_lvl = cfg_getint(cfg, "max_log_lvl");
	config->max_log_size = cfg_getint(cfg, "max_log_size");
	config->max_log_file = cfg_getint(cfg, "max_log_file");
	snprintf(config->log_prefix, sizeof(config->log_prefix), "%s", cfg_getstr(cfg, "log_prefix"));

	cfg_free(cfg);

	return 0;
}

int init_system(int argc, char **argv)
{
	int ret;
	char *conf_path = (argc == 2) ? argv[1] : NULL;
	ret = init_dbagt_config(conf_path, dbagt_cfg);
	if (ret == -1) {
		BOOT_LOG(-1, "Failed to init_dbagt_config");
	}

	ret = log_init(dbagt_cfg->log_dir, dbagt_cfg->max_log_lvl,
			dbagt_cfg->max_log_size, dbagt_cfg->max_log_file, dbagt_cfg->log_prefix);
	if (ret == -1) {
		BOOT_LOG(-1, "Failed to log_init");
	}

	if (init_dbagent_channels() == -1) {
		BOOT_LOG(-1, "Failed to init_dbagent_channels");
		return -1;
	}

	set_rlimit(DEFAULT_MAX_OPEN_FILES);
	return 0;
}

int main(int argc, char **argv)
{
	struct event_base *base;

	sys_clock = time(NULL);

	if (init_system(argc, argv) == -1) {
		BOOT_LOG(-1, "Failed to init_system");
	}

	init_daemon(dbagt_cfg->background, dbagt_cfg->debug_mode);

	if (init_db_connection() == -1) {
		BOOT_LOG(-1, "Failed to init_db_connection");
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
