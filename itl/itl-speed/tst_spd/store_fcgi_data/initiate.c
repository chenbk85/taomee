/*
 * load_conf.c
 *
 * Created on:	2011-8-4
 * Author:		Singku
 * Paltform:		Linux Fedora Core 8 x86-32
 *	Compiler:		GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <libtaomee/conf_parser/config.h>
#include <libtaomee/log.h>

#include "net_if.h"
#include "write_file.h"
#include "recv_data.h"

int data_forward = 0;
char forward_ip[16] = "10.1.1.58";
uint16_t forward_port = 8888;
int forward_fd = -1;

int my_mkdir(const char *path)
{
	struct stat buf;
	mode_t mode = 0x00fff;
	char tmp_path[1024];
	const char *slash=path;//必然最开始是'/'
	const char *cur;
	uint16_t len;
	if( !*( slash+1 ) )
		return 0;	//整个path就是'/'

	while ( (slash = strchr(slash+1,(int)'/') ) != NULL ) {
		cur=slash;//保存当前/所在位置
		len=slash-path+1;	//包括了“\0”
		snprintf(tmp_path,len,"%s",path);
		//判断存储目录是否存在
		if ( stat(tmp_path,&buf) == -1 ) {
			if(errno == ENOENT) {
				if (mkdir(tmp_path,mode) != 0)
					ERROR_RETURN(("mkdir %s,failed:%s\t", tmp_path, strerror(errno)), -1);
			}
		}
		//若存在但不是目录
		if( ( buf.st_mode & S_IFDIR ) == 0 ) {
			ERROR_RETURN(("mkdir:%s is an existed regular file.\t", tmp_path), -1);
			remove(tmp_path);	//不是一个目录则删除之
			mkdir(tmp_path,mode);//并创建一个目录mode的设置使得所有人对其有所有权限
		}
	}
	if(*(cur+1)) {//‘/’后面还有字符表示是最后的字符，则创建全名目录 path 否则是最后的/ 不需要创建
		//判断存储目录是否存在
		if( stat(path,&buf) == -1 ) {
			if(errno == ENOENT) {
				if (mkdir(path,mode) != 0)
					ERROR_RETURN(("mkdir %s,failed:%s\t", path, strerror(errno)), -1);
			}
		}
		//若存在但不是目录
		if( ( buf.st_mode & S_IFDIR ) == 0 ) {
			ERROR_RETURN(("mkdir:%s is an existed regular file.\t", path), -1);
			remove(path);	//不是一个目录则删除之
			mkdir(path,mode);//并创建一个目录mode的设置使得所有人对其有所有权限
		}
	}
	return 0;
}

static int build_dir()
{
	char cdn[1024];
	char url[1024];
	snprintf(cdn, sizeof(cdn), "%s/ver_%03d/cdn",data_store_path, current_version);
	snprintf(url, sizeof(url), "%s/ver_%03d/url",data_store_path, current_version);

	return (	my_mkdir(cdn)
			|	my_mkdir(url));
}

int initiate()
{
	int tmp_data_store_time;
	int tmp_memory_size;
	int tmp_current_version;
	char *str;

	tmp_data_store_time = config_get_intval("data_store_time", data_store_time);
	if (tmp_data_store_time >= 1 && tmp_data_store_time <= 1440)	//minutes 最长24小时存一个文件
		data_store_time = tmp_data_store_time;

	tmp_memory_size = config_get_intval("memory_size", memory_size);
	if (tmp_memory_size >= 1024*1024 && tmp_memory_size <= 1024*1024*1024)	//Bytes
		memory_size = tmp_memory_size;

	tmp_current_version = config_get_intval("current_version", current_version);
	if (tmp_current_version >= 1 && tmp_current_version < 1000)	//Bytes
		current_version = tmp_current_version;

	str = config_get_strval("data_store_path");
	if (str != NULL )
		strncpy(data_store_path, str, sizeof(data_store_path));
	else
		ERROR_RETURN(("can't load data_store_path"), -1);

	//added start for forward fcgi
	int tmp_data_forward;
	tmp_data_forward = config_get_intval("data_forward", data_forward);
	if (tmp_data_forward >= 0 && tmp_data_forward <= 1)	//
		data_forward = tmp_data_forward;
	if (data_forward) {
		str = config_get_strval("forward_ip");
		if (str != NULL )
			strncpy(forward_ip, str, sizeof(forward_ip));
		else
			ERROR_RETURN(("can't load forward_ip"), -1);

		int tmp_forward_port = config_get_intval("forward_port", forward_port);
		if (tmp_forward_port > 1024 && tmp_forward_port < 65535)
			forward_port = tmp_forward_port;

		//连接服务器
		forward_fd = connect_to_svr(forward_ip, forward_port, 1024, 1);
	}
	//added end for forward fcgi


	if (build_dir() == -1)
		return -1;

	g_buffer[0] = (char*)malloc(memory_size*2);
	if (g_buffer[0] == NULL)
		ERROR_RETURN(("malloc %u memory failed,please reset the memory_size variable",memory_size*2), -1);
	g_buffer[1] = g_buffer[0] + memory_size;

	g_buffer_ptr[0] = g_buffer[0];
	g_buffer_ptr[1] = g_buffer[1];

	return 0;

}
