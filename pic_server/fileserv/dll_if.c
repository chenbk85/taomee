
#include <iter_serv/net_if.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/log.h>

#include "libpt/qlist.h"
#include "proto.h"
#include "save_file.h"
#include "thumb.h"
#include "fs_list.h"
#include "change_thumb.h"

int g_thumbserv_fd = -1;

int init_service()
{
	DEBUG_LOG("INIT...");

	setup_timer();
	INIT_LIST_HEAD(&g_timer.timer_list);

	if (load_xmlconf(FILE_SERV_LIST, load_fslist_config) == -1
		|| load_xmlconf(THUMB_SERV_LIST, load_tslist_config) == -1
		|| get_fs_id(config_get_strval("bind_ip")) == -1) {
		return -1;
	}
	if (init_dir_index(config_get_strval("dir_index")) == -1) {
		return -1;
	}
	if (imp_init_dir_index(config_get_strval("imp_dir_index")) == -1) {
		return -1;
	}
	qlist_init(gqlist);
	init_upload_hash();
	ADD_TIMER_EVENT(&g_timer, scan_all_work_file, NULL, get_now_tv()->tv_sec + 1);
	init_chg_thumb_hash();
	connect_to_all_thumbserv();

	save_file_cnt = config_get_intval("save_file_cnt", 20);
	create_thumb_cnt = config_get_intval("create_thumb_cnt", 10);
	DEBUG_LOG("PTHREAD CNT\t[%u %u]", save_file_cnt, create_thumb_cnt);
	g_thread_init (NULL);
	pool1 = g_thread_pool_new ((GFunc)save_file, NULL, save_file_cnt, FALSE, NULL);
	pool2 = g_thread_pool_new ((GFunc)create_thumbs, NULL, create_thumb_cnt, FALSE, NULL);
	
	return 0;
}

int fini_service()
{
	DEBUG_LOG("FINI...");
	if (gcnt) {
		ERROR_RETURN(("%d works need to be done\t", gcnt), -1);
	}
	fini_upload_hash();
	fini_chg_thumb_hash();
	release_dir_index();
	release_imp_dir_index();
	return 0;
}

void proc_events()
{
	handle_timer();
}

int	get_pkg_len(int fd, const void* pkg, int pkglen)
{
	if (pkglen < 4)
		return 0;
	const protocol_t* pp = pkg;
	int len = pp->len;
	DEBUG_LOG("RECEIVE\t[%u prolen=%u pkglen=%u %u]", pp->id, len, pkglen, pp->cmd);
	if ((len > proto_max_len) || (len < (int)sizeof(protocol_t))) {
		ERROR_LOG("[c] invalid len=%d from fd=%d", len, fd);
		return -1;
	}
	return len;
}

int on_pkg_received(int sockfd, void* pkg, int pkglen)
{
	protocol_t* pp = pkg;
	uint16_t cmd = pp->cmd;
	if (pp->len != pkglen) {
		ERROR_RETURN(("error pkg len\t[%u %u]", pp->len, pkglen), -1);
	}
	DEBUG_LOG("RECEIVE DATA\t[%u %u %u %u]", pp->id, pp->len, cmd, pkglen);
	void* buf = pkg + (sizeof(protocol_t));
	int   len = pkglen - (sizeof(protocol_t));
	switch (cmd) {
	case proto_fs_cgi_upload_file:
		return upload_file_cmd(pp->id, cmd, sockfd, buf, len);
	case proto_fs_cgi_chg_thumb:
		return change_picture_thumbs_cmd(pp->id, cmd, sockfd, buf, len);
	case imp_proto_upload_file:
		return imp_upload_file(pp->id, cmd, sockfd, buf, len);
	case imp_proto_upload_logo:
		return imp_upload_logo(pp->id, cmd, sockfd, buf, len);
	case proto_ts_upload_thumb:
		return upload_thumb_callback(pp->id, cmd, sockfd, buf, len);
	case proto_ts_change_thumb:
		return change_picture_thumbs_callback(pp->id, cmd, sockfd, buf, len);
	case proto_fs_delete_file:
		return delete_file_cmd(pp->id, cmd, sockfd, buf, len);
	default:
		ERROR_LOG("invalid cmd\t[%u]", cmd);
		break;
	}
	return 0;
}

void on_conn_closed(int sockfd)
{
	check_thumbserv_fd(sockfd);
}

