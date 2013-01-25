/*
 * log.c 日志文件的打开与关闭
 *
 *  Created on:	2011-7-4
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

// needed for open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// needed for fprintf vsnprintf
#include <stdio.h>

// needed for exit
#include <stdlib.h>

// needed for strerror
#include <string.h>

// needed for va_list var_start va_end
#include <stdarg.h>

// needed for exit write
#include <unistd.h>

// needed for localtime
#include <time.h>

// needed for strerror
#include <errno.h>

#include "log.h"
//日志文件的打开、格式化写入、关闭

static int log_fd;//日志文件fd

static char log_time[1024];
/*
 * @brief 更新全局的时间结构
 */
static void flush_log_time()
{
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	//2011-05-24-13-04 错误日志中用到的的时间.
	sprintf(log_time, "%04d-%02d-%02d-%02d-%02d-%02d",
			1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
}

/*
 * @brief 打开日志文件
 */
int open_log_file()
{
	if ((log_fd = open("log",
			O_WRONLY | O_CREAT | O_APPEND, /*0644*/
			S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1 ) {
		return -1;
	}
	return 0;
}

/*
 * @brief 关闭日志文件
 */
void close_log_file()
{
	close(log_fd);
}

void write_log(const char *format, ...)
{
	static char buf[size_per_log];
	flush_log_time();
	va_list ap;
	va_start(ap, format);
	int log_time_len = snprintf(buf, sizeof(buf), "%s> ", log_time);
	int n = vsnprintf(buf + log_time_len, sizeof(buf), format, ap);
	va_end(ap);
	write(log_fd, buf, n + log_time_len);
}
