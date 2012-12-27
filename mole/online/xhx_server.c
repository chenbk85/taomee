/*
 * =====================================================================================
 *
 *       Filename:  xhx_server.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2011 03:48:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alexhe , alexhe@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <libtaomee/conf_parser/config.h>
#include <openssl/md5.h>

#include "util.h"
#include "sprite.h"
#include "protoheaders.h"
#include "proto.h"
#include "dbproxy.h"
#include "exclu_things.h"
#include "xhx_server.h"

int xhx_server_fd;

int send_request_to_xhxserver(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE] = {0};

	server_proto_t *pkg = NULL;
	int len;

	if (xhx_server_fd == -1) {
		xhx_server_fd = connect_to_svr(config_get_strval("xhx_switch_ip"), config_get_intval("xhx_switch_port", 0), 65535, 1);
        DEBUG_LOG("RECONNECT TO XIAO HUE XIAN SERVER [%d %d]", id, xhx_server_fd);
	}

	len = sizeof (server_proto_t) + body_len;
	if (xhx_server_fd == -1 || xhx_server_fd > epi.maxfd || 
			epi.fds[xhx_server_fd].cb.sndbufsz < epi.fds[xhx_server_fd].cb.sendlen + len ||
			body_len > sizeof(dbbuf) - sizeof(*pkg)) {
		ERROR_LOG ("send to xiao hua xian switch server failed, buflen=%d, fd=%d", 
					epi.fds[xhx_server_fd].cb.sendlen, xhx_server_fd); 
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
	DEBUG_LOG("sent to xhx server: len=%u body_len=%u", len, body_len);

	return net_send (xhx_server_fd, dbbuf, len, 0);
}

int handle_xhxserver_return(int fd, server_proto_t* dbpkg, int len, sprite_t** sp)
{
	int waitcmd = dbpkg->seq & 0xFFFF;
	int conn    = dbpkg->seq >> 16;

	if ( !dbpkg->seq )
		return 0;

	//to prevent from reclose the connection, it return 0
	if (!(*sp = get_sprite_by_fd(conn)) || (*sp)->waitcmd != waitcmd) {
		ERROR_RETURN(("connection has been closed, id=%u, cmd=0x%X %d, seq=%u",
						dbpkg->id, dbpkg->cmd, waitcmd, dbpkg->seq), 0);
	}

	DEBUG_LOG("XIAO HUA XIAN SERVER RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, dbpkg->id, waitcmd, dbpkg->cmd, dbpkg->ret);
	switch (dbpkg->ret) {
		case 0:
			break;
		case 1105:
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_not_exist_user, 1);
		default:
			return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}

	int ret = -1;
#define PROC_XHX_MESSAGE(n, func) \
	case n: ret = func(*sp, dbpkg->id, dbpkg->body, len - sizeof (server_proto_t)); break
		
	switch (dbpkg->cmd) {

#undef PROC_XHX_MESSAGE
		default:
			ERROR_LOG("unknow cmd=0x%x, id=%u, waitcmd=%d", dbpkg->cmd, (*sp)->id, (*sp)->waitcmd);
			break;
	}

	return ret;
}

/*
 * @brief 小花仙联运活动接取 4110
 */
int xhx_get_tast_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t flag = 0;
	int i = 0;
	UNPKG_UINT32(body, flag, i);
	*(uint32_t*)p->session = flag;
	if (flag > 2 ) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	uint32_t day_limit;
#ifndef TW_VER
	day_limit = 20111114;
#else
	day_limit = 20111121;
#endif
	if (flag == 2 && (get_today() >= day_limit)) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	
	return send_request_to_db(SVR_PROTO_XHX_GET_TASK, p, 4, &flag, p->id);
}

int xhx_get_tast_callback(sprite_t * p, uint32_t id, char * buf, int len)
{
	typedef struct {
		uint32_t ret_state;
	}__attribute__((packed)) xhx_get_task_cbk_pack_t;
	xhx_get_task_cbk_pack_t* cbk_pack = (xhx_get_task_cbk_pack_t*)buf;

	uint32_t flag = *(uint32_t*)p->session;
	uint32_t msgbuf[] = {p->id, 1};
	if (flag == 2) {
		msglog(statistic_logfile, 0x0409BD98, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
		if (cbk_pack->ret_state == 2) {	//花宝猪任务接取成功，向小花仙switch发消息
			uint32_t type = 1;
			send_request_to_xhxserver(XHX_SWITCH_PROTO_USER_SET_VALUE, NULL, 4, &type, p->id);	
			msglog(statistic_logfile, 0x0409BD99, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
		} else if (cbk_pack->ret_state == 4) {//已经成功洗礼过
			cbk_pack->ret_state = 2;
		}
	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->ret_state, l);
	init_proto_head(msg, p->waitcmd, l);
    return send_to_self(p, msg, l, 1);
}

/*
 * @brief 查询用户是否来自小花仙 4111
 */
int xhx_check_user_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);

	uint32_t day_limit;
#ifndef TW_VER
	day_limit = 20111114;
#else
	day_limit = 20111121;
#endif
	if (get_today() >= day_limit || !(get_now_tm()->tm_hour >= 19 && get_now_tm()->tm_hour < 21)) {
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, 0, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}

	uint8_t db_buf[1024] = {0};
	int l = 0;
	PKG_H_UINT32(db_buf, 99, l);
	PKG_H_UINT32(db_buf, 1351128, l);
	PKG_H_UINT32(db_buf, 1351129, l);
	PKG_H_UINT8(db_buf, 0, l);
	return send_request_to_db(SVR_PROTO_CHK_ITEM, p, l, db_buf, p->id);
}

int xhx_check_user_callback(sprite_t * p, uint32_t id, char * buf, int len)
{
	uint32_t count;
	int i = 0;
	UNPKG_H_UINT32(buf, count, i);
	uint32_t ret_state = 0;
	if (count == 0) {
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, ret_state, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}
	
	uint32_t daytype = 50006;
	*(uint32_t*)p->session = daytype;
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &daytype, p->id);
}

int user_check_entry_xhx_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t daytype = *(uint32_t*)p->session;
	uint32_t down = *(uint32_t*)buf;
	uint32_t ret_state;
	if (daytype == 50006) {
		if (down == 1) {	//50006完成，判断50007是否完成
			daytype = 50007;
		    *(uint32_t*)p->session = daytype;
		    return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &daytype, p->id);
		}
		ret_state = 1;
	} else if (daytype == 50007) {
		ret_state = (down == 1) ? 3 : 2;
	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_state, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 小花仙增加物品 4112
 */
int xhx_add_items_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t value;
	int i = 0;
	UNPKG_UINT32(body, value, i);
	*(uint32_t*)(p->session) = value;
	uint32_t day_limit;
#ifndef TW_VER
	day_limit = 20111114;
#else
	day_limit = 20111121;
#endif
	if (get_today() >= day_limit || !(get_now_tm()->tm_hour >= 19 && get_now_tm()->tm_hour < 21)) {
		DEBUG_LOG("xhx can not add item: id[%u] count[%u]", p->id, value);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	if (value == 1) {	//value=1时，通知小花仙获得拉姆花种子，1天只能1次
		return db_set_sth_done(p, 50006, 1, p->id);
	} else if (value == 2) {	//花宝猪吃小仙果
		uint32_t db_buf[]={1, 0, 0, 0, 99, 1351127, 1};
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);
		
		return xhx_add_one_item(p, 0);
	} else if (value == 3) {	//获得小花仙神奇水珠，1天只能1次
		return db_set_sth_done(p, 50007, 1, p->id);
	}

	return send_to_self_error(p, p->waitcmd, ERR_invalid_item_id, 1);
}

int xhx_check_add_items_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t value = *(uint32_t*)p->session;
	uint32_t type = 0;	
	if (value == 1) {//获得拉姆花种子
		type = 2;
	} else if (value == 3) {//获得神奇水珠
		type = 3;
	}
	if (type != 0) {
		send_request_to_xhxserver(XHX_SWITCH_PROTO_USER_SET_VALUE, NULL, 4, &type, p->id);
	}

	if (value == 3) {
		return xhx_add_one_item(p, 5);
	}
	return xhx_add_one_item(p, 1);
}

int xhx_add_one_item(sprite_t * p, uint32_t state)
{
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, state, l);
	init_proto_head(msg, p->waitcmd, l);
    return send_to_self(p, msg, l, 1);
}

int deal_xhx_event(sprite_t* p, uint32_t type)
{
	uint32_t msg_type = 0;
	uint32_t attireid = 0;
	switch (type) {
	case 1://从小花仙带回花宝猪，标记花宝猪任务小花仙部分完成
		{
			//uint32_t db_buf1[] = {10000, 3};
			//send_request_to_db(SVR_PROTO_SET_CONTINUE_LOGIN_VALUE, NULL, sizeof(db_buf1), db_buf1, p->id);
			attireid = 1351126;
			msg_type = 2;
		}
		break;
	case 2://小仙果,从小花仙获得小仙果，加入到摩尔庄园数据库，每天1次
		{
			attireid = 1351127;
			msg_type = 3;
			//db_set_sth_done(NULL, 50005, 1, p->id);
		}
		break;
	case 3://进入虫虫谷，添加一个临时物品标记用户时从小花仙过来的。
		{
			attireid = 1351128;
		}
		break;
	case 4://小花仙获得拉菲花
		{
			attireid = 1351129;
			msg_type = 4;
		}
		break;
	default:
		break;
	}
	if (attireid != 0) {
		//uint32_t db_buf[]={0, 1, 0, 0, 99, attireid, 1, 99999};
		//send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);
	}

	if (msg_type != 0) {//前端主动通知包,通知前端带回花宝猪、获得小仙果
		DEBUG_LOG("send to flash: msg uid[%u] type[%u] cmd[%d]", p->id, msg_type, PROTO_XHX_ADD_ITEMS);
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, msg_type, l);
		init_proto_head(msg, PROTO_XHX_ADD_ITEMS, l);
		send_to_self(p, msg, l, 0);
	}
	return 0;
}
