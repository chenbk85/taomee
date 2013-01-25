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
#include "../proto.h"


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
    memset(buf, 1, sizeof buf);

	
	int j = sizeof(protocol_t);
	PKG_H_UINT32(buf, 1, j);    			//key
	PKG_H_UINT32(buf, 2, j);				//type
	PKG_H_UINT32(buf, 1, j);				//branch_type, 1 jpg, 2 png, 3 gif
	PKG_H_UINT32(buf, 0, j);				//cnt
	PKG_H_UINT32(buf, 2*1024*1024, j);	//len
	j = j + 2 * 1024 * 1024;
	init_proto_head(buf, 1, 1, j);
	int sockfd = connect_to_svr("10.1.1.46", 6482);
    write(sockfd, (void*)buf, sizeof buf);

    ssize_t n = read(sockfd, buf, 4096);
	printf("received %u bytes", n);
	int i;
	for (i=0;i<n;i++){
		printf( "%d ", buf[i]);
	}
	//write(STDIN_FILENO, buf, n);
	putchar('\n');
	if (n == 7) {
    	read(sockfd, buf, 4096);
    	printf("%s\n", buf);
	}
    read(sockfd, buf, 4096);
    printf("%s\n", buf);
}
