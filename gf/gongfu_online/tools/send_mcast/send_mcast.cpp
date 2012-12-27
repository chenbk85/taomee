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
#include "send_mcast.hpp"

#define	MAX_SVR_CNT 100

static char mcast_ip[16];
//static char out_mcast_ip[] = "239.0.0.241";
//static char in_mcast_ip[] = "239.0.0.241";
//static uint16_t mcast_port = 8889;
static char out_mcast_ip[16] = {0};
static uint16_t mcast_port = 0;
static char mcast_if[10] = {0};

struct mcast_info_t {
	int mcast_fd;
	struct sockaddr_in mcast_addr;
} mcast_info;

#pragma pack(1)

struct mcast_pkg_header {
	uint16_t	pkg_type;   // for mcast_notify_addr: 1st, syn
	uint16_t	proto_type; // mcast_notify_addr, mcast_reload_text
	char		body[];
}mcast_pkg_header_t;

struct addr_mcast_pkg {
	uint32_t	svr_id;
	char		name[16];
	char		ip[16];
	uint16_t	port;
}addr_mcast_pkg_t;

#pragma pack()

static int 	svr_cnt;
static struct addr_mcast_pkg mcasts[MAX_SVR_CNT];

static char addr_buf[sizeof(struct mcast_pkg_header) + sizeof(struct addr_mcast_pkg)];

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
	memset(mcast_ip, 0, sizeof(mcast_ip));
	memcpy(mcast_ip, out_mcast_ip, sizeof(out_mcast_ip));
	return 0;
}

void send_mcast_pkg(struct addr_mcast_pkg* p_addr)
{
	memset(addr_buf, 0, sizeof(addr_buf));
	struct mcast_pkg_header* pkg = reinterpret_cast<struct mcast_pkg_header*>(addr_buf);
	pkg->pkg_type = 1;
	pkg->proto_type = 0;

	struct addr_mcast_pkg* pkg_body = reinterpret_cast<struct addr_mcast_pkg*>(pkg->body);
	pkg_body->svr_id = p_addr->svr_id;
	pkg_body->port = p_addr->port;
	strcpy(pkg_body->name, p_addr->name);
	strcpy(pkg_body->ip, p_addr->ip);
	//printf("addr[%u %u %u]:[%u %u %s %s]\n", sizeof(addr_buf), sizeof(struct mcast_pkg_header), sizeof(struct addr_mcast_pkg), pkg_body->svr_id, pkg_body->port, pkg_body->name, pkg_body->ip);

	struct sockaddr_in* addr = &(mcast_info.mcast_addr);
	int mcast_fd = mcast_info.mcast_fd;
	//printf("mcast fd:[%u]\n", mcast_fd);
	int err = sendto(mcast_fd, addr_buf, sizeof(addr_buf), 0, reinterpret_cast<sockaddr*>(addr), sizeof(*addr));
	if (err == -1) {
		perror("send faild!");
	}
}

int make_mcast_sock()
{
	mcast_info.mcast_fd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in* mcast_addr = &(mcast_info.mcast_addr);
	//mcast_set_if(mcast_info.mcast_fd, AF_INET, "eth0");
	mcast_set_if(mcast_info.mcast_fd, AF_INET, mcast_if);
	memset(mcast_addr, 0, sizeof(*mcast_addr));
	if (make_sockaddr(mcast_ip, mcast_port, mcast_addr) < 0) {
		printf("invalid mcast addr: ip=\"%s\", port=%u\n",mcast_ip, mcast_port);
		fprintf(stderr, "invalid mcast addr: ip=\"%s\", port=%u\n",
				mcast_ip, mcast_port);
		return -1;
	}
	//printf("mcast addr: ip=\"%s\", port=%u\n",mcast_ip, mcast_port);
	return mcast_info.mcast_fd;
}

void send_mcasts_addr()
{
	struct addr_mcast_pkg mcast_addr;
	for (int i = 0; i < svr_cnt; i++) {
		memset(&mcast_addr, 0, sizeof(struct addr_mcast_pkg));
		mcast_addr.svr_id = mcasts[i].svr_id;
		mcast_addr.port = mcasts[i].port;
		strcpy(mcast_addr.name, mcasts[i].name);
		strcpy(mcast_addr.ip, mcasts[i].ip);
		send_mcast_pkg(&mcast_addr);
	}
}

int load_mcasts_conf()
{
	svr_cnt = 0;
	FILE *fp = NULL;
	fp = fopen("./mcasts.conf", "r");
	if (!fp) {
		printf("open conf error!\n");
		return -1;
	}
	
	while (fscanf(fp, "%s\t%d\t%s\t%d\n",mcasts[svr_cnt].name, &(mcasts[svr_cnt].svr_id), 
				mcasts[svr_cnt].ip, &(mcasts[svr_cnt].port)) != EOF) {
		printf("svr[%d]:%s %d %s %d\n", svr_cnt+1, mcasts[svr_cnt].name, mcasts[svr_cnt].svr_id, 
				mcasts[svr_cnt].ip, mcasts[svr_cnt].port);
		svr_cnt++;
		if (svr_cnt >= MAX_SVR_CNT) {
			break;
		}
	}
	fclose(fp);

	FILE *fp1 = NULL;
	fp1 = fopen("./bench.conf", "r");
	if (!fp1) {
		printf("open conf error!\n");
		return -1;
	}
	char arg1[128], arg2[128];
	while (fscanf(fp1, "%s\t%s\n",arg1,arg2) != EOF) {
		printf("bench:%s %s\n", arg1, arg2);
		if (strcmp(arg1,"mcast_ip")==0) {
			strcpy(out_mcast_ip, arg2);
		} else if (strcmp(arg1,"mcast_port")==0) {
			mcast_port = atoi(arg2);
		} else if (strcmp(arg1,"mcast_if")==0) {
			strcpy(mcast_if, arg2);
		}
	}
	fclose(fp1);

	return 0;
}

int main(int argc, char* argv[])
{
	if (load_mcasts_conf() == -1) {
		exit(EXIT_FAILURE);
	}

	if (init_mcast_ip() < 0) {
		fprintf(stderr, "cannot determine is inner or outer\n");
		exit(EXIT_FAILURE);
	}

	memset(&mcast_info, 0, sizeof(mcast_info));
	if (make_mcast_sock() < 0)
		exit(EXIT_FAILURE);

	while (1) {
		send_mcasts_addr();
		sleep(20);
		//break;
	}

	close(mcast_info.mcast_fd);
	exit(EXIT_SUCCESS);
}
