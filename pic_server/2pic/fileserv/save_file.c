
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>

#include "util.h"
#include "error_nbr.h"
#include "proto.h"
#include "thumb.h"
#include "filelog.h"
#include "save_file.h"


GThreadPool *pool1, *pool2, *pool3;
qlist_t  gqlist, g_chg_list;
int gcnt = 0;

static p_file_index_t g_file_index;
static p_file_index_t* pfi = &g_file_index;

tmr_t  g_timer;
int save_file_cnt = 20;
int save_logo_cnt = 2;
int chg_thumb_cnt = 2;

#define map_page_size 	40960

static GHashTable* upload_maps;

void init_upload_hash()
{
	upload_maps = g_hash_table_new(g_int_hash, g_int_equal);
}

void fini_upload_hash()
{
	g_hash_table_destroy(upload_maps);
}

void upload_hash_remove_key(void* pkey)
{
	g_hash_table_remove(upload_maps, pkey);
}

int get_upload_hash_size()
{
	return g_hash_table_size(upload_maps);
}

work_data_t* get_user_from_upload_maps(uint32_t* userid)
{
	g_hash_table_lookup(upload_maps, userid);
}

int init_dir_index(const char* filepath)
{
	Q_MUTEX_INIT(pfi->qmutex, 1);
	int fd = open(filepath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		ERROR_RETURN(("fail to open file %s", filepath), -1);
	}
	long fsize = lseek(fd, 0, SEEK_END);
	char buff[map_page_size] = {0};
	if (fsize == 0) {
		write(fd, buff, map_page_size);
	}
	char* pmap = (char *)mmap(0, map_page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(pmap == (char *)-1) {
         close (fd);
         ERROR_RETURN(("fail to mmap %s", filepath), -1);
    }
	int lp;
	for (lp = 0; lp < MAX_FILE_TYPE; lp++) {
	    pfi->cfi[lp] = (file_index_t *)(pmap + lp * sizeof(file_index_t));
		DEBUG_LOG("date=%4u%2u%2u dirid=%03u picid=%03u", 
			pfi->cfi[lp]->today_date.tm_year + 1900, 
			pfi->cfi[lp]->today_date.tm_mon + 1, 
			pfi->cfi[lp]->today_date.tm_mday, 
			pfi->cfi[lp]->curr_dir_id, pfi->cfi[lp]->curr_pic_id);
	}
	pfi->mmap_file = pmap;
    close (fd);
	return 0;
}

void release_dir_index()
{	
	munmap(pfi->mmap_file, map_page_size);
}

void create_picpath_llocc_logpath(uint32_t userid, uint32_t type, int bran_type, uint32_t* dir_id, uint32_t* pic_id, char* filepath, char* lloccode, char* log_path)
{
	Q_MUTEX_ENTER(pfi->qmutex);

	struct tm td;
	time_t at_now = time(NULL);
	localtime_r(&at_now, &td);
	
	file_index_t* lfi = pfi->cfi[type - 1];
	if (lfi->today_date.tm_year == td.tm_year 
		&& lfi->today_date.tm_mon == td.tm_mon 
		&& lfi->today_date.tm_mday == td.tm_mday) {
		if (lfi->curr_pic_id >= MAX_PIC_ID) {
			lfi->curr_pic_id = 1;
			lfi->curr_dir_id++;
			if (lfi->curr_dir_id > MAX_DIR_ID){
				WARN_LOG("too much dir\t[%u]", lfi->curr_dir_id);
			}
		} else {
			lfi->curr_pic_id++;
		}
	} else {
		lfi->today_date = td;
		lfi->curr_dir_id = 1;
		lfi->curr_pic_id = 1;
	}
	
	*dir_id = lfi->curr_dir_id;
	*pic_id = lfi->curr_pic_id;
	Q_MUTEX_LEAVE(pfi->qmutex);
	char* suffix = "def";
	if (type == picture_file_type) {
		suffix = (bran_type == GIF) ? "GIF" : ((bran_type == PNG ? "PNG" : "JPEG"));
	}
	uint32_t filename = 0;
	char ldirpicsess[DIR_PIC_SESS_LEN + 1] = {0};
	create_dirpicsess_picid(ldirpicsess, *dir_id, pic_id);
	//int dir, pic;
	//get_dir_pic_from_sess(ldirpicsess, &dir, &pic);
	DEBUG_LOG("%s/%u/%4u%02u%02u/%03u/%u.%s", file_root, type, 
		lfi->today_date.tm_year + 1900,	lfi->today_date.tm_mon + 1,
		lfi->today_date.tm_mday, *dir_id, *pic_id, suffix);
	snprintf(filepath, MAX_PATH_LEN, "%s/%u/%4u%02u%02u/%03u/%u.%s", 
		file_root, type, lfi->today_date.tm_year + 1900,
		lfi->today_date.tm_mon + 1, lfi->today_date.tm_mday, 
		*dir_id, *pic_id, suffix);
	snprintf(lloccode, LLOCCODE_LEN, "%4u%02u%02u/%s.%s", 
		lfi->today_date.tm_year + 1900,	lfi->today_date.tm_mon + 1,
		lfi->today_date.tm_mday, ldirpicsess, suffix);
	if (log_path) {
		snprintf(log_path, MAX_PATH_LEN, "%s/%u-%4u%02u%02u-%03u-%u.log", 
			UPLOAD_LOG_ROOT, type, lfi->today_date.tm_year + 1900,	
			lfi->today_date.tm_mon + 1, lfi->today_date.tm_mday, *dir_id, *pic_id);
	}
	
}

void save_file (gpointer data, gpointer user_data)
{
	work_data_t* lwd = (work_data_t*)data;
	
	char log_str[MAX_LOG_LEN] = {0};
	if (do_save_file(lwd) != -1) {
		lwd->save_file_tag = 1;	
		snprintf(log_str, MAX_LOG_LEN, "Save file to %s succ\n", lwd->path);
	} else {
		lwd->save_file_tag = err_cannot_save_file;
		snprintf(log_str, MAX_LOG_LEN, "Save file to %s fail\n", lwd->path);
	}
	
	write_log_file(lwd->log_fd, log_str);
}

int do_save_file (work_data_t* lwd)
{
	char* dir_name = file_get_dir(lwd->path);
	if (!file_exist(dir_name)) {
		file_make_dir(dir_name, DEF_DIR_MODE, 1);
	}
	free(dir_name);

#ifndef CLIENT_LIMIT
	int ret = 0;
	int fd = open(lwd->path, O_RDWR|O_CREAT, DEF_FILE_MODE);
    if(fd < 0) {
		ERROR_RETURN(("open file %s error:%s", lwd->path, strerror(errno)), -1);
    }
	int limit_len = lwd->file_len;
	char* limit_data = lwd->file_data;
	int flag = 0;
	if (lwd->file_type == picture_file_type) {
		if (limit_picture(lwd, &limit_data, &limit_len, &flag) == -1) {
			close(fd);
			return -1;
		}
	}
	if (write(fd, limit_data, limit_len) == limit_len) {
		DEBUG_LOG("write file %s success\t[uid=%u len=%u]", 
			lwd->path, lwd->userid, lwd->file_len);
	} else {
		ret = -1;
		ERROR_LOG("write file %s error\t[uid=%u]", lwd->path, lwd->userid);
	}
out:
	close(fd);
	if (flag) {
		gdFree(limit_data);
	}
	return ret;
#else
	if (rename(lwd->file_path, lwd->path) != -1) {
		DEBUG_LOG("rename file %s success\t[uid=%u file_path=%s]", 
					lwd->path, lwd->userid, lwd->file_path);
		return 0;
	} else {
		ERROR_RETURN(("rename file %s error\t[uid=%u file_path=%s err=%s]",
			lwd->path, lwd->userid, lwd->file_path, strerror(errno)), -1);
	}
#endif
}

int read_file_from_cgi(work_data_t* awd)
{
	if ((awd->file_len = get_file_len(awd->file_path)) == -1) {
		send_error_to_cgi(awd->userid, awd->waitcmd, awd->sockfd, err_src_file_not_exist);
		ERROR_RETURN(("file not exist\t[%u %s]", awd->userid, awd->file_path), -1);
	}
	int file_fd = -1;
	if ((file_fd = open(awd->file_path, O_RDONLY)) == -1) {
		send_error_to_cgi(awd->userid, awd->waitcmd, awd->sockfd, err_src_file_not_exist);
		ERROR_RETURN(("open file error\t[%u %s]", awd->userid, awd->file_path), -1);
	}
	int len = 0;
	if ((len = safe_read(file_fd, ((char*)awd) + sizeof(work_data_t), awd->file_len)) == -1) {
		ERROR_LOG("read file %s error", awd->file_path);
	}
	DEBUG_LOG("file=%s len=%u read=%u", awd->file_path, awd->file_len, len);
	return 0;
}

int upload_file_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{

	int need_len = 24 + MAX_PATH_LEN + MAX_NAME_LEN;
	if (pkglen != need_len) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_RETURN(("upload-file pkg not enough\t [fd=%u userid=%u cmd=%u len=%u]",
			fd, userid, cmd, pkglen), -1);
	}
	
	if (g_hash_table_lookup(upload_maps, &userid)) {
		send_error_to_cgi(userid, cmd, fd, err_upload_busy_now);
		ERROR_RETURN(("upload-file upload too quickly\t [fd=%u userid=%u cmd=%u]",
			fd, userid, cmd), -1);
	}

	work_data_t* lwd = (work_data_t*)malloc(FILE_WORK_BUFFER_LEN);
	if (lwd == NULL) {
		ERROR_RETURN(("upload-file no memory"), -1);
	}
	memset(lwd, 0, FILE_WORK_BUFFER_LEN);
	lwd->userid 	= userid;
	lwd->waitcmd 	= cmd;
	lwd->sockfd 	= fd;
	lwd->upload_time = get_now_tv()->tv_sec;
	
	int j = 0;
	UNPKG_H_UINT32(pkg, lwd->channel, j);
	UNPKG_H_UINT32(pkg, lwd->b_key, j);
	UNPKG_H_UINT32(pkg, lwd->file_type, j);
	UNPKG_H_UINT32(pkg, lwd->branch_type, j);
	UNPKG_H_UINT32(pkg, lwd->w_limit, j);
	UNPKG_H_UINT32(pkg, lwd->h_limit, j);
	UNPKG_STR(pkg, lwd->file_name, j, MAX_NAME_LEN);
	DEBUG_LOG("FiLE\t [uid=%u b_key=%u f_type=%u b_type=%u w_limit=%u h_limit=%u]",
		userid, lwd->b_key, lwd->file_type, lwd->branch_type,
		lwd->w_limit, lwd->h_limit);
	if (lwd->file_type >= MAX_FILE_TYPE || lwd->branch_type > GIF 
		|| lwd->file_type == 0 || lwd->branch_type == 0
		|| lwd->w_limit <= 0 || lwd->h_limit <= 0) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("invalid file para\t [uid=%u f_type=%u b_type=%u w_limit=%u h_limit=%u]",
		userid, lwd->file_type, lwd->branch_type, lwd->w_limit, lwd->h_limit);
		goto err;
	}

	UNPKG_STR(pkg, lwd->file_path, j, MAX_PATH_LEN);
	if (read_file_from_cgi(lwd) == -1)
		goto err;
	create_picpath_llocc_logpath(lwd->userid, lwd->file_type, lwd->branch_type, &lwd->dir_idx,
		&lwd->pic_idx, lwd->path, lwd->lloccode, lwd->log_path);
	if ((lwd->log_fd = open_log_file(lwd->log_path)) == -1) {
		goto err;
	}
	char log_str[MAX_LOG_LEN] = {0};
	snprintf(log_str, MAX_LOG_LEN, "File %s received\n", lwd->lloccode);
	write_log_file(lwd->log_fd, log_str);
	g_hash_table_insert(upload_maps, &lwd->userid, lwd);
	DEBUG_LOG("UPLAOD-HASH-SIZE\t[%u]", get_upload_hash_size());
	qlist_add_tail (gqlist, &lwd->list, &gqlist.head);
	gcnt++;
	g_thread_pool_push (pool1, lwd, NULL);
	return 0;
err:
	free(lwd);
	return -1;
}

int upload_logo_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	if (pkglen < 24) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_RETURN(("upload-logo pkg not enough\t [fd=%u userid=%u cmd=%u]", 
			fd, userid, cmd), -1);
	}
	if (g_hash_table_lookup(upload_maps, &userid)) {
		send_error_to_cgi(userid, cmd, fd, err_upload_busy_now);
		ERROR_LOG("upload-logo too quickly\t [fd=%u userid=%u cmd=%u]", fd, userid, cmd);
		goto err;
	}

	work_data_t* lwd = (work_data_t*)malloc(LOGO_BUFFER_LEN);
	if (lwd == NULL) {
		ERROR_RETURN(("upload-file no memory"), -1);
	}
	memset(lwd, 0, LOGO_BUFFER_LEN);
	lwd->userid 	= userid;
	lwd->waitcmd 	= cmd;
	lwd->sockfd 	= fd;
	lwd->upload_time = get_now_tv()->tv_sec;

	int j = 0;
	UNPKG_H_UINT32(pkg, lwd->channel, j);
	UNPKG_H_UINT32(pkg, lwd->file_type, j);
	UNPKG_H_UINT32(pkg, lwd->branch_type, j);
	UNPKG_H_UINT32(pkg, lwd->w_limit, j);
	UNPKG_H_UINT32(pkg, lwd->h_limit, j);
	UNPKG_H_UINT32(pkg, lwd->thumb_cnt, j);
	
	DEBUG_LOG("FiLE\t [uid=%u f_typ=%u b_typ=%u w_limt=%u h_limt=%u th_cnt=%u]",
		userid, lwd->file_type, lwd->branch_type, lwd->w_limit, 
		lwd->h_limit, lwd->thumb_cnt);
	if (lwd->file_type != picture_file_type || lwd->branch_type > GIF 
		|| lwd->branch_type == 0 || lwd->thumb_cnt > MAX_THUMB_CNT_PER_PIC
		|| lwd->w_limit < 0 || lwd->h_limit < 0 ) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("invalid file para\t [uid=%u f_typ=%u b_typ=%u th_cnt=%u]", 
			userid, lwd->file_type, lwd->branch_type, lwd->thumb_cnt);
		goto err;
	}
	if (pkglen != 24 + lwd->thumb_cnt * 24 + MAX_PATH_LEN) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("pkg not enough\t [uid=%u type=%u b_type=%u thumb_cnt=%u]", 
			userid, lwd->file_type, lwd->branch_type, lwd->thumb_cnt);
		goto err;
	}
	int loop;
	for (loop = 0; loop < MAX_THUMB_CNT_PER_PIC && loop < lwd->thumb_cnt; loop++) {
		thumb_para_t* ltp = &lwd->thumb_paras[loop];
		UNPKG_H_UINT32(pkg, ltp->clip_w, j);
		UNPKG_H_UINT32(pkg, ltp->clip_h, j);
		UNPKG_H_UINT32(pkg, ltp->start_x, j);
		UNPKG_H_UINT32(pkg, ltp->start_y, j);
		UNPKG_H_UINT32(pkg, ltp->thumb_w, j);
		UNPKG_H_UINT32(pkg, ltp->thumb_h, j);
		if (ltp->clip_w <= 0 || ltp->clip_h <= 0) {
			ltp->clip_h = 0; ltp->clip_w = 0;
		}
		if (ltp->thumb_h <= 0 || ltp->thumb_w <= 0) {
			send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
			ERROR_LOG("invalid thubm para\t [uid=%u th_w=%d th_h=%d]", userid, 
				ltp->thumb_w, ltp->thumb_h);
			goto err;
		}
		DEBUG_LOG("THUMB PARA%u [%u %u %u %u %u %u]", loop, ltp->clip_w, ltp->clip_h, 
			ltp->start_x, ltp->start_y, ltp->thumb_w, ltp->thumb_h);
	}

	UNPKG_STR(pkg, lwd->file_path, j, MAX_PATH_LEN);
	if (read_file_from_cgi(lwd) == -1)
		goto err;
	create_picpath_llocc_logpath(lwd->userid, lwd->file_type, lwd->branch_type, &lwd->dir_idx,
		&lwd->pic_idx, lwd->path, lwd->lloccode, lwd->log_path);

	for (loop = 0; loop < MAX_THUMB_CNT_PER_PIC && loop < lwd->thumb_cnt; loop++) {
		thumb_para_t* ltp = &lwd->thumb_paras[loop];
		create_picpath_llocc_logpath(lwd->userid, lwd->file_type, lwd->branch_type, &ltp->dir_idx,
			&ltp->pic_idx, ltp->path, ltp->lloccode, NULL);
	}
	if ((lwd->log_fd = open_log_file(lwd->log_path)) == -1) {
		goto err;
	}
	char log_str[MAX_LOG_LEN] = {0};
	snprintf(log_str, MAX_LOG_LEN, "File %s received", lwd->lloccode);
	write_log_file(lwd->log_fd, log_str);
	g_hash_table_insert(upload_maps, &lwd->userid, lwd);
	DEBUG_LOG("UPLAOD-HASH-SIZE\t[%u]", get_upload_hash_size());
	qlist_add_tail (gqlist, &lwd->list, &gqlist.head);
	gcnt++;
	g_thread_pool_push (pool2, lwd, NULL);
	return 0;

err:
	free(lwd);
	return -1;
}

#if 0
int delete_file(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN_GE(pkglen, 76);
	char file_path[MAX_PATH_LEN] = {0};
	char part_file_path[64] = {0};
	del_thumb_para_t  ldt;
	uint32_t	key, hostid;
	int j = 0;
	UNPKG_H_UINT32(pkg, hostid, j);
	UNPKG_H_UINT32(pkg, key, j);
	UNPKG_STR(pkg, part_file_path, j, 64);
	UNPKG_H_UINT32(pkg, ldt.thumb_cnt, j);

	if (ldt.thumb_cnt > MAX_THUMB_COUNT) {
		ERROR_RETURN(("invalid thumb cnt\t[%u %u %u]", userid, key, ldt.thumb_cnt), 0);
	}
	CHECK_BODY_LEN_GE(pkglen, 76 + ldt.thumb_cnt * 4);
	int lp;
	for (lp = 0; lp < ldt.thumb_cnt; lp++) {
		UNPKG_H_UINT32(pkg, ldt.thumb_id[lp], j);
	}
	snprintf(file_path, MAX_PATH_LEN, "%s/%s", file_root, part_file_path);
	if (access(file_path, F_OK) == -1) {
		send_error_to_admin(userid, cmd, fd, err_src_file_not_exist);
		ERROR_RETURN(("file not exist\t[%u %u %s]", userid, key, file_path), 0);
	}
	DEBUG_LOG("DEL FILE\t [userid=%u fd=%u key=%u hostid=%u thumb_cnt=%u]",userid, fd, key, hostid, ldt.thumb_cnt);
	if (unlink(file_path) == -1) {
		ERROR_LOG("unlink file error[%u %u %s]: %s", userid, key, file_path, strerror(errno));
		return 0;
	}
	
	if (!ldt.thumb_cnt)
		goto out;
	
	char buff[128];
	int k = sizeof(protocol_t);
	PKG_H_UINT32(buff, key, k);
	int l = ldt.thumb_cnt * 4 + 4;
	memcpy(buff + k, &ldt, l);
	k += l;
	init_proto_head(buff, userid, proto_ts_delete_thumbs, k);
	if (send_to_thumbserv(buff, k) == -1) {
		ERROR_LOG("delete file thumbs error[%u %u %s]", userid, key, file_path);
		ERROR_LOG("thumbs para[%u %u %u %u %u %u]", userid, key, ldt.thumb_cnt, ldt.thumb_id[0],
			ldt.thumb_id[1], ldt.thumb_id[2], ldt.thumb_id[3]);
	}

out:
	return 0;
}
#endif

int delete_file_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN(pkglen, 4 + MAX_PATH_LEN);
	int loop;
	char file_path[MAX_PATH_LEN] = {0};
	int j = 0;
	UNPKG_H_UINT32(pkg, loop, j);
	UNPKG_STR(pkg, file_path, j, MAX_PATH_LEN);

	char buff[64] = {0};
	int k = sizeof(protocol_t);
	PKG_H_UINT32(buff, loop, k);
	if (access(file_path, F_OK) == -1) {
		ERROR_LOG("file not exist\t[%u %s]", userid, file_path);
		PKG_H_UINT32(buff, del_file_not_exist, k);
		goto out;
	}
	DEBUG_LOG("DEL FILE\t [userid=%u file_path=%s]",userid, file_path);
	errno = 0;
	if (unlink(file_path) == -1) {
		ERROR_LOG("unlink file error[%u %s]: %s", userid, file_path, strerror(errno));
		PKG_H_UINT32(buff, errno, k);
		goto out;
	}
	PKG_H_UINT32(buff, del_file_succ, k);
out:
	PKG_STR(buff, file_path, k, MAX_PATH_LEN);
	init_proto_head(buff, userid, cmd, k);
	net_send(fd, buff, k);
	return 0;
}


