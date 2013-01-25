#ifndef __THUMB_H
#define __THUMB_H

#include "save_file.h"

#define JPG	1
#define PNG	2
#define GIF	3


int scan_all_work_file(void* owner, void* data);
int post_llocc_callback(void* pkg, int pkglen);
int post_llocc_webproxy_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen, int err);


int limit_picture(work_data_t* lwd, char** limit_data, int* limit_len, int* limit_flag);


#endif

