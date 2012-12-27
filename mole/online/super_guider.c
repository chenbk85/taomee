/*
 * =====================================================================================
 *
 *       Filename:  super_mogu.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/08/2011 11:12:38 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "util.h"
#include "sprite.h"
#include "proto.h"
#include "dbproxy.h"
#include "exclu_things.h"
#include "super_guider.h"

//#define DEBUG_TEST_VER

int find_idle_super_guider_id(uint32_t *userid)
{
#ifndef DEBUG_TEST_VER
	sprite_t *p;
	uint32_t patrol_id[500] = {0};
	uint32_t patrol_cnt = 0;
	uint32_t idle_id[500] = {0};
	uint32_t idle_cnt = 0;

	uint32_t i =0;
    for (i = 0; i < HASH_SLOT_NUM; i++) {
		list_for_each_entry (p, &idslots[i], hash_list) {
			if (p) {
				if (p->profession[12] == 1) {
					if (p->sg_guidering_flag == 1) {
						patrol_id[patrol_cnt++] = p->id;
					} else {
						idle_id[idle_cnt++] = p->id;
					}
				}
			}
		}
	}
#else
	uint32_t patrol_id[500] = {50447};
	uint32_t patrol_cnt = 1;
	uint32_t idle_id[500] = {0};
	uint32_t idle_cnt = 0;
#endif

	*userid = 0;
	if (patrol_cnt != 0) {
		*userid = patrol_id[rand() % patrol_cnt];
	} else {
		if (idle_cnt != 0) {
			*userid = idle_id[rand() % idle_cnt];
		}
	}


    return *userid;
}

/*
 * @brief 主动通知场景，有人接受或完成巡逻任务
 * @prama type: type = 1接受任务，type=2完成任务
 */
int send_to_map_change_clothes(sprite_t* s, uint32_t type)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg,type,i);
	PKG_UINT32(msg,s->id,i);

	init_proto_head(msg, PROTO_SG_USER_CHANGE_GUIDER_STATE, i);
	send_to_map(s, msg, i, 0);
	return 0;
}

/*
 * @brief 巡逻任务接取 4113
 */
int sg_get_patrol_tast_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t type = 0;	//0查询 1接取 2完成
	int i = 0;
	UNPKG_UINT32(body, type, i);

	if (type > 2) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	if (p->profession[12] != 1) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_super_guider, 1);
	}
	uint32_t daytype = 50008;
	*(uint32_t*)p->session = type;
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &daytype, p->id);
}

int sg_get_patrol_tast_callback(sprite_t * p, uint32_t id, char * buf, int len)
{
	uint32_t value = *(uint32_t*)buf;
	uint32_t type = *(uint32_t*)p->session;
	uint32_t ret_state = 0;	//1未接 2已接 3可完成 4已完成
	uint32_t ret_code = 0;	//1接取失败 2完成失败
	uint32_t now_time = now.tv_sec;
	uint32_t daytype = 50008;

	if (value == 0) {	//未接
		ret_state = 1;
		if (type == 1) {	//接取
			ret_state = 2;
			if (p->sg_guidering_flag == 0) {	//接取任务时，要穿上巡逻装
				p->sg_guidering_flag = 1;
				send_to_map_change_clothes(p, 1);
				uint32_t msgbuf[] = {p->id, 1};
				msglog(statistic_logfile, 0x0409BDAA, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
			}
			db_set_sth_done_cnt(p, daytype, now_time, now_time, p->id);
		} else if (type == 2) {
			ret_code = 2;
		}
	} else if(value == 1) {	//已完成
		ret_state = 4;
		if (type == 1) {
			ret_code = 1;
		} else {
			ret_code = 2;
		}
	} else {	//正在进行中
		ret_state = 2;
		if (now_time - value >= 550) {	//已接可完成
			ret_state = 3;
			if (p->sg_guidering_flag == 1) {	//可完成状态下，要换回原来的衣服
				p->sg_guidering_flag = 0;
				send_to_map_change_clothes(p, 2);
			}
			
			if (type == 1) {
				ret_code = 1;
			} else if (type == 2) {
				ret_state = 4;
				uint32_t db_buf[2] = {daytype, 1};
				send_request_to_db(SVR_PROTO_SET_DAY_COUNT, p, sizeof(db_buf), db_buf, p->id);

				uint32_t buf[] = {0, 1, 0, 0, 99, 1351131, 1, 99999};
				send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(buf), buf, p->id);
			}
		} else {	//已接不可完成
			if (type == 1) {
				ret_code = 1;
			} else if (type == 2) {
				ret_code = 2;
			}
		}
	}
	
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, type, l);
	PKG_UINT32(msg, ret_state, l);
	PKG_UINT32(msg, ret_code, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 小摩尔呼叫寻求帮助 4114
 */
int sg_user_ask_for_help_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	uint32_t guider_id = 0;
	find_idle_super_guider_id(&guider_id);
	DEBUG_LOG("find super guider ID: [%u]", guider_id);

	int l = 0;
	int ret_state = 1;
	if (guider_id != 0) {
		sprite_t* s = get_sprite(guider_id);
		if (s != NULL) {
			s->sg_need_help_id = p->id;
			l = sizeof(protocol_t);
			PKG_UINT32(msg, p->id, l);
			PKG_STR(msg, p->nick, l, 16);
			PKG_MAP_ID(msg, p->tiles->id, l);
			DEBUG_LOG("send msg to super guider ID:[%u] nick[%s]", guider_id, p->nick);
			init_proto_head(msg, p->waitcmd, l);
			send_to_self(s, msg, l, 0);
		}
	}

	uint32_t msgbuf[] = {p->id, 1};
	msglog(statistic_logfile, 0x0409BD9D, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
	l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_state, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @breif 超级蘑菇向导回应小摩尔寻求帮助 4115
 */
int sg_guider_help_others_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t accept = 0;
	int i = 0;
	UNPKG_UINT32(body, accept, i);
	if (accept == 0 || accept > 2) {

	}
	int l = 0;
	if (p->sg_need_help_id != 0) {
		sprite_t* s = get_sprite(p->sg_need_help_id);
		if (s != NULL) {
			l = sizeof(protocol_t);
			PKG_UINT32(msg, p->id, l);
			PKG_UINT32(msg, accept, l);
			init_proto_head(msg, p->waitcmd, l);
			send_to_self(s, msg, l, 0);
		}
	}
	
	if (accept == 1) {
		uint32_t msgbuf[] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BD9E, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
	}

	p->sg_need_help_id = 0;
	int ret_state = 1;
	l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_state, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 获得成就奖励 4116
 */
int sg_get_guider_prize_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t prize_idx = 0;
	int i = 0;
	UNPKG_UINT32(body, prize_idx, i);
	if (prize_idx ==0 || prize_idx > 5) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	if (p->profession[12] != 1) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_super_guider, 1);
	}

	return send_request_to_db(SVR_PROTO_GET_GUIDER_PRIZE, p, 4, &prize_idx, p->id);
}

int sg_get_guider_prize_callback(sprite_t * p, uint32_t id, char * buf, int len)
{
	typedef struct {
		uint32_t flag;
	}__attribute__((packed)) get_prize_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(get_prize_cbk_pack_t));
	get_prize_cbk_pack_t* cbk_pack = (get_prize_cbk_pack_t*)buf;

	if (cbk_pack->flag == 1) {
		return send_to_self_error(p, p->waitcmd, -ERR_sg_not_enough_madel_count, 1);
	} else if (cbk_pack->flag == 2) {
		return send_to_self_error(p, p->waitcmd, -ERR_sg_has_got_the_prize, 1);
	}
	int l = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 查询获得的成就奖励 4117
 */
int sg_look_guider_prize_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	if (p->profession[12] != 1) {
		//return send_to_self_error(p, p->waitcmd, -ERR_not_super_guider, 1);
	}
	return send_request_to_db(SVR_PROTO_LOOK_GUIDER_PRIZE, p, 0, NULL, p->id);
}

int sg_look_guider_prize_callback(sprite_t * p, uint32_t id, char * buf, int len)
{
	typedef struct {
		uint32_t flag;
	}__attribute__((packed)) look_prize_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(look_prize_cbk_pack_t));
	look_prize_cbk_pack_t* cbk_pack = (look_prize_cbk_pack_t*)buf;

	//DEBUG_LOG("sg_look_guider_prize_callback: value[%u]", cbk_pack->flag);
	int l = sizeof(protocol_t);
	int i = 0;
	for (i = 0; i < 5; i++) {
		int flag = 0;
		if ((1<<i)&cbk_pack->flag) {
			flag = 1;
		}
	//	DEBUG_LOG("sg_look_guider_prize_callback:idx[%u] flag[%u]", i+ 1, flag);
		PKG_UINT32(msg, flag, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}
