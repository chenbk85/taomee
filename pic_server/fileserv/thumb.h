#ifndef __THUMB_H
#define __THUMB_H

#include "save_file.h"

#define JPG	1
#define PNG	2
#define GIF	3

#define send_error_to_admin(userid, cmd, fd, err) \
	send_error_to_cgi(userid, cmd, fd, err)

void create_thumbs (gpointer data, gpointer user_data);
int scan_all_work_file(void* owner, void* data);

extern int g_thumbserv_fd;

int upload_thumb_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
int send_pic_to_thumbserv(work_data_t* awd);
int limit_picture(work_data_t* lwd, char** limit_data, int* limit_len, int* limit_flag);
void send_error_to_cgi(uint32_t userid, uint16_t cmd, int fd, int err);
//void create_lloccode(work_data_t* awd);


#endif

