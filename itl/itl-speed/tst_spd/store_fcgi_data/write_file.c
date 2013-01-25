/*
 * write_file.c
 *
 * Created on:	2011-8-4
 * Author:		Singku
 * Paltform:		Linux Fedora Core 8 x86-32
 *	Compiler:		GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <libtaomee/timer.h>
#include <libtaomee/log.h>
#include "proto.h"
#include "initiate.h"
#include "recv_data.h"
#include "write_file.h"

tmr_t g_timer;//定时将内存数据写到磁盘
uint32_t current_version = 1;//当前版本
uint32_t data_store_time = 10;//默认10分钟
char data_store_path[1024] = "/home/cgi_data";

int called_count = 0;
//static fcgi_store_t aexample;

int write_file()
{
	if (called_count == 0)
		goto add_timer;

	time_t timep;
	struct tm *p;
	time(&timep);
	timep = timep - (timep % (data_store_time * 60));
	p = localtime(&timep);

	char path_today_cdn[1024];
	char path_today_now_cdn[1024];
	char path_today_now_cdn_tmp[1024];
	char path_cdn_special[1024];
	snprintf(path_today_cdn, sizeof(path_today_cdn),"%s/ver_%03d/cdn/%4d-%02d-%02d",
			data_store_path, current_version, 1900 + p->tm_year, p->tm_mon + 1, p->tm_mday);
	snprintf(path_today_now_cdn, sizeof(path_today_now_cdn), "%s/META-%4d-%02d-%02d-%02d-%02d-%02d",
			path_today_cdn, 1900 + p->tm_year, p->tm_mon + 1, p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);
	snprintf(path_today_now_cdn_tmp, sizeof(path_today_now_cdn_tmp), "%s/tmp.%4d-%02d-%02d-%02d-%02d-%02d",
			path_today_cdn, 1900 + p->tm_year, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	snprintf(path_cdn_special, sizeof(path_cdn_special),"%s/ver_%03d/cdn_special.dat",
			data_store_path, current_version);

	if (my_mkdir(path_today_cdn) == -1)
		return -1;
	int fd_cdn = open(path_today_now_cdn, O_RDWR | O_CREAT | O_APPEND, 00666);
	int fd_cdn_special = open(path_cdn_special, O_RDWR | O_CREAT | O_APPEND, 00666);

	char path_today_url[1024];
	char path_today_now_url[1024];
	char path_today_now_url_tmp[1024];
	char path_url_special[1024];
	snprintf(path_today_url, sizeof(path_today_url),"%s/ver_%03d/url/%4d-%02d-%02d",
			data_store_path, current_version, 1900 + p->tm_year, p->tm_mon + 1, p->tm_mday);
	snprintf(path_today_now_url, sizeof(path_today_now_url), "%s/META-%4d-%02d-%02d-%02d-%02d-%02d",
			path_today_url, 1900 + p->tm_year, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	snprintf(path_today_now_url_tmp, sizeof(path_today_now_url_tmp), "%s/tmp.%4d-%02d-%02d-%02d-%02d-%02d",
			path_today_url, 1900 + p->tm_year, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	snprintf(path_url_special, sizeof(path_url_special),"%s/ver_%03d/url_special.dat",
			data_store_path, current_version);

	if (my_mkdir(path_today_url) == -1)
		return -1;
	int fd_url = open(path_today_now_url, O_RDWR | O_CREAT | O_APPEND, 00666);
	int fd_url_special = open(path_url_special, O_RDWR | O_CREAT | O_APPEND, 00666);

	int tag = g_buffer_tag; //占用当前缓冲区
	g_buffer_tag ^= 1; //切换缓冲区 使得recv函数的数据写到另一个缓冲区.


	fcgi_store_t *record = (fcgi_store_t*)g_buffer[tag];
	int i;
	int data_len = sizeof(fcgi_store_t);;
	for (i = 0; i < sum_data_in_mem[tag]; i++) {
		switch (record->type) {
		case proto_fcgi_cdn_report: {
				write(fd_cdn, record, data_len);
				if (record->special_flag == 1) {
					write(fd_cdn_special, record, data_len);
				}
			}
			break;
		case proto_fcgi_url_report: {
				write(fd_url, record, data_len);
				if (record->special_flag == 1) {
					write(fd_url_special, record, data_len);
				}
			}
			break;
		default:
			break;
		}//switch
		record ++;
	}//for
	char zero[1024];
	memset(zero, 0x0, sizeof(zero));
	write(fd_cdn, zero, data_len);	//cdn分钟文件之间全0隔开
	write(fd_url, zero, data_len);	//url分钟文件之间全0隔开
	close(fd_cdn);
	close(fd_url);
	close(fd_cdn_special);
	close(fd_url_special);
	//rename(path_today_now_cdn_tmp, path_today_now_cdn);
	//rename(path_today_now_url_tmp, path_today_now_url);

	g_buffer_ptr[tag] = g_buffer[tag];
	sum_data_in_mem[tag] = 0;

add_timer:
	called_count++;
	ADD_TIMER_EVENT(&g_timer, write_file, NULL,
			get_now_tv()->tv_sec +  60 - get_now_tv()->tv_sec % 60);

	return 0;
}
