#ifndef _TCPIP_H_
#define _TCPIP_H_
#include "common.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "common.h" 

//连接类型
enum connect_type {TCP, UDP};


int open_socket (const char *svrip, u_short svrport);
int open_socket_udp (const char* svrip, u_short svrport,struct  sockaddr_in *servaddr);
int net_io(int socketfd, const char *sndbuf, int sndlen, char** rcvbuf, int *rcvlen);
int net_io_no_return(int sockfd, const char *sndbuf, int sndlen );

/**
 * ip_type :0x01 内网 
 * ip_type :0x02 外网 
 * */
const char * get_ip(const int ip_type );
#endif 
