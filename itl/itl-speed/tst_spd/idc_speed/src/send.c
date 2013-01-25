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

// needed for dns resolution
#include "ddns.h"

// needed for send_load_config
#include "init_and_destroy.h"

#include "send.h"

#ifdef		ENCODE_
#include "qdes.h"
#endif

//static char tmpfolder[FOLDER_NAME_LEN];
static char foldername[FOLDER_NAME_LEN];
static char filename[FILE_NAME_LEN];

server_ip_t	*analyze_server_set = NULL;					//服务器列表
char		analyze_server_domainname[DOMAIN_NAME_LEN];//服务器域名
uint16_t	server_conn_port = 7777;					//服务器通信端口
uint16_t	server_analyze_port = 8888;					//服务器分析端口
char		des_pass_phrase[PASSWD_LEN] = "TAOMEEV5";	//DES密码
uint8_t		if_ddns = 1;								//是否解析域名
int			epoll_time_out = 2000;						//连接服务器的超时


/*
 * @brief 根据初始化得到的服务器IP组，选择一个系统负载比较低的IP传输数据。
 * 数据交互
 * 查询负载
 * 8字节标识"TAOMEEV5" 8字节命令"GET_LOAD" 4字节数据长度＋4字节填补=0000+0000 CONTINUE/DATA_END
 * 服务器响应8字节标识"TAOMEEV5" 8字节命令"SHOWLOAD",4字节数据(8)+4字节填补(0) ||负载数值(4字节)+0000 +DATA_END
 * 发送数据
 * 8字节标识"TAOMEEV5" 8字节命令"SENDDATA" 8字节数据长度=4字节数据+4字节填补|| 8*N的数据+补齐+CONTINUE/DATA_END
 *
 * 所以服务器接受两种命令 SHOWLOAD 以及SENDDATA
 *数据包格式 标识	＋命令	＋数据长度	＋补齐长度	＋数据		+补齐		+尾部标识
 * 			8	 8			4			4  		N*struct	 1-7  	CONTINUE/DATA_END
 * @return host sequence of a ip address
 */
static uint32_t choose_server()
{
	int 			sockfd;						//遍历服务器的套接字
	uint32_t		best_server=0;
	float			best_server_load=100.0;
	struct			sockaddr_in server;
	char			buffer[NET_BUFFER_LEN];
	char			encrypt_buffer[NET_BUFFER_LEN];
	int				send_len;
	int				recv_len;
	int				total_send_len;
	int				total_recv_len;
	int				data_len;
	server_ip_t		*ip_ptr;

	ip_ptr = analyze_server_set;
	server.sin_family = AF_INET;
	server.sin_port = htons(server_conn_port);
	bzero(&(server.sin_zero),sizeof(server.sin_zero));
	while (ip_ptr != NULL) {
		memcpy(buffer, SYMBOL, P_SYMBOL_LEN);		//packet symbol
		memcpy(buffer + P_SYMBOL_LEN, C_GET_LOAD ,COMMAND_LEN);	//ask server's load command
		memset(buffer + P_SYMBOL_LEN + COMMAND_LEN, 0x0, DATA_LEN + FILL_LEN);//无后续数据
		memcpy(buffer + P_SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN, DATA_END, DATA_END_LEN);

#ifdef ENCODE_
		des_encrypt_n(des_pass_phrase,
				(const void*)buffer ,
				(void*)encrypt_buffer,
				(P_SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN + DATA_END_LEN)/ALIGN_LEN);
		memcpy(buffer, encrypt_buffer, sizeof(buffer));
#endif
		server.sin_addr.s_addr = htonl(ip_ptr->ipvalue);

		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			write_log("Choose server:create socket failed!%s\n", strerror(errno));
			ip_ptr = ip_ptr->next;
			continue;
		}
		if (-1 == fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK)) {
			write_log("Choose server:fcntl error!%s\n", strerror(errno));
			close(sockfd);
			ip_ptr = ip_ptr->next;
			continue;
		}

		int epfd;
		struct epoll_event ev;
		ev.events = EPOLLOUT;
		ev.data.fd = sockfd;
		if ((epfd = epoll_create(1)) == -1) {
			write_log("choose server:create epoll_fd failed!%s\n", strerror(errno));
			close(sockfd);
			ip_ptr = ip_ptr->next;
			continue;
		}

		if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev)) {
			write_log("Choose server:epoll_ctl error!%s\n", strerror(errno));
			close(sockfd);
			close(epfd);
			ip_ptr = ip_ptr->next;
			continue;
		}

		connect(sockfd, (struct sockaddr*)(&server), sizeof(server));

		int nfds;
		struct epoll_event events[128];
		if ((nfds = epoll_wait(epfd, events, 128, epoll_time_out)) == -1) {
			write_log("Choose server:epoll_ctl error!%s\n", strerror(errno));
			close(epfd);
			close(sockfd);
			ip_ptr = ip_ptr->next;
			continue;
		}

		if (nfds == 0) {
			write_log("Choose server %s:epoll timeout!\n", inet_ntoa(server.sin_addr));
			if (-1 == epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev)) {
				write_log("Choose server:epoll_ctl error!%s\n", strerror(errno));
				close(epfd);
				close(sockfd);
				ip_ptr = ip_ptr->next;
				continue;
			}
			close(epfd);
			close(sockfd);
			ip_ptr = ip_ptr->next;
			continue;
		}
		int i;
		for (i = 0 ; i < nfds; i++) {
			if (-1 == epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev)) {
				write_log("choose server:epoll_ctl error!%s\n", strerror(errno));
				close(epfd);
				close(sockfd);
				break;
			}
			if (events[i].data.fd == sockfd) {
				if (!(events[i].events & EPOLLOUT)) {
					close(sockfd);
					continue;//for next
				}

				if (-1 == fcntl(sockfd,F_SETFL, fcntl(sockfd, F_GETFL) ^ O_NONBLOCK)) {//blocking
					write_log("Choose server:fcntl error!%s\n", strerror(errno));
					close(epfd);
					close(sockfd);
					break;
				}

				send_len = 0;
				total_send_len = 0;
				data_len = P_SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN + DATA_END_LEN;
				int flag = 0;
				while ((send_len=send(sockfd, buffer+total_send_len, (data_len-total_send_len), 0))
						< (data_len - total_send_len)) {
					if (send_len == -1) {
						if (errno == EINTR)
							continue;
						write_log("Choose server: send error:%s\n", strerror(errno));
						close(epfd);
						close(sockfd);
						flag = 1;
						break; //break while
					}
					else
						total_send_len += send_len;
				}
				if (flag == 1)
					break; //break for
				total_send_len += send_len;

				recv_len = 0;
				total_recv_len = 0;
				data_len = P_SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN
							+ sizeof(float) + (ALIGN_LEN - sizeof(float)) + DATA_END_LEN; //40
				flag = 0;
				while ((recv_len = recv(sockfd, (void*)buffer+total_recv_len, (sizeof(buffer) - total_recv_len), 0)) < (data_len - total_recv_len)) {
					if (recv_len == -1) {
						if (errno == EINTR)
							continue;
						write_log("Choose server: recv error:%s\n", strerror(errno));
						close(epfd);
						close(sockfd);
						flag = 1;
						break; //break while
					}
					else if (recv_len == 0) {
						write_log("Choose server: recv error: server_closed %s\n", strerror(errno));
						close(epfd);
						close(sockfd);
						flag = 1;
						break; //break while
					}
					else
						total_recv_len += recv_len;
				}
				if (flag == 1 )
					break; //break for
				total_recv_len += recv_len;

#ifdef ENCODE_
				des_decrypt_n(des_pass_phrase, (const void*)buffer , (void*)encrypt_buffer, (data_len)/ALIGN_LEN);	//32BYTE/8BYTE
				memcpy(buffer, encrypt_buffer, sizeof(buffer));
#endif
				//server should reply "TAOMEEV5SHOWLOAD"+4B+0000+float+0000 + "DATA_END"
				if (memcmp(SYMBOL, buffer, P_SYMBOL_LEN) == 0
					&& memcmp(C_SHOW_LOAD, buffer + P_SYMBOL_LEN, COMMAND_LEN ) == 0) {
						float tmp = *((float*)(buffer + P_SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN));
						best_server = tmp < best_server_load ?ip_ptr->ipvalue :best_server;
						best_server_load = tmp < best_server_load ?tmp :best_server_load ;

				}
				else
					write_log("Chooser server: get load: bad respose:%s\n", buffer);
			}//if sockfd
			else
				close(events[i].data.fd);
		}//for
		close(sockfd);
		close(epfd);
		ip_ptr = ip_ptr->next;
	}
	return best_server;
}

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
 * @brief 处理后的链表 已经处理标识已经更改，将更改写回文件中.
 * @param filename 文件名; head 链表头; fh 文件头结构
 */
static void write_back_to_file(const char *filename, store_result_t *head, file_header_t fh)
{
	FILE *fp;
	store_result_t *ptr;
	fp = fopen(filename, "wb+");
	if (fp == NULL) {
		write_log("Can not open file:%s!!!\n", filename);
		free_send_link(head);
		return ;
	}

	fwrite(&fh, sizeof(file_header_t), 1, fp);
	ptr = head;
	while (ptr != NULL) {
		fwrite(ptr, sizeof(store_result_t)- sizeof((store_result_t*)0), 1, fp);
		ptr = ptr->next;
	}
	fclose(fp);
}

/*
 * @brief 将某个文件发送到服务器
 * @param const char *send_file:带发送的文件名指针
 * @return 0 表示发送成功，1表示非法文件, -1表示发送失败
 */
static int send_2_server(const char *send_file)
{
	int		        	sockfd;					//套接字
	struct				sockaddr_in	server;		//服务器
	file_header_t		file_head;				//文件头
	FILE 				*fp;					//文件打开指针
	int					read_count=0;			//读出的记录数
	int					write_count=0;			//写入buffer中的结构数
	int					read_len;				//实际从文件中读出的字节长度
	int					send_len;				//tcp实际发送的字节长度
	int					total_send_len;			//tcp总共发送了多少字节
	uint32_t			proper_server;			//choose_ip()返回的合适的服务器IP 本机字节序
	char				buffer[NET_BUFFER_LEN];			//数据缓冲区
	char				encrypt_buffer[NET_BUFFER_LEN];	//加密后的缓冲区
	char				*buf_ptr;				//缓冲区的剩余空间指针
	store_result_t		tmp_result;				//临时存放一个数据
	store_result_t		*result_ptr;			//指向数据的指针
	store_result_t		*tmp_result_ptr;		//保存result_ptr指针
	int					effective_size;			//结构体中的有效数据长度,需要减去next指针的大小
	store_result_t		*send_link=NULL;		//数据发送链，处理完毕后写回文件

	//(void)encrypt_buffer;

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
		/*
		  printf("%u,%u,%d,%u,%u,%u,%u,%u,%u\n",tmp_result.processed,tmp_result.machine_number
				,tmp_result.probe_time,tmp_result.probed_ip,tmp_result.reachable,tmp_result.hop
				,tmp_result.loss_percentage,tmp_result.avg_latency,tmp_result.previous_hop);
		*/
		if (read_len != 1)
			break;
		tmp_result.next = NULL;

		read_count++;	//记录数加1

		if ((result_ptr = (store_result_t*)malloc(sizeof(store_result_t))) == NULL) {
			write_log("Send_2_server:malloc failed!!\n");
			fclose(fp);
			pthread_exit(NULL);
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
	if ( read_count != file_head.total_record) {
		write_log("%s:file record not equal total_record field!!!\n", send_file);
		free_send_link(send_link);
		return 1;		//读到的记录数与总记录数不相等
	}
	if (read_count == 0 || send_link == NULL) {
		write_log("%s:file record==0!\n", send_file);
		free_send_link(send_link);
		return 1;		//没有读到记录
	}

	proper_server = choose_server();
	if (proper_server == 0) {
		write_log("Send file:no proper server exist!\n");
		free_send_link(send_link);
		return -1;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		write_log("Send file:create socket failed!%s\n", strerror(errno));
		free_send_link(send_link);
		return -1;
	}
	server.sin_addr.s_addr = htonl(proper_server);
	server.sin_family = AF_INET;
	server.sin_port = htons(server_conn_port);
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
			uint32_t fill_len = (ALIGN_LEN - len%ALIGN_LEN) % ALIGN_LEN;	//需要补齐的字节数
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
		uint32_t fill_len = (ALIGN_LEN - len%ALIGN_LEN)%ALIGN_LEN;	//需要补齐的字节数
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

		write_back_to_file(send_file, send_link, file_head);
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
 * @brief 更新目录
 * @param char *folder: 当前目录, int flag: 是真的更新目录还是测试下一个目录的存在性
 * @return 如果flag == TEST_FOLDER_EXISTENCE 返回0表示存在，-1表示不存在; 否则返回0表示更新成功 -1表示更新失败
 */
int update_folder(const char *folder,int flag)
{
    DIR *dir;
    struct dirent *ptr;
    char next_folder[16] = {0};
    int has_next_folder = 0;

    sprintf(next_folder, "%s", "9999-12-31");
    dir = opendir((const char*)data_store_path);
	if (dir == NULL) {
		write_log("Open dir <%s> failed!\n", data_store_path);
		return -1; 
	}
	while ((ptr = readdir(dir)) != NULL) {
		if (ptr->d_type == DT_DIR
                && strlen(ptr->d_name) == 10
                && strcmp(ptr->d_name, folder) > 0
                && strcmp(ptr->d_name, next_folder) < 0) 
        {
            sprintf(next_folder, "%s", ptr->d_name);
            has_next_folder = 1;
        }
    }

	if (flag == TEST_FOLDER_EXISTENCE) {
        return has_next_folder ? 0 : -1;
    }


    if (has_next_folder) {
	    FILE *fp = fopen("send.ini", "wt+");
	    if (fp == NULL) {
	    	write_log("Open init file \"send.ini\" failed:%s\n", strerror(errno));
	    	return -1;
	    }

	    fprintf(fp, "#FolderName\n");
	    fprintf(fp, "%s\n", next_folder);
	    fprintf(fp, "#FileName\n");
	    fprintf(fp, "N%04d.0000-00-00", machine_number);
	    fclose(fp);
    }

	return 0;
}

/*
 * @brief 更新文件
 * @param char *filename 当前文件
 * @return 0表示更新成功，-1表示更新失败
 */
int update_file(char *filename)
{
	FILE *fp;
	char buffer[FULL_PATH_LEN] = {0};

	fp = fopen("send.ini","rt+");
	if (fp == NULL) {
		write_log("Open init file \"send.ini\" failed:%s\n", strerror(errno));
		return -1;
	}

	fscanf(fp, "%s", buffer);
	fscanf(fp, "%s", buffer);
	fscanf(fp, "%s", buffer);
	fprintf(fp, "\n%s", filename);
	fclose(fp);
	return 0;
}

/*
 * @brief 处理当前目录下所有标记为N的文件
 * @param DIR *dir:目录指针; char *folder:目录名; char *file:当前文件名
 * @return	0表示处理成功且有新文件，可继续处理； -1表示处理失败，或处理成功但没有新文件，可休眠。
 */
int processdir(DIR *dir, const char *folder, const char *file)
{
	struct dirent 	*ptr;
	file_t			*mylink = NULL;
	file_t			*tmp = NULL;
	file_t			*fnext = NULL;
	file_t			*pos = NULL;

	char 			current_largest[FILE_NAME_LEN];
	char 			lastfile[FILE_NAME_LEN];

	snprintf(lastfile, FILE_NAME_LEN, "N%04d.%s-23-59", machine_number, folder);

	if (strcmp(file,lastfile) >= 0) {
		write_log("%s have already processed last time!\n", folder);
		return 0;		//this dir was processed last time;
	}

	snprintf(current_largest, FILE_NAME_LEN, "%s", file);
	while ((ptr = readdir(dir)) != NULL) {
		if (ptr->d_name[0]=='N' && strcmp(ptr->d_name,lastfile) <= 0) {
			if (strcmp(ptr->d_name, current_largest) > 0)
				strncpy(current_largest, ptr->d_name, FILE_NAME_LEN);

			if ((tmp = (file_t*)malloc(sizeof(file_t))) == NULL) {
				write_log("Process dir:malloc failed!\n");
				closedir(dir);
				pthread_exit(NULL);
			}
			snprintf(tmp->filename, FILE_NAME_LEN, "%s", ptr->d_name);
			snprintf(tmp->fullpath, FULL_PATH_LEN, "%s/%s/%s",
						data_store_path, folder, ptr->d_name);
			tmp->next = NULL;
			//insert sort;
			if (mylink == NULL)
				mylink = tmp;
			else {
				fnext = mylink;
				while (fnext != NULL) {
					if (strcmp(tmp->filename, fnext->filename) > 0) {
						pos = fnext;//暂存插入点的前一点
						fnext = fnext->next;
					}
					else
						break;
				}
				if (fnext == mylink) {
					tmp->next = mylink;
					mylink = tmp;
				}
				else if (fnext == NULL)
					pos->next = tmp;
				else {
					pos->next = tmp;
					tmp->next = fnext;
				}
			}//insert sort
		}//if
	}//while

	//process file in link;
	tmp = mylink;
	int retv;
	while (tmp != NULL) {
		retv = send_2_server(tmp->fullpath);
		if (retv == 0) {
			//ret =0 ok! update files and change file's name
			update_file(tmp->filename);
			char str[FULL_PATH_LEN];
			tmp->filename[0] = 'Y';
			snprintf(str, FULL_PATH_LEN, "%s/%s/%s", data_store_path, folder, tmp->filename);
			rename(tmp->fullpath, str);
		}
		else if (retv == 1) {
			//ret 1 , bad files,ignore just update files
			update_file(tmp->filename);

            //add by tonyliu 2012-11-09
            //rename bad file name from N0006.2012-11-09-04-55 to E0006.2012-11-09-04-55
			char str[FULL_PATH_LEN];
			tmp->filename[0] = 'E';
			snprintf(str, FULL_PATH_LEN, "%s/%s/%s", data_store_path, folder, tmp->filename);
			rename(tmp->fullpath, str);
		}
		else {
			//ret -1, can't send file, then return and have a rest
			// but you have to free memory first when reading dir
			// so just break and free memory down near by and test ret again
			break;
		}
		tmp = tmp->next;
	}

	//free memory
	tmp = mylink;
	while (mylink != NULL) {
		tmp = mylink;
		mylink = mylink->next;
		free(tmp);
	}

	if ( retv == -1)
		return -1;//ret== -1表示没有发送成功，则返回并休眠，下次再尝试发送

	if (strcmp(current_largest, lastfile) >= 0) {
		write_log("Good %s processed perfectly!!!\n", folder);
		return 0;//file complete  then ok
	}
	else if (update_folder(folder, TEST_FOLDER_EXISTENCE) == 0) {
		//file in this folder not complete but next folder exist,then process next folder
		//memcpy(tmpfolder, foldername, FOLDER_NAME_LEN);
		return 0;
	}
	else {	//file not complete and next folder does not exist then wait few seconds
		//memcpy(tmpfolder, foldername, FOLDER_NAME_LEN);//roll back change about tmpfolder
		int times = 1;
		while (times <= 5) {//test five times for coming new files
			rewinddir(dir);
			while ((ptr = readdir(dir)) != NULL) {
				if ( ptr->d_name[0] == 'N' && strcmp(ptr->d_name,current_largest) > 0 && strcmp(ptr->d_name,lastfile) <= 0) {
					strncpy(current_largest, ptr->d_name, FILE_NAME_LEN);
					char buffer[FULL_PATH_LEN];
					snprintf(buffer, FULL_PATH_LEN, "%s/%s/%s",data_store_path,folder,ptr->d_name);
					int retv;
					retv = send_2_server(buffer);
					if (retv == 0) {
						//ret =0,1 ok! update files and change file's name
						update_file(current_largest);
						char str[FULL_PATH_LEN];
						//current_largest[0]='Y';//不能改，否则影响后续循环
						snprintf(str, FULL_PATH_LEN, "%s/%s/Y%s", data_store_path, folder, &current_largest[1]);
						rename(buffer, str);
                        write_log("IN TEST FIVE: %s", str);//for debug
					}
					else if (retv == 1) {
						//ret == 1 bad files,ignore, just update files
						update_file(current_largest);

                        //add by tonyliu 2012-11-09
                        //rename bad file name from N0006.2012-11-09-04-55 to E0006.2012-11-09-04-55
						char str[FULL_PATH_LEN];
						snprintf(str, FULL_PATH_LEN, "%s/%s/E%s", data_store_path, folder, &current_largest[1]);
						rename(buffer, str);
                        write_log("IN TEST FIVE: %s", str);//for debug
					}
					else {
						//ret -1, can't send file, then return and have a rest
						// but you have to free memory first when reading dir
						// so just break and free memory down near by and test ret again
						break;
					}
					times=0;//have new file
				}
			}
			times++;
		}//while
	}//else

	return -1;//you can have a rest
}

/*
 * @brief 从send.ini中读出初始化处理的目录名和文件名
 * @param char *folder,目录buf; char *file 文件buf;
 * @return 0表示成功 -1表示找不到send.ini
 */
int get_ini(char *folder,char *file)
{
	FILE *fp;
	int flag = 0;
	fp = fopen("send.ini","rt");
	if (NULL == fp) {
		write_log("Fail to open init file \"send.ini\":%s\n", strerror(errno));
		return -1;
	}
	while (!feof(fp)) {
		char c = fgetc(fp);
		if (c == '#') {
			while (c != '\n')
				c = fgetc(fp);
			continue;
		}
		else if (c == ' ') {
			while (c == ' ')
				c = fgetc(fp);
			fseek(fp, -1L, SEEK_CUR);
			continue;
		}
		else if (c == '\n')
			continue;
		else if ((int)c == -1)
			continue;
		fseek(fp, -1L, SEEK_CUR);
		if(0 == flag) {
			fscanf(fp, "%s", folder);
			flag = 1;
		}
		else
			fscanf(fp, "%s", file);
	}
	fclose(fp);
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
	int 	flag;

	while (1) {
		get_ini(foldername, filename);
		//memcpy(tmpfolder, foldername, FOLDER_NAME_LEN);

		sprintf(dirbuffer, "%s/%s", data_store_path, foldername);
		dir = opendir((const char *)dirbuffer);
		if (dir == NULL) {
			write_log("Open dir <%s> failed!\n", dirbuffer);
			return 0; //return and have a rest may be directory have not been constructed
		}

		flag = processdir(dir, foldername, filename);
		if (-1 == flag) {
			closedir(dir);
			return -1;	//have a rest
		}
		//when completly processed this dir we want to get into next folder
		//but next folder may be have not constructed yet, but if collect module running,
		//at most 60 secondes elapsed,next folder must be exist!
		sleep(60);
		//update_folder(tmpfolder, REALLY_UPDATE_FOLDER);
		update_folder(foldername, REALLY_UPDATE_FOLDER);
		closedir(dir);
	}

	return 0;
}


/*
 * @brief 释放每次循环中为服务器列表分配的内存 因为每分钟执行一次，
 */
void free_server_set()
{
	server_ip_t *tmp,*tnext;
	tnext = analyze_server_set;
	while (tnext != NULL) {
		tmp = tnext;
		tnext = tnext->next;
		free(tmp);
	}
	analyze_server_set = NULL;
}

/*
 * @brief 发送线程
 */
void *data_send()
{
	//pthread_detach(pthread_self());
	while (1) {
		if (send_load_config() == -1)
			pthread_exit(NULL);
		if (if_ddns)
			dns_resolve(analyze_server_domainname);
		polling_files();
		free_server_set();
		sleep(30);
	}
}
