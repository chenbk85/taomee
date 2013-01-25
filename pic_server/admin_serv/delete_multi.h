#ifndef __DEL_MULTI_H
#define __DEL_MULTI_H

#include "lloccode.h"

#define DEL_ALBUM_FILE	"del_album_id"

typedef struct _del_file_info {
	char		lloccode[LLOCCODE_LEN + 1];
	int			hostid;
	int 		f_type;
	int 		b_type;
	int			path_len;
	char        file_path[MAX_PATH_LEN];
	uint32_t 	thumb_cnt;
	uint32_t	thumb_id[MAX_THUMB_CNT_PER_PIC];
}__attribute__((packed))del_file_info_t;

typedef struct _del_multi {
	int 			fd;
	uint32_t		userid;
	uint16_t 		cmd;
	uint32_t		key_albumid;
	uint32_t		del_album_flag;
	int				file_cnt;
	del_file_info_t dfi[MAX_PIC_NUM_PER_ABLUM];
}__attribute__((packed))del_multi_t;

void init_del_multi_hash();
void fini_del_multi_hash();
int get_del_multi_hash_size();
int delete_multi_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
int delete_multi_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
int delete_album_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);


extern int g_fd;

#endif




