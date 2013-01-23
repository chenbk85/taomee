#include <assert.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <linux/types.h>
#include <string.h>
#include "net.h"
#include "tsc.h"
#include "benchapi.h"
#include "daemon.h"
#include "notifier.h"
#include "dll.h"
#include "shmq.h"
/*
 * no lock in this file, these functions are not reentrant
 */

#ifdef EPOLL_MODE
struct epoll_event *ep_events;
int epfd; 
#else
struct pollfd *pollfd;
#endif

#define IN_EVENTS	0x01
#define OUT_EVENTS	0x02
#define HUP_EVENTS	0x04
#define ERR_EVENTS	0x08
int maxfd;
long long current_tsc;

inline void free_cb (struct conn_buf *p) {
	if (p->sndbufsz != 0) {
		free (p->sendptr);
	}
	if (p->rcvbufsz != 0) {
		free (p->recvptr);
	}
	memset (p, 0x0, sizeof (struct conn_buf));
} 
	
static inline long long make_id (u_short fd, int ip, int port)
{
/*
	long long retval, tmp;
	tmp = ip;
	retval = (tmp << 32) + (port << 16) + fd;
	return retval;
*/	
	static long long retval = 0;
	return (retval++);
}
static int add_events (int fd, u_char flag)
{
#ifdef EPOLL_MODE
	struct epoll_event ev;
//	ev.events = EPOLLET;
	ev.events = 0;

	if (flag & IN_EVENTS)
		ev.events |= EPOLLIN;
	if (flag & OUT_EVENTS)
		ev.events |=  EPOLLOUT;
	if (flag & HUP_EVENTS)
		ev.events |=  EPOLLHUP;
	if (flag & ERR_EVENTS)
		ev.events |=  EPOLLERR;
	
	ev.data.fd = fd;
epoll_add_again:
	if (unlikely (epoll_ctl (epfd, EPOLL_CTL_ADD, fd, &ev) != 0)) {
		if (errno == EINTR)
			goto epoll_add_again;
		ERROR_RETURN (("epoll_ctl add %d error: %m", fd), -1);
	}
#else
	int pos = fdinfo[fd].pollindex;
	pollfd[pos].revents = 0;

	if (flag & IN_EVENTS)
		pollfd[pos].events |= POLLIN;
	if (flag & OUT_EVENTS)
		pollfd[pos].events |= POLLOUT; 
	if (flag & HUP_EVENTS)
		pollfd[pos].events |= POLLHUP;
	if (flag & ERR_EVENTS)
		pollfd[pos].events |= POLLERR;
#endif
	fdinfo[fd].pe = flag;
	return 0; 
}
int do_add_conn (int fd, int flag, struct sockaddr_in *local, struct sockaddr_in *peer, long long timeout)
{
	memset (&fdinfo[fd], 0x0, sizeof (struct fdinfo));
#ifdef EPOLL_MODE
#else
	pollfd[maxfd].fd = fd;
	fdinfo[fd].pollindex = maxfd;
#endif
	fdinfo[fd].sk.sockfd = fd;
	fdinfo[fd].flag = flag | CN_VALID_MASK;
	if (!(flag & CN_PIPE_MASK)) {
		fdinfo[fd].sk.local_ip = local->sin_addr.s_addr;
		fdinfo[fd].sk.local_port = local->sin_port;
		fdinfo[fd].timeout = timeout;
	} 

	if (peer) {
		fdinfo[fd].sk.remote_ip = peer->sin_addr.s_addr;
		fdinfo[fd].sk.remote_port = peer->sin_port;
	}
	fdinfo[fd].pe = 0;
	fdinfo[fd].id = make_id(fd, fdinfo[fd].sk.remote_ip, fdinfo[fd].sk.remote_port);
	maxfd ++;

	TRACE_LOG ("add connection, fd=%d, id=%llu, flag=0x%x, timeout=%d", 
			fd, fdinfo[fd].id, fdinfo[fd].flag, timeout / tscsec); 
	return add_events (fd, IN_EVENTS);
}

void do_close_conn (int fd, int stat)
{
	switch (stat) {
	case 0:
		break;
	case 2:
		break;
	default:
		break;
	}

	if (unlikely((fdinfo[fd].flag & CN_LISTEN_MASK) || !(fdinfo[fd].flag & CN_VALID_MASK))) {
		ERROR_LOG ("listenfd=%d socket error", fd);
		return ;
	}

#ifdef EPOLL_MODE
#else
	int i = fdinfo[fd].pollindex;

	//swap tail and closed pos
	fdinfo[pollfd[maxfd - 1].fd].pollindex = i;
	pollfd[i] = pollfd[maxfd - 1];
	pollfd[maxfd - 1].fd = -1;
#endif
	free_cb (&fdinfo[fd].cb);
	memset (&fdinfo[fd], 0x0, sizeof (struct fdinfo));
	//epoll will auto clear epoll events when fd closed
	close (fd);
	maxfd --;
}

static int do_open_conn (int fd)
{
	struct sockaddr_in peer;
	struct sockaddr_in local;
	int newfd;

	local.sin_addr.s_addr = fdinfo[fd].sk.local_ip;
	local.sin_port = fdinfo[fd].sk.local_port;
	newfd = safe_tcp_accept (fd, &peer);
	if (newfd > 0) {
		do_add_conn (newfd, 0, &local, &peer, fdinfo[fd].timeout);
		fdinfo[newfd].sk.sendtm = 0;
		fdinfo[newfd].accept_fd = fd;
		rdtscll (fdinfo[newfd].sk.recvtm);
	}

	return newfd;
}

static int do_write_conn (int fd)
{
	int send_bytes;
	
	send_bytes = safe_tcp_send_n (fd, fdinfo[fd].cb.sendptr, fdinfo[fd].cb.sendlen);
	if (send_bytes > 0) {
		if (send_bytes == fdinfo[fd].cb.sendlen) {
			fdinfo[fd].cb.sendlen = 0;
		} else {
			//fixme, don't use memmove by adding mark field in conn_t structure
			memmove (fdinfo[fd].cb.sendptr, fdinfo[fd].cb.sendptr + send_bytes, 
					fdinfo[fd].cb.sendlen - send_bytes);
			fdinfo[fd].cb.sendlen -= send_bytes;
		}

		rdtscll (fdinfo[fd].sk.sendtm);
	}

	return send_bytes;
}

static int do_read_conn (int fd)
{
	int recv_bytes;
	
	if (fdinfo[fd].cb.rcvbufsz == 0) {
		assert (fdinfo[fd].cb.rcvprotlen == 0);

		fdinfo[fd].cb.recvptr = malloc (rcvbufsz); 
		if (!fdinfo[fd].cb.recvptr) 
			ERROR_RETURN (("malloc %d bytes failed", fdinfo[fd].cb.rcvbufsz), -1);
		fdinfo[fd].cb.rcvbufsz = rcvbufsz;
	} else if (fdinfo[fd].cb.rcvbufsz < fdinfo[fd].cb.rcvprotlen) {
		char *p = realloc (fdinfo[fd].cb.recvptr, fdinfo[fd].cb.rcvprotlen); 
		if (!p) 
			ERROR_RETURN (("realloc %d bytes failed", fdinfo[fd].cb.rcvprotlen), -1);

		fdinfo[fd].cb.recvptr = p;
		fdinfo[fd].cb.rcvbufsz = fdinfo[fd].cb.rcvprotlen;
	}

	recv_bytes = safe_tcp_recv (fd, fdinfo[fd].cb.recvptr + fdinfo[fd].cb.recvlen, 
			fdinfo[fd].cb.rcvbufsz - fdinfo[fd].cb.recvlen);
	if (recv_bytes > 0) {
		fdinfo[fd].cb.recvlen += recv_bytes;
		rdtscll (fdinfo[fd].sk.recvtm);
	}

	return recv_bytes;
}

inline int check_timeout (long long stamp, long long def)
{
	long long diff;

	if (def == 0)
		return 0;

	diff = current_tsc - stamp;
	if (unlikely (diff > def)) {
		TRACE_LOG ("connection timeout after %d, stamp=%llu", def / tscsec, stamp);
		return -1;
	}
	
	return 0;
}

static void do_check_conn (int fd)
{
	long long stamp;
	
	if (unlikely ((fdinfo[fd].flag & CN_FIN_MASK) && fdinfo[fd].cb.sendlen == 0)) {
		TRACE_LOG ("close connection, fd=%d, flag=0x%x", fd, fdinfo[fd].flag);
		do_close_conn (fd, 0);
	} else if (unlikely (!(fdinfo[fd].flag & CN_LISTEN_MASK) && fdinfo[fd].timeout > 0)) {
		stamp = fdinfo[fd].sk.recvtm > fdinfo[fd].sk.sendtm ? fdinfo[fd].sk.recvtm : fdinfo[fd].sk.sendtm;
		if (check_timeout (stamp, fdinfo[fd].timeout)) {
			TRACE_LOG ("connection fd=%d check timeout", fd);
			do_close_conn (fd, 2);
		}
	}
}
static int udp_recv (int fd)
{
	int udp_len;
	struct shm_block mb;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof (addr);
	
	if (fdinfo[fd].cb.rcvbufsz == 0) {
		fdinfo[fd].cb.recvptr = malloc (rcvbufsz);
		if (!fdinfo[fd].cb.recvptr)
			ERROR_RETURN (("malloc %d bytes failed, %m", rcvbufsz), -1);
		fdinfo[fd].cb.rcvbufsz = rcvbufsz;
	}
		
	udp_len = recvfrom (fd, fdinfo[fd].cb.recvptr, fdinfo[fd].cb.rcvbufsz, 
			MSG_TRUNC, (struct sockaddr *)&addr, &addrlen);

	if (udp_len <= 0) {
		ERROR_RETURN (("recvfrom error,fd=%d, %m", fd), -1);
	}

	TRACE_LOG ("recvfrom %s:%u, length=%d, rcvbufsz=%d", inet_ntoa (addr.sin_addr), 
		ntohs(addr.sin_port), udp_len, fdinfo[fd].cb.rcvbufsz);

	mb.skinfo = fdinfo[fd].sk;	
	mb.skinfo.remote_port = addr.sin_port;
	mb.skinfo.remote_ip = addr.sin_addr.s_addr;

	if (dll.handle_input (fdinfo[fd].cb.recvptr, udp_len, &mb.skinfo) != udp_len)
		ERROR_RETURN (("udp recvlen=%d, mismatch", fdinfo[fd].cb.recvlen), -1);
	else {
		mb.blk_id = fdinfo[fd].id;
		mb.type = DAT_BLOCK;
		rdtscll (mb.skinfo.recvtm);	
		mb.accept_fd = fdinfo[fd].accept_fd;
		mb.length = udp_len + sizeof (struct shm_block);

		if (shmq_push (&recvq, &mb, fdinfo[fd].cb.recvptr, 0))
			return -1;

		fdinfo[fd].cb.recvlen = 0;
		fdinfo[fd].cb.rcvprotlen = 0;
	}

	return 0;
}

static int run_once (int fd)
{
	struct shm_block mb;
	int newfd, send_bytes;
	int ret_code, i;

	TRACE_LOG ("fd=%d-------------------------------------------------------", fd);
	if (fdinfo[fd].flag & CN_UDP_MASK) {
		for (i = 0; i < 100; i++) {
			if (udp_recv (fd) == -1); {
				break;
			}
		}
		return 0;
	}
	//accept 
	if (fdinfo[fd].flag & CN_LISTEN_MASK) {
		while ((newfd = do_open_conn (fd)) > 0) {
			if (dll.handle_open && dll.handle_open (&fdinfo[newfd].cb.sendptr, 
					&fdinfo[newfd].cb.sendlen, &fdinfo[newfd].sk) == -1) {
				do_close_conn (newfd, 1);
				ERROR_RETURN (("handle_open return -1, fd=%d", newfd), -1);
			} else if (unlikely (fdinfo[newfd].cb.sendlen < 0 || fdinfo[newfd].cb.sendlen > RCVBUFMAX)) {
				do_close_conn (newfd, 1);
				ERROR_RETURN (("handle_open return length=%d invalid", fdinfo[newfd].cb.sendlen), -1);
			} else if (fdinfo[newfd].cb.sendlen > 0){
				fdinfo[newfd].cb.sndbufsz = fdinfo[newfd].cb.sendlen;

				send_bytes = safe_tcp_send_n (newfd, fdinfo[newfd].cb.sendptr, 
					fdinfo[newfd].cb.sendlen);
				if (send_bytes == -1) {
					do_close_conn (fd, 1);
					return -1;
				} 

				fdinfo[newfd].cb.sendlen -= send_bytes;
			}
			return 0;
		}
		return -1;
	}
	//recv
	if ((ret_code = do_read_conn (fd)) <= 0) {
		do_close_conn (fd, ret_code);
		return -1;
	}

	char* saved_recvptr = fdinfo[fd].cb.recvptr;
parse_again:	
	//unknow protocol length
	if (fdinfo[fd].cb.rcvprotlen == 0) {
		//parse
		fdinfo[fd].cb.rcvprotlen = dll.handle_input (fdinfo[fd].cb.recvptr, 
				fdinfo[fd].cb.recvlen, &fdinfo[fd].sk);
		TRACE_LOG ("handle_input return %d, buffer len=%d", fdinfo[fd].cb.rcvprotlen, fdinfo[fd].cb.recvlen);
	}

	//invalid protocol length
	if (unlikely (fdinfo[fd].cb.rcvprotlen < 0 || fdinfo[fd].cb.rcvprotlen > RCVBUFMAX)) {
		// 修复异常关闭连接时core掉的问题
		fdinfo[fd].cb.recvptr = saved_recvptr;
		do_close_conn (fd, 1);
		ERROR_RETURN (("handle_input return %d, invalid length", fdinfo[fd].cb.rcvprotlen), -1);
	//unknow protocol length
	} else if (unlikely (fdinfo[fd].cb.rcvprotlen == 0)) {
		if (fdinfo[fd].cb.recvlen == fdinfo[fd].cb.rcvbufsz) {
			// 修复异常关闭连接时core掉的问题
			fdinfo[fd].cb.recvptr = saved_recvptr;
			do_close_conn (fd, 1);
			ERROR_RETURN (("unsupported big protocol, recvlen=%d", fdinfo[fd].cb.recvlen), -1);
		}
	//integrity protocol	
	} else if (fdinfo[fd].cb.recvlen >= fdinfo[fd].cb.rcvprotlen){
		mb.blk_id = fdinfo[fd].id;
		mb.skinfo = fdinfo[fd].sk;	
		mb.type = DAT_BLOCK;
		mb.accept_fd = fdinfo[fd].accept_fd;
		mb.length = fdinfo[fd].cb.rcvprotlen + sizeof (struct shm_block);
	
		if (shmq_push (&recvq, &mb, fdinfo[fd].cb.recvptr, 0))
			return -1;

		if (fdinfo[fd].cb.recvlen > fdinfo[fd].cb.rcvprotlen) {
			fdinfo[fd].cb.recvptr += fdinfo[fd].cb.rcvprotlen;
		}
		fdinfo[fd].cb.recvlen -= fdinfo[fd].cb.rcvprotlen;
		fdinfo[fd].cb.rcvprotlen = 0;
		if (fdinfo[fd].cb.recvlen > 0) 
			goto parse_again;
	}

	if (fdinfo[fd].cb.recvptr != saved_recvptr) {
		if (fdinfo[fd].cb.recvlen) {
			memmove(saved_recvptr, fdinfo[fd].cb.recvptr, fdinfo[fd].cb.recvlen);
		}
		fdinfo[fd].cb.recvptr = saved_recvptr;
	}

	return 0;
}

int net_start (const char *listen_ip, u_short listen_port, int type, long long timeout)
{
	int ret_code = -1, val, listenfd;
	struct sockaddr_in servaddr;
	//fixme: set buffer to max
	const int bufsize = 200 * 1024;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (listen_port);
	inet_pton(AF_INET, listen_ip, &servaddr.sin_addr);

	listenfd = safe_socket_listen (&servaddr, type);

	if (listenfd > 0) {
		//set nonblock
		val = fcntl(listenfd, F_GETFL, 0);	
		val |= O_NONBLOCK;
		fcntl (listenfd, F_SETFL, val);

		setsockopt (listenfd, SOL_SOCKET, SO_RCVBUF, (char *) &bufsize, sizeof (int));
		setsockopt (listenfd, SOL_SOCKET, SO_SNDBUF, (char *) &bufsize, sizeof (int));

		fdinfo[listenfd].accept_fd = listenfd;
		if (type == SOCK_DGRAM)
			do_add_conn (listenfd, CN_LISTEN_MASK | CN_UDP_MASK, &servaddr, NULL, timeout);
		else
			do_add_conn (listenfd, CN_LISTEN_MASK, &servaddr, NULL, timeout);
		ret_code = 0;
	}

	BOOT_LOG (ret_code, "listen on %s:%u", listen_ip, listen_port); 
}

void net_stop ()
{
	int i;
	//attention: do_close_conn function has modified the maxfd value
	for (i = 0; i < MAXFDS; i++) {
		if (fdinfo[i].flag & (CN_VALID_MASK | CN_LISTEN_MASK))
			close (i);
	}

	free (fdinfo);
#ifdef EPOLL_MODE
#else
	free (pollfd);
#endif
}

int net_wait (int timeout)
{
	int nr;
#ifdef EPOLL_MODE
	nr = epoll_wait (epfd, ep_events, MAXFDS, timeout);
	if (unlikely (nr < 0 && errno != EINTR))
		ERROR_LOG ("epoll_wait failed, maxfd=%d, epfd=%d: %m", maxfd, epfd);
#else
	nr = poll (pollfd, maxfd, timeout);
	if (unlikely (nr < 0 && errno != EINTR)) 
		ERROR_LOG ("poll failed, maxfd=%d: %m", maxfd);
#endif
	return nr;
}

int net_loop (int nr)
{
	int pos;
#ifdef EPOLL_MODE
	int j;

	rdtscll (current_tsc);
	for (pos = 0; pos < nr; pos++) {
		int fd = ep_events[pos].data.fd;

		if (unlikely (fdinfo[fd].flag & CN_PIPE_MASK)) {
			read_pipe ();
			continue;
		}
		if (fdinfo[fd].flag & CN_FIN_MASK)
			continue;

		if (unlikely (ep_events[pos].events & (EPOLLHUP | EPOLLERR))) {
			do_close_conn (fd, 1);
			continue;
		}

		if (ep_events[pos].events & EPOLLIN) {
			//if (!(fdinfo[fd].flag & CN_LISTEN_MASK) || (fdinfo[fd].flag & CN_UDP_MASK))
			run_once (fd);
		}
	}

	for (pos = 0, j = 0; j < maxfd; pos++) {
		if (!(fdinfo[pos].flag & CN_VALID_MASK))
			continue; 
		else {
			if (!(fdinfo[pos].flag & CN_LISTEN_MASK) && !(fdinfo[pos].flag & CN_PIPE_MASK)) {
				if (fdinfo[pos].cb.sendlen > 0 && do_write_conn (pos) == -1) 
					do_close_conn (pos, 1);
				else
					do_check_conn (pos);
			}
			j++;
		}
	}
#else
	rdtscll (current_tsc);
	for (pos = 0; pos < maxfd; pos++) {	
		int fd = pollfd[pos].fd;

		if (unlikely (fdinfo[fd].flag & CN_PIPE_MASK)) {
			if (pollfd[pos].revents & POLLIN)
				read_pipe ();
			continue;
		}

		//error
		if (unlikely (pollfd[pos].revents & (POLLERR | POLLHUP))) {
			do_close_conn (fd, 1);
			continue;
		} 
		
	//	if (pollfd[pos].revents & POLLOUT) {
			if (fdinfo[fd].cb.sendlen > 0 && do_write_conn (fd) == -1) {
				do_close_conn (fd, 1);
			}
	//	} 
		if (pollfd[pos].revents & POLLIN) {
			run_once (fd);
		} else if (!(pollfd[pos].revents & POLLOUT))
			do_check_conn (fd);
	}
#endif

	return 0;
}

static int send_udp_block (const struct shm_block* mb)
{
	struct sockaddr_in addr;
	
	int data_len = mb->length - sizeof (struct shm_block);

	if (unlikely (data_len == 0))
		return 0;
	if (unlikely (data_len < 0 || data_len > RCVBUFMAX)) 
		ERROR_RETURN (("invalid block data length=%d", data_len), -1);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = mb->skinfo.remote_ip;
	addr.sin_port = mb->skinfo.remote_port;
	
	if (unlikely (sendto (mb->skinfo.sockfd, mb->data, data_len, 
			0, (struct sockaddr *)&addr, sizeof (addr)) == -1))
		ERROR_RETURN (("sendto %s:%u error, %m", inet_ntoa (addr.sin_addr), ntohs(mb->skinfo.remote_port)), -1);

	TRACE_LOG ("sendto %s:%u, length=%d", inet_ntoa (addr.sin_addr), ntohs(mb->skinfo.remote_port), data_len);
	return 0;
}

int schedule_output (struct shm_block *mb)
{
	int send_bytes, data_len;
	int fd = mb->skinfo.sockfd;

	if (unlikely (mb->type == PAD_BLOCK || fd >= MAXFDS))
		ERROR_RETURN (("bug: mb->type=%d, fd=%d, blk_id=%llu, fid=%llu", 
			mb->type, fd, mb->blk_id, fdinfo[fd].id), -1);

	if (!(fdinfo[fd].flag & CN_VALID_MASK) || mb->blk_id != fdinfo[fd].id) { 
		TRACE_LOG ("connection %d closed, discard %llu block", fd, mb->blk_id);
		return -1;
	}

	if (mb->type == FIN_BLOCK && !(fdinfo[fd].flag & CN_LISTEN_MASK)) 
		fdinfo[fd].flag |= CN_FIN_MASK;
	
	//udp
	if (fdinfo[fd].flag & CN_UDP_MASK) {
		int ret_code = send_udp_block (mb);
		return ret_code;
	}

	//tcp linger send
	if (fdinfo[fd].cb.sendlen > 0) {
		send_bytes = safe_tcp_send_n (fd, fdinfo[fd].cb.sendptr, fdinfo[fd].cb.sendlen);
		if (send_bytes < 0) {
			do_close_conn (fd, 1);
			return -1;
		// 修复了正好发送数据完毕时不进行memmove的bug
		} else if (send_bytes <= fdinfo[fd].cb.sendlen && send_bytes > 0) {
			memmove (fdinfo[fd].cb.sendptr, fdinfo[fd].cb.sendptr + send_bytes, 
					fdinfo[fd].cb.sendlen - send_bytes);
			fdinfo[fd].cb.sendlen -= send_bytes;
		}
	}

	//shm block send
	data_len = mb->length - sizeof (shm_block_t);
	send_bytes = 0;
	if ((fdinfo[fd].cb.sendlen == 0) && (data_len > 0)) {
		send_bytes = safe_tcp_send_n (fd, mb->data, data_len);
		if (send_bytes < 0) {
			do_close_conn (fd, 1);
			return -1;
		}
		
	}

	//merge buffer
	if (data_len > send_bytes){
		if (fdinfo[fd].cb.sndbufsz == 0) {
			fdinfo[fd].cb.sendptr = (char*)malloc (data_len - send_bytes);
			if (!fdinfo[fd].cb.sendptr)
				ERROR_RETURN (("malloc error, %s", strerror(errno)), -1);
			fdinfo[fd].cb.sndbufsz = data_len - send_bytes;
			
		} else if (fdinfo[fd].cb.sndbufsz < fdinfo[fd].cb.sendlen + data_len - send_bytes) {
			fdinfo[fd].cb.sendptr = (char*)realloc (fdinfo[fd].cb.sendptr,
					fdinfo[fd].cb.sendlen + data_len - send_bytes);
			if (!fdinfo[fd].cb.sendptr)
				ERROR_RETURN (("realloc error, %s", strerror(errno)), -1);
			fdinfo[fd].cb.sndbufsz = fdinfo[fd].cb.sendlen + data_len - send_bytes;
		}
			
		memcpy (fdinfo[fd].cb.sendptr + fdinfo[fd].cb.sendlen, mb->data + send_bytes, 
			data_len - send_bytes);
		fdinfo[fd].cb.sendlen += data_len - send_bytes;
	}
/*
	if (fdinfo[fd].cb.sendlen > 0) 
		poll_events |= OUT_EVENTS;
	if (fdinfo[fd].pe != poll_events);
		mod_events (fd, poll_events);
*/		
	return 0;
}

