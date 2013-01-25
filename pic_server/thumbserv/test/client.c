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


static int   conn_port = 6999;
static char conn_ip[32];
static int dir_cnt = 0;
static int file_cnt = 0;
static int thumb_cnt = 0;
static char lloccode[64];
static uint32_t key = 0;
static uint32_t new_key = 0;
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
		printf("connect error\t[%s]", strerror(errno));
		return -1;
    }

	return sockfd;
}

static const char *opt_string = "d:f:h:t:p:i:l:k:n:";
void display_usage()
{
    printf("-d  dir cnt\n");
    printf("-f  file cnt\n");
	printf("-t  thumb cnt\n");
    printf("-i  conn ip\n");
	printf("-p  conn port\n");
	printf("-l  lloccode\n");
	printf("-k	key\n");
	printf("-n  new_key(for move picture)\n");
    printf("-h  for help\n");
}

int main(int argc, char **argv)
{
	int oc ;
	char * b_opt_arg;
	while( (oc = getopt(argc, argv, opt_string)) != -1) {
		switch( oc ) {
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

			default:
				break;
		}
	}

	struct rlimit rlim;

	/* raise open files */
	rlim.rlim_cur = 20000;
	rlim.rlim_max = 20000;
	if (setrlimit(RLIMIT_NOFILE, &rlim) == -1) {
		printf("INIT FD RESOURCE FAILED");
	}

    char buf[3*1024*1024];
    memset(buf, 0, sizeof buf);
#if 0
	int pic_size;
	FILE*		in = fopen("myjpeg.jpg", "rb");
	gdImagePtr 	im = gdImageCreateFromJpeg(in);
	char* 		pic_ptr = gdImageJpegPtr(im, &pic_size, 100);
	int begin_time = time(NULL);
	int lp;

#endif
//	for (lp = 1; lp < dir_cnt; lp++) {
//		int lop;
//		for (lop = 1; lop < file_cnt; lop++) {
	int j = sizeof(protocol_t);
	PKG_H_UINT32(buf, key, j);				//key
	PKG_H_UINT32(buf, new_key, j);	
	PKG_STR(buf,lloccode, j,64);
	init_proto_head(buf, 50006, 3003, j);
	printf("j=%u\n",j);
	int sockfd = connect_to_svr(conn_ip, conn_port);
	printf("sockfd:%u",sockfd);
	if (sockfd == -1)
		exit(-1);
	write(sockfd, (void*)buf, j);
//	close(sockfd);
			//if (time(NULL) - begin_time > 5)
				//exit(0);
//		}
//	}
#if 0
	fclose(in);
	gdFree(pic_ptr);
	gdImageDestroy(im);
#endif	

    ssize_t n = read(sockfd, buf, 4096);
	uint32_t recv_key = 0;
	char new_lloccode[65];
	uint32_t Thumb_cnt = 0;
	uint32_t Thumbid[4];
//	write(STDIN_FILENO, buf, n);
	putchar('\n');
	if (n == 7) {
    	read(sockfd, buf, 4096);
		printf("------------\n");
    	printf("%s\n", buf);
	}
    read(sockfd, buf, 4096);
	protocol_t *tmp = (protocol_t *)buf;
    printf("%d\n", tmp->ret);
	j = sizeof(protocol_t);
	UNPKG_H_UINT32(buf,recv_key,j);
	UNPKG_STR(buf,new_lloccode,j,64);
	UNPKG_H_UINT32(buf,Thumb_cnt,j);
	printf("[%u %s %u]\n",recv_key,new_lloccode,Thumb_cnt);
	int lp = 0;
	for(lp = 0; lp < Thumb_cnt; lp ++){
		UNPKG_H_UINT32(buf,Thumbid[lp],j);
		printf("Thumbid[%u]:%u\n",lp,Thumbid[lp]);
	}

	close(sockfd);
	return 1;
}
