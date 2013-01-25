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

// needed for pthread_exit
#include <pthread.h>

#include "common.h"
#include "send.h"
#include "collect.h"
#include "log.h"
#include "init_and_destroy.h"

uint32_t		machine_number = 0;							//机器编号
time_string_t 	timestring;									//打印日志的时间，每次循环中更新
char 			data_store_path[FULL_PATH_LEN] = "/home/probe_ip";	//数据存储路径

/*
 *@brief collect线程从conf中读入全局设置
 */
int collect_load_config()
{
	FILE *fp;			//打开配置文件指针
	int a[4];			//临时存储点分IP的4个部分
	int flag;			//标识IP是否合法
	char s[TEXT_LINE_LEN];	//存储从配置文件中读取的一行数据
	uint32_t i;			//for循环计数
	char c,c1;			//连续从文件的一行中读取的两个字符，用以标识变量、IP、注释行、无用行
	char *eq = NULL;	//字符串中表示"="的指针
	int len = 0;		//读取的字符串长度，如果是一个变量，其标识符的长度要大于0
	uint32_t tmpi = 0;	//临时存储读取的变量数值
	ip_addr_t *tmpip = NULL;//指向待探测ip

	fp = fopen("conf", "rt");
	if (fp == NULL) {
		write_log("Can't open config file \"conf\":%s\n", strerror(errno));
		close_log_file();
		return -1;
	}

	while (!feof(fp)) {
		c = fgetc(fp);
		if ((int)c == -1)
			//EOF==-1
			break;
		if (c != '[' && c != '$') {
			while (c != '\n' && !feof(fp))
				c = fgetc(fp);
			continue;
		}
		if ((c1 = fgetc(fp)) != ':') {
			while (c != '\n' && !feof(fp))
				c = fgetc(fp);
			continue;
		}

		if ('$' == c && ':' ==c1) {
			//memset(s,0,TEXT_LINE_LEN);
			flag = 0;
			fgets(s,TEXT_LINE_LEN,fp);
			tmpi = 0;
			len = 0;
			//printf("%s",s);
			if ((eq = strchr(s, (int)'=')) == NULL)
				continue;
			len = eq - s;
			if (len <= 0)
				continue;

			if (bcmp(s, "machine_number", len) == 0) {
				sscanf(eq+1, "%u", &tmpi);
				if (tmpi >= 0)
					machine_number = tmpi;
			}
			else if (bcmp(s, "data_store_path", len) == 0) {
				len = strchr(eq + 1, (int)'\n') - ( eq + 1 );
				if ((len + 1) >= sizeof(data_store_path))
					continue;
				if (*( eq + 1 ) == '/')	//从根路径开始
					snprintf(data_store_path, len + 1, "%s", eq + 1 );
			}
		}
		else if ('[' == c && ':' == c1) {
			//memset(s,0,TEXT_LINE_LEN);
			fgets(s, TEXT_LINE_LEN, fp);
			tmpi = 0;
			len = 0;
			if((eq = strchr(s, ']')) == NULL)
				continue;
			len = eq-s;
			if ( len <= 0 )
				continue;

			if (bcmp(s, "BEGIN", len) == 0) {
				//read ip
				while (!feof(fp)) {
					fgets(s, TEXT_LINE_LEN, fp);
					len = 0;
					if ((eq = strchr(s, (int)'\n')) == NULL)
						continue;
					len = eq - s;
					if (len <= 0)
						continue;
					if (bcmp(s, "[:END]", len) == 0)
						break;
					if ((eq = strchr(s, (int)'=')) == NULL)
						continue;
					len = eq - s;
					if (len <= 0)
						continue;

					a[0] = a[1] = a[2] = a[3] = -1;
					sscanf(eq + 1, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
					for (i = 0; i <= 3; i++) {
						if (a[i] > 255 || a[i] < 0) {
							write_log("[Warning]:illegal probed ip in file \"conf\" [%s],ignored\n", s);
							flag = 1;	//IP有问题！则跳出并置位错误标记为真
							break;
						}
					}//for

					if (!flag) {
						if ((tmpip = (ip_addr_t*)malloc(sizeof(ip_addr_t))) == NULL) {
							write_log("Load_Config:read ip.malloc failed!\n");
							return -1;
						}

						tmpip->ipvalue = a[0]*256*256*256 + a[1]*256*256 + a[2]*256 + a[3];
						tmpip->ip[0] = a[0];
						tmpip->ip[1] = a[1];
						tmpip->ip[2] = a[2];
						tmpip->ip[3] = a[3];//不能用memcpy 因为数据类型不同
						tmpip->next = NULL;
						if (probe_ip_link == NULL)
							probe_ip_link = tmpip;
						else {
							tmpip->next = probe_ip_link;
							probe_ip_link = tmpip;
						}
					}//if legal ip
				}//while each ip
			}//if BEGIN
		}//else if [:
	}//while
	fclose(fp);
	return 0;
}

/*
 *@brief send线程从conf中读入全局设置
 */
int send_load_config()
{
	FILE *fp;			//打开配置文件指针
	int a[4];			//临时存储点分IP的4个部分
	int flag;			//标识IP是否合法
	char s[TEXT_LINE_LEN];		//存储从配置文件中读取的一行数据
	uint32_t i;			//for循环计数
	char c,c1;			//连续从文件的一行中读取的两个字符，用以标识变量、IP、注释行、无用行
	char *eq = NULL;	//字符串中表示"="的指针
	int len = 0;		//读取的字符串长度，如果是一个变量，其标识符的长度要大于0
	uint32_t tmpi = 0;	//临时存储读取的变量数值
	server_ip_t *tmpip = NULL;//指向待探测ip

	fp = fopen("conf", "rt");
	if (fp == NULL) {
		write_log("Can't open config file \"conf\":%s\n", strerror(errno));
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
			//memset(s,0,TEXT_LINE_LEN);
			flag = 0;
			fgets(s,TEXT_LINE_LEN,fp);
			tmpi = 0;
			len = 0;
			//printf("%s",s);
			if ((eq = strchr(s, (int)'=')) == NULL)
				continue;
			len = eq - s;
			if (len <= 0)
				continue;

			if (bcmp(s, "machine_number", len) == 0) {
				sscanf(eq+1, "%u", &tmpi);
				if (tmpi >= 0)
					machine_number = tmpi;
			}
			else if (bcmp(s, "data_store_path", len) == 0) {
				len = strchr(eq + 1, (int)'\n') - ( eq + 1 );
				if ((len + 1) >= sizeof(data_store_path))
					continue;
				if (*( eq + 1 ) == '/')	//从根路径开始
					snprintf(data_store_path, len + 1, "%s", eq + 1 );
			}
			else if (bcmp(s, "server_conn_port", len) == 0) {
				sscanf(eq+1, "%d", &tmpi);
				if (tmpi > 1024 && tmpi < 65536)
					server_conn_port = tmpi;
			}
			else if (bcmp(s, "server_analyze_port", len) == 0) {
				sscanf(eq+1, "%d", &tmpi);
				if (tmpi > 1024 && tmpi < 65536)
					server_analyze_port = tmpi;
			}
			else if (bcmp(s,"if_ddns",len) == 0) {
				sscanf(eq+1, "%d", &tmpi);
				if (tmpi <= 1 && tmpi >= 0)
					if_ddns = tmpi;
			}
			else if (bcmp(s, "epoll_time_out", len) == 0) {
				sscanf(eq+1, "%d", &tmpi);
				if(tmpi >= -1 && tmpi <= 10000)
					epoll_time_out = tmpi;
			}
			else if (bcmp(s, "analyze_server_domainname", len) == 0) {
				len = strchr(eq + 1, '\n') - (eq + 1);
				if ((len + 1) >= sizeof(analyze_server_domainname))
					continue;
				snprintf(analyze_server_domainname, len + 1,"%s", eq + 1 );
			}
			else if (bcmp(s,"des_pass_phrase",len) == 0) {
				len = strchr(eq + 1,'\n') - (eq + 1);
				if ((len + 1) >= sizeof(des_pass_phrase))
					continue;
				snprintf(des_pass_phrase, len + 1, "%s", eq + 1);
			}
			else if (bcmp(s, "analyze_server_ip", len) == 0) {
				a[0] = a[1] = a[2] = a[3] = -1;
				sscanf(eq + 1, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
				for (i = 0; i <= 3; i++) {
					if (a[i] > 255 || a[i] < 0) {
						write_log("[Warning]:illegal server ip in file \"conf\" [%s],ignored\n", s);
						flag = 1;	//IP有问题！则跳出并置位错误标记为真
						break;
					}
				}//for
				if (!flag) {
					if ((tmpip = (server_ip_t*)malloc(sizeof(server_ip_t))) == NULL) {
						write_log("Load_Config:read server.malloc failed!\n");
						return -1;
					}

					tmpip->ipvalue = a[0]*256*256*256 + a[1]*256*256 + a[2]*256 + a[3];
					tmpip->next = NULL;
					if (analyze_server_set == NULL)
						analyze_server_set = tmpip;
					else {
						tmpip->next = analyze_server_set;
						analyze_server_set = tmpip;
					}
				}//if legal ip
			}//server
		}//if $:
	}//while
	fclose(fp);
	return 0;
}

/*
 * @brief 更新全局的时间结构
 */
void get_time_string()
{
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	//2011-05-24-13-04 错误日志中用到的的时间，精确到分钟，以便和分钟数据的存储一致.
	sprintf(timestring.fullstring, "%04d-%02d-%02d-%02d-%02d-%02d",
			1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

	sprintf(timestring.result_string, "%04d-%02d-%02d-%02d-%02d",
				1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min);

	sprintf(timestring.year, "%04d", 1900+p->tm_year);
	sprintf(timestring.month, "%02d", p->tm_mon+1);
	sprintf(timestring.day, "%02d", p->tm_mday);
	sprintf(timestring.hour, "%02d", p->tm_hour);
	sprintf(timestring.min, "%02d", p->tm_min);
	sprintf(timestring.secs, "%02d", p->tm_sec);
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
			 fprintf(stderr, "%s> Change working dir:%s\n",
					 timestring.fullstring, strerror(errno));
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
	//得到打印日志的时间.
	get_time_string();
	//获取程序运行时路径前缀，改变工作目录，找到配置文件、打开错误文件.他们与可执行程序位于同一目录
	if (change_working_path(path) == -1)
		return -1;
	//在当前位置打开日志记录文件
	if (open_log_file() == -1)
		return -1;

	return 0;
}

/*
 * @brief 每轮循环结束后，释放内存，关闭日志
 */
void destroy()
{
	//关闭日志
	close_log_file();
}
