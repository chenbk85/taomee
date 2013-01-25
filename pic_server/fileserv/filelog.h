#ifndef __FILELOG_H
#define __FILELOG_H

#include "save_file.h"

#define MAX_LOG_LEN		512
#define UPLOAD_LOG_ROOT  "./upload_log/"

int open_log_file(work_data_t* aw);
int write_log_file(work_data_t* aw, char* str_log);

#endif

