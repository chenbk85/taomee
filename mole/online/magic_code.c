#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <libtaomee/conf_parser/config.h>

#include "util.h"
#include "sprite.h"
#include "protoheaders.h"
#include "proto.h"
#include "dbproxy.h"

#include "magic_code.h"

typedef struct mc_item{
	uint32_t gift_id;
	uint16_t gift_count;
}__attribute__((packed))mc_item_t;

typedef struct mc_gifts{
	uint8_t gift_type;//1套装，0非套装
	uint16_t select_num;
	uint16_t used_num;
	uint16_t itm_num;
	mc_item_t itm[0];
}__attribute__((packed))mc_gifts_t;

typedef struct consume_magic{
	userid_t userid;
	uint8_t  succ;
	uint16_t count;
	mc_item_t itm[0];
}__attribute__((packed))consume_magic_t;

#define mc_query_magic_code(p_, buf) \
		send_request_to_mc_server(MC_PROTO_QUERY_MAGIC_CODE_GIFT, p_, 32, buf, (p_)->id)

#define mc_consume_magic_code(p_, buf, len, uid) \
		send_request_to_mc_server(MC_PROTO_CONSUME_MAGIC_GIFT, p_, len, buf, uid)


int mc_server_fd; // magic code server file descriptor
int magicserver_fd;

int magic_passwd_rand_item_callback(sprite_t* p, uint32_t id, char* buf, int len, int err);

int query_magic_code_gift_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 36);

	int i = 0;
	uint32_t location;
	UNPKG_UINT32(body, location, i);
	if (location != 1 && location != 2) {
		ERROR_RETURN(("bad location\t[%u]", p->id), -1);
	}
	*(uint32_t*)p->session = location;

	DEBUG_LOG("QUIRE MAGIC\t [uid=%u]", p->id);
	return mc_query_magic_code(p, body + 4);
}

static int query_magic_code_gift_callback(sprite_t * p, uint32_t id, char * buf, int len, int ret_code)
{
	if (ret_code != 0 ){
		DEBUG_LOG("QUIRE MAGIC CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		switch 	( ret_code ){
		case	1:
		case 	2:
		case	3:
		case	4:
		case	5:
		case	6:
		case	7:
		case	8:
		case	9:
				return send_to_self_error(p, p->waitcmd, -(51000 + ret_code), 1);
		default:
				ERROR_RETURN(("db error\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code), -1);
		}
	}
	CHECK_BODY_LEN_GE(len, 4);

	mc_gifts_t* mg = (mc_gifts_t*)buf;
	int k = sizeof(protocol_t);
	PKG_UINT8(msg, mg->gift_type, k);
	PKG_UINT16(msg, mg->select_num, k);
	PKG_UINT16(msg, mg->used_num, k);
	PKG_UINT16(msg, mg->itm_num, k);
	CHECK_BODY_LEN(len, 7 + mg->itm_num * 6);

	int loop;
	for (loop = 0; loop < mg->itm_num; loop++) {
		PKG_UINT32(msg, mg->itm[loop].gift_id, k);
		PKG_UINT32(msg, mg->itm[loop].gift_count, k);
	}
	if (mg->itm_num != 0) {
		item_kind_t* kind = find_kind_of_item(mg->itm[0].gift_id);
		if (!kind)
			ERROR_RETURN(("bad itmid\t[%u]", p->id), -1);
		if (kind->kind == CARD_ITEM_KIND && *(uint32_t*)p->session == 1) {
			return send_to_self_error(p, p->waitcmd, -ERR_magic_code_location_err, 1);
		}
	}
	DEBUG_LOG("QUIRE MAGIC CALLBACK\t[uid=%u select=%u used=%u num=%u]", p->id, mg->select_num, mg->used_num, mg->itm_num);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);

}

int consume_magic_code_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN_GE(len, 38);
	CHECK_VALID_ID(p->id);

	uint8_t buf_tmp[128];
	uint16_t count;
	userid_t uid;
	uint32_t itmid;
	uint16_t itm_cnt;

	int j = 0;
	UNPKG_STR(body, buf_tmp, j, MAGIC_CODE_LEN);
	UNPKG_UINT32(body, uid, j);
	UNPKG_UINT16(body, count, j);
	CHECK_BODY_LEN(len, 38 + count * 8);

	int i = MAGIC_CODE_LEN;
	PKG_H_UINT32(buf_tmp, uid, i);
	PKG_H_UINT32(buf_tmp, p->fdsess->remote_ip, i);
	PKG_H_UINT16(buf_tmp, count, i);
	int loop;
	for (loop = 0; loop < count; loop++){
		UNPKG_UINT32(body, itmid, j);
		UNPKG_UINT32(body, itm_cnt, j);

		PKG_H_UINT32(buf_tmp, itmid, i);
		PKG_H_UINT16(buf_tmp, itm_cnt, i);
	}

	uint8_t mc[MAGIC_CODE_LEN] = {0};
	memcpy(mc, buf_tmp, MAGIC_CODE_LEN);
	DEBUG_LOG("CONSUME MAGIC\t[uid=%u count=%u magic=%s]", p->id, count, mc);
	return mc_consume_magic_code(p, buf_tmp, i, uid);
}

static int consume_magic_code_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code)
{
	if (ret_code != 0 ){
		DEBUG_LOG("CONSUME MAGIC CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		switch 	( ret_code ){
		case	1:
		case 	2:
		case	3:
		case	4:
		case	5:
		case	6:
		case	7:
		case	8:
		case	9:
				return send_to_self_error(p, p->waitcmd, -(51000 + ret_code), 1);
		default:
				ERROR_RETURN(("db error\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code), -1);
		}
	}

	CHECK_BODY_LEN_GE(len, 7);
	consume_magic_t* cm = (consume_magic_t*)buf;
	CHECK_BODY_LEN(len, 7 + 6 * cm->count);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, cm->userid, j);
	PKG_UINT8(msg, cm->succ, j);
	PKG_UINT16(msg, cm->count, j);
	int loop;
	for (loop = 0; loop < cm->count; loop++){
		DEBUG_LOG("CONSUME MAGIC CALLBACK\t[uid=%u succ=%u count=%u itmid=%u]", p->id, cm->succ, cm->count, cm->itm[loop].gift_id);
		PKG_UINT32(msg, cm->itm[loop].gift_id, j);
		PKG_UINT32(msg, cm->itm[loop].gift_count, j);
	}

	DEBUG_LOG("CONSUME MAGIC CALLBACK\t[uid=%u succ=%u count=%u]", p->id, cm->succ, cm->count);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);

}


// function definitions
int send_request_to_mc_server(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];

	server_proto_t *pkg;
	int len;

	if (mc_server_fd == -1) {
		mc_server_fd = connect_to_svr(config_get_strval("mc_server_ip"), config_get_intval("mc_server_port", 0), 65535, 1);
	}
	DEBUG_LOG("mc_fd[%u %u]", p ? p->id : 0, mc_server_fd);

	len = sizeof (server_proto_t) + body_len;
	if (mc_server_fd == -1 || mc_server_fd > epi.maxfd ||
			epi.fds[mc_server_fd].cb.sndbufsz < epi.fds[mc_server_fd].cb.sendlen + len ||
			body_len > sizeof(dbbuf) - sizeof(*pkg)) {
		ERROR_LOG ("send to mc failed, buflen=%d, fd=%d",
					epi.fds[mc_server_fd].cb.sendlen, mc_server_fd);
		if (p)
			return send_to_self_error(p, p->waitcmd, -ERR_system_error, 1);
		return -1;
	}

	pkg = (server_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	if (!p) pkg->seq = 0;
	else pkg->seq = (sprite_fd (p) << 16) | p->waitcmd;
	pkg->ret = 0;
	memcpy (pkg->body, body_buf, body_len);

	return net_send (mc_server_fd, dbbuf, len, 0);
}

/**
  * @brief handle the mibiserver return message
  *
  * @param fd, the file discriptor which used to communicate with mibi server.
  * @param mibipkg,  message from mibi server.
  * @param len, the length of message
  * @param sp,  used to store sprite pointer
  *
  * @return int, 0 on success, -1 on error.
  */
int handle_mc_server_return(int fd, server_proto_t* mc_pkg, int len, sprite_t** sp)
{
	int waitcmd = mc_pkg->seq & 0xFFFF;
	int conn    = mc_pkg->seq >> 16;

	if ( !mc_pkg->seq )
		return 0;

	//to prevent from reclose the connection, it return 0
	if (!(*sp = get_sprite_by_fd(conn)) || (*sp)->waitcmd != waitcmd) {
		ERROR_RETURN(("connection has been closed, id=%u, cmd=0x%X %d, seq=%u",
						mc_pkg->id, mc_pkg->cmd, waitcmd, mc_pkg->seq), 0);
	}

	DEBUG_LOG("MC SERVER RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, mc_pkg->id, waitcmd, mc_pkg->cmd, mc_pkg->ret);

	int ret = -1;
#define PROC_MC_MESSAGE(n, func) \
	case n: ret = func(*sp, mc_pkg->id, mc_pkg->body, len - sizeof (server_proto_t), mc_pkg->ret); break

	switch (mc_pkg->cmd) {
		PROC_MC_MESSAGE(MC_PROTO_QUERY_MAGIC_CODE_GIFT, query_magic_code_gift_callback);
		PROC_MC_MESSAGE(MC_PROTO_CONSUME_MAGIC_GIFT, consume_magic_code_callback);

#undef PROC_MC_MESSAGE
		default:
			ERROR_LOG("unknow cmd=0x%x, id=%u, waitcmd=%d", mc_pkg->cmd, (*sp)->id, (*sp)->waitcmd);
			break;
	}

	return ret;
}

// function definitions
int send_request_to_magicserver(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];

	server_proto_t *pkg;
	int len;

	if (magicserver_fd == -1) {
		magicserver_fd = connect_to_svr(config_get_strval("magicserver_ip"), config_get_intval("magicserver_port", 0), 65535, 1);
	}

	len = sizeof (server_proto_t) + body_len;
	if (magicserver_fd == -1 || magicserver_fd > epi.maxfd ||
			epi.fds[magicserver_fd].cb.sndbufsz < epi.fds[magicserver_fd].cb.sendlen + len ||
			body_len > sizeof(dbbuf) - sizeof(*pkg)) {
		ERROR_LOG ("send to dbproxy failed, buflen=%d, fd=%d",
					epi.fds[magicserver_fd].cb.sendlen, magicserver_fd);
		if (p)
			return send_to_self_error(p, p->waitcmd, -ERR_system_error, 1);
		return -1;
	}

	pkg = (server_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	if (!p) pkg->seq = 0;
	else pkg->seq = (sprite_fd (p) << 16) | p->waitcmd;
	pkg->ret = 0;
	memcpy (pkg->body, body_buf, body_len);

	return net_send (magicserver_fd, dbbuf, len, 0);
}

int handle_magicserver_return(int fd, server_proto_t* magicpkg, int len, sprite_t** sp)
{
	int waitcmd = magicpkg->seq & 0xFFFF;
	int conn    = magicpkg->seq >> 16;

	if (!magicpkg->seq)
		return 0;

	//to prevent from reclose the connection, it return 0
	if (!(*sp = get_sprite_by_fd(conn)) || (*sp)->waitcmd != waitcmd) {
		ERROR_RETURN(("connection has been closed, id=%u, cmd=0x%X %d, seq=%u",
						magicpkg->id, magicpkg->cmd, waitcmd, magicpkg->seq), 0);
	}

	DEBUG_LOG("MAGICSERVER RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, magicpkg->id, waitcmd, magicpkg->cmd, magicpkg->ret);

	int ret = -1;
#define PROC_MAGIC_MESSAGE(n, func) \
	case n: ret = func(*sp, magicpkg->id, magicpkg->body, len - sizeof (server_proto_t), magicpkg->ret); break

	switch (magicpkg->cmd) {
		PROC_MAGIC_MESSAGE(MAGIC_PROTO_GET_MAGIC_CODE_ITEMS, magic_passwd_rand_item_callback);
		PROC_MAGIC_MESSAGE(MAGIC_PROTO_LOOKUP_MAGIC_CODE_ITEMS, magic_passwd_rand_item_callback);

#undef PROC_MAGIC_MESSAGE
		default:
			ERROR_LOG("unknow cmd=0x%x, id=%u, waitcmd=%d", magicpkg->cmd, (*sp)->id, (*sp)->waitcmd);
			break;
	}

	return ret;
}

int magic_passwd_item_lookup_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	 CHECK_BODY_LEN(bodylen, 32);
	 CHECK_VALID_ID(p->id);
	 return send_request_to_magicserver(MAGIC_PROTO_LOOKUP_MAGIC_CODE_ITEMS, p, bodylen, body, p->id);
}

int magic_passwd_rand_item_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 32);

	char buff[128] = {0};

	memcpy(buff + 32, body, 32);
	memcpy(buff + 64,&p->fdsess->remote_ip, 4);
	return send_request_to_magicserver(MAGIC_PROTO_GET_MAGIC_CODE_ITEMS, p, 68,buff, p->id);
}

int magic_passwd_rand_item_callback(sprite_t* p, uint32_t id, char* buf, int len, int err)
{
	if (err) {
		return send_to_self_error(p, p->waitcmd, -(err + ERR_magic_server_system_base), 1);
	}

	CHECK_BODY_LEN_GE(len, 6);
	uint32_t gift_pool = *(uint16_t *)buf;
	uint32_t count = *(uint32_t*)(buf + 2);
	CHECK_BODY_LEN(len, 6 + count * sizeof(uint32_t));

	int j = sizeof(protocol_t);
    PKG_UINT32(msg, gift_pool, j);
    PKG_UINT32(msg, count, j);
    int loop;
    for (loop = 0; loop < count; loop++) {
        uint32_t itemid = *(uint32_t *)(buf + 6 + loop * sizeof(uint32_t));
        PKG_UINT32(msg, itemid, j);
    }

    init_proto_head(msg, p->waitcmd, j);
    return send_to_self(p, msg, j, 1);
}



