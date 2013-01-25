
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


GThreadPool *pool1, *pool2;
qlist_t  gqlist;
int gcnt = 0;
static int uniq_key = 0xF;
static p_file_index_t g_file_index;
static p_file_index_t* pfi = &g_file_index;

static p_imp_file_index_t g_file_index_for_import;
static p_imp_file_index_t* pfi_fi = &g_file_index_for_import;

tmr_t  g_timer;
int save_file_cnt = 20;
int create_thumb_cnt = 10;

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

int imp_init_dir_index(const char* filepath)
{
	Q_MUTEX_INIT(pfi_fi->qmutex, 1);
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
	for (lp = 0; lp < MAX_IMP_DAYS; lp++) {
	    pfi_fi->cfi[lp] = (file_index_t *)(pmap + lp * sizeof(file_index_t));
		if (lp < 31) {
			pfi_fi->cfi[lp]->today_date.tm_year = 110;
			pfi_fi->cfi[lp]->today_date.tm_mon = 9;
			pfi_fi->cfi[lp]->today_date.tm_mday = lp + 1;
		} else if (lp < 61) {
			pfi_fi->cfi[lp]->today_date.tm_year = 110;
			pfi_fi->cfi[lp]->today_date.tm_mon = 10;
			pfi_fi->cfi[lp]->today_date.tm_mday = lp - 30;
		} else if (lp < 92) {
			pfi_fi->cfi[lp]->today_date.tm_year = 110;
			pfi_fi->cfi[lp]->today_date.tm_mon = 11;
			pfi_fi->cfi[lp]->today_date.tm_mday = lp - 60;
		} else if (lp < 123) {
			pfi_fi->cfi[lp]->today_date.tm_year = 111;
			pfi_fi->cfi[lp]->today_date.tm_mon = 0;
			pfi_fi->cfi[lp]->today_date.tm_mday = lp - 91;
		}
		DEBUG_LOG("date %4u%02u%02u dirid=%03u picid=%03u", 
			pfi_fi->cfi[lp]->today_date.tm_year + 1900, 
			pfi_fi->cfi[lp]->today_date.tm_mon + 1, 
			pfi_fi->cfi[lp]->today_date.tm_mday,
			pfi_fi->cfi[lp]->curr_dir_id, pfi_fi->cfi[lp]->curr_pic_id);
	}
	pfi_fi->mmap_file = pmap;
    close (fd);
	return 0;
}

void release_dir_index()
{	
	munmap(pfi->mmap_file, map_page_size);
}

void release_imp_dir_index()
{	
	munmap(pfi_fi->mmap_file, map_page_size);
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
	DEBUG_LOG("%s/%u/%4u%02u%02u/%03u/%03u.%s", file_root, type, 
		lfi->today_date.tm_year + 1900,	lfi->today_date.tm_mon + 1,
		lfi->today_date.tm_mday, *dir_id, *pic_id, suffix);
	snprintf(filepath, MAX_PATH_LEN, "%s/%u/%4u%02u%02u/%03u/%03u.%s", 
		file_root, type, lfi->today_date.tm_year + 1900,
		lfi->today_date.tm_mon + 1, lfi->today_date.tm_mday, 
		*dir_id, *pic_id, suffix);
	snprintf(lloccode, LLOCCODE_LEN, "%02X%u%u%4u%02u%02u%03u%03u%010u", 
		get_fs_id(config_get_strval("bind_ip")), type, bran_type, 
		lfi->today_date.tm_year + 1900,	lfi->today_date.tm_mon + 1,
		lfi->today_date.tm_mday, *dir_id, *pic_id, userid);
	snprintf(log_path, MAX_PATH_LEN, "%s%s", UPLOAD_LOG_ROOT, lloccode);
	
}

void imp_create_pic_path(uint32_t userid, uint32_t type, int bran_type, uint32_t* dir_id, uint32_t* pic_id, char* filepath, char* lloccode, time_t at_now)
{
	Q_MUTEX_ENTER(pfi_fi->qmutex);

	struct tm td;
	localtime_r(&at_now, &td);
	DEBUG_LOG("DATE\t [%u %u %u]", at_now, td.tm_mon, td.tm_year);
	file_index_t* lfi;
	int idx = 0;
	if (td.tm_mon == 9) {
		idx = td.tm_mday - 1;
	} else if (td.tm_mon == 10) {
		idx = 30 + td.tm_mday;
	} else if (td.tm_mon == 11) {
		idx = 60 + td.tm_mday;
	} else if (td.tm_mon == 0) {
		idx = 91 + td.tm_mday;
	} 
	
	lfi = pfi_fi->cfi[idx];
	if (lfi->curr_pic_id >= MAX_PIC_ID) {
		lfi->curr_pic_id = 1;
		lfi->curr_dir_id++;
		if (lfi->curr_dir_id > MAX_DIR_ID){
			WARN_LOG("too much dir\t[%u]", lfi->curr_dir_id);
		}
	} else {
		lfi->curr_pic_id++;
	}
	
	*dir_id = lfi->curr_dir_id;
	*pic_id = lfi->curr_pic_id;
	Q_MUTEX_LEAVE(pfi_fi->qmutex);
	char* suffix = "def";
	if (type == picture_file_type) {
		suffix = (bran_type == GIF) ? "GIF" : ((bran_type == PNG ? "PNG" : "JPEG"));
	}
	DEBUG_LOG("%s/%u/%4u%02u%02u/%03u/%03u.%s", file_root, type, 
		lfi->today_date.tm_year + 1900,	lfi->today_date.tm_mon + 1, 
		lfi->today_date.tm_mday, *dir_id, *pic_id, suffix);
	snprintf(filepath, MAX_PATH_LEN, "%s/%u/%4u%02u%02u/%03u/%03u.%s", 
		file_root, type, lfi->today_date.tm_year + 1900,
		lfi->today_date.tm_mon + 1, lfi->today_date.tm_mday, 
		*dir_id, *pic_id, suffix);
	snprintf(lloccode, LLOCCODE_LEN, "%02u%u%u%4u%02u%02u%03u%03u%010u", 
		get_fs_id(config_get_strval("bind_ip")), type, bran_type, 
		lfi->today_date.tm_year + 1900,	lfi->today_date.tm_mon + 1,
		lfi->today_date.tm_mday, *dir_id, *pic_id, userid);
}

int file_exist(const char *filepath) {
	if(access(filepath, F_OK) == 0) return 1;
	return 0;
}

char* file_get_dir(const char *filepath) {
	char *path = strdup(filepath);
	char *dname = dirname(path);
	DEBUG_LOG("DIR NAME\t[%s]", dname);
	char *dir = strdup(dname);
	free(path);
	return dir;
}

int file_make_dir(const char *dirpath, mode_t mode, int recursive) {
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

void save_file (gpointer data, gpointer user_data)
{
	work_data_t* lwd = (work_data_t*)data;
	char* dir_name = file_get_dir(lwd->path);
	if (!file_exist(dir_name)) {
		file_make_dir(dir_name, DEF_DIR_MODE, 1);
	}
	free(dir_name);
	
	int fd = open(lwd->path, O_RDWR|O_CREAT, DEF_FILE_MODE);
    if(fd < 0) {
		ERROR_LOG("open file %s error:%s", lwd->path, strerror(errno));
		return;
    }
	int limit_len = lwd->file_len;
	char* limit_data = lwd->file_data;
	int flag = 0;
	if (lwd->file_type == picture_file_type) {
		if (limit_picture(lwd, &limit_data, &limit_len, &flag) == -1) {
			close(fd);
			return;
		}
	}
	if (write(fd, limit_data, limit_len) == limit_len) {
		DEBUG_LOG("write file %s success\t[uid=%u u_key=%u len=%u]", 
			lwd->path, lwd->userid, lwd->unique_key, lwd->file_len);
		lwd->save_file_tag = 1;
		char log_str[MAX_LOG_LEN] = {0};
		snprintf(log_str, MAX_LOG_LEN, "Saving file to %s", lwd->path);
		write_log_file(lwd, log_str);
	} else {
		ERROR_LOG("write file %s error\t[uid=%u u_key=%u]",
			lwd->path, lwd->userid, lwd->unique_key);
	}
	close(fd);
	if (flag) {
		gdFree(limit_data);
	}
}

int upload_file_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	work_data_t* lwd = (work_data_t*)malloc(FILE_WORK_BUFFER_LEN);
	if (lwd == NULL) {
		ERROR_RETURN(("upload-file no memory"), -1);
	}
	memset(lwd, 0, FILE_WORK_BUFFER_LEN);
	lwd->unique_key = uniq_key++;
	lwd->userid 	= userid;
	lwd->waitcmd 	= cmd;
	lwd->sockfd 	= fd;
	lwd->upload_time = get_now_tv()->tv_sec;

	if (pkglen < 24) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("upload-file pkg not enough\t [fd=%u userid=%u cmd=%u]", fd, userid, cmd);
		goto err;
	}
	
	int j = 0;
	UNPKG_H_UINT32(pkg, lwd->key, j);  // unique key for album id
	if (g_hash_table_lookup(upload_maps, &lwd->key)) {
		send_error_to_cgi(userid, cmd, fd, err_upload_busy_now);
		ERROR_LOG("upload-file upload too quickly\t [fd=%u userid=%u cmd=%u]", fd, userid, cmd);
		goto err;
	}
	UNPKG_H_UINT32(pkg, lwd->file_type, j);
	UNPKG_H_UINT32(pkg, lwd->branch_type, j);
	UNPKG_H_UINT32(pkg, lwd->w_limit, j);
	UNPKG_H_UINT32(pkg, lwd->h_limit, j);
	UNPKG_H_UINT32(pkg, lwd->thumb_cnt, j);
	
	DEBUG_LOG("FiLE\t [uid=%u b_key=%u u_key=%u f_type=%u b_type=%u w_limit=%u h_limit=%u th_cnt=%u]",
		userid, lwd->key, lwd->unique_key, lwd->file_type, lwd->branch_type,
		lwd->w_limit, lwd->h_limit, lwd->thumb_cnt);
	if (lwd->file_type >= MAX_FILE_TYPE || lwd->branch_type > GIF 
		|| lwd->file_type == 0 || lwd->branch_type == 0
		|| lwd->w_limit < 0 || lwd->h_limit < 0
		|| lwd->thumb_cnt > MAX_THUMB_CNT_PER_PIC) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("invalid file para\t [uid=%u b_key=%u u_key=%u type=%u branch_type=%u thumb_cnt=%u]", 
			userid, lwd->key, lwd->unique_key, lwd->file_type, lwd->branch_type, lwd->thumb_cnt);
		goto err;
	}
	if (pkglen < 24 + lwd->thumb_cnt * 24 + 4) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("pkg not enough\t [uid=%u b_key=%u u_key=%u type=%u branch_type=%u thumb_cnt=%u]", 
			userid, lwd->key, lwd->unique_key, lwd->file_type, lwd->branch_type, lwd->thumb_cnt);
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
			ERROR_LOG("invalid thubm para\t [uid=%u key=%u thumb_w=%d thumb_h=%d]", userid, 
				lwd->key, ltp->thumb_w, ltp->thumb_h);
			goto err;
		}
		DEBUG_LOG("THUMB PARA%u [u_key=%u %u %u %u %u %u %u]",lwd->unique_key, loop, 
			ltp->clip_w, ltp->clip_h, ltp->start_x, ltp->start_y, ltp->thumb_w, ltp->thumb_h);
	}
	UNPKG_H_UINT32(pkg, lwd->file_len, j);
	if (pkglen < 24 + lwd->thumb_cnt * 24 + 4 + lwd->file_len) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("file content not enough\t [uid=%u key=%u file_len=%u pkglen=%u]", 
			userid, lwd->key, lwd->file_len, pkglen);
		goto err;
	}
	if (lwd->file_len > MAX_FILE_LEN) {
		send_error_to_cgi(userid, cmd, fd, err_file_too_large);
		ERROR_LOG("invalid file len\t[uid=%u, len=%u]", userid, lwd->file_len);
		goto err;
	}
	UNPKG_STR(pkg, lwd->file_data, j, lwd->file_len);
	if (lwd->file_type != picture_file_type) {
		lwd->thumb_cnt = 0;
	}
	create_picpath_llocc_logpath(lwd->userid, lwd->file_type, lwd->branch_type, &lwd->dir_idx,
		&lwd->pic_idx, lwd->path, lwd->lloccode, lwd->log_path);
	if (open_log_file(lwd) == -1) {
		goto err;
	}
	char log_str[MAX_LOG_LEN] = {0};
	snprintf(log_str, MAX_LOG_LEN, "File %s received, saving to album %u", lwd->lloccode, lwd->key);
	write_log_file(lwd, log_str);
	g_hash_table_insert(upload_maps, &lwd->key, lwd);
	DEBUG_LOG("UPLAOD-HASH-SIZE\t[%u]", get_upload_hash_size());
	qlist_add_tail (gqlist, &lwd->list, &gqlist.head);
	gcnt++;
	g_thread_pool_push (pool1, lwd, NULL);
	if (!lwd->thumb_cnt) {
		lwd->done_thumb_tag = 1;
		create_lloccode(lwd->lloccode, 0, 0, 0, 0);
		send_result_to_cgi(lwd);
		return -1; // close connection
	} else {
		g_thread_pool_push (pool2, lwd, NULL);
	}
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
	CHECK_BODY_LEN(pkglen, MAX_PATH_LEN);
	char file_path[MAX_PATH_LEN] = {0};
	int j = 0;
	UNPKG_STR(pkg, file_path, j, MAX_PATH_LEN);
	if (access(file_path, F_OK) == -1) {
		ERROR_RETURN(("file not exist\t[%u %s]", userid, file_path), 0);
	}
	DEBUG_LOG("DEL FILE\t [userid=%u file_path=%s]",userid, file_path);
	if (unlink(file_path) == -1) {
		ERROR_LOG("unlink file error[%u %s]: %s", userid, file_path, strerror(errno));
	}
	return 0;
}

int imp_upload_file(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	work_data_t* lwd = (work_data_t*)malloc(FILE_WORK_BUFFER_LEN);
	if (lwd == NULL) {
		ERROR_RETURN(("no memory"), -1);
	}
	memset(lwd, 0, FILE_WORK_BUFFER_LEN);
	lwd->unique_key = uniq_key++;
	lwd->userid = userid;
	lwd->waitcmd = cmd;
	lwd->sockfd = fd;
	lwd->upload_time = get_now_tv()->tv_sec;

	if (pkglen < 28) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("pkg not enough\t [fd=%u userid=%u cmd=%u]", fd, userid, cmd);
		free(lwd);
		return -1;
	}
	
	int j = 0;
	UNPKG_H_UINT32(pkg, lwd->key, j);  // unique key for album id
	UNPKG_H_UINT32(pkg, lwd->file_type, j);
	UNPKG_H_UINT32(pkg, lwd->branch_type, j);
	UNPKG_H_UINT32(pkg, lwd->imp_upload_time, j);
	UNPKG_H_UINT32(pkg, lwd->w_limit, j);
	UNPKG_H_UINT32(pkg, lwd->h_limit, j);
	UNPKG_H_UINT32(pkg, lwd->thumb_cnt, j);

	struct tm td;
	time_t lt = lwd->imp_upload_time;
	localtime_r(&lt, &td);
	DEBUG_LOG("DATE\t [%u %u %u]", lwd->imp_upload_time, td.tm_mon, td.tm_year);
	if ((td.tm_mon != 9 && td.tm_mon != 10 && td.tm_mon != 11 && td.tm_mon != 0)
		|| (td.tm_year != 110 && td.tm_year != 111)){
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("invalid month\t [%u %u %u]", lwd->imp_upload_time, td.tm_mon, td.tm_year);
		free(lwd);
		return -1;
	}
	if (pkglen < 28 + lwd->thumb_cnt * 24 + 4) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("pkg not enough\t [fd=%u key=%u type=%u branch_type=%u thumb_cnt=%u]", fd, lwd->key, lwd->file_type, lwd->branch_type, lwd->thumb_cnt);
		free(lwd);
		return -1;
	}
	DEBUG_LOG("FiLE\t [fd=%u key=%u type=%u branch_type=%u thumb_cnt=%u]", fd, lwd->key, lwd->file_type, lwd->branch_type, lwd->thumb_cnt);
	if (lwd->file_type >= MAX_FILE_TYPE || lwd->branch_type > GIF 
		|| lwd->file_type == 0 || lwd->branch_type == 0
		|| lwd->thumb_cnt > MAX_THUMB_CNT_PER_PIC) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("invalid file para\t [fd=%u key=%u type=%u branch_type=%u thumb_cnt=%u]", fd, lwd->key, lwd->file_type, lwd->branch_type, lwd->thumb_cnt);
		free(lwd);
		return -1;
	}
	int loop;
	for (loop = 0; loop < MAX_THUMB_CNT_PER_PIC && loop < lwd->thumb_cnt; loop++) {
		UNPKG_H_UINT32(pkg, lwd->thumb_paras[loop].clip_w, j);
		UNPKG_H_UINT32(pkg, lwd->thumb_paras[loop].clip_h, j);
		UNPKG_H_UINT32(pkg, lwd->thumb_paras[loop].start_x, j);
		UNPKG_H_UINT32(pkg, lwd->thumb_paras[loop].start_y, j);
		UNPKG_H_UINT32(pkg, lwd->thumb_paras[loop].thumb_w, j);
		UNPKG_H_UINT32(pkg, lwd->thumb_paras[loop].thumb_h, j);

		DEBUG_LOG("THUMB PARA%u [%u %u %u %u %u %u]", loop, lwd->thumb_paras[loop].clip_w, lwd->thumb_paras[loop].clip_h,
			lwd->thumb_paras[loop].start_x, lwd->thumb_paras[loop].start_y, lwd->thumb_paras[loop].thumb_w, lwd->thumb_paras[loop].thumb_h);
	}
	UNPKG_H_UINT32(pkg, lwd->file_len, j);
	if (pkglen < 28 + lwd->thumb_cnt * 24 + 4 + lwd->file_len) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("pkg not enough\t [fd=%u key=%u type=%u branch_type=%u thumb_cnt=%u file_len]", 
			fd, lwd->key, lwd->file_type, lwd->branch_type, lwd->thumb_cnt, lwd->file_len);
		free(lwd);
		return -1;
	}
	if (lwd->file_len > MAX_FILE_LEN) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("invalid file len\t[uid=%u, len=%u]", userid, lwd->file_len);
		free(lwd);
		return -1;
	}
	UNPKG_STR(pkg, lwd->file_data, j, lwd->file_len);	
	if (lwd->file_type != picture_file_type) {
		lwd->thumb_cnt = 0;
	}
	imp_create_pic_path(lwd->userid, lwd->file_type, lwd->branch_type, &lwd->dir_idx, &lwd->pic_idx, lwd->path, lwd->lloccode, lwd->imp_upload_time);
	
	qlist_add_tail (gqlist, &lwd->list, &gqlist.head);
	gcnt++;
	g_thread_pool_push (pool1, lwd, NULL);
	if (!lwd->thumb_cnt) {
		lwd->done_thumb_tag = 1;
		create_lloccode(lwd->lloccode, 0, 0, 0, 0);
		send_result_to_cgi(lwd);
		return -1; // close connection
	} else {
		g_thread_pool_push (pool2, lwd, NULL);
	}
	return 0;

}

int imp_upload_logo(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	work_data_t* lwd = (work_data_t*)malloc(FILE_WORK_BUFFER_LEN);
	if (lwd == NULL) {
		ERROR_RETURN(("no memory"), -1);
	}
	memset(lwd, 0, FILE_WORK_BUFFER_LEN);
	lwd->unique_key = uniq_key++;
	lwd->userid = userid;
	lwd->waitcmd = cmd;
	lwd->sockfd = fd;
	lwd->upload_time = get_now_tv()->tv_sec;

	if (pkglen < 20) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("pkg not enough\t [fd=%u userid=%u cmd=%u]", fd, userid, cmd);
		free(lwd);
		return -1;
	}
	
	int j = 0;
	UNPKG_H_UINT32(pkg, lwd->key, j);  // unique key for album id
	UNPKG_H_UINT32(pkg, lwd->file_type, j);
	UNPKG_H_UINT32(pkg, lwd->branch_type, j);
	UNPKG_H_UINT32(pkg, lwd->imp_upload_time, j);
	UNPKG_H_UINT32(pkg, lwd->thumb_cnt, j);

	struct tm td;
	time_t lt = lwd->imp_upload_time;
	localtime_r(&lt, &td);
	DEBUG_LOG("DATE\t [%u %u %u]", lwd->imp_upload_time, td.tm_mon, td.tm_year);
	if ((td.tm_mon != 9 && td.tm_mon != 10 && td.tm_mon != 11 && td.tm_mon != 0)
		|| (td.tm_year != 110 && td.tm_year != 111)){
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("invalid month\t [%u %u %u]", lwd->imp_upload_time, td.tm_mon, td.tm_year);
		free(lwd);
		return -1;
	}
	DEBUG_LOG("FiLE\t [fd=%u key=%u type=%u branch_type=%u thumb_cnt=%u]", 
		fd, lwd->key, lwd->file_type, lwd->branch_type, lwd->thumb_cnt);
	int loop;
	for (loop = 0; loop < MAX_THUMB_CNT_PER_PIC && loop < lwd->thumb_cnt; loop++) {
		thumb_para_t* ltp = &lwd->thumb_paras[loop];
		UNPKG_H_UINT32(pkg, ltp->thumb_len, j);
		int len = ltp->thumb_len;
		ltp->thumb_data = malloc(len);
		memset(lwd, 0, len);
		if (pkglen < j + len) {
			send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
			ERROR_LOG("pkg not enough1\t[%u %u %u]", j, len, pkglen);
			free(lwd);
			return -1;
		}
		UNPKG_STR(pkg, ltp->thumb_data, j, len);
		DEBUG_LOG("THUMB LEN%u [%u]", loop, len);
	}
	UNPKG_H_UINT32(pkg, lwd->file_len, j);
	if (pkglen < j + lwd->file_len) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("pkg not enough2\t[%u %u %u]", j, lwd->file_len, pkglen);
		free(lwd);
		return -1;
	}
	if (lwd->file_len > MAX_FILE_LEN) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("invalid file len\t[uid=%u, len=%u]", userid, lwd->file_len);
		free(lwd);
		return -1;
	}
	UNPKG_STR(pkg, lwd->file_data, j, lwd->file_len);	
	imp_create_pic_path(userid, lwd->file_type, lwd->branch_type, &lwd->dir_idx, &lwd->pic_idx, lwd->path, lwd->lloccode, lwd->imp_upload_time);
	
	qlist_add_tail (gqlist, &lwd->list, &gqlist.head);
	gcnt++;
	g_thread_pool_push (pool1, lwd, NULL);
	lwd->make_thumb_tag = 1;
	return 0;

}

