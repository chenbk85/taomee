#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>

#include <libtaomee/log.h>

int
mole_safe_tcp_recv (int sockfd, void *buf, int bufsize)
{
	int cur_len;
recv_again:
	cur_len = recv (sockfd, buf, bufsize, 0);
	//closed by client
	if (cur_len == 0)
	{
		TRACE_LOG ("connection closed by peer, fd=%d", sockfd);
		return 0;
	}
	else if (cur_len == -1)
	{
		if (errno == EINTR)
			goto recv_again;
		else
			ERROR_LOG ("recv error, fd=%d, errno=%d %m", sockfd, errno);
	}

	return cur_len;
}


int
mole_safe_tcp_recv_n (int sockfd, void *buf, int total)
{
	int recv_bytes, cur_len;

	for (recv_bytes = 0; recv_bytes < total; recv_bytes += cur_len)
	{
		cur_len = recv (sockfd, buf + recv_bytes, total - recv_bytes, 0);
		//closed by client
		if (cur_len == 0)
		{
			TRACE_LOG ("connection closed by peer, fd=%d", sockfd);
			return -1;
		}
		else if (cur_len == -1)
		{
			if (errno == EINTR)
				cur_len = 0;
			else if (errno == EAGAIN)
			{
				TRACE_LOG ("recv %d bytes from fd=%d", recv_bytes, sockfd);
				return recv_bytes;
			}
			else
			{
				ERROR_RETURN (("recv tcp packet error, fd=%d, %m", sockfd), -1);
			}
		}
	}

	return recv_bytes;
}

/*
 * safe_tcp_send: send wrapper for non-block tcp connection
 * @return :
 */
int
mole_safe_tcp_send_n (int sockfd, const void *buf, int total)
{
	int send_bytes, cur_len;

	for (send_bytes = 0; send_bytes < total; send_bytes += cur_len)
	{
		cur_len = send (sockfd, buf + send_bytes, total - send_bytes, 0);
		//closed by client
		if (cur_len == 0)
		{
			TRACE_LOG ("send tcp packet error, fd=%d, %m", sockfd);
			return -1;
		}
		else if (cur_len == -1)
		{
			if (errno == EINTR)
				cur_len = 0;
			else if (errno == EAGAIN)
			{
				return send_bytes;
			}
			else
			{
				TRACE_LOG ("send tcp packet error, fd=%d, %m", sockfd);
				return -1;
			}
		}
	}

	//TRACE_LOG ("send: fd=%d, len=%d", sockfd, send_bytes);
	return send_bytes;
}

/*
 * safe_tcp_listen: create tcp socket to listen
 * @return :	listen socket description
 *	> 0  ok
 * 	= -1 error
 */
int
mole_safe_socket_listen (struct sockaddr_in *servaddr, int type)
{
	int listenfd;
	int reuse_addr = 1;

	if ((listenfd = socket (AF_INET, type, 0)) == -1)
		ERROR_RETURN (("socket error, %s", strerror (errno)), -1);

	setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,
			sizeof reuse_addr);
	if (bind (listenfd, (struct sockaddr *) servaddr, sizeof (struct sockaddr))
			== -1)
	{
		ERROR_LOG ("bind error, %s", strerror (errno));
		close (listenfd);
		return -1;
	}

	if (type == SOCK_STREAM && listen (listenfd, 10) == -1)
	{
		ERROR_LOG ("listen error, %s", strerror (errno));
		close (listenfd);
		return -1;
	}

	return listenfd;
}

/*
 * safe_tcp_accept:	accept a client tcp connection
 * @return :	new socket description
 *	> 0  ok
 * 	= -1 error
 */
int
mole_safe_tcp_accept (int sockfd, struct sockaddr_in *peer)
{
	socklen_t peer_size;
	int newfd, val, err;

	for (;;)
	{
		peer_size = sizeof (struct sockaddr_in);
		if ((newfd = accept (sockfd, (struct sockaddr *) peer, &peer_size)) < 0)
		{
			if (errno == EINTR)
				continue;		/* back to for () */

			if (errno == EAGAIN)
				return -1;

			ERROR_RETURN (("accept failed, listenfd=%d", sockfd), -1);
		}

		break;
	}

	val = O_NONBLOCK;
	fcntl (newfd, F_SETFL, val);

	val = 1;
	err = setsockopt(newfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&val, sizeof(val));

	TRACE_LOG ("accept connection from %s:%u, listen fd=%u, accepted fd=%u, setsockopt=%d",
			inet_ntoa (peer->sin_addr), ntohs (peer->sin_port), sockfd,
			newfd, err);
	return newfd;
}

int
mole_safe_tcp_connect (const char *ipaddr, u_short port, int timeout)
{
	int val, sockfd, state;
	struct sockaddr_in peer;
	fd_set writefds;
	struct timeval tm;

	bzero (&peer, sizeof (peer));
	peer.sin_family = AF_INET;
	peer.sin_port = htons (port);
	if (inet_pton (AF_INET, ipaddr, &peer.sin_addr) <= 0)
		ERROR_RETURN (("inet_pton %s failed, %m", ipaddr), -1);

	if ((sockfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
		ERROR_RETURN (("socket failed, %s", strerror (errno)), -1);

	if (timeout > 0) {
		val = O_NONBLOCK | fcntl (sockfd, F_GETFL);
		fcntl (sockfd, F_SETFL, val);
	}
	state = 0;
again:
	if (connect (sockfd, (const struct sockaddr *) &peer, sizeof (peer)) == 0)
		return sockfd;

	if (errno != EINPROGRESS || timeout <= 0){
		close (sockfd);
		ERROR_RETURN (("connect %s:%u failed, %m", ipaddr, port), -1);
	}

	tm.tv_sec = timeout;
	tm.tv_usec = 0;
	FD_ZERO (&writefds);
	FD_SET (sockfd, &writefds);
	if (state == 0 && select (sockfd + 1, NULL, &writefds, NULL, &tm) > 0){
		state++;
		goto again;
	}

	close (sockfd);
	ERROR_RETURN (("connect %s:%u timeout", ipaddr, port), -1);
}
