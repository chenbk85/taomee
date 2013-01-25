/*
 * store_data.c
 *
 *将数据队列中的数据取出保存到文件中，这是一个定时任务。
 *而且取数据和往队列中挂数据
 *  Created on:	2011-7-7
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

// needed for file io operation, snprintf(), remove
#include <stdio.h>

// needed for strerror strchr
#include <string.h>

// needed for time localtime_r
#include <time.h>

// needed for stat, mkdir
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// needed for errno
#include <errno.h>

// needed for write_log
#include "log.h"

// needed for file head, store_result_t
#include "protocol.h"

// needed for timestring
#include "initiate.h"

#include "store_data.h"

char data_store_path[FULL_PATH_LEN] = "/home/probe_ip_save";

/*
 * @brief 递归创建目录
 * @param const char *path,	数据存储路径
 * @return int 0表示创建成功，-1表示创建失败
 */
static int my_mkdir(const char *path)
{
	struct stat buf;
	mode_t mode = 0x00fff;
	char tmp_path[FULL_PATH_LEN];
	const char *slash = path;//必然最开始是'/'
	const char *cur;
	uint16_t len;
	if (!*( slash+1 ))
		return 0;	//整个path就是'/'

	while ((slash = strchr(slash+1,(int)'/')) != NULL ) {
		cur = slash;		//保存当前/所在位置
		len = slash-path+1;	//包括了“\0”
		snprintf(tmp_path, len, "%s", path);
		//判断存储目录是否存在
		if (stat(tmp_path, &buf) == -1)
			if (errno == ENOENT) {
				if (mkdir(tmp_path,mode) == -1) {
					write_log("Can't create dir %s :%s\n", tmp_path, strerror(errno));
					return -1;
				}
			}

		//若存在但不是目录
		if( ( buf.st_mode & S_IFDIR ) == 0 ) {
			remove(tmp_path);	//不是一个目录则删除之
			if (mkdir(tmp_path,mode) == -1) {//并创建一个目录mode的设置使得所有人对其有所有权限
				write_log("Can't create dir %s :%s\n", tmp_path, strerror(errno));
				return -1;
			}
		}
	}
	if (*(cur+1)) {	//‘/’后面还有字符表示是最后的字符，则创建全名目录 path 否则是最后的/ 不需要创建
		//判断存储目录是否存在
		if (stat(path,&buf) == -1)
			if (errno == ENOENT)
				if (mkdir(path,mode) == -1) {
					write_log("Can't create dir %s :%s\n", path, strerror(errno));
					return -1;
				}

		//若存在但不是目录
		if (( buf.st_mode & S_IFDIR ) == 0) {
			remove(path);	//不是一个目录则删除之
			if (mkdir(path,mode) == -1) {//并创建一个目录mode的设置使得所有人对其有所有权限
				write_log("Can't create dir %s :%s\n", path, strerror(errno));
				return -1;
			}
		}
	}
	return 0;
}

/*
 * @brief 将一个node维护的recv_link中的所有数据存到一个文件中
 * @param store_result_t *linkhead	也即recv_link的头节点
 * @param uint32_t linkhead的链表长度
 * @return int	0表示成功，-1表示写文件失败.
 */
int write_file(store_result_t *linkhead, uint32_t len)
{
	FILE *resultfile;
	store_result_t *tmpnext = linkhead;
	uint32_t seconds;
	uint32_t zero = 0;

	struct tm p;
	int machine_number;
	//取消子目录，全部放在同一个目录，方便分析程序分析，分析完毕直接删除
	char data_store_path_day[FULL_PATH_LEN];	//存储路径下的日期子目录名.
	char tmp_file_full_path_name[FULL_PATH_LEN];//临时文件全名
	char file_full_path_name[FULL_PATH_LEN];//文件全名,临时文件写完毕后改成这个名字

	if (linkhead == NULL)
		return 0;

	//注意probe_time是32位的,而localtime_r需要的time_t 是64位的
	uint64_t t_probe_time = linkhead->probe_time;
	localtime_r((const time_t*)&t_probe_time, &p);
	machine_number = linkhead->machine_number;

	//sprintf(data_store_path_day,"%s/%04d-%02d-%02d",
	//		data_store_path,p.tm_year+1900,p.tm_mon +1,p.tm_mday);
	sprintf(data_store_path_day,"%s",data_store_path);

	if (my_mkdir( (const char *)data_store_path_day ) == -1)//递归创建目录
		return -1;

	sprintf(tmp_file_full_path_name, "%s/tmp.N%04d.%04d-%02d-%02d-%02d-%02d",
			data_store_path_day,machine_number,
			p.tm_year+1900, p.tm_mon+1, p.tm_mday,
			p.tm_hour, p.tm_min);//以路径和时间命名

	sprintf(file_full_path_name,"%s/N%04d.%04d-%02d-%02d-%02d-%02d",
			data_store_path_day, machine_number,
			p.tm_year+1900, p.tm_mon+1, p.tm_mday,
			p.tm_hour, p.tm_min);//以路径和时间命名

	resultfile = fopen(tmp_file_full_path_name, "wb+");	//以二进制方式写
	if (resultfile == NULL) {
		write_log("Can not open result file %s!!!,please check dir and privilleges\n",
				tmp_file_full_path_name);
		return -1; //直接返回即可 不用结束程序
	}

	seconds = time((time_t*)NULL);
	//写文件头
	fwrite(FILE_HEAD_SYMBOL, 1, FILE_HEAD_LEN, resultfile);//文件标识
	fwrite(&seconds, sizeof(seconds), 1, resultfile);//创建时间
	fwrite(&seconds, sizeof(seconds), 1, resultfile);//最后读取时间
	fwrite(&len, sizeof(len), 1, resultfile);//总记录数
	fwrite(&zero, sizeof(zero), 1, resultfile);//已处理记录数

	//写数据
	while (tmpnext != NULL) {
		fwrite(tmpnext, (sizeof(store_result_t) - sizeof((store_result_t*)0)), 1, resultfile);
		tmpnext = tmpnext->next;
	}

	fclose(resultfile);
	//临时文件更名
	rename(tmp_file_full_path_name, file_full_path_name);

	return 0;
}
