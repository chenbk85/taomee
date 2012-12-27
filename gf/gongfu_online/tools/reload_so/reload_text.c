#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static int reload_mcast_fd;
static struct sockaddr_in reload_mcast_addr;
static int   reload_mcast_port = 8888;
static char group_nbr[32];
static char bind_ip[32];
static char infa[16];

typedef struct args {
	char        svr_name[16];
	uint32_t    svr_id;
	char        new_so_name[32];
}__attribute__((packed))args_t;

typedef struct msg_header {
	uint16_t        pkg_type;   // for mcast_notify_addr: 1st, syn
	uint16_t        proto_type; // mcast_notify_addr 0, mcast_reload_text 1
	char            body[];
}__attribute__((packed))msg_header_t;

typedef struct msg_body {
	char        svr_name[16];
	uint32_t    svr_id;
	char        new_so_name[32];
}__attribute__((packed))msg_body_t;

static args_t arg_list;
static uint8_t msg_reload[4096];
static const char *opt_string = "ad:h:m:g:b:p:i:s:";

static int connect_to_broadcaster()
{
	reload_mcast_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (reload_mcast_fd == -1) {
		printf("Failed to Create `reload_mcast_fd`: err=%d %s\n", errno, strerror(errno));
		return -1;
	}
	memset(&reload_mcast_addr, 0, sizeof(reload_mcast_addr));
	reload_mcast_addr.sin_family = AF_INET;
	inet_pton(AF_INET, group_nbr, &(reload_mcast_addr.sin_addr));
	reload_mcast_addr.sin_port = htons(reload_mcast_port);
									
	int on = 1;
	setsockopt(reload_mcast_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
	in_addr_t ipaddr;
	inet_pton(AF_INET, bind_ip, &ipaddr);
	if (setsockopt(reload_mcast_fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {
		printf("Failed to Set Outgoing Interface: err=%d %s %s\n",
			errno, strerror(errno), bind_ip);
		return -1;
	}

	if (bind(reload_mcast_fd, (struct sockaddr*)&reload_mcast_addr, sizeof reload_mcast_addr) == -1) {
		printf("Failed to Bind `reload_mcast_fd`: err=%d %s\n", errno, strerror(errno));
		return -1;
	}

	struct group_req req;
	req.gr_interface = if_nametoindex(infa);
	memcpy(&req.gr_group, &reload_mcast_addr, sizeof reload_mcast_addr);
	if (setsockopt(reload_mcast_fd, IPPROTO_IP, MCAST_JOIN_GROUP, &req, sizeof req) == -1) {
		printf("Failed to Join Mcast Grp: err=%d %s\n", errno, strerror(errno));
		return -1;
	}

	return 0;
}


void display_usage()
{
	printf("-m  svr_name(\"seer.online\"... etc.)\n");
    printf("-a  send command to all svr_ids under svr_name\n");
    printf("-d  send command to specified svr_id under svr_name\n");
	printf("-s  new text so name to be reloaded\n");
    printf("-g  mcast group ip(default to 239.0.0.7)\n");
	printf("-p  mcast group port(default to 8888)\n");
    printf("-b  socket ip to bind\n");
    printf("-i  bind interface(\"default eth0\"... etc.)\n");
    printf("-h  for help\n");
}

int main(int argc, char* argv[])
{
	//default value if not specified
    arg_list.svr_id= -1;
	strcpy(infa, "eth0");
	strcpy(group_nbr, "239.0.0.7");
	strcpy(arg_list.new_so_name, "./bin/libkf.so");

    int i = 0;
    int oc ;
    char * b_opt_arg;
    while( (oc = getopt(argc, argv, opt_string)) != -1) {
		switch( oc ) {
            case 'a':
                arg_list.svr_id = 0;
                break;
            case 'd':
                b_opt_arg = optarg;
                arg_list.svr_id = atoi(b_opt_arg);
                break;
			case 'm':
                b_opt_arg = optarg;
                strcpy(arg_list.svr_name, b_opt_arg);
                break;
			case 's':
                b_opt_arg = optarg;
                strcpy(arg_list.new_so_name, b_opt_arg);
                break;
			case 'g':
                b_opt_arg = optarg;
                strcpy(group_nbr, b_opt_arg);
                break;
			case 'b':
                b_opt_arg = optarg;
                strcpy(bind_ip, b_opt_arg);
                break;
			case 'p':
                b_opt_arg = optarg;
                reload_mcast_port = strtol(b_opt_arg, NULL, 10);
                break;
			case 'i':
                b_opt_arg = optarg;
                strcpy(infa, b_opt_arg);
                break;
            case 'h':
                display_usage();
                break;
            default:
                break;
        }
    }
    if(arg_list.svr_id < 0) {
        display_usage();
        return 0;
    }
	if (group_nbr[0] == 0 || bind_ip[0] == 0 || infa[0] == 0 || reload_mcast_port == 0) {
        display_usage();
        return 0;
	}

	printf("svr name:\t\t%s\n", arg_list.svr_name);
	printf("svr id:\t\t%d\n", arg_list.svr_id);
	printf("new so name:\t\t%s\n", arg_list.new_so_name);
	printf("mcast ip:\t\t%s\n", group_nbr);
	printf("mcast port:\t\t%d\n", reload_mcast_port);
	printf("bind ip:\t\t%s\n", bind_ip);
	printf("output interface:\t\t%s\n", infa);

	int ret = connect_to_broadcaster();
	msg_header_t* pkg = (msg_header_t*)msg_reload;
    pkg->pkg_type = 0; /* NA in reload text mcast */
    pkg->proto_type = 1; /* mcast_reload_text */
	msg_body_t* body = (msg_body_t*)(pkg->body);
	snprintf(body->svr_name, 16, "%s", arg_list.svr_name);
	body->svr_id = arg_list.svr_id;
	snprintf(body->new_so_name, 32, "%s", arg_list.new_so_name);

	return sendto(reload_mcast_fd, &msg_reload, sizeof(msg_header_t) + sizeof(msg_body_t), 0, (void*)&reload_mcast_addr, sizeof reload_mcast_addr);
}

