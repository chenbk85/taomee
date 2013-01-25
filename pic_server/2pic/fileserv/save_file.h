#ifndef __SAVE_FILE_H
#define __SAVE_FILE_H

#include <libgen.h>
#include <glib.h>
#include <libpt/internal.h>
#include <libpt/qlist.h>
#include <gd.h>
#include "proto.h"
#include "lloccode.h"

#include "common.h"

#define MAX_FILE_TYPE			10
#define MAX_BRANCH_TYPE			10
#define FILE_WORK_BUFFER_LEN	3 * 1024 * 1024
#define LOGO_BUFFER_LEN			3 * 1024 * 1024
#define MAX_FILE_LEN			2 * 1024 * 1024

enum _file_type {
	picture_file_type  		= 1,
	doc_file_type			= 2
};

enum _save_big_pic_ret {
	save_big_pic_success  	= 1,
	save_big_pic_fail		= 2
};

typedef struct _file_index {
	struct tm	today_date;
	int			curr_dir_id;
	int			curr_pic_id;
}__attribute__((packed))file_index_t;

typedef struct _p_file_index {
	Q_MUTEX_T		qmutex;
	char*			mmap_file;
	file_index_t* 	cfi[MAX_FILE_TYPE];
}__attribute__((packed))p_file_index_t;

typedef struct _thumb_para {
	// receive para from client
	int32_t		clip_w;
	int32_t		clip_h;
	int32_t		start_x;
	int32_t		start_y;
	int32_t		thumb_w;
	int32_t		thumb_h;

	uint32_t	dir_idx;        // directory ID
	uint32_t	pic_idx;        // picture ID
	char		path[MAX_PATH_LEN];
	char		lloccode[LLOCCODE_LEN + 1];

	int			thumb_len;
	char* 		thumb_data;
}__attribute__((packed))thumb_para_t;

typedef struct _work_data {
	list_head_t		list;

	int				sockfd;
	uint32_t	    userid;
	uint16_t		waitcmd;
	uint32_t		upload_time;
	uint32_t		channel;
	uint32_t		b_key;
	uint32_t 		file_type;    	// 1, for picture; 2 for doc
	uint32_t		branch_type;  	// when file_type is 1, 1 for jpg, 2 for png, 3 for gif
	int32_t			w_limit;  		// limit width
	int32_t			h_limit;  		// limit high
	uint32_t		dir_idx;        // directory ID
	uint32_t		pic_idx;        // picture ID
	char			path[MAX_PATH_LEN];
	char			lloccode[LLOCCODE_LEN + 1];
	uint32_t		photoid;
	char 			log_path[MAX_PATH_LEN];
	int 			log_fd;

	int 			save_file_tag;   // 1, mean already saved source file
	int 			sent_llocc;
	int 			posted_llocc;
	
	char			file_path[MAX_PATH_LEN];    // source file data
	char 			file_name[MAX_NAME_LEN];
	int 			thumb_cnt;
	thumb_para_t	thumb_paras[MAX_THUMB_CNT_PER_PIC];
	int				file_len;
	char			file_data[];
}__attribute__((packed))work_data_t;

typedef struct _tmr{
	list_head_t timer_list;
} tmr_t;

int init_dir_index(const char* filepath);
void release_dir_index();
void send_result_to_cgi(work_data_t* awd);
void save_file (gpointer data, gpointer user_data);
void save_logo (gpointer data, gpointer user_data);
int upload_file_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
int upload_logo_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);

int delete_file_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);

static inline int send_error_to_cgi(uint32_t userid, uint16_t cmd, int fd, int err)
{
	static char buf[256] = {0};
	int j = sizeof(protocol_t);
	protocol_t* pp = (protocol_t*)buf;
	pp->cmd = cmd;
	pp->id = userid;
	pp->len = j;
	pp->ret = err;
	
	DEBUG_LOG("SEND ERR\t[%u %u %u %u %u]", userid, err, cmd, fd, j);
	return net_send(fd, buf, j);
}

#define send_error_to_admin(userid, cmd, fd, err) \
	send_error_to_cgi(userid, cmd, fd, err)

#define send_ok_to_admin(userid, cmd, fd) \
	send_error_to_cgi(userid, cmd, fd, 0)

void init_upload_hash();
void fini_upload_hash();
int get_upload_hash_size();
void upload_hash_remove_key(void* pkey);
work_data_t* get_user_from_upload_maps(uint32_t* userid);


extern GThreadPool *pool1, *pool2, *pool3;
extern qlist_t  gqlist;
extern qlist_t  g_chg_list;

extern int gcnt;
extern tmr_t  g_timer;
extern int save_file_cnt, save_logo_cnt, chg_thumb_cnt;

static inline int file_exist(const char *filepath) 
{
	if(access(filepath, F_OK) == 0) return 1;
	return 0;
}

static inline char* file_get_dir(const char *filepath) 
{
	char *path = strdup(filepath);
	char *dname = dirname(path);
	DEBUG_LOG("DIR NAME\t[%s]", dname);
	char *dir = strdup(dname);
	free(path);
	return dir;
}

static inline int file_make_dir(const char *dirpath, mode_t mode, int recursive) 
{
	DEBUG_LOG("try to create directory %s", dirpath);
	if(mkdir(dirpath, mode) == 0) return 1;

	DEBUG_LOG("errno=%u", errno);
	if(recursive == 1 && errno == ENOENT) {
		char *parentpath = file_get_dir(dirpath);
		if(file_make_dir(parentpath, mode, recursive) == 1
		&& file_make_dir(dirpath, mode, recursive) == 1) {
			free(parentpath);
			return 1;
		}
		free(parentpath);
	}
	return 0;
}

#endif
