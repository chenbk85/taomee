#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <libtaomee/conf_parser/config.h>

#include "util.h"
#include "sprite.h"
#include "protoheaders.h"
#include "proto.h"
#include "dbproxy.h"

#include "cache_svr.h"

#define cs_get_jy_and_pet_state(p_, buf, len, uid) \
		send_request_to_cache_server(CS_GET_JY_AND_PET_STATE, p_, len, buf, uid)
#define cs_get_friend_home_hot(p_, buf, len, uid) \
		send_request_to_cache_server(CS_GET_FRIEND_HOME_HOT_STATE, p_, len, buf, uid)

int stampsvr_tcp_fd;

int cs_set_shop_info(uint32_t exp, uint32_t money, uint32_t uid)
{
	uint32_t buff[2] = {exp,money};

	return send_request_to_cache_server(CS_SET_FRIEND_SHOP_EXP_MONEY, NULL, 8, buff, uid);
}

int cs_get_shops_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	int count;
	uint32_t buff[500 + 1];
	uint32_t* uin = buff + 1;

	DEBUG_LOG ("CHK cs_get_shops_info_cmd\t[%u]", p->id);
	if (IS_GUEST_ID (p->id) || IS_NPC_ID (p->id)) {
		ERROR_RETURN(("not a register user\t[%u]", p->id), -1);
	}

	CHECK_BODY_LEN_GE(len, 4);

	int i = 0;
	UNPKG_UINT32(body, count, i);
	buff[0] = count;
	if (count > 500) {
		ERROR_RETURN(("too many users, count=%d", count), -1);
	}

	CHECK_BODY_LEN(len, count * 4 + 4);

	int j = 0;
	int cnt = 0;
	uint32_t uid = 0;
	for (; j != count; ++j) {
		UNPKG_UINT32(body, uid, i);
		if(!uid || IS_NPC_ID(uid) || IS_GUEST_ID(uid)) continue;
		uin[cnt++] = uid;
	}
	buff[0] = cnt;
	len = cnt * 4 + 4;
	DEBUG_LOG("uin[%d]=%u",cnt-1,uin[cnt-1]);

	return send_request_to_cache_server(CS_GET_FRIEND_SHOP_EXP_MONEY, p, len, buff, p->id);
}

int cs_get_shops_info_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code)
{
	typedef struct cs_shop_info{
			uint32_t	uid;
			uint32_t	exp;
			uint32_t	money;
	} cs_shop_info_t;

	if (ret_code != 0 ){
		ERROR_RETURN(("cache server return error\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code), -1);
	}

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + 12 * count);

	DEBUG_LOG("cs_get_shops_info_callback\t[uid=%u count=%u]", p->id, count);

	int loop;
	int cnt = 0;
	int j = sizeof(protocol_t) + 4;
	cs_shop_info_t* jp = (cs_shop_info_t*)(buf + 4);
	for (loop = 0; loop < count; loop++,jp++) {
		if(jp->uid != p->id && jp->exp == 0 && jp->money == 0)
			continue;
		cnt++;
		PKG_UINT32(msg, jp->uid, j);
		PKG_UINT32(msg, jp->exp, j);
		PKG_UINT32(msg, jp->money, j);
	}
	count = sizeof(protocol_t);
	PKG_UINT32(msg,cnt,count);
	j = sizeof(protocol_t) + 4 + 12 * cnt;

	DEBUG_LOG("cs_get_shops_info_callback\t[uid=%u cnt=%u]", p->id, cnt);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int chk_jy_and_pet_state_cmd(sprite_t* p, const uint8_t* body, int len)
{
	int count;
	uint32_t buff[SPRITE_FRIENDS_MAX + 1];
	uint32_t* uin = buff + 1;

	DEBUG_LOG ("CHK JY PET STATE\t[%u]", p->id);
	if (IS_GUEST_ID (p->id) || IS_NPC_ID (p->id)) {
		ERROR_RETURN(("not a register user\t[%u]", p->id), -1);
	}

	CHECK_BODY_LEN_GE(len, 4);

	int i = 0;
	UNPKG_UINT32(body, count, i);
	buff[0] = count;
	if (count > SPRITE_FRIENDS_MAX) {
		ERROR_RETURN(("too many users, count=%d", count), -1);
	}
	CHECK_BODY_LEN(len, count * 4 + 4);

	int j = 0;
	for (; j != count; ++j) {
		UNPKG_UINT32(body, uin[j], i);
		CHECK_VALID_ID(uin[j]);
	}

	return cs_get_jy_and_pet_state(p, buff, i, 0);
}

int get_jy_and_pet_state_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code)
{
typedef struct jy_pet_state{
		uint32_t	uid;
		uint32_t	change_time;
		uint32_t	pet_status;
} __attribute__((packed)) jy_pet_state_t;

	if (ret_code != 0 ){
		ERROR_RETURN(("cache server return error\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code), -1);
	}

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + 12 * count);

	int j = sizeof(protocol_t);
	int loop;
	PKG_UINT32(msg, count, j);
	jy_pet_state_t* jp = (jy_pet_state_t*)(buf + 4);
	for (loop = 0; loop < count; loop++) {
		PKG_UINT32(msg, jp->uid, j);
		PKG_UINT32(msg, jp->change_time, j);
		PKG_UINT32(msg, jp->pet_status, j);
		//DEBUG_LOG("JY PET state\t[uid=%u %u %u %u count=%u]", p->id, jp->uid, jp->change_time, jp->pet_status, count);

		jp++;
	}

	DEBUG_LOG("CHK JY PET CALLBACK\t[uid=%u count=%u]", p->id, count);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);

}

int get_friend_home_hot_cmd(sprite_t* p, const uint8_t* body, int len)
{
	int count;
	uint32_t buff[SPRITE_FRIENDS_MAX + 1];
	uint32_t* uin = buff + 1;

	DEBUG_LOG ("GET FRIEND HOME HOT\t[%u]", p->id);
	if (IS_GUEST_ID (p->id) || IS_NPC_ID (p->id)) {
		ERROR_RETURN(("not a register user\t[%u]", p->id), -1);
	}

	CHECK_BODY_LEN_GE(len, 4);

	int i = 0;
	UNPKG_UINT32(body, count, i);
	buff[0] = count;
	if (count > SPRITE_FRIENDS_MAX) {
		ERROR_RETURN(("too many users, count=%d", count), -1);
	}
	CHECK_BODY_LEN(len, count * 4 + 4);

	int j = 0;
	for (; j != count; ++j) {
		UNPKG_UINT32(body, uin[j], i);
		CHECK_VALID_ID(uin[j]);
	}

	return cs_get_friend_home_hot(p, buff, i, 0);
}

int get_friend_home_hot_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code)
{
typedef struct home_hot{
		uint32_t	uid;
		uint32_t	home_hot;
} __attribute__((packed)) home_hot_t;

	if (ret_code != 0 ){
		ERROR_RETURN(("cache server return error\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code), -1);
	}

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + 8 * count);

	int j = sizeof(protocol_t);
	int loop;
	PKG_UINT32(msg, count, j);
	home_hot_t* hh = (home_hot_t*)(buf + 4);
	for (loop = 0; loop < count; loop++) {
		PKG_UINT32(msg, hh->uid, j);
		PKG_UINT32(msg, hh->home_hot, j);
		//DEBUG_LOG("JY PET state\t[uid=%u %u %u %u count=%u]", p->id, jp->uid, jp->change_time, jp->pet_status, count);

		hh++;
	}

	DEBUG_LOG("GET FRIEND HOME HOT CALLBACK\t[uid=%u count=%u]", p->id, count);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);

}

// function definitions
int send_request_to_cache_server(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];

	cs_proto_t *pkg;
	int len;

	if (stampsvr_tcp_fd == -1) {
		stampsvr_tcp_fd = connect_to_svr(config_get_strval("stamp_server_ip"), config_get_intval("stamp_server_port", 0), 65535, 1);
	}

	len = sizeof (cs_proto_t) + body_len;
	if (stampsvr_tcp_fd == -1 || stampsvr_tcp_fd > epi.maxfd ||
			epi.fds[stampsvr_tcp_fd].cb.sndbufsz < epi.fds[stampsvr_tcp_fd].cb.sendlen + len ||
			body_len > sizeof(dbbuf) - sizeof(*pkg)) {
		ERROR_LOG ("send to cache server failed, buflen=%d, fd=%d",
					epi.fds[stampsvr_tcp_fd].cb.sendlen, stampsvr_tcp_fd);
		if (p)
			return send_to_self_error(p, p->waitcmd, -ERR_system_error, 1);
		return -1;
	}

	pkg = (cs_proto_t*)dbbuf;
	pkg->len = htonl(len);
	pkg->cmd = htonl(cmd);
	pkg->uid = id;
	if (!p) pkg->seq = 0;
	else pkg->seq = (sprite_fd (p) << 16) | p->waitcmd;
	pkg->ver = 0;
	memcpy (pkg->body, body_buf, body_len);

	return net_send (stampsvr_tcp_fd, dbbuf, len, 0);
}

/**
  * @brief handle the cache server return message
  *
  * @param fd, the file discriptor which used to communicate with cache server.
  * @param cs_pkg,  message from mibi server.
  * @param len, the length of message
  * @param sp,  used to store sprite pointer
  *
  * @return int, 0 on success, -1 on error.
  */
int handle_cs_server_return(int fd, server_proto_t* cs_pkg, int len, sprite_t** sp)
{
	int waitcmd = cs_pkg->seq & 0xFFFF;
	int conn    = cs_pkg->seq >> 16;

	if ( !cs_pkg->seq )
		return 0;

	//to prevent from reclose the connection, it return 0
	if (!(*sp = get_sprite_by_fd(conn)) || (*sp)->waitcmd != waitcmd) {
		ERROR_RETURN(("connection has been closed, id=%u, cmd=0x%X %d, seq=%u",
						cs_pkg->id, cs_pkg->cmd, waitcmd, cs_pkg->seq), 0);
	}

	DEBUG_LOG("CACHE SERVER RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, cs_pkg->id, waitcmd, cs_pkg->cmd, cs_pkg->ret);

	int ret = -1;
#define PROC_CS_MESSAGE(n, func) \
	case n: ret = func(*sp, cs_pkg->id, cs_pkg->body, len - sizeof (server_proto_t), cs_pkg->ret); break

	switch (cs_pkg->cmd) {
		PROC_CS_MESSAGE(CS_GET_JY_AND_PET_STATE, get_jy_and_pet_state_callback);
		PROC_CS_MESSAGE(CS_GET_FRIEND_HOME_HOT_STATE, get_friend_home_hot_callback);
		PROC_CS_MESSAGE(CS_GET_FRIEND_SHOP_EXP_MONEY, cs_get_shops_info_callback);

#undef PROC_CS_MESSAGE
		default:
			ERROR_LOG("unknow cmd=0x%x, id=%u, waitcmd=%d", cs_pkg->cmd, (*sp)->id, (*sp)->waitcmd);
			break;
	}

	return ret;
}




