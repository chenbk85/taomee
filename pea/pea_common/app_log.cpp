#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "log.hpp"
#include "app_log.hpp"




//static const char* loglevel_strs[] = { "ERROR", "WARNING", "INFO", "DEBUG" };

#ifdef ENABLE_GONGFU_LOG

static LOG_HANDLE gongfu_log_handle = NULL;

int gongfu_log_open(const char* url)
{
	gongfu_log_handle = log_open(url);	
	return gongfu_log_handle!=NULL ? 0:-1;
}

int gongfu_log_close()
{
	if( gongfu_log_handle == NULL)return 0;
	return log_close(gongfu_log_handle);	
}

void gongfu_log_write(int level, const char* src, const char* fmt, ...)
{
	time_t timet;	
	struct tm time_tm;	
	char buf[10240];	
	int count;	
	va_list valist;

	if( gongfu_log_handle == NULL)return;

	time(&timet);	
	memcpy(&time_tm, localtime(&timet), sizeof(time_tm));

	sprintf(buf, "[%s][%s][%02d-%02d %02d:%02d:%02d] ", 
			loglevel_strs[level], src,
			time_tm.tm_mon+1, time_tm.tm_mday, 
			time_tm.tm_hour, time_tm.tm_min, time_tm.tm_sec);
			count = (int)strlen(buf);
			va_start(valist, fmt);	
			
	count += vsnprintf(buf+count, sizeof(buf)-count, fmt, valist);	
	va_end(valist);	
	log_puts(gongfu_log_handle, level, buf, count);	
}


#endif
