#include <cassert>
#include <cstring>

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>

#include <async_serv/net_if.h>
}

#include "dbproxy.hpp"

int proxysvr_fd = -1;

int send_request_to_db(userid_t id, uint16_t cmd, const void* body_buf, int body_len)
{
	assert(body_len >= 0);

	static uint8_t dbbuf[pkg_size];

	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("dbproxy_ip"), 0, 65535, 1);
	}

	uint32_t len = sizeof(svr_proto_t) + body_len;
	if ((proxysvr_fd == -1) || 	(len > sizeof(dbbuf))) {
		ERROR_LOG("send to dbproxy failed: fd=%d len=%d", proxysvr_fd, len);
		return -1;
	}

	svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(dbbuf);
	pkg->len = len;
	pkg->seq = 0;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	memcpy(pkg->body, body_buf, body_len);

	return net_send(proxysvr_fd, dbbuf, len);
}

