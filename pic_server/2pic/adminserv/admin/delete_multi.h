#ifndef __DEL_MULTI_H
#define __DEL_MULTI_H

#include "lloccode.h"


#define MAX_DEL_FILE_NUM_ONCE		16

int init_del_log_mmap();
void release_del_log_mmap();

int delete_multi_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);



#endif




