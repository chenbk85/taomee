/*
 * chooser_server.c 根据初始化得到的IP组，选择一个系统负载比较低的IP传输数据。
 *
 *  Created on:	2011-7-14
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

/*
 * 数据交互
 * 查询负载
 * 8字节标识"TAOMEEV5" 8字节命令"GET_LOAD" 4字节数据长度＋4字节填补=0000+0000 CONTINUE/DATA_END
 * 服务器响应8字节标识"TAOMEEV5" 8字节命令"SHOWLOAD",4字节数据(8)+4字节填补(0) ||负载数值(4字节)+0000 +DATA_END
 * 发送数据
 * 8字节标识"TAOMEEV5" 8字节命令"SENDDATA" 8字节数据长度=4字节数据+4字节填补|| 8*N的数据+补齐+CONTINUE/DATA_END
 *
 * 所以服务器接受两种命令 SHOWLOAD 以及SENDDATA
 *数据包格式 标识＋命令＋数据长度＋补齐长度＋数据		+补齐	+尾部标识
 * 				8	 8		4			4		N*struct	  1-7  CONTINUE/DATA_END
 */

#include <stdlib.h>
#include <fcgi_stdio.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/epoll.h>
#include <errno.h>

#include "initiate.h"
#include "protocol.h"

#include "choose_server.h"

#include "log.h"

#ifdef		ENCODE_
#include "qdes.h"
#endif

uint32_t choose_server()
{
	int 			sockfd;	//遍历服务器的套接字
	uint32_t		best_server=0;
	float			best_server_load=100.0;
	struct			sockaddr_in server;
	char			buffer[1024];
	char			encrypt_buffer[1024];
	int				send_len;
	int				recv_len;
	int				total_send_len;
	int				total_recv_len;
	int				data_len;
	server_ip_t		*ip_ptr;

	ip_ptr=analyze_server_set;
	server.sin_family = AF_INET;
	server.sin_port = htons(server_conn_port);
	bzero(&(server.sin_zero),sizeof(server.sin_zero));

	while (ip_ptr != NULL) {
		memcpy(buffer, SYMBOL, SYMBOL_LEN);//packet symbol
		memcpy(buffer + SYMBOL_LEN, C_GET_LOAD ,COMMAND_LEN);	//ask server's load command
		memset(buffer + SYMBOL_LEN + COMMAND_LEN, 0x0, DATA_LEN + FILL_LEN);//无后续数据
		memcpy(buffer + SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN, DATA_END, DATA_END_LEN);

#ifdef ENCODE_
		des_encrypt_n(
			des_pass_phrase,
			(const void*)buffer ,
			(void*)encrypt_buffer,
			(SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN + DATA_END_LEN)/8);//24BYTE/8BYTE
		memcpy(buffer, encrypt_buffer, sizeof(buffer));
#endif
		server.sin_addr.s_addr=htonl(ip_ptr->ipvalue);

		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			write_log("choose server:create socket failed!%s\n", strerror(errno));
			goto next_;
		}
		if (-1 == fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK)) {
			write_log("choose server:fcntl error!%s\n", strerror(errno));
			goto next_sockfd;
		}
		int epfd;
		struct epoll_event ev;
		ev.events = EPOLLOUT;
		ev.data.fd = sockfd;
		if ((epfd = epoll_create(1)) == -1) {
			write_log("choose server:create epoll_fd failed!%s\n", strerror(errno));
			goto next_sockfd;
		}

		if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev)) {
			write_log("choose server:epoll_ctl error!%s\n", strerror(errno));
			goto next_epfd;
		}

		connect(sockfd, (struct sockaddr*)(&server), sizeof(server));

		int nfds;
		struct epoll_event events[128];
		if ((nfds = epoll_wait(epfd, events, 128, time_out)) == -1) {
			write_log("choose server:epoll_ctl error!%s\n", strerror(errno));
			goto next_epfd;
		}

		if (nfds == 0) {
			write_log("choose server %s:epoll timeout!\n", inet_ntoa(server.sin_addr));
			if (-1 == epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev)) {
				write_log("choose server:epoll_ctl error!%s\n", strerror(errno));
				goto next_epfd;
			}

			goto next_epfd;
		}

		int i;
		for (i = 0 ; i < nfds; i++) {
			if (-1 == epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev)) {
				write_log("choose server:epoll_ctl error!%s\n", strerror(errno));
				break;//break for
			}

			if (events[i].data.fd == sockfd) {
				if (!(events[i].events & EPOLLOUT))
					continue;

				if (-1 == fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) ^ O_NONBLOCK)) {//blocking
					write_log("choose server:fcntl error!%s\n", strerror(errno));
					break;//break for
				}
				send_len = 0;
				total_send_len = 0;
				data_len = SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN + DATA_END_LEN;
				while ((send_len = send(sockfd, buffer+total_send_len, (data_len-total_send_len), 0))
						< (data_len - total_send_len)) {
					if (send_len == -1) {
						if (errno == EINTR)
							continue;
						write_log("choose server: send error:%s\n", strerror(errno));
						goto next_epfd;
					}
					else
						total_send_len += send_len;
				}
				total_send_len += send_len;

				recv_len = 0;
				total_recv_len = 0;
				data_len = SYMBOL_LEN
						+ COMMAND_LEN
						+ DATA_LEN + FILL_LEN
						+ sizeof(float)
						+ (8 - sizeof(float))
						+ DATA_END_LEN; //32

				while ((recv_len =
						recv(sockfd, (void*)buffer+total_recv_len, (sizeof(buffer) - total_recv_len), 0))
						< (data_len - total_recv_len)) {
					if (recv_len == -1) {
						if (errno == EINTR)
							continue;
						write_log("choose server: recv error:%s\n", strerror(errno));
						goto next_epfd;;
					}
					else if (recv_len == 0) {
						write_log("choose server: recv error: server_closed %s\n", strerror(errno));
						goto next_epfd;
					}
					else
						total_recv_len += recv_len;
				}
				total_recv_len += recv_len;

#ifdef ENCODE_
				des_decrypt_n(des_pass_phrase, (const void*)buffer ,
						(void*)encrypt_buffer, (data_len)/8);	//32BYTE/8BYTE
				memcpy(buffer,encrypt_buffer,sizeof(buffer));
#endif
				//server should reply "TAOMEEV5SHOWLOAD"+4B+0000+float+0000 + "DATA_END"
				if( memcmp(SYMBOL, buffer, SYMBOL_LEN) == 0
					&& memcmp(C_SHOW_LOAD, buffer + SYMBOL_LEN, COMMAND_LEN ) == 0) {
						float tmp=*((float*)(buffer + SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN));
						best_server = tmp < best_server_load ?ip_ptr->ipvalue :best_server;
						best_server_load = tmp < best_server_load ?tmp :best_server_load ;
				}
				else
					write_log("chooser server: get load: bad respose:%s\n", buffer);
			}
		}

next_epfd:
		close(epfd);
next_sockfd:
		close(sockfd);
next_:
		ip_ptr=ip_ptr->next;
	}
	return best_server;
}
