#ifndef __TRANS_MULTI_PIC_H
#define __TRANS_MULTI_PIC_H

#include "lloccode.h"

#define MAX__ONCE_TRANS_NUM		16

enum trans_multi_err{
		err_not_find_thumb		= 1,
		err_album_full			= 2,
};

typedef struct _thumb {
	uint32_t	thumb_id;
	int			new_thumb_id;
}__attribute__((packed))thumb_t;

typedef struct _file_info {
	char		lloccode[LLOCCODE_LEN + 1];
	int			ret;
	uint32_t 	thumb_cnt;
	thumb_t		sth[MAX_THUMB_CNT_PER_PIC];
}__attribute__((packed))file_info_t;

typedef struct _trans_multi {
	int 		fd;
	uint32_t	userid;
	uint16_t 	cmd;
	uint32_t	key_albumid;
	uint32_t	new_key_albumid;
	int			file_cnt;
	file_info_t fi[MAX__ONCE_TRANS_NUM];
}__attribute__((packed))trans_multi_t;

void init_trans_multi_hash();
void fini_trans_multi_hash();
int get_trans_multi_hash_size();
int trans_multi_file_get(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
int trans_multi_file_add(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
int trans_multi_file_del(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);



#endif

