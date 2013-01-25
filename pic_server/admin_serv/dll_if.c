
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <iter_serv/net_if.h>

#include "util.h"
#include "proto.h"
#include "fs_list.h"
#include "admin.h"
#include "delete_multi.h"
#include "trans_multi_pics.h"
#include "change_file_attr.h"

int init_service()
{
	DEBUG_LOG("INIT...");
	if (load_xmlconf(FILE_SERV_LIST, load_fslist_config) == -1
		|| load_xmlconf(THUMB_SERV_LIST, load_tslist_config) == -1) {
		return -1;
	}
	if ((g_fd = open(DEL_ALBUM_FILE, O_RDWR|O_APPEND|O_CREAT, S_IRWXU)) == -1) {
		ERROR_RETURN(("open file %s error", DEL_ALBUM_FILE), -1);
	}
	connect_to_all_fileserv();
	connect_to_all_thumbserv();

	init_del_multi_hash();
	init_trans_multi_hash();
	init_change_file_attr_hash();
	return 0;
}

int fini_service()
{
	DEBUG_LOG("FINI...");
	if (get_del_multi_hash_size() || get_trans_multi_hash_size())
		return -1;
	fini_del_multi_hash();
	fini_trans_multi_hash();
	fini_change_file_attr_hash();
	close(g_fd);
	return 0;
}

void proc_events()
{
}

int	get_pkg_len(int fd, const void* pkg, int pkglen)
{
	if (pkglen < 4) {
		return 0;
	}
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
		#if 0
		case proto_as_delete_file:
			return delete_file(pp->id, cmd, sockfd, buf, len);
		#endif
		case proto_as_delete_multi:
			return delete_multi_cmd(pp->id, cmd, sockfd, buf, len);
		case proto_ts_delete_thumbs:
			return delete_multi_callback(pp->id, cmd, sockfd, buf, len);
		case proto_ts_delete_album:
			return delete_album_callback(pp->id, cmd, sockfd, buf, len);
		#if 0
		case proto_admin_transfer_file:
			return transfer_file_del(pp->id, cmd, sockfd, buf, len);
		case proto_ts_transfer_thumbs:
			return transfer_file_add(pp->id, cmd, sockfd, buf, len);
		case proto_ts_cgi_upload_thumb:
			return transfer_file_add_callback(pp->id, cmd, sockfd, buf, len);
		#endif
		case proto_as_trans_multi_file:
			return trans_multi_file_get(pp->id, cmd, sockfd, buf, len);
		case proto_ts_trans_multi_get_thumbs:
			return trans_multi_file_add(pp->id, cmd, sockfd, buf, len);
		case proto_ts_trans_multi_add_thumbs:
			return trans_multi_file_del(pp->id, cmd, sockfd, buf, len);
		case proto_as_change_file_attr:
			return change_file_attr(pp->id, cmd, sockfd, buf, len);
		case proto_fs_change_file_attr:
			return change_file_attr_callback(pp->id, cmd, sockfd, buf, len);
		default:
			ERROR_RETURN(("invalid proto\t[%u %u]", pp->id, pp->cmd), -1);
	}
}

void on_conn_closed(int sockfd)
{
	if (check_thumbserv_fd(sockfd) == 1)
		return;
	if (check_fileserv_fd(sockfd) == 1)
		return;
}

