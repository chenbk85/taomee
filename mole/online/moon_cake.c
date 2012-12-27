#include "item.h"
#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "message.h"
#include "moon_cake.h"

/*
 * @brief 吃月饼 4100
 */
int user_eat_mooncake_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	uint32_t daytype = 50002;
	*(uint32_t*)p->session = daytype;

	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &daytype, p->id);
}

int user_get_eat_mooncake_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t eat_cnt = *(uint32_t*)buf;
	uint32_t daytype = *(uint32_t*)p->session;

	//如果是在每天的20-21点间，还需要查出50003的数量
	if (daytype == 50002 && (get_now_tm()->tm_hour >= 20 && get_now_tm()->tm_hour < 21)) {
		daytype = 50003;
		*(uint32_t*)p->session = daytype;
		*(uint32_t*)(p->session + 4) = eat_cnt;
		return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &daytype, p->id);
	}

	uint32_t item_id = 0;
	if (daytype == 50003) {
		uint32_t eat_cnt_1 = *(uint32_t*)(p->session + 4);//50002的数量
		if (eat_cnt_1 < 100 && eat_cnt == 30){
			item_id = 0;
		} else if (eat_cnt_1 == 100 && eat_cnt < 30) {
			item_id = 190886;
		} else if (eat_cnt_1 < 100 && eat_cnt < 30){
			item_id = (rand() % 2) ? 0 : 190886;
		} else {
			int l = sizeof(protocol_t);
			PKG_UINT32(msg, 0, l);
			PKG_UINT32(msg, 0, l);
			PKG_UINT32(msg, 0, l);
			init_proto_head(msg, p->waitcmd, l);
			return send_to_self(p, msg, l, 1);
		}

		if (item_id == 0) {//如果itemid为0，DB根据mooncake_type来给月饼
			eat_cnt = eat_cnt_1;
			*(uint32_t*)p->session = 50002;
		}
	}
	if (daytype == 50002) {
		if (eat_cnt >= 100) {
			int l = sizeof(protocol_t);
			PKG_UINT32(msg, 0, l);
			PKG_UINT32(msg, 0, l);
			PKG_UINT32(msg, 0, l);
			init_proto_head(msg, p->waitcmd, l);
			return send_to_self(p, msg, l, 1);
		}
	}

	uint32_t db_buff[] = {eat_cnt, item_id};
	return send_request_to_db(SVR_PROTO_USER_EAT_MOONCAKE, p, 8, &db_buff, p->id);
}

/*
 * @brief 吃月饼 callback
 */
int user_eat_mooncake_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t left_cnt;
		uint32_t item_id;
		uint32_t count;
	}__attribute__((packed)) eat_mooncake_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(eat_mooncake_cbk_pack_t));
	eat_mooncake_cbk_pack_t* cbk_pack = (eat_mooncake_cbk_pack_t*)buf;

	uint32_t daytype = *(uint32_t*)p->session;
	uint32_t limit_cnt = (daytype == 50002) ? 100 : 30;

	if (cbk_pack->left_cnt <= limit_cnt) {
		int32_t db_buff[] = {daytype, limit_cnt, 1};
		send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, 12, db_buff, p->id);

		uint32_t msgbuf[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409B47C, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
	}
	cbk_pack->left_cnt = limit_cnt - cbk_pack->left_cnt;
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->left_cnt, l);
	PKG_UINT32(msg, cbk_pack->item_id, l);
	PKG_UINT32(msg, cbk_pack->count, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

/*
 * @brief 查询对方可以交换的月饼 4101
 */
int user_get_mooncake_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t uid = 0;
	int i = 0;
	UNPKG_UINT32(buf, uid, i);
	CHECK_VALID_ID(uid);

//	DEBUG_LOG("get other user mooncake: uid[%u]", uid);
	if (uid != p->id) {
		sprite_t *s = get_sprite(uid);
		if (s == NULL) {
			return send_to_self_error(p, p->waitcmd, -ERR_swap_mooncake_state_err, 1);
		}
	}
	*(uint32_t*)p->session = uid;
	return send_request_to_db(SVR_PROTO_USER_GET_MOONCAKE, p, 0, NULL, uid);
}

/*
 * @brief 查询对方可以交换的月饼 callback
 */
int user_get_mooncake_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(get_mooncake_cbk_pack_t));
	get_mooncake_cbk_pack_t* cbk_pack = (get_mooncake_cbk_pack_t*)buf;

	switch (p->waitcmd) {
	case PROTO_USER_GET_MOONCAKE:
		{
			int l = sizeof(protocol_t);
			PKG_UINT32(msg, *(uint32_t*)p->session, l);
			PKG_UINT32(msg, cbk_pack->sytle_id, l);
			if (cbk_pack->sytle_id != 0) {
				PKG_UINT32(msg, cbk_pack->item_id, l);
				PKG_UINT32(msg, cbk_pack->count, l);
			}
			init_proto_head(msg, p->waitcmd, l);
			return send_to_self(p, msg, l, 1);
		}
	case PROTO_USER_SWAP_MOONCAKE:
		if (*(uint32_t*)(p->session + 8) == PROTO_USER_SWAP_MOONCAKE) {
			return get_swap_mooncake_callback1(p, cbk_pack);
		} else {
			return get_swap_mooncake_callback2(p, cbk_pack);
		}
	default:
		break;
	}
	return 0;
}

/*
 * @brief 与对方交换月饼 4102
 */
int user_swap_mooncake_cmd(sprite_t *p, const uint8_t *buf, int len)
{
	CHECK_BODY_LEN(len, 8);

	uint32_t uid;
	uint32_t swap_cnt;
	int i = 0;
	UNPKG_UINT32(buf, uid, i);
	UNPKG_UINT32(buf, swap_cnt, i);

	CHECK_VALID_ID(uid);
	DEBUG_LOG("user_swap_mooncake_cmd: mid[%u] uid[%u] count[%u]", p->id, uid, swap_cnt);

	sprite_t* s = get_sprite(uid);
	if (uid == p->id || s == NULL) {
		return send_to_self_error(p, p->waitcmd, -ERR_swap_mooncake_state_err, 1);
	}

	int8_t idx = 0;
	for (idx = 0; idx < s->item_cnt && s->items[idx] != 13961; idx++);
	if (idx == s->item_cnt /*|| s->action != 3*/) {	//身上没有月饼装或者没有坐下来
		ERROR_LOG("user swap mooncake item err: uid[%u] idx[%u]", uid, idx);
		return send_to_self_error(p, p->waitcmd, -ERR_swap_mooncake_state_err, 1);
	}

//	DEBUG_LOG("begin swap mooncake: mid[%u], uid[%u], count[%u]", p->id, uid, swap_cnt);
	*(uint32_t*)p->session = uid;
	*(uint32_t*)(p->session + 4) = swap_cnt;
	*(uint32_t*)(p->session + 8) = PROTO_USER_SWAP_MOONCAKE;
	return send_request_to_db(SVR_PROTO_USER_GET_MOONCAKE, p, 0, NULL, uid);
}

/*
 * @brief 获得对方的月饼数量 callback
 */
int get_swap_mooncake_callback1(sprite_t* p, get_mooncake_cbk_pack_t* cbk_pack)
{
	if (cbk_pack->sytle_id == 0 || cbk_pack->sytle_id > 3) {	//判断是否吃过月饼
		return send_to_self_error(p, p->waitcmd, -ERR_swap_mooncake_not_enough, 1);
	}
	uint32_t swap_cnt = *(uint32_t*)(p->session + 4);
	if (swap_cnt > cbk_pack->count) {	//判断好友要兑换的月饼数量是否足够
		ERROR_LOG("user swap mooncake not enough: mid[%u] uid[%u] count[%u]", p->id, *(uint32_t*)p->session, swap_cnt);
		return send_to_self_error(p, p->waitcmd, -ERR_swap_mooncake_not_enough, 1);
	}
//	DEBUG_LOG("swap mooncake cbk1: mid[%u], itmid[%u], itmcnt[%u]", p->id, cbk_pack->item_id, cbk_pack->count);
	*(uint32_t*)(p->session + 8) = cbk_pack->item_id;
	return send_request_to_db(SVR_PROTO_USER_GET_MOONCAKE, p, 0, NULL, p->id);
}

static int db_swap_mooncake(uint32_t id, uint32_t item_in, uint32_t item_out, uint32_t count)
{
	uint32_t buf[] = {1, 1, 0, 0, 0, item_in, count, 0, item_out, count, 99999};
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(buf), buf, id);
}

/*
 * @brief 获得自己的月饼数量 callback
 */
int get_swap_mooncake_callback2(sprite_t* p, get_mooncake_cbk_pack_t* cbk_pack)
{
	if (cbk_pack->sytle_id == 0 || cbk_pack->sytle_id > 3) {	//判断是否吃过月饼
		return send_to_self_error(p, p->waitcmd, -ERR_swap_mooncake_not_enough, 1);
	}
	uint32_t uid = *(uint32_t*)p->session;
	uint32_t swap_cnt = *(uint32_t*)(p->session + 4);
	uint32_t swap_item = *(uint32_t*)(p->session + 8);
//	DEBUG_LOG("swap mooncake cbk2: mid[%u], uid[%u], itmid[%u], itmcnt[%u]", p->id, uid, cbk_pack->item_id, cbk_pack->count);

	if (swap_cnt > cbk_pack->count || swap_item == cbk_pack->item_id) {	//判断自己要兑换的月饼数量是否足够
		return send_to_self_error(p, p->waitcmd, -ERR_swap_mooncake_not_enough, 1);
	}

	db_swap_mooncake(p->id, cbk_pack->item_id, swap_item, swap_cnt);
	db_swap_mooncake(uid, swap_item, cbk_pack->item_id, swap_cnt);

	uint32_t msgbuf[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B480, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
	msgbuf[0] = uid;
	msglog(statistic_logfile, 0x0409B480, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, 1, l);
	PKG_UINT32(msg, swap_item, l);
	PKG_UINT32(msg, swap_cnt, l);
	init_proto_head(msg, p->waitcmd, l);
	send_to_self(p, msg, l, 1);

	sprite_t* s = get_sprite(uid);
	if (uid == p->id || s == NULL) {
		//return send_to_self_error(p, p->waitcmd, -ERR_swap_mooncake_state_err, 1);
		return 0;
	}
	l = sizeof(protocol_t);
	PKG_UINT32(msg, 1, l);
	PKG_UINT32(msg, cbk_pack->item_id, l);
	PKG_UINT32(msg, swap_cnt, l);
	return send_to_self(s, msg, l, 0);
}
