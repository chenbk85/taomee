
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>


#include <iter_serv/net_if.h>
#include <iter_serv/net.h>

#include "util.h"
#include "error_nbr.h"
#include "common.h"
#include "save_file.h"
#include "proto.h"
#include "filelog.h"
#include "thumb.h"

gdImagePtr create_image(int pic_type, int size, void* data)
{
	switch (pic_type) {
	case JPG:
		return gdImageCreateFromJpegPtr(size, data);
	case PNG:
		return gdImageCreateFromPngPtr(size, data);
	case GIF:
		return gdImageCreateFromGifPtr(size, data);
	default:
		ERROR_LOG("bad pic type\t[%u]", pic_type);
		return NULL;
	}
}

char* create_picture(gdImagePtr im, uint32_t pic_type, int* pic_size, uint64_t revise)
{
	switch (pic_type) {
	case JPG:
		return gdImageJpegPtr(im, pic_size, revise);
	case PNG:
		return gdImagePngPtrEx(im, pic_size, 1); // 9 compress as much as possible
	case GIF:
		return gdImageGifPtr(im, pic_size);
	default:
		ERROR_LOG("bad pic type\t[%u]", pic_type);
		return NULL;
	}
}

int do_create_thumb(gdImagePtr src_im, thumb_para_t* pth, work_data_t* awd)
{
	int clip_w = pth->clip_w;
	int clip_h = pth->clip_h;
	int start_x = pth->start_x;
	int start_y = pth->start_y;
	int thumb_w = pth->thumb_w;
	int thumb_h = pth->thumb_h;
	
	int dst_start_x = 0, dst_start_y = 0;
	int src_w = src_im->sx, src_h = src_im->sy;
	int to_w = thumb_w, to_h = thumb_h;

	float w_rate = (float)(thumb_w) / src_im->sx;
	float h_rate = (float)(thumb_h) / src_im->sy;
	if (start_x > src_w) start_x = 0;
	if (start_y > src_h) start_y = 0;
	if (clip_w && clip_h) { // if need to be cliped
		if (start_x < 0) {
			dst_start_x = -start_x * thumb_w / clip_w;
			start_x = 0;
		}
		if (start_y < 0) {
			dst_start_y = -start_y * thumb_h / clip_h;
			start_y = 0;
		}
		
		if (start_x + clip_w > src_w) src_w = src_w - start_x;
		if (start_y + clip_h > src_h) src_h = src_h - start_y;
		to_w = thumb_w * src_w / clip_w;
		to_h = thumb_h * src_h / clip_h;
	} else if (thumb_w > src_w && thumb_h > src_h) {
		to_w = thumb_w = src_w;
		to_h = thumb_h = src_h;
	} else if (w_rate > h_rate) {
		int tmp = src_w * thumb_h / src_h;
		to_w = thumb_w = tmp ? tmp : 2;
	} else if (h_rate > w_rate) {
		int tmp = src_h * thumb_w / src_w;
		to_h = thumb_h = tmp ? tmp : 2;
	}
	
	gdImagePtr dst_im = gdImageCreateTrueColor(thumb_w, thumb_h);
	if (!dst_im) {
		ERROR_LOG("no memory\t[%u %u %u]", awd->unique_key, thumb_w, thumb_h);
		return -1;
	}
	int bg_color = gdImageColorAllocate(dst_im, 255, 255, 255);
	gdImageFilledRectangle(dst_im, 0, 0, thumb_w, thumb_h, bg_color);
	gdImageCopyResampled(dst_im, src_im, dst_start_x, dst_start_y, start_x, start_y, to_w, to_h, src_w, src_h);
	uint64_t revise = 1;
	if (awd->branch_type == JPG) {
		revise = revise * 100 * src_im->sx * src_im->sy * 386204;
		revise = revise / 800 / 600 / awd->file_len;
		DEBUG_LOG("REVISE\t[%u %u %u %u]", revise, src_im->sx, src_im->sy, awd->file_len);
		revise = (revise < 10) ? 10 : revise;
	}
	pth->thumb_data = create_picture(dst_im, awd->branch_type, &pth->thumb_len, revise);
	if (!pth->thumb_data) {
		ERROR_LOG("no memory\t[%u]", awd->unique_key);
		gdImageDestroy(dst_im);
		return -1;
	}
	DEBUG_LOG("THUMB DATA\t[len=%u, data=%p]", pth->thumb_len, pth->thumb_data);
	gdImageDestroy(dst_im);
	return 0;
}

void create_thumbs (gpointer data, gpointer user_data)
{
	work_data_t* lwd = (work_data_t*)data;
	write_log_file(lwd, "Begin generating thumbs");
	if (!lwd->thumb_cnt) {
		write_log_file(lwd, "No need");
		return;
	}

	gdImagePtr src_im = create_image(lwd->branch_type, lwd->file_len, lwd->file_data);
	if (!src_im) {
		ERROR_LOG("cannot create image\t[%u %u]", lwd->unique_key, lwd->key);
		return;
	}

	int loop;
	for (loop = 0; loop < lwd->thumb_cnt; loop++) {
		if (do_create_thumb(src_im, &lwd->thumb_paras[loop], lwd) == -1)
			break;
	}
	gdImageDestroy(src_im);
	lwd->make_thumb_tag = 1;
}

int limit_picture(work_data_t* lwd, char** limit_data, int* limit_len, int* limit_flag)
{
	if (lwd->w_limit <= 0 || lwd->h_limit <= 0 || lwd->branch_type == GIF) {
		return 0;
	}
	gdImagePtr src_im = create_image(lwd->branch_type, lwd->file_len, lwd->file_data);
	if (!src_im) {
		ERROR_RETURN(("cannot create image\t[%u %u]", lwd->unique_key, lwd->key), -1);
	}
	
	if (lwd->w_limit >= src_im->sx && lwd->h_limit >= src_im->sy
		|| src_im->sx == 0 || src_im->sy == 0) {
		gdImageDestroy(src_im);
		return 0;
	}
	DEBUG_LOG("LIMIT PICTURE\t[%u %u %u]", lwd->unique_key, lwd->file_type, lwd->branch_type);
	float w_rate = (float)(lwd->w_limit) / src_im->sx;
	float h_rate = (float)(lwd->h_limit) / src_im->sy;
	if (lwd->w_limit < src_im->sx && lwd->h_limit > src_im->sy) {
		lwd->h_limit = src_im->sy * lwd->w_limit / src_im->sx;
	}else if (lwd->h_limit < src_im->sy && lwd->w_limit > src_im->sx) {
		lwd->w_limit = src_im->sx * lwd->h_limit / src_im->sy;
	} else if (w_rate < h_rate){
		lwd->h_limit = src_im->sy * lwd->w_limit / src_im->sx;
	} else if (w_rate > h_rate){
		lwd->w_limit = src_im->sx * lwd->h_limit / src_im->sy;
	}
	gdImagePtr dst_im = gdImageCreateTrueColor(lwd->w_limit, lwd->h_limit);
	if (!dst_im) {
		gdImageDestroy(src_im);
		ERROR_LOG("no memory or para err\t[%u %u %u]", lwd->unique_key, lwd->w_limit, lwd->h_limit);
		return 0;
	}
	int bg_color = gdImageColorAllocate(dst_im, 255, 255, 255);
	gdImageFilledRectangle(dst_im, 0, 0, lwd->w_limit, lwd->h_limit, bg_color);
	gdImageCopyResampled(dst_im, src_im, 0, 0, 0, 0, lwd->w_limit, lwd->h_limit, src_im->sx, src_im->sy);
	*limit_data = create_picture(dst_im, lwd->branch_type, limit_len, 100);
	gdImageDestroy(src_im);
	gdImageDestroy(dst_im);
	if (!(*limit_data)) {
		ERROR_RETURN(("cannot create picture when limit picture\t[%u]", lwd->key), -1);
	}
	*limit_flag = 1;
	return 0;
}

void remove_thumb_picture(work_data_t* lwd)
{
	int loop;
	for (loop = 0; loop < lwd->thumb_cnt && loop < MAX_THUMB_CNT_PER_PIC; loop++) {
		thumb_para_t* ltp = &lwd->thumb_paras[loop];
		if (ltp->thumb_data) {
			if (lwd->waitcmd == imp_proto_upload_logo) {
				free(ltp->thumb_data);
			} else {
				gdFree(ltp->thumb_data);
			}
			ltp->thumb_data = NULL;
			ltp->thumb_len = 0;
		}
	}
}

#if 0
void create_lloccode(work_data_t* awd)
{
	int len = strlen(awd->lloccode);
	char* str_begin = len + awd->lloccode;
	snprintf(str_begin, LLOCCODE_LEN - len, "%010u%08X%08X%08X%08X", awd->userid,
		awd->thumb_paras[0].thumb_id,
		awd->thumb_paras[1].thumb_id,
		awd->thumb_paras[2].thumb_id,
		awd->thumb_paras[3].thumb_id);
	
	uint16_t chksum = lloccode_checksum((uint8_t*)(awd->lloccode), LLOCCODE_LEN - 4);
	uint8_t	 short_chksum = chksum;
	DEBUG_LOG("CHKSUM\t[%x %x]", chksum, short_chksum);
	snprintf(awd->lloccode + LLOCCODE_LEN - 4, 5, "%02X%02u", short_chksum, 0);
	DEBUG_LOG("LLOCCODE\t[%s]", awd->lloccode);
}
#endif
void send_error_to_cgi(uint32_t userid, uint16_t cmd, int fd, int err)
{
	
	static char buf[256] = {0};
	int j = sizeof(protocol_t);
	protocol_t* pp = (protocol_t*)buf;
	pp->cmd = cmd;
	pp->id = userid;
	pp->len = j;
	pp->ret = err;
	
	DEBUG_LOG("SEND ERR\t[%u %u %u %u %u]", userid, err, cmd, fd, j);
	net_send(fd, buf, j);
}

void send_result_to_cgi(work_data_t* awd)
{
	static char buf[256] = {0};
	int j = sizeof(protocol_t);
	PKG_H_UINT32(buf, awd->key, j);
	PKG_STR(buf, awd->lloccode, j, sizeof(awd->lloccode));
	PKG_H_UINT32(buf, awd->thumb_cnt, j);
	int lp;
	for (lp = 0; lp < awd->thumb_cnt; lp++) {
		PKG_H_UINT32(buf, awd->thumb_paras[lp].thumb_id, j);
		DEBUG_LOG("THUMBID%u\t[%u]", lp, awd->thumb_paras[lp].thumb_id);
	}
	init_proto_head(buf, awd->userid, awd->waitcmd, j);
	DEBUG_LOG("SEND RET\t[uid=%u cmd=%u fd=%u b_key=%u u_key=%u len=%u]",
		awd->userid, awd->waitcmd, awd->sockfd, awd->key, awd->unique_key, j);
	
	net_send(awd->sockfd, buf, j);
}

static char thumbserv_buffer[4 * 1024 * 1024] = {0};
int send_pic_to_thumbserv(work_data_t* awd)
{
	int j = sizeof(protocol_t);
	PKG_H_UINT32(thumbserv_buffer, awd->unique_key, j);
	PKG_H_UINT32(thumbserv_buffer, awd->key, j);
	PKG_H_UINT32(thumbserv_buffer, awd->thumb_cnt, j);
	int lp;
	for (lp = 0; lp < awd->thumb_cnt; lp++) {
		if (awd->thumb_paras[lp].thumb_data) {
			PKG_H_UINT32(thumbserv_buffer, awd->thumb_paras[lp].thumb_len, j);
			PKG_STR(thumbserv_buffer, awd->thumb_paras[lp].thumb_data, j, awd->thumb_paras[lp].thumb_len);
		}
	}
	init_proto_head(thumbserv_buffer, awd->userid, proto_ts_upload_thumb, j);
	return send_to_thumbserv(thumbserv_buffer, j, awd->key);
}

void debug_thumb_para(work_data_t* awd)
{
	ERROR_LOG("THUMB CNT\t[fd=%u th_cnt=%u]", awd->sockfd, awd->thumb_cnt);
	int lp;
	thumb_para_t* ptp;
	for (lp = 0; lp < awd->thumb_cnt; lp++) {
		ptp = &awd->thumb_paras[lp];
		ERROR_LOG("THUMB%u PARA\t[%u %u %u %u %u %u %u]", lp, ptp->clip_w, ptp->clip_h, 
			ptp->start_x, ptp->start_y, ptp->thumb_w, ptp->thumb_h, ptp->thumb_len);
	}
}

static void delete_work(work_data_t* awd, list_head_t* acur, int succ)
{
	close(awd->log_fd);
	if (succ) {
		unlink(awd->log_path);
	}
	upload_hash_remove_key(&awd->key);
	DEBUG_LOG("DELETE-HASH-SIZE\t[%u]", get_upload_hash_size());
	remove_thumb_picture(awd);
	do_del_conn(awd->sockfd);
	list_del(acur);
	gcnt--;
	free(awd);
}

static void need_remove_or_send(work_data_t* awd, list_head_t* acur) 
{
	if (awd->save_file_tag && awd->done_thumb_tag) {
		DEBUG_LOG("DONE UPLOAD\t[%u]", awd->key);
		delete_work(awd, acur, 1);
	} else if (awd->make_thumb_tag && !awd->send_thumb_tag) {
		write_log_file(awd, "Thumbs generated successfully, sending to Thumb Server");
		if (send_pic_to_thumbserv(awd) != -1) {
			write_log_file(awd, "Thumbs sent successfully");;
		}
		awd->send_thumb_tag = 1;
	} else if(get_now_tv()->tv_sec - awd->upload_time > 2 * 60) {
		ERROR_LOG("CANNOT DONE UPLOAD\t[%u %u %u %u %u %u %u %u %u]",awd->unique_key, 
			awd->key, awd->file_type, awd->branch_type, awd->imp_upload_time,
			awd->save_file_tag, awd->make_thumb_tag, awd->send_thumb_tag,
			awd->done_thumb_tag);
		debug_thumb_para(awd);
		send_error_to_cgi(awd->userid, awd->waitcmd, awd->sockfd, err_invalid_file);
		unlink(awd->path);
		delete_work(awd, acur, 0);
	}
}

int scan_all_work_file(void* owner, void* data)
{
	int cnt = 0;
	list_head_t* cur = NULL;
	list_head_t* next = NULL;
	work_data_t* lwd;
	q_enter_list_for_each_safe(cur, next, gqlist) {
		lwd = list_entry(cur, work_data_t, list);
		uint32_t uni_key = lwd->unique_key;
		DEBUG_LOG("SCAN WOKR[unique_key=%u cnt=%u]", uni_key, cnt);
		need_remove_or_send(lwd, cur);
		if (cnt++ > 100) {
			break;
		}
	}
	q_exit_list_for_each_safe(gqlist);

	ADD_TIMER_EVENT(&g_timer, scan_all_work_file, NULL, get_now_tv()->tv_sec + 1);
	return 0;
}


int upload_thumb_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	uint32_t uniq, thumb_cnt;
	int j = 0;
	CHECK_BODY_LEN_GE(pkglen, 8);
	UNPKG_H_UINT32(pkg, uniq, j);
	list_head_t* cur = NULL;
	list_head_t* next = NULL;
	work_data_t* lwd;
	DEBUG_LOG("upload thumb callback\t[%u]", uniq);
	int found = 0;
	q_enter_list_for_each_safe(cur, next, gqlist) {
		lwd = list_entry(cur, work_data_t, list);
		DEBUG_LOG("SEARCH WOKR[unique_key=%u uniq=%u gcnt=%u]", lwd->unique_key, uniq, gcnt);
		if (lwd->unique_key == uniq) {
			found = 1;
			break;
		}
		need_remove_or_send(lwd, cur);
	}
	if (!found) {
		q_exit_list_for_each_safe(gqlist);
		ERROR_RETURN(("cannot find work\t[%u %u %u]", userid, cmd, uniq), -1);
	}
	lwd->done_thumb_tag = 1;
	
	UNPKG_H_UINT32(pkg, thumb_cnt, j);				// cnt
	if (pkglen < 8 + thumb_cnt * 4){
		ERROR_LOG("invalid pkg\t[%u %u %u %u %u]", userid, lwd->waitcmd, lwd->unique_key, pkglen, thumb_cnt);
		send_error_to_cgi(userid, lwd->waitcmd, lwd->sockfd, err_system_fatal_err);
		goto err;
	}
	
	int lp;
	for (lp = 0; lp < thumb_cnt; lp++) {
		UNPKG_H_UINT32(pkg, lwd->thumb_paras[lp].thumb_id, j);
		if (!lwd->thumb_paras[lp].thumb_id) {
			send_error_to_cgi(userid, lwd->waitcmd, lwd->sockfd, err_album_too_much_thumbs);
			goto err;
		}
	}
	create_lloccode(lwd->lloccode, lwd->thumb_paras[0].thumb_id, lwd->thumb_paras[1].thumb_id,
		lwd->thumb_paras[2].thumb_id, lwd->thumb_paras[3].thumb_id);
	send_result_to_cgi(lwd);

	char log_str[MAX_LOG_LEN] = {0};
	snprintf(log_str, MAX_LOG_LEN, "Thumbs saved successfully: thumbid1=%u, thumbid2=%u, thumbid3=%u, thumbid4=%u", 
		lwd->thumb_paras[0].thumb_id, lwd->thumb_paras[1].thumb_id,
		lwd->thumb_paras[2].thumb_id, lwd->thumb_paras[3].thumb_id);
	write_log_file(lwd, log_str);
	if (lwd->save_file_tag && lwd->done_thumb_tag) {
		delete_work(lwd, cur, 1);
	}
	q_exit_list_for_each_safe(gqlist);
	return 0;
err:
	if (lwd->save_file_tag && lwd->done_thumb_tag) {
		delete_work(lwd, cur, 0);
	}
	q_exit_list_for_each_safe(gqlist);
	return 0;
}

