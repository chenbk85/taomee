
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "filelog.h"


int open_log_file(work_data_t* aw) 
{
	aw->log_fd = open(aw->log_path, O_RDWR|O_CREAT, DEF_FILE_MODE);
	if (aw->log_fd == -1) {
		ERROR_RETURN(("open file error\t[err=%d path:%s]", errno, aw->log_path), -1);
	}
}

int write_log_file(work_data_t* aw, char* str_log)
{
	if (aw->log_fd == -1) return -1;

	write(aw->log_fd, str_log, strlen(str_log));
	return 0;
}

