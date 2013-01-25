/*
 * initiate.c
 *
 *  Created on:	2011-7-14
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

/*
 * 初始化
 * 	加载配置文件
 * 		加载端口号
 * 		加载IP列表
 * 		加载域名
 *		加载数据发送的加密密码
 * 	选择合适IP
*/
#include <stdlib.h>
#include <fcgi_stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "log.h"
#include "ddns.h"
#include "initiate.h"

#define	FULL_PATH_LEN 1024
#define	MAX_LINE_LEN 4096

server_ip_t	*analyze_server_set=NULL;
char		analyze_server_domainname[256]="velocitytest.taomee.com";
uint16_t	server_analyze_port=8888;
uint16_t	server_conn_port=7777;
uint32_t	data_backup_server = 3232273940; //"192.168.150.20";
uint16_t	data_backup_port = 9999;
char		des_pass_phrase[16]="TAOMEEV5";
uint8_t		if_ddns = 1;
uint8_t		if_choose_server;
uint8_t		if_log_data = 0;
int			time_out = 2000;
time_string_t timestring;

/*
 * @brief 得到当前运行时时间，以便打印错误日志时记录时间
 *
 */
void get_time_string()
{
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	//2011-05-24-13-04 错误日志中用到的的时间，精确到分钟，以便和分钟数据的存储一致.
	sprintf(timestring.fullstring,"%04d-%02d-%02d-%02d-%02d-%02d",
			1900+p->tm_year,1+p->tm_mon,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	sprintf(timestring.result_string,"%04d-%02d-%02d-%02d-%02d",
				1900+p->tm_year,1+p->tm_mon,p->tm_mday,p->tm_hour,p->tm_min);

	sprintf(timestring.year,"%04d",1900+p->tm_year);
	sprintf(timestring.month,"%02d",p->tm_mon+1);
	sprintf(timestring.day,"%02d",p->tm_mday);
	sprintf(timestring.hour,"%02d",p->tm_hour);
	sprintf(timestring.min,"%02d",p->tm_min);
	sprintf(timestring.secs,"%02d",p->tm_sec);
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
 *  @brief 一轮数据包发送完毕后释放服务器列表
 */
void free_server_set()
{
	server_ip_t *tmp,*tnext;
	tnext = analyze_server_set;
	while(tnext != NULL) {
		tmp = tnext;
		tnext = tnext->next;
		free(tmp);
	}
	analyze_server_set = NULL;
}

/*
 *  @brief 从fcgi.config中加载配置文件
 */
static int load_config()
{
	FILE *fp;		//打开配置文件指针
	int a[4];		//临时存储点分IP的4个段
	int flag;		//标识IP是否合法
	char s[MAX_LINE_LEN];	//存储从配置文件中读取的一行数据
	uint32_t i;		//for循环计数
	char c,c1;		//连续从文件的一行中读取的两个字符，用以标识变量、IP、注释行无用行
	char *eq = NULL;//字符串中表示"="的指针
	int len = 0;	//读取的字符串长度，如果是一个变量，其标识符的长度要大于0
	int tmpi = 0;	//临时存储读取的变量数值
	server_ip_t *tmpip = NULL;//指向待探测ip

	fp = fopen("fcgi.conf","rt");
	if( fp == NULL ) {
		write_log("can't open file 'fcgi.conf'\n");
		return -1;
	}
	while( !feof(fp) ) {
		c = fgetc(fp);
		if( (int)c == -1 ) {	//EOF==-1
			break;
		}
		if( c != '[' && c != '$' ) {
			while( c != '\n' && !feof(fp)) {
				c = fgetc(fp);
			}
			continue;
		}
		if( (c1 = fgetc(fp)) != ':' ) {
			while( c != '\n' && !feof(fp)) {
				c = fgetc(fp);
			}
			continue;
		}

		if( '$' == c && ':' ==c1 ) {
			//memset(s,0,1024);
			flag = 0;
			fgets(s,MAX_LINE_LEN,fp);
			tmpi = 0;
			len = 0;
			//printf("%s",s);
			if( (eq = strchr(s,'=')) == NULL )
				continue;
			len = eq - s;
			if( len <= 0 )
				continue;

			if( bcmp(s,"server_analyze_port",len) == 0 ) {
				sscanf(eq+1,"%u",&tmpi);
				if( tmpi > 1024 && tmpi < 65536 )
					server_analyze_port = tmpi;
			}
			else if( bcmp(s,"server_conn_port",len) == 0 ) {
				sscanf(eq+1,"%u",&tmpi);
				if( tmpi > 1024 && tmpi < 65536 )
					server_conn_port = tmpi;
			}
			else if( bcmp(s,"if_ddns",len) == 0 ) {
				sscanf(eq+1,"%u",&tmpi);
				if( tmpi <= 1 && tmpi >= 0 )
					if_ddns = tmpi;
			}
			else if( bcmp(s,"if_log_data",len) == 0 ) {
				sscanf(eq+1,"%u",&tmpi);
				if( tmpi <= 1 && tmpi >= 0 )
					if_log_data = tmpi;							            
			}
			else if( bcmp(s,"if_choose_server",len) == 0 ) {
				sscanf(eq+1,"%u",&tmpi);
				if( tmpi <= 1 && tmpi >= 0 )
					if_choose_server = tmpi;
			}
			else if( bcmp(s,"time_out",len) == 0 ) {
				sscanf(eq+1,"%u",&tmpi);
				if( tmpi >= -1 && tmpi <= 10000 )
					time_out = tmpi;
			}
			else if( bcmp(s,"des_pass_phrase",len) == 0 ) {
				len = strchr(eq + 1,'\n') - ( eq + 1 );
				if( (len + 1) >= sizeof(des_pass_phrase))
					continue;
				snprintf(des_pass_phrase,len + 1,"%s",eq + 1 );
			}
			else if ( bcmp(s,"data_backup_server",len) == 0 ) {
				a[0] = a[1] = a[2] = a[3] = -1;
				sscanf(eq + 1,"%d.%d.%d.%d",&a[0],&a[1],&a[2],&a[3]);
				for (i = 0;i < 3;i++) {
					if (a[i] > 255 || a[i] < 0) {
						write_log("Illegal backup server ip in conf file [%s],ignored\n", s);
						flag = 1;	//IP有问题！则跳出并置位错误标记为真
						break;
					}
				}//for
				if (!flag)
					data_backup_server = a[0]*256*256*256 + a[1]*256*256 + a[2]*256 + a[3];
			}
			else if( bcmp(s,"data_backup_port",len) == 0 ) {
				sscanf(eq+1,"%u",&tmpi);
				if( tmpi > 1024 && tmpi <= 65535 )
					data_backup_port = tmpi;
			}
			else if( bcmp(s,"analyze_server_domainname",len) == 0 ) {
				len = strchr(eq + 1,'\n') - ( eq + 1 );
				if((len + 1) >= sizeof(analyze_server_domainname))
					continue;
				snprintf(analyze_server_domainname,len + 1,"%s",eq + 1 );
			}
			else if( bcmp(s,"analyze_server_ip",len) == 0 ) {
				a[0] = a[1] = a[2] = a[3] = -1;
				sscanf(eq + 1,"%d.%d.%d.%d",&a[0],&a[1],&a[2],&a[3]);
				for(i = 0;i < 3;i++) {
					if( a[i] > 255 || a[i] < 0 ) {
						write_log("Illegal server ip in conf file [%s],ignored\n", s);
						flag = 1;	//IP有问题！则跳出并置位错误标记为真
						break;
					}
				}//for
				if( !flag ) {
					if( (tmpip = (server_ip_t*)malloc(sizeof(server_ip_t))) == NULL ) {
						write_log("Load_Config:read server.malloc failed!\n");
						fclose(fp);
						return -1;
					}

					tmpip->ipvalue = a[0]*256*256*256 + a[1]*256*256 + a[2]*256 + a[3];
					tmpip->next = NULL;
					if( analyze_server_set == NULL )
						analyze_server_set = tmpip;
					else {
						tmpip->next = analyze_server_set;
						analyze_server_set = tmpip;
					}
				}//if legal ip
			}//server
		}//variable
	}//while
	fclose(fp);
	return 0;
}

/*
 *  @brief 初始化各类信息
 */
int initiate(char *path)
{
	static int run_count = 0;
	get_time_string();//打印日志的时间，和探测时间
	if(run_count == 0)
	{
		run_count ++;
		if (change_working_path(path) == -1)
			return -1;
		if (open_log_file("fcgi.log") == -1)
			return -1;
	}
	free_server_set();
	if (load_config() == -1)//读取配置文件，包括服务器IP列表.
		return -1;

	if(if_ddns)		//解析DNS成IP地址,和服务器IP列表一起组成数据接收服务器集
		if (dns_resolve(analyze_server_domainname) == -1)
			return -1;
	return 0;
}

void destroy()
{
	close_log_file();
	free_server_set();
}

