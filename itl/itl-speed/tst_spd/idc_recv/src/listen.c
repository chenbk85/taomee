/*
 * listen.c 监听逻辑
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

// needed for socket
#include <sys/types.h>
#include <sys/socket.h>

// needed for sockaddr_in
#include <netinet/in.h>

// needed for htons
#include <arpa/inet.h>

// needed for pthread
#include <pthread.h>

// needed for exit
#include <unistd.h>
#include <stdlib.h>

// needed for strerror
#include <string.h>

// needed for errno
#include <errno.h>

// needed for common struct and Macro
#include "protocol.h"

// needed for write_log
#include "log.h"

// needed for handler()
#include "recv_data.h"

// needed for thread control
#include "thread.h"

// needed for timestring
#include "initiate.h"

#include "listen.h"

uint16_t server_conn_port = 7777;
char bind_ip[20]= "192.168.150.20";
/*
 * @brief 监听线程函数
 */
void start_listen()
{
	int serverfd, newfd;
	struct sockaddr_in server, client;
	unsigned int sin_size;

	server.sin_family = AF_INET;
	server.sin_port = htons(server_conn_port);
	server.sin_addr.s_addr = inet_addr(bind_ip);//INADDR_ANY;
	bzero(&(server.sin_zero), sizeof(server.sin_zero));

	if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		write_log("Create socket failed:%s\n", strerror(errno));
		close_log_file();
		exit(-1);
	}

	int f = 1;
	if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &f, sizeof(f)) == -1) {
		write_log("Setsockopt failed:%s\n", strerror(errno));
		close_log_file();
		close(serverfd);
		exit(-1);
	}
	if (bind(serverfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
		write_log("Bind failed:%s\n", strerror(errno));
		close_log_file();
		close(serverfd);
		exit(-1);
	}
	if (listen(serverfd, 1024) == -1) {
		write_log("Listen failed:%s\n", strerror(errno));
		close_log_file();
		close(serverfd);
		exit(-1);
	}
	//infinite loop
	while (1) {
		sin_size = sizeof(struct sockaddr);
		if ((newfd = accept(serverfd, (struct sockaddr*)&client, &sin_size)) == -1) {
			write_log("Accept failed:%s\n", strerror(errno));
			close_log_file();
			exit(-1);
		}
		get_time_string();				//打印日志的时间，写文件的时间,更新
		pthread_mutex_lock(&thread_count_mutex);
		if (thread_count == MAX_THREAD)
			pthread_cond_wait(&thread_count_cond, &thread_count_mutex);
		pthread_mutex_unlock(&thread_count_mutex);

		pthread_t thread;
		if (pthread_create(&thread, NULL, (void*)handler, (void*)newfd) != 0)
			write_log("Create thread failed:%s\n", strerror(errno));
	}
}
