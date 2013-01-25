#ifndef __DEL_MULTI_H
#define __DEL_MULTI_H

#include <libtaomee/timer.h>

#include "lloccode.h"

#define MAX_DEL_FILE_NUM_ONCE		16
#define DEL_FILE_SUCC_LOG_PATH			"./del_log/del_file_succ.log"
#define DEL_FILE_FAIL_LOG_PATH			"./del_log/del_file_fail.log"

typedef struct _tmr{
	list_head_t timer_list;
} tmr_t;

extern tmr_t g_timer;

int delete_file_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
int scan_del_file(void* owner, void* data);




#endif




