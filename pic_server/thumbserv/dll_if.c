
#include <iter_serv/net.h>
#include <iter_serv/net_if.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/log.h>

#include "proto.h"
#include "thumb.h"

int init_service()
{
	DEBUG_LOG("INIT...");

	setup_timer();
	//INIT_LIST_HEAD(&tmr.timer_list);
	init_mmaps();
	return 0;
}

int fini_service()
{
	DEBUG_LOG("FINI...");
	return 0;
}

void proc_events()
{
	//handle_timer();
}

int	get_pkg_len(int fd, const void* pkg, int pkglen)
{
	if (pkglen < 4) {
		return 0;
	}

	const protocol_t* pp = pkg;

	int len = pp->len;
	//DEBUG_LOG("RECEIVE %u DATA", len);
	if ((len > proto_max_len) || (len < (int)sizeof(protocol_t))) {
		ERROR_LOG("[c] invalid len=%d from fd=%d", len, fd);
		return -1;
	}
	return len;
}

int on_pkg_received(int sockfd, void* pkg, int pkglen)
{
	protocol_t* pp = pkg;
	uint32_t userid = pp->id;
	int len = pp->len;
	uint16_t cmd = pp->cmd;
	if (len != pkglen) {
		ERROR_RETURN(("error pkg len\t[%u %u]", len, pkglen), -1);
	}
	//DEBUG_LOG("RECEIVE DATA\t[%u %u %u %u]", userid, len, cmd, pkglen);
	switch (cmd) {
	case proto_ts_upload_thumb:
		return upload_thumb_cmd(userid, cmd, sockfd, pkg + (sizeof(protocol_t)), pkglen - (sizeof(protocol_t)));
	//case proto_ts_cgi_get_thumbs:
		//return get_album_thumbs_cmd(userid, cmd, sockfd, pkg + (sizeof(protocol_t)), pkglen - (sizeof(protocol_t)));
	case proto_ts_cgi_get_thumb:
		get_thumb_cmd(userid, cmd, sockfd, pkg + (sizeof(protocol_t)), pkglen - (sizeof(protocol_t)));
		return -1;
	case proto_ts_delete_thumbs:
		return del_thumbs_cmd(userid, cmd, sockfd, pkg + (sizeof(protocol_t)), pkglen - (sizeof(protocol_t)));
	case proto_ts_delete_album:
		return del_album_cmd(userid, cmd, sockfd, pkg + (sizeof(protocol_t)), pkglen - (sizeof(protocol_t)));
	case proto_ts_change_thumb:
		return change_thumb_cmd(userid, cmd, sockfd, pkg + (sizeof(protocol_t)), pkglen - (sizeof(protocol_t)));
	case proto_ts_trans_multi_get_thumbs:
		return trans_multi_get_thumbs_cmd(userid, cmd, sockfd, pkg + (sizeof(protocol_t)), pkglen - (sizeof(protocol_t)));
	case proto_ts_trans_multi_add_thumbs:
		return trans_multi_add_thumbs_cmd(userid, cmd, sockfd, pkg + (sizeof(protocol_t)), pkglen - (sizeof(protocol_t)));
	case proto_ts_trans_multi_del_thumbs:
		return trans_multi_del_thumbs_cmd(userid, cmd, sockfd, pkg + (sizeof(protocol_t)), pkglen - (sizeof(protocol_t)));
	default:
		ERROR_LOG("invalid cmd\t[%u]", cmd);
		break;
	}
	return -1;
}

