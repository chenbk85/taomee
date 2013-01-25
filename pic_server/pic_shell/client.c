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


#include "proto.h"


static int   conn_port = 6998;
static char conn_ip[32] = "10.1.1.58";
static int dir_cnt = 0;
static int file_cnt = 0;
static int thumb_cnt = 0;
static char lloccode[32][65];
static uint32_t key = 0;
static uint32_t new_key = 0;
static uint32_t lloccode_cnt = 0;
static uint32_t cmdid = 0;
static uint32_t userid = 0;
static uint32_t DEBUG = 0;

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
		TEST_DEBUG_LOG("connect error\t[%s]", strerror(errno));
		return -1;
    }

	return sockfd;
}

static const char *opt_string = "d:f:ht:p:i:l:k:n:m:c:u:D";
void display_usage()
{
    TEST_DEBUG_LOG("-d  dir cnt\n");
    TEST_DEBUG_LOG("-f  file cnt\n");
	TEST_DEBUG_LOG("-t  thumb cnt\n");
    TEST_DEBUG_LOG("-i  conn ip\n");
	TEST_DEBUG_LOG("-p  conn port\n");
	TEST_DEBUG_LOG("-l  lloccode\n");
	TEST_DEBUG_LOG("-k	key\n");
	TEST_DEBUG_LOG("-n  new_key(for move picture)\n");
	TEST_DEBUG_LOG("-m  method\n");
	TEST_DEBUG_LOG("-c  file_cnt\n");
	TEST_DEBUG_LOG("-u	userid\n");
    TEST_DEBUG_LOG("-h  for help\n");
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
				//TEST_DEBUG_LOG("dir cnt:\t\t%d\n", dir_cnt);
				TEST_DEBUG_LOG("dir cnt:\t\t%d\n", dir_cnt);
				break;
			
			case 'D':
				DEBUG = 1;
				break;

			case 'm':
				b_opt_arg = optarg;
				cmdid = atoi(b_opt_arg);
				TEST_DEBUG_LOG("cmdid:\t\t%d\n", cmdid);
				break;
			case 'u':
				b_opt_arg = optarg;
				userid = atoi(b_opt_arg);
				TEST_DEBUG_LOG("userid:\t\t%d\n", userid);
				break;

			case 'f':
				b_opt_arg = optarg;
				file_cnt = atoi(b_opt_arg);
				TEST_DEBUG_LOG("file cnt:\t\t%d\n", file_cnt);
				break;
			case 't':
				b_opt_arg = optarg;
				thumb_cnt = atoi(b_opt_arg);
				TEST_DEBUG_LOG("thumb cnt:\t\t\t%d\n", thumb_cnt);
				break;
			case 'i':
				b_opt_arg = optarg;
				strcpy(conn_ip, b_opt_arg);
				TEST_DEBUG_LOG("connect ip:\t\t%s\n", conn_ip);
				break;
			case 'p':
				b_opt_arg = optarg;
				conn_port = strtol(b_opt_arg, NULL, 10);
				TEST_DEBUG_LOG("connect port:\t\t%d\n", conn_port);
				break;
			case 'h':
				display_usage();
				break;
			case 'l':
				b_opt_arg = optarg;
				strcpy(lloccode[lloccode_cnt], b_opt_arg);
				TEST_DEBUG_LOG("lloccode[%u]:\t\t%s\n", lloccode_cnt,lloccode[lloccode_cnt]);
				lloccode_cnt ++;
			break;
			case 'k':
				b_opt_arg = optarg;
				key = atoi(b_opt_arg);
				TEST_DEBUG_LOG("key:\t\t%u\n", key);
			break;

			case 'n':
				b_opt_arg = optarg;
				new_key = atoi(b_opt_arg);
				TEST_DEBUG_LOG("new:\t\t%u\n", new_key);
			break;

			default:
				break;
		}
	}

    char buf[3*1024*1024];
    memset(buf, 0, sizeof buf);
	int j = sizeof(protocol_t);
	int k =0;
	if(cmdid == 3003){
		PKG_H_UINT32(buf, key, j);				//key
		PKG_H_UINT32(buf, new_key, j);
		PKG_H_UINT32(buf,file_cnt,j);
		for(k =0;k<file_cnt;k++){
			PKG_STR(buf,lloccode[k], j,64);
		}
	
		init_proto_head(buf, 50006, 3003, j);
		TEST_DEBUG_LOG("j=%u\n",j);
		int sockfd = connect_to_svr(conn_ip, conn_port);
		TEST_DEBUG_LOG("sockfd:%d",sockfd);
		if (sockfd == -1)
		exit(-1);
		write(sockfd, (void*)buf, j);

    	ssize_t n = read(sockfd, buf, 4096);
		uint32_t recv_key = 0;
		char new_lloccode[32][65];
		uint32_t Thumb_cnt = 0;
		uint32_t Thumbid[4];
		uint32_t f_cnt = 0;
		uint32_t ret = 0;
		putchar('\n');
		if (n == 7) {
    		read(sockfd, buf, 4096);
			TEST_DEBUG_LOG("------------\n");
    		TEST_DEBUG_LOG("%s\n", buf);
		}
    	read(sockfd, buf, 4096);
		protocol_t *tmp = (protocol_t *)buf;
    	TEST_DEBUG_LOG("%d\n", tmp->ret);
		if(tmp->ret != 0){
			return 1;
		}
		j = sizeof(protocol_t);
		UNPKG_H_UINT32(buf,recv_key,j);
		UNPKG_H_UINT32(buf,f_cnt,j);
		TEST_DEBUG_LOG("[%u %u]\n",recv_key,f_cnt);
		for(k=0;k<f_cnt;k++){
			UNPKG_STR(buf,new_lloccode[k],j,64);
			UNPKG_H_UINT32(buf,ret,j);
			TEST_DEBUG_LOG("[%u][%s %u]\n",k,new_lloccode[k],ret);
			UNPKG_H_UINT32(buf,Thumb_cnt,j);
			int lp = 0;
			for(lp = 0; lp < Thumb_cnt; lp ++){
				UNPKG_H_UINT32(buf,Thumbid[lp],j);
				TEST_DEBUG_LOG("Thumbid[%u]:%u\n",lp,Thumbid[lp]);
			}
		}
		close(sockfd);
	}else if(cmdid == 3002){	
		PKG_H_UINT32(buf, key, j);				//key
		PKG_H_UINT32(buf,file_cnt,j);
		for(k =0;k<file_cnt;k++){
			PKG_STR(buf,lloccode[k], j,64);
		}
		init_proto_head(buf, 50006, 3002, j);
		TEST_DEBUG_LOG("del j=%u\n",j);
		int sockfd = connect_to_svr(conn_ip, conn_port);
		TEST_DEBUG_LOG("sockfd:%u",sockfd);
		if (sockfd == -1)
		exit(-1);
		write(sockfd, (void*)buf, j);
    	ssize_t n = read(sockfd, buf, 4096);
		putchar('\n');
		if (n == 7) {
    		read(sockfd, buf, 4096);
			TEST_DEBUG_LOG("------------\n");
    		TEST_DEBUG_LOG("%s\n", buf);
		}
    	read(sockfd, buf, 4096);
		protocol_t *tmp = (protocol_t *)buf;
    	TEST_DEBUG_LOG("%d\n", tmp->ret);
		close(sockfd);
	}else if(cmdid == 1001){
		uint32_t recv_key = 0;
		init_proto_head(buf, userid, cmdid, j);
		TEST_DEBUG_LOG("del j=%u\n",j);
		int sockfd = connect_to_svr(conn_ip, conn_port);
		TEST_DEBUG_LOG("sockfd:%u",sockfd);
		if (sockfd == -1)
			exit(-1);
		write(sockfd, (void*)buf, j);
		ssize_t n = read(sockfd, buf, 4096);
		if (n == 7) {
			read(sockfd, buf, 4096);
			TEST_DEBUG_LOG("------------\n");
			TEST_DEBUG_LOG("%s\n", buf);
		}
		read(sockfd, buf, 4096);
		protocol_t *tmp = (protocol_t *)buf;
		TEST_DEBUG_LOG("%d\n", tmp->ret);
		if(tmp->ret != 0){
			return 1;					        
		}
		j = sizeof(protocol_t);
		UNPKG_H_UINT32(buf,recv_key,j);
		printf("%u\t%u\t%u\n",userid,key,recv_key);
		close(sockfd);
	}else{
		printf("cmdid[%u]",cmdid);
	}
	return 1;
}
