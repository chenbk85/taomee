#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include <libtaomee/list.h>
#include <libtaomee/log.h>

#include <iter_serv/net_if.h>
#include <iter_serv/net.h>

#include "util.h"
#include "proto.h"
#include "lloccode.h"
#include "filelog.h"
#include "fs_list.h"
#include "file_sets.h"
#include "error_nbr.h"

#include "delete_file.h"

tmr_t g_timer;

static del_file_set_t	gdfs;
static char last_fp[MAX_PATH_LEN];
static int last_fd  = -1;

static void do_del_files()
{
	char buff[512] = { 0 };
	int j = sizeof(protocol_t);
	int j_label = j;
	
	int loop;
	for (loop = 0; loop < gdfs.file_cnt; loop++) {
		del_file_t* lpd = &gdfs.dfi[loop];
		if (!lpd->del_ret) {
			j = j_label;
			PKG_H_UINT32(buff, loop, j);
			PKG_STR(buff, lpd->file_path, j, MAX_PATH_LEN);
			DEBUG_LOG("DEL FILE\t[%u %s]", loop, lpd->file_path);
			init_proto_head(buff, lpd->userid, proto_fs_delete_file, j);
			send_to_fileserv(buff, j, lpd->hostid);
		}
	}

}

static void proc_a_log_file(const char* file_name)
{
	char* file_path = last_fp;
	snprintf(file_path, MAX_PATH_LEN, "%s%s", DEL_FILE_LOG_DIR, file_name);
	int size = get_file_len(file_path);
	last_fd = open_log_file(file_path);
	if (last_fd == -1) {
		ERROR_LOG("cannot open file %s", file_path);
		return;
	}
	if (safe_read(last_fd, &gdfs, size) == -1) {
		ERROR_LOG("cannot read file %s", file_path);
		return;
	}
	do_del_files();
}

static void chk_proc_result()
{
	del_file_set_t* ldf = &gdfs;
	if (last_fd != -1 && ldf->file_cnt) {
		DEBUG_LOG("STAT INFO\t[%s %u %u]", last_fp, ldf->lloc_err_cnt, ldf->ret_cnt);
		if (ldf->file_cnt <= ldf->lloc_err_cnt + ldf->ret_cnt) {
			if (unlink(last_fp)) {
				ERROR_LOG("remove file %s err: %s", last_fp, strerror(errno));
			}
		} else {
			size_t size = ((unsigned long)&ldf->dfi[ldf->file_cnt] - (unsigned long)ldf);
			if (safe_write(last_fd, ldf, size) == -1) {
				ERROR_LOG("write file %s err", last_fp);
			}
			close(last_fd);
			last_fd = -1;
		}
		memset(ldf, 0, sizeof(del_file_set_t));
	}

}

int scan_del_file(void* owner, void* data)
{
	chk_proc_result();
	
    struct dirent* lsd;
	DIR* lD = opendir(DEL_FILE_LOG_DIR);
	if (lD == NULL) {
		ERROR_RETURN(("cannot open dir %s", DEL_FILE_LOG_DIR), -1);
	}
    while ((lsd = readdir(lD)) != NULL) {
    	if (!strncmp(lsd->d_name, df_prefix, 2)) {
			proc_a_log_file(lsd->d_name);
			// only process one log file every scan
			break;
		}
    }
	closedir(lD);

	ADD_TIMER_EVENT(&g_timer, scan_del_file, NULL, get_now_tv()->tv_sec + 2);	
	return 0;
}

int delete_file_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN(pkglen, 8 + MAX_PATH_LEN);
	int j = 0, loop, ret;
	char fp[MAX_PATH_LEN] = {0};
	
	UNPKG_H_UINT32(pkg, loop, j);
	UNPKG_H_UINT32(pkg, ret, j);
	UNPKG_STR(pkg, fp, j, MAX_PATH_LEN);

	del_file_t* lpd = &gdfs.dfi[loop];
	if (strcmp(lpd->file_path, fp))
		ERROR_RETURN(("overdue message %s", fp), 0);

	lpd->del_ret = ret;
	gdfs.ret_cnt++;
	if (ret && ret != del_file_succ) {
		ERROR_LOG("del file %s error\t[%u %s]", lpd->lloccode, ret, lpd->file_path);
	}
	return 0;
}

#if 0
int delete_multi_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	
	CHECK_BODY_LEN_GE(pkglen, 4);
	int file_cnt;
	int j = 0;
	UNPKG_H_UINT32(pkg, file_cnt, j);
	DEBUG_LOG("DEL MUT\t[uid=%u file_cnt=%u]", userid, file_cnt);
	if (file_cnt > MAX_DEL_FILE_NUM_ONCE) {
		send_err_to_php(fd, cmd, userid, err_del_too_many_once);
		ERROR_RETURN(("del too many files one time\t[uid=%u]", userid), -1);
	}
	if (pkglen != 4 + file_cnt * (LLOCCODE_LEN + 4)) {
		send_err_to_php(fd, cmd, userid, err_invalid_para);
		ERROR_RETURN(("invalid para pkg\t[uid=%u]", userid), -1);
	}
	
	del_multi_t* lpdm = (del_multi_t*)g_slice_alloc0(sizeof(del_multi_t));
	if (!lpdm) {
		send_err_to_php(fd, cmd, userid, err_adminserv_busy);
		ERROR_RETURN(("no memory--del file\t[%u %u %u]", userid, cmd, fd), -1);
	}
	lpdm->u_key = ++g_u_key;
	lpdm->fd = fd;
	lpdm->userid = userid;
	lpdm->cmd = cmd;
	lpdm->file_cnt = file_cnt;
	int lp;
	for (lp = 0; lp < lpdm->file_cnt; lp++) {
		del_file_info_t* lpdf = &lpdm->dfi[lp];
		UNPKG_H_UINT32(pkg, lpdf->hostid, j);
		UNPKG_STR(pkg, lpdf->lloccode, j, LLOCCODE_LEN);
		if (analyse_lloccode_ex(lpdf->lloccode, lpdf->file_path, &lpdf->path_len, 
			&lpdf->f_type, &lpdf->b_type) == -1) {
			ERROR_LOG("lloccode err\t[%u %u %s]", lpdm->u_key, userid, lpdf->lloccode);
			lpdf->ret = del_llocc_err;
			lpdm->ret_cnt++;
			write_del_fail_log(lpdm->u_key, userid, lpdf);
		}
	}
	
	int loop;
	for (loop = 0; loop < lpdm->file_cnt; loop++) {
		del_file_info_t* ldfi = &lpdm->dfi[loop];
		if (ldfi->ret)
			continue;
		char buff[256];
		int k = sizeof(protocol_t);
		PKG_H_UINT32(buff, lpdm->u_key, k);
		PKG_H_UINT32(buff, loop, k);
		PKG_STR(buff, ldfi->file_path, k, MAX_PATH_LEN);	
		init_proto_head(buff, userid, proto_fs_delete_file, k);
		send_to_fileserv(buff, k, ldfi->hostid);
		DEBUG_LOG("DEL FILE[%u %u %s]", lpdm->userid, ldfi->hostid, ldfi->file_path);
	}

	send_ok_to_php(lpdm->fd, lpdm->cmd, userid);
	do_del_conn(lpdm->fd);
	
	g_hash_table_replace(del_multi_maps, &(lpdm->fd), lpdm);
	return 0;
}

int delete_file_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN(pkglen, 12);
	uint32_t u_key;
	int j = 0, loop, ret;
	UNPKG_H_UINT32(pkg, u_key, j);
	UNPKG_H_UINT32(pkg, loop, j);
	UNPKG_H_UINT32(pkg, ret, j);
	del_multi_t* lpdm = g_hash_table_lookup(del_multi_maps, &u_key);
	if (!lpdm) {
		ERROR_RETURN(("not find key\t[%u %u %u]", userid, cmd, u_key), -1);
	}
	DEBUG_LOG("DEL FILE CALLBACK\t[%u %u]", userid, u_key, loop);
	del_file_info_t* ldfi = &lpdm->dfi[loop];
	ldfi->ret = ret;
	(ret) ? write_del_fail_log(u_key, userid, ldfi) : write_del_succ_log(u_key, userid, ldfi);
	lpdm->ret_cnt++;
	if (lpdm->ret_cnt == lpdm->file_cnt)
		g_hash_table_remove(del_multi_maps, &lpdm->fd);
	return 0;
}
#endif
