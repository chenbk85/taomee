#ifndef __CHG_MULTI_H
#define __CHG_MULTI_H

#include "lloccode.h"

typedef struct _chg_thumb {
	int 			fd;
	uint32_t		userid;
	uint16_t 		cmd;
	uint32_t		ret;
	uint32_t		hostid;
}__attribute__((packed))chg_thumb_t;

int chg_thumb_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
int chg_thumb_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen, int ret);


void init_chg_thumb_hash();
void fini_chg_thumb_hash();
int get_chg_thumb_hash_size();


#endif





