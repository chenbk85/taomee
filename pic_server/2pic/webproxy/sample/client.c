#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <openssl/md5.h>
#include "../../include/proto.h"


int connect_to_svr(const char* ipaddr, uint16_t port)
{
	struct sockaddr_in peer;
	int fd;

	bzero(&peer, sizeof (peer));
	peer.sin_family  = AF_INET;
	peer.sin_port    = htons(port);
	if (inet_pton (AF_INET, ipaddr, &peer.sin_addr) <= 0) {
        	return -1;
    	}
    
    	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    	connect(sockfd, (const struct sockaddr *)&peer, sizeof(peer));

	return sockfd;
}

int main(int argc, char **argv)
{
    	char buf[3*1024*1024];
    	memset(buf, 0, sizeof buf);

	
	int j = sizeof(protocol_t);
	PKG_H_UINT32(buf, 3, j);				//hostid
	PKG_H_UINT32(buf, 2345, j);				//filelen
	PKG_STR(buf, "20110407/0BA39F6FBC665931.JPEG", j, 64);
	PKG_STR(buf, "20110407/239392DA43465BA7.JPEG", j, 256);
	init_proto_head(buf, 5949501, 5001, j);
	int sockfd = connect_to_svr("10.1.1.58", 5001);
	printf("sockfd=%d\n", sockfd);
    	write(sockfd, (void*)buf, j);
	sleep(1);
	close(sockfd);
}
