#ifndef _NET_H_
#define _NET_H_
#include "shmq.h"

#ifdef EPOLL_MODE
#include <sys/epoll.h>
#else
#include <sys/poll.h>
#endif

#include <netinet/in.h>
#define CN_VALID_MASK   0x01
#define CN_LISTEN_MASK  0x04
#define CN_PIPE_MASK	0x08
#define CN_UDP_MASK  	0x10
#define CN_FIN_MASK  	0x20

#define RCVBUFSZ	4096
#define RCVBUFMAX	1 << 23
struct conn_buf {
	int sndbufsz;
	int rcvbufsz;
	int rcvprotlen;

    int recvlen;
    int sendlen;
    char *recvptr;
	char *sendptr;
};

struct fdinfo {
	long long id;
	u_short accept_fd; 
#ifdef EPOLL_MODE
#else
	u_short pollindex;
#endif
	struct conn_buf cb;
	skinfo_t sk;
	long long timeout;
	char pe;
	char flag;
};
extern struct fdinfo *fdinfo;

#ifdef EPOLL_MODE
struct epoll_event *ep_events;
int epfd; 
#else
struct pollfd *pollfd;
#endif

extern int rcvbufsz;
extern int sndbufsz;
extern int maxfd;

extern int do_add_conn (int fd, int flag, struct sockaddr_in *local, struct sockaddr_in *peer, long long timeout);
extern void do_close_conn (int fd, int stat);
extern int net_loop (int nr);
extern int net_wait (int timeout);
extern int net_start (const char *listen_ip, u_short listen_port, int type, long long timeout);
extern void net_stop ();
extern void fdinfo_dump ();
extern int schedule_output (struct shm_block *mb);
extern inline int check_timeout (long long stamp, long long def);
#endif

