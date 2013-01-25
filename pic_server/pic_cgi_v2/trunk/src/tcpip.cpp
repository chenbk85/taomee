#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include "tcpip.h"
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>


extern "C" {

#include <libtaomee/inet/tcp.h>
}

int
open_socket(const char *ipaddr, u_short port,uint32_t timeout )
{
	int socketfd=safe_tcp_connect(ipaddr,port,1,0);	
	if (socketfd == -1){
		return socketfd;
	}
	//设置接收发送超时
	struct timeval tv = { timeout, 0 };
	if (setsockopt (socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof (tv)) != 0
		|| setsockopt (socketfd, SOL_SOCKET, SO_SNDTIMEO, (char *) &tv, sizeof (tv)) != 0)
	{
		close(socketfd);
		return -1;
	}
	return socketfd;
}

int open_socket_udp (const char* svrip, u_short svrport,struct  sockaddr_in *servaddr)
{
	int     sockfd  ;
	bzero(servaddr, sizeof(struct  sockaddr_in));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons(svrport);
	inet_pton(AF_INET,svrip , &(servaddr->sin_addr));
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	return sockfd;
}

int net_io_no_return(int sockfd, const char *sndbuf, int sndlen )
{
	int send_bytes,  cur_len;
	for (send_bytes = 0; send_bytes < sndlen; send_bytes += cur_len){
		cur_len = send (sockfd, sndbuf + send_bytes, sndlen - send_bytes, 0);
		//closed by client
		if (cur_len == 0) {
			return NET_ERR;
		}else if (cur_len == -1){
			if (errno == EINTR) cur_len = 0;
			else return NET_ERR;
		}
   }
  return SUCC;
}

int
net_io(int sockfd, const char *sndbuf, int sndlen, char **rcvbuf,
	int  *rcvlen)
{
	int send_bytes, recv_bytes, cur_len;
 	//log send data 
	for (send_bytes = 0; send_bytes < sndlen; send_bytes += cur_len){
		cur_len = send (sockfd, sndbuf + send_bytes, sndlen - send_bytes, 0);
		//closed by client
		if (cur_len == 0) {
		//	DEBUG_LOG("send fail");
			return errno;
		}else if (cur_len == -1){
			if (errno == EINTR) cur_len = 0;
			else return  errno;
		}
	}

	recv_bytes = recv (sockfd, (void *) rcvlen, 4, 0);
	if (recv_bytes != 4) {
		return NET_ERR;
	}

	if (*rcvlen > (1 << 22)) {
		return NET_ERR;
    }
  	
	if (!(*rcvbuf = (char *) malloc (*rcvlen))) {
		return SYS_ERR;
	}

  *((int*)(*rcvbuf))= *rcvlen;
	

  for (recv_bytes = 4; recv_bytes < *rcvlen; recv_bytes += cur_len) {
      cur_len = recv (sockfd, *rcvbuf  + recv_bytes, *rcvlen - recv_bytes, 0);
	if (cur_len == 0) {
		free (*rcvbuf);
		return NET_ERR;
	}else if (cur_len == -1) {
		if (errno == EINTR) {
			cur_len = 0;
		}else{
	      free (*rcvbuf);
	      return NET_ERR;
		}
	}
  }
  return SUCC;
}
/**
 * ip_type :0x01 内网 
 * ip_type :0x02 外网 
 * */
const char * get_ip_ex(const int ip_type )
{
	#define MAXINTERFACES 16
	register int fd, intrface;
	struct ifreq buf[MAXINTERFACES];
	struct ifconf ifc;
	char * tmp_ip;
	int tmp_type;
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0){
		ifc.ifc_len = sizeof buf;
		ifc.ifc_buf = (caddr_t) buf;
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)){
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			while (intrface-- > 0){
				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface]))) {
					tmp_ip=(char*) inet_ntoa( 
							((struct sockaddr_in *) ( &( buf[intrface].ifr_addr)))-> sin_addr);
					if (strncmp(tmp_ip,"192.168",7)==0){
						tmp_type=0x01;
					}else if (strncmp(tmp_ip,"10.",3)==0 ){
						tmp_type=0x01;
					}else if (strncmp(tmp_ip,"127.",4)==0 ){
						tmp_type=0x04;
					}else{//外网
						tmp_type=0x02;
					}
					if ((ip_type & tmp_type)==tmp_type)	{
						close (fd);
						return tmp_ip;
					}	
				}
			}
		}
	}
	close (fd);
	return "";
}

