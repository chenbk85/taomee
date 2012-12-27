#include <cstring>
#include <libtaomee++/inet/pdumanip.hpp>
#include <arpa/inet.h>

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>

#include <async_serv/net_if.h>
}

#include "battle.hpp"
#include "player.hpp"
#include "item.hpp"

#include "dbproxy.hpp"

/**
  * @brief data type for handlers that handle protocol packages from dbproxy
  */
typedef int (*dbproto_hdlr_t)(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief data type for handles that handle protocol packages from dbproxy
  */
struct dbproto_handle_t {
	dbproto_handle_t(dbproto_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
		{ hdlr = h; len = l; cmp_method = cmp; }

	dbproto_hdlr_t		hdlr;
	uint32_t			len;
	uint8_t				cmp_method;
};

/*! save handles for handling protocol packages from dbproxy */
static dbproto_handle_t db_handles[65536];

/*! for packing protocol data and send to dbproxy */
uint8_t dbpkgbuf[dbproto_max_len];

/*! udp socket address for db server */
static struct sockaddr_in dbsvr_udp_sa;

/*! dbproxy socket fd */
int proxysvr_fd = -1;

/*! dbsvr udp socket fd */
int dbsvr_udp_fd = -1;

//-----------------------------------------------------------------

/**
  * @brief set dbproxy protocol handle
  */
#define SET_DB_HANDLE(op_, len_, cmp_) \
		do { \
			if (db_handles[dbproto_ ## op_].hdlr != 0) { \
				ERROR_LOG("duplicate cmd=%u name=%s", dbproto_ ## op_, #op_); \
				return false; \
			} \
			db_handles[dbproto_ ## op_] = dbproto_handle_t(db_ ## op_ ## _callback, len_, cmp_); \
		} while (0)

//-----------------------------------------------------------------

/**
  * @brief send a request to db proxy
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_request_to_db(Player* p, userid_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, uint32_t body_len)
{
	static uint8_t dbbuf[dbproto_max_len];

	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("dbproxy_name"), 0, 65535, 1);
	}

	if ((proxysvr_fd == -1) || 	(body_len > (sizeof(dbbuf) - sizeof(db_proto_t)))) {
		ERROR_LOG("send to dbproxy failed: fd=%d len=%d", proxysvr_fd, body_len);
		if (p) {
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	db_proto_t* pkg = reinterpret_cast<db_proto_t*>(dbbuf);
	pkg->len = sizeof(db_proto_t) + body_len;
	pkg->seq = (p ? p->btl->id() : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	pkg->role_tm  = role_tm;
	memcpy(pkg->body, body_buf, body_len);

	if (p) {
		DEBUG_LOG("TO DB\t[uid=%u %u cmd=%u 0x%X]", p->id, id, p->waitcmd, cmd);
	}
	return net_send(proxysvr_fd, dbbuf, pkg->len);
}

/**
  * @brief handle package return from dbproxy
  * @param dbpkg package from dbproxy
  * @param pkglen length of dbpkg
  */
void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen)
{
	if (!dbpkg->seq) {
		return;
	}

	Player* p = get_player(dbpkg->id);
	if ((p == 0) || (p->btl == 0) || (p->btl->id() != dbpkg->seq)) {
		ERROR_LOG("battle ended already: uid=%u grpid=%u dbcmd=0x%X",
					dbpkg->id, dbpkg->seq, dbpkg->cmd);
		return;
	}

	DEBUG_LOG("DB R\t[uid=%u %u cmd=0x%X %u ret=%u]",
				p->id, dbpkg->id, dbpkg->cmd, p->waitcmd, dbpkg->ret);

	int err = -1;
	uint32_t bodylen = dbpkg->len - sizeof(db_proto_t);

	//db timeout
	//if (dbpkg->ret == 1017) {
	if (dbpkg->len == 18) {
		ERROR_LOG("dbpkg len error[%u %u %u %u %u]", dbpkg->len, dbpkg->seq, 
				dbpkg->cmd, dbpkg->ret, dbpkg->id);
		bodylen = 0;
	}

	if ( ((db_handles[dbpkg->cmd].cmp_method == cmp_must_eq) && (bodylen != db_handles[dbpkg->cmd].len))
			|| ((db_handles[dbpkg->cmd].cmp_method == cmp_must_ge) && (bodylen < db_handles[dbpkg->cmd].len)) ) {
		if ((bodylen != 0) || (dbpkg->ret == 0)) {
			ERROR_LOG("invalid package len=%u needlen=%u cmd=0x%X cmpmethod=%d ret=%u",
						bodylen, db_handles[dbpkg->cmd].len, dbpkg->cmd, 
						db_handles[dbpkg->cmd].cmp_method, dbpkg->ret);
			goto ret;
		}
	}

	err = db_handles[dbpkg->cmd].hdlr(p, dbpkg->id, dbpkg->body, bodylen, dbpkg->ret);

ret:
	if (err) {
		close_client_conn(p->fd);
	}
}

/**
  * @brief init handles to handle protocol packages from dbproxy
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_db_proto_handles()
{
	/* operation, bodylen, compare method */
	SET_DB_HANDLE(player_pick_item, sizeof(player_pick_item_rsp_t), cmp_must_eq);
	SET_DB_HANDLE(player_treasure_box, sizeof(db_treasure_box_rsp_t), cmp_must_ge);
	SET_DB_HANDLE(player_pick_roll_item, sizeof(player_pick_roll_item_rsp_t), cmp_must_eq);
	//SET_DB_HANDLE(pve_get_player_btl_info, 20, cmp_must_eq);

	return true;
}

/**
  * @brief init UDP socket for db server
  */
int	init_dbsvr_udp_socket()
{
	if (dbsvr_udp_fd != -1) {
		close(dbsvr_udp_fd);
	}
	bzero(&dbsvr_udp_sa, sizeof(dbsvr_udp_sa));
	dbsvr_udp_sa.sin_family = AF_INET;
	dbsvr_udp_sa.sin_port = htons(config_get_intval("dbsvr_udp_port", 0));
	if (inet_pton(AF_INET, config_get_strval("dbsvr_udp_ip"), &(dbsvr_udp_sa.sin_addr)) <= 0)
		return -1;
	dbsvr_udp_fd = socket(PF_INET, SOCK_DGRAM, 0);

	TRACE_LOG("init_dbsvr_udp_socket: fd[%d] addr[%s : %d]", dbsvr_udp_fd, 
		config_get_strval("dbsvr_udp_ip"), config_get_intval("dbsvr_udp_port", 0));
	return dbsvr_udp_fd;
}

/**
  * @brief send a UDP request to db server
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_udp_request_to_db(const Player* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
	static uint8_t dbbuf[dbproto_max_len];

	if (body_len > (sizeof(dbbuf) - sizeof(db_proto_t))) {
		ERROR_LOG("send to udp db failed: fd=%d len=%d", dbsvr_udp_fd, body_len);
		return -1;
	}

	db_proto_t* pkg = reinterpret_cast<db_proto_t*>(dbbuf);
	pkg->len = sizeof(db_proto_t) + body_len - 4;
	pkg->seq = 0;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	TRACE_LOG("SEND submit fd:%d, len:%u  body_len:%u", dbsvr_udp_fd, pkg->len, body_len);
	memcpy(&(pkg->role_tm), dbpkgbuf, body_len); // dbproxy proto head lenth diff

	if (sendto(dbsvr_udp_fd, dbbuf, pkg->len, 0, reinterpret_cast<const sockaddr*>(&dbsvr_udp_sa), sizeof(dbsvr_udp_sa)) < 0 ) {
		if (init_dbsvr_udp_socket() < 0) {
			ERROR_LOG("send submit failed and init udp socket to db server failed too");
			return -1;
		}
		if (sendto(dbsvr_udp_fd, dbbuf, pkg->len, 0, reinterpret_cast<const sockaddr*>(&dbsvr_udp_sa), sizeof(dbsvr_udp_sa)) < 0 ) {
			ERROR_LOG("send submit failed fd:%d, len:%u", dbsvr_udp_fd, pkg->len);
		}
	}
	return 0;
}
void report_add_to_monitor(const Player* p, uint32_t opt_type, 
    uint32_t exv2, uint32_t exv3, uint32_t exv4)
{
    int idx_udp = 0;
    uint8_t buf[dbproto_max_len] = {0};
    taomee::pack_h(buf, static_cast<uint32_t>(6), idx_udp);
    taomee::pack_h(buf, p->id, idx_udp);
    taomee::pack_h(buf, static_cast<uint32_t>(get_now_tv()->tv_sec), idx_udp);
    taomee::pack_h(buf, static_cast<uint32_t>(opt_type), idx_udp);
    taomee::pack_h(buf, static_cast<int32_t>(p->role_type), idx_udp);
    taomee::pack_h(buf, static_cast<int32_t>(exv2), idx_udp);
    taomee::pack_h(buf, static_cast<int32_t>(exv3), idx_udp);
    taomee::pack_h(buf, static_cast<int32_t>(exv4), idx_udp);
    TRACE_LOG("add item monitor:uid=%u, opt_type=%u, exv2=%u, exv3=%u, exv4=%u",
        p->id, opt_type, exv2, exv3, exv4);
    send_udp_request_to_db(p, p->id, 0xF135, buf, idx_udp);
}

