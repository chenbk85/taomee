/*
 * main.c
 *
 *  Created on:	2011-7-14
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */


#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
//#include <stdio.h> fcgi_stdio.h 重写了stdio.h
#include <string.h>

#include <fcgi_stdio.h>
#include <pthread.h>
#include "cgic.h"
#include "initiate.h"
#include "data_manipulate.h"

int analyze_fd;
int backup_fd;

uint32_t analyze_broken = 0;

int main(int argc, char **argv)
{

	if (initiate(argv[0]) == -1)
		return -1;

	if (connect_2_analyze_server(&analyze_fd) == -1)
		analyze_broken = 1;
	else
		analyze_broken = 0;
	connect_2_backup_server(&backup_fd);

	store_result_t result;
	while( FCGI_Accept() >= 0) {
		//用cgic初始化获取post数据
		cgic_initiate(0, 0);
		FCGI_printf("Content-type:image/gif\r\n\r\n");
		//判断数据合法性，如合法则放入result中
		if( prepare_data(&result) == -1)
			goto continue_tag;

		//先发到缓存
		if (backup_fd == -1)
			connect_2_backup_server(&backup_fd);//重新连接
		if (backup_fd != -1)
			send_2_server(&result, &backup_fd, "backup", analyze_broken);


		//将数据发送出去
		if (analyze_fd == -1) {
			if (connect_2_analyze_server(&analyze_fd) == -1)//重新连接
				analyze_broken = 1;
			else
				analyze_broken = 0;
		}
		if(analyze_fd != -1)
			send_2_server(&result, &analyze_fd, "analyze", 0);

continue_tag:
		//释放cgic中取数据所分配的内存
		cgiFreeResources();
		FCGI_Finish();
	}
	close(analyze_fd);
	close(backup_fd);
	destroy();
	return 1;
}
