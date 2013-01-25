
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

int do_create_thumb(uint32_t userid, gdImagePtr src_im, thumb_para_t* pth, int b_type, int file_len)
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
		to_w = thumb_w = ( tmp ? tmp : 2);
	} else if (h_rate > w_rate) {
		int tmp = src_h * thumb_w / src_w;
		to_h = thumb_h = ( tmp ? tmp : 2);
	}
	
	gdImagePtr dst_im = gdImageCreateTrueColor(thumb_w, thumb_h);
	if (!dst_im) {
		ERROR_LOG("no memory\t[%u %u %u]", userid, thumb_w, thumb_h);
		return -1;
	}
	int bg_color = gdImageColorAllocate(dst_im, 255, 255, 255);
	gdImageFilledRectangle(dst_im, 0, 0, thumb_w, thumb_h, bg_color);
	gdImageCopyResampled(dst_im, src_im, dst_start_x, dst_start_y, start_x, start_y, to_w, to_h, src_w, src_h);
	uint64_t revise = 1;
	if (b_type == JPG) {
		revise = revise * 100 * src_im->sx * src_im->sy * 386204;
		revise = revise / 800 / 600 / file_len;
		DEBUG_LOG("REVISE\t[%u %u %u %u]", revise, src_im->sx, src_im->sy, file_len);
		revise = (revise < 10) ? 10 : revise;
	}
	pth->thumb_data = create_picture(dst_im, b_type, &pth->thumb_len, revise);
	if (!pth->thumb_data) {
		ERROR_LOG("no memory\t[%u]", userid);
		gdImageDestroy(dst_im);
		return -1;
	}
	DEBUG_LOG("THUMB DATA\t[len=%u, data=%p]", pth->thumb_len, pth->thumb_data);
	gdImageDestroy(dst_im);
	return 0;
}

int do_save_thumb(uint32_t userid, thumb_para_t* atp)
{
	char* dir_name = file_get_dir(atp->path);
	if (!file_exist(dir_name)) {
		file_make_dir(dir_name, DEF_DIR_MODE, 1);
	}
	free(dir_name);

	int fd = open(atp->path, O_RDWR|O_CREAT, DEF_FILE_MODE);
    if(fd < 0) {
		ERROR_RETURN(("open file %s error:%s", atp->path, strerror(errno)), -1);
    }
	if (write(fd, atp->thumb_data, atp->thumb_len) == atp->thumb_len) {
		DEBUG_LOG("write file %s success\t[uid=%u len=%u]", 
			atp->path, userid, atp->thumb_len);
	} else {
		close(fd);
		ERROR_RETURN(("write file %s error\t[uid=%u]", atp->path, userid), -1);
	}
	close(fd);
	return 0;

}

void save_logo (gpointer data, gpointer user_data)
{
	work_data_t* lwd = (work_data_t*)data;
	
	static char log_str[MAX_LOG_LEN] = {0};
	if (do_save_file(lwd) != -1) {
		lwd->save_file_tag = 1;
		snprintf(log_str, MAX_LOG_LEN, "Save file to %s succ\n", lwd->path);
		write_log_file(lwd->log_fd, log_str);
	} else {
		snprintf(log_str, MAX_LOG_LEN, "Save file to %s fail\n", lwd->path);
		write_log_file(lwd->log_fd, log_str);
		return;
	}

	gdImagePtr src_im = create_image(lwd->branch_type, lwd->file_len, lwd->file_data);
	if (!src_im) {
		ERROR_LOG("cannot create image\t[%u %s]", lwd->userid, lwd->lloccode);
		return;
	}

	int loop;
	for (loop = 0; loop < lwd->thumb_cnt; loop++) {
		thumb_para_t* ltp = &lwd->thumb_paras[loop];
		if (do_create_thumb(lwd->userid, src_im, ltp, lwd->branch_type, lwd->file_len) == -1) {
			lwd->save_file_tag = err_cannot_create_image;
			goto err;
		}
		if (do_save_thumb(lwd->userid, ltp) == -1) {	
			snprintf(log_str, MAX_LOG_LEN, "Saving file to %s fail\n", ltp->path);
			write_log_file(lwd->log_fd, log_str);
			lwd->save_file_tag = err_cannot_save_file;
			goto err;
		} else {
			snprintf(log_str, MAX_LOG_LEN, "Saving file to %s succ\n", ltp->path);
			write_log_file(lwd->log_fd, log_str);
		}
	}
	lwd->save_file_tag = 1;
err:
	gdImageDestroy(src_im);
}

int limit_picture(work_data_t* lwd, char** limit_data, int* limit_len, int* limit_flag)
{
	if (lwd->w_limit <= 0 || lwd->h_limit <= 0 || lwd->branch_type == GIF) {
		return 0;
	}
	gdImagePtr src_im = create_image(lwd->branch_type, lwd->file_len, lwd->file_data);
	if (!src_im) {
		ERROR_RETURN(("cannot create image\t[%u %u]", lwd->userid, lwd->file_len), -1);
	}
	
	if (lwd->w_limit >= src_im->sx && lwd->h_limit >= src_im->sy
		|| src_im->sx == 0 || src_im->sy == 0) {
		gdImageDestroy(src_im);
		return 0;
	}
	DEBUG_LOG("LIMIT PICTURE\t[%u %u]", lwd->file_type, lwd->branch_type);
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
	int bg_color = gdImageColorAllocate(dst_im, 255, 255, 255);
	gdImageFilledRectangle(dst_im, 0, 0, lwd->w_limit, lwd->h_limit, bg_color);
	gdImageCopyResampled(dst_im, src_im, 0, 0, 0, 0, lwd->w_limit, lwd->h_limit, src_im->sx, src_im->sy);
	*limit_data = create_picture(dst_im, lwd->branch_type, limit_len, 100);
	gdImageDestroy(src_im);
	gdImageDestroy(dst_im);
	if (!(*limit_data)) {
		ERROR_RETURN(("cannot create picture when limit picture\t[%u]", lwd->userid), -1);
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
			gdFree(ltp->thumb_data);
			ltp->thumb_data = NULL;
			ltp->thumb_len = 0;
		}
	}
}

static void send_up_file_rst_to_cgi(work_data_t* awd)
{
	static char buf[256] = {0};
	int j = sizeof(protocol_t);
	PKG_H_UINT32(buf, get_fs_id(config_get_strval("bind_ip")), j);
	PKG_H_UINT32(buf, awd->b_key, j);
	PKG_H_UINT32(buf, awd->photoid, j);
	PKG_STR(buf, awd->lloccode, j, LLOCCODE_LEN);
	PKG_H_UINT32(buf, awd->file_len, j);
	
	init_proto_head(buf, awd->userid, awd->waitcmd, j);
	DEBUG_LOG("UPLOAD RST\t[uid=%u host=%u b_key=%u photo=%u llocc=%s]",
		awd->userid, *(uint32_t*)(buf + sizeof(protocol_t)), 
		awd->b_key, awd->photoid, awd->lloccode);
		
	net_send(awd->sockfd, buf, j);
}

static void send_up_logo_rst_to_cgi(work_data_t* awd)
{
	static char buf[256] = {0};
	int j = sizeof(protocol_t);
	PKG_H_UINT32(buf, get_fs_id(config_get_strval("bind_ip")), j);
	PKG_STR(buf, awd->lloccode, j, LLOCCODE_LEN);
	PKG_H_UINT32(buf, awd->file_len, j);
	PKG_H_UINT32(buf, awd->thumb_cnt, j);
	int loop;
	for (loop = 0; loop < awd->thumb_cnt; loop++) {
		PKG_STR(buf, awd->thumb_paras[loop].lloccode, j, LLOCCODE_LEN);
	}
	init_proto_head(buf, awd->userid, awd->waitcmd, j);
	DEBUG_LOG("UPLOAD RST\t[uid=%u host=%u llocc=%s]",
		awd->userid, *(uint32_t*)(buf + sizeof(protocol_t)), awd->lloccode);
	
	DEBUG_LOG("SEND RST\t[uid=%u cmd=%u fd=%u len=%u]",
		awd->userid, awd->waitcmd, awd->sockfd, j);
	
	net_send(awd->sockfd, buf, j);
}

void send_result_to_cgi(work_data_t* awd)
{
	if (awd->waitcmd == proto_fs_cgi_upload_file) {
		send_up_file_rst_to_cgi(awd);
	} else if (awd->waitcmd == proto_fs_cgi_upload_logo) {
		send_up_logo_rst_to_cgi(awd);
	}
}

static void delete_upload_work(work_data_t* awd, list_head_t* acur)
{
	close(awd->log_fd);
	unlink(awd->log_path);
	upload_hash_remove_key(&awd->userid);
	DEBUG_LOG("DEL HASH-SIZE\t[%u]", get_upload_hash_size());
	do_del_conn(awd->sockfd);
	remove_thumb_picture(awd);
	list_del(acur);
	gcnt--;
	free(awd);
}

static int post_llocc(work_data_t* awd)
{
#ifdef USE_WEB_SERV
	char* url = "GET /?method=bus.album.addPhoto&";
	char* slast = "HTTP/1.1\r\nHost: melody.api.taomee.com\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.10) Gecko/20100914 Firefox/3.6.10\r\nAccept: text/html,application/xhtml+xml, application/xml;q=0.9,*/*q=0.8\r\nAccept-Language: zh-cn,zh;q=0.5\r\nAccept-Charset: GB2312,utf-8;q=0.7,*;q=0.7\r\nKeep-Alive: 115\r\nConnection: keep-alive\r\n\r\n";
	
	static char buf[2048] = {0};
	int tmp_len = strlen(url);
	memcpy(buf, url, tmp_len + 1);
	char* itr = buf + tmp_len;
	int len = snprintf(itr, 2048 - tmp_len, "albumid=%u&uid=%u&hostid=%u&len=%u&lloc=%s&file_name=%s %s", 
		awd->b_key, awd->userid, get_fs_id(config_get_strval("bind_ip")), 
		get_file_len(awd->path), awd->lloccode, awd->file_name, slast);
	
	DEBUG_LOG("POST LLOCC\t[uid=%u album=%u llocc=%s]", awd->userid, awd->b_key, awd->lloccode);
	return send_to_webserv(buf, len + tmp_len) ;
#else
	static char buff[2048];
	int j = sizeof(protocol_t);
	PKG_H_UINT32(buff, awd->b_key, j);
	PKG_H_UINT32(buff, get_fs_id(config_get_strval("bind_ip")), j);
	PKG_H_UINT32(buff, get_file_len(awd->path), j);
	PKG_STR(buff, awd->lloccode, j, LLOCCODE_LEN);
	PKG_STR(buff, awd->file_name, j, MAX_NAME_LEN);
	init_proto_head(buff, awd->userid, proto_wp_post_llocc, j);
	return send_to_webproxy(buff, j);
#endif
}

static void chk_upload_file(work_data_t* awd, list_head_t* acur) 
{
	if (awd->save_file_tag == 1) {
		if (awd->posted_llocc == 1) {
			DEBUG_LOG("DONE UPLOAD\t[%u %s]", awd->userid, awd->lloccode);
			send_result_to_cgi(awd);
			delete_upload_work(awd, acur);
			return;
		}
		if (awd->posted_llocc != 1 && awd->posted_llocc) {
			ERROR_LOG("upload file error\t[%u %u %u %u]", awd->userid,
				awd->file_type, awd->branch_type, awd->posted_llocc);
			send_error_to_cgi(awd->userid, awd->waitcmd, awd->sockfd, awd->posted_llocc);
			goto err;
		}
		if (!awd->sent_llocc) {
			DEBUG_LOG("sent lloc\t[%u %s]", awd->userid, awd->lloccode);
			if (post_llocc(awd) == 0) {
				awd->sent_llocc = 1;
			}
			return;
		}
	} 

	if(awd->save_file_tag != 1 && awd->save_file_tag) {
		ERROR_LOG("upload file error\t[%u %u %u %u]", awd->userid,
			awd->file_type, awd->branch_type, awd->save_file_tag);
		send_error_to_cgi(awd->userid, awd->waitcmd, awd->sockfd, awd->save_file_tag);
		goto err;
	} 

	if(get_now_tv()->tv_sec - awd->upload_time > 2 * 60) {
		ERROR_LOG("upload file timeout\t[%u %u %u %u]", awd->userid,
			awd->file_type, awd->branch_type, awd->save_file_tag);
		send_error_to_cgi(awd->userid, awd->waitcmd, awd->sockfd, err_system_fatal_err);
		goto err;
	}
	return;
err:
	unlink(awd->path);
	delete_upload_work(awd, acur);
		
}

static void scan_uploads()
{
	int cnt = 0;
	list_head_t* cur = NULL;
	list_head_t* next = NULL;
	work_data_t* lwd;
	q_enter_list_for_each_safe(cur, next, gqlist) {
		lwd = list_entry(cur, work_data_t, list);
		DEBUG_LOG("SCAN WOKR[uid=%u cnt=%u]", lwd->userid, cnt);
		chk_upload_file(lwd, cur);
		if (cnt++ > 100) {
			break;
		}
	}
	q_exit_list_for_each_safe(gqlist);
}

int scan_all_work_file(void* owner, void* data)
{
	scan_uploads();
	scan_chg_thumbs();
	ADD_TIMER_EVENT(&g_timer, scan_all_work_file, NULL, get_now_tv()->tv_sec + 1);
	return 0;
}


int post_llocc_callback(void* pkg, int pkglen)
{
	int ret_tag = 1;

	DEBUG_LOG("web server back %s", (char*)pkg);
	char* u_k = "\"uid\":";
	char* itr_uid = strstr(pkg, u_k);
	char* p_k = "\"photoid\":";
	char* itr_pid = strstr(pkg, p_k);
	if (!itr_uid || !itr_pid) {
		ret_tag = 0;
		return 0;
	}
	//if (itr_uid && !itr_pid)
	
	itr_uid += strlen(u_k);
	itr_pid += strlen(p_k);
	char* ct = strchr(itr_uid, '}');
    if (ct) *ct = '\0';
	ct = strchr(itr_uid, ',');
    if (ct) *ct = '\0';

	ct = strchr(itr_pid, '}');
	if (ct) *ct = '\0';
	ct = strchr(itr_pid, ',');
	if (ct) *ct = '\0';

	uint32_t userid = atoi(itr_uid);
	uint32_t photoid = atoi(itr_pid);
	
	DEBUG_LOG("POST BACK[uid=%u photo=%u]", userid, photoid);
	work_data_t* lwd = get_user_from_upload_maps(&userid);
	if (lwd == NULL) {
		ERROR_RETURN(("post back, cannot find user\t [userid=%u]", userid), -1);
	}
	lwd->photoid = photoid;
    lwd->posted_llocc = 1;
	return 0;
}

int post_llocc_webproxy_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen, int err)
{
	work_data_t* lwd = get_user_from_upload_maps(&userid);
	if (lwd == NULL) {
		ERROR_RETURN(("post back, cannot find user\t [userid=%u]", userid), -1);
	}
	if (err) {
		lwd->photoid = 0;
		lwd->posted_llocc = err;
		DEBUG_LOG("POST BACK ERR\t[uid=%u errno=%u]", userid, err);
		return 0;
	}
	
	CHECK_BODY_LEN(pkglen, 4);
	int j = 0;
	uint32_t photoid;
	UNPKG_H_UINT32(pkg, photoid, j);

	DEBUG_LOG("POST BACK\t[uid=%u photo=%u]", userid, photoid);
	lwd->photoid = photoid;
	lwd->posted_llocc = 1;
	return 0;
}

