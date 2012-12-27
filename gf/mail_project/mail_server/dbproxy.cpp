#include <cassert>
#include <cstring>

extern "C" 
{
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>
#include <async_serv/net_if.h>
}
#include "online.hpp"
#include "dbproxy.hpp"

int proxysvr_fd = -1;

static uint8_t dbbuf[pkg_size];

typedef int (*dbproto_hdlr_t)(online_info_t* info, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

struct dbproto_handle_t
{
	dbproto_handle_t(dbproto_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
	{ 
		hdlr = h; len = l; cmp_method = cmp; 
	}
	dbproto_hdlr_t      hdlr;
	uint32_t            len;
	uint8_t             cmp_method;
};

#define SET_DB_HANDLE(op_, len_, cmp_) \
	do { \
		if (db_handles[dbproto_ ## op_].hdlr != 0) { \
			ERROR_LOG("duplicate cmd=%u name=%s", dbproto_ ## op_, #op_); \
			return false; \
		} \
		db_handles[dbproto_ ## op_] = dbproto_handle_t(db_ ## op_ ## _callback, len_, cmp_); \
	} while (0)


static dbproto_handle_t db_handles[65536];


int send_request_to_db(int online_fd, uint32_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, int body_len)
{
	assert(body_len >= 0);

	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("dbproxy_name"), 0, 65535, 1);
	}

	uint32_t len = sizeof(db_proto_t) + body_len;
	if ((proxysvr_fd == -1) || 	(len > sizeof(dbbuf))) 
	{
		ERROR_LOG("send to dbproxy failed: fd=%d len=%d", proxysvr_fd, len);
		return -1;
	}

	db_proto_t* pkg = reinterpret_cast<db_proto_t*>(dbbuf);
	pkg->len = len;
	pkg->seq = online_fd;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	pkg->role_tm  = role_tm;
	memcpy(pkg->body, body_buf, body_len);

	return net_send(proxysvr_fd, dbbuf, len);
}


void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen)
{
	if (!dbpkg->seq) return;
	int online_fd = dbpkg->seq;
	online_info_t* info = Online::get_online_info_by_fd(online_fd);
	if(info == NULL)return;
	
	uint32_t bodylen = dbpkg->len - sizeof(db_proto_t);
	int err = -1;

	if( db_handles[dbpkg->cmd].cmp_method == cmp_must_eq && bodylen != db_handles[dbpkg->cmd].len)
	{
		return ;	
	}

	if( db_handles[dbpkg->cmd].cmp_method == cmp_must_ge && bodylen < db_handles[dbpkg->cmd].len )
	{
		return ;	
	}

	if( db_handles[dbpkg->cmd].hdlr == NULL)
	{
		return ;	
	}

	err = db_handles[dbpkg->cmd].hdlr(info, dbpkg->id, dbpkg->body, bodylen, dbpkg->ret);
}


bool init_db_proto_handles()
{
	memset(db_handles, 0, sizeof(db_handles));	
	SET_DB_HANDLE(mail_head_list,  sizeof(mail_head_list_rsp_t), cmp_must_ge);	
	SET_DB_HANDLE(mail_body, sizeof(mail_body_rsp_t), cmp_must_eq);
	return true;
}

