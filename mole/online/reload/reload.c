
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
static const int   reload_mcast_port = 5538;

typedef struct CommunicatorBody {
	uint64_t mapid;
	int	opid;
	char body[];
} __attribute__((packed)) communicator_body_t;

// For Broadcaster
typedef struct CommunicatorHeader {
	int	len;
 	int	online_id;
 	short	cmd;
 	int	ret;
 	int	id;
 	uint64_t mapid;
	int	opid;
	char body[];
} __attribute__((packed)) communicator_header_t;

static int connect_to_broadcaster()
{
	reload_mcast_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (reload_mcast_fd == -1) {
		printf("Failed to Create `reload_mcast_fd`: err=%d %s\n", errno, strerror(errno));
		return -1;
	}
	memset(&reload_mcast_addr, 0, sizeof(reload_mcast_addr));
	reload_mcast_addr.sin_family = AF_INET;
	inet_pton(AF_INET, "239.0.0.1", &(reload_mcast_addr.sin_addr));
	reload_mcast_addr.sin_port = htons(reload_mcast_port);
									
	int on = 1;
	setsockopt(reload_mcast_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
	in_addr_t ipaddr;
	inet_pton(AF_INET, "192.168.1.27", &ipaddr);
	//inet_pton(AF_INET, "10.1.1.5", &ipaddr);
	if (setsockopt(reload_mcast_fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {
		printf("Failed to Set Outgoing Interface: err=%d %s %s\n",
			errno, strerror(errno), "192.168.1.27");
		return -1;
	}

	if (bind(reload_mcast_fd, (struct sockaddr*)&reload_mcast_addr, sizeof reload_mcast_addr) == -1) {
		printf("Failed to Bind `reload_mcast_fd`: err=%d %s\n", errno, strerror(errno));
		return -1;
	}

	struct group_req req;
	req.gr_interface = if_nametoindex("eth1");
	//req.gr_interface = if_nametoindex("eth0");
	memcpy(&req.gr_group, &reload_mcast_addr, sizeof reload_mcast_addr);
	if (setsockopt(reload_mcast_fd, IPPROTO_IP, MCAST_JOIN_GROUP, &req, sizeof req) == -1) {
		printf("Failed to Join Mcast Grp: err=%d %s\n", errno, strerror(errno));
		return -1;
	}

	return 0;
}


int send_to_broadcaster(int cmd, int body_len, const void* body_buf, int id, uint64_t mapid, int opid)
{
	static char bcpkg[1024];

	int len = sizeof(communicator_header_t) + body_len;
	if ((body_len > sizeof(bcpkg) - sizeof(communicator_header_t)) ) {
			printf("Failed to Send Mcast Pkg, fd=%d\n", reload_mcast_fd);
			return -1;
	}

	communicator_header_t* pkg = (void*)bcpkg;
	pkg->len        = len;
	pkg->online_id  = 0xFFFFFF;
	pkg->cmd        = cmd;
	pkg->ret        = 0;
	pkg->id         = id;
	pkg->mapid      = mapid;
	pkg->opid       = opid;
	memcpy(pkg->body, body_buf, body_len);

	return sendto(reload_mcast_fd, bcpkg, len, 0, (void*)&reload_mcast_addr, sizeof reload_mcast_addr);
}

struct args_t {
    int online_id;
    int cmd_id;
    int value;
}arg_list;
struct msg_t{
    int online_id;
    int value;
}msg_reload;
static const char *opt_string = "ad:c:hv:";
void display_usage()
{
    printf("-a  send command to all hosts\n");
    printf("-d  send command to online id specified by this arg\n");
    printf("-c  define the command\n");
    printf("-v  command's value\n");
    printf("    1,  update items\n");
    printf("    2,  update games\n");
    printf("    3,  update questions\n");
	printf("    4,  update gifts\n");
	printf("    5,  update game dbproxy ip\n");
	printf("    6,  update pic svr ip\n");
	printf("    7,  update tuya svr ip\n");
	printf("    8,  set db timeout; value(1),turn off;value(2),turn on\n");
	printf("    9,  update holidays\n");
	printf("   10,  set homesvr switch; value(1),turn off;value(2),turn on\n");
	printf("   11,  update dirty filter\n");
	printf("   12,  update rand_type\n");
	printf("   13,  update pro_wrok\n");
	printf("   14,  update candy\n");
	printf("   15,  update cards\n");
	printf("   16,  update exchange\n");
	printf("   17,  update exclu_things\n");
	printf("   18,  update tasks\n");
	printf("   19,  update npc tasks\n");
	printf("   20,  update swap\n");
	printf("   21,  update rand\n");
	printf("   22,  update game bonus\n");
    printf("-h  display this message\n");
}
static char *cmd_txt[] = {
    "update items",
    "update games",
    "update questions",
    "update gifts",
    "update game dbproxy ip",
    "update pic svr ip",
    "update tuya svr ip",
    "set db timeout",
    "update holidays",
    "set homesvr switch",
	"update dirty filter",
	"update rand_type",
	"update pro_wrok",
	"update candy",
	"update cards",
	"update exchange",
	"update exclu_things",
	"update tasks",
	"update npc tasks",
	"update swap",
	"update rand",
	"update game bonus",
};

int cmd_num[] = {
    62001,
    62002,
    62003,
    62004,
    62005,
    62006,
    62007,
    62008,
    62009,
    62010,
    62011,
    62012,
    62013,
    62014,
    62015,
    62016,
    62017,
    62018,
    62019,
    62020,
    62021,
    62022
};

int main(int argc, char* argv[])
{
    arg_list.online_id= -1;
    arg_list.cmd_id= -1;
    arg_list.value = -1;
    int i = 0;
    int oc ;
    char * b_opt_arg;
    while( (oc = getopt(argc, argv, opt_string)) != -1) {
        switch( oc ) {
            case 'a':
                arg_list.online_id = 0;
                break;
            case 'd':
                b_opt_arg = optarg;
                arg_list.online_id = atoi(b_opt_arg);
                break;
            case 'c':
                b_opt_arg = optarg;
                arg_list.cmd_id= atoi(b_opt_arg) - 1;
                break;
            case 'v':
                b_opt_arg = optarg;
                arg_list.value = atoi(b_opt_arg);
                break;
            case 'h':
                display_usage();
                break;
            default:
                break;
        }
    }
    if (arg_list.cmd_id >= (sizeof(cmd_num)/sizeof(int)) ||arg_list.cmd_id < 0 || arg_list.online_id ==  -1)
    {
        display_usage();
        return 0;
    }
    if((arg_list.cmd_id == 7 || arg_list.cmd_id == 9) && arg_list.value != 1 && arg_list.value != 2) {
        display_usage();
        return 0;
    }

    if(arg_list.online_id) {
        printf("Send cmd [%s] to %d\n",cmd_txt[arg_list.cmd_id], arg_list.online_id);
    } else {
        printf("Send cmd [%s] to all online\n",cmd_txt[arg_list.cmd_id]);
    }

	int ret = connect_to_broadcaster();
    msg_reload.online_id = arg_list.online_id;
    msg_reload.value = arg_list.value;
	ret = send_to_broadcaster(62000, sizeof(msg_reload), &msg_reload, 0, 0, cmd_num[arg_list.cmd_id]);
	return ret;
}
