#include <cassert>
#include <cstring>

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>

#include <async_serv/net_if.h>
}

#include "dbproxy.hpp"
#include "online.hpp"

/**
  * @brief compare methods
  */
enum {
	/*! len must equal to another given len */
	cmp_must_eq		= 1,
	/*! len must be greater or equal to another given len */
	cmp_must_ge		= 2,
};


/**
  * @brief data type for handlers that handle protocol packages from dbproxy
  */
typedef int (*dbproto_func_t)(userid_t id, uint8_t* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief data type for handles that handle protocol packages from dbproxy
  */
typedef struct dbproto_handle {
	dbproto_func_t		func;
	uint32_t			len;
	uint8_t				cmp_method;
}dbproto_handle_t;

/*! save handles for handling protocol packages from dbproxy */
dbproto_handle_t db_handles[65536];

//-----------------------------------------------------------------
/**
  * @brief set dbproxy protocol handle  
  */
#define SET_DB_HANDLE(op_, func_, len_, cmp_) \
		do { \
			if (db_handles[op_].func != 0) { \
				ERROR_RETURN(("duplicate cmd=%u", op_), -1); \
			} \
			db_handles[op_].func = func_; \
			db_handles[op_].len = len_; \
			db_handles[op_].cmp_method = cmp_; \
		} while (0)

int db_get_val_callback(userid_t id, uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	uint32_t* pval = (uint32_t *)body;
	if (ret || bodylen != pval[0] * 8 + 4) {
		ERROR_LOG("db err\t[%u %u %u %u]", id, ret, bodylen, pval[0]);
		return 0;
	}

	Online::s_beast_cnt_ = 0;

	for (uint32_t loop = 0; loop < pval[0]; loop ++) {
		if (pval[loop * 2 + 1] == beast_val_id) {
			Online::s_beast_cnt_ = pval[loop * 2 + 2];
			break;
		}
	}

	DEBUG_LOG("BEAST CNT\t[%u]", Online::s_beast_cnt_);

	Online::broadcast_beast_count();
	
	return 0;
}

/**
  * @brief init handles to handle protocol packages from dbproxy
  * @return true if all handles are inited successfully, false otherwise
  */
int init_db_proto_handles()
{
	/* operation, bodylen, compare method */
	memset(db_handles, 0, sizeof(db_handles));
	SET_DB_HANDLE(dbcmd_get_val, db_get_val_callback, 4, cmp_must_ge);

	return 0;
}


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
	pkg->seq = (id > begin_user_id) ? 0 : id;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	memcpy(pkg->body, body_buf, body_len);

	DEBUG_LOG("SEND DB CMD\t[%u %u %u]", pkg->id, pkg->cmd, pkg->len);

	return net_send(proxysvr_fd, dbbuf, len);
}

void handle_db_return(svr_proto_t* dbpkg, uint32_t pkglen)
{
	if (!dbpkg->seq) {
		return;
	}

	if (!db_handles[dbpkg->cmd].func) {
		KERROR_LOG(0, "db_handle is NULL: cmd=%d", dbpkg->cmd);
		return;
	}

	uint32_t bodylen = dbpkg->len - sizeof(svr_proto_t);

	if ( ((db_handles[dbpkg->cmd].cmp_method == cmp_must_eq) && (bodylen != db_handles[dbpkg->cmd].len))
			|| ((db_handles[dbpkg->cmd].cmp_method == cmp_must_ge) && (bodylen < db_handles[dbpkg->cmd].len)) ) {
		if ((bodylen != 0) || (dbpkg->ret == 0)) {
			KERROR_LOG(0, "invalid package len=%u needlen=%u cmd=0x%X cmpmethod=%d ret=%u",
						bodylen, db_handles[dbpkg->cmd].len, dbpkg->cmd, 
						db_handles[dbpkg->cmd].cmp_method, dbpkg->ret);
			return;
		}
	}

	db_handles[dbpkg->cmd].func(dbpkg->id, dbpkg->body, bodylen, dbpkg->ret);
}


