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

// needed for get_mem recycle_mem
#include "mem_manage.h"

// needed for write_log
#include "log.h"

// needed for write_file
#include "store_data.h"

#include "recv_data.h"

#ifdef		ENCODE_
#include "qdes.h"
#endif

char			des_pass_phrase[PASSWD_LEN] = "TAOMEEV5";
q_node_t 		*file_recv_node[256];//每一个连接对应一个recv_node，最多支持256个客户端发送文件
uint32_t		file_recv_node_len[256];// 保存每一个recv_node的数据长度，recv_node实际对应一个链表头
file_queue_t	file_queue;// 这些node 组织成一个队列 因为每个node其实是收取一个文件 所以命名为file_queue


/*
 * @brief 初始化队列，将256个空闲node全部入队列，node的获取与回收由get_node和recycle_node完成
 * @brief 在initiate()中调用
 */
void file_queue_init()
{
	int i;
	q_node_t *tmp;
	file_queue.head = file_queue.tail = NULL;
	for (i = 0; i < 256; i++) {
		if ((tmp = (q_node_t*)malloc(sizeof(q_node_t))) == NULL) {
			write_log("File_queue init:malloc failed!\n");
			close_log_file();
			exit(-1);
		}
		file_recv_node[i] = tmp;

		file_recv_node[i]->next = NULL;
		file_recv_node[i]->recv_link = NULL;
		file_recv_node[i]->number = i;
		file_recv_node_len[i] = 0;

		if (file_queue.head == NULL)
			file_queue.head = file_queue.tail = file_recv_node[i];
		else {
			file_queue.tail->next = file_recv_node[i];
			file_queue.tail = file_recv_node[i];
		}
	}
}

/*
 * @brief 销毁队列，释放为每个node分配的内存，node拥有的recv_link的内存由recycle_recv_node
 * @brief 调用mem_manage的接口recycle_mem来回收，自身只维护一个指针。
 * @brief 在destroy()中调用
 */
void file_queue_destroy()
{
	int i;
	file_queue.head = file_queue.tail = NULL;
	for (i = 0; i< 256; i++) {
		file_recv_node[i]->next = NULL;
		file_recv_node[i]->recv_link = NULL;	// 他的内存分配与回收都是由mem_manage.c完成的
		file_recv_node[i]->number = 0;
		free(file_recv_node[i]);
		file_recv_node_len[i] = 0;
	}
}

/*
 * @brief 从队列中获取队头的节点，来处理一个客户端发送文件的连接 handler中调用
 * @brief 如果队列为空，则一直等待直到其他线程处理完文件释放了一个node 用pthread_cond_signal唤醒它.
 * @return q_node_t* 一个node类型的节点.
 */
q_node_t *get_recv_node()
{
	q_node_t *tmp_node;
	pthread_mutex_lock(&file_queue_mutex);
	if (file_queue.head == NULL)
		pthread_cond_wait(&file_queue_cond, &file_queue_mutex);
	else {
		tmp_node = file_queue.head;
		if (file_queue.head == file_queue.tail)
			file_queue.head = file_queue.tail =NULL;
		else
			file_queue.head = file_queue.head->next;
	}
	pthread_mutex_unlock(&file_queue_mutex);

	return tmp_node;
}

/*
 * @brief 回收一个node节点 handler使用完毕后调用它
 * @param q_node_t *node	一个待回收的node
 */
void recycle_recv_node(q_node_t *node)
{
	node->next = NULL;
	file_recv_node_len[node->number] = 0;

	store_result_t *tmp,*tmpnext;
	tmpnext = node->recv_link;
	while (tmpnext != NULL) {
		tmp = tmpnext;
		tmpnext=tmpnext->next;
		recycle_mem(tmp);	//回收内存
	}
	node->recv_link = NULL;

	pthread_mutex_lock(&file_queue_mutex);
	if (file_queue.head == NULL)
		file_queue.head = file_queue.tail = node;
	else {
		file_queue.tail->next = node;
		file_queue.tail = node;
	}
	pthread_cond_signal(&file_queue_cond);
	pthread_mutex_unlock(&file_queue_mutex);
}

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
	char buffer[NET_BUFFER_LEN], encrypt_buffer[NET_BUFFER_LEN];
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
	q_node_t *node;

	pthread_detach(pthread_self());

	add_count();
	node = get_recv_node();

	//while recv_len == -1看起来很奇怪，似乎将while变成if更加合理
	//但是如果在recv时候被信号中断等待过程,则需要重新recv 使用while可以在recv到数据或者0的时候立即
	//退出并进一步判断,在recv返回-1的时候判断errno选择是退出还是继续recv
	while ((recv_len = recv(fd, (void*)buffer, sizeof(buffer), 0)) == -1) {
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
			while ((recv_len = recv(fd, (void*)(buffer+total_recv_len),
										sizeof(buffer) - total_recv_len, 0)) == -1) {
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
		des_decrypt_n(des_pass_phrase, (const void*)buffer ,
						(void*)encrypt_buffer, prefix_len/ALIGN_LEN);	//32BYTE/8BYTE
		memcpy(buffer,encrypt_buffer,prefix_len);
#endif

		payload_len = *(int*)(buffer + SYMBOL_LEN + COMMAND_LEN);
		fill_len = *(int*)(buffer + SYMBOL_LEN + COMMAND_LEN + DATA_LEN);
		data_len = prefix_len + payload_len + fill_len + DATA_END_LEN;

		if (data_len > sizeof(buffer)) {
			write_log("data_len[%u] > buffer size[%lu]!!!\
					 please enlarge your buffer size\n",
					 data_len, sizeof(buffer));
			goto ret;
		}

		if ((total_recv_len - prefix_len) < (payload_len + fill_len + DATA_END_LEN)) {
			while ((recv_len = recv(fd, (void*)buffer+total_recv_len,
									(sizeof(buffer) - total_recv_len), 0))
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

		if (memcmp(SYMBOL, buffer, SYMBOL_LEN) != 0) {
			write_log("Bad connection, bad symbol![%s]\n", buffer);
			goto ret;
		}

		if (memcmp(C_GET_LOAD, buffer + SYMBOL_LEN, COMMAND_LEN) == 0) {
			buf_ptr = buffer;
			memcpy(buf_ptr, SYMBOL, SYMBOL_LEN);		//packet symbol
			buf_ptr += SYMBOL_LEN;

			memcpy(buf_ptr, C_SHOW_LOAD, COMMAND_LEN);	//ask server's load command
			buf_ptr += COMMAND_LEN;

			data_len = sizeof(float);
			fill_len = ALIGN_LEN - sizeof(float);
			memcpy(buf_ptr, &data_len, DATA_LEN );
			buf_ptr += DATA_LEN;

			memcpy(buf_ptr, &fill_len, FILL_LEN );
			buf_ptr += FILL_LEN;
			//================getload============
			FILE *result;
			char result_buf[TEXT_LINE_LEN];
			result = popen("awk 'BEGIN{\"uptime\"|getline; print $11;}'","r");
			fread(result_buf,sizeof(char),sizeof(result_buf),result);
			pclose(result);
			float server_load = atof(result_buf);
			//============================
			memcpy(buf_ptr, &server_load, data_len);
			buf_ptr += data_len;
			memset(buf_ptr, 0x0, fill_len);
			buf_ptr += fill_len;

			memcpy(buf_ptr,DATA_END,DATA_END_LEN);
			buf_ptr += DATA_END_LEN;

#ifdef ENCODE_
			des_encrypt_n(des_pass_phrase, (const void*)buffer ,
							(void*)encrypt_buffer, (buf_ptr - buffer)/ALIGN_LEN);	//32BYTE/8BYTE
			memcpy(buffer, encrypt_buffer, sizeof(buffer));
#endif
			//sent to client
			send_len = 0;
			total_send_len = 0;
			data_len = buf_ptr - buffer;
			while ((send_len = send(fd, buffer+total_send_len, (data_len-total_send_len), 0) )
					< (data_len - total_send_len) ) {
				if (send_len == -1) {
					if (errno == EINTR)
						continue;
					write_log("Send load: send error:%s\n", strerror(errno));
					goto ret;
				}
				else
					total_send_len += send_len;
			}
			total_send_len += send_len;
			goto ret;
		}

		if (memcmp(C_SEND_DATA, buffer + SYMBOL_LEN, COMMAND_LEN) == 0) {
			//解析数据体
#ifdef ENCODE_
			des_decrypt_n(des_pass_phrase, (const void*)(buffer+prefix_len),
					(void*)encrypt_buffer, (data_len - prefix_len)/ALIGN_LEN);	//32BYTE/8BYTE
			memcpy(buffer + prefix_len, encrypt_buffer, data_len - prefix_len);
#endif
			buf_ptr = buffer + prefix_len;
			store_result_t *t1;
			store_result_t *tmp;
			int effctive_size = sizeof(store_result_t) - sizeof((store_result_t*)0);
			//===========================================
			while ((buf_ptr - (buffer + prefix_len)) < payload_len) {
				t1 = (store_result_t*)buf_ptr;
				//将这批数据挂在recvlink上
				tmp = get_mem();
				memcpy(tmp, t1, effctive_size);
				tmp->next = NULL;
				buf_ptr += effctive_size;

				if (node->recv_link == NULL)
					node->recv_link = tmp;
				else {
					tmp->next = node->recv_link;
					node->recv_link = tmp;
				}
				file_recv_node_len[node->number]++ ;
			}
			//============================================
			buf_ptr += fill_len;//跳到后续数据标识符

			//若为其他(应该是"DATA_END") 则可以退出，若为CONTINU,则继续接收

			uint32_t packet_len = prefix_len + payload_len + fill_len + DATA_END_LEN;
			//printf("\t\t%u,%u\n",total_recv_len,packet_len);
			if (memcmp(buf_ptr, CONTINUE, DATA_END_LEN) == 0) {
				//有后续数据
				buf_ptr += DATA_END_LEN;
				if ((buf_ptr - buffer) == sizeof(buffer)) {
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
					memcpy(encrypt_buffer, buf_ptr, total_recv_len - packet_len);
					memcpy(buffer, encrypt_buffer, total_recv_len - packet_len);
					recv_len = total_recv_len - packet_len;
					total_recv_len = recv_len;
					continue_flag = 1;
				}
			}
			else
				continue_flag = 0; //no more data transmitted
		}//send data
	}//while continue
	//store_data
	int status = write_file(node->recv_link, file_recv_node_len[node->number]);

	if(status == 0) {
		//echo DATA__OK to client;
		buf_ptr = buffer;
		memcpy(buf_ptr, SYMBOL, SYMBOL_LEN);		//packet symbol
		buf_ptr += SYMBOL_LEN;

		memcpy(buf_ptr, C_DATA_OK, COMMAND_LEN);	//ask server's load command
		buf_ptr += COMMAND_LEN;

		memset(buf_ptr, 0x0, DATA_LEN + FILL_LEN);
		buf_ptr += (DATA_LEN + FILL_LEN);

		memcpy(buf_ptr,DATA_END,DATA_END_LEN);
		buf_ptr += DATA_END_LEN;

#ifdef ENCODE_
		des_encrypt_n(des_pass_phrase, (const void*)buffer ,
				(void*)encrypt_buffer, (buf_ptr - buffer)/ALIGN_LEN);	//32BYTE/8BYTE
		memcpy(buffer,encrypt_buffer,sizeof(buffer));
#endif
		//sent to client
		send_len = 0;
		total_send_len = 0;
		data_len = buf_ptr - buffer;
		while ((send_len = send(fd, buffer+total_send_len, (data_len-total_send_len), 0))
				< (data_len - total_send_len)) {
			if (send_len == -1) {
				if (errno == EINTR)
					continue;
				write_log("Send load: send error:%s\n", strerror(errno));
				goto ret;
			}
			else
				total_send_len += send_len;
		}
		total_send_len += send_len;
	}

ret:

	recycle_recv_node(node);
	sub_count();

	pthread_mutex_lock(&thread_count_mutex);
	pthread_cond_signal(&thread_count_cond);
	pthread_mutex_unlock(&thread_count_mutex);

	close(fd);
	pthread_exit(0);
	return 0;
}
