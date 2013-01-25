#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>


#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/event.h>

#include "common.h"
#include "struct.h"
#include "shm_keys.h"
#include "shm.h"
#include "channel.h"
#include "util.h"
#include "log.h"
#include "daemon.h"

#include "db_interface.h"

time_t sys_clock;

int interface_id;
struct dbif_config_t dbif_config, *dbif_cfg = &dbif_config;

struct event *read_event;
struct event *write_event;
struct event *tmrev_renew_sys_clock;
struct event *tmrev_statistics;
struct event *quit_signal_event;
struct event *tmrev_chk_resp_chl;

struct shm_mgr_t db_interface_channel_shm_mgr, *dbif_chl_mgr = &db_interface_channel_shm_mgr;

char recvbuf[RCV_BUF_LEN];
char sendbuf[SND_BUF_LEN];

int succ, fail; /* TODO: DELETE ME */


int is_writable = 0;

void enable_write_event(void)
{
	if (is_writable == 0) {
		event_add(write_event, NULL);
		is_writable = 1;
	}
}

void disable_write_event(void)
{
	if (is_writable == 1) {
		event_del(write_event);
		is_writable = 0;
	}
}

/**
 *@brief 创建一个 udp 的 socket, 并 bind 到给定的 addr 上, 其中 addr 支持的格式有:
 * [ipv6]:port
 * ipv6
 * [ipv6]
 * ipv4:port
 * ipv4
 *
 * 注意: 调用者要保证 addr 不为 NULL;
 *
 * @return -1: failed, 有效fd: success;
 */
int create_udp_socket(const char *addr)
{
	int sock;
    struct sockaddr_in sin;
	int socklen = sizeof(sin);

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		return -1;
	}
	set_socket_nonblock(sock);
	set_socket_reuseaddr(sock);
	set_socket_rwmem(sock, SET_SOCK_RWMEM_SIZE);

	if (evutil_parse_sockaddr_port(addr, (struct sockaddr *)&sin, &socklen) < 0) {
		ERROR_LOG("Invalid addr: %s", addr);
		return -1;
	}

	if (bind(sock, (const struct sockaddr *)&sin, sizeof(sin)) == -1) {
		ERROR_LOG("failed to bind to addr: %s, err(%d):%s",
				addr, errno, strerror(errno));
		close(sock);
		return -1;
	}

	return sock;
}

/**
 * @brief 初始化 interface 与业务之间的 shm-channel
 * @return -1: failed, 0: success
 */
int init_db_interface_channels(void)
{
	/* TODO: config-shm related;
	 */
	key_t key = SHM_KEY_DB_INTERFACE_CHANNEL;
	init_shm_mgr(dbif_chl_mgr, key);
	if (get_channel_shm(dbif_chl_mgr, create_noexist_yes) == -1) {
		ERROR_LOG("Failed to get_channel_shm for key: %#x", key);
		return -1;
	}
	init_channel_by_shm_mgr(dbif_chl_mgr);

	return 0;
}

int init_dbif_config(char *conf_path, struct dbif_config_t *config)
{
	int ret;

	cfg_opt_t opts[] = {
		CFG_BOOL("background", cfg_false, CFGF_NONE),
		CFG_BOOL("debug_mode", cfg_false, CFGF_NONE),

		CFG_STR("bind_ethx", "eth1", CFGF_NONE),
		CFG_INT("bind_port", 0, CFGF_NONE),

		CFG_STR("log_dir", "./log", CFGF_NONE),
		CFG_INT("max_log_lvl", log_lvl_debug, CFGF_NONE),
		CFG_INT("max_log_size", MAX_LOG_SIZE, CFGF_NONE),
		CFG_INT("max_log_file", 0, CFGF_NONE),
		CFG_STR("log_prefix", "dbif_", CFGF_NONE),

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
	snprintf(config->bind_ethx, sizeof(config->bind_ethx), "%s", cfg_getstr(cfg, "bind_ethx"));
	config->bind_port = cfg_getint(cfg, "bind_port");

	snprintf(config->log_dir, sizeof(config->log_dir), "%s", cfg_getstr(cfg, "log_dir"));
	config->max_log_lvl = cfg_getint(cfg, "max_log_lvl");
	config->max_log_size = cfg_getint(cfg, "max_log_size");
	config->max_log_file = cfg_getint(cfg, "max_log_file");
	snprintf(config->log_prefix, sizeof(config->log_prefix), "%s", cfg_getstr(cfg, "log_prefix"));

	if (get_local_eth_ipstr(config->bind_ethx, config->bind_ip) == -1) {
		BOOT_LOG(-1, "Cannot get local ip for: %s", config->bind_ethx);
	}
	snprintf(config->dbif_addr, sizeof(config->dbif_addr), "%s:%hu",
			config->bind_ip, config->bind_port);

	cfg_free(cfg);

	return 0;
}

int init_system(int argc, char **argv)
{
	int ret;
	char *conf_path = (argc == 2) ? argv[1] : NULL;
	ret = init_dbif_config(conf_path, dbif_cfg);
	if (ret == -1) {
		BOOT_LOG(-1, "Failed to init_dbif_config");
	}

	ret = log_init(dbif_cfg->log_dir, dbif_cfg->max_log_lvl,
			dbif_cfg->max_log_size, dbif_cfg->max_log_file, dbif_cfg->log_prefix);
	if (ret == -1) {
		BOOT_LOG(-1, "Failed to log_init");
	}

	if (init_db_interface_channels() == -1) {
		BOOT_LOG(-1, "Failed to init_db_interface_channels");
	}

	set_rlimit(DEFAULT_MAX_OPEN_FILES);

	return 0;
}

void recv_req_from_conn(evutil_socket_t fd, short what, void *arg)
{
	ssize_t rcvlen;
	uint32_t svr_pkg_len;
	struct ac_chl_pkg_t *chl_pkg = (struct ac_chl_pkg_t *)recvbuf;
	struct svr_pkg_t *svr_pkg = &(chl_pkg->svr_pkg);
	size_t rcv_size = sizeof(recvbuf) - sizeof(struct ac_chl_pkg_t);
	struct sockaddr_in recv_from;
	socklen_t socklen;

	while (1) {
		rcvlen = recvfrom(fd, (char *)svr_pkg, rcv_size,
				0, (struct sockaddr *)&recv_from, &socklen);
		if (rcvlen == -1) {
			if (!SOCK_ERR_RW_RETRIABLE(errno)) {
				ERROR_LOG("Failed to recvfrom, err(%d): %s", errno, strerror(errno));
				fail++;
			}
			return;
		}
		if (rcvlen < sizeof(struct svr_pkg_t)) {
			/* recvbuf broken */
			ERROR_LOG("recvlen(%zd) less than sizeof(svr_pkg_t)(%zd)",
					rcvlen, sizeof(struct svr_pkg_t));
			fail++;
			return;
		}

		svr_pkg_len = ntohl(svr_pkg->len);
		if (rcvlen != svr_pkg_len) {
			/* invalid svr_pkg */
			ERROR_LOG("Invalid svr_pkg, recvlen(%zd) != svr_pkg->len(%u)",
					rcvlen, svr_pkg_len);
			fail++;
			return;
		}

		chl_pkg->head.len = svr_pkg_len + sizeof(struct ac_chl_head_t);
		memcpy(&chl_pkg->head.recv_from, &recv_from, sizeof(chl_pkg->head.recv_from));

		/* handle one client_pkg */
		/* 由于只有一条逻辑通道, 因此所有命令都直接塞到channel中;
		 * TODO: 支持多个通道, 需要根据配置的命令号分别装入不同的channel中;
		 * TODO: 而分拣的命令号, 可根据配置而定, 也可以根据一定的区间确定; */
		add_to_channel_shm_mgr(dbif_chl_mgr, chl_pkg->head.len, (char *)chl_pkg, REQUEST_CHANNEL);

		//enable_write_event();

		succ++;
	}
}

void send_resp_to_conn(evutil_socket_t fd, short what, void *arg)
{
#if 0
	int ret;
	socklen_t socklen;
	uint32_t rpkglen = sizeof(recvbuf);
	struct relay_pkg_t *rpkg = (struct relay_pkg_t *)recvbuf;
	struct channel_t *chl = (struct channel_t *)(dbif_chl_mgr->shm);

	ret = get_svr_rpkg_from_channel(chl, rpkg, rpkglen, RESPONSE_CHANNEL);
	if (ret < 0) {
		ERROR_LOG("Failed to get_rpkg_from_channel(resp)");
		return;
	}

	if (ret == 0) {
#if 1
		disable_write_event();
#endif
		return;
	}

	uint32_t pkglen = rpkg->head.len - sizeof(rpkg->head);
	socklen = sizeof(rpkg->head.cli_addr);
	ret = sendto(fd, (const void *)(&rpkg->body), pkglen, 0,
			(const struct sockaddr *)&(rpkg->head.cli_addr), socklen);

	if (ret == -1) {
		if (SOCK_ERR_RW_RETRIABLE(errno)) {
			disable_write_event();
		} else {
			char ipbuf[INET_ADDRSTRLEN] = { 0 };
			inet_ntop(AF_INET, &(rpkg->head.cli_addr.sin_addr),
					ipbuf, INET_ADDRSTRLEN);
			ERROR_LOG("Failed sendto: pkglen=%u, conn: %s:%hu, err(%d): %s",
					pkglen, ipbuf, ntohs(rpkg->head.cli_addr.sin_port), errno, strerror(errno));
		}
	}
#endif
}

#if 0
void fork_childs(int child_num)
{
	int count;
	for(count = 0; count < child_num; count++) {
		if (fork() == 0) {
			interface_id = count + 1;
			return;
		}
	}

	interface_id = 0;
}
#endif

static void quit_signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = user_data;
	struct timeval delay = { 0, 400000 };
	DEBUG_LOG("Done, interface=%d, succ=%d, fail=%d", interface_id, succ, fail);
	event_base_loopexit(base, &delay);
}

void init_net_event(struct event_base *base, int fd)
{
	read_event = event_new(base, fd, EV_READ|EV_PERSIST, recv_req_from_conn, base);
	event_priority_set(read_event, 2);
	event_add(read_event, NULL);

	write_event = event_new(base, fd, EV_WRITE|EV_PERSIST, send_resp_to_conn, base);
	event_priority_set(write_event, 2);
}

void tmr_renew_sys_clock(evutil_socket_t pad, short events, void *user_data)
{
	sys_clock = time(NULL);
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
	DEBUG_LOG("Stat[%ld]: succ: %d, fail: %d", sys_clock, succ, fail);
}

int add_statistics_timer_event(struct event_base *base)
{
	struct timeval tmr_tv;

	tmr_tv.tv_sec = DBIF_STATISTICS_INTERVAL;
	tmr_tv.tv_usec = 0;
	tmrev_statistics = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, tmr_do_statistics, NULL);
	evtimer_add(tmrev_statistics, &tmr_tv);

	return 0;
}

void tmr_chk_resp_chl(evutil_socket_t pad, short events, void *user_data)
{
	struct channel_t *chl = (struct channel_t *)(dbif_chl_mgr->shm);
	if (!is_channel_empty(chl, RESPONSE_CHANNEL)) {
		enable_write_event();
	}
}

int add_chk_resp_chl_timer_event(struct event_base *base)
{
	struct timeval tmr_tv;

	/* 
	 * 3ms 检查一次 response channel, 这是安全的, 因为:
	 * 假设 interface 连接的逻辑进程 3ms内把10MB的 response channel 填满了,
	 * 则该 channel 此时的带宽为 10MB / 0.003 = 3333MBps = 26666Mbps
	 * 而早在 100Mbps 或 1000Mbps 甚至 10000Mbps 的时候, 网络就先达到瓶颈了;
	 */
	tmr_tv.tv_sec = 0;
	tmr_tv.tv_usec = 3000;
	tmrev_chk_resp_chl = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, tmr_chk_resp_chl, base);
	evtimer_add(tmrev_chk_resp_chl, &tmr_tv);
	
	return 0;
}

void add_timer_events(struct event_base *base)
{
	add_statistics_timer_event(base);
	//add_chk_resp_chl_timer_event(base);
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

int main(int argc, char **argv)
{
	int sock;
	struct event_base *base;

	sys_clock = time(NULL);

#if 0
	/* TODO: config */
	if (argc == 2) {
		fork_childs(atoi(argv[1]));
	} else {
		fork_childs(1);
	}
#endif

	if (init_system(argc, argv) == -1) {
		BOOT_LOG(-1, "Failed to init_system");
	}

	init_daemon(dbif_cfg->background, dbif_cfg->debug_mode);

	sock = create_udp_socket(dbif_cfg->dbif_addr);
	if (sock == -1) {
		BOOT_LOG(-1, "Failed to create_udp_socket, addr: %s", dbif_cfg->dbif_addr);
	}

	base = event_base_new();
	if (!base) {
		BOOT_LOG(-1, "Could not initialize libevent!");
		return -1;
	}
	event_base_priority_init(base, 3);

	init_net_event(base, sock);
	add_sys_clock_timer(base);
	add_timer_events(base);
	add_signal_events(base);

	/* main loop */
	event_base_dispatch(base);

	event_base_free(base);

	return 0;
}
