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
 * @brief 将某个文件发送到服务器
 * @param const char *send_file:带发送的文件名指针
 * @return 0 表示发送成功, -1表示发送失败
 */

static int send_buffer(uint32_t data_len, char *buf, int sockfd)
{
	//sent to server
	int send_len = 0;
	int total_send_len = 0;

	while ((send_len = send(sockfd, buf + total_send_len, (data_len - total_send_len), 0))
			< (data_len - total_send_len)) {
		if (send_len == -1 ) {
			if (errno == EINTR)
				continue;
			write_log("Send file: send error:%s\n", strerror(errno));
			return -1;
		}
		else
			total_send_len += send_len;
	}
	total_send_len += send_len;
	return 0;
}

static int send_2_server(const char *fullpath, const char *filename)
{
	int sockfd;//套接字
	struct sockaddr_in	server;//服务器
	int fd;//文件打开指针
	int read_len;//实际从文件中读出的字节长度
	char name_buffer[FILE_NAME_LEN];
	char *buf_ptr;//缓冲区的剩余空间指针

	fd = open(fullpath, O_RDONLY);
	if (fd == -1) {
		write_log("Can not open file %s: %s\n", fullpath, strerror(errno));
		return -1;
	}


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		write_log("Send file:create socket failed!%s\n", strerror(errno));
		close(fd);
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
		close(fd);
		return -1;
	}
	write_log("Send file:%s\n",filename);
	memset(name_buffer, 0x0, sizeof(name_buffer));
	snprintf(name_buffer, sizeof(name_buffer), "%s", filename);
	//buffer的前16个字节是"TAOMEEV5SENDDATA",接着8个字节是数据长度
	buf_ptr = net_buffer;
	memcpy(buf_ptr, SYMBOL, P_SYMBOL_LEN);
	buf_ptr += P_SYMBOL_LEN;
	memcpy(buf_ptr, C_WITH_NAME, COMMAND_LEN);
	buf_ptr += COMMAND_LEN;
	buf_ptr += (DATA_LEN + FILL_LEN);		//跳过数据长度部分
	memcpy(buf_ptr, name_buffer, sizeof(name_buffer));//第一个包 拷贝文件名
	buf_ptr += FILE_NAME_LEN;
	int max_read = NET_BUFFER_LEN - DATA_END_LEN - (buf_ptr - net_buffer);
	max_read = max_read - max_read % ALIGN_LEN;
	int data_read_ok = 0;
	int file_len = FILE_NAME_LEN;
	while ((read_len = read(fd, buf_ptr, max_read)) >= -1) {
		if (read_len == -1) {
			if (errno == EINTR)
				write_log("read been interrupted\n");
			else {
				write_log("read error: %s\n", strerror(errno));
				close(sockfd);
				close(fd);
				return -1;
			}
		} else {
			if (read_len < max_read)
				data_read_ok = 1;//get to EOF
			read_len += file_len;//第一次还要文件名长度
			//已经容纳不下更多数据，填写实际的数据长度(8字节),加密缓冲区并发送缓冲区内容
			memcpy(net_buffer + P_SYMBOL_LEN + COMMAND_LEN, &read_len, DATA_LEN );
			uint32_t fill_len = (ALIGN_LEN - (read_len % ALIGN_LEN)) % ALIGN_LEN;	//需要补齐的字节数
			memcpy(net_buffer + P_SYMBOL_LEN + COMMAND_LEN + DATA_LEN, &fill_len, FILL_LEN);
			buf_ptr += (read_len - file_len);
			file_len = 0;
			memset(buf_ptr, 0x0, fill_len );//保证是8字节的整数倍
			buf_ptr += fill_len;
			if (data_read_ok == 0)
				memcpy(buf_ptr, CONTINUE, DATA_END_LEN);
			else
				memcpy(buf_ptr, DATA_END, DATA_END_LEN);
			buf_ptr += DATA_END_LEN;			//buf_ptr指向整个数据的末端

#ifdef		ENCODE_
			des_encrypt_n(des_pass_phrase, (const void*)net_buffer ,
					(void*)encrypt_net_buffer, (buf_ptr - net_buffer)/ALIGN_LEN);
			memcpy(net_buffer, encrypt_net_buffer, buf_ptr - net_buffer);
#endif
			//sent to server
			if (send_buffer((buf_ptr - net_buffer), net_buffer, sockfd) == -1) {
				close(fd);
				close(sockfd);
				return -1;
			}
			if (data_read_ok)
				break;
			else {
				buf_ptr = net_buffer + P_SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN;
				if (memcmp(net_buffer + P_SYMBOL_LEN, C_SEND_DATA, COMMAND_LEN) != 0)
					memcpy(net_buffer + P_SYMBOL_LEN, C_SEND_DATA, COMMAND_LEN);
				max_read = NET_BUFFER_LEN - DATA_END_LEN - (buf_ptr - net_buffer);
				max_read = max_read - max_read % ALIGN_LEN;
			}
		}//else
	}//while
	close(fd);

	write_log("File:%s wait Server response\n",filename);
	// wait for server's response
	int recv_len = 0;
	int total_recv_len = 0;
	int data_len = P_SYMBOL_LEN + COMMAND_LEN + DATA_LEN + FILL_LEN + DATA_END_LEN;
	while ((recv_len = recv(	sockfd,
									(void*)net_buffer + total_recv_len,
									(NET_BUFFER_LEN - total_recv_len), 0))
			< (data_len - total_recv_len)) {
		if (recv_len == -1) {
			if (errno == EINTR)
				continue;
			write_log("Send file:receive server response error:%s\n", strerror(errno));
			close(sockfd);
			return -1;
		}
		else if (recv_len == 0) {
			write_log("Send file:receive server response error:%s\n", strerror(errno));
			close(sockfd);
			return -1;
		}
		else
			total_recv_len += recv_len;
	}
	total_recv_len += recv_len;

#ifdef		ENCODE_
	des_decrypt_n(des_pass_phrase, (const void*)net_buffer,
						(void*)encrypt_net_buffer, (total_recv_len)/ALIGN_LEN);	//32BYTE/8BYTE
	memcpy(net_buffer, encrypt_net_buffer, total_recv_len);
#endif
	//server should reply "TAOMEEV5DATA__OK"+0000+0000 + "DATA_END"
	if (memcmp(SYMBOL, net_buffer, P_SYMBOL_LEN) == 0
		&& memcmp(C_DATA_OK, net_buffer + P_SYMBOL_LEN, COMMAND_LEN ) == 0) {
		//DATA_OK
		close(sockfd);
		return 0;
	}
	else {
		write_log("Send_file:wait response: bad respose:%s\n", net_buffer);
		close(sockfd);
		return -1;
	}
}


/*
 * @brief 更新目录
 * @param char *folder: 当前目录, int flag: 是真的更新目录还是测试下一个目录的存在性
 * @return 返回0表示更新成功 -1表示更新失败
 */
int update_folder(char *folder)
{
	FILE *fp;
	int year;
	int month;
	int day;

	sscanf(folder, "%d-%d-%d", &year, &month, &day);

	++day;
	switch (month) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		if (32 <= day) {
			day = 1;
			if (++month == 13) {
				month = 1;
				++year;
			}
		}
		break;
	case 2:
		if ( (year%4 == 0 && year%100 != 0) || year%400 == 0 ) {
			if (30 <= day) {
				day = 1;
				++month;
			}
		}
		else {
			if (29 <= day)
			{
				day = 1;
				++month;
			}
		}
		break;
	default:
		if (31 <= day)
		{
			day = 1;
			++month;
		}
		break;
	}

	sprintf(folder, "%04d-%02d-%02d", year, month, day);


	fp = fopen("send.ini", "wt+");
	if (fp == NULL) {
		write_log("Open init file \"send.ini\" failed:%s\n", strerror(errno));
		return -1;
	}

	fprintf(fp, "#FolderName\n");
	fprintf(fp, "%s\n", folder);
	fprintf(fp, "#FileName\n");
	fprintf(fp, "META-%s-00-00-00", folder);
	fclose(fp);
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
int processdir(DIR *dir, char *folder)
{
	struct dirent 	*ptr;
	file_t *mylink = NULL;
	file_t *tmp = NULL;
	file_t *fnext = NULL;
	file_t *pos = NULL;

	while ((ptr = readdir(dir)) != NULL) {
		if (ptr->d_name[0]=='M'
			&& strcmp(ptr->d_name, current_filename) >= 0
			&& strcmp(ptr->d_name, stop_filename) <= 0) {

			if ((tmp = (file_t*)malloc(sizeof(file_t))) == NULL) {
				write_log("Process dir:malloc failed!\n");
				exit(-1);
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
		retv = send_2_server(tmp->fullpath, tmp->filename);
		if (retv == 0)
			update_file(tmp->filename);
		else if (retv == -1)
			return -1;

		tmp = tmp->next;
	}

	//free memory
	tmp = mylink;
	while (mylink != NULL) {
		tmp = mylink;
		mylink = mylink->next;
		free(tmp);
	}

	return 0;
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

	while (1) {
		if (get_ini(current_folder, current_filename) == -1)
			return -1;
		if (strcmp(current_filename, stop_filename) > 0
			|| strcmp(current_folder, stop_folder) > 0)
			return 0;

		sprintf(dirbuffer, "%s/%s", data_store_path, current_folder);
		dir = opendir((const char *)dirbuffer);
		if (dir == NULL)
			write_log("Open dir <%s> failed!\n", dirbuffer);
		else {

			if (processdir(dir, current_folder) == -1) {
				closedir(dir);
				write_log("Dir %s not complete!\n", current_folder);
				return -1;
			}
			closedir(dir);
		}
		update_folder(current_folder);
	}
	return 0;
}
/*
 * @brief 发送线程
 */
int data_send()
{
	if (send_load_config() == 0) {
		write_log("start:%s,stop:%s\n", start_filename, stop_filename);
		if (polling_files() == -1)
			return -1;
	}
	return 0;
}
