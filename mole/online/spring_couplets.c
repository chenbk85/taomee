/*
 * =====================================================================================
 *
 *       Filename:  spring_couplets.c
 *
 *    Description:  春联魔法墙
 *
 *        Version:  1.0
 *        Created:  01/09/2012 02:20:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alexhe, alexhe@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "item.h"
#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "message.h"
#include "spring_couplets.h"

int send_state_to_self(sprite_t* p, uint32_t ret_state)
{
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_state, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 春联魔法墙:提交春联 4124
 */
int user_submit_spring_couplets_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, (MAX_COUPLETS_TITLE_SIZE + 2 * MAX_COUPLETS_WORD_SIZE) * sizeof(uint32_t));
	
	spring_couplets_info_t package;
	memset(&package, 0, sizeof(package));

	uint32_t temp_id = 0;
	int idx = 0;
	uint32_t l = 0;
	for (idx = 0; idx < MAX_COUPLETS_TITLE_SIZE; idx++) {
		UNPKG_UINT32(buf, temp_id, l);
		if ((temp_id == 0) || (temp_id > MAX_COUPLETS_WORD_ID)) {
			return send_state_to_self(p, 1);
		}
		package.couplets_title[idx] = (uint8_t)(temp_id);
	}

	uint32_t first_len = 0;
	for (idx = 0; idx < MAX_COUPLETS_WORD_SIZE; idx++) {
		UNPKG_UINT32(buf, temp_id, l);
		if ((temp_id == 0 && idx < 5) || (temp_id > MAX_COUPLETS_WORD_ID)) {
			return send_state_to_self(p, 1);
		}
		if (temp_id == 0 && first_len == 0) {//当遇到第一个为0的id时，表示春联的长度,对len进行赋值
			first_len = idx;
		}
		package.first_line[idx] = (uint8_t)(temp_id);
	}

	uint32_t second_len = 0;
	for (idx = 0; idx < MAX_COUPLETS_WORD_SIZE; idx++) {
		UNPKG_UINT32(buf, temp_id, l);
		if ((temp_id == 0 && idx < 5) || (temp_id > MAX_COUPLETS_WORD_ID)) {
			return send_state_to_self(p, 1);
		}
		if (temp_id == 0 && second_len == 0) {
			second_len = idx;
		}
		package.second_line[idx] = (uint8_t)(temp_id);
	}
	
	DEBUG_LOG("user_submit_spring_couplets_cmd: uid[%u] idx1[%u] idx2[%u]", p->id, first_len, second_len);
	if (first_len != second_len) {
		return send_state_to_self(p, 1);
	}
	
	return send_request_to_db(SVR_PROTO_USER_SUBMIT_SPRING_COUPLETS, p, sizeof(package), &package, p->id);
}

int user_submit_spring_couplets_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t ret_state;
	}__attribute__((packed))submit_spring_couplets_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(submit_spring_couplets_cbk_pack_t));
	
	submit_spring_couplets_cbk_pack_t* cbk_pack = (submit_spring_couplets_cbk_pack_t*)buf;
	
	if (cbk_pack->ret_state == 1) {	//第一次提交成功，需要插入到全局表
		typedef struct {
			uint32_t userid;
			char nick[USER_NICK_LEN];
			//uint32_t couplets_id;
		}__attribute__((packed)) add_couplets_to_sysdb_package_t;
		add_couplets_to_sysdb_package_t package;
		package.userid = p->id;
		memcpy(package.nick, p->nick, USER_NICK_LEN);
		//package.couplets_id = cbk_pack->couplets_id;
		send_request_to_db(SVR_PROTO_SET_SYSARG_COUPLETS_INFO, NULL, sizeof(package), &package, p->id);
		cbk_pack->ret_state = 0;//改成成功状态
	}

	if (cbk_pack->ret_state == 0) {
		uint32_t msgbuff[] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C300, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	return send_state_to_self(p, cbk_pack->ret_state);
}

/*
 * @brief 春联魔法墙:查看用户的所有春联 4125
 */
int user_get_spring_couplets_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t uid = 0;
	int i = 0;
	UNPKG_UINT32(buf, uid, i);
	CHECK_VALID_ID(uid);

	//DEBUG_LOG("user_get_spring_couplets_cmd:uid[%u]", uid);
	*(uint32_t*)p->session = uid;
	return send_request_to_db(SVR_PROTO_USER_GET_SPRING_COUPLETS, p, 0, NULL, uid);
}

int user_get_spring_couplets_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		char nick[USER_NICK_LEN];
		uint32_t prize_id;
		uint32_t couplets_cnt;
	}__attribute__((packed)) spring_couplets_cbk_head_t;

	CHECK_BODY_LEN_GE(len, sizeof(spring_couplets_cbk_head_t));
	spring_couplets_cbk_head_t* cbk_head = (spring_couplets_cbk_head_t*)buf;
	CHECK_BODY_LEN(len, cbk_head->couplets_cnt * sizeof(spring_couplets_info_t) + sizeof(spring_couplets_cbk_head_t));
	spring_couplets_info_t* p_cbk_pack = (spring_couplets_info_t*)(buf + sizeof(spring_couplets_cbk_head_t));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, id, l);
	PKG_STR(msg, cbk_head->nick, l, USER_NICK_LEN);
	PKG_UINT32(msg, cbk_head->prize_id, l);
	PKG_UINT32(msg, cbk_head->couplets_cnt, l);

	int i = 0;
	for (i = 0; i < cbk_head->couplets_cnt; i++) {
		int j = 0;
		PKG_UINT32(msg, (p_cbk_pack+i)->couplets_id, l);
		for (j = 0; j < MAX_COUPLETS_TITLE_SIZE; j++) {
			PKG_UINT8(msg, (p_cbk_pack+i)->couplets_title[j], l);
		}
		for (j = 0; j < MAX_COUPLETS_WORD_SIZE; j++) {
			PKG_UINT8(msg, (p_cbk_pack+i)->first_line[j], l);
		}
		for (j = 0; j < MAX_COUPLETS_WORD_SIZE; j++) {
			PKG_UINT8(msg, (p_cbk_pack+i)->second_line[j], l);
		}
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 春联魔法墙:查看墙上的春联列表 4126
 */
int user_get_couplets_userid_list_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	
	uint32_t page;
	int i = 0;
	UNPKG_UINT32(buf, page, i);

	if (page > 100) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	*(uint32_t*)p->session = page;
	return send_request_to_db(SVR_PROTO_USER_GET_COUPLETS_USERID_LIST, p, sizeof(page), &page, p->id);
}

int user_get_couplets_userid_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t all_page;
		uint32_t all_cnt;
	}__attribute__((packed)) couplets_userid_list_cbk_head_t;
	typedef struct {
		uint32_t userid;
		char nick[USER_NICK_LEN];
		//uint32_t couplets_id;
	}__attribute__((packed)) couplets_userid_list_cbk_pack_t;
	CHECK_BODY_LEN_GE(len, sizeof(couplets_userid_list_cbk_head_t));
	couplets_userid_list_cbk_head_t* cbk_head = (couplets_userid_list_cbk_head_t*)buf;

	CHECK_BODY_LEN(len, sizeof(couplets_userid_list_cbk_head_t) + cbk_head->all_cnt * sizeof(couplets_userid_list_cbk_pack_t));
	couplets_userid_list_cbk_pack_t* p_cbk_pack = (couplets_userid_list_cbk_pack_t*)(buf + sizeof(couplets_userid_list_cbk_head_t));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_head->all_page, l);
	PKG_UINT32(msg, *(uint32_t*)p->session, l);
	PKG_UINT32(msg, cbk_head->all_cnt, l);

	int idx = 0;
	for (idx = 0; idx < cbk_head->all_cnt; idx++) {
		PKG_UINT32(msg, (p_cbk_pack + idx)->userid, l);
		PKG_STR(msg, (p_cbk_pack + idx)->nick, l, USER_NICK_LEN);
		//PKG_UINT32(msg, (p_cbk_pack + idx)->couplets_id, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 春联魔法墙:贴了春联后许愿 4128
 */
int user_set_couplets_prize_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	typedef struct{
		uint32_t prize_id;
	}__attribute__((packed))couplets_prize_package_t;

	couplets_prize_package_t package = {0};
	int i = 0;
	UNPKG_UINT32(buf, package.prize_id, i);
	const uint32_t prize_list[] = {13195, 13196, 13194, 13201, 160178, 160179};
	int idx = 0;
	for (idx = 0; idx < sizeof(prize_list) / sizeof(uint32_t); idx++) {
		if (package.prize_id == prize_list[idx]) {
			break;
		}
	}
	if (idx == sizeof(prize_list) / sizeof(uint32_t) ) {//非法的奖励ID
		return send_state_to_self(p, 3);
	}
	
	return send_request_to_db(SVR_PROTO_USER_SET_COUPLETS_PRIZE, p, sizeof(package), &package, p->id);
}

int user_set_couplets_prize_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t ret_state = *(uint32_t*)buf;

	return send_state_to_self(p, ret_state);
}

/*
 * @brief 春联魔法墙:领取礼物 4127
 */
int user_get_couplets_prize_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	
	return send_request_to_db(SVR_PROTO_USER_GET_COUPLETS_PRIZE, p, 0, NULL, p->id);
}

int user_get_couplets_prize_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t prize_id;
	}__attribute__((packed))couplets_prize_cbk_pack;
	
	CHECK_BODY_LEN(len, sizeof(couplets_prize_cbk_pack));
	couplets_prize_cbk_pack* cbk_pack = (couplets_prize_cbk_pack*)buf;
	
	uint32_t ret_state = 0;
	//const uint32_t prize_list[] = {13195, 13196, 13194, 13201, 160178, 160179};
	uint32_t prize_id = cbk_pack->prize_id;
	switch (cbk_pack->prize_id) {
	case 0:	//没有许愿
		ret_state = 1;
		break;
	case 1:	//已经领取了
		ret_state = 2;
		break;
	case 13195:
		prize_id = 14105;
		break;
	case 13196:
		prize_id = 14106;
		break;
	case 13194:
		prize_id = 14107;
		break;
	case 13201:
		prize_id = 14108;
		break;
	case 160178:
	case 160179:
		break;
	default:
		ret_state = 1;
		break;
	}
	
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_state, l);
	if (ret_state != 0) {
		PKG_UINT32(msg, 0, l);
	} else {
		PKG_UINT32(msg, 1, l);
		PKG_UINT32(msg, prize_id, l);
		PKG_UINT32(msg, 1, l);

		int i = 0;
		uint8_t db_buf[256] = {0};
		PKG_H_UINT32(db_buf, 0, i);
        PKG_H_UINT32(db_buf, 1, i);
        PKG_H_UINT32(db_buf, 0, i);
        PKG_H_UINT32(db_buf, 0, i);
		if (pkg_item_kind(p, db_buf, prize_id, &i) != -1) {
			PKG_H_UINT32(db_buf, prize_id, i);
			PKG_H_UINT32(db_buf, 1, i);
			item_t* item_u = get_item_prop(prize_id);
			if (item_u) {
				PKG_H_UINT32(db_buf, item_u->max, i);
				send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, i, db_buf, p->id);
			}
		}
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}
