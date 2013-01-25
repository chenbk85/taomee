/*
 * main.c
 *
 * Created on:	2011-7-27
 * Author:		Singku
 * Paltform:		Linux Fedora Core 8 x86-32
 *	Compiler:		GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "proto.h"

int main(int argc, char **argv)
{
	printf("\t Usage:%s ip proj pageid\n",argv[0]);
	protocol_t proto, *pp;
	int proto_head_len = sizeof(protocol_t);
	char send_buf[1024];
	char recv_buf[1000000],*buf_ptr, *data_start;
	int sockfd;
	struct sockaddr_in server;
	server.sin_addr.s_addr = inet_addr("10.1.1.141");
	if (argc >= 2)
		server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);
	bzero(&(server.sin_zero), sizeof(server.sin_zero));

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("create socket failed\n");
		exit(-1);
	}
	if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
		printf("cant connect to %s\n", inet_ntoa(server.sin_addr));
		close(sockfd);
		exit (-1);
	}
	int prjid = atoi(argv[2]);

	int send_len = 0, total_send_len = 0;
	int recv_len = 0, total_recv_len = 0;
	int data_len;
	//-test idc area
	proto.cmd = 0x3001;proto.id = prjid; proto.len = proto_head_len;proto.seq = 5;proto.ret = 0;
	memcpy(send_buf, &proto, proto_head_len);
	while ((send_len = send(sockfd, send_buf + total_send_len,
				(proto_head_len - total_send_len), 0)) < proto_head_len) {
		if (send_len == -1) {
			if (errno == EINTR)
				continue;
			printf("idc area send error\n");
			close(sockfd);
			exit(-1);
		}
		else
			total_send_len += send_len;
	}
	total_send_len += send_len;

	while ((recv_len = recv(sockfd, recv_buf + total_recv_len,
				sizeof(recv_buf) - total_recv_len, 0)) < proto_head_len) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			printf("idc area recv error\n");
			close(sockfd);
			exit(-1);
		} else if (recv_len == 0) {
			printf("idc area server closed!\n");
			break;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

	pp = (protocol_t *)recv_buf;
	printf("idc area proto head len:%u, cmd:%x, id:%u, seg:%u\n", pp->len, pp->cmd, pp->id, pp->seq);
	if (pp->len <= proto_head_len) {
		printf("idc area received data too short\n");
	}

	while ((recv_len = recv(sockfd, recv_buf + total_recv_len,
				sizeof(recv_buf) - total_recv_len, 0)) < (pp->len - total_recv_len)) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			printf("idc area recv error\n");
			close(sockfd);
			exit(-1);
		} else if (recv_len == 0) {
			printf("idc area server closed!\n");
			break;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

	proto_cache_idc_area_t *idc;
	buf_ptr = recv_buf + proto_head_len;
	data_start = buf_ptr;
	idc = (proto_cache_idc_area_t*)buf_ptr;
	data_len = total_recv_len - proto_head_len;
	while((buf_ptr-data_start) < data_len) {
		printf("province:%u,delay:%u,loss:%u,hop:%u\n",
				idc->province_code,idc->avg_delay,idc->avg_loss,idc->avg_hop);
		idc ++;
		buf_ptr = (char*)idc;
	}

	send_len = 0; total_send_len = 0;
	recv_len = 0; total_recv_len = 0;
	close(sockfd);
	//sleep(2);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("create socket failed\n");
		exit(-1);
	}
	if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
		printf("cant connect to %s\n", inet_ntoa(server.sin_addr));
		close(sockfd);
		exit (-1);
	}

	//-test idc ip
	proto.cmd = 0x3002;proto.id = prjid;proto.len = proto_head_len;proto.seq = 5;proto.ret = 0;
	memcpy(send_buf, &proto, proto_head_len);
	while ((send_len = send(sockfd, send_buf + total_send_len,
				(proto_head_len - total_send_len), 0)) < proto_head_len) {
		if (send_len == -1) {
			if (errno == EINTR)
				continue;
			printf("idc ip send error\n");
			close(sockfd);
			exit(-1);
		}
		else
			total_send_len += send_len;
	}
	total_send_len += send_len;

	while ((recv_len = recv(sockfd, recv_buf + total_recv_len,
				sizeof(recv_buf) - total_recv_len, 0)) < proto_head_len) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			printf("idc recv error\n");
			close(sockfd);
			exit(-1);
		} else if (recv_len == 0) {
			printf("idc server closed!\n");
			break;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

	pp = (protocol_t *)recv_buf;
	printf("idc ip proto head len:%u, cmd:%x, id:%u, seg:%u\n", pp->len, pp->cmd, pp->id, pp->seq);
	if (pp->len <= proto_head_len) {
		printf("idc ip received data too short\n");
	}

	while ((recv_len = recv(sockfd, recv_buf + total_recv_len,
				sizeof(recv_buf) - total_recv_len, 0)) < (pp->len - total_recv_len)) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			printf("idc ip recv error\n");
			close(sockfd);
			exit(-1);
		} else if (recv_len == 0) {
			printf("idc ip server closed!\n");
			break;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

	proto_cache_idc_ip_t *idc_ip;
	buf_ptr = recv_buf + proto_head_len;
	data_start = buf_ptr;
	idc_ip = (proto_cache_idc_ip_t*)buf_ptr;
	data_len = total_recv_len - proto_head_len;
	while((buf_ptr-data_start) < data_len) {
		printf("ip:%u,province:%u,city:%u, delay:%u,loss:%u,hop:%u\n",
				idc_ip->ipvalue,idc_ip->province_code,
				idc_ip->city_code,idc_ip->avg_delay,idc_ip->avg_loss,idc_ip->avg_hop);
		idc_ip ++;
		buf_ptr = (char*)idc_ip;
	}

	//-test cdn area
	send_len = 0; total_send_len = 0;
	recv_len = 0; total_recv_len = 0;
	close(sockfd);
	//sleep(2);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("create socket failed\n");
		exit(-1);
	}
	if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
		printf("cant connect to %s\n", inet_ntoa(server.sin_addr));
		close(sockfd);
		exit (-1);
	}

	proto.cmd = 0x4001;proto.id = prjid;proto.len = proto_head_len;proto.seq = 5;proto.ret = 0;
	memcpy(send_buf, &proto, proto_head_len);
	while ((send_len = send(sockfd, send_buf + total_send_len,
				(proto_head_len - total_send_len), 0)) < proto_head_len) {
		if (send_len == -1) {
			if (errno == EINTR)
				continue;
			printf("cdn send error\n");
			close(sockfd);
			exit(-1);
		}
		else
			total_send_len += send_len;
	}
	total_send_len += send_len;

	while ((recv_len = recv(sockfd, recv_buf + total_recv_len,
				sizeof(recv_buf) - total_recv_len, 0)) < proto_head_len) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			printf("cdn recv error\n");
			close(sockfd);
			exit(-1);
		} else if (recv_len == 0) {
			printf("cdn server closed!\n");
			break;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

	pp = (protocol_t *)recv_buf;
	printf("cdn proto head len:%u, cmd:%x, id:%u, seg:%u\n", pp->len, pp->cmd, pp->id, pp->seq);
	if (pp->len <= proto_head_len) {
		printf("cdn received data too short\n");
	}

	while ((recv_len = recv(sockfd, recv_buf + total_recv_len,
				sizeof(recv_buf) - total_recv_len, 0)) < (pp->len - total_recv_len)) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			printf("cdn recv error\n");
			close(sockfd);
			exit(-1);
		} else if (recv_len == 0) {
			printf("cdn server closed!\n");
			break;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

	proto_cache_cdn_area_t *cdn;
	buf_ptr = recv_buf + proto_head_len;
	data_start = buf_ptr;
	cdn = (proto_cache_cdn_area_t*)buf_ptr;
	data_len = total_recv_len - proto_head_len;
	while((buf_ptr-data_start) < data_len) {
		printf("province:%u,speed:%u\n",
				cdn->province_code,cdn->avg_speed);
		cdn ++;
		buf_ptr = (char*)cdn;
	}


	//-test cdn node
	send_len = 0; total_send_len = 0;
	recv_len = 0; total_recv_len = 0;
	close(sockfd);
	//sleep(2);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("create socket failed\n");
		exit(-1);
	}
	if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
		printf("cant connect to %s\n", inet_ntoa(server.sin_addr));
		close(sockfd);
		exit (-1);
	}

	proto.cmd = 0x4002;proto.id = prjid;proto.len = proto_head_len;proto.seq = 5;proto.ret = 0;
	memcpy(send_buf, &proto, proto_head_len);
	while ((send_len = send(sockfd, send_buf + total_send_len,
				(proto_head_len - total_send_len), 0)) < proto_head_len) {
		if (send_len == -1) {
			if (errno == EINTR)
				continue;
			printf("node send error\n");
			close(sockfd);
			exit(-1);
		}
		else
			total_send_len += send_len;
	}
	total_send_len += send_len;

	while ((recv_len = recv(sockfd, recv_buf + total_recv_len,
				sizeof(recv_buf) - total_recv_len, 0)) < proto_head_len) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			printf("node recv error\n");
			close(sockfd);
			exit(-1);
		} else if (recv_len == 0) {
			printf("idc server closed!\n");
			break;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

	pp = (protocol_t *)recv_buf;
	printf("node proto head len:%u, cmd:%x, id:%u, seg:%u\n", pp->len, pp->cmd, pp->id, pp->seq);
	if (pp->len <= proto_head_len) {
		printf("node received data too short\n");
		goto url;
	}
	while ((recv_len = recv(sockfd, recv_buf + total_recv_len,
				sizeof(recv_buf) - total_recv_len, 0)) < (pp->len - total_recv_len)) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			printf("node recv error\n");
			close(sockfd);
			exit(-1);
		} else if (recv_len == 0) {
			printf("node server closed!\n");
			break;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

	proto_cache_cdn_node_t *node;
	buf_ptr = recv_buf + proto_head_len;
	data_start = buf_ptr;
	node = (proto_cache_cdn_node_t*)buf_ptr;
	data_len = total_recv_len - proto_head_len;
	while((buf_ptr-data_start) < data_len) {
		uint32_t n_addr = htonl(node->node_value * 256);
		printf("province:%u,city:%u,node:%s_%d,speed:%u\n",
				node->province_code, node->city_code, inet_ntoa(*(struct in_addr*)&n_addr),
				node->node_value, node->avg_speed);
		node ++;
		buf_ptr = (char*)node;
	}

url:
	//-test url area
	send_len = 0; total_send_len = 0;
	recv_len = 0; total_recv_len = 0;
	close(sockfd);
	//sleep(2);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("create socket failed\n");
		exit(-1);
	}
	if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
		printf("cant connect to %s\n", inet_ntoa(server.sin_addr));
		close(sockfd);
		exit (-1);
	}
	int pageid = 1;
	if(argc >= 3)
		pageid = atol(argv[3]);
	proto.cmd = 0x5001;proto.id = 2;proto.len = proto_head_len;proto.seq = 5;proto.ret = pageid;
	memcpy(send_buf, &proto, proto_head_len);
	while ((send_len = send(sockfd, send_buf + total_send_len,
				(proto_head_len - total_send_len), 0)) < proto_head_len) {
		if (send_len == -1) {
			if (errno == EINTR)
				continue;
			printf("url send error\n");
			close(sockfd);
			exit(-1);
		} else
			total_send_len += send_len;
	}
	total_send_len += send_len;

	while ((recv_len = recv(sockfd, recv_buf + total_recv_len,
				sizeof(recv_buf) - total_recv_len, 0)) < proto_head_len) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			printf("url recv error\n");
			close(sockfd);
			exit(-1);
		} else if (recv_len == 0) {
			printf("url server closed!\n");
			break;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

	pp = (protocol_t *)recv_buf;
	printf("proto head len:%u, cmd:%x, id:%u, page:%u,seg:%u\n",
			pp->len, pp->cmd, pp->id, pp->ret, pp->seq);
	if (pp->len <= proto_head_len) {
		printf("url received data too short\n");
	}

	while ((recv_len = recv(sockfd, recv_buf + total_recv_len,
				sizeof(recv_buf) - total_recv_len, 0)) < (pp->len - total_recv_len)) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			printf("url recv error\n");
			close(sockfd);
			exit(-1);
		} else if (recv_len == 0) {
			printf("url server closed!\n");
			break;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

	proto_cache_url_area_t *url;
	buf_ptr = recv_buf + proto_head_len;
	data_start = buf_ptr;
	url = (proto_cache_url_area_t*)buf_ptr;
	data_len = total_recv_len - proto_head_len;
	while((buf_ptr-data_start) < data_len) {
		printf("province:%u,spent:%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",url->province_code,
				url->avg_spent[0],url->avg_spent[1],url->avg_spent[2],url->avg_spent[3],url->avg_spent[4]
				,url->avg_spent[5],url->avg_spent[6],url->avg_spent[7],url->avg_spent[8],url->avg_spent[9]);
		url ++;
		buf_ptr = (char*)url;
	}

	return 0;
}


