/*
 * send.c 轮询目录并发送数据到选定的服务器上
 *
 *  Created on:	2011-7-5
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

// needed for file io operation
#include <stdio.h>

// needed for malloc()
#include <stdlib.h>

// needed for close() sleep()..
#include <unistd.h>

// needed for mkdir, socket
#include <sys/types.h>
#include <sys/stat.h>

// needed for dir operation
#include <dirent.h>

// needed for string operation and memcpy ,memcmp etc..
#include <string.h>

// needed for pthread_exit when fatal error occurred
#include <pthread.h>

// needed for fcntl() when adjust socket model
#include <fcntl.h>

// needed for socket
#include <sys/socket.h>

// needed for htons ntohl inet_xtox
#include <arpa/inet.h>

// needed for sockaddr_in
#include <netinet/in.h>

// needed for epoll operation
#include <sys/epoll.h>

// needed for time() when modify file head
#include <time.h>

// needed for errno
#include <errno.h>

// needed for common struct
#include "common.h"

// needed for communicate protocol
#include "proto.h"

// needed for log
#include "log.h"


// needed for send_load_config
#include "init_and_destroy.h"

#include "send.h"

#ifdef		ENCODE_
#include "qdes.h"
#endif


uint32_t accept_addr = 167838093;	//转发目的地址 10.1.1.141
uint16_t accept_port = 7777;		//转发目的端口
char		des_pass_phrase[PASSWD_LEN] = "TAOMEEV5";	//DES密码

/*
 * @brief 文件在发送的时候会将内容读入一个链表，发送完毕后需要释放链表
 * @param 结果链表的头部
 */
static void free_send_link(store_result_t *head)
{
	store_result_t *tmp,*tmpnext;
	tmpnext = head;
	while (tmpnext != NULL) {
		tmp = tmpnext;
		tmpnext = tmpnext->next;
		free(tmp);
	}
}


/*
 * @brief 将某个文件发送到服务器
 * @param const char *send_file:带发送的文件名指针
 * @return 0 表示发送成功，1表示非法文件, -1表示发送失败
 */
static int send_2_server(const char *send_file)
{
	int		        	sockfd;					//套接字
	struct				sockaddr_in	server;	//服务器
	file_header_t	file_head;				//文件头
	FILE 				*fp;						//文件打开指针
	int					read_count = 0;			//读出的记录数
	int					write_count = 0;		//写入buffer中的结构数
	int					read_len;					//实际从文件中读出的字节长度
	int					send_len;					//tcp实际发送的字节长度
	int					total_send_len;			//tcp总共发送了多少字节
	char				buffer[NET_BUFFER_LEN];			//数据缓冲区
	char				encrypt_buffer[NET_BUFFER_LEN];	//加密后的缓冲区
	char				*buf_ptr;					//缓冲区的剩余空间指针
	store_result_t	tmp_result;				//临时存放一个数据
	store_result_t	*result_ptr;				//指向数据的指针
	store_result_t	*tmp_result_ptr;		//保存result_ptr指针
	int					effective_size;			//结构体中的有效数据长度,需要减去next指针的大小
	store_result_t	*send_link=NULL;		//数据发送链，处理完毕后写回文件

	fp=fopen(send_file, "rb");
	if (fp == NULL) {
		write_log("Can not open file:%s!!!\n", send_file);
		return 1;
	}

	if ((read_len=fread(&file_head, sizeof(file_header_t), 1, fp)) != 1 ) {
		write_log("File not complete:%s!!!\n", send_file);
		fclose(fp);
		return 1;//file not complete;
	}
	if (memcmp(file_head.symbol,FILE_HEAD_SYMBOL, F_SYMBOL_LEN) != 0) {
		write_log("%s is not a valid velocity test file!!!\n", send_file);
		fclose(fp);
		return 1;//not a valid velocity test file;
	}
	if (file_head.processed_record == file_head.total_record) {
		write_log("%s has already been send!!!\n", send_file);
		fclose(fp);
		return 0; //no need to handle this file
	}

	effective_size = sizeof(store_result_t) - sizeof(store_result_t*);//丢掉next指针的长度 这里是36字节
	while (!feof(fp)) {
		read_len = fread(&tmp_result, effective_size, 1, fp);

		if (read_len != 1)
			break;
		tmp_result.next = NULL;

		read_count++;	//记录数加1

		if ((result_ptr = (store_result_t*)malloc(sizeof(store_result_t))) == NULL) {
			write_log("Send_2_server:malloc failed!!\n");
			fclose(fp);
			exit(-1);
		}

		memcpy(result_ptr, &tmp_result, sizeof(store_result_t));

		if (send_link == NULL)
			send_link = result_ptr;
		else {
			result_ptr->next=send_link;
			send_link=result_ptr;
		}
	}//while
	fclose(fp);
	if (read_count != file_head.total_record) {
		write_log("%s:file record not equal total_record field!!!\n", send_file);
		free_send_link(send_link);
		return 1;		//读到的记录数与总记录数不相等
	}
	if (read_count == 0 || send_link == NULL) {
		write_log("%s:file record==0!\n", send_file);
		free_send_link(send_link);
		return 1;		//没有读到记录
	}


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		write_log("Send file:create socket failed!%s\n", strerror(errno));
		free_send_link(send_link);
		return -1;
	}
	server.sin_addr.s_addr = htonl(accept_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(accept_port);
	bzero(&(server.sin_zero), sizeof(server.sin_zero));
	if (connect(sockfd, (struct sockaddr*)(&server), sizeof(server))  == -1) {
		char buf[INET_ADDRSTRLEN];
		write_log("Send file:connetct to server %s failed!%s\n",
				inet_ntop(AF_INET, &(server.sin_addr), buf, INET_ADDRSTRLEN),
				strerror(errno));
		close(sockfd);
		free_send_link(send_link);
		return -1;
	}

	result_ptr = send_link;
	//tmp_result_ptr = result_ptr;
	int buffer_full_flag = 1;//等于1表示缓冲区已经满了，已经发送，需要重新建立头部信息
	int	 buffer_remain_flag = 0;//buffer里面还有没有数据 默认没了
	while (result_ptr != NULL) {
		if (result_ptr->processed) {
			result_ptr = result_ptr->next;
			continue;
		}
		if (buffer_full_flag == 1) {
			//重建buffer的头部
			buf_ptr = buffer;
			buffer_full_flag = 0;
			write_count = 0;
			//buffer的前16个字节是"TAOMEEV5SENDDATA",接着8个字节是数据长度
			memcpy(buf_ptr, SYMBOL, P_SYMBOL_LEN);
			buf_ptr += P_SYMBOL_LEN;

			memcpy(buf_ptr, C_SEND_DATA, COMMAND_LEN);
			buf_ptr += COMMAND_LEN;

			buf_ptr += (DATA_LEN + FILL_LEN);		//跳过数据长度部分
		}

		if ( (buffer + sizeof(buffer) - buf_ptr) >= (effective_size + DATA_END_LEN) )	{	//如果buffer的剩余空间还可以容纳一个结构

			memcpy(buf_ptr, result_ptr, effective_size);
			buf_ptr += effective_size;
			write_count++;
			buffer_remain_flag = 1;
		}
		else {	//已经容纳不下更多数据，填写实际的数据长度(8字节),加密缓冲区并发送缓冲区内容
			uint32_t len = write_count * effective_size;	//写入的数据长度
			memcpy(buffer + P_SYMBOL_LEN + COMMAND_LEN, &len, DATA_LEN );
			uint32_t fill_len = ALIGN_LEN - len%ALIGN_LEN;	//需要补齐的字节数
			memcpy(buffer + P_SYMBOL_LEN + COMMAND_LEN + DATA_LEN, &fill_len, FILL_LEN);
			memset(buf_ptr, 0x0, fill_len );//保证是8字节的整数倍
			buf_ptr += fill_len;
			//进入到else说明还后后续数据
			memcpy(buf_ptr, CONTINUE, DATA_END_LEN);
			buf_ptr += DATA_END_LEN;			//buf_ptr指向整个数据的末端

#ifdef		ENCODE_
			des_encrypt_n(des_pass_phrase, (const void*)buffer , (void*)encrypt_buffer, (buf_ptr - buffer)/ALIGN_LEN);
			memcpy(buffer, encrypt_buffer, buf_ptr - buffer);
#endif
			//sent to server
			send_len = 0;
			total_send_len = 0;
			int data_len = buf_ptr - buffer;
			while ((send_len = send(sockfd, buffer+total_send_len, (data_len-total_send_len), 0) ) < (data_len - total_send_len)) {
				if (send_len == -1 ) {
					if (errno == EINTR)
						continue;
					write_log("Send file: send error:%s\n", strerror(errno));
					close(sockfd);
					free_send_link(send_link);
					return -1;
				}
				else
					total_send_len += send_len;
			}
			total_send_len += send_len;

			buffer_full_flag = 1;
			buffer_remain_flag = 0;	//buffer里面没数据了

			continue;
		}//else

		result_ptr=result_ptr->next;
	}//while

	if (buffer_remain_flag == 1) {//结束while循环的条件有多种,结束后Buffer中可能还有数据
		uint32_t len = write_count * effective_size;	//写入的数据长度
		memcpy(buffer + P_SYMBOL_LEN + COMMAND_LEN, &len, DATA_LEN );
		uint32_t fill_len = ALIGN_LEN - len%ALIGN_LEN;	//需要补齐的字节数
		memcpy(buffer + P_SYMBOL_LEN + COMMAND_LEN + DATA_LEN, &fill_len, FILL_LEN);
		memset(buf_ptr, 0x0, fill_len );//保证是8字节的整数倍
		buf_ptr += fill_len;		//buf_ptr指向整个数据的末端
		//到这一步，说明已经没有后续数据了，可以结束发送,DATA_END字段全置"DATA_END"
		memcpy(buf_ptr,DATA_END,DATA_END_LEN);
		buf_ptr += DATA_END_LEN;			//buf_ptr指向整个数据的末端

#ifdef		ENCODE_
		des_encrypt_n(des_pass_phrase, (const void*)buffer , (void*)encrypt_buffer, (buf_ptr - buffer)/ALIGN_LEN);
		memcpy(buffer, encrypt_buffer, buf_ptr - buffer);
#endif
		//sent to server
		send_len = 0;
		total_send_len = 0;
		int data_len = buf_ptr - buffer;
		while ((send_len=send(sockfd, buffer+total_send_len, (data_len-total_send_len), 0) ) < (data_len - total_send_len)) {
			if (send_len == -1) {
				if (errno == EINTR)
					continue;
				write_log("Send file: send error:%s\n", strerror(errno));
				close(sockfd);
				free_send_link(send_link);
				return -1;
			}
			else
				total_send_len += send_len;
		}
		total_send_len += send_len;
	}

	// wait for server's response
	int recv_len = 0;
	int total_recv_len = 0;
	int data_len = P_SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN + DATA_END_LEN; //32
	while ((recv_len = recv(sockfd, (void*)buffer+total_recv_len, (sizeof(buffer) - total_recv_len), 0)) < (data_len - total_recv_len)) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			write_log("Send file:receive server response error:%s\n", strerror(errno));
			close(sockfd);
			free_send_link(send_link);
			return -1;
		}
		else if (recv_len == 0) {
			write_log("Send file:receive server response error:%s\n", strerror(errno));
			close(sockfd);
			free_send_link(send_link);
			return -1;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

#ifdef		ENCODE_
	des_decrypt_n(des_pass_phrase, (const void*)buffer, (void*)encrypt_buffer, (data_len)/ALIGN_LEN);	//32BYTE/8BYTE
	memcpy(buffer, encrypt_buffer, sizeof(buffer));
#endif
	//server should reply "TAOMEEV5DATA__OK"+0000+0000 + "DATA_END"
	if (memcmp(SYMBOL, buffer, P_SYMBOL_LEN) == 0
		&& memcmp(C_DATA_OK, buffer + P_SYMBOL_LEN, COMMAND_LEN ) == 0) {
		//DATA_OK
		file_head.processed_record = file_head.total_record;
		file_head.last_modify = time((time_t*)0);
		//after sending buffer reset proceesed flag
		tmp_result_ptr = send_link;
		while (tmp_result_ptr != NULL) {
			tmp_result_ptr->processed = 1;
			tmp_result_ptr = tmp_result_ptr->next;
		}

		close(sockfd);
		free_send_link(send_link);
		return 0;
	}
	else {
		write_log("Send_file:wait response: bad respose:%s\n", buffer);
		close(sockfd);
		free_send_link(send_link);
		return -1;
	}
}





/*
 * @brief 处理当前目录下所有标记为N的文件
 * @param DIR *dir:目录指针; char *folder:目录名; char *file:当前文件名
 * @return	0表示处理成功且有新文件，可继续处理； -1表示处理失败，或处理成功但没有新文件，可休眠。
 */
int processdir(DIR *dir)
{
	struct dirent 	*ptr;
	int retv;
	char fullpath[FULL_PATH_LEN];

	while ((ptr = readdir(dir)) != NULL) {
		if (ptr->d_name[0]=='N') {
			snprintf(fullpath, FULL_PATH_LEN, "%s/%s", data_store_path, ptr->d_name);
			get_time_string();
			retv = send_2_server(fullpath);
			if (retv >= 0) {
				//ret =0 ok! 1 bad files
				remove(fullpath);
			} else {
				//ret -1, can't send file, then return and have a rest
				return -1;
			}
		}//if
	}//while
	return 0;
}


/*
 * @brief 轮询目录和文件
 * @return 0表示成功，可继续休眠 -1表示轮询过程中出错
 */
int polling_files()
{
	DIR 	*dir;
	char 	dirbuffer[FULL_PATH_LEN];

	while (1) {
		sprintf(dirbuffer, "%s", data_store_path);
		dir = opendir((const char *)dirbuffer);
		if (dir == NULL) {
			write_log("Open dir <%s> failed!\n", dirbuffer);
			return 0; //return and have a rest may be directory have not been constructed
		}

		processdir(dir);

		sleep(60);
		closedir(dir);
	}
	return 0;
}

/*
 * @brief 发送线程
 */
void data_send()
{
	while (1) {
		if (send_load_config() == 0)
			polling_files();
		sleep(30);
	}
}
