#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

extern "C" {
#include "benchapi.h"
}

//extend this class later ! TODO------------

#ifndef  NET_COMM_INC
#define  NET_COMM_INC

class CNetComm {
	private:
		char ip[16];
		u_short port;
		int sockfd;
#define TIMEOUT	5
	public:
		int open_conn ();
		void close_conn ();
		int net_out (const char *sndbuf, int sndlen); //with no return data : send
		int net_in	(char *rcvbuf, int rcvlen); //recv data : recv
		int safe_net_out (const char *sndbuf, int sndlen);
		int net_io (const char *sndbuf, int sndlen, char *rcvbuf, int rcvlen); //send & recv

		int net_safe_io(const char* sndbuf, int sndlen, char* rcvbuf);

	public:
		CNetComm (const char *ip, u_short port);
		virtual ~CNetComm ();
};

#endif   /* ----- #ifndef NET_COMM_INC  ----- */
