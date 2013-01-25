/*
 * ddns.c 解析DNS成IP地址，以更新服务器列表
 *
 *  Created on:	2011-7-6
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

// needed for malloc
#include <stdlib.h>

// needed for ntohl
#include <arpa/inet.h>

// needed for getaddrinfo gai_strerror freeaddrinfo
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

// needed for pthread_exit when malloc failed,
// this function actually called in data_send thread
#include <pthread.h>

// needed for common struct of "server_ip_t"
#include "initiate.h"

#include "log.h"
#include "ddns.h"

/*
 * @brief 解析域名
 * @param const char *domainname:DNS名称
 */
int dns_resolve(const char *domainname)
{
	struct addrinfo 		*ailist, *aip;
	struct addrinfo 		hint;
	struct sockaddr_in		*sinp;
	int						err;
	server_ip_t				*tmpip;

	hint.ai_flags = AI_CANONNAME;		//need a canononic name instead of an alias
	hint.ai_family = AF_INET;			//IPV4
	hint.ai_socktype = SOCK_STREAM;		//TCP
	hint.ai_protocol = 6;					//tcp
	hint.ai_addrlen = 0;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;

	if (( err=getaddrinfo(domainname, NULL, &hint, &ailist)) != 0) {
		write_log("Ddns:%s\n", gai_strerror(err));
		return -1;
	}

	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		if (aip->ai_family == AF_INET) {
			sinp=(struct sockaddr_in*)aip->ai_addr;

			if ((tmpip=(server_ip_t*)malloc(sizeof(server_ip_t))) == NULL) {
				write_log("Ddns:malloc failed!\n");
				return -1;
			}
			tmpip->ipvalue = ntohl(sinp->sin_addr.s_addr);
			tmpip->next = NULL;
			if (analyze_server_set != NULL) {
				tmpip->next=analyze_server_set;
				analyze_server_set=tmpip;
			}
			else
				analyze_server_set=tmpip;
		}
	}
	freeaddrinfo(ailist);
	return 0;
}
