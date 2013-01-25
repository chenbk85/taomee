#ifndef __FILE_SETS_H
#define __FILE_SETS_H

#include <lloccode.h>



#define MAX_FILE_NUM_IN_SET				100000
#define DEL_FILE_LOG_DIR			"../del_log/"
#define df_prefix					"df"

typedef struct _del_file {
	uint32_t		userid;
	uint32_t	hostid;
	int 		f_type;
	int 		b_type;
	int			del_ret;
	char		lloccode[LLOCCODE_LEN + 1];
	char        file_path[MAX_PATH_LEN];
}__attribute__((packed))del_file_t;

typedef struct _del_file_set {
	struct tm		set_tm;
	int				file_nbr;
	uint32_t		file_cnt;
	uint32_t		ret_cnt;
	uint32_t		lloc_err_cnt;
	del_file_t 		dfi[MAX_FILE_NUM_IN_SET];
}__attribute__((packed))del_file_set_t;

#endif
