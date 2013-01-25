#ifndef _FILE_LOG_H
#define _FILE_LOG_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib.h>
#include <libtaomee/log.h>
#include <libtaomee/crypt/qdes.h>

#define	DEF_DIR_MODE			(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
#define	DEF_FILE_MODE			(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

#define MAX_LOG_LEN		512
#define UPLOAD_LOG_ROOT  	"./upload_log/"

static inline int open_log_file(const char* log_path) 
{
	int log_fd = open(log_path, O_RDWR|O_CREAT, DEF_FILE_MODE);
	if (log_fd == -1) {
		return -1;
	}
	return log_fd;
}

static inline int write_log_file(int log_fd, char* str_log)
{
	if (log_fd == -1) return -1;

	write(log_fd, str_log, strlen(str_log));
	return 0;
}

static inline int safe_read(int fd, void* buf, int count) 
{
	int read_sum = 0, n = 0;
	for ( ; ; ) {
		n = read(fd, buf + read_sum, count);
		if (n == -1){
			return -1;
		}
		read_sum += n;
		DEBUG_LOG("safe_read: %d %d %d", n, count, read_sum);	
		if (n == count) {
			return read_sum;
		}

		count =- read_sum;
	}
}

static inline int safe_write(int fd, const void* buf, size_t count) 
{
	int n = 0, written = 0;
	for ( ;; ) {
        	n = write(fd, buf + written, count);
        	if (n == -1) {
            		return -1;
        	}

        	written += n;
        	if ((size_t) n == count) {
            		return written;
        	}

        	count -= n;
    	}
}


#endif
