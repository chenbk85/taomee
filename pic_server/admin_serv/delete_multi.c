#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <iter_serv/net_if.h>
#include <iter_serv/net.h>

#include "util.h"
#include "proto.h"
#include "lloccode.h"
#include "fs_list.h"
#include "admin.h"

#include "delete_multi.h"

int g_fd = 0;
static GHashTable* del_multi_maps;

//------------------------------------------------------
static inline void
free_del_multi(void* adm)
{
	del_multi_t* p = adm;
	DEBUG_LOG("Free Del Multi: [fd=%u old_key=%u file_cnt=%u]", 
		p->fd, p->key_albumid, p->file_cnt);
	
	g_slice_free1(sizeof(del_multi_t), adm);
}

void init_del_multi_hash()
{
	del_multi_maps = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_del_multi);
}

void fini_del_multi_hash()
{
	g_hash_table_destroy(del_multi_maps);
}

int get_del_multi_hash_size()
{
	return g_hash_table_size(del_multi_maps);
}


int delete_multi_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	
	CHECK_BODY_LEN_GE(pkglen, 12);
	uint32_t key_albumid, del_album_flag;
	int file_cnt;
	int j = 0;
	UNPKG_H_UINT32(pkg, key_albumid, j);
	UNPKG_H_UINT32(pkg, del_album_flag, j);
	UNPKG_H_UINT32(pkg, file_cnt, j);
	DEBUG_LOG("DEL MUT\t[albumid=%u del_flag=%u file_cnt=%u]", key_albumid, del_album_flag, file_cnt);
	CHECK_BODY_LEN(pkglen, 12 + file_cnt * LLOCCODE_LEN);
	
	del_multi_t* lpdm = (del_multi_t*)g_slice_alloc0(sizeof(del_multi_t));
	if (!lpdm) {
		ERROR_RETURN(("no memory\t[%u %u %u]", userid, cmd, fd), -1);
	}
	lpdm->fd = fd;
	lpdm->userid = userid;
	lpdm->cmd = cmd;
	lpdm->del_album_flag = del_album_flag;
	lpdm->key_albumid = key_albumid;
	lpdm->file_cnt = (file_cnt > MAX_PIC_NUM_PER_ABLUM) ? MAX_PIC_NUM_PER_ABLUM : file_cnt;
	int lp;
	for (lp = 0; lp < lpdm->file_cnt; lp++) {
		del_file_info_t* lpdf = &lpdm->dfi[lp];
		UNPKG_STR(pkg, lpdf->lloccode, j, LLOCCODE_LEN);
		DEBUG_LOG("DEL MULTI\t[%u %u %u %s]", fd, userid, cmd, lpdf->lloccode);
		if (analyse_lloccode_ex(lpdf->lloccode, &lpdf->hostid, lpdf->thumb_id, 
			&lpdf->thumb_cnt, lpdf->file_path, &lpdf->path_len, &lpdf->f_type, 
			&lpdf->b_type) == -1) {
			ERROR_LOG("cannot delete file\t[%u %u %s]", userid, lpdm->key_albumid, lpdf->lloccode);
			g_slice_free1(sizeof(del_multi_t), lpdm);
			return -1;
		}
	}
		
	char buff[MAX_PIC_NUM_PER_ABLUM * 32];
	int k = sizeof(protocol_t);
	PKG_H_UINT32(buff, lpdm->fd, k);
	PKG_H_UINT32(buff, lpdm->key_albumid, k);
	if (lpdm->del_album_flag) {
		PKG_H_UINT32(buff, 1, k);  // tell thumb server, cmd from admin server
		init_proto_head(buff, userid, proto_ts_delete_album, k);
	} else {
		PKG_H_UINT32(buff, lpdm->file_cnt, k);
		for (lp = 0; lp < lpdm->file_cnt; lp++) {
			del_file_info_t* lpdf = &lpdm->dfi[lp];
			PKG_H_UINT32(buff, lpdf->thumb_cnt, k);
			int loop;
			for (loop = 0; loop < lpdf->thumb_cnt; loop++) {
				PKG_H_UINT32(buff, lpdf->thumb_id[loop], k);
			}
		}
		init_proto_head(buff, userid, proto_ts_delete_thumbs, k);
	}
	
	if (send_to_thumbserv(buff, k, lpdm->key_albumid) == -1) {
		send_err_to_php(lpdm->fd, lpdm->cmd, lpdm->userid, err_thumbserv_net_err);
		g_slice_free1(sizeof(del_multi_t), lpdm);
		return -1;
	}

	g_hash_table_replace(del_multi_maps, &(lpdm->fd), lpdm);
	return 0;
}

int delete_multi_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN(pkglen, 12);
	int user_fd, file_cnt;
	uint32_t key_albumid;
	int j = 0;
	UNPKG_H_UINT32(pkg, user_fd, j);
	UNPKG_H_UINT32(pkg, key_albumid, j);
	UNPKG_H_UINT32(pkg, file_cnt, j);
	DEBUG_LOG("DEL MULTI CALLBACK\t[%u %u %u %u %u]",fd, user_fd, key_albumid, file_cnt, pkglen);
	del_multi_t* lpdm = g_hash_table_lookup(del_multi_maps, &user_fd);
	if (!lpdm) {
		ERROR_RETURN(("not find fd\t[%u %u %u]", userid, cmd, user_fd), -1);
	}

	int loop;
	for (loop = 0; loop < lpdm->file_cnt; loop++) {
		del_file_info_t* ldfi = &lpdm->dfi[loop];
		char buff[256];
		int k = sizeof(protocol_t);
		PKG_STR(buff, ldfi->file_path, k, MAX_PATH_LEN);	
		init_proto_head(buff, userid, proto_fs_delete_file, k);
		send_to_fileserv(buff, k, ldfi->hostid);
		DEBUG_LOG("DEL FILE[%u %u %s]", ldfi->hostid, key_albumid, ldfi->file_path);
	}

	send_ok_to_php(lpdm->fd, lpdm->cmd, userid);
	do_del_conn(lpdm->fd);
	g_hash_table_remove(del_multi_maps, &lpdm->fd);
	return 0;
}

int delete_album_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	CHECK_BODY_LEN(pkglen, 8);
	int user_fd;
	uint32_t key_albumid;
	int j = 0;
	UNPKG_H_UINT32(pkg, user_fd, j);
	UNPKG_H_UINT32(pkg, key_albumid, j);
	DEBUG_LOG("DEL ALBUM CALLBACK\t[%u %u %u %u]",fd, user_fd, key_albumid, pkglen);
	del_multi_t* lpdm = g_hash_table_lookup(del_multi_maps, &user_fd);
	if (!lpdm) {
		ERROR_RETURN(("not find fd\t[%u %u %u]", userid, cmd, user_fd), -1);
	}

	int loop;
	for (loop = 0; loop < lpdm->file_cnt; loop++) {
		del_file_info_t* ldfi = &lpdm->dfi[loop];
		char buff[256];
		int k = sizeof(protocol_t);
		PKG_STR(buff, ldfi->file_path, k, MAX_PATH_LEN);	
		init_proto_head(buff, userid, proto_fs_delete_file, k);
		send_to_fileserv(buff, k, ldfi->hostid);
		DEBUG_LOG("DEL FILE[%u %u %s]", ldfi->hostid, key_albumid, ldfi->file_path);
	}

	char buff[64] = {0};
	snprintf(buff, 64, "%u\n", lpdm->key_albumid);
	DEBUG_LOG("RETRIEVE ALBUM ID [%u %u]", userid, lpdm->key_albumid);
	int len = strlen(buff);
	if(write(g_fd, buff, len) != len) {
		ERROR_LOG("retrieve key_albumid error [%u %u %s]", 
			userid, lpdm->key_albumid, strerror(errno));
	}

	send_ok_to_php(lpdm->fd, lpdm->cmd, userid);
	do_del_conn(lpdm->fd);
	g_hash_table_remove(del_multi_maps, &lpdm->fd);
	return 0;
}

