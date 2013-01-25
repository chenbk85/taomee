#ifndef ITERATIVE_SERVER_NET_H_
#define ITERATIVE_SERVER_NET_H_

#ifdef __cplusplus
extern "C" {
#endif

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

enum {
	//page_size	= 8192
	def_page_size = 8192
};

typedef struct conn_buf {
	uint32_t	rcvprotlen;
	uint32_t	recvlen;
	uint32_t	sendlen;
	uint32_t	sndbufsz;
	uint8_t*	recvptr;
	uint8_t*	sendptr;
} conn_buf_t;

typedef struct skinfo {
	uint16_t	remote_port;
	uint32_t	remote_ip;
	uint32_t	last_tm;
} __attribute__((packed)) skinfo_t;

enum {
	fd_type_unused = 0,
	fd_type_listen,
	fd_type_remote,
	fd_type_server,
	fd_type_mcast
};

typedef struct fdinfo {
	int				sockfd;
	uint8_t		type;
	uint8_t		flag;
	conn_buf_t	cb;
	skinfo_t		sk;
	list_head_t	list;
} fdinfo_t;

struct epinfo {
	fdinfo_t*	fds;
	struct epoll_event*	evs;
	list_head_t	etin_head;
	int			epfd;
	int			maxfd;
	int			fdsize;
	int			count;
};

extern struct epinfo epi;
extern time_t socket_timeout;
extern int page_size;

int  net_init(int size, int maxevents);
int  net_loop(int timeout, int max_len);
int  net_start(const char *listen_ip, uint16_t listen_port);
void net_exit();

int  mod_events(int epfd, int fd, uint32_t flag);
int  do_write_conn(int fd);
int  do_add_conn(int fd, uint8_t type, struct sockaddr_in *peer);
void do_del_conn(int fd);

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // ITERATIVE_SERVER_NET_H_
