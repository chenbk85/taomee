#ifndef _NET_H_
#define _NET_H_
#include <stdint.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <libtaomee/list.h>
#define CN_NEED_CLOSE	0x01
#define CN_NEED_POLLIN	0x02
#define CN_NOTIFY_CLOSE	0x04

#define RCVBUFSZ	4096

#define PAGESIZE 8912*2

/**
 * @brief 用于网络的发送与接收数据的缓存区结构
 */
struct conn_buf
{
	uint16_t rcvprotlen; ///< 协议的长度
	uint16_t recvlen;
	uint16_t sendlen; ///< 发送数据的长度
	uint16_t sndbufsz; ///< 发送数据缓存区的大小
	uint8_t *recvptr;
	uint8_t *sendptr;
};

/**
 * @brief 记录一个远程socket的信息
 */
struct skinfo
{
	uint16_t remote_port;
	uint32_t remote_ip;
	uint32_t last_tm; ///< 上次时间
}__attribute__((packed));

enum
{
	UNUSED_TYPE_FD = 0,
	LISTEN_TYPE_FD,
	PIPE_TYPE_FD,
	REMOTE_TYPE_FD,
	MCAST_ADDR_TYPE_FD
};

struct fdinfo
{
	uint32_t id;
	int sockfd;
	uint8_t type;
	uint8_t flag;
	struct conn_buf cb;
	struct skinfo sk;
	struct list_head list;
};

struct epinfo
{
	struct fdinfo *fds;
	struct epoll_event *evs;
	struct list_head close_head;
	struct list_head etin_head;
	int epfd;
	int pipefd;
	int maxfd;
	int fdsize;
	int count;
};

struct shm_block;

extern struct epinfo epi;

extern int net_init (int size, int pipefd);
extern int net_loop (int timeout, int max_len, int is_conn);
extern int net_start (const char *listen_ip, uint16_t listen_port);
extern void net_exit ();
extern int net_send (int fd, const uint8_t * data, uint32_t len, int is_conn);
extern int connect_to_svr (const char *ipaddr, uint16_t port, int bufsz, int timeout);
void do_del_conn (int fd, int is_conn);
int do_add_conn (int fd, uint8_t type, struct sockaddr_in *peer);

#endif

