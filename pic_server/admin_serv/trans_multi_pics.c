#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <iter_serv/net_if.h>
#include <iter_serv/net.h>

#include "util.h"
#include "proto.h"
#include "lloccode.h"
#include "fs_list.h"
#include "admin.h"
#include "trans_multi_pics.h"


static GHashTable* trans_multi_maps;

//------------------------------------------------------
static inline void
free_trans_multi(void* atm)
{
	trans_multi_t* p = atm;
	DEBUG_LOG("Free trans Multi: [fd=%u old_key=%u new_key=%u file_cnt=%u]", 
		p->fd, p->key_albumid, p->new_key_albumid, p->file_cnt);
	
	g_slice_free1(sizeof(trans_multi_t), atm);
}

void init_trans_multi_hash()
{
	trans_multi_maps = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_trans_multi);
}

void fini_trans_multi_hash()
{
	g_hash_table_destroy(trans_multi_maps);
}

int get_trans_multi_hash_size()
{
	return g_hash_table_size(trans_multi_maps);
}

int do_get_multi_thumbs(trans_multi_t* atm)
{
	uint8_t buff[1024];
	int k = sizeof(protocol_t);
	PKG_H_UINT32(buff, atm->fd, k);
	PKG_H_UINT32(buff, atm->key_albumid, k);
	PKG_H_UINT32(buff, atm->file_cnt, k);
	int loop;
	for (loop = 0; loop < atm->file_cnt; loop++) {
		uint32_t hostid, uid = 0;
		char type = 0, branchtype = 0;
		char date[9] = {0}, dir[4] = {0}, pic[4] = {0};
		file_info_t* lfi = &atm->fi[loop];
		
		sscanf(lfi->lloccode, "%2X%c%c%8s%3s%3s%10u%8X%8X%8X%8X",
				&hostid, &type, &branchtype, date, dir, pic, &uid, &(lfi->sth[0].thumb_id),
				&(lfi->sth[1].thumb_id), &(lfi->sth[2].thumb_id), &(lfi->sth[3].thumb_id));

		if (type != '1') { // only for picture now
			ERROR_RETURN(("invalid lloccode type\t[%u %u %s]", atm->userid, type, lfi->lloccode), -1);
		}

		int label_k = k; k += 4;
		int lp, thumb_cnt = 0;
		for(lp = 0; lp < MAX_THUMB_CNT_PER_PIC; lp++){
			if (lfi->sth[lp].thumb_id) {
				PKG_H_UINT32(buff, lfi->sth[lp].thumb_id, k);
				thumb_cnt++;
			}
		}
		PKG_H_UINT32(buff, thumb_cnt, label_k);
		lfi->thumb_cnt = thumb_cnt;
		DEBUG_LOG("GET M THUMB[%u %s %u %u %u %u %u]", atm->key_albumid, lfi->lloccode, lfi->thumb_cnt,
			lfi->sth[0].thumb_id, lfi->sth[1].thumb_id, lfi->sth[2].thumb_id, lfi->sth[3].thumb_id);
	}
	
	init_proto_head(buff, atm->userid, proto_ts_trans_multi_get_thumbs, k);
	if (send_to_thumbserv(buff, k, atm->key_albumid) == -1) {
		send_err_to_php(atm->fd, atm->cmd, atm->userid, err_thumbserv_net_err);
		return -1;
	}
	return 0;
}

int trans_multi_file_get(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN_GE(pkglen, 12);
	trans_multi_t* lptm = (trans_multi_t*)g_slice_alloc0(sizeof(trans_multi_t));
	if (!lptm) {
		ERROR_RETURN(("no memory\t[%u %u %u]", userid, cmd, fd), -1);
	}
	lptm->fd = fd;
	lptm->userid = userid;
	lptm->cmd = cmd;
	int j = 0;
	UNPKG_H_UINT32(pkg, lptm->key_albumid, j);
	UNPKG_H_UINT32(pkg, lptm->new_key_albumid, j);
	UNPKG_H_UINT32(pkg, lptm->file_cnt, j);
	if (lptm->file_cnt > MAX__ONCE_TRANS_NUM) {
		send_err_to_php(lptm->fd, lptm->cmd, lptm->userid, err_trans_too_much_file);
		ERROR_LOG("trans multi too much files\t[%u %u %u %u]", userid, 
			lptm->key_albumid, lptm->new_key_albumid, lptm->file_cnt);
		g_slice_free1(sizeof(trans_multi_t), lptm);
		return -1;
	}
	CHECK_BODY_LEN(pkglen, 12 + lptm->file_cnt * LLOCCODE_LEN);
	int lp;
	for (lp = 0; lp < lptm->file_cnt; lp++) {
		UNPKG_STR(pkg, lptm->fi[lp].lloccode, j, LLOCCODE_LEN);
	}

	if (do_get_multi_thumbs(lptm) == -1) {
		send_err_to_php(lptm->fd, lptm->cmd, lptm->userid, err_file_not_pic);
		ERROR_LOG("cannot get multi files\t[%u %u %u]", userid, lptm->key_albumid, lptm->new_key_albumid);
		g_slice_free1(sizeof(trans_multi_t), lptm);
		return -1;
	}

	g_hash_table_replace(trans_multi_maps, &(lptm->fd), lptm);
	return 0;
}

#define MULTI_THUMB_BUFF_LEN MAX_THUMB_SIZE * MAX_THUMB_CNT_PER_PIC * MAX_TRANS_NUM
int trans_multi_file_add(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen) 
{
	CHECK_BODY_LEN_GE(pkglen, 12);

	int			user_fd, old_key_album, file_cnt;
	int 		j = 0;
	UNPKG_H_UINT32(pkg, user_fd, j);
	UNPKG_H_UINT32(pkg, old_key_album, j);
	UNPKG_H_UINT32(pkg, file_cnt, j);
	trans_multi_t* lptm = g_hash_table_lookup(trans_multi_maps, &user_fd);
	if (!lptm) {
		ERROR_RETURN(("not find fd\t[%u %u %u]", userid, cmd, user_fd), -1);
	}
	
	uint8_t* multi_thumbs_buff = (uint8_t*)g_slice_alloc0(MULTI_THUMB_BUFF_LEN);
	if (!multi_thumbs_buff) {
		send_err_to_php(lptm->fd, lptm->cmd, lptm->userid, err_system_fatal_err);
		do_del_conn(lptm->fd);
		g_hash_table_remove(trans_multi_maps, &lptm->fd);
		ERROR_RETURN(("not enough memory\t[%u %u]", userid, cmd), -1);
	}
	if (file_cnt != lptm->file_cnt || userid != lptm->userid || old_key_album != lptm->key_albumid) {
		ERROR_LOG("not consist\t[ad_old_key=%u th_ok=%u ad_file_cnt=%u th_fc=%u ad_user=%u th_user=%u]", 
			lptm->key_albumid, old_key_album, lptm->file_cnt, file_cnt, lptm->userid, userid);
		goto err;
	}
	
	DEBUG_LOG("TRANSFER TO\t[%u %u]", old_key_album, lptm->new_key_albumid);
	int k = sizeof(protocol_t);
	PKG_H_UINT32(multi_thumbs_buff, user_fd, k);
	PKG_H_UINT32(multi_thumbs_buff, lptm->new_key_albumid, k);
	int label_file_k = k; k += 4;
	//PKG_H_UINT32(multi_thumbs_buff, file_cnt, k);

	int loop;
	for (loop = 0; loop < lptm->file_cnt; loop++) {
		file_info_t* lfi = &lptm->fi[loop];
		CHECK_BODY_LEN_GE(pkglen, j + 4);
		int thumb_cnt = 0;
		UNPKG_H_UINT32(pkg, thumb_cnt, j);
		if (thumb_cnt != lfi->thumb_cnt) {
			ERROR_LOG("thumb_cnt err\t[userid=%u old_key=%u ad_thumb_cnt=%u th_tc=%u]", 
				userid, old_key_album, lfi->thumb_cnt, thumb_cnt);
			send_err_to_php(lptm->fd, lptm->cmd, lptm->userid, err_system_fatal_err);
			goto err;
		}
		int label_thumb_cnt_k = k; k += 4;
		//PKG_H_UINT32(multi_thumbs_buff, thumb_cnt, k);
		int lp;
		for (lp = 0; lp < lfi->thumb_cnt; lp++) {
			uint32_t  thumb_id = 0, thumb_len = 0;
			CHECK_BODY_LEN_GE(pkglen, j + 4);
			UNPKG_H_UINT32(pkg, thumb_id, j);
			if (thumb_id != lfi->sth[lp].thumb_id) {
				ERROR_LOG("thumb_id err\t[userid=%u old_key=%u idx=%u ad_thumb_id=%u th_tc=%u]", 
					userid, old_key_album, lp, lfi->sth[lp].thumb_id, thumb_id);
				send_err_to_php(lptm->fd, lptm->cmd, lptm->userid, err_system_fatal_err);
				goto err;
			}
			CHECK_BODY_LEN_GE(pkglen, j + 4);
			UNPKG_H_UINT32(pkg, thumb_len, j);
			if (lfi->ret == err_not_find_thumb) {
				CHECK_BODY_LEN_GE(pkglen, j + thumb_len);
				j += thumb_len;
				continue;
			}
				
			if (!thumb_len) {
				lfi->ret = err_not_find_thumb;
			} else {
				DEBUG_LOG("TRAN ADD\t[id=%u len=%u]", thumb_id, thumb_len);
				PKG_H_UINT32(multi_thumbs_buff, thumb_id, k);
				PKG_H_UINT32(multi_thumbs_buff, thumb_len, k);
				CHECK_BODY_LEN_GE(pkglen, j + thumb_len);
				UNPKG_STR(pkg, multi_thumbs_buff + k, j, thumb_len);
				k += thumb_len;
			}
		}
		if (!lfi->ret) {
			PKG_H_UINT32(multi_thumbs_buff, thumb_cnt, label_thumb_cnt_k);
		} else {
			k = label_thumb_cnt_k; file_cnt--;
		}
	}
	PKG_H_UINT32(multi_thumbs_buff, file_cnt, label_file_k);

	init_proto_head(multi_thumbs_buff, userid, proto_ts_trans_multi_add_thumbs, k);
	if (send_to_thumbserv(multi_thumbs_buff, k, lptm->new_key_albumid) == -1) {
		send_err_to_php(lptm->fd, lptm->cmd, userid, err_thumbserv_net_err);
		goto err;
	}
	g_slice_free1(MULTI_THUMB_BUFF_LEN, multi_thumbs_buff);
	return 0;
err:
	do_del_conn(lptm->fd);
	g_slice_free1(MULTI_THUMB_BUFF_LEN, multi_thumbs_buff);
	g_hash_table_remove(trans_multi_maps, &lptm->fd);
	return -1;
}

void send_trans_multi_result_to_php(trans_multi_t* atm)
{
	char buff[10240];
	int j = sizeof(protocol_t);
	//PKG_H_UINT32(buff, atm->key_albumid, j);
	PKG_H_UINT32(buff, atm->new_key_albumid, j);
	PKG_H_UINT32(buff, atm->file_cnt, j);
	int loop;
	for (loop = 0; loop < atm->file_cnt; loop++) {
		file_info_t* lfi = &atm->fi[loop];
		if (!lfi->ret) {
			create_lloccode(lfi->lloccode, lfi->sth[0].new_thumb_id, lfi->sth[1].new_thumb_id, 
				lfi->sth[2].new_thumb_id, lfi->sth[3].new_thumb_id);
		}
		PKG_STR(buff, lfi->lloccode, j, LLOCCODE_LEN);
		PKG_H_UINT32(buff, lfi->ret, j);
		PKG_H_UINT32(buff, lfi->thumb_cnt, j);
		
		DEBUG_LOG("TRAN FILE\t[%u %u %s %u]", loop, lfi->ret, lfi->lloccode, lfi->ret);
		int lp;
		for (lp = 0; lp < lfi->thumb_cnt; lp++) {
			DEBUG_LOG("TRAN THUMB\t[%u %u %u]", lp, lfi->sth[lp].thumb_id, lfi->sth[lp].new_thumb_id);
			if (lfi->ret)
				PKG_H_UINT32(buff, lfi->sth[lp].thumb_id, j);
			else 
				PKG_H_UINT32(buff, lfi->sth[lp].new_thumb_id, j);
		}
	}
	DEBUG_LOG("TRANS MULTI OK\t[%u %u %u]", atm->userid, atm->new_key_albumid, atm->file_cnt);
	init_proto_head(buff, atm->userid, atm->cmd, j);
	net_send(atm->fd, buff, j);
}

static void debug_del_thumb_err(trans_multi_t* atm)
{
	int f_cnt = atm->file_cnt;
	int loop;
	for (loop = 0; loop < atm->file_cnt; loop++) {
		file_info_t* lfi = &atm->fi[loop];
		if (lfi->ret) {
			f_cnt--; continue;
		}
		int lp;
		for(lp = 0; lp < MAX_THUMB_CNT_PER_PIC && lp < lfi->thumb_cnt; lp++){
			ERROR_LOG("DEL THUMB ERR[key=%u thumb_id=%u]", 
				atm->key_albumid, lfi->sth[lp].thumb_id);
		}
	}
}

int do_del_multi_thumbs(trans_multi_t* atm)
{
	uint8_t buff[1024];
	int k = sizeof(protocol_t);
	PKG_H_UINT32(buff, atm->fd, k);
	PKG_H_UINT32(buff, atm->key_albumid, k);
	//PKG_H_UINT32(buff, atm->file_cnt, k);
	int f_cnt = atm->file_cnt;
	int label_file_cnt_k = k; k += 4;
	int loop;
	for (loop = 0; loop < atm->file_cnt; loop++) {
		file_info_t* lfi = &atm->fi[loop];
		if (lfi->ret) {
			f_cnt--; continue;
		}

		PKG_H_UINT32(buff, lfi->thumb_cnt, k);
		int lp;
		for(lp = 0; lp < MAX_THUMB_CNT_PER_PIC && lp < lfi->thumb_cnt; lp++){
			PKG_H_UINT32(buff, lfi->sth[lp].thumb_id, k);
		}
		DEBUG_LOG("DEL M THUMB[%u %s %u %u %u %u %u]", 
			atm->key_albumid, lfi->lloccode, lfi->thumb_cnt, lfi->sth[0].thumb_id,
			lfi->sth[1].thumb_id, lfi->sth[2].thumb_id, lfi->sth[3].thumb_id);
	}
	PKG_H_UINT32(buff, f_cnt, label_file_cnt_k);
	init_proto_head(buff, atm->userid, proto_ts_trans_multi_del_thumbs, k);
	if (send_to_thumbserv(buff, k, atm->key_albumid) == -1) {
		debug_del_thumb_err(atm);
		send_err_to_php(atm->fd, atm->cmd, atm->userid, err_thumbserv_net_err);
		return -1;
	}
	return 0;
}

int trans_multi_file_del(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen) 
{
	CHECK_BODY_LEN_GE(pkglen, 12);
	int user_fd, file_cnt;
	uint32_t new_key_albumid;
	int j = 0;
	UNPKG_H_UINT32(pkg, user_fd, j);
	UNPKG_H_UINT32(pkg, new_key_albumid, j);
	UNPKG_H_UINT32(pkg, file_cnt, j);
	trans_multi_t* lptm = g_hash_table_lookup(trans_multi_maps, &user_fd);
	if (!lptm) {
		ERROR_RETURN(("not find fd\t[%u %u %u]", userid, cmd, user_fd), -1);
	}
	if (new_key_albumid != lptm->new_key_albumid) {
		ERROR_LOG("new_key_album not consist\t[uid=%u ad_new_key_album=%u th_nka=%u]", 
			userid, new_key_albumid, lptm->new_key_albumid);
		send_err_to_php(lptm->fd, lptm->cmd, lptm->userid, err_system_fatal_err);
		goto err;
	}
	int loop;
	for (loop = 0; loop < lptm->file_cnt; loop++) {
		file_info_t* lfi = &lptm->fi[loop];	
		if (lfi->ret == err_not_find_thumb)
			continue;
		CHECK_BODY_LEN_GE(pkglen, j + 4);
		int thumb_cnt;
		UNPKG_H_UINT32(pkg, thumb_cnt, j);
		int lp;
		for (lp = 0; thumb_cnt && lp < lfi->thumb_cnt && lp < MAX_THUMB_CNT_PER_PIC; lp++) {
			CHECK_BODY_LEN_GE(pkglen, j + 4);
			UNPKG_H_UINT32(pkg, lfi->sth[lp].new_thumb_id, j);
			if (lfi->ret == err_album_full)
				continue;
			
			if (!lfi->sth[lp].new_thumb_id) { // album full
				lfi->ret = err_album_full;
			}
		}
	}

	if (do_del_multi_thumbs(lptm) == -1) {
		goto err;
	}
	send_trans_multi_result_to_php(lptm);
	
	do_del_conn(lptm->fd);
	g_hash_table_remove(trans_multi_maps, &lptm->fd);
	return 0;

err:	
	do_del_conn(lptm->fd);
	g_hash_table_remove(trans_multi_maps, &lptm->fd);
	return -1;
}

