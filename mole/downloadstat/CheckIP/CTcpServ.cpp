#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include "CTcpServ.h"

#define UNXI_SOCKET "./CheckIP.sock"

C_TcpServ::C_TcpServ (std::string i, uint16_t p) : listenfd(-1), ip(i), port(p)
{

}


C_TcpServ::~C_TcpServ ()
{

}

int
C_TcpServ::socket_listen ()
{
	int reuse_addr = -1;
	struct sockaddr_un servaddr;
	const int bufsize = 1024;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNXI_SOCKET);
    unlink(UNXI_SOCKET);
	// inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr);

	if ((listenfd = socket (AF_LOCAL, SOCK_STREAM, 0)) == -1) {
		ERROR_LOG ("socket error : %s", std::strerror (errno));
		return -1;
	}

	//must put here
	setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof reuse_addr);
	if (bind (listenfd, (struct sockaddr *)&servaddr, sizeof (struct sockaddr)) == -1) {
		ERROR_LOG ("bind error : %s", std::strerror (errno));
		close (listenfd);
		listenfd = -1;
		return -1;
	}

	if (listen (listenfd, 10) == -1) {
		ERROR_LOG ("listen error : %s", std::strerror (errno));
		close (listenfd);
		listenfd = -1;
		return -1;
	}

	//set the attribute
	int val = fcntl (listenfd, F_GETFL, 0);
	val |= O_NONBLOCK;
	fcntl (listenfd, F_SETFL, val);

	setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, (char *) &bufsize, sizeof (int));
	setsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, (char *) &bufsize, sizeof (int));
	return 0;
}


int
C_TcpServ::tcp_accept ()
{
	struct sockaddr_un peer;
	socklen_t peer_size;
	int newfd, err;

	for (;;) {
		peer_size = sizeof (struct sockaddr_un);
		if ((newfd = accept (listenfd, (struct sockaddr *)&peer, &peer_size)) < 0) {
			if (errno == EINTR)
				continue;
			if (errno == EAGAIN)
				return -1;
			ERROR_LOG ("accept failed, listenfd = %d", listenfd);
		}
		break;
	}

	int val = 1;
	err = setsockopt (newfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&val, sizeof(val));
	return newfd;
}

int
C_TcpServ::tcp_recv (int sockfd, int total, std::pair<uint8_t *, int> &data)
{
	int recv_bytes, cur_len;

	for (recv_bytes = 0; recv_bytes < total; recv_bytes += cur_len)
	{
		cur_len = recv (sockfd, data.first+recv_bytes, total-recv_bytes, 0);
		//closed by client
		if (cur_len == 0) {
			// ERROR_LOG ("connection closed by peer, fd=%d", sockfd);
			return -1;
		}
		else if (cur_len == -1) {
			if (errno == EINTR)
				cur_len = 0;
			else if (errno == EAGAIN) {
				data.second = recv_bytes;
				DEBUG_LOG ("recv %d bytes from fd=%d", recv_bytes, sockfd);
				return recv_bytes;
			}
			else {
				ERROR_LOG ("recv tcp packet error, fd=%d, error : %s", sockfd, std::strerror(errno));
				return -1;
			}
		}
	}
	data.second = recv_bytes;

	return recv_bytes;
}

int
C_TcpServ::tcp_send (int sockfd, const std::pair<uint8_t *, int> &data)
{
	int send_bytes, cur_len;

	for (send_bytes = 0; send_bytes < data.second; send_bytes += cur_len) {
		cur_len = send (sockfd, data.first + send_bytes, data.second - send_bytes, 0);
		//closed by client
		if (cur_len == 0) {
			ERROR_LOG ("send tcp packet error : fd=%d, error : %s", sockfd, std::strerror(errno));
			return -1;
		}
		else if (cur_len == -1) {
			if (errno == EINTR)
				cur_len = 0;
			else if (errno == EAGAIN) {
				return send_bytes;
			}
			else {
				ERROR_LOG ("send tcp packet error : fd=%d, error : %s", sockfd, std::strerror(errno));
				return -1;
			}
		}
	}

	return send_bytes;
}
