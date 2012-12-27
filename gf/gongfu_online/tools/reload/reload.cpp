#include <cerrno>
#include <cstdio>
#include <cstring>
#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/inet/mcast.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <arpa/inet.h>
}

#include "mcast_proto.hpp"
#include "reload_conf.hpp"
#include "reload.hpp"

static char proc_param[] = "[-v] [-h] [[-0...] [mcast / server_id(1, 2, ...)]]";
static char mcast_ip[16];
static char out_mcast_ip[] = "239.0.0.241";
static char in_mcast_ip[] = "239.0.0.241";
static uint16_t mcast_port = 8888;
static char cmd_str_buf[128];
static char sendbuf[1024];
static int	is_inner; /* 0: inner, 1: outer */

struct mcast_info_t {
	int mcast_fd;
	struct sockaddr_in mcast_addr;
} mcast_info;

static struct option long_options[] = {
	{"item", 1, 0, '0'},
	{"btl", 1, 0, '1'},
	{"daily", 1, 0, '2'},
	{"task", 1, 0, '3'},
	{"stage", 1, 0, '4'},
	{"swap_action", 1, 0, '5'},
	{"active data", 1, 0, '6'},
	{"restriction", 1, 0, '7'},
	{"bench", 1, 0, '8'},
	{"btl_svr", 1, 0, '9'},
	{"mon_bonus", 1, 0, 'a'},
	{"exchange", 1, 0, 'b'},
	{"evolve", 1, 0, 'c'},
	{"hero_tower", 1, 0, 'd'},
	{"nono", 1, 0, 'e'},
	{"team_pk", 1, 0, 'f'},
	{"help", 0, 0, 'h'},
	{"version", 0, 0, 'v'},
};

long get_target_id(char* id_str)
{
	char* endptr = 0;
	long val;

	errno = 0;
	val = strtol(id_str, &endptr, 10);

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		exit(EXIT_FAILURE);
	}

	if (endptr == id_str) {
		fprintf(stderr, "No digits were found\n");
		exit(EXIT_FAILURE);
	}

	/* something uncenssary in the id_str, which is not safe condition... */
	if (*endptr != '\0') {
		fprintf(stderr, "Further characters after number: %s\n", endptr);
		exit(EXIT_FAILURE);
	}

	/* val == 0 is not allowed when single reloading */
	if (!val) {
		fprintf(stderr, "target_id == 0 is not allowed when single reloading\n");
		exit(EXIT_FAILURE);
	}
	return val;
}

inline char* cmd_string(uint32_t cmd)
{
	memset(cmd_str_buf, 0, sizeof(cmd_str_buf));
	sprintf(cmd_str_buf, "reload_%s_config_cmd", long_options[cmd - 1].name);
	return cmd_str_buf;
}

int make_sockaddr(char* ip, uint16_t port, sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
	if (inet_pton(AF_INET, ip, &(addr->sin_addr)) != 1)
		return -1;
	if (port > 65535)
		return -1;
	addr->sin_port = htons(port);
	return 0;
}

void init_reload_pkg(mcast_pkg_t* pkg, uint32_t minor_cmd)
{
	pkg->main_cmd  = mcast_reload_conf;
	pkg->minor_cmd = minor_cmd;
	pkg->server_id = 0; /* mcast pkg_head id */
}

int init_mcast_ip(void)
{
	FILE *fp = NULL;
	/* try test evn */
	fp = fopen("../../bench.conf", "r");
	if (fp) {
		is_inner = 0;
		fclose(fp);
		memset(mcast_ip, 0, sizeof(mcast_ip));
		memcpy(mcast_ip, in_mcast_ip, sizeof(in_mcast_ip));
		printf("mcast_ip:\"%s\"\n", mcast_ip);
		return 0;
	}

	fp = fopen("../OnlineA/bench.conf", "r");
	if (fp) {
		is_inner = 1;
		fclose(fp);
		memset(mcast_ip, 0, sizeof(mcast_ip));
		memcpy(mcast_ip, out_mcast_ip, sizeof(out_mcast_ip));
		return 0;
	}

	fp = fopen("../homeA/bench.conf", "r");
	if (fp) {
		is_inner = 1;
		fclose(fp);
		memset(mcast_ip, 0, sizeof(mcast_ip));
		memcpy(mcast_ip, out_mcast_ip, sizeof(out_mcast_ip));
		return 0;
	}

	return -1;
}

void reload_config(char* id_str, char* mip, uint16_t port, uint32_t cmd)
{
	printf("reload_config\n");
	memset(sendbuf, 0, sizeof(sendbuf));
	mcast_pkg_t* pkg = reinterpret_cast<mcast_pkg_t *>(sendbuf);
	struct sockaddr_in* addr = &(mcast_info.mcast_addr);
	int mcast_fd = mcast_info.mcast_fd;

	init_reload_pkg(pkg, cmd);
	int idx = sizeof(mcast_pkg_t);

	uint32_t target_id = 0;
	if (strcmp(id_str, "mcast"))
		target_id = get_target_id(id_str);

	taomee::pack_h(sendbuf, target_id, idx);

	printf("mcast_ip:\"%s\", port=%u, cmd=%s, target_id=%u\n",
			mip, port, cmd_string(cmd), target_id);
	int err = sendto(mcast_fd, sendbuf, idx, 0, reinterpret_cast<sockaddr*>(addr), sizeof(*addr));
	if (err == -1) {
		perror("send faild!");
	}
}

int make_mcast_sock()
{
	mcast_info.mcast_fd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in* mcast_addr = &(mcast_info.mcast_addr);
	mcast_set_if(mcast_info.mcast_fd, AF_INET, "eth1");
	memset(mcast_addr, 0, sizeof(*mcast_addr));
	if (make_sockaddr(mcast_ip, mcast_port, mcast_addr) < 0) {
		printf("invalid mcast addr: ip=\"%s\", port=%u\n",mcast_ip, mcast_port);
		fprintf(stderr, "invalid mcast addr: ip=\"%s\", port=%u\n",
				mcast_ip, mcast_port);
		return -1;
	}
	printf("mcast addr: ip=\"%s\", port=%u\n",mcast_ip, mcast_port);
	return mcast_info.mcast_fd;
}

int main(int argc, char* argv[])
{
	int c;
	if (argc == 1) {
		print_desc();
		print_usage(argv, proc_param);
		print_ver();
		exit(EXIT_SUCCESS);
	}

	if (init_mcast_ip() < 0) {
		fprintf(stderr, "cannot determine is inner or outer\n");
		exit(EXIT_FAILURE);
	}

	memset(&mcast_info, 0, sizeof(mcast_info));
	if (make_mcast_sock() < 0)
		exit(EXIT_FAILURE);

	while (1) {
		int option_index = 0;

		c = getopt_long(argc, argv, "0:1:2:3:4:5:6:7:8:9:a:b:c:d:e:f:vh",
				long_options, &option_index);
		printf("c=[%c%d]\n",c,c);
		if (c == -1)
			break;

		switch (c) {
		case '1':
			reload_config(optarg, mcast_ip, mcast_port, reload_item_config_cmd);
			break;
		case '2':
			reload_config(optarg, mcast_ip, mcast_port, reload_battle_config_cmd);
			break;
		case '3':
			reload_config(optarg, mcast_ip, mcast_port, reload_daily_activity_cmd);
			break;
		case '4':
			reload_config(optarg, mcast_ip, mcast_port, reload_task_cmd);
			break;
		case '5':
			reload_config(optarg, mcast_ip, mcast_port, reload_active_stage_cmd);
			break;
		case '6':
			reload_config(optarg, mcast_ip, mcast_port, reload_swap_action_cmd);
			break;
		case '7':
			reload_config(optarg, mcast_ip, mcast_port, reload_active_data_cmd);
			break;
	/*  case '7':
			reload_config(optarg, mcast_ip, mcast_port, reload_restriction_config_cmd);
			break;
		case '8':
			reload_config(optarg, mcast_ip, mcast_port, reload_bench_config_cmd);
			break;
		case '9':
			reload_config(optarg, mcast_ip, mcast_port, reload_btl_svr_config_cmd);
			break;
		case 'a':
			reload_config(optarg, mcast_ip, mcast_port, reload_mon_bonus_config_cmd);
			break;
		case 'b':
			reload_config(optarg, mcast_ip, mcast_port, reload_exchange_config_cmd);
			break;
		case 'c':
			reload_config(optarg, mcast_ip, mcast_port, reload_cond_evolves_config_cmd);
			break;
		case 'd':
			reload_config(optarg, mcast_ip, mcast_port, reload_hero_tower_config_cmd);
			break;
		case 'e':
			reload_config(optarg, mcast_ip, mcast_port, reload_nono_config_cmd);
			break;
		case 'f':
			reload_config(optarg, mcast_ip, mcast_port, reload_team_pk_config_cmd);
			break;
		case 'h':
			print_usage(argv, proc_param);
			break;
		case 'v':
			print_ver();
			break;
		case '?':
			break;*/
		default: /* unsupported options */
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	close(mcast_info.mcast_fd);
	exit(EXIT_SUCCESS);
}
