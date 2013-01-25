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

static void delete_chg_thumb(chg_thumb_data_t* act, list_head_t* acur)
{
	DEBUG_LOG("Free CHG Thumb: [loccode=%s]", act->lloccode);
	if (act->new_thumb.thumb_data)
		gdFree(act->new_thumb.thumb_data);
	list_del(acur);
	free(act);
}

void send_chg_thumb_to_admin(chg_thumb_data_t* act)
{
	char buff[128];
	int j = sizeof(protocol_t);
	PKG_STR(buff, act->new_thumb.lloccode, j, LLOCCODE_LEN);
	init_proto_head(buff, act->userid, act->waitcmd, j);
	net_send(act->sockfd, buff, j);
}

static void chk_chg_thumb_result(chg_thumb_data_t* act, list_head_t* acur) 
{
	if (act->chg_thumb_tag == 1) {
		DEBUG_LOG("Chg-Thumb Down\t[%u %s]", act->userid, act->lloccode);
		send_chg_thumb_to_admin(act);
		delete_chg_thumb(act, acur);
	} else if ( act->chg_thumb_tag ) {
		ERROR_LOG("Chg-Thumb error\t[%u %u %u %u]", act->userid,
			act->f_type, act->b_type, act->chg_thumb_tag);
		send_error_to_admin(act->userid, act->waitcmd, act->sockfd, act->chg_thumb_tag);
		delete_chg_thumb(act, acur);
	} else if(get_now_tv()->tv_sec - act->chg_time > 2 * 60) {
		ERROR_LOG("Chg-Thumb time out\t[%u %u %u]", act->userid,
			act->f_type, act->b_type);
		send_error_to_admin(act->userid, act->waitcmd, act->sockfd, err_system_fatal_err);
		delete_chg_thumb(act, acur);
	}
}

void scan_chg_thumbs()
{
	int cnt = 0;
	list_head_t* cur = NULL;
	list_head_t* next = NULL;
	chg_thumb_data_t* lct;
	q_enter_list_for_each_safe(cur, next, g_chg_list) {
		lct = list_entry(cur, chg_thumb_data_t, list);
		DEBUG_LOG("SCAN Chg-Thumb[uid=%u cnt=%u]", lct->userid, cnt);
		chk_chg_thumb_result(lct, cur);
		if (cnt++ > 100) {
			break;
		}
	}
	q_exit_list_for_each_safe(g_chg_list);
}

gdImagePtr create_image_from_file(int pic_type, char* file_path)
{
        gdImagePtr lgd;
        FILE *fr = fopen(file_path, "r");
        switch (pic_type) {
        case JPG:
                lgd = gdImageCreateFromJpeg(fr);
                break;
        case PNG:
                lgd = gdImageCreateFromPng(fr);
                break;
        case GIF:
                lgd = gdImageCreateFromGif(fr);
                break;
        default:
                ERROR_LOG("bad pic type\t[%u]", pic_type);
                lgd = NULL;
                break;
        }
        fclose(fr);
        return lgd;
}

int do_chg_thumb(chg_thumb_data_t* act)
{
	gdImagePtr src_im = create_image_from_file(act->b_type, act->path);
	if (!src_im || src_im->sx == 0 || src_im->sy == 0) {
		ERROR_RETURN(("cannot create image\t[%u %s]", act->b_type, act->path), -1);
	}
	
	if (do_create_thumb(act->userid, src_im, &act->new_thumb, act->b_type, act->file_len)
		== -1) {
		gdImageDestroy(src_im);
		return -1;
	}
	gdImageDestroy(src_im);
	return 0;
}

void chg_thumb(gpointer data, gpointer user_data)
{
	chg_thumb_data_t* lct = (chg_thumb_data_t*)data;
	if (do_chg_thumb(lct) == -1) {
		lct->chg_thumb_tag = err_cannot_create_image;
		return;
	}

	if (do_save_thumb(lct->userid, &lct->new_thumb) == -1) {
		lct->chg_thumb_tag = err_cannot_save_file;
		return;
	}
	lct->chg_thumb_tag = 1;
}

int chg_thumb_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	int need_len = LLOCCODE_LEN + 6 * 4;
	if (pkglen != need_len) {
		send_error_to_admin(userid, cmd, fd, err_invalid_para);
		ERROR_RETURN(("pkg not enough\t [fd=%u userid=%u cmd=%u]", fd, userid, cmd), -1);
	}
	chg_thumb_data_t* lct = (chg_thumb_data_t*)malloc(FILE_WORK_BUFFER_LEN);
	if (lct == NULL) {
		send_error_to_admin(userid, cmd, fd, err_system_fatal_err);
		ERROR_RETURN(("no memory"), -1);
	}
	memset(lct, 0, FILE_WORK_BUFFER_LEN);
	lct->sockfd = fd;
	lct->userid = userid;
	lct->waitcmd = cmd;
	lct->chg_time = get_now_tv()->tv_sec;

	thumb_para_t* lnt = &lct->new_thumb;
	int j = 0;
	UNPKG_STR(pkg, lct->lloccode, j, LLOCCODE_LEN);
	
	UNPKG_H_UINT32(pkg, lnt->clip_w, j);
	UNPKG_H_UINT32(pkg, lnt->clip_h, j);
	UNPKG_H_UINT32(pkg, lnt->start_x, j);
	UNPKG_H_UINT32(pkg, lnt->start_y, j);
	UNPKG_H_UINT32(pkg, lnt->thumb_w, j);
	UNPKG_H_UINT32(pkg, lnt->thumb_h, j);

	if (lnt->clip_w <= 0 || lnt->clip_h <= 0) {
		lnt->clip_h = 0; lnt->clip_w = 0;
	}
	if (lnt->thumb_h <= 0 || lnt->thumb_w <= 0) {
		send_error_to_admin(userid, cmd, fd, err_invalid_para);
		ERROR_RETURN(("invalid thubm para\t [thumb_w=%d thumb_h=%d]",
			lnt->thumb_w, lnt->thumb_h), -1);
		goto err;
	}
	DEBUG_LOG("CHG THUMB PARA%u [%d %d %d %d %d %d]", lnt->clip_w, lnt->clip_h,
		lnt->start_x, lnt->start_y, lnt->thumb_w, lnt->thumb_h);

	int path_len = 0;
	if (analyse_lloccode_ex(lct->lloccode, lct->path, &path_len,
		&lct->f_type, &lct->b_type) == -1) {
		send_error_to_admin(userid, cmd, fd, err_llocc_fault);
		ERROR_LOG("big pic loccode err\t[%u %s]", userid, lct->lloccode);
		goto err;
	}

	if (access(lct->path, F_OK) == -1) {
		send_error_to_admin(userid, cmd, fd, err_src_file_not_exist);
		ERROR_LOG("file not exist\t[%u %s]", userid, lct->path);
		goto err;
	}
	if ((lct->file_len = get_file_len(lct->path)) == -1) {
		send_error_to_admin(userid, cmd, fd, err_src_file_not_exist);
		ERROR_LOG("file not exist\t[%u %s]", userid, lct->path);
		goto err;
	}
	
	create_picpath_llocc_logpath(userid, lct->f_type, lct->b_type, &lnt->dir_idx,
		&lnt->pic_idx, lnt->path, lnt->lloccode, NULL);
	
	qlist_add_tail (g_chg_list, &lct->list, &g_chg_list.head);
	g_thread_pool_push (pool3, lct, NULL);
	return 0;
	
err:	
	free(lct);	
	return -1;		
}



