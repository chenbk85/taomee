#include <stdlib.h>

#include <libtaomee/bitmanip/bitmanip.h>
#include <libtaomee/utils.h>

#include "exclu_things.h"
#include "logic.h"
#include "login.h"
#include "message.h"
#include "proto.h"

#include "hallowmas.h"
#include "mole_car.h"
#include "small_require.h"
#include "communicator.h"

uint32_t pumpkin_num_3 = 0;
uint32_t uids_3[2048] = {0};
uint32_t pumpkin_num_1 = 0;
uint32_t uids_1[2048] = {0};
uint32_t pumpkin_num_2 = 0;
uint32_t uids_2[2048] = {0};

uint32_t black_cat_status = 0;

int do_become_pumpkin(map_t* mp)
{
	uint32_t itmid = PUMPKIN; // only for hallowmas
	uint32_t* pumpkin_num;
	uint32_t* uids;

	if (mp->id == 1) {
		pumpkin_num = &pumpkin_num_1;
		uids = uids_1;
	} else if(mp->id == 2) {
		pumpkin_num = &pumpkin_num_2;
		uids = uids_2;
	} else if(mp->id == 3) {
		pumpkin_num = &pumpkin_num_3;
		uids = uids_3;
	}

	*pumpkin_num = 0;
	int head_len = sizeof(protocol_t);
	int i = head_len + 4;
	list_head_t* p;
	list_for_each(p, &mp->sprite_list_head) {
		sprite_t* l = list_entry(p, sprite_t, map_list);
		if (!IS_GUEST_ID(l->id) && !IS_NPC_ID(l->id) && *pumpkin_num < 2048) {
			l->action = itmid;
			PKG_UINT32(msg, l->id, i);
			uids[(*pumpkin_num)++] = l->id;
		}
	}
	PKG_UINT32(msg, *pumpkin_num, head_len);
	init_proto_head(msg, PROTO_BECOME_PUMPKIN, i);
	send_to_map2(mp, msg, i);

	ADD_TIMER_EVENT(mp, become_pumpkin_expeired, 0, now.tv_sec + 15);
	return 0;
}

int do_become_pumpkin_expired(map_t* mp)
{
	uint32_t itmid = 0; // only for hallowmas
	uint32_t* pumpkin_num;
	uint32_t* uids;

	DEBUG_LOG("PUMPKIN EXPIRED\t[map=%lu]", mp->id);
	if (mp->id == 1) {
		pumpkin_num = &pumpkin_num_1;
		uids = uids_1;
	} else if(mp->id == 2) {
		pumpkin_num = &pumpkin_num_2;
		uids = uids_2;
	} else if(mp->id == 3) {
		pumpkin_num = &pumpkin_num_3;
		uids = uids_3;
	}
	else
	{
		return 0;
	}

	int loop;
	for (loop = 0; loop < *pumpkin_num; loop++) {
		sprite_t* p = get_sprite(uids[loop]);
		if (p) {
			p->action = itmid;
			if (p->tiles) {
				response_proto_uint32_map(p->tiles->id, PROTO_BECOME_NANGUA_EXPIRE, p->id);
				DEBUG_LOG("PUMPKIN EXPIRED\t[map=%lu uid=%u]", mp->id, p->id);
			}
		}
	}

	memset(uids, 0, sizeof(uids));
	*pumpkin_num = 0;
	return 0;
}

int become_pumpkin_expeired(void* owner, void* data)
{
	map_t* mp = owner;

	DEBUG_LOG("PUMPKIN EXPIRED\t[mid=%lu]", mp->id);

	do_become_pumpkin_expired(mp);
	return 0;
}

int poison_gas(void* owner, void* data)
{
	map_t* mp = owner;

	DEBUG_LOG("POISON GAS\t[mid=%lu]", mp->id);

	int i = sizeof(protocol_t);
	init_proto_head(msg, PROTO_POISON_GAS, i);
	send_to_map2(mp, msg, i);

	do_become_pumpkin(mp);
	ADD_TIMER_EVENT(mp, poison_gas, 0, now.tv_sec + 40);
	return 0;
}

int get_candy_count_cmd(sprite_t* p, uint8_t* body, int len)
{
	return send_request_to_db(SVR_PROTO_GET_CANDY_COUNT, p, 0, NULL, p->id);
}

int get_candy_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t candy_num, prop_num;
	int j = 0, i = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, candy_num, j);
	PKG_UINT32(msg, candy_num, i);
	UNPKG_H_UINT32(buf, prop_num, j);
	PKG_UINT32(msg, prop_num, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

//ERR_you_already_sub_vip   ERR_vip_bu_gou_le ERR_user_not_online
int sub_candy_count_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t uid;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	sprite_t * puser = get_sprite(uid);
	if (!puser && p->tiles) {
		puser = get_across_svr_sprite(uid, p->tiles->id);
	}
	if (! puser) {
		return send_to_self_error(p, p->waitcmd, -ERR_user_not_online, 1);
	}
	if (!ISVIP(puser->flag)) {
		DEBUG_LOG("user not vip");
		return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
	}
	*(uint32_t*)p->session = uid;
	uint32_t tmp[] = {p->id};
	return send_request_to_db(SVR_PROTO_SUB_VIP_CANDY, p, 4, tmp, uid);
}

int sub_vip_candy_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	db_single_item_op(0, p->id, 190441, 1, 1);
	return do_sub_candy_count(p);
}

int do_sub_candy_count(sprite_t* p)
{
	uint32_t tmp[] = {1, 0, p->driver_time};
	uint32_t uid = *(uint32_t*)p->session;
	return send_request_to_db(SVR_PROTO_CANDY_PROP, p, 12, tmp, uid);
}

int exchange_prop_cmd(sprite_t* p, uint8_t* body, int len)
{
	return db_chk_itm_cnt(p,  1300006, 1300007);
}

int do_send_car_and_prop(sprite_t * p)
{
	response_proto_uint32(p, p->waitcmd, 1300006, 0);
	return 0;
}

int change_candy_prop_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t flag, prop;
	int j = 0;
	UNPKG_H_UINT32(buf, flag, j);
	UNPKG_H_UINT32(buf, prop, j);
	switch(p->waitcmd) {
		case PROTO_SUB_CANDY_COUNT:
			response_proto_head(p, p->waitcmd, 0);
			break;
		case PROTO_EXCHANGE_PROP:
		{
			DEBUG_LOG("CHANGE_CANDY\t[uid=%u  flag=%u  prop=%u]", p->id, flag, prop);
			uint32_t car_count = *(uint32_t*)p->session;
			if (flag == 1 && car_count == 0) {
				if (p->driver_time) {
					*(uint32_t*)p->session = prop;
					return db_add_car(p, 1300006, 0, p->id);
				} else {
					response_proto_uint32(p, p->waitcmd, 888888, 0);
					return 0;
				}
			}
			if (prop == 1) {
				return send_to_self_error(p, p->waitcmd, -ERR_too_few_to_exchange, 1);
			} else if (prop == 0){
				uint32_t deng[] = {160500, 160501, 160502};
				int indx = rand()%3;
				uint32_t itmid = deng[indx];
				db_buy_itm_freely(0, p->id, itmid, 1);
				response_proto_uint32(p, p->waitcmd, itmid, 0);
			} else if (prop == 2) {
				if (flag == 2)
					response_proto_uint32(p, p->waitcmd, 888888, 0); //mei jia zhao
				else
					response_proto_uint32(p, p->waitcmd, 999999, 0);  //nan na guo le
			}
			break;
		}
		default:
			ERROR_RETURN(("invalid cmd\t[uid=%u cmd=%u]", p->id, p->waitcmd), -1);
	}
	return 0;
}

int blackcat_car(void* owner, void* data)
{
	map_t* mp = owner;


	//uint32_t wek_day = get_now_tm()->tm_wday;
	uint32_t hour = get_now_tm()->tm_hour;
	if (hour != 19) {
		black_cat_status = 0;
		ADD_TIMER_EVENT(mp, blackcat_car, 0, now.tv_sec + 30);
		return 0;
	}

	DEBUG_LOG("BLACK CAT CAR\t[mid=%lu]", mp->id);
	black_cat_status++;
	if (black_cat_status == 7)
		black_cat_status = 1;

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, black_cat_status, i);
	init_proto_head(msg, PROTO_BLACK_CAT_APPEAR, i);
	send_to_map3(1, msg, i);
	send_to_map3(2, msg, i);
	send_to_map3(3, msg, i);

	ADD_TIMER_EVENT(mp, blackcat_car, 0, now.tv_sec + 5*60);
	if (black_cat_status == 2) {
		map_t* p = get_map(1);
		do_become_pumpkin(p);
	}
	if (black_cat_status == 4) {
		map_t* p = get_map(2);
		do_become_pumpkin(p);
	}
	if (black_cat_status == 6) {
		map_t* p = get_map(3);
		do_become_pumpkin(p);
	}
	return 0;
}

int get_black_car_status_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	response_proto_uint32(p, p->waitcmd, black_cat_status, 0);
	return 0;
}

int get_candy_from_other_home_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 8);

	userid_t uid;
	uint32_t candy_cnt;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	UNPKG_UINT32(body, candy_cnt, j);
	CHECK_VALID_ID(uid);
	if (candy_cnt > 5)
		ERROR_RETURN(("can't get too many candys one time\t[%u %u]", p->id, candy_cnt), -1);

	DEBUG_LOG("GET CANDY FROM[%u %u %u]", p->id, uid, candy_cnt);
	*(uint32_t*)p->session = candy_cnt;
	*(uint32_t*)(p->session + 4) = uid;
	return db_set_sth_done(p, 207, 5, p->id);
}

//PROTO_VIP_PATCH_WORK
int vip_work_patch_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	if (! ISVIP(p->flag)) {
		return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
	}
	return db_set_sth_done(p, 208, 5, p->id);
}

int do_real_send_gift(sprite_t * p)
{
	uint32_t itms[] = {180067, 180066};
	int indx = rand()%2;
	db_single_item_op(0, p->id, itms[indx], 1, 1);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, 2, i);
	PKG_UINT32(msg, 0, i);
	PKG_UINT32(msg, 200, i);
	PKG_UINT32(msg, itms[indx], i);
	PKG_UINT32(msg, 1, i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

