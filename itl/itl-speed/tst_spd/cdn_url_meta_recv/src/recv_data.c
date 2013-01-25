/*
 * recv_data.c
 *  收包线程,开启服务监听收包端口
 *  每来一个连接，创建一个线程，调用handler函数处理收包业务
 *  因此handler应该是可重入的
 *  每个连接的handler函数对于数据队列是争夺的，因此需要加锁。
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

// needed for popen
#include <stdio.h>

// needed for malloc free
#include <stdlib.h>

// needed for socket operation send recv
#include <sys/types.h>
#include <sys/socket.h>

// needed for memcpm memcpy strerror etc.
#include <string.h>

// needed for errno
#include <errno.h>

// needed for close exit
#include <unistd.h>

// needed for lock unlock
#include <pthread.h>

// needed for communicate protocol on application level
#include "protocol.h"

// needed for timestring
#include "initiate.h"

// needed for mutext and cond
#include "thread.h"

// needed for write_log
#include "log.h"

// needed for write_file
#include "store_data.h"

#include "recv_data.h"

#ifdef		ENCODE_
#include "qdes.h"
#endif

char			des_pass_phrase[PASSWD_LEN] = "TAOMEEV5";

/*
 * @brief 线程计数增加,进入handler中后调用
 */
static inline void add_count()
{
	//==========================
	pthread_mutex_lock(&thread_count_mutex);
	thread_count++;
	pthread_mutex_unlock(&thread_count_mutex);
	//==========================
}
/*
 * @brief 线程计数减少，退出handler时调用
 */
static inline void sub_count()
{
	//==========================
	pthread_mutex_lock(&thread_count_mutex);
	thread_count--;
	pthread_mutex_unlock(&thread_count_mutex);
	//==========================
}

/*
 * @brief listen每次accept一个连接，则创建一个线程收取文件，线程的call_back函数即为handler，参数是accept的fd
 * @param int fd
 */
int handler(int fd)
{
	char *net_buffer, *encrypt_net_buffer;
	char *buf_ptr;

	int recv_len = 0;
	int send_len = 0;
	int total_recv_len = 0;
	int total_send_len = 0;
	int prefix_len = SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN;
	int payload_len = 0;
	int fill_len = 0;
	int data_len = 0;
	int continue_flag = 1;
	char filename[FILE_NAME_LEN];

	pthread_detach(pthread_self());

	net_buffer = (char*)malloc(NET_BUFFER_LEN);
	if (net_buffer == NULL) {
		write_log("net_buffer malloc failed!\n");
		return -1;
	}
	encrypt_net_buffer = (char*)malloc(NET_BUFFER_LEN);
	if (encrypt_net_buffer == NULL) {
		write_log("encrypt net_buffer malloc failed!\n");
		return -1;
	}
	add_count();

	//while recv_len == -1看起来很奇怪，似乎将while变成if更加合理
	//但是如果在recv时候被信号中断等待过程,则需要重新recv 使用while可以在recv到数据或者0的时候立即
	//退出并进一步判断,在recv返回-1的时候判断errno选择是退出还是继续recv
	while ((recv_len = recv(fd, (void*)net_buffer, NET_BUFFER_LEN, 0)) == -1) {
		if (errno == EINTR)
			continue;
		else {
			write_log("Handler,recv:%s\n", strerror(errno));
			goto ret;
			//or just break, and recv_len = -1,program will goto ret at the if condition down there
		}
	}
	if (recv_len == 0) {
		write_log("Recv 0:client_closed %s\n", strerror(errno));
		goto ret;
	}

	total_recv_len = recv_len;
	if (recv_len < prefix_len) {
		//刚开始收数据时，缓冲区是空的，而本fd连头部信息都没有收完全.
		write_log("Bad connection,recv command error![recv_len=%u,prefixlen=%u]\n", recv_len,prefix_len);
		goto ret;
	}

	while (continue_flag) {
		while (total_recv_len < prefix_len) {
			while ((recv_len = recv(fd, (void*)(net_buffer+total_recv_len),
					NET_BUFFER_LEN - total_recv_len, 0)) == -1) {
				if (errno == EINTR)
					continue;
				else {
					write_log("Recv error:%s\n", strerror(errno));
					goto ret;
				}
			}
			total_recv_len += recv_len;
			if (recv_len == 0) {
				write_log("recv 0:%s\n", strerror(errno));
				goto ret;
			}
		}
		//暂时只解析头部
#ifdef ENCODE_
		des_decrypt_n(des_pass_phrase, (const void*)net_buffer ,
						(void*)encrypt_net_buffer, prefix_len/ALIGN_LEN);	//32BYTE/8BYTE
		memcpy(net_buffer,encrypt_net_buffer,prefix_len);
#endif

		payload_len = *(int*)(net_buffer + SYMBOL_LEN + COMMAND_LEN);
		fill_len = *(int*)(net_buffer + SYMBOL_LEN + COMMAND_LEN + DATA_LEN);
		data_len = prefix_len + payload_len + fill_len + DATA_END_LEN;

		if (data_len > NET_BUFFER_LEN) {
			write_log("data_len[%u] > net_buffer size[%u]!!! please enlarge your net_buffer size\n",
					 data_len, NET_BUFFER_LEN);
			goto ret;
		}

		if ((total_recv_len - prefix_len) < (payload_len + fill_len + DATA_END_LEN)) {
			while ((recv_len = recv(fd, (void*)net_buffer+total_recv_len,
									(NET_BUFFER_LEN - total_recv_len), 0))
					< (data_len - total_recv_len)) {
				if (recv_len == -1) {
					if (errno == EINTR)
						continue;
					write_log("Handler,recv:%s\n", strerror(errno));
					goto ret;
				}
				else if (recv_len == 0) {
					write_log("Handler,recv 0 :%s\n", strerror(errno));
					goto ret;
				}
				else
					total_recv_len += recv_len;
			}
			total_recv_len += recv_len;
		}

		if (memcmp(SYMBOL, net_buffer, SYMBOL_LEN) != 0) {
			write_log("Bad connection, bad symbol![%s]\n", net_buffer);
			goto ret;
		}

		if (memcmp(C_SEND_DATA, net_buffer + SYMBOL_LEN, COMMAND_LEN) == 0
			||memcmp(C_WITH_NAME, net_buffer + SYMBOL_LEN, COMMAND_LEN) == 0) {
			//解析数据体
#ifdef ENCODE_
			des_decrypt_n(des_pass_phrase, (const void*)(net_buffer+prefix_len),
					(void*)encrypt_net_buffer, (data_len - prefix_len)/ALIGN_LEN);	//32BYTE/8BYTE
			memcpy(net_buffer + prefix_len, encrypt_net_buffer, data_len - prefix_len);
#endif
			buf_ptr = net_buffer + prefix_len;
			if (memcmp(C_WITH_NAME, net_buffer + SYMBOL_LEN, COMMAND_LEN) == 0) {
				snprintf(filename, sizeof(filename), "%s", net_buffer + prefix_len);
				buf_ptr += FILE_NAME_LEN;
				payload_len -= FILE_NAME_LEN;
			}

			if (write_file(filename, buf_ptr, payload_len) == -1)
				goto ret;
			//============================================
			buf_ptr += (payload_len + fill_len);//跳到后续数据标识符

			//若为其他(应该是"DATA_END") 则可以退出，若为CONTINU,则继续接收

			uint32_t packet_len = prefix_len + payload_len + fill_len + DATA_END_LEN;
			//printf("\t\t%u,%u\n",total_recv_len,packet_len);
			if (memcmp(buf_ptr, CONTINUE, DATA_END_LEN) == 0) {
				//有后续数据
				buf_ptr += DATA_END_LEN;
				if ((buf_ptr - net_buffer) == NET_BUFFER_LEN) {
					//buffer收满了，则重新收
					recv_len = 0;
					total_recv_len =0;
					continue_flag = 1;
				}
				else if (total_recv_len == packet_len) {
					//如果接收到了一个完整的数据分段,则重新接收
					recv_len = 0;
					total_recv_len = 0;
					continue_flag = 1;
				}
				else {
					//如果缓冲区中有后续包的分段,则前移动,因为这些数据没有解密，所以直接移动
					memcpy(encrypt_net_buffer, buf_ptr, total_recv_len - packet_len);
					memcpy(net_buffer, encrypt_net_buffer, total_recv_len - packet_len);
					recv_len = total_recv_len - packet_len;
					total_recv_len = recv_len;
					continue_flag = 1;
				}
			}
			else
				continue_flag = 0; //no more data transmitted
		}//send data
	}//while continue

	//echo DATA__OK to client;
	buf_ptr = net_buffer;
	memcpy(buf_ptr, SYMBOL, SYMBOL_LEN);		//packet symbol
	buf_ptr += SYMBOL_LEN;

	memcpy(buf_ptr, C_DATA_OK, COMMAND_LEN);	//
	buf_ptr += COMMAND_LEN;

	memset(buf_ptr, 0x0, DATA_LEN + FILL_LEN);
	buf_ptr += (DATA_LEN + FILL_LEN);

	memcpy(buf_ptr,DATA_END,DATA_END_LEN);
	buf_ptr += DATA_END_LEN;

#ifdef ENCODE_
	des_encrypt_n(des_pass_phrase, (const void*)net_buffer ,
			(void*)encrypt_net_buffer, (buf_ptr - net_buffer)/ALIGN_LEN);	//32BYTE/8BYTE
	memcpy(net_buffer,encrypt_net_buffer,(buf_ptr - net_buffer));
#endif
	//sent to client
	send_len = 0;
	total_send_len = 0;
	data_len = buf_ptr - net_buffer;
	while ((send_len = send(fd, net_buffer+total_send_len, (data_len-total_send_len), 0))
			< (data_len - total_send_len)) {
		if (send_len == -1) {
			if (errno == EINTR)
				continue;
			write_log("Send DATA_OK: send error:%s\n", strerror(errno));
			goto ret;
		}
		else
			total_send_len += send_len;
	}
	total_send_len += send_len;

ret:
	sub_count();

	pthread_mutex_lock(&thread_count_mutex);
	pthread_cond_signal(&thread_count_cond);
	pthread_mutex_unlock(&thread_count_mutex);

	free(net_buffer);
	free(encrypt_net_buffer);
	close(fd);
	pthread_exit(0);
	return 0;
}
