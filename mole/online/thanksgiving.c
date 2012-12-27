#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "exclu_things.h"
#include "login.h"
#include "map.h"
#include "small_require.h"
#include "mole_car.h"
#include "mole_class.h"
#include "mole_pasture.h"
#include "thanksgiving.h"
#include "event.h"
#include "fashion_data.h"

uint32_t pos_info[2] = {0};
uint32_t pos_cut[3] = {0};

#define BOOTH_MAX			4
#define SELL_ITM_MAX		11
#define SELL_VIP_ITM_MAX	8
booth_t gbooth[BOOTH_MAX];
uint32_t sell_itm[SELL_ITM_MAX] 		= {160665, 160666, 160667, 160704, 160705, 1220130, 1220131, 1220132, 1220133, 1220134, 1220135};
uint32_t sell_vip_itm[SELL_VIP_ITM_MAX] = {160208, 160230, 160239, 160240, 160360, 160361, 160443, 160620};

//PROTO_GET_VIP_INVATATION
int get_vip_invitation_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	if (! ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_no_super_lahm, 1);
	}
	return db_chk_itm_cnt(p, -1/*公主邀请函*/, 0);
}

int set_egg_pos_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t pos, id;
	int j = 0;
	UNPKG_UINT32(body, pos, j);
	UNPKG_UINT32(body, id, j);
	if (pos != 0 && pos != 1 && pos != 2) {
		ERROR_RETURN(("I am sorry"), -1);
	}
	if (id != 1270018 && id != 1270029) {
		ERROR_RETURN(("I am sorry"), -1);
	}
	*(uint32_t*)p->session = pos;
	uint32_t tmp[] = {pos, id};
	return send_request_to_db(SVR_PROTO_SET_DAN, p, 8, tmp, p->id);
}

int set_egg_pos_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t pos = *(uint32_t*)p->session;
	response_proto_uint32_uint32(p, p->waitcmd, 1,  pos, 0);
	return 0;
}

int process_fudan_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t uid, pos;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	UNPKG_UINT32(body, pos, j);
	if (pos != 0 && pos != 1 && pos != 2) {
		ERROR_RETURN(("I am sorry"), -1);
	}
	*(uint32_t*)p->session = pos;
	uint32_t tmp[] = {p->id, pos};
	DEBUG_LOG("process_fudan_cmd\t[uid=%u uid2=%u position=%u]", p->id, uid, pos);
	return send_request_to_db(SVR_PROTO_FUDAN, p, 8, tmp, uid);
}

int process_fudan_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	uint32_t pos = *(uint32_t*)p->session;
	response_proto_uint32(p, p->waitcmd, pos, 0);
	return 0;
}

int get_fudan_times_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t pos, uid;
	int j = 0;
	UNPKG_UINT32(body, pos, j);
	UNPKG_UINT32(body, uid, j);
	if (pos != 0 && pos != 1 && pos != 2) {
		ERROR_RETURN(("I am sorry"), -1);
	}
	uint32_t tmp[] = {pos};
	*(uint32_t*)p->session = pos;
	return send_request_to_db(SVR_PROTO_GET_FUDAN_TIMES, p, 4, tmp, uid);
}

int get_fudan_times_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t times, total, pos;
	int j = 0;
	UNPKG_H_UINT32(buf, times, j);
	UNPKG_H_UINT32(buf, total, j);
	pos = *(uint32_t*)p->session;
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, times, i);
	PKG_UINT32(msg, total-times, i);
	PKG_UINT32(msg, pos, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int cut_yang_mao_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	if (!p->animal_nbr || (p->animal_follow_info.tag_animal.itemid != 1270006 && p->animal_follow_info.tag_animal.itemid != 1270007)) {
		return send_to_self_error(p, p->waitcmd, -ERR_pls_tack_your_yang, 1);
	}
	if (p->animal_follow_info.tag_animal.outgo & 0x04) {
		return send_to_self_error(p, p->waitcmd, -ERR_already_cut, 1);
	}
	uint32_t tmp[] = {p->animal_follow_info.tag_animal.number};
	return send_request_to_db(SVR_PROTO_CUT_YANGMAO, p, 4, tmp, p->id);
}



int get_egg_only_one_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	return db_set_sth_done(p, 1000000011, 1, p->id);
}

int do_get_egg_only_one(sprite_t* p)
{
	db_single_item_op(0, p->id, 1270018/*huoji egg*/, 1, 1);
	db_single_item_op(0, p->id, 190472/*siliao*/, 10, 1);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}


int get_occupy_rabit_pos_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    int i = sizeof(protocol_t);
	int pos;
	for (pos = 0; pos < 2; pos ++) {
		PKG_UINT32(msg, pos_info[pos], i);
		sprite_t * oldp = get_sprite(pos_info[pos]);
		if (oldp) {
			PKG_UINT32(msg, oldp->animal_follow_info.tag_animal.itemid, i);
		} else {
			PKG_UINT32(msg, 0, i);
		}

	}
	init_proto_head(msg, PROTO_BROADCAST_RABIT_POS, i);
	send_to_self(p, msg, i, 1);
	return 0;

}


int broadcast_pos()
{
	int i = sizeof(protocol_t);
	int pos;
	for (pos = 0; pos < 2; pos ++) {
		PKG_UINT32(msg, pos_info[pos], i);
		sprite_t * oldp = get_sprite(pos_info[pos]);
		if (oldp && !(oldp->animal_follow_info.tag_animal.outgo & 0x08)) {
			PKG_UINT32(msg, oldp->animal_follow_info.tag_animal.itemid, i);
		} else {
			PKG_UINT32(msg, 0, i);
		}

	}
	init_proto_head(msg, PROTO_BROADCAST_RABIT_POS, i);
	send_to_map3(107, msg, i);
	return 0;
}

int clear_rabit_pos(uint32_t uid)
{
	if (pos_info[0] == uid) {
		pos_info[0] = 0;
	}
	if (pos_info[1] == uid) {
		pos_info[1] = 0;
	}
	return 0;
}

int occupy_rabit_pos_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t op, pos;
	int j = 0;
	UNPKG_UINT32(body, op, j);
	UNPKG_UINT32(body, pos, j);
	if (op != 0 && op != 1) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_op, 1);
	}
	if (pos != 0 && pos != 1) {
		return send_to_self_error(p, p->waitcmd, -ERR_candy_invaild_sit, 1);
	}
	if (p->animal_nbr == 0 || (p->animal_nbr && p->animal_follow_info.tag_animal.itemid != 1270015 && p->animal_follow_info.tag_animal.itemid != 1270016
	    && p->animal_follow_info.tag_animal.itemid != 1270017 && p->animal_follow_info.tag_animal.itemid != 1270054 && p->animal_follow_info.tag_animal.itemid != 1270058))
	{
		return send_to_self_error(p, p->waitcmd, -ERR_not_rabit, 1);
	}
	if (! p->animal_follow_info.tag_animal.mature_time) {
		return send_to_self_error(p, p->waitcmd, -ERR_rabit_too_little, 1);
	}
	DEBUG_LOG("occupy_rabit_pos_cmd\t[uid=%u op=%u]", p->id, op);
	if (op == 1) {
		*(uint32_t*)p->session = op;
		*(uint32_t*)(p->session + 4) = pos;
		return send_request_to_db(SVR_PROTO_IS_RABIT_SHENG, p, 0, NULL, p->id);
	} else if (op == 0) {
		clear_rabit_pos(p->id);
		response_proto_uint32_uint32(p, p->waitcmd, 4, 0, 0);
		broadcast_pos();
		return 0;
	}
	ERROR_RETURN(("Invalid position"), -1);
}

int check_is_sheng_rabit_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t is_sheng;
	int j = 0;
	UNPKG_H_UINT32(buf, is_sheng, j);
	DEBUG_LOG("check_is_sheng_rabit_callback\t[uid=%u is_sheng=%u]", p->id, is_sheng);
	if (! is_sheng) {
		return do_occupy_rabit_pos(p);
	} else {
		response_proto_uint32_uint32(p, p->waitcmd, 3, 0, 0);
		return 0;
	}
	return 0;
}


int is_animal_match(uint32_t itemid, uint32_t other_itemid)
{
    uint32_t tu[] = {1270015, 1270016, 1270017};
	uint32_t dog[] = {1270054, 1270058};
	if ((itemid == dog[0] && other_itemid == dog[1]) || (itemid == dog[1] && other_itemid == dog[0]))
	{
	    return 1;
	}

	if ((itemid == tu[0] && (other_itemid == tu[1] || other_itemid == tu[2]))
	    || (itemid == tu[1] && other_itemid == tu[0]) || (itemid == tu[2] && other_itemid == tu[0]) )
	{
	    return 1;
	}

	return 0;

}

int do_occupy_rabit_pos(sprite_t *p)
{
	uint32_t position = *(uint32_t*)(p->session + 4);
	if (pos_info[0] == 0 && pos_info[1] == 0) {
		pos_info[position] = p->id;
		broadcast_pos();
		response_proto_uint32_uint32(p, p->waitcmd, 0, 0, 0);
		return 0;
	}
	if (pos_info[position] != 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_candy_no_sit_ex, 1);
	}
	uint32_t uid = pos_info[(position + 1)%2];
	CHECK_VALID_ID(uid);
	sprite_t * oldp = get_sprite(uid);
	if (! oldp) {
		ERROR_RETURN(("Sorry, I must do this"), -1);
	}

	DEBUG_LOG("userid:%d,animalid:%d, old userid:%d, animalid:%d", p->id, p->animal_follow_info.tag_animal.itemid, oldp->id, oldp->animal_follow_info.tag_animal.itemid);

	if ( !is_animal_match(p->animal_follow_info.tag_animal.itemid, oldp->animal_follow_info.tag_animal.itemid))
	{
		response_proto_uint32_uint32(p, p->waitcmd, 1, 0, 0);
		return 0;
	}

	clear_rabit_pos(uid);

	uint32_t tmp1[] = {p->animal_follow_info.tag_animal.number, p->animal_follow_info.tag_animal.itemid};
	uint32_t tmp2[] = {oldp->animal_follow_info.tag_animal.number, oldp->animal_follow_info.tag_animal.itemid};
	send_request_to_db(SVR_PROTO_START_SHENG_RABIT, 0, 8, tmp1, p->id);
	send_request_to_db(SVR_PROTO_START_SHENG_RABIT, 0, 8, tmp2, oldp->id);
	oldp->animal_follow_info.tag_animal.outgo |= 0x08;
	p->animal_follow_info.tag_animal.outgo |= 0x08;

	broadcast_pos();

	uint16_t cmd = p->waitcmd;
	response_proto_uint32_uint32_not_complete(oldp, cmd, 2, p->id, 0);
	response_proto_uint32_uint32(p, cmd, 2, oldp->id, 0);

    if (p->animal_follow_info.tag_animal.itemid >= 1270015 && p->animal_follow_info.tag_animal.itemid <= 1270017)
    {
	    uint32_t msg_buff[2] = {1, p->id};
        msglog(statistic_logfile, 0x02103303,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
    }
    else if (p->animal_follow_info.tag_animal.itemid == 1270054 || p->animal_follow_info.tag_animal.itemid == 1270058)
    {
        uint32_t msg_buff[2] = {1, p->id};
        msglog(statistic_logfile, 0x02103304,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
    }

	return 0;
}

int get_little_rabit_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_GET_LITTLE_RABIT_INFO, p, 0, NULL, p->id);
}

int get_little_rabit_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count;
	int j = 0, i = sizeof(protocol_t), loop;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN_GE(len, 4 + 12*count);
	PKG_UINT32(msg, count, i);
	for (loop = 0; loop < count; loop ++) {
		uint32_t itemid, animal_id, ltime;
		UNPKG_H_UINT32(buf, itemid, j);
		UNPKG_H_UINT32(buf, animal_id, j);
		UNPKG_H_UINT32(buf, ltime, j);

		PKG_UINT32(msg, itemid, i);
		PKG_UINT32(msg, animal_id, i);
		PKG_UINT32(msg, ltime, i);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int fetch_little_rabit_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t animal_id;
	int j = 0;
	UNPKG_UINT32(body, animal_id, j);
	return send_request_to_db(SVR_PROTO_GET_LITTLE_RABIT, p, 4, &animal_id, p->id);
}

int fetch_little_rabit_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t tuid;
	int j = 0;
	UNPKG_H_UINT32(buf, tuid, j);
	p->animal_follow_info.tag_animal.outgo &= ~0x08;

    if (tuid >= 1270015 && tuid <= 1270017)
    {
	    uint32_t msg_buff[2] = {1, p->id};
        msglog(statistic_logfile, 0x02103305, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
    }
    else if (tuid == 1270054 || tuid == 1270058)
    {
        uint32_t msg_buff[2] = {1, p->id};
        msglog(statistic_logfile, 0x02103306,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
    }

	response_proto_uint32(p, p->waitcmd, tuid, 0);
	return 0;//1105
}

//0:item id; 1: geren limit; 2: total limit;  3:is-vip; 4 :partime; 5: type
uint32_t limit_thing[][6] = {
								{1270007, 5, 30000,	 1, 1, 500},
								{1270044, 1, 10000,  0, 1, 501},
							};
int buy_limited_item_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t type;
	int j = 0;
	UNPKG_UINT32(body, type, j);
	if (type >= sizeof(limit_thing)/20)
		ERROR_RETURN(("bad type"), -1);
	if (limit_thing[type][3] && !ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_no_super_lahm, 1);
	}
	*(uint32_t*)p->session = limit_thing[type][0];
	*(uint32_t*)(p->session + 4) = limit_thing[type][2];
	return db_set_sth_done(p, limit_thing[type][5], limit_thing[type][1], p->id);
}

int do_buy_limited_item(sprite_t * p)
{
	uint32_t itemid = *(uint32_t *)p->session;
	uint32_t max = *(uint32_t*)(p->session + 4);
	return db_add_animal_num(p, itemid, -1, max);
}

int buy_limited_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t itmid = *(uint32_t*)p->session;
	item_t* pitm = get_item_prop(itmid);
	if (p->yxb < pitm->price)
		return send_to_self_error(p, p->waitcmd, ERR_not_enough_xiaomee, 1);

	p->yxb -= pitm->price;
	item_kind_t*    itmkind = find_kind_of_item(itmid);
	db_buy_items(p, pitm, 1, itmkind, 0, 0);
	response_proto_uint32(p, p->waitcmd, itmid, 0);
	return 0;
}

int get_limited_item_num_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t type;
	int j = 0;
	UNPKG_UINT32(body, type, j);
	if (type >= sizeof(limit_thing)/20)
		ERROR_RETURN(("bad type"), -1);

	*(uint32_t*)p->session = limit_thing[type][0];
	return send_request_to_db(SVR_PROTO_GET_LIMIT_ITEM_NUM, p, 4, &limit_thing[type][0], 0);
}

int get_limited_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t count = *(uint32_t*)buf;
	uint32_t itmid  = *(uint32_t*)p->session;
	response_proto_uint32_uint32(p, p->waitcmd, itmid, count, 0);
	return 0;
}

int break_fire_egg_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	map_t *m = p->tiles;
	if (!m) {
		ERROR_RETURN((""), -1);
	}
	if (GET_UID_IN_PASTURE(m->id) != p->id) {
		return send_to_self_error(p, p->waitcmd, -ERR_egg_is_not_yours, 1);
	}

	uint32_t pos;
	int j = 0;
	UNPKG_UINT32(body, pos, j);
	*(uint32_t*)p->session = pos;
	return send_request_to_db(SVR_PROTO_BREAK_EGG, p, 4, &pos, p->id);
}

int break_fire_egg_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t egg_id;
	int j = 0;
	UNPKG_H_UINT32(buf, egg_id, j);
	uint32_t pos = *(uint32_t*)p->session;
	response_proto_uint32_uint32(p, p->waitcmd, pos, egg_id, 0);
	return 0;
}

void clear_cut_array(uint32_t uid)
{
	int i;
	for (i = 0; i < 3; i ++) {
		if (pos_cut[i] == uid) {
			pos_cut[i] = 0;
		}
	}
}


int cut_occupy_pos_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t op, pos;
	int j = 0;
	UNPKG_UINT32(body, op, j);
	UNPKG_UINT32(body, pos, j);
	if (op != 2 && op != 1 && op != 3) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_op, 1);
	}
	if (pos > 3) {
		return send_to_self_error(p, p->waitcmd, -ERR_candy_invaild_sit, 1);
	}
	if (op == 1) {
		if (pos_cut[pos]) {
			int i = sizeof(protocol_t);
			PKG_UINT32(msg, 1, i);
			PKG_UINT32(msg, 0, i);
			PKG_UINT32(msg, 0, i);
			PKG_UINT32(msg, op, i);
			init_proto_head(msg, p->waitcmd, i);
			return send_to_self(p, msg, i, 1);
		}
		if (!p->animal_nbr || (p->animal_nbr && p->animal_follow_info.tag_animal.itemid != 1270006 && p->animal_follow_info.tag_animal.itemid != 1270007)) {
			return send_to_self_error(p, p->waitcmd, -ERR_not_rabit, 1);
		}
		if (p->animal_follow_info.tag_animal.outgo & 0x04) {
			int i = sizeof(protocol_t);
			PKG_UINT32(msg, 0, i);
			PKG_UINT32(msg, 1, i);
			PKG_UINT32(msg, p->animal_follow_info.tag_animal.itemid, i);
			PKG_UINT32(msg, op, i);
			init_proto_head(msg, p->waitcmd, i);
			return send_to_self(p, msg, i, 1);
		}
		pos_cut[pos] = p->id;
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, p->animal_follow_info.tag_animal.itemid, i);
		PKG_UINT32(msg, op, i);
		init_proto_head(msg, p->waitcmd, i);
		return send_to_self(p, msg, i, 1);
	 } else if (op == 2){
	 	clear_cut_array(p->id);
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, 0, i);
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, p->animal_follow_info.tag_animal.itemid, i);
		PKG_UINT32(msg, op, i);
		init_proto_head(msg, p->waitcmd, i);
		return send_to_self(p, msg, i, 1);
	} else {
		clear_cut_array(p->id);
		uint32_t tmp[] = {p->animal_follow_info.tag_animal.number};
		*(uint32_t*)p->session = op;
		return send_request_to_db(SVR_PROTO_CUT_YANGMAO, p, 4, tmp, p->id);
	}
	ERROR_RETURN(("TOO SORRY"), -1);
}

int cut_yangmao_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	p->animal_follow_info.tag_animal.outgo |= 0x04;
	db_single_item_op(0, p->id, 190474, 1, 1);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_cut_position_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, pos_cut[0], i);
	PKG_UINT32(msg, pos_cut[1], i);
	PKG_UINT32(msg, pos_cut[2], i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

//PROTO_BEAUTY_COMPETE
int beauty_dress_compete_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
 	int tm_wday = get_now_tm()->tm_wday;
	//if ((get_now_tm()->tm_wday != 0) && (get_now_tm()->tm_wday != 6)) {
   if(!is_holiday(tm_wday)) {
		p->waitcmd = 0;
		ERROR_RETURN(("dress compete not in time %u %d", p->id, get_now_tm()->tm_wday), 0);
	}
	return db_set_sth_done(p, 211, 8, p->id);
}

int do_beauty_dress_compete(sprite_t *p)
{
	int i = 0, loop;
	PKG_H_UINT32(msg, p->item_cnt, i);
	for (loop = 0; loop < p->item_cnt; loop ++) {
		PKG_H_UINT32(msg, p->items[loop], i);
	}
	for (; loop < 12; loop ++) {
		PKG_H_UINT32(msg, 0, i);
	}
	//
	return send_request_to_db(SVR_PROTO_GET_SCORE, p, i, msg, p->id);
	//return do_beauty_dress_compete_ex(p);
}

void check_win_fashion_prize(sprite_t* p,uint32_t value)
{
	typedef struct {
		uint32_t userid;
	} fashion_stat_t;

	if(p->fashion >= 25000 && p->fashion - value < 25000 ) {
		fashion_stat_t buff;
		db_single_item_op(0, p->id, 13566, 1, 1);
		buff.userid=p->id;
		msglog(statistic_logfile, 0x02100700, now.tv_sec, &buff, sizeof(buff));
#ifndef TW_VER
		send_postcard("丝尔特",0,p->id,1000157,"你现在已经成为庄园的时尚达人了，这根象征着时尚界权威的摩登权杖就作为奖励送给你啦。",0);
#else
		send_postcard("絲爾特",0,p->id,1000157,"你現在已經成為莊園的時尚達人了，這根象徵著時尚界權威的摩登權杖就作為獎勵送給你啦。",0);
#endif
	}
}

//#define ROOMINFO_ROOMINFO_DRESSING_MARK_GET_CMD (0xB458)
int beauty_dress_compete_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);
	uint32_t mark1, mark2, mark3;
	int j = 0;
	UNPKG_H_UINT32(buf, mark1, j);
	if (mark1) {
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, mark1, i);
		UNPKG_H_UINT32(buf, mark2, j);
		PKG_UINT32(msg, mark2, i);
		UNPKG_H_UINT32(buf, mark3, j);
		PKG_UINT32(msg, mark3, i);
		PKG_UINT32(msg, 0, i);

		uint32_t value = 5;
		send_request_to_db(SVR_PROTO_SET_FASHION, 0, 4, &value, p->id);
		uint32_t old_level = calculation_level_from_fashion(p->fashion);
		p->fashion += value;
		check_win_fashion_prize(p, value);
		uint32_t new_level = calculation_level_from_fashion(p->fashion);
		if (new_level != old_level) {
			uint8_t buf[128] = {0};
			int l = sizeof(protocol_t);
			PKG_UINT32(buf, new_level, l);
			uint32_t itmid = 0;
			if (new_level == 3) {
				itmid = 13108;
			} else if (new_level == 7) {
				itmid = 13109;
			} else if (new_level == 10) {
				itmid = 13161;
			} else if (new_level == 14) {
				itmid = 13251;
			} else if (new_level == 20) {
				itmid = 13363;
			} else if (new_level == 25) {
				itmid = 13364;
			} else if (new_level == 30) {
				itmid = 13459;
			}
			PKG_UINT32(buf, itmid, l);
			if (itmid)
				db_single_item_op(0, p->id, itmid, 1, 1);
			init_proto_head(buf, PROTO_UP_LEVEL, l);
			send_to_self(p, buf, l, 0);
		}
		init_proto_head(msg, p->waitcmd, i);
		send_to_map(p, msg, i, 1);
		return 0;
	}
	return do_beauty_dress_compete_ex(p);
}


int do_beauty_dress_compete_ex(sprite_t *p)
{
	uint32_t npc_1 = 0, npc_2 = 0, npc_3 = 0;
	uint32_t cnt = p->item_cnt;
	if (!cnt) {
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, 1, i);
		PKG_UINT32(msg, 0, i);
		init_proto_head(msg, p->waitcmd, i);
		send_to_map(p, msg, i, 1);
		return 0;//send_to_self(p, msg, i, 1);
	}
	uint32_t general = cnt * 3;
	uint32_t jiangli_part1 = ((cnt >= 8) ? 15 : (cnt >= 6 ? 5 : 0));
	npc_1 += general;
	npc_1 += jiangli_part1;
	npc_2 = npc_3 = npc_1;
	npc_1 += rand()%5;
	npc_2 += rand()%5;
	npc_3 += rand()%5;
	int loop, loop1, flag = 0;
	for (loop = 0; loop < cnt; loop ++) {
		uint32_t itm_id = p->items[loop];
		item_t * pitm;
		for(loop1 = 0; loop1 < itms_one[0]; loop1 ++) {
			if (itm_id == itms_one[loop1 + 1]) {
				npc_1 += 6;
				pitm = get_item_prop(itm_id);
				int layer = pitm->layer;
				if (layer == 0 || layer == 35) {
					npc_1 += 8;
				}
				if (layer == 5|| layer == 25 ||layer ==65) {
					npc_1 += 3;
				}
				break;
			}
		}
		for(loop1 = 0; loop1 < itms_two[0]; loop1 ++) {
			if (itm_id == itms_two[loop1 + 1]) {
				npc_2 += 3;
				pitm = get_item_prop(itm_id);
				int layer = pitm->layer;
				if (layer == 0) {
					npc_2 += 10;
				}
				if (layer == 5|| layer == 25 ||layer ==35 || layer == 65) {
					npc_2 += 6;
				}
				break;
			}
		}
		for(loop1 = 0; loop1 < itms_three[0]; loop1 ++) {
			if (itm_id == itms_three[loop1 + 1]) {
				npc_3 += 4;
				pitm = get_item_prop(itm_id);
				int layer = pitm->layer;
				if (layer == 35 || layer == 65) {
					npc_3 += 10;
				}
				if ( layer == 25 ||layer ==55) {
					npc_3 += 5;
				}
				break;
			}
		}
		for(loop1 = 0; loop1 < itms_zhengui[0]; loop1 ++) {
			if (itm_id == itms_zhengui[loop1 + 1]) {
				npc_1 += 8;
				npc_2 += 8;
				npc_3 += 8;
				break;
			}
		}
		if (!flag) {
			for(loop1 = 0; loop1 < itms_tuijian[0]; loop1 ++) {
				if (itm_id == itms_tuijian[loop1 + 1]) {
					flag = 1;
					//npc_1 += 8;
					//npc_2 += 8;
					//npc_3 += 8;
					break;
				}
			}
		}
	}
	for (loop = 0; loop < suit[0][0]; loop ++) {
		if (p->item_cnt < suit[loop][0])
			continue;
		int loo, leavel = 1, nothing = 0;
		for (loo = 1; loo < suit[loop][0]; loo ++) {
			int lo;
			for (lo = 0; lo < p->item_cnt; lo ++) {
				if (p->items[lo] == suit[loop][loo]) {
					leavel = 0;
					break;
				}
			}
			if (leavel) {
				nothing = 1;
				break;
			}
		}
		if (nothing == 1) {
			continue;
		}
		npc_3 += 15;
	}
	uint32_t max = npc_1;
	if (npc_2 > npc_1) {
		max = (npc_2 > npc_3) ? npc_2 : npc_3;
	} else {
		max = (npc_1 > npc_3) ? npc_1 : npc_3;
	}
	int i = 0;
	if (npc_1 > 200) {
		npc_1 =200;
	}
	if (npc_2 > 200) {
		npc_2 =200;
	}
	if (npc_3 > 200) {
		npc_3 =200;
	}
	*(uint32_t*)p->session = npc_1;
	*(uint32_t*)(p->session + 4) = npc_2;
	*(uint32_t*)(p->session + 8) = npc_3;
	*(uint32_t*)(p->session + 12) = flag;
	*(uint32_t*)(p->session + 16) = npc_1 + npc_2 + npc_3;
	uint32_t value = (npc_1 + npc_2 + npc_3) / 10;
	send_request_to_db(SVR_PROTO_SET_FASHION, 0, 4, &value, p->id);
	uint32_t old_level = calculation_level_from_fashion(p->fashion);
	p->fashion += value;
	check_win_fashion_prize(p, value);
	uint32_t new_level = calculation_level_from_fashion(p->fashion);
	if (new_level != old_level) {
		uint8_t buff[128] = {0};
		int l = sizeof(protocol_t);
		PKG_UINT32(buff, new_level, l);
		uint32_t itmid = 0;
		if (new_level == 3) {
			itmid = 13108;
		} else if (new_level == 7) {
			itmid = 13109;
		} else if (new_level == 10) {
			itmid = 13161;
		} else if (new_level == 14) {
			itmid = 13251;
		} else if (new_level == 20) {
			itmid = 13363;
		} else if (new_level == 25) {
			itmid = 13364;
		} else if (new_level == 30) {
			itmid = 13459;
		}

		PKG_UINT32(buff, itmid, l);
		if (itmid)
			db_single_item_op(0, p->id, itmid, 1, 1);
		init_proto_head(buff, PROTO_UP_LEVEL, l);
		send_to_self(p, buff, l, 0);
	}
	PKG_H_UINT32(msg, p->item_cnt, i);
	for (loop = 0; loop < p->item_cnt; loop ++) {
		PKG_H_UINT32(msg, p->items[loop], i);
	}
	for (; loop < 12; loop ++) {
		PKG_H_UINT32(msg, 0, i);
	}
	PKG_H_UINT32(msg, npc_1, i);
	PKG_H_UINT32(msg, npc_2, i);
	PKG_H_UINT32(msg, npc_3, i);
	return send_request_to_db(SVR_PROTO_UPDATE_SCORE, p, i, msg, p->id);
}

int beauty_dress_compete_update_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
		uint32_t npc_1 = *(uint32_t*)p->session;
		uint32_t npc_2 = *(uint32_t*)(p->session + 4);
		uint32_t npc_3 = *(uint32_t*)(p->session + 8);
		uint32_t flag = *(uint32_t*)(p->session + 12);
		uint32_t tatol = *(uint32_t*)(p->session + 16);
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, npc_1, i);
		PKG_UINT32(msg, npc_2, i);
		PKG_UINT32(msg, npc_3, i);

		if (0 && flag && tatol > 210 && p->daily_limit[1] == 0) {
			p->daily_limit[1] ++;
			db_set_sth_done(0, 1002, 1, p->id);
			return send_request_to_db(SVR_QUERY_MODLE_INDEX, p, 0, NULL, p->id);
		} else {
			PKG_UINT32(msg, 0, i);
			init_proto_head(msg, p->waitcmd, i);
			send_to_map(p, msg, i, 1);
			return 0;
		}
}

int show_query_model_index_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t index_item;
	int j = 0, i = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, index_item, j);

	db_single_item_op(0, p->id, index_item, 1, 1);
	uint32_t npc_1 = *(uint32_t*)p->session;
	uint32_t npc_2 = *(uint32_t*)(p->session + 4);
	uint32_t npc_3 = *(uint32_t*)(p->session + 8);
	PKG_UINT32(msg, npc_1, i);
	PKG_UINT32(msg, npc_2, i);
	PKG_UINT32(msg, npc_3, i);
	PKG_UINT32(msg, index_item, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

int do_beauty_dress_compete_update(sprite_t* p, char* buf, int len)
{

	uint32_t count;
	int j = 0, i = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, count, j);
	db_single_item_op(0, p->id, 190460 + count, 1, 1);
	uint32_t npc_1 = *(uint32_t*)p->session;
	uint32_t npc_2 = *(uint32_t*)(p->session + 4);
	uint32_t npc_3 = *(uint32_t*)(p->session + 8);
	PKG_UINT32(msg, npc_1, i);
	PKG_UINT32(msg, npc_2, i);
	PKG_UINT32(msg, npc_3, i);
	PKG_UINT32(msg, 190460 + count, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

int get_beauty_show_stat_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	response_proto_uint32(p, p->waitcmd, show_status, 0);
	//response_proto_uint32(p, p->waitcmd, 1, 0);
	return 0;
}

int get_beauty_show_score_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t delta, sub;
	uint32_t level = calculation_level_from_fashion(p->fashion);
	if (level == 1) sub = 1;
	else if(level == 2) sub = 101;
	else if(level == 3) sub = 301;
	else if(level == 4) sub = 601;
	else if(level == 5) sub = 901;
	else if(level == 6) sub = 1301;
	else if(level == 7) sub = 1601;
	else if(level == 8) sub = 1901;
	else if(level == 9) sub = 2401;
	else if(level == 10) sub = 2801;
	else if(level == 11) sub = 3401;
	else if(level == 12) sub = 4001;
	else if(level == 13) sub = 5001;
	else if(level == 14) sub = 5501;
	else if(level == 15) sub = 6301;
	else if(level == 16) sub = 7101;
	else if(level == 17) sub = 8401;
	else if(level == 18) sub = 9001;
	else if(level == 19) sub = 10001;
	else if(level == 20) sub = 11001;
	else if(level == 21) sub = 12601;
	else if(level == 22) sub = 13601;
	else if(level == 23) sub = 15001;
	else if(level == 24) sub = 16401;
	else if(level == 25) sub = 18401;
	else if(level == 26) sub = 19801;
	else if(level == 27) sub = 21201;
	else if(level == 28) sub = 22601;
	else if(level == 29) sub = 25000;
	else if(level == 30) sub = p->fashion;
	delta = sub - p->fashion;
	response_proto_uint32_uint32(p, p->waitcmd, level, delta, 0);
	return 0;
}

int set_model_wear_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 8);
	uint32_t model_id, model_count;
	int j = 0;
	UNPKG_UINT32(msg, model_id, j);
	UNPKG_UINT32(msg, model_count, j);
	item_t* pitm = get_item_prop(model_id);
	if (!pitm)
		ERROR_RETURN(("model not exist\t[%u %u %u]", p->id, model_id, model_count), -1);
	if (model_count >= 12) {
		ERROR_RETURN(("cannot put on too many items\t[%u %u %u]", p->id, model_id, model_count), -1);
	}

	CHECK_BODY_LEN(bodylen, 8 + model_count * 4);
	uint32_t buff[14], buff_len;
	buff[0] = model_id;
	buff[1] = model_count;
	int loop;
	for (loop = 0; loop < model_count; loop++) {
		UNPKG_UINT32(msg, buff[loop + 2], j);
	}
	buff_len = 8 + model_count * 4;

	return send_request_to_db(SVR_PROTO_SET_MODEL_WEAR, p, buff_len, buff, p->id);
}

int set_model_wear_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_model_wear_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	if (!p->tiles || (!IS_HOUSE_MAP(p->tiles->id) && !IS_HOUSE1_MAP(p->tiles->id) && !IS_HOUSE2_MAP(p->tiles->id))) {
		ERROR_RETURN(("not in home[%u]", p->id), -1);
	}
	return send_request_to_db(SVR_PROTO_GET_MODEL_WEAR, p, 0, NULL, GET_UID_IN_HOUSE(p->tiles->id));
}

int get_model_wear_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count;
	int j = 0;
	int i = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, count, j);
	PKG_UINT32(msg, count, i);

	int loop;
	uint32_t buff_len = 4;
	for (loop = 0; loop < count; loop++) {
		CHECK_BODY_LEN_GE(len, buff_len + 8);
		uint32_t model_id, cloth_count;
		UNPKG_H_UINT32(buf, model_id, j);
		UNPKG_H_UINT32(buf, cloth_count, j);
		PKG_UINT32(msg, model_id, i);
		PKG_UINT32(msg, cloth_count, i);

		CHECK_BODY_LEN_GE(len, buff_len + 8 + cloth_count * 4);
		int loop_cloth;
		for (loop_cloth = 0; loop_cloth < cloth_count; loop_cloth++) {
			uint32_t itmid;
			UNPKG_H_UINT32(buf, itmid, j);
			PKG_UINT32(msg, itmid, i);
			DEBUG_LOG("MODEL CLOTH\t[%u %u]", p->id, itmid);
		}
		buff_len += 8 + cloth_count * 4;
	}
	init_proto_head(msg, p->waitcmd, i);
	DEBUG_LOG("GET MODEL\t[%u %u]", p->id, count);
	return send_to_self(p, msg, i, 1);
}

int set_model_wear_to_mole_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	if (!p->tiles || (!IS_HOUSE_MAP(p->tiles->id) && !IS_HOUSE1_MAP(p->tiles->id) && !IS_HOUSE2_MAP(p->tiles->id))) {
		ERROR_RETURN(("not in home[%u]", p->id), -1);
	}

	uint32_t itmid;
	int j = 0;
	UNPKG_UINT32(body, itmid, j);
	item_t* pitm = get_item_prop(itmid);
	if (!pitm)
		ERROR_RETURN(("model not exist\t[%u %u]", p->id, itmid), -1);
	return send_request_to_db(SVR_PROTO_SET_MOLE_MODEL_WEAR, p, 4, &itmid, p->id);
}

int set_model_wear_to_mole_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);

	int j = 0;
	uint32_t model_id, cloth_count;
	UNPKG_H_UINT32(buf, model_id, j);
	UNPKG_H_UINT32(buf, cloth_count, j);
	if (cloth_count >= 12) {
		ERROR_LOG("too many cloth\t[%u %u]", p->id, cloth_count);
		cloth_count = 12;
	}

	int loop_cloth, k = sizeof(protocol_t);
	uint8_t tmp_buff[512];
	// broadcast to mole
	PKG_UINT32(tmp_buff, p->id, k);
	PKG_UINT32(tmp_buff, p->item_cnt, k);
	for (loop_cloth = 0; loop_cloth < p->item_cnt; loop_cloth++) {
		PKG_UINT32(tmp_buff, p->items[loop_cloth], k);
		PKG_UINT8(tmp_buff, 0, k);
		DEBUG_LOG("MODEL->MOLE CLOTH OFF\t[%u %u]", p->id, p->items[loop_cloth]);
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, model_id, i);
	PKG_UINT32(msg, cloth_count, i);
	p->item_cnt = cloth_count;
	CHECK_BODY_LEN_GE(len, 8 + cloth_count * 4);
	for (loop_cloth = 0; loop_cloth < cloth_count; loop_cloth++) {
		uint32_t itmid;
		UNPKG_H_UINT32(buf, itmid, j);
		PKG_UINT32(msg, itmid, i);

		PKG_UINT32(tmp_buff, itmid, k);
		PKG_UINT8(tmp_buff, 1, k);
		p->items[loop_cloth] = itmid;
		DEBUG_LOG("MODEL->MOLE CLOTH ON\t[%u %u]", p->id, itmid);
	}
	// mole info
	init_proto_head(tmp_buff, PROTO_USER_ITEM_USE, k);
	if (p->tiles)
		send_to_map(p, tmp_buff, k, 0);

	// model info
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int set_mole_wear_to_model_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	if (!p->tiles || (!IS_HOUSE_MAP(p->tiles->id) && !IS_HOUSE1_MAP(p->tiles->id) && !IS_HOUSE2_MAP(p->tiles->id))) {
		ERROR_RETURN(("not in home[%u]", p->id), -1);
	}

	uint32_t itmid;
	int j = 0;
	UNPKG_UINT32(body, itmid, j);
	item_t* pitm = get_item_prop(itmid);
	if (!pitm)
		ERROR_RETURN(("model not exist\t[%u %u]", p->id, itmid), -1);

	return send_request_to_db(SVR_PROTO_SET_MODEL_MOLE_WEAR, p, 4, &itmid, p->id);
}

int set_mole_wear_to_model_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	DEBUG_LOG("SET MOLE WEAR TO MODEL\t[%u]", p->id);
	if (p->tiles) {
		int loop_cloth, i = sizeof(protocol_t);
		PKG_UINT32(msg, p->id, i);
		PKG_UINT32(msg, p->item_cnt, i);
		for (loop_cloth = 0; loop_cloth < p->item_cnt; loop_cloth++) {
			PKG_UINT32(msg, p->items[loop_cloth], i);
			PKG_UINT8(msg, 0, i);
			DEBUG_LOG("MOLE->MODEL CLOTH OFF\t[%u %u]", p->id, p->items[loop_cloth]);
		}
		init_proto_head(msg, PROTO_USER_ITEM_USE, i);
		send_to_map(p, msg, i, 0);
	}

	p->item_cnt = 0;
	memset(p->items, 0, sizeof(p->items));
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

static uint32_t cakeinfo[5] = {0};
static void response_cake_info(sprite_t* p, int completed)
{
	int j = sizeof(protocol_t);
	int loop;
	for (loop = 0; loop < 5; loop++) {
		PKG_UINT32(msg, cakeinfo[loop], j);
	}
	init_proto_head(msg, PROTO_GET_CAKE_INFO, j);
	send_to_map(p, msg, j, completed);
}

int set_cake_cmd (sprite_t *p, const uint8_t *body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 8);
	int j = 0;
	uint32_t position, type;
	UNPKG_UINT32(body, position, j);
	UNPKG_UINT32(body, type, j);
	DEBUG_LOG("SET CAKE1\t[uid=%u]", p->id);

	if (position >= 5 || (type != 0 && type != 1))
		ERROR_RETURN(("pos or type error\t[%u %u %u]", p->id, position, type), -1);
	if (type == 0) {
		if (!ISVIP(p->flag)) {
			return send_to_self_error(p, p->waitcmd, -ERR_no_super_lahm, 1);
		}
		db_single_item_op(0, p->id, 190490, 1, 0);
		cakeinfo[position] = 1;
		int cake_times = 1;
		send_request_to_db(SVR_PROTO_SET_CAKE_TIMES, 0, 4, &cake_times, p->id);
	} else {
		if (cakeinfo[position] == 0) {
			return send_to_self_error(p, p->waitcmd, -ERR_this_pos_no_cake, 1);
		}
		cakeinfo[position] = 0;
	}
	response_cake_info(p, 0);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_cake_info_cmd (sprite_t *p, const uint8_t *body, int len)
{
	response_cake_info(p, 1);
	return 0;
}

int get_rabit_master_cloth_cmd (sprite_t *p, const uint8_t *body, int len)
{
	if (!ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_no_super_lahm, 1);
	}

	int cake_times = -30;
	return send_request_to_db(SVR_PROTO_SET_CAKE_TIMES, p, 4, &cake_times, p->id);
}

int get_rabit_master_cloth_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	int j = 0;
	uint32_t enough;
	UNPKG_H_UINT32(buf, enough, j);

	if (enough) {
		db_single_item_op(0, p->id, 13096, 1, 1);
		response_proto_uint32(p, p->waitcmd, 13096, 0);
	} else {
		response_proto_uint32(p, p->waitcmd, 0, 0);
	}
	return 0;
}

static int calc_yuanbao(uint32_t cnt)
{
	if (cnt == 0)
		return 0;
	else if (cnt < 5)
		return 5;
	else if (cnt < 10)
		return 7;
	else if (cnt < 15)
		return 9;
	else if (cnt < 20)
		return 11;
	else if (cnt < 25)
		return 13;
	else if (cnt < 30)
		return 15;
	else
		return 17;
}

static void pack_seller_info(uint8_t* buf, uint32_t pos, int* indx)
{
    PKG_UINT32(buf, pos, *indx);
    PKG_UINT32(buf, gbooth[pos].uid, *indx);
	PKG_UINT32(buf, gbooth[pos].selled_cnt, *indx);
	PKG_UINT32(buf, gbooth[pos].itm_selling, *indx);
	PKG_UINT32(buf, gbooth[pos].zaoxing, *indx);
}

static int notify_seller_info(sprite_t* p)
{
    int l = sizeof(protocol_t);
	PKG_UINT32(msg, BOOTH_MAX, l);
	int loop;
    for (loop = 0; loop < BOOTH_MAX; loop++) {
		pack_seller_info(msg, loop, &l);
    }

    init_proto_head(msg, PROTO_GET_ALL_SELLER_INFO, l);
    send_to_map(p, msg, l, 0);
    return 0;
}

static void notify_seller_quit(sprite_t* p, int pos, int cmd, int completed)
{
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, gbooth[pos].uid, l);
	PKG_UINT32(msg, gbooth[pos].selled_cnt, l);
	PKG_UINT32(msg, calc_yuanbao(gbooth[pos].selled_cnt), l);
    init_proto_head(msg, cmd, l);
    send_to_map(p, msg, l, completed);
}

static int remove_seller(void* owner, void* data)
{
    sprite_t *p = owner;
    if(!p) {
        return -1;
    }
	int posit = -1;
	int loop;
	for (loop = 0; loop < BOOTH_MAX; loop++) {
		if (gbooth[loop].uid == p->id) {
			uint32_t bonus_cnt = calc_yuanbao(gbooth[loop].selled_cnt);
			//db_change_xiaome(0, bonus_cnt, 0, 0, p->id);
			//p->yxb += bonus_cnt;
			db_single_item_op(0, p->id, 190602, bonus_cnt, 1);
			posit = loop;
			break;
		}
	}

    if(posit != -1) {
        notify_seller_quit(p, posit, PROTO_RMOVE_SELLER, 0);
        DEBUG_LOG("TIMER REMOVE SELLER [%d %d]", p->id, posit);
        memset(&gbooth[posit], 0, sizeof(booth_t));
    }
    return notify_seller_info(p);
}

int clean_booth(uint32_t uid)
{
	int loop;
    for (loop = 0; loop < BOOTH_MAX; loop++) {
		if (gbooth[loop].uid == uid) {
        	memset(&gbooth[loop], 0, sizeof(booth_t));
			break;
		}
    }
	return 0;
}

int occupy_booth_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t position;
    UNPKG_UINT32(body, position, i);
    if(position > (BOOTH_MAX - 1)) {
        ERROR_RETURN(("booth pos error [%d %d]", p->id, position), -1);
    }
    if(gbooth[position].uid) {
        return send_to_self_error(p, p->waitcmd, -ERR_occupied_pos, 1);
    }
	int loop;
    for (loop = 0; loop < BOOTH_MAX; loop++) {
		if (gbooth[loop].uid == p->id)
        	return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
    }

	*(uint32_t*)p->session = position;
	return db_set_sth_done(p, 214, 6, p->id);
}

int do_occupy_booth(sprite_t* p)
{
	uint32_t position = *(uint32_t*)p->session;
    gbooth[position].uid = p->id;
	gbooth[position].selled_cnt = 0;
	//if (position == 4)
		//gbooth[position].itm_selling = sell_vip_itm[rand() % SELL_VIP_ITM_MAX];
	//else
		gbooth[position].itm_selling = sell_itm[rand() % SELL_ITM_MAX];
	gbooth[position].zaoxing = rand()%3;

    gbooth[position].seller_timer = ADD_TIMER_EVENT(p, remove_seller, 0, now.tv_sec + 300);
	p->action = 3;
	p->direction = 0;

    DEBUG_LOG("SELLER SIT [%d %d]", p->id, position);
	response_proto_head(p, p->waitcmd, 0);
    return notify_seller_info(p);
}

int leave_booth_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t position;
    UNPKG_UINT32(body, position, i);
    if(position > (BOOTH_MAX - 1)) {
        ERROR_RETURN(("booth pos error [%d %d]", p->id, position), -1);
    }
	if(!gbooth[position].uid || gbooth[position].uid != p->id) {
        return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
    }

    DEBUG_LOG("SELLER LEAVE [%d %d]", p->id, position);

	uint32_t bonus_cnt = calc_yuanbao(gbooth[position].selled_cnt);
	//db_change_xiaome(0, bonus_cnt, 0, 0, p->id);
	//p->yxb += bonus_cnt;
	db_single_item_op(0, p->id, 190602, bonus_cnt, 1);
	notify_seller_quit(p, position, p->waitcmd, 1);

	REMOVE_TIMER(gbooth[position].seller_timer);
	memset(&gbooth[position], 0, sizeof(booth_t));
    return notify_seller_info(p);
}

int get_boothes_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
    DEBUG_LOG("GET BOOTHES INFO [%d]", p->id);

    int l = sizeof(protocol_t);
	PKG_UINT32(msg, BOOTH_MAX, l);
	int loop;
    for (loop = 0; loop < BOOTH_MAX; loop++) {
		pack_seller_info(msg, loop, &l);
    }

    init_proto_head(msg, p->waitcmd, l);
    return send_to_self(p, msg, l, 1);
}

int buy_furni_from_booth_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 8);
    int i = 0;
    uint32_t position, itmid;
    UNPKG_UINT32(body, position, i);
	UNPKG_UINT32(body, itmid, i);
    if(position > (BOOTH_MAX - 1)) {
        ERROR_RETURN(("booth pos error [%d %d]", p->id, position), -1);
    }
    if(!gbooth[position].itm_selling || gbooth[position].itm_selling != itmid) {
        return send_to_self_error(p, p->waitcmd, -ERR_alrdy_sell_to_other, 1);
    }

	item_t* pitm = get_item_prop(itmid);
	int money = pitm->price;
	const event_t* ev = get_event(10);
	if (ev->status) {
		item_kind_t *kind =	find_kind_of_item(itmid);
		if (kind->kind == HOME_ITEM_KIND) {
			money *= 0.5;
		}
		//money *= pitm->discount;
	}
	if (money > p->yxb) {
		return send_to_self_error(p, p->waitcmd, -ERR_not_enough_xiaomee, 1);
	}

	gbooth[position].selled_cnt++;

    do_buy_item(p, itmid, 1, 0, 0);
	p->yxb -= money;
	response_proto_uint32_uint32(p, p->waitcmd, p->yxb, itmid, 0);

	return notify_seller_info(p);
}

int set_christmas_wish_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 360);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_UPDATE_CHRISTMAS_WISH, p, bodylen, body, p->id);
}

int set_christmas_wish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_christmas_wish_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	if (!p->tiles || (!IS_JY_MAP(p->tiles->id))) {
		ERROR_RETURN(("not in JY[%u]", p->id), -1);
	}
	return send_request_to_db(SVR_PROTO_GET_CHRISTMAS_WISH, p, 0, NULL, GET_UID_IN_JY(p->tiles->id));
}

int get_christmas_wish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 360);
	response_proto_str(p, p->waitcmd, 360, buf, 0);
	return 0;
}

/* @brief old tiger get a little female tiger
 */
int get_little_tiger_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	if (!p->animal_nbr || (p->animal_follow_info.tag_animal.itemid != 1270040)) {
		return send_to_self_error(p, p->waitcmd, -ERR_pls_tack_your_tiger, 1);
	}

	if (p->animal_follow_info.tag_animal.other_attr & 0x01) {
		return send_to_self_error(p, p->waitcmd, -ERR_already_get_tiger, 1);
	}
	uint32_t db_buf[2];
	db_buf[0] = p->animal_follow_info.tag_animal.number;
	db_buf[1] = 1; /*set 1st bit*/
	return send_request_to_db(SVR_PROTO_SET_ANIMAL_FLAG, p, 8, db_buf, p->id);
}

/* @brief old tiger get a little female tiger callback
 */
int get_little_tiger_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	p->animal_follow_info.tag_animal.other_attr |= 0x01;
	db_single_item_op(0, p->id, 1270041, 1, 1);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief set yuanxiao wish
 */
int set_yuanxiao_wish_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 360);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_SET_YUANXIAO_WISH, p, bodylen, body, p->id);
}

/* @brief callback for set yuanxiao wish
 */
int set_yuanxiao_wish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @breif get yuanxiao wish
 */
int get_yuanxiao_wish_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_GET_YUANXIAO_WISH, p, 0, NULL, p->id);
}

/* @brief callback for get yuanxiao wish
 */
int get_yuanxiao_wish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 360);
	response_proto_str(p, p->waitcmd, 360, buf, 0);
	return 0;
}

/* @brief change gold with silver or chnage silver with gold
 */
int exchange_gold_and_silver_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t in_itemid = 0, in_count = 0;
	int i = 0;
	UNPKG_UINT32(body, in_itemid, i);
	UNPKG_UINT32(body, in_count, i);

	uint32_t out_itemid = 0, out_count = 0, max = 0;
	if (in_itemid == 190601) {
		out_itemid = 190602;
		out_count = in_count * 2;
		max = 99999;
	} else if (in_itemid == 190602) {
		out_itemid = 190601;
		out_count = in_count / 2;
		max = 99999;
	} else {
		ERROR_RETURN(("in_itemid is wrong %u %u", p->id, in_itemid), -1);
	}
	uint32_t db_buf[11] = {1, 1, 0, 0, 0, in_itemid, in_count, 0, out_itemid, out_count, max};
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, sizeof(db_buf), db_buf, p->id);
}

int get_the_event_recharge_months_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_GET_EVENT_RECHARGE_MONTHS, p, 0, NULL, p->id);
}

int get_the_event_recharge_months_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	struct ret_3176 {
		uint32_t months;
		uint32_t expire;
		uint32_t bonus_used;
	};

	CHECK_BODY_LEN(len, 12);

	struct ret_3176 *dat = (struct ret_3176*)buf;

	uint32_t flag = 0;
	const int bit = (56  - 1) % 32;
	if(p->only_one_flag[1] & (1 << bit)) {
		flag = 1;
	}

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg,dat->months,bytes);
	PKG_UINT32(msg,dat->expire,bytes);
	PKG_UINT32(msg,dat->bonus_used,bytes);
	PKG_UINT32(msg,flag,bytes);
	init_proto_head(msg, p->waitcmd, bytes);
	return send_to_self(p, msg, bytes, 1);
}

int set_the_event_recharge_replay_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	set_only_one_bit(p,56);
	response_proto_head(p,p->waitcmd,0);
	return 0;
}

int get_used_mibi_count_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	send_request_to_db(SVR_PROTO_GET_USED_MIBI_COUNT, p, 0, NULL, p->id);
	return 0;
}

int get_used_mibi_count_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	uint32_t mibi_cnt = 0;
	int i = 0;
	UNPKG_H_UINT32(buf, mibi_cnt, i);
	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg, mibi_cnt, bytes);
	init_proto_head(msg, p->waitcmd, bytes);
	return send_to_self(p, msg, bytes, 1);
}

static uint32_t get_bean_from_mibi(uint32_t mibi)
{
	uint32_t bean = 0;
	if (mibi == 0) {
		bean = 0;
	} else if (mibi == 20) {
		bean = 10;
	} else if (mibi == 60) {
		bean = 30;
	} else if (mibi == 70) {
		bean = 50;
	} else if (mibi == 120) {
		bean = 70;
	} else if (mibi == 130) {
		bean = 80;
	} else if (mibi == 140) {
		bean = 90;
	} else if (mibi == 150) {
		bean = 100;
	}
	return bean;
}

int exchange_beans_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	int i = 0;
	uint32_t mibi;
	UNPKG_UINT32(body, mibi, i);
	uint32_t bean = get_bean_from_mibi(mibi);
	if (bean == 0) {
		DEBUG_LOG("mibi count wrong %u", mibi);
		return send_to_self_error(p, p->waitcmd, -ERR_mibi_count, 1);
	}
	*(uint32_t*)p->session = bean;
	return send_request_to_db(SVR_PROTO_DEC_USED_MIBI, p, 4, &mibi, p->id);
}

int dec_used_mibi_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
    uint32_t mibi_cnt = 0;
	int i = 0;
	UNPKG_H_UINT32(buf, mibi_cnt, i);

	uint32_t bean = *(uint32_t*)p->session;
	uint32_t db_buf[] = {0, 1, 0, 0, 0, 16012, bean, 99999999};
	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);
	response_proto_uint32_uint32(p, p->waitcmd, mibi_cnt, bean, 0);
	return 0;
}
