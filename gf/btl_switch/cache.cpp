/*
 * =====================================================================================
 *
 *       Filename:  cache.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2012 05:44:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
extern "C" {
#include <async_serv/net_if.h>
#include <libtaomee/log.h>
#include <string.h>
#include <libtaomee/conf_parser/config.h>
}

#include "cache.hpp"
#include "team_contest.hpp"

int cachesvr_fd = -1;

int send_request_to_cache_svr(uint32_t uid, uint32_t role_tm, uint32_t cmd, void * body, int bodylen)
{
	static uint8_t ch_pkg[8192];

	if (cachesvr_fd == -1) {
		cachesvr_fd = connect_to_svr(config_get_strval("cachesvr_ip"), config_get_intval("cachesvr_port", 0), 65535, 1);
	}

	if ((cachesvr_fd == -1) || ((uint32_t)bodylen > (8192 - sizeof(cachesvr_proto_t)))) {
		ERROR_LOG("send package to cache server failed: fd=%d bodylen=%d", cachesvr_fd, bodylen);
		return 0;
	}

	uint32_t len = sizeof(cachesvr_proto_t) + bodylen;

	//Build the package
	cachesvr_proto_t* pkg = reinterpret_cast<cachesvr_proto_t*>(ch_pkg);
	pkg->len = len;
	pkg->seq = 0; // set higher 16 bytes fd and lower 16 bytes waitcmd
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->uid = uid;
	pkg->role_tm = role_tm;
	memcpy(pkg->body, body, bodylen);

	int ret = net_send(cachesvr_fd, ch_pkg, len);
	if (ret) {
		ERROR_LOG("SEND TO CACHESVR FAIL %d %d %d", ret, cachesvr_fd, len); 
		cachesvr_fd = -1;
	}
	return ret;
}


void handle_cachesvr_callback(void * data, int len)
{
	cachesvr_proto_t * pkg = reinterpret_cast<cachesvr_proto_t*>(data);
	switch (pkg->cmd) {
		case cache_get_team_server_info:
			handle_server_info_form_cache_svr(pkg);
			break;
		default:
			break;
	}	
}

