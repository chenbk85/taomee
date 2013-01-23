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
#include <errno.h>
#include "log.hpp"

#define FILE_BUFFER_SIZE 2*1024*1024


#define FILE_MODE_SINGLE		0
#define FILE_MODE_ONCE			1
#define FILE_MODE_EVERYDAY		2
#define FILE_MODE_EVERYWEEK		3
#define FILE_MODE_EVERYMOUTH	4

typedef struct FILE_STREAM_CTX {
		FILE*			fp;	
		char			path[200];	
		char			name[32];	
		char			filename[300];	
		int				mode;	
		time_t			covertime;
}FILE_STREAM_CTX;


typedef struct LOG_CTX{
		struct {
			char* 			buf;
			uint32_t    	cur;
			uint32_t    	len;
			uint32_t    	max;
			pthread_t   	thread;
			sem_t       	inque;
			sem_t       	ouque;
			pthread_mutex_t mtx;
			int         	inque_size;
			int         	ouque_size;
		}stream;
		FILE_STREAM_CTX     *file;
}LOG_CTX;

static uint32_t  log_thread(void* arg);
static int stream_write(LOG_CTX* ctx, const char* msg, uint32_t len);
static int stream_read(LOG_CTX* ctx, char* msg, uint32_t len);
//static const char* get_level_string(int level);
static int file_open(LOG_CTX* ctx, const char* url);
static int file_close(LOG_CTX* ctx);
static int file_write(LOG_CTX* ctx, int level, const char* msg, uint32_t len);

static FILE* file_create(FILE_STREAM_CTX* ctx);

int os_isdir(const char* dir)
{
	int ret = 0;	
	struct stat s;	
	ret = stat(dir, &s);	
	return ret==0 && (s.st_mode&S_IFDIR)!=0;
}

int os_mkdir(const char* dir)
{
	int ret = 0;	
	struct stat s;	
	char tmp[200];	
	const char* cur;	
	const char * cur1;
    const char * cur2;	
	ret = stat(dir, &s);	
	if(ret==0) {
			return (s.st_mode&S_IFDIR)==0?-1:0;	
	}	
	cur = dir;	
	while(1) 
	{
		cur1 = strchr(cur, '/');		
		cur2 = strchr(cur, '\\');		
		if(cur1==cur++) continue;		
		if(cur1==NULL && cur2==NULL) break;		
		if(cur1!=NULL && cur2!=NULL && cur1>cur2) cur1 = cur2;		
		if(cur1==NULL) cur1=cur2;		
		strcpy(tmp, dir);		
		tmp[cur1-dir] = '\0';		
		ret = stat(tmp, &s);		
		if(ret==0) {
			if((s.st_mode&S_IFDIR)==0) {
				return -1;			
			} else {
				continue;			
			}		
		}		
		ret = mkdir(tmp, S_IRWXU | S_IRWXG | S_IRWXO);		
		if(ret== -1)return -1;
	}	
	return 0;
}


static struct {
	const char* name;
	int         tbuflen;
	int         size;
	int (*func_open)(LOG_CTX* ctx, const char* url);	
	int (*func_close)(LOG_CTX* ctx);	
	int (*func_write)(LOG_CTX* ctx, int level, const char* msg, unsigned int msglen);
} map[] = {
	{ "file://",	FILE_BUFFER_SIZE,	sizeof(FILE_STREAM_CTX),	file_open,	file_close,	file_write }
};

LOG_HANDLE log_open(const char* url)
{
	int ret =0;
	
	LOG_CTX* ctx = NULL;

	if( memcmp(map[0].name, url, strlen(map[0].name))==0){
		ctx = (LOG_CTX*)malloc(sizeof(LOG_CTX) + map[0].size+map[0].tbuflen);
		memset(ctx, 0, sizeof(LOG_CTX)+map[0].size+map[0].tbuflen);
	}

	if(ctx == NULL)return NULL;

	ctx->file  = ( FILE_STREAM_CTX* )(ctx+1);
	if( map[0].tbuflen == 0){
		memset(&ctx->stream, 0, sizeof(ctx->stream));		
		return ctx;
	}

	ctx->stream.buf		= (char*)ctx + sizeof(LOG_CTX) + map[0].size;
	ctx->stream.cur		= 0;
	ctx->stream.len		= 0;
	ctx->stream.max		= map[0].tbuflen;

	sem_init(&ctx->stream.inque, 0, 0);
	sem_init(&ctx->stream.ouque, 0, 0);
	pthread_mutex_init(&ctx->stream.mtx, NULL);
	ctx->stream.inque_size	= 0;
	ctx->stream.ouque_size	= 0;

	ret = map[0].func_open(ctx, url+strlen(map[0].name));	
	if(ret!= 0) {
		 free(ctx); return NULL; 
	}
	
	pthread_create( &ctx->stream.thread, NULL,  (void* (*)(void*))log_thread, (void*)ctx);	

	return ctx;
}


int log_close(LOG_HANDLE ctx)
{
	if( ctx->stream.buf != NULL){
		sem_post( &ctx->stream.inque);
		pthread_join(ctx->stream.thread, NULL);
		sem_destroy( &ctx->stream.inque);
		sem_destroy( &ctx->stream.ouque);
		pthread_mutex_destroy( &ctx->stream.mtx);
	}

	map[ 0 ].func_close(ctx);
	free(ctx);
	return 0;
}

int log_write(LOG_HANDLE ctx, int level, const char* fmt, ...)
{
	char buf[2*1024];
	int count;
	va_list argptr;

	va_start(argptr, fmt);
	count = vsnprintf(buf, sizeof(buf), fmt, argptr);
	va_end(argptr);

	return log_puts(ctx, level, buf, count); 
}


int log_puts(LOG_HANDLE ctx, int level, const char* msg, unsigned int msglen)
{
	if( msglen == 0) msglen = (unsigned int)strlen(msg);
	if( ctx->stream.buf == NULL) return map[0].func_write(ctx, level, msg, msglen);

	while(1){
		pthread_mutex_lock(&ctx->stream.mtx);		
		if(ctx->stream.len + sizeof(msglen)+msglen <= ctx->stream.max) break;		
		ctx->stream.ouque_size++;		
		pthread_mutex_unlock(&ctx->stream.mtx);		
		os_sem_wait(&ctx->stream.ouque);	
	}
	
	stream_write(ctx, (char*)&msglen, (unsigned int)sizeof(msglen));	
	stream_write(ctx, msg, msglen);	
	ctx->stream.inque_size++;
	
	if( ctx->stream.ouque_size > 0){
		ctx->stream.ouque_size--;
		sem_post(&ctx->stream.ouque);
	}
	sem_post(&ctx->stream.inque);
	pthread_mutex_unlock(&ctx->stream.mtx);

	return 0;
}


uint32_t  log_thread(void* arg)
{
	LOG_CTX* ctx = (LOG_CTX*)arg;	
	unsigned int msglen = ctx->stream.len;	
	char buf[1000];	int level = 0;
	while(1){
		os_sem_wait(&ctx->stream.inque);
		pthread_mutex_lock(&ctx->stream.mtx);
		if(ctx->stream.inque_size==0) {
			 pthread_mutex_unlock(&ctx->stream.mtx); 
			 break; 
		}
		
		stream_read(ctx, (char*)&msglen, (unsigned int)sizeof(msglen));		
		stream_read(ctx, buf, msglen);		
		buf[msglen] = '\0';

		if( ctx->stream.ouque_size > 0){
			ctx->stream.ouque_size--;
			sem_post(&ctx->stream.ouque);
		}

		ctx->stream.inque_size--;
		pthread_mutex_unlock(&ctx->stream.mtx);
		map[0].func_write(ctx, level, buf, msglen);
	}
	return 0;
}

int stream_write(LOG_CTX* ctx, const char* buf, unsigned int len)
{
	int start;	
	if(ctx->stream.max < ctx->stream.len + len) return -1;	
	start = ( ctx->stream.cur + ctx->stream.len) % ctx->stream.max;	
	if(start + len > ctx->stream.max) {
		memcpy(ctx->stream.buf+start, buf, ctx->stream.max - start);		
		memcpy(ctx->stream.buf, buf+ctx->stream.max-start, len - (ctx->stream.max - start));	
	} else {
		memcpy(ctx->stream.buf+start, buf, len);	
	}	
	ctx->stream.len += len;	
	return 0;
}


int stream_read(LOG_CTX* ctx, char* buf, unsigned int len)
{
	if( ctx->stream.len < len)return -1;
	if( ctx->stream.cur+len>ctx->stream.max) {
		int elen = ctx->stream.max-ctx->stream.cur;		
		memcpy(buf, ctx->stream.buf+ctx->stream.cur, elen);		
		memcpy(buf + elen, ctx->stream.buf, len-elen);
	}else {
		memcpy(buf, ctx->stream.buf+ctx->stream.cur, len );
	}
	ctx->stream.cur = (ctx->stream.cur+len) % ctx->stream.max;	
	ctx->stream.len -= len;
	return 0;
}

/*
const char* get_level_string(int level)
{
		switch(level) 
		{
			case LOG_ERROR:		
				return "ERROR";	
			case LOG_WARNING:	
				return "WARNING";	
			case LOG_INFO:		
				return "INFO";	
			case LOG_DEBUG:		
				return "DEBUG";	
			default:			
				return "";	
		}
}

*/

int file_open(LOG_CTX* ctx, const char* url)
{
	const char* v = NULL;
    v = strstr(url, "@");
	if(v){
		char mode[20];		
		memcpy(&mode, url, v-url);		
		mode[v-url] = '\0';		
		if(strcmp(mode, "single")==0) {
			ctx->file->mode = FILE_MODE_SINGLE;		
		} else if(strcmp(mode, "once")==0) {
			ctx->file->mode =  FILE_MODE_ONCE;		
		} else if(strcmp(mode, "day")==0) {
			ctx->file->mode =  FILE_MODE_EVERYDAY;		
		} else if(strcmp(mode, "week")==0) {
			ctx->file->mode =  FILE_MODE_EVERYWEEK;		
		} else if(strcmp(mode, "mouth")==0) {
			ctx->file->mode =  FILE_MODE_EVERYMOUTH;		
		} else {
			return -1;		
		}		
		url = v+1;
	} else {
		ctx->file->mode = FILE_MODE_SINGLE;
	}

	v = strrchr(url, '/');	
	if(v==NULL) v = strrchr(url, '\\');	
	if(v==NULL) v = url; 
	else v++;	
	strcpy(ctx->file->name, v);	
	memset(ctx->file->path, 0, sizeof(ctx->file->path));	
	memcpy(ctx->file->path, url, v-url);	
	if(!os_isdir(ctx->file->path) && os_mkdir(ctx->file->path)!=0)		
		return -1;	
	
	ctx->file->fp = file_create(ctx->file);	
	if(ctx->file->fp==NULL) return -1;	
	return 0;
}

int file_close(LOG_CTX* ctx)
{
	fclose(ctx->file->fp);
	return 0;
}	


int file_write(LOG_CTX* ctx, int level, const char* msg, unsigned int len)
{
	time_t curtime;	
	curtime = time(NULL);	
	if(ctx->file->fp==NULL || (ctx->file->covertime>0 && ctx->file->covertime < curtime)) 
	{
		if(ctx->file->fp) fclose(ctx->file->fp);		
		ctx->file->fp = NULL;		
		ctx->file->fp = file_create(ctx->file);		
		if(ctx->file->fp==NULL) return -1;	
	}	
	fputs(msg, ctx->file->fp);	
	fputs("\n", ctx->file->fp);	
	return 0;
}


FILE* file_create(FILE_STREAM_CTX *ctx)
{
	time_t timet;	
	struct tm time_tm;	
	const char* mode;	
	char filename[200];	
	switch(ctx->mode) 
	{
		case FILE_MODE_SINGLE:		
			sprintf(filename, "%s%s.log", ctx->path, ctx->name);		
			ctx->covertime = 0;		
			mode = "a";		
		break;	
			
		case FILE_MODE_ONCE:		
			sprintf(filename, "%s%s-%d.log", ctx->path, ctx->name, getpid());		
			ctx->covertime = 0;		
			mode = "w";		
		break;	
			
		case FILE_MODE_EVERYDAY:		
			timet = time(NULL);		
			memcpy(&time_tm, localtime(&timet), sizeof(time_tm)); 		
			sprintf(filename, "%s%s_%04d%02d%02d", ctx->path, ctx->name, time_tm.tm_year+1900, time_tm.tm_mon+1,time_tm.tm_mday); 				ctx->covertime = timet - timet % (60 * 60 * 24) + 60* 60 * 24;		
			mode = "a";		
		break; 	
		
		case FILE_MODE_EVERYWEEK:		
			timet = time(NULL);		
			memcpy(&time_tm, localtime(&timet), sizeof(time_tm));		
			timet -= time_tm.tm_wday * 3600 * 24;		
			memcpy(&time_tm, localtime(&timet), sizeof(time_tm)); 		
			sprintf(filename, "%s%s-%04d%02d%02d", ctx->path, ctx->name, time_tm.tm_year+1900, time_tm.tm_mon+1,time_tm.tm_mday);				time_tm.tm_hour = 0; 
			time_tm.tm_sec = 0; 
			time_tm.tm_min = 0;		
			ctx->covertime = mktime(&time_tm) + 60 * 60 * 24 * 7;		
			mode = "a"; 		
		break;	 	
		
		case FILE_MODE_EVERYMOUTH:		
			timet = time(NULL);		
			memcpy(&time_tm, localtime(&timet), sizeof(time_tm));		
			timet -= time_tm.tm_mday * 3600 * 24;		
			memcpy(&time_tm, localtime(&timet), sizeof(time_tm)); 		
			sprintf(filename, "%s%s-%04d%02d", ctx->path, ctx->name, time_tm.tm_year+1900, time_tm.tm_mon+1);		
			time_tm.tm_hour = 0; 
			time_tm.tm_sec = 0; 
			time_tm.tm_min = 0;		
			ctx->covertime = mktime(&time_tm) + 60 * 60 * 24 * 7;		
			mode = "a"; 		
			break;	
		
		default: 
			return NULL;	
		}	
		return fopen(filename, mode);
}


int os_sem_wait(sem_t* handle)
{
	 while(1) 
	 {
		if(sem_wait(handle)==0) return 0;
		if(errno!=EINTR) return errno;					     
	 }
}
