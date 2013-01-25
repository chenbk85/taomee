#include <arpa/inet.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>
#include <async_serv/async_serv.h>

#ifdef __cplusplus
}
#endif


#include "code.h"
#include "sns.h"
#include "cli_proto.h"
#include "cli_login.h"
#include "items.h"
#include "./proto/mole2_db.h"

int check_magic_code_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	KDEBUG_LOG(p->id, "CHK CODE");
	return send_request_to_code(p, proto_code_check_code, body, len);
}

int use_magic_code_cmd(sprite_t* p, uint8_t* body, uint32_t bodylen)
{
	char code[MAGIC_CODE_LEN];
	uint32_t count;
	int j = 0;
	UNPKG_STR(body, code, j, MAGIC_CODE_LEN);
	UNPKG_UINT32(body, count, j);
	CHECK_BODY_LEN(bodylen, j + count * 8);

	KDEBUG_LOG(p->id, "USE CODE\t[%u]", count);

	if (count > 20) return -1;

	uint8_t outbuf[256];
	int i = 0;
	PKG_STR(outbuf, code, i, MAGIC_CODE_LEN);
	PKG_H_UINT32(outbuf, p->id, i);
	PKG_H_UINT32(outbuf, get_cli_ip(p->fdsess), i);
	PKG_H_UINT8(outbuf, idc_type == 2 ? 2 : 1, i);
	PKG_H_UINT16(outbuf, count, i);
	uint32_t loop;
	for (loop = 0; loop < count; loop ++) {
		uint32_t giftid, giftcnt;
		UNPKG_UINT32(body, giftid, j);
		UNPKG_UINT32(body, giftcnt, j);
		PKG_H_UINT32(outbuf, giftid, i);
		PKG_H_UINT16(outbuf, giftcnt, i);
	}
	return send_request_to_code(p, proto_code_use_code, outbuf, i);
}

int magic_check_code_callback(sprite_t* p, uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_CODEERR(p, ret);
	uint32_t allcnt, usedcnt, hascnt, giftid, giftcnt;
	int j = 0;
	UNPKG_H_UINT16(body, allcnt, j);
	UNPKG_H_UINT16(body, usedcnt, j);
	UNPKG_H_UINT16(body, hascnt, j);
	CHECK_BODY_LEN(bodylen, j + hascnt * 6);

	KDEBUG_LOG(p->id, "CHK CODE BACK\t[%u %u %u]", allcnt, usedcnt, hascnt);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, allcnt, i);
	PKG_UINT32(msg, usedcnt, i);
	PKG_UINT32(msg, hascnt, i);
	
	uint32_t loop;
	for (loop = 0; loop < hascnt; loop ++) {
		UNPKG_H_UINT32(body, giftid, j);
		UNPKG_H_UINT16(body, giftcnt, j);
		PKG_UINT32(msg, giftid, i);
		PKG_UINT32(msg, giftcnt, i);
		KDEBUG_LOG(p->id, "CODE ITEM\t[%u %u]", giftid, giftcnt);
	}
	
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int magic_use_code_callback(sprite_t* p, uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_CODEERR(p, ret);
	uint32_t uid, result, count, giftid, giftcnt;
	int j = 0;
	UNPKG_H_UINT32(body, uid, j);
	UNPKG_H_UINT8(body, result, j);
	UNPKG_H_UINT16(body, count ,j);
	CHECK_BODY_LEN(bodylen, j + count * 6);
	
	KDEBUG_LOG(p->id, "USE CODE BACK\t[%u %u %u]", uid, result, count);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, result, i);
	PKG_UINT32(msg, count, i);

	uint32_t loop;
	for (loop = 0; loop < count; loop ++) {
		UNPKG_H_UINT32(body, giftid, j);
		UNPKG_H_UINT16(body, giftcnt, j);
		normal_item_t* pni = get_item(giftid);
		if (pni && pni->function == item_for_pet_exchange)
			msg_log_pet_egg(pni->petinfo.beastid, pet_egg_magic_code, 1);
		PKG_UINT32(msg, giftid, i);
		PKG_UINT32(msg, giftcnt, i);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);

}

int check_spacetime_code(sprite_t* p, Cmessage* c_in)
{
	check_spacetime_code_in* p_in = P_IN;
	KDEBUG_LOG(p->id, "CHK SPACE TIME CODE");
	return send_request_to_spacetime(p, proto_code_check_code, p_in->code, MAGIC_CODE_LEN);
}

int use_spacetime_code(sprite_t* p, Cmessage* c_in)
{	
	use_spacetime_code_in* p_in = P_IN;
	uint8_t outbuf[256];
	int i = 0;
	PKG_STR(outbuf, p_in->code, i, MAGIC_CODE_LEN);
	PKG_H_UINT8(outbuf, idc_type == 2 ? 2 : 1, i);
	PKG_H_UINT32(outbuf, get_cli_ip(p->fdsess), i);
	KDEBUG_LOG(p->id, "USE SPACE TIME CODE\t[%d]", i);
	return send_request_to_spacetime(p, proto_code_use_code, outbuf, i);
}

int spacetime_check_code_callback(sprite_t* p, uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_CODEERR(p, ret);
	uint32_t total_cnt, giftid;
	int j = 0;

	UNPKG_H_UINT32(body, total_cnt, j);
	KDEBUG_LOG(p->id, "CHK SPACE TIME BACK\t[%u]", total_cnt);
	CHECK_BODY_LEN(bodylen, j + 4 * total_cnt);
	
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, total_cnt, i);
	
	uint32_t loop;
	for (loop = 0; loop < total_cnt; loop ++) {
		UNPKG_H_UINT32(body, giftid, j);
		PKG_UINT32(msg, giftid, i);
		KDEBUG_LOG(p->id, "SPACE TIME ITEM\t[%u]", giftid);
	}
	
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int spacetime_use_code_callback(sprite_t* p, uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_CODEERR(p, ret);
	uint32_t giftid, giftcnt;
	int j = 0;
	UNPKG_H_UINT32(body, giftcnt, j);
	CHECK_BODY_LEN(bodylen, j + giftcnt * 4);
	
	KDEBUG_LOG(p->id, "USE SPACE TIME BACK\t[%u]", giftcnt);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, giftcnt, i);

	uint32_t loop;
	for (loop = 0; loop < giftcnt; loop ++) {
		UNPKG_H_UINT32(body, giftid, j);
		PKG_UINT32(msg, giftid, i);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);

}

//-----------------------------------------------------------------
/**
  * @brief set dbproxy protocol handle  
  */
#define SET_CODE_HANDLE(op_, func_, len_, cmp_) \
		do { \
			if (rstart == 0 && code_handles[op_].func != 0) { \
				ERROR_RETURN(("duplicate cmd=%u", op_), -1); \
			} \
			code_handles[op_].func = func_; \
			code_handles[op_].len = len_; \
			code_handles[op_].cmp_method = cmp_; \
		} while (0)

#define SET_SPACETIME_HANDLE(op_, func_, len_, cmp_) \
		do { \
			if (rstart == 0 && spacetime_handles[op_].func != 0) { \
				ERROR_RETURN(("duplicate cmd=%u", op_), -1); \
			} \
			spacetime_handles[op_].func = func_; \
			spacetime_handles[op_].len = len_; \
			spacetime_handles[op_].cmp_method = cmp_; \
		} while (0)

/**
  * @brief handle package return from dbproxy
  * @param dbpkg package from dbproxy
  * @param pkglen length of dbpkg
  */
void handle_magic_return(code_proto_t* cpkg, uint32_t pkglen)
{
	sprite_t* p = get_sprite(cpkg->id);
	if (!p) {
		KERROR_LOG(cpkg->id, "user not find: cmd=%u ret=%u", cpkg->cmd, cpkg->ret);
		return;
	}

	if (p->waitcmd != cpkg->cmd) {
		KERROR_LOG(cpkg->id, "cmd not match: wcmd=%u cmd=%u ret=%u", p->waitcmd, cpkg->cmd, cpkg->ret);
		return;
	}

	KDEBUG_LOG(p->id, "CODE\t[c=%u ret=%u]", cpkg->cmd, cpkg->ret);

	int err = -1;
	uint32_t bodylen = cpkg->len - sizeof(code_proto_t);

	if (!code_handles[cpkg->cmd].func) {
		KERROR_LOG(0, "code_handle is NULL: cmd=%d", cpkg->cmd);
		return;
	}

	if ( ((code_handles[cpkg->cmd].cmp_method == cmp_must_eq) && (bodylen != code_handles[cpkg->cmd].len))
			|| ((code_handles[cpkg->cmd].cmp_method == cmp_must_ge) && (bodylen < code_handles[cpkg->cmd].len)) ) {
		if ((bodylen != 0) || (cpkg->ret == 0)) {
			KERROR_LOG(p->id, "invalid package len=%u needlen=%u cmd=0x%X cmpmethod=%d ret=%u",
						bodylen, code_handles[cpkg->cmd].len, cpkg->cmd, code_handles[cpkg->cmd].cmp_method, cpkg->ret);
			goto ret;
		}
	}

	err = code_handles[cpkg->cmd].func(p, cpkg->body, bodylen, cpkg->ret);

ret:
	if (err) {
		KDEBUG_LOG(p->id, "CODE RET KICK USER OFF\t[%u]", err);
		close_client_conn(p->fd);
	}
}


/**
  * @brief init handles to handle protocol packages from dbproxy
  * @return true if all handles are inited successfully, false otherwise
  */
int init_magic_code_proto_handles(int rstart)
{
	/* operation, bodylen, compare method */
	SET_CODE_HANDLE(proto_code_check_code, magic_check_code_callback, 6, cmp_must_ge);
	SET_CODE_HANDLE(proto_code_use_code, magic_use_code_callback, 7, cmp_must_ge);

	return 0;
}

void handle_spacetime_return(code_proto_t* cpkg, uint32_t pkglen)
{
	sprite_t* p = get_sprite(cpkg->id);
	if (!p) {
		KERROR_LOG(cpkg->id, "user not find: cmd=%u ret=%u", cpkg->cmd, cpkg->ret);
		return;
	}

	KDEBUG_LOG(p->id, "CODE\t[c=%u ret=%u]", cpkg->cmd, cpkg->ret);

	if (p->waitcmd != cpkg->cmd + 1000) {
		KERROR_LOG(cpkg->id, "cmd not match: wcmd=%u cmd=%u ret=%u", p->waitcmd, cpkg->cmd, cpkg->ret);
		return;
	}

	int err = -1;
	uint32_t bodylen = cpkg->len - sizeof(code_proto_t);

	if (!spacetime_handles[cpkg->cmd].func) {
		KERROR_LOG(0, "code_handle is NULL: cmd=%d", cpkg->cmd);
		return;
	}

	if ( ((spacetime_handles[cpkg->cmd].cmp_method == cmp_must_eq) && (bodylen != spacetime_handles[cpkg->cmd].len))
			|| ((spacetime_handles[cpkg->cmd].cmp_method == cmp_must_ge) && (bodylen < spacetime_handles[cpkg->cmd].len)) ) {
		if ((bodylen != 0) || (cpkg->ret == 0)) {
			KERROR_LOG(p->id, "invalid package len=%u needlen=%u cmd=0x%X cmpmethod=%d ret=%u",
						bodylen, spacetime_handles[cpkg->cmd].len, cpkg->cmd, spacetime_handles[cpkg->cmd].cmp_method, cpkg->ret);
			goto ret;
		}
	}

	err = spacetime_handles[cpkg->cmd].func(p, cpkg->body, bodylen, cpkg->ret);

ret:
	if (err) {
		KDEBUG_LOG(p->id, "SPACE RET KICK USER OFF\t[%u]", err);
		close_client_conn(p->fd);
	}
}


int init_spacetime_code_proto_handles(int rstart)
{
	/* operation, bodylen, compare method */
	SET_SPACETIME_HANDLE(proto_code_check_code, spacetime_check_code_callback, 4, cmp_must_ge);
	SET_SPACETIME_HANDLE(proto_code_use_code, spacetime_use_code_callback, 4, cmp_must_ge);
	return 0;
}
