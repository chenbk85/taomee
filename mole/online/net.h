#ifndef _NET_H_
#define _NET_H_

// C89
#include <time.h>
// C99
#include <stdint.h>
// Posix
#include <netinet/in.h>
#include <sys/socket.h>
// Linux
#include <sys/epoll.h>

#include <libtaomee/list.h>

#define CN_NEED_CLOSE	0x01
#define CN_NEED_POLLIN	0x02
#define CN_NOTIFY_CLOSE	0x04

#define PAGESIZE	(1024 * 32)

struct conn_buf {
	uint16_t rcvprotlen;
	uint16_t recvlen;
	uint16_t sendlen;
	uint16_t sndbufsz;
	uint8_t *recvptr;
	uint8_t *sendptr;
};

struct skinfo {
	uint16_t remote_port;
	uint32_t remote_ip;
	uint32_t last_tm;
}__attribute__((packed));

enum {
	UNUSED_TYPE_FD = 0,
	LISTEN_TYPE_FD,
	PIPE_TYPE_FD,
	REMOTE_TYPE_FD,
	MCAST_TYPE_FD,
	MCAST_ADDR_TYPE_FD
};

struct bind_config_elem;

struct fdinfo {
	uint32_t id;
	int sockfd;
	uint8_t type;
	uint8_t flag;
	struct conn_buf cb;
	struct skinfo sk;
	struct bind_config_elem* bc_elem;
	struct list_head list;
};

struct epinfo {
	struct fdinfo *fds;
	struct epoll_event *evs;
	struct list_head close_head;
	struct list_head etin_head;
	int epfd;
	int maxfd;
	int fdsize;
	int count;
};

extern struct epinfo epi;

int  net_init(int size, int maxevents);
int  net_loop(int timeout, int max_len, int is_conn);
int  net_start(const char *listen_ip, uint16_t listen_port, struct bind_config_elem* bc_elem);
void net_exit();
int  net_send(int fd, const uint8_t * data, uint32_t len, int is_conn);
int  connect_to_svr(const char *ipaddr, uint16_t port, int bufsz, int timeout);

int  do_add_conn(int fd, uint8_t type, struct sockaddr_in *peer, struct bind_config_elem* bc_elem);
void do_del_conn(int fd, int is_conn);

#endif
