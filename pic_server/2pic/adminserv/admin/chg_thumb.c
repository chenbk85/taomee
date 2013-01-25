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
#include "chg_thumb.h"

static GHashTable* chg_thumb_maps;

static inline void
free_chg_thumb(void* chg)
{
	chg_thumb_t* p = chg;
	DEBUG_LOG("Free Chg Thumb: [fd=%u uid=%u]", p->fd, p->userid);
	g_slice_free1(sizeof(chg_thumb_t), chg);
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

int chg_thumb_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen)
{
	int need_len = 4 + LLOCCODE_LEN + 6 * 4;	
	if (pkglen != need_len) {
		send_err_to_php(fd, cmd, userid, err_invalid_para);
		ERROR_RETURN(("chg-thumb para err\t[%u %u %u]", userid, cmd, fd), -1);
	}

	
	if (g_hash_table_lookup(chg_thumb_maps, &userid)) {
		send_err_to_php(fd, cmd, userid, err_chg_logo_to_quickly);
		ERROR_RETURN(("chg thumb too quickly\t[fd=%u userid=%u cmd=%u]", fd, userid, cmd), -1);
	}
	
	chg_thumb_t* lct = (chg_thumb_t*)g_slice_alloc0(sizeof(chg_thumb_t));
	if (!lct) {
		send_err_to_php(fd, cmd, userid, err_adminserv_busy);
		ERROR_RETURN(("no memory\t[%u %u %u]", userid, cmd, fd), -1);
	}
	lct->fd = fd;
	lct->userid = userid;
	lct->cmd = cmd;

	int j = 0;
	UNPKG_H_UINT32(pkg, lct->hostid, j);

	DEBUG_LOG("CHG THUMB\t[uid=%u hostid=%u]", userid, lct->hostid);
	char buff[256] = {0};
	int k = sizeof(protocol_t);
	memcpy(buff + k, pkg + 4, pkglen - 4);
	k += pkglen - 4;
	init_proto_head(buff, userid, proto_fs_chg_logo, k);
	if (send_to_fileserv(buff, k, lct->hostid) == -1) {
		send_err_to_php(fd, cmd, userid, err_adminserv_busy);
		g_slice_free1(sizeof *lct, lct);
		return -1;
	}

	g_hash_table_replace(chg_thumb_maps, &(lct->userid), lct);
	return 0;
}

int chg_thumb_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen, int ret)
{
	chg_thumb_t* lct = g_hash_table_lookup(chg_thumb_maps, &userid);
	if (!lct) {
		ERROR_RETURN(("cannot find in chg-thumb-maps\t[%u %u %u]", userid, cmd, fd), -1);
	}

	DEBUG_LOG("CHG THUMB CALLBACK\t[%u %u]", userid, ret);
	if (ret) {
		send_err_to_php(lct->fd, lct->cmd, lct->userid, ret);
		do_del_conn(lct->fd);
		return 0;
	}
	if (pkglen != LLOCCODE_LEN) {
		send_err_to_php(lct->fd, lct->cmd, lct->userid, err_system_fatal_err);
		do_del_conn(lct->fd);
		return 0;

	}
	char buff[512];
	int j = sizeof(protocol_t);
	memcpy(buff + j, pkg, pkglen);
	j += pkglen;
	init_proto_head(buff, userid, lct->cmd, j);
	net_send(lct->fd, buff, j);

	do_del_conn(lct->fd);
	g_hash_table_remove(chg_thumb_maps, &lct->userid);
	return 0;
}


