/*
 * =====================================================================================
 *
 *       Filename:  logsavefile.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年12月03日 14时58分44秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include <assert.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
//文件名
char logdir[200]="protolog/log";
static inline void set_log_file_name(char* file_name,struct tm* tm)
{
	sprintf (file_name, "%s/%s/%04d%02d%02d%02d",getenv("HOME"),logdir, 
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,tm->tm_hour);
	//保存当前的日志时段 
}

static int open_logfd(struct tm* now)
{
	char file_name[FILENAME_MAX];
	int val;

	set_log_file_name ( file_name, now);
	int logfd = open (file_name, O_WRONLY|O_CREAT|O_APPEND, 0644);
	if (logfd > 0){
		val = fcntl(logfd, F_GETFD, 0);
		val |= FD_CLOEXEC;
		fcntl(logfd, F_SETFD, val);
	}
	return logfd;
}

int write_proto_log (char * buf)
{
	int logfd=-1; 
	struct tm tm;
	time_t now=time(NULL);
	int bufsize=*((int*)buf);	
	localtime_r(&now, &tm);
	if ((logfd=open_logfd(&tm))==-1)
		return -1;
	bufsize=write (logfd, buf,bufsize);
	close(logfd);		
	return bufsize;
}
