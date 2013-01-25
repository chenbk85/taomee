/*
 * main.c
 *
 * Created on:	2011-8-9
 * Author:		Singku
 * Paltform:		Linux Fedora Core 8 x86-32
 *	Compiler:		GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "proto.h"

char *filename;
uint32_t server_ip;
uint16_t server_port;
uint16_t send_type;
int actual_data_len;

int connect_2_server(uint32_t serverip, uint16_t port, int *sockfd)
{
	struct sockaddr_in server;
	server.sin_addr.s_addr = htonl(serverip);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	bzero(&(server.sin_zero),sizeof(server.sin_zero));
	printf("connect to ip:%s,port:%u\n",inet_ntoa(server.sin_addr),port);
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("create socket:");
		*sockfd = -1;
		return -1;
	}

	if (connect(*sockfd, (struct sockaddr*)(&server),sizeof(server)) == -1) {
		perror("connect server:");
		*sockfd = -1;
		return -1;
	}

	return 0;
}

static char buffer[8192];//encrypt_buffer[8192];

int send_2_server(fcgi_store_t *result, int sockfd)
{
	char *buf_ptr;

	//sent to server
	int send_len = 0;
	int total_send_len = 0;
	int data_len;

	buf_ptr = buffer;
	if (result->type == proto_fcgi_cdn_report) {
		actual_data_len = 	sizeof(fcgi_common_t) +
								sizeof(fcgi_cdn_t);
		data_len = sizeof(protocol_t) + actual_data_len;
		init_proto_head(buf_ptr, data_len, proto_fcgi_cdn_report, 0, 0, 0);
	}
	else if (result->type == proto_fcgi_url_report) {
		actual_data_len =	sizeof(fcgi_common_t) +
								sizeof(result->record.m_url.count) +
								sizeof(result->record.m_url.page_id) +
								result->record.m_url.count * sizeof(result->record.m_url.vlist[0]);
		data_len = sizeof(protocol_t) + actual_data_len;
		init_proto_head(buf_ptr, data_len, proto_fcgi_url_report, 0, 0, 0);
	}
	else {
		printf("type = %x\n",result->type);
		return 0;
	}

	buf_ptr += sizeof(protocol_t);
	memcpy(buf_ptr, &result->project_number, actual_data_len);

	while ((send_len = send(sockfd, buffer+total_send_len, (data_len-total_send_len), 0) )
			< (data_len - total_send_len) ) {
		if (send_len == -1) {
			if (errno == EINTR)
				continue;
			perror("send 2 server:");
			return -1;
		}
		else
			total_send_len += send_len;
	}
	return 0;
}

int main(int argc, char **argv)
{
	if (argc != 5) {
		printf("\t Usage:%s filename server port send_type(0 meta,1 special)\n", argv[0]);
		return -1;
	}
	filename = argv[1];
	struct in_addr inp;
	if (inet_aton(argv[2], &inp) == 0) {
		perror("invalid server ip:");
		return -1;
	}
	server_ip = ntohl(inp.s_addr);
	server_port = strtoul(argv[3], NULL, 10);

	int filefd = open(filename, O_RDONLY);
	if (filefd == -1) {
		perror("open file:");
		return -1;
	}

	int sockfd;
	if (connect_2_server(server_ip, server_port, &sockfd) == -1)
		return -1;

	int rdlen;
	fcgi_store_t record;
	//actual_data_len = sizeof(fcgi_store_t) - sizeof(record.type) - sizeof(record.special_flag);
	printf("start to send...\n");
	while ((rdlen = read(filefd, &record, sizeof(fcgi_store_t))) >= -1) {
		if (rdlen == -1) {
			if (errno == EINTR)
				perror("read been interrupted:");
			else {
				perror("read error:");
				close(sockfd);
				close(filefd);
				return -1;
			}
		} else if (rdlen < sizeof(fcgi_store_t)) {
			close(sockfd);
			close(filefd);
			remove(filename);
			return -1;
		} else if (rdlen == sizeof(fcgi_store_t)) {
			if (send_2_server(&record, sockfd) == -1) {
				close(sockfd);
				close(filefd);
				return -1;
			}
		}
	}

	return 0;
}
