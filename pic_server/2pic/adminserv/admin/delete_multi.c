#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <iter_serv/net_if.h>
#include <iter_serv/net.h>

#include "util.h"
#include "proto.h"
#include "lloccode.h"
#include "filelog.h"
#include "fs_list.h"
#include "admin.h"
#include "file_sets.h"

#include "delete_multi.h"

static del_file_set_t* pgdf = NULL;
static const char* del_file_mmap_path = DEL_FILE_LOG_DIR"del_file_set";

int init_del_log_mmap()
{
	int map_size = sizeof(del_file_set_t);
	int fd = open(del_file_mmap_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		ERROR_RETURN(("fail to open file %s", del_file_mmap_path), -1);
	}
	ftruncate(fd, map_size);

	char* pmap = (char *)mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(pmap == (char *)-1) {
         close (fd);
         ERROR_RETURN(("fail to mmap %s", del_file_mmap_path), -1);
    }
	pgdf = (del_file_set_t*)pmap;
    close (fd);
	return 0;
}

void release_del_log_mmap()
{	
	munmap(pgdf, sizeof(del_file_set_t));
}

static void reset_del_log_mmap()
{
	const struct tm  	*cur_tm = get_now_tm();
	struct tm	*mp_tm = &pgdf->set_tm;

	DEBUG_LOG("RESET DEL MMAP");
	if (cur_tm->tm_year <= mp_tm->tm_year
		&& cur_tm->tm_mon <= mp_tm->tm_mon
		&& cur_tm->tm_mday <= mp_tm->tm_mday
		&& cur_tm->tm_hour <= mp_tm->tm_hour
		&& cur_tm->tm_min <= mp_tm->tm_min) {
		pgdf->file_nbr++;
	} else {
		memcpy(mp_tm, cur_tm, sizeof(*mp_tm));
		pgdf->file_nbr = 0;
	}
	pgdf->file_cnt = 0;
	pgdf->ret_cnt = 0;
	pgdf->lloc_err_cnt = 0;
}

static int need_create_new_file(del_file_t* adf)
{
	if (pgdf->file_cnt >= MAX_FILE_NUM_IN_SET) {
		return 1;
	}

	const struct tm  	*cur_tm = get_now_tm();
	struct tm	*mp_tm = &pgdf->set_tm;
	if (cur_tm->tm_year <= mp_tm->tm_year
		&& cur_tm->tm_mon <= mp_tm->tm_mon
		&& cur_tm->tm_mday <= mp_tm->tm_mday
		&& cur_tm->tm_hour <= mp_tm->tm_hour
		&& cur_tm->tm_min <= mp_tm->tm_min) {
		return 0;
	}

	return 1;
}

static int write_a_new_file()
{
	char path[MAX_PATH_LEN] = {0};
	if (pgdf->file_cnt == 0) return 0;
	
	struct tm	*mp_tm = &pgdf->set_tm;
	snprintf(path, MAX_PATH_LEN, "%s%s%4u%02u%02u%02u%02u%02u", DEL_FILE_LOG_DIR, df_prefix,
		mp_tm->tm_year + 1900, mp_tm->tm_mon + 1, mp_tm->tm_mday, mp_tm->tm_hour, 
		mp_tm->tm_min, pgdf->file_nbr);

	DEBUG_LOG("WRITE FILE\t[%s]", path);
	int fd = open_log_file(path);
	if (fd == -1) 
		ERROR_RETURN(("open file %s err", path), -1);

	size_t size = ((unsigned long)&pgdf->dfi[pgdf->file_cnt] - (unsigned long)pgdf);
	if (safe_write(fd, pgdf, size) == -1) {
		ERROR_RETURN(("write file %s err", path), -1);
	}
	return 0;
}

static int add_del_log(del_file_t* adf)
{
	if (need_create_new_file(adf)) {
		if (write_a_new_file() == -1) 
			return -1;
		reset_del_log_mmap();
	}

	memcpy(&pgdf->dfi[pgdf->file_cnt], adf, sizeof(del_file_t));
	pgdf->file_cnt++;
	return 0;
}

int delete_multi_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	
	CHECK_BODY_LEN_GE(pkglen, 4);
	int file_cnt;
	int j = 0;
	UNPKG_H_UINT32(pkg, file_cnt, j);
	DEBUG_LOG("DEL MUT\t[uid=%u file_cnt=%u fd=%d]", userid, file_cnt, fd);
	if (file_cnt > MAX_DEL_FILE_NUM_ONCE) {
		send_err_to_php(fd, cmd, userid, err_del_too_many_once);
		ERROR_RETURN(("del too many files one time\t[uid=%u]", userid), -1);
	}
	if (pkglen != 4 + file_cnt * (LLOCCODE_LEN + 4)) {
		send_err_to_php(fd, cmd, userid, err_invalid_para);
		ERROR_RETURN(("invalid para pkg\t[uid=%u]", userid), -1);
	}

	del_file_t   ldf;
	del_file_t*  pld = &ldf;
	memset(pld, 0, sizeof(del_file_t));
	int lp;
	for (lp = 0; lp < file_cnt; lp++) {
		
		UNPKG_H_UINT32(pkg, pld->hostid, j);
		UNPKG_STR(pkg, pld->lloccode, j, LLOCCODE_LEN);
		DEBUG_LOG("DEL LLOCC\t[%u %s]", userid, pld->lloccode);
		pld->userid = userid;
		if (analyse_lloccode_ex(pld->lloccode, pld->file_path, NULL, 
			&pld->f_type, &pld->b_type) == -1) {
			ERROR_LOG("lloccode err\t[%u %u %s]", userid, pld->lloccode);
			pld->del_ret = del_llocc_err;
		}
		if (add_del_log(pld) == -1) {
			send_err_to_php(fd, cmd, userid, err_adminserv_busy);
			ERROR_RETURN(("no disk to write\t[uid=%u]", userid), -1);
		}
	}
	
	send_ok_to_php(fd, cmd, userid);
	//do_del_conn(fd);
	return -1;
}
