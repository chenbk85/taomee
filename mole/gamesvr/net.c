#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <linux/types.h>

#include <ant/utils.h>

#include "game.h"
#include "proto.h"
#include "net.h"
#include <libtaomee/log.h>
#include "tcp.h"
#include "daemon.h"
#include "util.h"
#include "service.h"
#include "shmq.h"
#include "sprite.h"
#include "timer.h"
#include "mcast.h"

inline void free_cb(struct conn_buf *p)
{
	if (p->sendptr) {
		free (p->sendptr);
		p->sendptr = NULL;
	}
	if (p->recvptr) {
		munmap (p->recvptr, RCVBUFSZ);
		p->recvptr = NULL;
	}

	p->recvlen = 0;
	p->sendlen = 0;
}
/*
typedef struct {
	list_head_t timer_list;
} sock_close_queue_t;

sock_close_queue_t sock_closeq;

static inline int
close_sockfd(void* owner, void* fd)
{
	close((int)fd);
	DEBUG_LOG("sockfd %d closed", (int)fd);
	return 0;
}*/

inline void del_from_close_queue (int fd)
{
	if (epi.fds[fd].flag & CN_NEED_CLOSE) {
		epi.fds[fd].flag &= ~CN_NEED_CLOSE;
		list_del_init (&epi.fds[fd].list);
	}
}
inline void del_from_etin_queue (int fd)
{
	if (epi.fds[fd].flag & CN_NEED_POLLIN) {
		epi.fds[fd].flag &= ~CN_NEED_POLLIN;
		list_del_init (&epi.fds[fd].list);
		TRACE_LOG ("del fd=%d from etin queue", fd);
	}
}
inline void add_to_etin_queue (int fd)
{
	if (!(epi.fds[fd].flag & (CN_NEED_CLOSE | CN_NEED_POLLIN))) {
		list_add_tail (&epi.fds[fd].list, &epi.etin_head);
		epi.fds[fd].flag |= CN_NEED_POLLIN;
		TRACE_LOG ("add fd=%d to etin queue", fd);
	}
}

inline void add_to_close_queue (int fd)
{
	del_from_etin_queue (fd);
	if (!(epi.fds[fd].flag & CN_NEED_CLOSE)) {
		list_add_tail (&epi.fds[fd].list, &epi.close_head);
		epi.fds[fd].flag |= CN_NEED_CLOSE;
		TRACE_LOG ("add fd=%d to close queue, %x", fd, epi.fds[fd].flag);
	}
}

static inline int add_events (int epfd, int fd, uint32_t flag)
{
	struct epoll_event ev;

	ev.events = flag;
	ev.data.fd = fd;
epoll_add_again:
	if (unlikely (epoll_ctl (epfd, EPOLL_CTL_ADD, fd, &ev) != 0)) {
		if (errno == EINTR)
			goto epoll_add_again;
		ERROR_RETURN (("epoll_ctl add %d error: %m", fd), -1);
	}
	return 0;
}

static int mod_events (int epfd, int fd, uint32_t flag)
{
	struct epoll_event ev;

	ev.events = EPOLLET | flag;
	ev.data.fd = fd;
epoll_mod_again:
	if (unlikely (epoll_ctl (epfd, EPOLL_CTL_MOD, fd, &ev) != 0)) {
		ERROR_LOG ("epoll_ctl mod %d error: %m", fd);
		if (errno == EINTR)
			goto epoll_mod_again;
		return -1;
	}

	return 0;
}

int do_add_conn(int fd, uint8_t type, struct sockaddr_in *peer)
{
	static uint32_t seq = 0;
	uint32_t flag;

	if (type == PIPE_TYPE_FD)
		flag = EPOLLIN;
	else
		flag = EPOLLIN | EPOLLET;

	if (add_events (epi.epfd, fd, flag) == -1)
		return -1;

	memset (&epi.fds[fd], 0x0, sizeof (struct fdinfo));
	epi.fds[fd].sockfd = fd;
	epi.fds[fd].type = type;
	epi.fds[fd].id = ++seq;
	if ( seq == 0 ) epi.fds[fd].id = ++seq;
	if (peer) {
		epi.fds[fd].sk.remote_ip = peer->sin_addr.s_addr;
		epi.fds[fd].sk.remote_port = peer->sin_port;
	}
	epi.maxfd = epi.maxfd > fd ? epi.maxfd : fd;
	epi.count ++;

	TRACE_LOG ("add fd=%d, type=%d, id=%u", fd, type, epi.fds[fd].id);
	return 0;
}

void do_del_conn (int fd, int is_conn)
{
	if (epi.fds[fd].type == UNUSED_TYPE_FD)
		return ;

	if (is_conn == 0) {
		handle_close (fd, 0);
	} else if (is_conn == 1){
		struct shm_block mb;
		mb.id = epi.fds[fd].id;;
		mb.fd = fd;
		mb.type = CLOSE_BLOCK;
		mb.length = sizeof (mb);
		shmq_push (&recvq, &mb, NULL);
	}

	del_from_etin_queue (fd);
	del_from_close_queue (fd);

	free_cb (&epi.fds[fd].cb);
	epi.fds[fd].type = UNUSED_TYPE_FD;

	//epoll will auto clear epoll events when fd closed
	close (fd);
	epi.count --;

/*
	time_t exptm = get_now_tv()->tv_sec + 5;
	ADD_TIMER_EVENT(&sock_closeq, close_sockfd, (void*)fd, exptm);
*/

	if (epi.maxfd == fd) {
		int i;
		for (i = fd - 1; i >= 0; i--)
			if (epi.fds[i].type != UNUSED_TYPE_FD)
				break;
		epi.maxfd = i;
	}
	TRACE_LOG ("close fd=%d", fd);
}

static int do_open_conn (int fd)
{
	struct sockaddr_in peer;
	int newfd;

	newfd = safe_tcp_accept (fd, &peer);
	if (newfd > 0) {
		struct shm_block mb;

		epi.fds[newfd].sk.last_tm = now.tv_sec;
		do_add_conn (newfd, REMOTE_TYPE_FD, &peer);

		mb.id = epi.fds[newfd].id;
		mb.fd = newfd;
		mb.type = OPEN_BLOCK;
		mb.length = sizeof (mb) + sizeof (struct skinfo);
		if (shmq_push (&recvq, &mb, (const uint8_t *)&epi.fds[newfd].sk) == -1)
			do_del_conn (newfd, 2);

	}

	return newfd;
}

int do_write_conn (int fd)
{
	int send_bytes;

	send_bytes = safe_tcp_send_n (fd, epi.fds[fd].cb.sendptr, epi.fds[fd].cb.sendlen);
	if (send_bytes == 0)
		return 0;
	else if (send_bytes > 0) {
		if (send_bytes < epi.fds[fd].cb.sendlen)
			memmove (epi.fds[fd].cb.sendptr, epi.fds[fd].cb.sendptr + send_bytes,
					epi.fds[fd].cb.sendlen - send_bytes);

		epi.fds[fd].cb.sendlen -= send_bytes;
		epi.fds[fd].sk.last_tm = now.tv_sec;
	} else {
		return -1;
	}

	return send_bytes;
}

static int do_read_conn (int fd, int max)
{
	int recv_bytes;

	if (!epi.fds[fd].cb.recvptr) {
		epi.fds[fd].cb.rcvprotlen = 0;
		epi.fds[fd].cb.recvlen = 0;
		epi.fds[fd].cb.recvptr = mmap (0, RCVBUFSZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (epi.fds[fd].cb.recvptr == MAP_FAILED)
			ERROR_RETURN (("mmap failed"), -1);
	}

	if (RCVBUFSZ == epi.fds[fd].cb.recvlen) {
		TRACE_LOG ("recv buffer is full, fd=%d", fd);
		return 0;
	}

	recv_bytes = safe_tcp_recv (fd, epi.fds[fd].cb.recvptr + epi.fds[fd].cb.recvlen,
			max - epi.fds[fd].cb.recvlen);
	if (recv_bytes > 0) {
		epi.fds[fd].cb.recvlen += recv_bytes;
		epi.fds[fd].sk.last_tm = now.tv_sec;
	//close
	} else if (recv_bytes == 0){
		return -1;
	}
	//EAGAIN ...

	if (epi.fds[fd].cb.recvlen == max)
		add_to_etin_queue (fd);
	else
		del_from_etin_queue (fd);

	return recv_bytes;
}

static int net_recv(int fd, int max, int is_conn)
{
	int cnt = 0;

	assert(max <= RCVBUFSZ);
	if (epi.fds[fd].type == PIPE_TYPE_FD) {
		read(fd, epi.fds[fd].cb.recvptr, max);
		return 0;
	}
	if (do_read_conn(fd, max) == -1) {
		return -1;
	}

	uint8_t* saved_recvptr = epi.fds[fd].cb.recvptr;
parse_again:
	//unknow protocol length
	if (epi.fds[fd].cb.rcvprotlen == 0) {
		//parse
		epi.fds[fd].cb.rcvprotlen = handle_input((const char *)epi.fds[fd].cb.recvptr,
													epi.fds[fd].cb.recvlen, fd, is_conn);
		TRACE_LOG("handle_parse pid=%d return %d, buffer len=%d, fd=%d", getpid(),
					epi.fds[fd].cb.rcvprotlen, epi.fds[fd].cb.recvlen, fd);
	}

	//invalid protocol length
	if (unlikely(epi.fds[fd].cb.rcvprotlen > max)) {
		epi.fds[fd].cb.recvptr = saved_recvptr;
		ERROR_RETURN(("handle_parse return %d, invalid length", epi.fds[fd].cb.rcvprotlen), -1);
	//unknow protocol length
	} else if (unlikely(epi.fds[fd].cb.rcvprotlen == 0)) {
		if (epi.fds[fd].cb.recvlen == max) {
			epi.fds[fd].cb.recvptr = saved_recvptr;
			ERROR_RETURN(("unsupported big protocol, recvlen=%d", epi.fds[fd].cb.recvlen), -1);
		}
	//integrity protocol
	} else if (epi.fds[fd].cb.recvlen >= epi.fds[fd].cb.rcvprotlen) {
		if (!is_conn) {
			// handle msg from DB server
			handle_process(epi.fds[fd].cb.recvptr, epi.fds[fd].cb.rcvprotlen, fd, 0);
		} else {
			struct shm_block mb;
			epi2shm(fd, &mb);
			if (shmq_push(&recvq, &mb, epi.fds[fd].cb.recvptr)) {
				epi.fds[fd].cb.recvptr = saved_recvptr;
				return -1;
			}
		}

		cnt++;
		if (epi.fds[fd].cb.recvlen > epi.fds[fd].cb.rcvprotlen) {
			epi.fds[fd].cb.recvptr += epi.fds[fd].cb.rcvprotlen;
		}
		epi.fds[fd].cb.recvlen    -= epi.fds[fd].cb.rcvprotlen;
		epi.fds[fd].cb.rcvprotlen  = 0;
		if (epi.fds[fd].cb.recvlen > 0)
			goto parse_again;
	}

	if (epi.fds[fd].cb.recvptr != saved_recvptr) {
		if (epi.fds[fd].cb.recvlen) {
			memmove(saved_recvptr, epi.fds[fd].cb.recvptr, epi.fds[fd].cb.recvlen);
		}
		epi.fds[fd].cb.recvptr = saved_recvptr;
	}

	return cnt;
}

int net_start(const char* listen_ip, uint16_t listen_port)
{
	int ret_code = -1, val, listenfd;
	struct sockaddr_in servaddr;
	const int bufsize = 240 * 1024;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (listen_port);
	inet_pton(AF_INET, listen_ip, &servaddr.sin_addr);

	listenfd = safe_socket_listen (&servaddr, SOCK_STREAM);

	if (listenfd > 0) {
		//set nonblock
		val = fcntl(listenfd, F_GETFL, 0);
		val |= O_NONBLOCK;
		fcntl (listenfd, F_SETFL, val);

		setsockopt (listenfd, SOL_SOCKET, SO_RCVBUF, (char *) &bufsize, sizeof (int));
		setsockopt (listenfd, SOL_SOCKET, SO_SNDBUF, (char *) &bufsize, sizeof (int));

		do_add_conn (listenfd, LISTEN_TYPE_FD, NULL);
		ret_code = 0;
	}

	BOOT_LOG (ret_code, "Listen on %s:%u", listen_ip, listen_port);
}

int connect_to_svr (const char *ipaddr, uint16_t port, int bufsz, int timeout)
{
	struct sockaddr_in peer;
	int fd;

	bzero (&peer, sizeof (peer));
	peer.sin_family = AF_INET;
	peer.sin_port = htons (port);
	if (inet_pton (AF_INET, ipaddr, &peer.sin_addr) <= 0)
		ERROR_RETURN (("inet_pton %s failed, %m", ipaddr), -1);

	fd = safe_tcp_connect (ipaddr, port, timeout);
	if (fd != -1) {
		DEBUG_LOG ("connect to %s:%u", ipaddr, port);
		do_add_conn (fd, REMOTE_TYPE_FD, &peer);
		epi.fds[fd].cb.sndbufsz = bufsz;
		epi.fds[fd].cb.sendptr = malloc (epi.fds[fd].cb.sndbufsz);
		assert (epi.fds[fd].cb.sendptr != NULL);
	}

	return fd;
}

int net_send (int fd, const uint8_t *data, uint32_t len, int is_conn)
{
	int prev_stat = 0;
	int send_bytes;

	//tcp linger send
	if (epi.fds[fd].cb.sendlen > 0) {
		if (do_write_conn (fd) == -1) {
			do_del_conn (fd, is_conn);
			return -1;
		}
		prev_stat = 1;
	}

	send_bytes = 0;
	if (epi.fds[fd].cb.sendlen == 0) {
		send_bytes = safe_tcp_send_n (fd, data, len);
		if (send_bytes == -1) {
			do_del_conn (fd, is_conn);
			return -1;
		}

	}

	//merge buffer
	if (len > send_bytes){
		if (!epi.fds[fd].cb.sendptr) {
			epi.fds[fd].cb.sendptr = (uint8_t*) malloc (len - send_bytes);
			if (!epi.fds[fd].cb.sendptr)
				ERROR_RETURN (("malloc error, %s", strerror(errno)), -1);
			epi.fds[fd].cb.sndbufsz = len - send_bytes;

		} else if (epi.fds[fd].cb.sndbufsz < epi.fds[fd].cb.sendlen + len - send_bytes) {
			epi.fds[fd].cb.sendptr = (uint8_t*)realloc (epi.fds[fd].cb.sendptr,
					epi.fds[fd].cb.sendlen + len - send_bytes);
			if (!epi.fds[fd].cb.sendptr)
				ERROR_RETURN (("realloc error, %s", strerror(errno)), -1);
			epi.fds[fd].cb.sndbufsz = epi.fds[fd].cb.sendlen + len - send_bytes;
		}

		memcpy (epi.fds[fd].cb.sendptr + epi.fds[fd].cb.sendlen, data + send_bytes,
			len - send_bytes);
		epi.fds[fd].cb.sendlen += len - send_bytes;
	}

	if (epi.fds[fd].cb.sendlen > 0 && !prev_stat)
		mod_events (epi.epfd, fd, EPOLLOUT | EPOLLIN);
	else if (prev_stat && epi.fds[fd].cb.sendlen == 0)
		mod_events (epi.epfd, fd, EPOLLIN);

	return 0;
}

static int schedule_output (struct shm_block *mb)
{
	int data_len;
	int fd = mb->fd;

	if (unlikely (fd > epi.maxfd)) {
		DEBUG_LOG ("discard the message: mb->type=%d, fd=%d, maxfd=%d, id=%u, fid=%u",
			mb->type, fd, epi.maxfd, mb->id, epi.fds[fd].id);
		return -1;
	}

	if (epi.fds[fd].type != REMOTE_TYPE_FD || mb->id != epi.fds[fd].id) {
		TRACE_LOG ("connection %d closed, discard %u, %u block", fd, mb->id, epi.fds[fd].id);
		return -1;
	}

	if (mb->type == FIN_BLOCK && epi.fds[fd].type != LISTEN_TYPE_FD) {
		add_to_close_queue (fd);
		return 0;
	}

	//shm block send
	data_len = mb->length - sizeof (shm_block_t);
	return net_send (fd, mb->data, data_len, 1);
}

int net_init (int size, int pipefd)
{
//	INIT_LIST_HEAD(&sock_closeq.timer_list);

	if ((epi.epfd = epoll_create (size)) < 0) {
		ERROR_LOG ("epoll_create failed, %s", strerror (errno));
		return -1;
	}
	epi.evs = (struct epoll_event*) calloc (size, sizeof (struct epoll_event));
	if (!epi.evs)
		goto events_fail;

	epi.fds = (struct fdinfo*) calloc (size, sizeof (struct fdinfo));
	if (!epi.fds)
		goto fd_fail;

	epi.fdsize = size;
	epi.maxfd = 0;
	epi.count = 0;
	epi.pipefd = pipefd;
	INIT_LIST_HEAD (&epi.etin_head);
	INIT_LIST_HEAD (&epi.close_head);
	epi.fds[pipefd].cb.recvptr = NULL;
	do_add_conn (pipefd, PIPE_TYPE_FD, NULL);
	return 0;

fd_fail:
	free (epi.evs);
events_fail:
	close (epi.epfd);
	ERROR_RETURN (("malloc failed, size=%d", size), -1);
}

void net_exit ()
{
	int i;
	for (i = 0; i < epi.maxfd + 1; i++) {
		if (epi.fds[i].type == UNUSED_TYPE_FD)
			continue;

		free_cb (&epi.fds[i].cb);
		close (i);
	}

	free (epi.fds);
	free (epi.evs);
	close (epi.epfd);
}

static inline void handle_send_queue ()
{
	struct shm_block *mb;

	while (shmq_pop (&sendq, &mb) == 0) {
		schedule_output (mb);
	}
}

static inline void handle_recv_queue ()
{
	struct shm_block *mb;

	while (shmq_pop (&recvq, &mb) == 0) {
		switch (mb->type) {
			case PAD_BLOCK:
			case FIN_BLOCK:
				ERROR_LOG ("shmq_pop invalid shm_block, type=%u, fd=%u", mb->type, mb->fd);
				break;
			case CLOSE_BLOCK:
			{
				sprite_t* p = get_sprite_by_fd(mb->fd);
				if ( p && p->group ) {
					SET_SPRITE_OFFLINE(p);
					uint8_t reason = GER_player_offline;
					p->waitcmd = 0;
					leave_game_cmd(p, &reason, 1);
				} else {
					handle_close(mb->fd, 1);
				}
				break;
			}
			case OPEN_BLOCK:
				if (handle_open (mb) == -1) {
					mb->type = FIN_BLOCK;
					mb->length = sizeof (*mb);
					shmq_push (&sendq, mb, NULL);
				}
				break;
			case DATA_BLOCK:
				// The corresponding `sprite` had already been deleted
				if ((mb->fd <= fds.fdmax) && (mb->length > sizeof(*mb))) // handle data from parent process
					handle_process (mb->data, mb->length - sizeof (*mb), mb->fd, 1);
				break;
		}
	}
}

int net_loop (int timeout, int max_len, int is_conn)
{
	int pos, nr;
	struct list_head *l, *p;
	struct fdinfo *fi;
	char trash[RCVBUFSZ];

	list_for_each_safe (p, l, &epi.close_head) {
		fi = list_entry (p, struct fdinfo, list);
		if (fi->cb.sendlen > 0)
			do_write_conn (fi->sockfd);
		do_del_conn (fi->sockfd, 0);
	}

	list_for_each_safe (p, l, &epi.etin_head) {
		fi = list_entry (p, struct fdinfo, list);
		if (net_recv (fi->sockfd, max_len, is_conn) == -1)
			do_del_conn (fi->sockfd, is_conn);
	}

	nr = epoll_wait (epi.epfd, epi.evs, epi.maxfd + 1, timeout);
	if (unlikely (nr < 0 && errno != EINTR)) {
		ERROR_RETURN (("epoll_wait failed, maxfd=%d, epfd=%d: %m",
					epi.maxfd, epi.epfd), -1);
	}

	gettimeofday(&now, 0);
	localtime_r(&now.tv_sec, &tm_cur);

	if (is_conn) handle_send_queue ();

	for (pos = 0; pos < nr; pos++) {
		int fd = epi.evs[pos].data.fd;
		if (fd > epi.maxfd || epi.fds[fd].sockfd != fd || epi.fds[fd].type == UNUSED_TYPE_FD) {
			ERROR_LOG ("delayed epoll events: event fd=%d, cache fd=%d, maxfd=%d, type=%d pid=%u",
					fd, epi.fds[fd].sockfd, epi.maxfd, epi.fds[fd].type, getpid());
			continue;
		}
		
		if ( unlikely(epi.fds[fd].type == PIPE_TYPE_FD) ) {
			if (epi.evs[pos].events & EPOLLHUP) {
				if (is_conn) { // Child Crashed
					CRIT_LOG("CHILD PROCESS CRASHED!");
					restart_child_process();
				} else { // Parent Crashed
					stop = 1;
					DEBUG_LOG ("parent has exit!");
					return -1;
				}
			} else {
				while ( read(fd, trash, RCVBUFSZ) == RCVBUFSZ) ;
			}
			continue;
		}


		if (epi.evs[pos].events & EPOLLIN) {
			//accept
			if (epi.fds[fd].type == LISTEN_TYPE_FD)
				while (do_open_conn (fd) > 0);
			else if (epi.fds[fd].type == MCAST_ADDR_TYPE_FD) {
				static char buf[PAGESIZE];
			    int  i;
			    for (i = 0; i != 100; ++i) {
					int len = recv(fd, buf, PAGESIZE, MSG_DONTWAIT);
				    if (len > 0) {
						proc_addr_mcast_pkg(buf, len);
				   	} else {
					   break;
			   		}
	   			}
			} else if (net_recv (fd, max_len, is_conn) == -1) {
				do_del_conn (fd, is_conn);
			}
		}

		if (epi.evs[pos].events & EPOLLOUT) {
			if (epi.fds[fd].cb.sendlen > 0 && do_write_conn (fd) == -1)
				do_del_conn (fd, is_conn);
			if (epi.fds[fd].cb.sendlen == 0)
				mod_events (epi.epfd, fd, EPOLLIN);
		}

		if (epi.evs[pos].events & EPOLLHUP) {
			do_del_conn(fd, is_conn);
			TRACE_LOG("EPOLLHUP\t[fd=%d, is_conn=%d]", fd, is_conn);
		}
	}

	if(!is_conn) {
		handle_recv_queue ();
		handle_timer ();
		time_t now_sec = get_now_tv()->tv_sec;
		if (now_sec > next_del_addrs_tm) {
			del_expired_addrs();
		}
	}

	return 0;
}
