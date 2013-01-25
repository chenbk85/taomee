#ifndef __CHANGE_FILE_ATTR_H
#define __CHANGE_FILE_ATTR_H

#include "lloccode.h"

typedef struct _change_file_info {
	char		lloccode[LLOCCODE_LEN + 1];
	int			hostid;
	int 		f_type;
	int 		b_type;
	int			path_len;
	char        file_path[MAX_PATH_LEN];
	uint32_t 	thumb_cnt;
	uint32_t	thumb_id[MAX_THUMB_CNT_PER_PIC];
}__attribute__((packed))change_file_info_t;


typedef struct _change_file_attr {
	int 		fd;
	uint32_t	userid;
	uint16_t 	cmd;
	uint32_t	action;
	char        lloccode[LLOCCODE_LEN + 1];
	char		url[GET_PIC_URL_LEN + 1];
	change_file_info_t file_info;
}__attribute__((packed))change_file_attr_t;

void init_change_file_attr_hash();
void fini_change_file_attr_hash();
int get_change_file_attr_hash_size();

int change_file_attr(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
int change_file_attr_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
#endif

