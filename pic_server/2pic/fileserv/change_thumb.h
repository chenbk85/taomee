#ifndef _CHANGE_THUMB_H
#define _CHANGE_THUMB_H


#include <libgen.h>
#include <glib.h>
#include <libpt/internal.h>
#include <libpt/qlist.h>
#include <gd.h>
#include "proto.h"
#include "lloccode.h"

#include "common.h"

typedef struct _chg_thumb_data{
	list_head_t		list;

	int				sockfd;
	uint32_t	    userid;
	uint16_t	    waitcmd;
	uint32_t		chg_time;
	uint32_t 		f_type;    	// 1, for picture; 2 for doc
	uint32_t		b_type;  	// when file_type is 1, 1 for jpg, 2 for png, 3 for gif
	uint32_t		dir_idx;        // directory ID
	uint32_t		pic_idx;        // picture ID
	char			path[MAX_PATH_LEN];
	char			lloccode[LLOCCODE_LEN + 1];
	uint32_t		photoid;
	char 			log_path[MAX_PATH_LEN];
	int 			log_fd;

	int 			chg_thumb_tag;   // 1, mean chg succ
	
	char			file_path[MAX_PATH_LEN];    // source file data
	char 			file_name[MAX_NAME_LEN];
	thumb_para_t	new_thumb;
	int				file_len;
	char			file_data[];
}__attribute__((packed))chg_thumb_data_t;

void scan_chg_thumbs();
int chg_thumb_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);

#endif