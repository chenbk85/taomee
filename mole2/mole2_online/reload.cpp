
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
static int   reload_mcast_port = 5601;
static char group_nbr[32];
static char bind_ip[32];
static char infa[16];

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

typedef struct args {
    int online_id;
    int cmd_id;
    int type;
	char  module_name[32];
	char svr_name[16];
}__attribute__((packed))args_t;

typedef struct msg{
    short pkg_type;
    short proto_type;
    char  svr_name[16];
    int   online_id;
	char  module_name[32];
}__attribute__((packed))msg_t;

typedef struct conf_msg{
    short       online_id;
    uint32_t    type;
}__attribute__((packed))conf_msg_t;

args_t arg_list;
msg_t  msg_reload;
conf_msg_t conf_msg;
static const char *opt_string = "ad:c:h:s:m:t:g:b:p:i:";
void display_usage()
{
    printf("-a  send command to all hosts\n");
    printf("-d  send command to online id specified by this arg\n");
    printf("-c  define the command\n");
	printf("-m  module name\n");
	printf("-s  svr name\n");
	printf("-t  conf reload type\n");
    printf("-g  mcast ip\n");
    printf("-b  bind ip\n");
	printf("-p  mcast port\n");
    printf("-i  bind interface\n");
    printf("-h  for help\n");
}

int main(int argc, char* argv[])
{
    arg_list.online_id= -1;
    arg_list.cmd_id= -1;
    int i = 0;
    int oc ;
    char * b_opt_arg;
    while( (oc = getopt(argc, argv, opt_string)) != -1) {
        switch( oc ) {
            case 'a':
                arg_list.online_id = 0;
				printf("online id:\t\t%d\n", arg_list.online_id);
                break;
            case 'd':
                b_opt_arg = optarg;
                arg_list.online_id = atoi(b_opt_arg);
				printf("online id:\t\t%d\n", arg_list.online_id);
                break;
            case 'c':
                b_opt_arg = optarg;
                arg_list.cmd_id= atoi(b_opt_arg);
				printf("cmd id:\t\t\t%d\n", arg_list.cmd_id);
                break;
			case 'm':
                b_opt_arg = optarg;
                strcpy(arg_list.module_name, b_opt_arg);
				printf("module name:\t\t%s\n", arg_list.module_name);
                break;
			case 's':
				b_opt_arg = optarg;
				strcpy(arg_list.svr_name, b_opt_arg);
				printf("svr name:\t\t%s\n", arg_list.svr_name);
				break;
			case 't':
                b_opt_arg = optarg;
                arg_list.type = atoi(b_opt_arg);
				printf("conf type value:\t\t%d\n", arg_list.type);
                break;
			case 'g':
                b_opt_arg = optarg;
                strcpy(group_nbr, b_opt_arg);
				printf("mcast ip:\t\t%s\n", group_nbr);
                break;
			case 'b':
                b_opt_arg = optarg;
                strcpy(bind_ip, b_opt_arg);
				printf("bind ip:\t\t%s\n", bind_ip);
                break;
			case 'p':
                b_opt_arg = optarg;
                reload_mcast_port = strtol(b_opt_arg, NULL, 10);
				printf("mcast port:\t\t%d\n", reload_mcast_port);
                break;
			case 'i':
                b_opt_arg = optarg;
                strcpy(infa, b_opt_arg);
				printf("interface:\t\t%s\n", infa);
                break;
            case 'h':
                display_usage();
                break;
            default:
                break;
        }
    }
    if((arg_list.cmd_id != 1 && arg_list.cmd_id != 3) || arg_list.online_id < 0 || arg_list.online_id > 1200) {
        display_usage();
        return 0;
    }
	if (group_nbr[0] == 0 || bind_ip[0] == 0 || infa[0] == 0 || reload_mcast_port == 0) {
        display_usage();
        return 0;
	}

	int ret = connect_to_broadcaster();
    if (arg_list.cmd_id == 1) {
        msg_reload.pkg_type = 1;
        msg_reload.online_id = arg_list.online_id;
        msg_reload.proto_type = 1;
	    snprintf(msg_reload.svr_name, 16, "%s", arg_list.svr_name);
	    snprintf(msg_reload.module_name, 32, "%s", arg_list.module_name); 
	    return sendto(reload_mcast_fd, &msg_reload, sizeof(msg_reload), \
                0, (void*)&reload_mcast_addr, sizeof reload_mcast_addr);
    } else if (arg_list.cmd_id == 3) {
  		//conf_msg.pkg_type = 1;
		//conf_msg.proto_type = 1;
        conf_msg.online_id = arg_list.online_id;
        conf_msg.type = arg_list.type;
        printf("CONF RELOAD\t[%u %u]\n", conf_msg.online_id, conf_msg.type);
	    return sendto(reload_mcast_fd, &conf_msg, sizeof(conf_msg), \
                0, (void*)&reload_mcast_addr, sizeof reload_mcast_addr);
    }
}
