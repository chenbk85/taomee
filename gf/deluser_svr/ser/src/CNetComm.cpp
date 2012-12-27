#include <iostream>
#include <string.h>

#include "CNetComm.h"

CNetComm::CNetComm (const char *i, u_short p) : port (p) , sockfd (-1)
{
	strcpy (this->ip, i);
	open_conn ();
}

CNetComm::~CNetComm ()
{
	if (sockfd != -1) 
		close (sockfd);
}

int
CNetComm::open_conn ()
{
	struct sockaddr_in servaddr;
	struct timeval tv = { TIMEOUT, 0 };

	memset (&servaddr, 0, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (this->port);
	inet_pton (AF_INET, this->ip, &servaddr.sin_addr);

	sockfd = socket (AF_INET, SOCK_STREAM, 0); //TCP here
    // sockfd = socket (AF_INET, SOCK_DGRAM, 0); // UDP here
	if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof (tv)) != 0 || \
			setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *) &tv, sizeof (tv)) != 0) {
		DEBUG_LOG ("setsockopt error !");
		close (sockfd);
		sockfd = -1;
		return -1;
	}
	if (connect (sockfd, (const struct sockaddr *) &servaddr, sizeof (servaddr)) != 0) {
		DEBUG_LOG ("connect remote server error !");
		close (sockfd);
		sockfd = -1;
		return -1;
	}

	DEBUG_LOG ("sock connected to %s:%d !",ip,port);
	return 0;
}


void
CNetComm::close_conn ()
{
	if (sockfd != -1)
		close (sockfd);
	sockfd = -1;
}


int
CNetComm::net_out(const char* sndbuf, int sndlen)
{
	//only send here
	int send_bytes, cur_len;

	for (send_bytes = 0; send_bytes < sndlen; send_bytes += cur_len) {
		cur_len = send(sockfd, sndbuf + send_bytes, sndlen - send_bytes, 0);

		if (cur_len == -1) {
			DEBUG_LOG("Enter cur_len = -1");
			if (errno == EINTR)
				cur_len = 0;
			else {
				DEBUG_LOG("send tcp packet error, fd=%d, error : %s", sockfd, strerror(errno));
				close_conn();
				return -1;
			}
		}
	}

	return send_bytes;
}

int
CNetComm::net_in (char *rcvbuf, int rcvlen)
{
	int recv_bytes, cur_len;

	for (recv_bytes = 0; recv_bytes < rcvlen; recv_bytes += cur_len) {
		cur_len = recv(sockfd, rcvbuf + recv_bytes, rcvlen - recv_bytes, 0);
		if (cur_len == 0) {
			DEBUG_LOG("closed by peer : fd= %d ", sockfd);
			close_conn();
			return -1;
		} else if (cur_len == -1) {
			if (errno == EINTR)
				cur_len = 0;
			 else if (errno == EAGAIN) {
				 DEBUG_LOG("recv %d bytes from fd = %d", recv_bytes, sockfd);
				 return recv_bytes;
			 }
			else {
				DEBUG_LOG("recv tcp packet error, fd = %d", sockfd);
				close_conn();
				return -1;
			}
		}
	}
	return recv_bytes;
}

int
CNetComm::safe_net_out (const char *sndbuf, int sndlen)
{
	DEBUG_LOG("enter the safe_net_out ....len:[%d]",sndlen);
	int len = net_out(sndbuf, sndlen);
	if (len == -1) {
		DEBUG_LOG("reconnect the server ...");
		if (open_conn() == -1) {
			close_conn();
			DEBUG_LOG("reconnect the server fail !");
			return -1;
		}

		return net_out(sndbuf, sndlen); //send the data second time
	}

	return len;
}

int
CNetComm::net_io(const char* sndbuf, int sndlen, char* rcvbuf, int rcvlen)
{
	if (safe_net_out(sndbuf, sndlen) != sndlen) {
		return -1;
	}

	return net_in(rcvbuf, rcvlen);

}

int CNetComm::net_safe_io(const char* sndbuf, int sndlen, char* rcvbuf)
{
	if (safe_net_out(sndbuf, sndlen) != sndlen) {
		ERROR_LOG("net_safe_io 1: -1");
		return -1;
	}
	if (net_in(rcvbuf, 4)!=4)
	{
		ERROR_LOG("net_safe_io 2: -1");
		return -1;
	}
	if (net_in(rcvbuf+4, (*(int*)rcvbuf)-4)!=(*(int*)rcvbuf)-4)
	{
		ERROR_LOG("net_safe_io 3: -1");
		return -1;
	}
	DEBUG_LOG("net_safe_io : recv result: [%d]",*(int*)(rcvbuf+10));
	return (*(int*)rcvbuf);
}