/*
 * initiate.c
 *  加载端口号 加载数据发送的加密密码 加载数据存储路径
 *  初始化文件链表，初始化内存链表
 *
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

// needed for io operation
#include <stdio.h>

// needed for time(), localtime()
#include <time.h>

// needed for strcmp, memcpy, bzero etc..
#include <string.h>

// needed for exit();
#include <unistd.h>
#include <stdlib.h>
// needed for errno
#include <errno.h>

// needed for log
#include "log.h"

// needed for common Macro definition in enum
#include "protocol.h"

// needed for data_store_path
#include "store_data.h"

// needed for des_pass_phrase
#include "recv_data.h"

// needed for file_queue_init and destroy
#include "recv_data.h"

// needed for server_conn_port
#include "listen.h"

#include "initiate.h"

time_string_t	timestring;

/*
 * @brief 得到当前运行时时间，以便打印错误日志时记录时间
 *
 */

/*
 *  @brief 从fcgi.config中加载配置文件
 */
static int load_config()
{
	FILE *fp;			//打开配置文件指针
	char s[TEXT_LINE_LEN];	//存储从配置文件中读取的一行数据
	char c,c1;		//连续从文件的一行中读取的两个字符，用以标识变量、IP、注释行无用行
	char *eq = NULL;	//字符串中表示"="的指针
	int len = 0;		//读取的字符串长度，如果是一个变量，其标识符的长度要大于0

	fp = fopen("conf","rt");
	if (fp == NULL) {
		fprintf(stderr,"%s> \e[1m\e[31m Can't open config file \"conf\" %s\e[m \n",
				timestring.fullstring, strerror(errno));
		close_log_file();
		return -1;
	}

	while (!feof(fp)) {
		c = fgetc(fp);
		if ((int)c == -1)
			//EOF==-1
			break;
		if (c != '$') {
			while (c != '\n' && !feof(fp))
				c = fgetc(fp);
			continue;
		}
		if ((c1 = fgetc(fp)) != ':') {
			while (c != '\n' && !feof(fp))
				c = fgetc(fp);
			continue;
		}

		if ('$' == c && ':' == c1) {
			fgets(s,TEXT_LINE_LEN,fp);
			len = 0;
			if ((eq = strchr(s,'=')) == NULL)
				continue;
			len = eq - s;
			if (len <= 0)
				continue;

			if (bcmp(s, "des_pass_phrase", len) == 0) {
				len = strchr(eq + 1, '\n') - ( eq + 1 );
				if ((len + 1) >= sizeof(des_pass_phrase))
					continue;
				snprintf(des_pass_phrase, len + 1, "%s", eq + 1 );
			}
			else if (bcmp(s, "data_store_path", len) == 0) {
				len = strchr(eq + 1, '\n') - ( eq + 1 );
				if ((len + 1) >= sizeof(data_store_path))
					continue;
				snprintf(data_store_path, len + 1, "%s", eq + 1 );
			}
			else if (bcmp(s, "bind_ip", len) == 0) {
				len = strchr(eq + 1, '\n') - ( eq + 1 );
				if ((len + 1) >= sizeof(data_store_path))
					continue;
				snprintf(bind_ip, len + 1, "%s", eq + 1 );
			}
			else if (bcmp(s,"server_conn_port",len) == 0) {
				int tmpi;
				sscanf(eq+1, "%d", &tmpi);
				if (tmpi > 1024 && tmpi < 65536)
					server_conn_port = tmpi;
			}
		}//variable
	}//while
	fclose(fp);
	return 0;
}

/*
 * @brief 改变工作目录，只需在程序第一次循环时候运行
 * @param char *path, 工作路径
 */
static int change_working_path(char *path)
{
	char *slash = strrchr(path, (int)('/'));
	char working_path[FULL_PATH_LEN];
	if (slash > path) {
		 snprintf(working_path, (slash - path + 1), "%s", path);
		 if (chdir(working_path) == -1) {
			 fprintf(stderr, "%s> Change working dir:%s\n",
					 timestring.fullstring, strerror(errno));
			 return -1;
		 }
	}
	return 0;
}

/*
 * @brief 初始化，包括设置本次循环时间，改变工作目录，加载日志文件(每次循环都加载,以便修改配置文件后不需要重启),打开日志
 * @prame 执行路径  char *path
 */
int initiate(char *path)
{
	if (change_working_path(path) == -1)
		return -1;
	if (open_log_file() == -1)
		return -1;
	if (load_config() == -1)					//读取配置文件，包括服务器IP列表.
		return -1;

	return 0;
}

/*
 * @brief 每轮循环结束后，释放内存，关闭日志
 */
void destroy()
{
	close_log_file();
}
