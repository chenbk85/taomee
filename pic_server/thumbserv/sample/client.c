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

static const char *opt_string = "d:f:h:t:p:i:";
void display_usage()
{
    printf("-d  dir cnt\n");
    printf("-f  file cnt\n");
	printf("-t  thumb cnt\n");
    printf("-i  conn ip\n");
	printf("-p  conn port\n");
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
    memset(buf, 1, sizeof buf);

	int pic_size;
	FILE*		in = fopen("myjpeg.jpg", "rb");
	gdImagePtr 	im = gdImageCreateFromJpeg(in);
	char* 		pic_ptr = gdImageJpegPtr(im, &pic_size, 100);

	int begin_time = time(NULL);
	int lp;
	for (lp = 1; lp < dir_cnt; lp++) {
		int lop;
		for (lop = 1; lop < file_cnt; lop++) {
			int j = sizeof(protocol_t);
			//PKG_H_UINT32(buf, 38815, j);				//key
			PKG_H_UINT32(buf, file_cnt, j);				//key
			PKG_H_UINT32(buf, 1, j);				//type
			PKG_H_UINT32(buf, 1, j);				//type
			PKG_H_UINT32(buf, 800, j);
			PKG_H_UINT32(buf, 600, j);
			PKG_H_UINT32(buf, thumb_cnt, j);				//cnt
			int loop;
			for (loop = 0; loop < thumb_cnt; loop++) {
				PKG_H_UINT32(buf, 0, j);
				PKG_H_UINT32(buf, 0, j);	
				PKG_H_UINT32(buf, 0, j);	
				PKG_H_UINT32(buf, 0, j);	
				PKG_H_UINT32(buf, 100, j);	
				PKG_H_UINT32(buf, 100, j);	
			}

			
			PKG_H_UINT32(buf, pic_size, j);	//len
			PKG_STR(buf, pic_ptr, j, pic_size);
			init_proto_head(buf, 5949501, 1, j);
			int sockfd = connect_to_svr(conn_ip, conn_port);
			if (sockfd == -1)
				exit(-1);
		    write(sockfd, (void*)buf, sizeof buf);
			close(sockfd);
			//if (time(NULL) - begin_time > 5)
				//exit(0);
		}
		sleep (1);
	}

	fclose(in);
	gdFree(pic_ptr);
	gdImageDestroy(im);
	return 0;
/*
    ssize_t n = read(sockfd, buf, 4096);
	write(STDIN_FILENO, buf, n);
	putchar('\n');
	if (n == 7) {
    	read(sockfd, buf, 4096);
    	printf("%s\n", buf);
	}
    read(sockfd, buf, 4096);
    printf("%s\n", buf);
*/
}
