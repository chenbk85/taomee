#ifndef __SAVE_FILE_H
#define __SAVE_FILE_H

#include <libgen.h>
#include <glib.h>
#include <libpt/internal.h>
#include <libpt/qlist.h>
#include <gd.h>
#include "lloccode.h"

#include "common.h"

#define MAX_FILE_TYPE			10
#define MAX_BRANCH_TYPE			10

#define MAX_IMP_DAYS			365

#define FILE_WORK_BUFFER_LEN	3 * 1024 * 1024
#define MAX_FILE_LEN			2 * 1024 * 1024
#define MAX_DIR_ID				999
#define MAX_PIC_ID				999

#define	DEF_DIR_MODE			(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
#define	DEF_FILE_MODE			(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

enum _file_type {
	picture_file_type  		= 1,
	doc_file_type			= 2
};

enum _save_big_pic_ret {
	save_big_pic_success  	= 1,
	save_big_pic_fail		= 2
};

enum _create_thumb_ret {
	create_thumb_success  	= 1,
	create_thumb_fail		= 2
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

typedef struct _p_imp_file_index {
	Q_MUTEX_T		qmutex;
	char*			mmap_file;
	file_index_t* 	cfi[MAX_IMP_DAYS];
}__attribute__((packed))p_imp_file_index_t;

typedef struct _thumb_para {
	// receive para from client
	int32_t		clip_w;
	int32_t		clip_h;
	int32_t		start_x;
	int32_t		start_y;
	int32_t		thumb_w;
	int32_t		thumb_h;

	//make thumb, later will be sent to thumb server
	uint32_t	thumb_id;
	int			thumb_len;
	char* 		thumb_data;
}__attribute__((packed))thumb_para_t;

typedef struct _del_thumb_para {
	int32_t		thumb_cnt;
	uint32_t	thumb_id[MAX_THUMB_CNT_PER_PIC];
}__attribute__((packed))del_thumb_para_t;

typedef struct _work_data {
	list_head_t		list;

	int 			unique_key;
	int				sockfd;
	uint32_t	    userid;
	uint16_t		waitcmd;
	uint32_t		upload_time;
	uint32_t		imp_upload_time;
	uint32_t		key;		  	// the unique key for album ID
	uint32_t 		file_type;    	// 1, for picture; 2 for doc
	uint32_t		branch_type;  	// when file_type is 1, 1 for jpg, 2 for png, 3 for gif
	int32_t			w_limit;  		// limit width
	int32_t			h_limit;  		// limit high
	uint32_t		thumb_cnt;   	// thumb coount
	thumb_para_t	thumb_paras[4]; // thumb para
	uint32_t		dir_idx;        // directory ID
	uint32_t		pic_idx;        // picture ID
	char			path[MAX_PATH_LEN];
	char			lloccode[LLOCCODE_LEN + 1];
	char 			log_path[MAX_PATH_LEN];
	int 			log_fd;

	char 			save_file_tag;   // 1, mean already saved source file
	char			make_thumb_tag;  // 1, mean already made thumb 
	char 			send_thumb_tag;  // 1, mean already sent thumb
	char 			done_thumb_tag;  // 1, mean thumb server saved thumbs
	
	uint32_t		file_len;       // source file length from client
	char			file_data[];    // source file data
}__attribute__((packed))work_data_t;

typedef struct _tmr{
	list_head_t timer_list;
} tmr_t;

int init_dir_index(const char* filepath);
void release_dir_index();
void release_imp_dir_index();
void send_result_to_cgi(work_data_t* awd);
void save_file (gpointer data, gpointer user_data);
int upload_file_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
int delete_file_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);

void init_upload_hash();
void fini_upload_hash();
int get_upload_hash_size();
void upload_hash_remove_key(void* pkey);

extern GThreadPool *pool1;
extern GThreadPool *pool2;
extern qlist_t  gqlist;
extern int gcnt;
extern tmr_t  g_timer;
extern int save_file_cnt;
extern int create_thumb_cnt;


#endif
