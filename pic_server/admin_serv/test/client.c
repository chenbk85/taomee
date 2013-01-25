#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/resource.h>

#include <gd.h>

#include <openssl/md5.h>
#include "../../include/proto.h"

static char method[32];
static int   conn_port = 6998;
static char conn_ip[32];
static int dir_cnt = 0;
static int file_cnt = 0;
static int thumb_cnt = 0;
static char lloccode[64];
static uint32_t key = 0;
static uint32_t new_key = 0;
static uint32_t action = 0;
int connect_to_svr(const char* ipaddr, uint16_t port)
{
	struct sockaddr_in peer;
	//int fd;

	bzero(&peer, sizeof (peer));
	peer.sin_family  = AF_INET;
	peer.sin_port    = htons(port);
	if (inet_pton (AF_INET, ipaddr, &peer.sin_addr) <= 0) {
        return -1;
    }
    
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == connect(sockfd, (const struct sockaddr *)&peer, sizeof(peer))) {
		printf("connect error\t[%s]\n", strerror(errno));
		return -1;
    }
	printf("ip %s port %u %d",ipaddr,port,sockfd);
	return sockfd;
}

static const char *opt_string = "a:d:f:h:t:p:i:l:k:n:m:";
void display_usage()
{
	printf("-a  action\n");
    printf("-d  dir cnt\n");
    printf("-f  file cnt\n");
	printf("-t  thumb cnt\n");
    printf("-i  conn ip\n");
	printf("-p  conn port\n");
	printf("-l  lloccode\n");
	printf("-k	key\n");
	printf("-n  new_key(for move picture)\n");
    printf("-h  for help\n");
	printf("-m  method\n");
}

int main(int argc, char **argv)
{
	int oc ;
	char * b_opt_arg;
	while( (oc = getopt(argc, argv, opt_string)) != -1) {
		switch( oc ) {
			case 'a':
				b_opt_arg = optarg;
				action = atoi(b_opt_arg);
				printf("action:\t\t%d\n", action);
				break;
			case 'd':
				b_opt_arg = optarg;
				dir_cnt = atoi(b_opt_arg);
				printf("dir cnt:\t\t%d\n", dir_cnt);
				break;
			case 'f':
				b_opt_arg = optarg;
				file_cnt = atoi(b_opt_arg);
				printf("file cnt:\t\t%d\n", file_cnt);
				break;
			case 't':
				b_opt_arg = optarg;
				thumb_cnt = atoi(b_opt_arg);
				printf("thumb cnt:\t\t\t%d\n", thumb_cnt);
				break;
			case 'i':
				b_opt_arg = optarg;
				strcpy(conn_ip, b_opt_arg);
				printf("connect ip:\t\t%s\n", conn_ip);
				break;
			case 'p':
				b_opt_arg = optarg;
				conn_port = strtol(b_opt_arg, NULL, 10);
				printf("connect port:\t\t%d\n", conn_port);
				break;
			case 'h':
				display_usage();
				break;
			case 'l':
				b_opt_arg = optarg;
				strcpy(lloccode, b_opt_arg);
				printf("lloccode:\t\t%s\n", lloccode);
			break;
			case 'k':
				b_opt_arg = optarg;
				key = atoi(b_opt_arg);
				printf("key:\t\t%u\n", key);
			break;

			case 'n':
				b_opt_arg = optarg;
				new_key = atoi(b_opt_arg);
				printf("new:\t\t%u\n", new_key);
				break;

			case 'm':
				b_opt_arg = optarg;
				strcpy(method, b_opt_arg);
				printf("method:\t\t%s strlen[%u]\n", method,strlen(method));
				break;

			default:
				break;
		}
	}
	printf("method %s\n",method);
	if(!strncmp(method,"chomd",5)){
		char buf[1024];
		//	char *buf = "PURGE /t01/fcgi_pic.fcgi?session_key=6b7f92ffccd999b1704bcf407de55f63c0b858b9b8c1bb2d HTTP/1.1\r\nHost:10.1.1.141:8080\r\nConnection: close\r\n\r\n";
		memset(buf, 0, sizeof buf);
		int j = sizeof(protocol_t);
		printf("action:%u\n",action);
		char url[176] = "/p01/fcgi_pic.fcgi?session_key=e89b638fb6b0a4848a7cd0043154d88d95bb0f8d6a7daf0fdc6047bde1d1fb76c0c902b620c19494742480e8472460029e880c3ab660070a9e880c3ab660070a4a64a2a1a05cebae";
		PKG_H_UINT32(buf, action, j);				//action
		PKG_STR(buf,lloccode, j,64);
		PKG_STR(buf,url, j,175);
		init_proto_head(buf, 50006, 3004, j);
		printf("j = %d\n",j);
		//	char *str = "PURGE http://10.1.1.141:8080/t01/fcgi_pic.fcgi?session_key=6b7f92ffccd999b1704bcf407de55f63c0b858b9b8c1bb2d HTTP/1.1\r\n";
		int sockfd = connect_to_svr(conn_ip,conn_port);
		printf("sockfd:%u\n",sockfd);
		if (sockfd == -1)
			exit(-1);
		write(sockfd, (void*)buf, j);
		//   	char buff[4096] = {0};
		ssize_t n = read(sockfd, buf, 4096);
		putchar('\n');
		if (n == 7) {
			read(sockfd, buf, 4096);
			printf("------------\n");
			printf("%s\n", buf);
		}
		//	printf("size recv:%d",n);
		//	printf("buf %s",buff);
		read(sockfd, buf, 4096);
		protocol_t *tmp = (protocol_t *)buf;
		printf("%d\n", tmp->ret);
		close(sockfd);
		return 1;
	}else if(strncmp(method,"purge",strlen("purge")) == 0){
		char *buf = "PURGE /t01/fcgi_pic.fcgi?session_key=6b7f92ffccd999b1704bcf407de55f63c0b858b9b8c1bb2d HTTP/1.1\r\nHost:10.1.1.141:8080\r\nConnection: close\r\n\r\n";
//		char *buf = "url.purge /t01/fcgi_pic.fcgi?session_key=6b7f92ffccd999b1704bcf407de55f63c0b858b9b8c1bb2d";
		int sockfd = connect_to_svr(conn_ip,conn_port);
		printf("sockfd:%u\n",sockfd);
		if (sockfd == -1)
			exit(-1);
		write(sockfd, (void*)buf, strlen(buf));
		char buff[1024] = {0};
		ssize_t n = read(sockfd, buff, 1024);
		putchar('\n');
		if (n == 7) {
			read(sockfd, buff, 1024);
			printf("------------\n");
			printf("%s\n", buff);
		}
		printf("size recv:%d",n);
		printf("buf %s",buff);
		close(sockfd);
		return 1;
	}
	else{
		printf("method error method[%s]\n",method);	
		return 1;
	}
}
