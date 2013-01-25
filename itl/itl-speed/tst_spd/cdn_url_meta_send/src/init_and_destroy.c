/*
 * initiate.c 程序初始化
 *
 *  Created on:	2011-7-4
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

// needed for sprintf, snprintf, fopen
#include <stdio.h>

// needed for string operation
#include <string.h>

// needed for malloc
#include <stdlib.h>

// needed for time
#include <time.h>

// needed for chdir, exit, write,close
#include <unistd.h>

// needed for errno and strerror
#include <errno.h>

#include "common.h"
#include "send.h"
#include "log.h"
#include "init_and_destroy.h"

char 	data_store_path[FULL_PATH_LEN] = "/home/singku/cgi_data/ver003/cdn";	//数据存储路径

char start_folder[FOLDER_NAME_LEN];
char start_filename[FILE_NAME_LEN];
char stop_folder[FOLDER_NAME_LEN];
char stop_filename[FILE_NAME_LEN];
char current_folder[FOLDER_NAME_LEN];
char current_filename[FILE_NAME_LEN];
char *net_buffer;
char *encrypt_net_buffer;
/*
 *@brief send线程从conf中读入全局设置
 */
int send_load_config()
{
	FILE *fp;						//打开配置文件指针
	int a[4];						//临时存储点分IP的4个部分
	int flag;						//标识IP是否合法
	char s[TEXT_LINE_LEN];	//存储从配置文件中读取的一行数据
	uint32_t i;					//for循环计数
	char c,c1;					//连续从文件的一行中读取的两个字符，用以标识变量、IP、注释行、无用行
	char *eq = NULL;			//字符串中表示"="的指针
	int len = 0;					//读取的字符串长度，如果是一个变量，其标识符的长度要大于0
	uint32_t tmpi = 0;			//临时存储读取的变量数值

	fp = fopen("conf", "rt");
	if (fp == NULL) {
		write_log("Can't open config file \"conf\":%s\n", strerror(errno));
		return -1;
	}

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
			flag = 0;
			fgets(s,TEXT_LINE_LEN,fp);
			tmpi = 0;
			len = 0;
			if ((eq = strchr(s, (int)'=')) == NULL)
				continue;
			len = eq - s;
			if (len <= 0)
				continue;

			if (bcmp(s, "data_store_path", len) == 0) {
				len = strchr(eq + 1, (int)'\n') - ( eq + 1 );
				if ((len + 1) >= sizeof(data_store_path))
					continue;
				if (*( eq + 1 ) == '/')	//从根路径开始
					snprintf(data_store_path, len + 1, "%s", eq + 1 );
			}
			else if (bcmp(s, "accept_port", len) == 0) {
				sscanf(eq+1, "%d", &tmpi);
				if (tmpi > 1024 && tmpi < 65536)
					accept_port = tmpi;
			}
			else if (bcmp(s,"des_pass_phrase",len) == 0) {
				len = strchr(eq + 1,'\n') - (eq + 1);
				if ((len + 1) >= sizeof(des_pass_phrase))
					continue;
				snprintf(des_pass_phrase, len + 1, "%s", eq + 1);
			}
			else if (bcmp(s,"stop",len) == 0) {
				len = strchr(eq + 1,'\n') - (eq + 1);
				if ((len + 1) >= sizeof(stop_filename)){
					write_log("bad stop_time!\n");
					return -1;
				}
				snprintf(stop_filename, len + 1, "%s", eq + 1);
				int year,mon,day;
				sscanf(stop_filename, "META-%04d-%02d-%02d", &year, &mon, &day);
				snprintf(stop_folder, sizeof(stop_folder), "%04d-%02d-%02d", year, mon, day);
			}
			else if (bcmp(s, "accept_addr", len) == 0) {
				a[0] = a[1] = a[2] = a[3] = -1;
				sscanf(eq + 1, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
				for (i = 0; i <= 3; i++) {
					if (a[i] > 255 || a[i] < 0) {
						write_log("[Warning]:illegal server ip in file \"conf\" [%s],ignored\n",s);
						flag = 1;	//IP有问题！则跳出并置位错误标记为真
						break;
					}
				}//for
				if (!flag)
					accept_addr = a[0]*256*256*256 + a[1]*256*256 + a[2]*256 + a[3];
			}//server
		}//if $:
	}//while
	fclose(fp);
	return 0;
}



/*
 * @brief 改变工作目录，只需在程序第一次循环时候运行
 */
static int change_working_path(char *path)
{
	char *slash = strrchr(path, (int)('/'));
	char working_path[FULL_PATH_LEN];
	if (slash > path) {
		 snprintf(working_path, (slash - path + 1), "%s", path);
		 if (chdir(working_path) == -1) {
			 fprintf(stderr, "Change working dir:%s\n", strerror(errno));
			 return -1;
		 }
	}
	return 0;
}
/*
 * @brief 初始化，包括设置本次循环时间，改变工作目录，加载日志文件(每次循环都加载,以便修改配置文件后不需要重启),打开日志
 * @prame 执行路径 argv[0]
 */
int initiate(char *path)
{
	//获取程序运行时路径前缀，改变工作目录，找到配置文件、打开错误文件.他们与可执行程序位于同一目录
	if (change_working_path(path) == -1)
		return -1;
	//在当前位置打开日志记录文件
	if (open_log_file() == -1) {
		fprintf(stderr, "open log file failed!\n");
		return -1;
	}
	return 0;
}

/*
 * @brief 每轮循环结束后，释放内存，关闭日志
 */
void destroy()
{
	//关闭日志
	close_log_file();

	free(net_buffer);
	free(encrypt_net_buffer);
}
