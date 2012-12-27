#include <errno.h>
#include <string.h>

#include <net/if.h>
#include <arpa/inet.h>

#include <glib.h>

#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/random/random.h>

#include "dll.h"
#include "net.h"

#include "mcast.h"

#pragma pack(1)

struct addr_mcast_pkg {
	unsigned int	pkg_type; // 1st, syn
	unsigned int	svr_id;
	char			name[16];
	char			ip[16];
	unsigned short	port;
};

#pragma pack()

struct addr_cache {
	char		svr_name[16];
	GHashTable*	addr_tbl;
};

typedef struct addr_mcast_pkg addr_mcast_pkg_t;
typedef struct addr_cache addr_cache_t;

time_t next_syn_addr_tm  = 0x7FFFFFFF;
time_t next_del_addrs_tm = 0x7FFFFFFF;

static GHashTable* svr_tbl;

static addr_mcast_pkg_t addr_pkg;

// multicast socket for service name synchronization
static int addr_mcast_fd = -1;
static struct sockaddr_in addr_mcast_addr;

// multicast socket
static int mcast_fd = -1;
static struct sockaddr_in mcast_addr;

//--------------------------------------------------------


/*static inline uint32_t get_server_id()*/
/*{*/
/*return config_cache.bc_elem->online_id;*/
/*}*/

/*static inline const char * get_server_ip()*/
/*{*/
/*return config_cache.bc_elem->bind_ip;*/
/*}*/
/*static inline in_port_t get_server_port()*/
/*{*/
/*return config_cache.bc_elem->bind_port;*/
/*}*/
/*static inline const char * get_server_name()*/
/*{*/
/*return config_cache.bc_elem->online_name;*/
/*}*/
static void free_addr_cache(void* addr_cache)
{
	addr_cache_t* addr_c = addr_cache;
	g_hash_table_destroy(addr_c->addr_tbl);
	g_slice_free1(sizeof(*addr_c), addr_c);
}

static void free_addr_node(void* addr_node)
{
	g_slice_free1(sizeof(addr_node_t), addr_node);
}

static gboolean addr_pred(gpointer key, gpointer value, gpointer user_data)
{
	int* n = user_data;
	if (*n == 0) {
		return TRUE;
	} else {
		(*n)--;
		return FALSE;
	}
}

static gboolean del_an_expired_addr(gpointer key, gpointer value, gpointer user_data)
{
	addr_node_t* n = value;
	if ( (get_now_tv()->tv_sec - n->last_syn_tm) > 125 ) {
		INFO_LOG("DEL AN ADDR\t[id=%u ip=%s port=%d last_tm=%ld]",
					n->svr_id, n->ip, n->port, n->last_syn_tm);
		return TRUE;
	}
	return FALSE;
}

static void do_del_expired_addrs(gpointer key, gpointer value, gpointer user_data)
{
	addr_cache_t* ac = value;
	g_hash_table_foreach_remove(ac->addr_tbl, del_an_expired_addr, 0);
}

//--------------------------------------------------------

int create_addr_mcast_socket()
{
	srand(time(0));

	next_del_addrs_tm = get_now_tv()->tv_sec + 125;
	svr_tbl = g_hash_table_new_full(g_str_hash, g_str_equal, 0, free_addr_cache);

	addr_mcast_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (addr_mcast_fd == -1) {
		ERROR_RETURN(("Failed to Create `addr_mcast_fd`: err=%d %s", errno, strerror(errno)), -1);
	}

	memset(&addr_mcast_addr, 0, sizeof addr_mcast_addr);
	addr_mcast_addr.sin_family = AF_INET;
	inet_pton(AF_INET, config_get_strval("addr_mcast_ip"), &(addr_mcast_addr.sin_addr));
	addr_mcast_addr.sin_port = htons(config_get_intval("addr_mcast_port", 54321));
	int on = 1;
	setsockopt(addr_mcast_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);

	// Set Default Interface For Outgoing Multicasts
	in_addr_t ipaddr;
	inet_pton(AF_INET, config_get_strval("addr_mcast_outgoing_if"), &ipaddr);
	if (setsockopt(addr_mcast_fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {
		ERROR_RETURN(("Failed to Set Outgoing Interface: err=%d %s %s",
						errno, strerror(errno), config_get_strval("addr_mcast_outgoing_if")), -1);
	}
	if (bind(addr_mcast_fd, (struct sockaddr*)&addr_mcast_addr, sizeof addr_mcast_addr) == -1) {
		ERROR_RETURN(("Failed to Bind `addr_mcast_fd`: err=%d %s", errno, strerror(errno)), -1);
	}
	// Join the Multicast Group
	struct group_req req;
	req.gr_interface = if_nametoindex(config_get_strval("addr_mcast_incoming_if"));
	memcpy(&req.gr_group, &addr_mcast_addr, sizeof addr_mcast_addr);
	if (setsockopt(addr_mcast_fd, IPPROTO_IP, MCAST_JOIN_GROUP, &req, sizeof req) == -1) {
		ERROR_RETURN(("Failed to Join Mcast Grp: err=%d %s", errno, strerror(errno)), -1);
	}

	/*addr_pkg.svr_id = get_server_id();*/
	/*strcpy(addr_pkg.name, get_server_name());*/
	/*strcpy(addr_pkg.ip, get_server_ip());*/
	/*addr_pkg.port = get_server_port();*/
	return do_add_conn(addr_mcast_fd, MCAST_ADDR_TYPE_FD, &addr_mcast_addr);
}

void send_addr_mcast_pkg(uint32_t pkg_type)
{
	addr_pkg.pkg_type = pkg_type;
	sendto(addr_mcast_fd, &addr_pkg, sizeof(addr_pkg), 0, (void*)&addr_mcast_addr, sizeof(addr_mcast_addr));
	next_syn_addr_tm  = time(0) + ranged_random(40, 60);
}

addr_node_t* get_service_ipport(const char* service, unsigned int svr_id)
{
	addr_cache_t* ac = g_hash_table_lookup(svr_tbl, service);
	if (ac && g_hash_table_size(ac->addr_tbl)) {
		if (svr_id) {
			return g_hash_table_lookup(ac->addr_tbl, &svr_id);
		} else {
			int n = rand() % g_hash_table_size(ac->addr_tbl);
			return g_hash_table_find(ac->addr_tbl, addr_pred, &n);
		}
	}

	return 0;
}

int connect_to_service(const char* service_name, uint32_t svr_id, int bufsz, int timeout)
{
	INFO_LOG("TRY CONNECTING TO\t[name=%s id=%u]", service_name, svr_id);
	addr_node_t* n = get_service_ipport(service_name, svr_id);
	if (n) {
		INFO_LOG("SERVICE RESOLVED\t[name=%s id=%u %u ip=%s port=%d]",
		service_name, svr_id, n->svr_id, n->ip, n->port);
		int fd = connect_to_svr(n->ip, n->port, bufsz, timeout);
		if (fd != -1) {
			INFO_LOG("CONNECTED TO\t[%s:%u fd=%d]", n->ip, n->port, fd);
		}
		return fd;
	}
	ERROR_LOG("no server with the name [%s] and server id [%u] is found", service_name, svr_id);
	return -1;
}



void proc_addr_mcast_pkg(const void* data, int len)
{
	const addr_mcast_pkg_t* pkg = data;

	/*if ( (strcmp(pkg->name, get_server_name()) == 0) */
	/*&& (pkg->svr_id == get_server_id()) ) {*/
	/*return;*/
	/*}*/

	addr_cache_t* ac = g_hash_table_lookup(svr_tbl, pkg->name);
	if (!ac) {
		ac = g_slice_alloc(sizeof(*ac));
		strcpy(ac->svr_name, pkg->name);
		ac->addr_tbl = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_addr_node);
		g_hash_table_insert(svr_tbl, ac->svr_name, ac);
	}

	addr_node_t* addr_node = g_hash_table_lookup(ac->addr_tbl, &(pkg->svr_id));
	if (!addr_node) {
		addr_node = g_slice_alloc(sizeof(*addr_node));
		addr_node->svr_id = pkg->svr_id;
		g_hash_table_insert(ac->addr_tbl, &(addr_node->svr_id), addr_node);
	}
	strcpy(addr_node->ip, pkg->ip);
	addr_node->port        = pkg->port;
	addr_node->last_syn_tm = get_now_tv()->tv_sec;

	/*if (pkg->pkg_type == addr_mcast_1st_pkg) {*/
	/*send_addr_mcast_pkg(addr_mcast_syn_pkg);*/
	/*}*/

	INFO_LOG("RECV SERVICE NAME\t[name=%s id=%u ip=%s port=%d]",
				pkg->name, pkg->svr_id, pkg->ip, pkg->port);
}

void del_expired_addrs()
{
	g_hash_table_foreach(svr_tbl, do_del_expired_addrs, 0);
	next_del_addrs_tm = get_now_tv()->tv_sec + 125;
}

/*//--------------------------------------------------------*/
/*int create_mcast_socket()*/
/*{*/
/*mcast_fd = socket(AF_INET, SOCK_DGRAM, 0);*/
/*if (mcast_fd == -1) {*/
/*ERROR_RETURN(("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno)), -1);*/
/*}*/

/*memset(&mcast_addr, 0, sizeof mcast_addr);*/
/*mcast_addr.sin_family = AF_INET;*/
/*inet_pton(AF_INET, config_get_strval("mcast_ip"), &(mcast_addr.sin_addr));*/
/*mcast_addr.sin_port = htons(config_get_intval("mcast_port", 54321));*/

/*int on = 1;*/
/*setsockopt(mcast_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);*/

/*// Set Default Interface For Outgoing Multicasts*/
/*in_addr_t ipaddr;*/
/*inet_pton(AF_INET, config_get_strval("mcast_outgoing_if"), &ipaddr);*/
/*if (setsockopt(mcast_fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {*/
/*ERROR_RETURN(("Failed to Set Outgoing Interface: err=%d %s %s",*/
/*errno, strerror(errno), config_get_strval("mcast_outgoing_if")), -1);*/
/*}*/

/*if (bind(mcast_fd, (struct sockaddr*)&mcast_addr, sizeof mcast_addr) == -1) {*/
/*ERROR_RETURN(("Failed to Bind `mcast_fd`: err=%d %s", errno, strerror(errno)), -1);*/
/*}*/

/*// Join the Multicast Group*/
/*struct group_req req;*/
/*req.gr_interface = if_nametoindex(config_get_strval("mcast_incoming_if"));*/
/*memcpy(&req.gr_group, &mcast_addr, sizeof mcast_addr);*/
/*if (setsockopt(mcast_fd, IPPROTO_IP, MCAST_JOIN_GROUP, &req, sizeof req) == -1) {*/
/*ERROR_RETURN(("Failed to Join Mcast Grp: err=%d %s", errno, strerror(errno)), -1);*/
/*}*/

/*return do_add_conn(mcast_fd, fd_type_mcast, &mcast_addr, 0);*/
/*}*/

int send_mcast_pkg(const void* data, int len)
{
	return sendto(mcast_fd, data, len, 0, (void*)&mcast_addr, sizeof mcast_addr);
}

