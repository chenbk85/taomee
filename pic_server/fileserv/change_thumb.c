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
#include "lloccode.h"
#include "thumb.h"
#include "change_thumb.h"




static GHashTable* chg_thumb_maps;

//------------------------------------------------------
static inline void
free_chg_thumb(void* afc)
{
	work_data_t* p = afc;
	DEBUG_LOG("Free CHG Thumb: [fd=%u albumid=%u loccode=%s]", 
		p->sockfd, p->key, p->lloccode);
	remove_thumb_picture(p);
	free(p);
}

void init_chg_thumb_hash()
{
	chg_thumb_maps = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_chg_thumb);
}

void fini_chg_thumb_hash()
{
	g_hash_table_destroy(chg_thumb_maps);
}

int get_chg_thumb_hash_size()
{
	return g_hash_table_size(chg_thumb_maps);
}


gdImagePtr create_image_from_file(int pic_type, char* file_path)
{
	FILE *fr = fopen(file_path, "r");
	switch (pic_type) {
	case JPG:
		return gdImageCreateFromJpeg(fr);
	case PNG:
		return gdImageCreateFromPng(fr);
	case GIF:
		return gdImageCreateFromGif(fr);
	default:
		ERROR_LOG("bad pic type\t[%u]", pic_type);
		return NULL;
	}
	fclose(fr);
}

static char thumbs_buffer[4 * 1024 * 1024] = {0};
int chg_thumbs_to_thumbserv(work_data_t* awd, int cnt, uint32_t thumbids[MAX_THUMB_CNT_PER_PIC])
{
	int j = sizeof(protocol_t);
	PKG_H_UINT32(thumbs_buffer, awd->sockfd, j);
	PKG_H_UINT32(thumbs_buffer, awd->key, j);
	PKG_H_UINT32(thumbs_buffer, cnt, j);
	int lp;
	for (lp = 0; lp < cnt; lp++) {
		PKG_H_UINT32(thumbs_buffer, thumbids[lp], j);
	}
	PKG_H_UINT32(thumbs_buffer, awd->thumb_cnt, j);
	for (lp = 0; lp < awd->thumb_cnt; lp++) {
		if (awd->thumb_paras[lp].thumb_data) {
			PKG_H_UINT32(thumbs_buffer, awd->thumb_paras[lp].thumb_len, j);
			PKG_STR(thumbs_buffer, awd->thumb_paras[lp].thumb_data, j, awd->thumb_paras[lp].thumb_len);
		}
	}
	init_proto_head(thumbs_buffer, awd->userid, proto_ts_change_thumb, j);
	return send_to_thumbserv(thumbs_buffer, j, awd->key);
}

static int get_file_len(const char* path)
{
	struct stat buf;
	int ret = stat(path, &buf);
	if (ret)
		return -1;

	return buf.st_size;
}

int change_picture_thumbs_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	work_data_t* lwd = (work_data_t*)malloc(FILE_WORK_BUFFER_LEN);
	if (lwd == NULL) {
		ERROR_RETURN(("no memory"), -1);
	}
	memset(lwd, 0, FILE_WORK_BUFFER_LEN);
	lwd->unique_key = 0;
	lwd->userid = userid;
	lwd->waitcmd = cmd;
	lwd->sockfd = fd;
	lwd->upload_time = get_now_tv()->tv_sec;

	if (pkglen < 72) {
		ERROR_LOG("pkg not enough\t [fd=%u userid=%u cmd=%u]", fd, userid, cmd);
		goto err;
	}
	
	int j = 0;
	UNPKG_H_UINT32(pkg, lwd->key, j);  // unique key for album id
	UNPKG_STR(pkg, lwd->lloccode, j, LLOCCODE_LEN);
	UNPKG_H_UINT32(pkg, lwd->thumb_cnt, j);
	if (lwd->thumb_cnt > MAX_THUMB_CNT_PER_PIC) {
		send_error_to_cgi(userid, cmd, fd, err_cgi_para_err);
		ERROR_LOG("invalid thumb cnt\t [fd=%u userid=%u loccode=%s thumb_cnt=%u]", 
			fd, userid, lwd->lloccode, lwd->thumb_cnt);
		goto err;
	}

	if (pkglen < 72 + lwd->thumb_cnt * 24) {
		ERROR_LOG("pkg not enough\t [fd=%u userid=%u loccode=%s]", fd, userid, lwd->lloccode);
		goto err;
	}
	
	DEBUG_LOG("CHG THUMB\t [fd=%u userid=%u thumb_cnt=%u loccode=%s]", fd, userid, 
		lwd->thumb_cnt, lwd->lloccode);
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
			ERROR_LOG("invalid thubm para\t [fd=%u key=%u thumb_w=%d thumb_h=%d]", fd, 
				lwd->key, ltp->thumb_w, ltp->thumb_h);
			goto err;
		}
		DEBUG_LOG("CHG THUMB PARA%u [%u %d %d %d %d %d]", loop, ltp->clip_w, ltp->clip_h,
			ltp->start_x, ltp->start_y, ltp->thumb_w, ltp->thumb_h);
	}

	int hostid = 0, path_len = 0;
	uint32_t thumbids[MAX_THUMB_CNT_PER_PIC] = {0}, thum_cnt = 0;
	int type = 0, branchtype = 0;
	if (analyse_lloccode_ex(lwd->lloccode, &hostid, thumbids, &thum_cnt, lwd->path,
		&path_len, &type, &branchtype) == -1) {
		ERROR_LOG("invalid file loccode\t [fd=%u userid=%u type=%u bt=%u loccode=%s]", fd, userid, 
			type, branchtype, lwd->lloccode);
		goto err;
	}
	if (type != picture_file_type || branchtype > GIF) {
		ERROR_LOG("invalid file loccode\t [fd=%u userid=%u type=%u bt=%u loccode=%s]", fd, userid, 
			type, branchtype, lwd->lloccode);
		goto err;
	}
	lwd->file_type = type;
	lwd->branch_type = branchtype;

	if (access(lwd->path, F_OK) == -1) {
		send_error_to_cgi(userid, cmd, fd, err_src_file_not_exist);
		ERROR_LOG("file not exist\t[%u %s]", userid, lwd->path);
		goto err;
	}
	
	if ((lwd->file_len = get_file_len(lwd->path)) == -1)
		lwd->file_len = 500*1024;
	
	gdImagePtr src_im = create_image_from_file(lwd->branch_type, lwd->path);
	if (!src_im || src_im->sx == 0 || src_im->sy == 0) {
		ERROR_LOG("cannot create image\t[%u %u]", lwd->unique_key, lwd->key);
		goto err;
	}
	
	int lp;
	for (lp = 0; lp < lwd->thumb_cnt; lp++) {
		if (do_create_thumb(src_im, &lwd->thumb_paras[lp], lwd) == -1) {
			gdImageDestroy(src_im);
			goto err;
		}
	}
	gdImageDestroy(src_im);
	
	//  change thumbs
	if (chg_thumbs_to_thumbserv(lwd, thum_cnt, thumbids) == -1) {
		goto err;
	}
	g_hash_table_replace(chg_thumb_maps, &(lwd->sockfd), lwd);
	return 0;
err:
	remove_thumb_picture(lwd);
	free(lwd);
	return -1;		
}

int change_picture_thumbs_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN_GE(pkglen, 8);
	int j = 0, sockfd = 0, add_cnt;
	UNPKG_H_UINT32(pkg, sockfd, j);
	work_data_t* lwd = g_hash_table_lookup(chg_thumb_maps, &sockfd);
	if (lwd == NULL) {
		ERROR_RETURN(("cannot find work data\t[%u %u %u]", userid, cmd, sockfd), -1);
	}
	
	UNPKG_H_UINT32(pkg, add_cnt, j);
	if (add_cnt != lwd->thumb_cnt) {
		send_error_to_cgi(userid, lwd->waitcmd, lwd->sockfd, err_system_fatal_err);
		goto out;
	}

	CHECK_BODY_LEN_GE(pkglen, 8 + add_cnt * 4);
	int lp;
	for (lp = 0; lp < add_cnt; lp++) {
		UNPKG_H_UINT32(pkg, lwd->thumb_paras[lp].thumb_id, j);
		if (!lwd->thumb_paras[lp].thumb_id) {
			send_error_to_cgi(userid, lwd->waitcmd, lwd->sockfd, err_album_too_much_thumbs);
			goto out;
		}
	}
	create_lloccode(lwd->lloccode, lwd->thumb_paras[0].thumb_id, lwd->thumb_paras[1].thumb_id,
		lwd->thumb_paras[2].thumb_id, lwd->thumb_paras[3].thumb_id);
	
	send_result_to_cgi(lwd);
	do_del_conn(lwd->sockfd);
out:
	g_hash_table_remove(chg_thumb_maps, &lwd->sockfd);
	return 0;
}

