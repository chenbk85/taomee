#ifndef _TCP_H_
#define _TCP_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
extern int mole_safe_tcp_accept (int sockfd, struct sockaddr_in *peer);
extern int mole_safe_tcp_connect (const char *ipaddr, u_short port, int timeout);
extern int mole_safe_socket_listen (struct sockaddr_in *servaddr, int type);
extern int mole_safe_tcp_send_n (int sockfd, const void *buf, int total);
extern int mole_safe_tcp_recv (int sockfd, void *buf, int bufsize);
extern int mole_safe_tcp_recv_n (int sockfd, void *buf, int total);
#endif
