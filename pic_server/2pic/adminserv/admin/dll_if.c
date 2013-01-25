
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
#include "chg_thumb.h"

int init_service()
{
	DEBUG_LOG("INIT...");
	if (load_xmlconf(FILE_SERV_LIST, load_fslist_config) == -1) {
		return -1;
	}
	connect_to_all_fileserv();
	init_del_log_mmap();
	init_chg_thumb_hash();

	return 0;
}

int fini_service()
{
	DEBUG_LOG("FINI...");
	release_del_log_mmap();
	return 0;
}

void proc_events()
{

}

int get_pkg_len(int fd, const void* pkg, int pkglen)
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
		case proto_as_delete_multi:
			return delete_multi_cmd(pp->id, cmd, sockfd, buf, len);
		case proto_as_chg_logo:
			return chg_thumb_cmd(pp->id, cmd, sockfd, buf, len);
			
		case proto_fs_chg_logo:
			return chg_thumb_callback(pp->id, cmd, sockfd, buf, len, pp->ret);
		default:
			ERROR_RETURN(("invalid proto\t[%u %u]", pp->id, pp->cmd), -1);
	}
}

void on_conn_closed(int sockfd)
{
	if (check_fileserv_fd(sockfd) == 1)
		return;
}

