
#include <libtaomee/dataformatter/bin_str.h>

#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "small_require.h"
#include "central_online.h"
#include "mole_homeland.h"
#include "mole_pasture.h"
#include "central_online.h"
#include "small_require.h"


static animal_skills_t animals_skills[ANIMALS_SKILLS_MAX];
static map_pos_t maps_poss[ANIMAL_RAIN_EGG_MAP_MAX];


#define db_get_pasture_item(p_, id) \
		send_request_to_db(SVR_PROTO_GET_PASTURE, p_, 0, NULL, id)
#define db_get_storehouse_item(p_, buf_) \
		send_request_to_db(SVR_PROTO_GET_STOREHOUSE_ITEM, p_, 8, buf_, (p_)->id)
#define db_capture_animal(p_, buf, uid_) \
		send_request_to_db(SVR_PROTO_CAPTURE_ANIMAL, p_, 12, buf, uid_)
#define db_herd_animal(p_, buf) \
		send_request_to_db(SVR_PROTO_HERD_ANIMAL, p_, 4, buf, (p_)->id)
#define db_get_pasture_animal(p_, buf_, id) \
		send_request_to_db(SVR_PROTO_PASTURE_GET_ANIMAL, p_, 4, buf_, id)
#define db_pasture_add_water(p_, uid_) \
		send_request_to_db(SVR_PROTO_PASTURE_ADD_WATER, p_, 0, NULL, uid_)
#define db_pasture_lock(p_, buf, id) \
		send_request_to_db(SVR_PROTO_PASTURE_LOCK, p_, 4, buf, id)

int get_pasture_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t id;
	if (unpkg_uint32(body, len, &id) == -1) return -1;

	CHECK_VALID_ID(id);

	return db_get_pasture_item(p, id);
}
static void response_animal_hunger(uint32_t uid)
{
	sprite_t* p = get_sprite(uid);
	if (p) {
		uint8_t buff[256];
		int j = sizeof(protocol_t);
		init_proto_head(buff, PROTO_REPORT_ANIMAL_HUNGER, j);
		send_to_self(p, buff, j, 0);
	}
}

int get_pasture_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 32);

	uint32_t animal_outgo, fish_lock, pool_state, itm_cnt, animal_count, feed_count, egg_count, insect_house;
	int j = 0;
	UNPKG_H_UINT32(buf, animal_outgo, j);
	UNPKG_H_UINT32(buf, fish_lock, j);
	UNPKG_H_UINT32(buf, pool_state, j);
	UNPKG_H_UINT32(buf, insect_house, j);
	UNPKG_H_UINT32(buf, animal_count, j);
	UNPKG_H_UINT32(buf, itm_cnt, j);
	UNPKG_H_UINT32(buf, feed_count, j);
	UNPKG_H_UINT32(buf, egg_count, j);
	DEBUG_LOG("PASTURE STAUTS [%d %d %d %d %d %d %d %d]", p->id, id, fish_lock, pool_state, animal_count, itm_cnt, feed_count, egg_count);

	CHECK_BODY_LEN(len, 32 + animal_count * sizeof(animal_attr_t) + itm_cnt * 16 + feed_count * 8 + egg_count * 16);

	if (animal_outgo && (animal_outgo < 1000)) {
		char txt[256];
#ifdef  TW_VER
		snprintf(txt, sizeof(txt), "   因為你很長時間都沒喂牧場的小動物了，所以他們逃跑啦，你的魅力值也減少了5點哦，記得一定要好好照顧它們！");
		send_postcard("公民管理處", 0, id, 1000098, txt, 0);
#else
		snprintf(txt, sizeof(txt), "   因为你很长时间都没喂牧场的小动物了，所以它们逃跑啦，你的魅力值也减少了5点哦，记得一定要好好照顾它们！");
		send_postcard("公民管理处", 0, id, 1000098, txt, 0);
#endif
		send_attr_update_noti(0, 0, id, 3);

		p->lovely -= 5;
		do_db_attr_op(0, p->id, 0, 0, 0, 0, -5, 0, 0);
	//machine dog catch animal back
	} else if (animal_outgo == 1000) {
#ifdef  TW_VER
			send_postcard("公民管理處", 0, id, 1000120, "", 0);
#else
		send_postcard("公民管理处", 0, id, 1000120, "", 0);
#endif
	}

	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, id, i);
	if (p->id == id) {
		PKG_STR(msg, p->nick, i, sizeof p->nick);
		PKG_UINT32 (msg, 1, i);
	} else {
		sprite_t* op = get_sprite(id);
		//if (!op)
			//op = get_across_svr_sprite(id, id);

		if (op) {
			PKG_STR(msg, op->nick, i, sizeof p->nick);
			PKG_UINT32 (msg, 1, i);
		} else {
			char buf[sizeof p->nick];
			snprintf(buf, sizeof buf, "%u", id);
			PKG_STR(msg, buf, i, sizeof buf);
			PKG_UINT32 (msg, 0, i);
		}
	}

	PKG_UINT32(msg, fish_lock, i);
	PKG_UINT32(msg, pool_state, i);
	PKG_UINT32(msg, insect_house, i);
	PKG_UINT32(msg, animal_count, i);
	PKG_UINT32(msg, itm_cnt, i);
	PKG_UINT32(msg, feed_count, i);
	PKG_UINT32(msg, egg_count, i);
	int loop;
	for (loop = 0; loop < animal_count; loop++) {

		animal_attr_t* animal = (animal_attr_t*)(buf + j);
		pkg_animal_info(msg, animal, &i);
		j += sizeof(animal_attr_t);
		if (animal->favor < 100 && animal->favor > 0 && animal->sickflag) {
			response_animal_hunger(id);
		}

	}

	for (loop = 0; loop < itm_cnt; loop++) {
		uint32_t itemid;

		UNPKG_H_UINT32(buf, itemid, j);
		PKG_UINT32(msg, itemid, i);

		memcpy (msg + i, buf + j, 12);
		i += 12;
		j += 12;
	}

	for (loop = 0; loop < feed_count; loop++) {
		uint32_t itemid;
		uint32_t cnt;

		UNPKG_H_UINT32(buf, itemid, j);
		PKG_UINT32(msg, itemid, i);

		UNPKG_H_UINT32(buf, cnt, j);
		PKG_UINT32(msg, cnt, i);
	}
	for (loop = 0; loop < egg_count; loop++) {
		uint32_t itemid,  pos, num, rest_num;

		UNPKG_H_UINT32(buf, itemid, j);
		PKG_UINT32(msg, itemid, i);

		UNPKG_H_UINT32(buf, pos, j);
		PKG_UINT32(msg, pos, i);

		UNPKG_H_UINT32(buf, num, j);
		PKG_UINT32(msg, num, i);

		UNPKG_H_UINT32(buf, rest_num, j);
		PKG_UINT32(msg, rest_num - num, i);
		DEBUG_LOG("[ uid=%u eggid=%u position=%u already=%u rest=%u]", p->id, itemid, pos, num, rest_num);
	}
	DEBUG_LOG(" [uid=%u len=%u]", p->id, i);
	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int get_store_items_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);

	if (!p->tiles || !IS_PASTURE_MAP(p->tiles->id))
		ERROR_RETURN(("not in PASTURE\t[uid=%u]", p->id), -1);

	uint32_t tmp_buf[2]; // start itemid  and end itemid
	get_item_kind_range (ANIMAL_BABY_ITEM, &tmp_buf[0],  &tmp_buf[1]);

	DEBUG_LOG("GET PASTURE STORE\t[uid=%u]", p->id);
	return db_get_storehouse_item(p, tmp_buf);
}

int get_store_items_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count;
	int k = 0;
	UNPKG_H_UINT32 (buf, count, k);
	CHECK_BODY_LEN(len, 4 + count * 8);

	int i, j = sizeof (protocol_t);
	PKG_UINT32 (msg, count, j);
	for (i = 0; i < count; i++) {
		uint32_t itm, cnt;
		UNPKG_H_UINT32 (buf, itm, k);
		UNPKG_H_UINT32 (buf, cnt, k);
		PKG_UINT32 (msg, itm, j);
		PKG_UINT32 (msg, cnt, j);
//		DEBUG_LOG ("GET STORE\t[%u %u %u]", p->id, itm, cnt);
	}

	init_proto_head (msg, p->waitcmd, j);

	DEBUG_LOG ("GET STORE CALLBACK\t[%u %u]", p->id, count);
	return send_to_self (p, msg, j, 1);

}

int pasture_capture_animal_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t buff[3];
	UNPKG_UINT32(body, buff[0], j);  // 0, yu; 1, animal
	buff[1] = p->id;
	UNPKG_UINT32(body, buff[2], j);  // which number

	if (!p->tiles || !IS_PASTURE_MAP(p->tiles->id))
		ERROR_RETURN(("not in PASTURE\t[uid=%u]", p->id), -1);
	if (p->id != GET_UID_IN_PASTURE(p->tiles->id) && p->daily_limit[2] >= 5) {
		return send_to_self_error(p, p->waitcmd, ERR_get_too_many_fish_from_other, 1);
	}

	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0405BCA3, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	*(uint32_t*)p->session = buff[2];
	DEBUG_LOG("CAPTURE ANIMAL\t[uid=%u seq=%u]", p->id, buff[2]);
	return db_capture_animal(p, buff, GET_UID_IN_PASTURE(p->tiles->id));
}

int pasture_capture_animal_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);
	if(id == p->id) {
		int breed_diff = *(uint32_t*)buf - p->breed_skill;
		notify_skill_change(p, 0, breed_diff);
		uint32_t old_breed_level = calculation_level_from_skill(p->breed_skill);
		p->breed_skill = *(uint32_t*)buf;
		uint32_t new_breed_level = calculation_level_from_skill(p->breed_skill);
		level_change_and_send_postcard(new_breed_level, old_breed_level, 1000110, p->id);
	}

	uint32_t itmid = *(uint32_t*)(buf + 4);
	uint32_t lvl = *(uint32_t*)(buf + 8);

	DEBUG_LOG("CAPTURE ANIMAL CALLBACK\t[%u %u]", p->id, itmid);
	msglog(statistic_logfile, 0x0404010A,get_now_tv()->tv_sec, &(p->id), 4);

	if(id != p->id) {
		p->daily_limit[2]++;
		db_set_sth_done(0, 1003, 5, p->id);
		db_single_item_op(0, p->id, itmid, 1, 1);
	}
	/* 极品圣光兽 */
	if (lvl == 3) {
		p->animal_count++;
		uint32_t msg_buff[2] = {p->id, p->animal_count};
		msglog(statistic_logfile, 0x040413FE, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}

	uint32_t number = *(uint32_t*)p->session;
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, itmid, j);
	PKG_UINT32(msg, number, j);
	PKG_UINT32(msg, lvl, j);

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int pasture_add_feed_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t itemid;
	uint32_t cnt;
	UNPKG_UINT32(body, itemid, j);
	UNPKG_UINT32(body, cnt, j);

	if (!p->tiles || !IS_PASTURE_MAP(p->tiles->id))
		ERROR_RETURN(("not in PASTURE\t[uid=%u]", p->id), -1);

	userid_t uid = GET_UID_IN_PASTURE(p->tiles->id);

	DEBUG_LOG("ADD FEED\t[uid=%u %u %u %u]", p->id, uid, itemid, cnt);

	p->sess_len = 0;
	PKG_H_UINT32(p->session, uid, p->sess_len);
	PKG_H_UINT32(p->session, p->id, p->sess_len);
	PKG_H_UINT32(p->session, itemid, p->sess_len);
	PKG_H_UINT32(p->session, cnt, p->sess_len);
	if (p->id == uid) {
		uint32_t tmp_buf[3];
		tmp_buf[0] = p->id;
		tmp_buf[1] = itemid;
		tmp_buf[2] = cnt;
		return db_pasture_add_feed(p, tmp_buf, p->id);
	} else {
		return db_single_item_op(p, p->id, itemid, cnt, 0);
	}
}

int pasture_add_feed_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);
	//uint32_t feed_count = *(uint32_t*)(p->session + 12);
	add_breed_exp(p, 2);
	int j = 0;
	uint32_t self, count, itemid, cnt;

	UNPKG_H_UINT32(buf, self, j);
	UNPKG_H_UINT32(buf, count, j);
	DEBUG_LOG("pasture_add_feed_callback\t[uid=%u self=%u count=%u]", p->id, self, count);
	CHECK_BODY_LEN(len, 8 + 8 * count);

	/*
	if (!self) {
		do_db_attr_op(0, p->id, 0, 0, 0, 0, 1, 0, 0);
	}
	*/

	int loop, k = sizeof(protocol_t);
	PKG_UINT32(msg, count, k);
	for (loop = 0; loop < count; loop++) {
		UNPKG_H_UINT32(buf, itemid, j);
		UNPKG_H_UINT32(buf, cnt, j);
		PKG_UINT32(msg, itemid, k);
		PKG_UINT32(msg, cnt, k);
	}
	DEBUG_LOG("PASTURE ADD FEED CALLBACK\t[%u]", p->id);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int pasture_herd_animal_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t itemid;
	UNPKG_UINT32(body, itemid, j);

	if (!p->tiles || !IS_PASTURE_MAP(p->tiles->id) || (p->id != GET_UID_IN_PASTURE(p->tiles->id)))
		ERROR_RETURN(("not in right pasture\t[uid=%u]", p->id), -1);

	DEBUG_LOG("HERD ANIMAL\t[uid=%u %u]", p->id, itemid);

	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0405BCA2, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	*(uint32_t *)p->session = itemid;
	return db_herd_animal(p, &itemid);
}

int pasture_herd_animal_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(animal_attr_t));

	msglog(statistic_logfile, 0x0404010A,get_now_tv()->tv_sec, &(p->id), 4);

	add_breed_exp(p, 2);
	int i = sizeof (protocol_t);
	animal_attr_t* animal = (animal_attr_t*)buf;
	uint32_t item_id = *(uint32_t *)p->session;
	pkg_animal_info(msg, animal, &i);
	PKG_UINT32(msg, item_id, i);
	DEBUG_LOG("HERD ANIMAL CALLBACK[uid=%u, %u %u %u %u %u %u %u %u %u %u %u %u]", p->id, id, animal->number, animal->itemid, \
	animal->sickflag, animal->value, \
	animal->eat_time, animal->drink_time, \
	animal->output_cnt, animal->output_time, animal->update_time, animal->mature_time, animal->max_output);

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int pasture_get_animal_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t animal_type;
	UNPKG_UINT32(body, animal_type, j);

	if (!p->tiles || !IS_PASTURE_MAP(p->tiles->id))
		ERROR_RETURN(("not in pasture\t[uid=%u]", p->id), -1);

	DEBUG_LOG("GET FISH\t[uid=%u %lu]", p->id, p->tiles->id);
	return db_get_pasture_animal(p, &animal_type, GET_UID_IN_PASTURE(p->tiles->id));
}

int pasture_get_animal_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);

	uint32_t animal_count, animal_type;;
	int j = 0;
	UNPKG_H_UINT32(buf, animal_type, j);
	UNPKG_H_UINT32(buf, animal_count, j);

	CHECK_BODY_LEN(len, 8 + animal_count * sizeof(animal_attr_t));

	int i = sizeof (protocol_t);
	PKG_UINT32(msg, animal_type, i);
	PKG_UINT32(msg, animal_count, i);
	int loop;
	for (loop = 0; loop < animal_count; loop++) {

		animal_attr_t* animal = (animal_attr_t*)(buf + j);

		pkg_animal_info(msg, animal, &i);
		j += sizeof(animal_attr_t);

	}


	init_proto_head (msg, p->waitcmd, i);
	DEBUG_LOG ("GET ANIMAL CALLBACK\t[%u %u %u %u]", p->id, id, animal_type, animal_count);
	return send_to_self (p, msg, i, 1);

}

int pasture_add_water_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	if (!p->tiles || !IS_PASTURE_MAP(p->tiles->id))
		ERROR_RETURN(("not in pasture\t[uid=%u]", p->id), -1);

	DEBUG_LOG("ADD WATER\t[uid=%u %lu]", p->id, p->tiles->id);
	return db_pasture_add_water(p, GET_UID_IN_PASTURE(p->tiles->id));
}

int pasture_add_water_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	add_breed_exp(p, 2);
	uint32_t animal_count;
	int j = 0;
	UNPKG_H_UINT32(buf, animal_count, j);

	CHECK_BODY_LEN(len, 4 + animal_count * sizeof(animal_attr_t));

	int i = sizeof (protocol_t);
	PKG_UINT32(msg, animal_count, i);
	int loop;
	for (loop = 0; loop < animal_count; loop++) {

		animal_attr_t* animal = (animal_attr_t*)(buf + j);

		pkg_animal_info(msg, animal, &i);
		j += sizeof(animal_attr_t);

	}


	init_proto_head (msg, p->waitcmd, i);
	DEBUG_LOG ("ADD WATER CALLBACK\t[%u %u %u]", p->id, id, animal_count);
	return send_to_self (p, msg, i, 1);
}

/*
 *@ list recent 50 visitors
 */
int get_recent_pasture_visitors_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	int j = 0;
	userid_t uid = 0;
	UNPKG_UINT32(body, uid, j);

	return db_list_recent_pasture_visitors(p, uid);
}

/*
 *@ get recent visitors information from db, handle db's reply
 */
int get_recent_pasture_visitors_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	pv_list_res_t* vp = (pv_list_res_t*)buf;
	CHECK_BODY_LEN(len, vp->cnt * sizeof(visitor_pasture_t) + 4);

	int i = sizeof(protocol_t);
	PKG_UINT16(msg, vp->cnt, i);

	int loop = 0;
	for(loop = 0; loop < vp->cnt; loop++) {
		PKG_UINT32(msg, vp->visitor[loop].userid, i);
		PKG_UINT32(msg, vp->visitor[loop].op_type, i);
		PKG_STR(msg, vp->visitor[loop].nick, i, 16);
		PKG_UINT32(msg, vp->visitor[loop].color, i);
		PKG_UINT8(msg, vp->visitor[loop].isvip, i);
		PKG_UINT32(msg, vp->visitor[loop].stamp, i);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int get_feed_items_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);

	if (!p->tiles || !IS_PASTURE_MAP(p->tiles->id))
		ERROR_RETURN(("not in PASTURE\t[uid=%u]", p->id), -1);

	int  type = 5;
	char buf[13];
	userid_t uid = GET_UID_IN_PASTURE(p->tiles->id);

	int i = 0;
	PKG_H_UINT32 (buf, 5, i);
	PKG_H_UINT32 (buf, 0, i);
	PKG_H_UINT32 (buf, 0xFFFFFFFF, i);
	PKG_UINT8 (buf, 0, i);
	DEBUG_LOG("GET PASTURE %d ITEMS [%d %u]", type, p->id, uid);
	return send_request_to_db (SVR_PROTO_GET_ITEM, p, i, buf, uid);
}

static int
unpkg_set_pasture_item(uint8_t* body, int len, int *used, int *not_used)
{
	int j, i;
	home_item_t *hi;

	if (len < 8)
		ERROR_RETURN (("error len=%d", len), -1);

	i = 0;
	UNPKG_UINT32 (body, *used, i);
	UNPKG_UINT32 (body, *not_used, i);
	j = 0;
	PKG_H_UINT32 (body, *used, j);
	PKG_H_UINT32 (body, *not_used, j);

	CHECK_BODY_LEN (len, 8 + (*used) * sizeof (home_item_t) + (*not_used) * 8);

	for (j = 0; j < *not_used; j++) {
		uint32_t * t = (uint32_t*) (body + 8 + j * 8);
		*t = ntohl (*t);
		*(t + 1) = ntohl (*(t + 1));
		//DEBUG_LOG("itmid=%u cnt=%u", *t, *(t + 1));
	}
	for (j = 0; j < *used; j++) {
		hi = (home_item_t*) (body + 8 + (*not_used) * 8 + j * sizeof (home_item_t));
		hi->itemid = ntohl (hi->itemid);
		hi->x = ntohs (hi->x);
		hi->y = ntohs (hi->y);

		//DEBUG_LOG("itmid=%u %u %u %u %u %u %u %u", hi->itemid, hi->x, hi->y, hi->way, hi->visible, hi->layer, hi->type, hi->rotation);
		if (!get_item_prop (hi->itemid))
			ERROR_RETURN (("error item id=%d", hi->itemid), -1);
	}

	return 0;
}

int set_pasture_items_cmd(sprite_t* p, uint8_t* body, int len)
{
	int used, not_used;

	if (unpkg_set_pasture_item (body, len, &used, &not_used) == -1)
		return -1;
	DEBUG_LOG("SET PASTURE ITEM [%d]", p->id);
	return send_request_to_db(SVR_PROTO_SET_PASTURE_ITEMS, p, len, body, p->id);
}

int set_pasture_items_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int pasture_lock_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t state; // 1 lock animal, 2 lock fish
	UNPKG_UINT32(body, state, j);  // which number

	if (!p->tiles || !IS_PASTURE_MAP(p->tiles->id))
		ERROR_RETURN(("not in PASTURE\t[uid=%u]", p->id), -1);

	DEBUG_LOG("PASTURE LOCK\t[uid=%u state=%u]", p->id, state);
	return db_pasture_lock(p, &state, p->id);
}

int pasture_lock_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t state = *(uint32_t*)buf;
	DEBUG_LOG("PASTURE LOCK CALLBACK\t[%u state=%u]", p->id, state);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, state, i);
	init_proto_head(msg, p->waitcmd, i);

	send_to_map(p, msg, i, 1);
	return 0;
}

int get_sheep_from_vip_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	//if(ISVIP(p->flag)) {
	//    ERROR_RETURN(("vip can not do [%d]", p->id), -1);
	//}
	int i = 0;
	uint32_t uid ;
	UNPKG_UINT32(body, uid, i);
	CHECK_VALID_ID(uid);
	if(uid == p->id) {
		ERROR_RETURN(("get self sheep [%d]", p->id), -1);
	}
	*(uint32_t*)p->session = uid;

	return db_chk_itm_cnt(p, 190351, 190352);
	//return db_set_sth_done(p, 154, 2, p->id);
}

int do_get_sheep(sprite_t* p, uint32_t count, uint8_t* buf)
{
	//db_single_item_op(0, *(uint32_t*)p->session, 190246, 1, 0);
	//return do_get_sheep2(p);
	if (count) {
		uint32_t itm, cnt;
		int k = 0;
		UNPKG_H_UINT32 (buf, itm, k);
		UNPKG_H_UINT32 (buf, cnt, k);
		DEBUG_LOG("GET SHEEP\t[%u %u %u]", p->id, count, cnt);
		if (cnt >= 10) {
			return db_set_sth_done(p, 154, 2, p->id);
		}
	}
	return send_to_self_error(p, p->waitcmd, -ERR_not_enough_yinghuocao, 1);
}

int do_get_sheep2(sprite_t* p)
{
	uint32_t uid = *(uint32_t*)p->session;
	DEBUG_LOG("GET SHEEP CALLBACK[%d %d]", p->id, uid);

	db_single_item_op(0, uid, 190246, 1, 0);
	db_single_item_op(NULL, p->id, 1270006, 1, 1);
	do_db_attr_op(NULL, uid, 0, 0, 0, 0, 2, ATTR_CHG_roll_back, 0);
	db_add_pasture_visitors(p, uid, PASTURE_GET_SHEEP_LIST);

	uint32_t tmp_buf[3];
	tmp_buf[0] = p->id;
	tmp_buf[1] = 190351;
	tmp_buf[2] = 10;
	db_pasture_add_feed(p, tmp_buf, uid);  // add feed to uid
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_sheep_reward_cmd(sprite_t* p, uint8_t* body, int len)
{
	return send_request_to_db(SVR_PROTO_SHEEP_OUTPUT_NUM, p, 0, NULL, p->id);
}

int get_sheep_reward_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t sheep_output_cnt = *(uint32_t*)buf;
	if(sheep_output_cnt >= 20) {
		char buf[20];
		int i = 0;
		PKG_H_UINT32(buf, 2, i);
		PKG_H_UINT32(buf, 1220083, i);
		PKG_H_UINT32(buf, 1, i);
		PKG_H_UINT32(buf, 1, i);
		PKG_H_UINT32(buf, 0, i);
		send_request_to_db(SVR_PROTO_BUY_ITEM, NULL, 20, buf, p->id);
		response_proto_uint32(p, p->waitcmd, 1220083, 0);
	} else {
		response_proto_uint32(p, p->waitcmd, sheep_output_cnt, 0);
	}
	return 0;
}

int follow_animal_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 8);

	int j = 0;
	uint32_t nbr, out_or_in;
	UNPKG_UINT32(body, nbr, j);			// animal number
	UNPKG_UINT32(body, out_or_in, j);	// out or in

	if (out_or_in != 0 && out_or_in != 1) {
		ERROR_RETURN(("animal not out, not in\t[%u %u %u]", p->id, nbr, out_or_in), -1);
	}

	*(uint32_t*)p->session = nbr;
	*(uint32_t*)(p->session + 4) = out_or_in;
	return set_sth_follow( p, nbr, out_or_in, FAT_ANI );
//
// 	if (out_or_in && p->animal_nbr) {
// 		if((p->animal.itemid< 1353401) && (p->animal.itemid> 0)){
// 			db_animal_set_outgo(0, p->id, p->animal_nbr, 0);
// 		}else if(p->animal.itemid >= 1353401){
// 			db_put_angel_home(0, p->id, p->animal_nbr);
// 		}
// 	}
//
// 	DEBUG_LOG("FOLLOW ANIMAL\t[%u %u %u %u]", p->id, p->animal_nbr, nbr, out_or_in);
// 	return db_animal_set_outgo(p, p->id, nbr, out_or_in);
}

int follow_animal_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	int j = 0;
	uint32_t count;
	UNPKG_H_UINT32(buf, count, j);
	CHECK_BODY_LEN(len, sizeof(animal_attr_t) * count + 4);

	uint32_t nbr = *(uint32_t*)p->session;
	uint32_t out_or_in = *(uint32_t*)(p->session + 4);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, nbr, i);
	PKG_UINT32(msg, out_or_in, i);
	PKG_UINT32(msg, count, i);
	int loop;
	for (loop = 0; loop < count; loop++) {
		animal_attr_t* animal = (animal_attr_t*)(buf + j);
		pkg_animal_info(msg, animal, &i);
		j += sizeof(animal_attr_t);
		if (animal->number == nbr) {
			if (out_or_in) {
				p->animal_nbr = nbr;
				memcpy(&(p->animal_follow_info.tag_animal), animal, sizeof(animal_attr_t));
			} else {
				p->animal_nbr = 0;
				memset(&(p->animal_follow_info.tag_animal), 0, sizeof(animal_attr_t));
			}
		}
	}

	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

int animal_chicao_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);

	int j = 0;
	uint32_t nbr;
	UNPKG_UINT32(body, nbr, j);			// animal number
	if (p->animal_nbr != nbr) {
		ERROR_RETURN(("animal number not right\t[%u %u %u]", p->id, p->animal_nbr, nbr), -1);
	}

	return db_animal_chicao(p, p->id, nbr);
}

int animal_chicao_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(animal_attr_t));
	animal_attr_t* animal = (animal_attr_t*)buf;

	p->animal_nbr = animal->number;
	memcpy(&(p->animal_follow_info.tag_animal), animal, sizeof(animal_attr_t));
	int i = sizeof(protocol_t);
	pkg_animal_info(msg, animal, &i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

int pasture_get_nets_cmd(sprite_t* p, uint8_t* body, int len)
{
	return send_request_to_db(SVR_PROTO_PASTURE_GET_NETS_STATUS, p, 0, NULL, p->id);
}

int pasture_get_nets_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	return do_pasture_get_nets(p);
}

int pasture_get_nets_status_cmd(sprite_t* p, uint8_t* body, int len)
{
	return send_request_to_db(SVR_PROTO_PASTURE_GET_NETS_STATUS, p, 0, NULL, p->id);
}

int do_pasture_get_nets(sprite_t *p)
{
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int pasture_get_nets_status_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t res = *(uint32_t*)buf;
	switch (p->waitcmd) {
		case PROTO_PASTURE_NETS_STATUS:
			response_proto_uint32(p, p->waitcmd, res, 0);
			return 0;
		case PROTO_PASTURE_GET_NETS:
			if (res > 1) {
				return send_to_self_error(p, p->waitcmd, -ERR_pasture_already_have_nets, 1);
			}
			return db_change_xiaome(p, -100, 0, 0, p->id);
		default:
			ERROR_RETURN(("invalid cmd\t[uid=%u cmd=%u]", p->id, p->waitcmd), -1);
	}
	return 0;
}

int pasture_catch_fish_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	return send_request_to_db(SVR_PROTO_PASTURE_CATCH_FISH, p, 0, NULL, p->id);
}

int pasture_catch_fish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 12);
	int j = 0, i = sizeof(protocol_t);

	int breed_diff = *(uint32_t*)buf - p->breed_skill;
	notify_skill_change(p, 0, breed_diff);
	uint32_t old_bred_skill = calculation_level_from_skill(p->breed_skill);
	UNPKG_H_UINT32(buf, p->breed_skill, j);
	uint32_t new_bred_skill = calculation_level_from_skill(p->breed_skill);
	level_change_and_send_postcard(new_bred_skill, old_bred_skill, 1000110, p->id);

	uint32_t type_count, netstatus;
	UNPKG_H_UINT32(buf, netstatus, j);
	UNPKG_H_UINT32(buf, type_count, j);
	CHECK_BODY_LEN(len, 12 + 8 * type_count);
	PKG_UINT32(msg, type_count, i);
	PKG_UINT32(msg, netstatus, i);
	int loop;
	uint32_t itmid, count;
	for (loop = 0; loop < type_count; loop ++) {
		UNPKG_H_UINT32(buf, itmid, j);
		UNPKG_H_UINT32(buf, count, j);
		PKG_UINT32(msg, itmid, i);
		PKG_UINT32(msg, count, i);
	}
	init_proto_head(msg,  p->waitcmd,  i);
	return send_to_self(p, msg, i, 1);
}

int calculation_level_from_skill(uint32_t skill)
{
	uint32_t level = 1;
	if (skill >= 43500) {
		level = 30;
	} else if(skill >= 40600) {
		level = 29;
	} else if(skill >= 37800) {
		level = 28;
	} else if(skill >= 35100) {
		level = 27;
	} else if(skill >= 32500) {
		level = 26;
	} else if(skill >= 30000) {
		level = 25;
	} else if(skill >= 27600) {
		level = 24;
	} else if(skill >= 25300) {
		level = 23;
	} else if(skill >= 23100) {
		level = 22;
	} else if(skill >= 21000) {
		level = 21;
	} else if(skill >= 19000) {
		level = 20;
	} else if(skill >= 17100) {
		level = 19;
	} else if(skill >= 15300) {
		level = 18;
	} else if(skill >= 13600) {
		level = 17;
	} else if(skill >= 12000) {
		level = 16;
	} else if(skill >= 10500) {
		level = 15;
	} else if(skill >= 9100) {
		level = 14;
	} else if(skill >= 7800) {
		level = 13;
	} else if(skill >= 6600) {
		level = 12;
	} else if(skill >= 5500) {
		level = 11;
	} else if(skill >= 4500) {
		level = 10;
	} else if(skill >= 3600) {
		level = 9;
	} else if(skill >= 2800) {
		level = 8;
	} else if(skill >= 2100) {
		level = 7;
	} else if(skill >= 1500) {
		level = 6;
	} else if(skill >= 1000) {
		level = 5;
	} else if(skill >= 600) {
		level = 4;
	} else if(skill >= 300) {
		level = 3;
	} else if(skill >= 100) {
		level = 2;
	}
	return level;
}

int calculation_level_from_fashion(uint32_t fashion)
{
	uint32_t level = 1;
	if (fashion >= 25000) {
		level = 30;
	} else if (fashion >= 22601) {
		level = 29;
	} else if (fashion >= 21201) {
		level = 28;
	} else if (fashion >= 19801) {
		level = 27;
	} else if (fashion >= 18401) {
		level = 26;
	}else if (fashion >= 16401) {
		level = 25;
	}else if (fashion >= 15001) {
		level = 24;
	}else if (fashion >= 13601) {
		level = 23;
	}else if (fashion >= 12601) {
		level = 22;
	} else if (fashion >= 11001) {
		level = 21;
	} else if (fashion >= 10001) {
		level = 20;
	} else if (fashion >= 9001) {
		level = 19;
	} else if (fashion >= 8401) {
		level = 18;
	} else if (fashion >= 7101) {
		level = 17;
	} else if (fashion >= 6301) {
		level = 16;
	} else if(fashion >= 5501) {
		level = 15;
	} else if (fashion >= 5001) {
		level = 14;
	} else if(fashion >= 4001) {
		level = 13;
	} else if (fashion >= 3401) {
		level = 12;
	} else if (fashion >= 2801) {
		level = 11;
	} else if(fashion >= 2401) {
		level = 10;
	} else if(fashion >= 1901) {
		level = 9;
	} else if(fashion >= 1601) {
		level = 8;
	} else if(fashion >= 1301) {
		level = 7;
	} else if(fashion >= 901) {
		level = 6;
	} else if(fashion >= 601) {
		level = 5;
	} else if(fashion >= 301) {
		level = 4;
	} else if(fashion >= 101) {
		level = 3;
	} else if(fashion >= 2) {
		level = 2;
	} else {
		level = 1;
	}
	return level;
}

int level_change_and_send_postcard(uint32_t new_level, uint32_t old_level, uint32_t post_id, uint32_t uid)
{
	//if (new_level > old_level && (new_level == 5 || new_level == 10 ||new_level == 15)) {
	if (new_level > old_level) {
		//send_postcard("克勞神父", 0, uid, 1000067, txt, 0);
		char buf[1024] = {0};
		char name[128] = {0};
#ifdef TW_VER
		if (new_level == 5) {
			if(post_id == 1000109) //jiayuan
				sprintf(buf, "嗯，你很用心的照料你的植物，技能越來越熟練。祝賀你被授予花園小匠稱號！嘿喲嘿喲。");
			else
				sprintf(buf, "在過去的日子裏，你通過努力把牧場打理得很好。授予你實習牧場主稱號，要加油哦！");
		}
		else if (new_level == 10) {
			if(post_id == 1000109) //jiayuan
				sprintf(buf, "很高興的通知你，你被授予花園工匠稱號啦！這都是你辛勤勞動的結果，是不是漸漸喜歡上收穫的感覺呢？加油，要繼續努力哦。");
			else
				sprintf(buf, "很高興通知你，你被授予優秀牧場主稱號啦！牧場在你的努力下變得井井有條，保持下去，你能做的更好！");
		}
		else if (new_level == 15) {
			if(post_id == 1000109) //jiayuan
				sprintf(buf, "隆重的授予你花園巧匠稱號。此時的你已經對植物了如指掌了，打理家園十分在行，今後也希望你能用自己的能力幫助更多人。快去家園雜誌中領取花園巧匠禮包吧！");
			else
				sprintf(buf, "隆重授予你超級牧場主稱號。健康成長的小動物們都是你努力的見證，你將是所有小摩爾的榜樣！快去牧場雜誌中領取超級牧場主禮包吧！");
		}
		if (post_id == 1000109)
			sprintf(name, "梅森");
		else
			sprintf(name, "尤尤");
#else
		if (new_level == 2) {
			if(post_id == 1000109) {
				sprintf(buf, "恭喜你脱离新手阶段，成为优秀的农场小助手，还有更多精彩在等着你哟！");
			}
		} else if (new_level == 3) {
			if(post_id == 1000109) {
				sprintf(buf, "美丽的植物在成长，你也在成长哦，继续加油！");
			}
		} else if (new_level == 4) {
			if(post_id == 1000109) {
				sprintf(buf, "美丽的植物在成长，你也在成长哦，继续加油！");
			}
		} else if (new_level == 5) {
			if(post_id == 1000109) {
				post_id = 1000122;
				sprintf(buf, "恩，你很用心的照料你的植物，技能越来越熟练。祝贺你被授予花园小匠称号！");
			} else {
				sprintf(buf, "在过去的日子里，你通过努力把牧场打理的很好。授予你实习牧场主称号，要加油哦！");
			}
		} else if (new_level == 6) {
			if(post_id == 1000109) {
				sprintf(buf, "聪明的花园小匠你又进步了，为了自己的成长而喝彩吧，看看是不是又有什么新的作物呢？");
			}
		} else if (new_level == 7) {
			if(post_id == 1000109) {
				sprintf(buf, "每一滴汗水都会酝酿出果实的芳香，每一次努力都会带来成长，为自己的努力喝彩吧！");
			}
		} else if (new_level == 8) {
			if(post_id == 1000109) {
				sprintf(buf, "每一滴汗水都会酝酿出果实的芳香，每一次努力都会带来成长，为自己的努力喝彩吧！");
			}
		} else if (new_level == 9) {
			if(post_id == 1000109) {
				sprintf(buf, "每一滴汗水都会酝酿出果实的芳香，每一次努力都会带来成长，为自己的努力喝彩吧！");
			}
		} else if (new_level == 10) {
			if(post_id == 1000109) {
				post_id = 1000123;
				sprintf(buf, "现在的成长是你辛勤劳动的结果，是不是渐渐喜欢上收获的感觉呢？加油，要继续努力哦。");
			} else {
				sprintf(buf, "很高兴通知你，你被授予优秀牧场主称号啦！牧场在你的努力下变得井井有条，保持下去，你能做的更好！");
			}
		} else if (new_level == 11) {
			if(post_id == 1000109) {
				sprintf(buf, "恭喜恭喜，你又升级了，在这段时间的努力中有没有感受到挥洒汗水的快乐？还要努力哦！");
			}
		} else if (new_level == 12) {
			if(post_id == 1000109) {
				sprintf(buf, "芝麻开花节节高，恭喜你再次升级，庄园中的还藏着怎样珍稀的种子呢？");
			}
		} else if (new_level == 13) {
			if(post_id == 1000109) {
				sprintf(buf, "芝麻开花节节高，恭喜你再次升级，庄园中的还藏着怎样珍稀的种子呢？");
			}
		} else if (new_level == 14) {
			if(post_id == 1000109) {
				sprintf(buf, "芝麻开花节节高，恭喜你再次升级，庄园中的还藏着怎样珍稀的种子呢？");
			}
		} else if (new_level == 15) {
			if(post_id == 1000109) {
				post_id = 1000124;
				sprintf(buf, "看来你对植物的习性已经了如指掌，你要用这些技能帮助别人哦！去农场手册中领取奖励吧！");
			} else {
				sprintf(buf, "隆重的授予你超级牧场主称号。小动物们都是你努力的见证者，你是所有小摩尔的榜样！快去牧场杂志中领取超级牧场主礼包吧！");
			}
		}
		else
		{
			if (post_id == 1000109)
			{
				sprintf(buf, "恭喜你的种植等级提升到%d级，你的家园越来越漂亮了呢，为了更高的目标而努力吧！", new_level);
			}
			else
			{
				sprintf(buf, "恭喜你的养殖等级提升到%d级，你的小动物越来越可爱了呢，为了更高的目标而努力吧！", new_level);
			}

		}

		if (post_id == 1000109 || post_id == 1000122 || post_id == 1000123 || post_id == 1000124)
			sprintf(name, "梅森");
		else
			sprintf(name, "尤尤");
#endif
		send_postcard(name, 0, uid, post_id, buf, 0);
		send_attr_update_noti(0, 0, uid, 3);

		return 0;
	}
	return 0;
}

int add_breed_exp(sprite_t* p, int breed_exp)
{
	uint32_t old_breed_level = calculation_level_from_skill(p->breed_skill);
	if (p->breed_day_limit + breed_exp > 20)
		breed_exp = 20 - p->breed_day_limit;

	p->breed_skill += breed_exp;
	p->breed_day_limit += breed_exp;
	notify_skill_change(p, 0, breed_exp);
	DEBUG_LOG ("ADD BREED EXP\t[%u %u %u]", p->id, p->cult_day_limit, p->cultivate_skill);
	uint32_t new_breed_level = calculation_level_from_skill(p->breed_skill);
	level_change_and_send_postcard(new_breed_level, old_breed_level, 1000110, p->id);
	return db_add_culti_breed_exp(0, breed_exp, 2, p->id);
}

int pasture_get_level_cmd(sprite_t* p, uint8_t* body, int len)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->cultivate_skill, i);
	PKG_UINT32(msg, p->breed_skill, i);
	PKG_UINT32(msg, calculation_level_from_skill(p->cultivate_skill), i);
	PKG_UINT32(msg, calculation_level_from_skill(p->breed_skill), i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int pasture_get_level_ex_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len,  4);
	uint32_t uid;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	CHECK_VALID_ID(uid);
	return send_request_to_db(SVR_PROTO_GET_USER_SKILL, p, 0, NULL, uid);
}

int pasture_get_level_ex_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t cultivate_skill, breed_skill;
	int j = 0;
	UNPKG_H_UINT32(buf, cultivate_skill, j);
	UNPKG_H_UINT32(buf, breed_skill, j);
	DEBUG_LOG ("USER SKILL\t[uid=%u  cultivate_skill=%u  breed_skill=%u]", p->id, cultivate_skill, breed_skill);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, cultivate_skill, i);
	PKG_UINT32(msg, breed_skill, i);
	PKG_UINT32(msg, calculation_level_from_skill(cultivate_skill), i);
	PKG_UINT32(msg, calculation_level_from_skill(breed_skill), i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int pasture_release_animal_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 8);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t buff[2];
	UNPKG_UINT32(body, buff[0], j);  // which number
	UNPKG_UINT32(body, buff[1], j);  // 0, yu; 1, animal; 2, insect

	if (!p->tiles || !IS_PASTURE_MAP(p->tiles->id))
		ERROR_RETURN(("not in PASTURE\t[uid=%u]", p->id), -1);

	if (buff[1] > 4)
		ERROR_RETURN(("bad type\t[uid=%u]", p->id), -1);

	*(uint32_t*)p->session = buff[0];
	DEBUG_LOG("RELEASE ANIMAL\t[uid=%u seq=%u]", p->id, buff[0]);
	return send_request_to_db(SVR_PROTO_RELEASE_ANIMAL , p, 8, buff, p->id);
}

int pasture_release_animal_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t number = *(uint32_t*)p->session;
	response_proto_uint32(p, p->waitcmd, number, 0);
	return 0;
}

int notify_skill_change(sprite_t* p, int cultiv_skill_change, int breed_skill_change)
{
	response_proto_uint32_uint32_not_complete(p, PROTO_NOTIFY_SKILL_UPDATE, cultiv_skill_change, breed_skill_change, 0);
	return 0;
}

/* @brief miao miao milk
 */
int get_milk_from_cow_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);

	if ((p->animal_follow_info.tag_animal.itemid != 1270044 && p->animal_follow_info.tag_animal.itemid != 1270131) || (p->animal_follow_info.tag_animal.mature_time == 0)) {
		return send_to_self_error(p, p->waitcmd, -ERR_animal_not_adult, 1);
	}
	uint32_t db_buf = p->animal_follow_info.tag_animal.number;
	return send_request_to_db(SVR_PROTO_GET_MILK_FROM_COW, p, sizeof(uint32_t), &db_buf, p->id);
}

/* @brief callback function
 */
int get_milk_from_cow_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	db_single_item_op(0, p->id, 190648, 1, 1);
	response_proto_uint32(p, p->waitcmd, 190648, 0);
	return 0;
}

/* @brief 得到具有星级的动物的星级，返回的是每个动物一个记录即每个动物对应(id, lvl)
 */
int get_animal_lvl_cmd(sprite_t *p, uint8_t body[], int len)
{
	uint32_t user_id = 0;
	unpack(body, sizeof(user_id), "L", &user_id);
	return send_request_to_db(SVR_PROTO_GET_ANIMAL_LVL, p, 0, NULL, user_id);
}

/* @brief 返回动物的星级
 */
int get_animal_lvl_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	typedef struct used_skill_info {
		uint32_t skill_id;
		uint32_t used_count;
	}__attribute__((packed)) used_skill_info_t;

	typedef struct animal_id_lvl {
		uint32_t adult_id;
		uint32_t animal_id;
		uint32_t lvl;
		uint32_t animal_index;
		uint32_t cnt;
		used_skill_info_t used_skills[10];
	}__attribute__((packed)) animal_id_lvl_t;

	uint32_t count = *(uint32_t *)buf;
	animal_id_lvl_t *info = (animal_id_lvl_t *)(buf + sizeof(count));
	int index = sizeof(protocol_t);
	PKG_UINT32(msg, count, index);
	int i = 0;
	for (i = 0; i < count; i++) {
		PKG_UINT32(msg, (info + i)->adult_id, index);
		PKG_UINT32(msg, (info + i)->animal_id, index);
		PKG_UINT32(msg, (info + i)->lvl, index);
		PKG_UINT32(msg, (info + i)->animal_index, index);
		PKG_UINT32(msg, (info + i)->cnt, index);
		int j = 0;
		for (j = 0; j < (info + i)->cnt; j++)
		{
			PKG_UINT32(msg, (info + i)->used_skills[j].skill_id, index);
			PKG_UINT32(msg, (info + i)->used_skills[j].used_count, index);
		}
	}
	init_proto_head(msg, p->waitcmd, index);
	return send_to_self(p, msg, index, 1);
}

int farm_animal_use_item_cmd(sprite_t * p,const uint8_t * body,int len)
{
	int i = 0;
	int id = 0;
	int itemid = 0;

	CHECK_BODY_LEN(len, 8);
	UNPKG_UINT32(body,id,i);
	UNPKG_UINT32(body,itemid,i);
	if (itemid != 190840)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	*(uint32_t*)p->session  = itemid;
	uint32_t buff[5] = {0};
	buff[0] = id;
	buff[1] = itemid;
	buff[2] = 5;
	buff[3] = 1;
	buff[4] = 5;
	return  send_request_to_db(SVR_PROTO_FARM_ANIMAL_USE_ITEM, p, 20, buff, p->id);

}


int farm_animal_use_item_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	struct animal_info {
		uint32_t index;
		uint32_t animal_id;
		uint32_t state;
		uint32_t growth;
		uint32_t eat_time;
		uint32_t drink_time;
		uint32_t total_output;
		uint32_t output_time;
		uint32_t lastcal_time;
		uint32_t mature_time;
		uint32_t animal_type ;
		uint32_t favor_time;
		uint32_t favor;
		uint32_t outgo;
		uint32_t animal_flag;
		uint32_t pollinate_num;
		uint32_t pollinate_tm;
		uint32_t max_output;
		uint32_t diff_mature;
		uint32_t cur_grow;
		uint32_t level;
	}__attribute__((packed));

	struct animal_info *p_info = (struct animal_info*)buf;
	CHECK_BODY_LEN(len, sizeof(struct animal_info));

	uint32_t itemid = *(uint32_t*)p->session;

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, l);
	PKG_UINT32(msg, p_info->index, l);
	PKG_UINT32(msg, p_info->animal_id, l);
	PKG_UINT32(msg, p_info->state, l);
	PKG_UINT32(msg, p_info->growth, l);
	PKG_UINT32(msg, p_info->eat_time, l);
	PKG_UINT32(msg, p_info->drink_time, l);
	PKG_UINT32(msg, p_info->total_output, l);
	PKG_UINT32(msg, p_info->output_time, l);
	PKG_UINT32(msg, p_info->lastcal_time, l);
	PKG_UINT32(msg, p_info->mature_time, l);
	PKG_UINT32(msg, p_info->animal_type, l);
	PKG_UINT32(msg, p_info->favor_time, l);
	PKG_UINT32(msg, p_info->favor, l);
	PKG_UINT32(msg, p_info->outgo, l);
	PKG_UINT32(msg, p_info->animal_flag, l);
	PKG_UINT32(msg, p_info->pollinate_num, l);
	PKG_UINT32(msg, p_info->max_output, l);
	PKG_UINT32(msg, p_info->diff_mature, l);
	PKG_UINT32(msg, p_info->cur_grow, l);
	PKG_UINT32(msg, p_info->level, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

static int parse_level_single_skill(skill_info_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"skill")))
		{
			DECODE_XML_PROP_INT (iut[j].skill_id, cur, "skill_id");
			DECODE_XML_PROP_INT (iut[j].type, cur, "type");
			DECODE_XML_PROP_INT (iut[j].cold_time, cur, "cold_time");
			DECODE_XML_PROP_INT (iut[j].max_time, cur, "max_time");
			j++;
		}
		cur = cur->next;
	}

	*cnt = j;

	return 0;
}

static int parse_level_skills(level_skills_t *git, uint32_t* cnt, xmlNodePtr chl)
{
	xmlNodePtr cur;
	int i = 0;
	int level_id = 0;

	while (chl)
	{
		if (!xmlStrcmp(chl->name, (const xmlChar *)"level"))
		{
			DECODE_XML_PROP_INT(level_id, chl, "level_id");
			git[level_id].level_id = level_id;

			cur = chl->xmlChildrenNode;
			if (parse_level_single_skill(git[level_id].level_skills, &(git[level_id].skill_count), cur) != 0)
			{
				return -1;
			}

			i++;
		}
		chl = chl->next;
	}

	*cnt = i;
	return 0;

}


static int parse_star_skills(star_levels_t *git, uint32_t* cnt, xmlNodePtr chl)
{
	xmlNodePtr cur;
	int i = 0;
	int star_id = 0;

	while (chl)
	{
		if (!xmlStrcmp(chl->name, (const xmlChar *)"star"))
		{
			DECODE_XML_PROP_INT(star_id, chl, "star_id");
			git[star_id].star_id = star_id;

			cur = chl->xmlChildrenNode;
			if (parse_level_skills(git[star_id].star_levels, &(git[star_id].level_count), cur) != 0)
			{
				return -1;
			}

			i++;
		}
		chl = chl->next;
	}

	*cnt = i;
	return 0;

}

int load_animal_skills_conf(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int ex_count;

	ex_count = 0;
	memset(animals_skills, 0, sizeof(animals_skills));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"animal"))) {
			DECODE_XML_PROP_INT(i, cur, "animal_id");
			if (ex_count >= ANIMALS_SKILLS_MAX - 1 || i <= 0) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}
			animals_skills[i-ANIMAL_ID_MIN_VALUE].animal_id = i;
			if (animals_skills[i-ANIMAL_ID_MIN_VALUE].animal_id > (ANIMALS_SKILLS_MAX + ANIMAL_ID_MIN_VALUE))
			{
				ERROR_LOG ("parse %s failed, Count=%d, animal_id=%d", file, ex_count, animals_skills[i-1].animal_id);
				goto exit;
			}

			chl = cur->xmlChildrenNode;
			if ((parse_star_skills(animals_skills[i-ANIMAL_ID_MIN_VALUE].animal_stars, &(animals_skills[i-ANIMAL_ID_MIN_VALUE].star_count), chl) != 0) )
				goto exit;

			ex_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load animal skills item file %s", file);

}

int get_animal_used_skill_info_cmd(sprite_t *p, uint8_t body[], int len)
{
	uint32_t animal_index = 0;
	unpack(body, sizeof(animal_index), "L", &animal_index);
	return send_request_to_db(SVR_PROTO_GET_ANIMAL_USED_SKILL_INFO, p, 4, &animal_index, p->id);
}

int get_animal_used_skill_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	typedef struct used_skill_info {
		uint32_t skill_id;
		uint32_t cold_time;
		uint32_t used_count;
		uint32_t type;
	}__attribute__((packed)) used_skill_info_t;

	uint32_t count = *(uint32_t *)buf;
	used_skill_info_t *info = (used_skill_info_t *)(buf + sizeof(count));
	int index = sizeof(protocol_t);
	PKG_UINT32(msg, count, index);
	int i = 0;
	for (i = 0; i < count; i++) {
		PKG_UINT32(msg, (info + i)->skill_id, index);
		PKG_UINT32(msg, (info + i)->cold_time, index);
		PKG_UINT32(msg, (info + i)->used_count, index);
		PKG_UINT32(msg, (info + i)->type, index);
	}
	init_proto_head(msg, p->waitcmd, index);
	return send_to_self(p, msg, index, 1);
}

int farm_animal_use_skill_cmd(sprite_t * p,const uint8_t * body,int len)
{
	uint32_t animal_index = 0;
	uint32_t skill_id = 0;
	unpack(body, sizeof(animal_index)+sizeof(skill_id), "LL", &animal_index,&skill_id);
	*(uint32_t*)p->session  = skill_id;
	return send_request_to_db(SVR_PROTO_GET_ANIMAL_BASE_INFO, p, 4, &animal_index, p->id);
}

int cal_level_by_growth(uint32_t animalid, uint32_t growth)
{
	item_t *itm = NULL;
	itm = get_item_prop(animalid);
	if (!itm)
	{
		return 1;
	}

	int i = 0;
	for (i = 0; i < 3; i++)
	{
		if (growth < itm->u.animal_tag.levelNum[i])
		{
			return i+1;
		}
	}

	return i+1;

}

int check_skill_use_enable(level_skills_t * p_level_skills, uint32_t skill_id)
{
	uint32_t i = 0;
	for (i = 0; i < p_level_skills->skill_count; i++)
	{
		DEBUG_LOG("skill_id:%d level_skills[i].skill_id:%d",skill_id, p_level_skills->level_skills[i].skill_id);
		if (skill_id == p_level_skills->level_skills[i].skill_id)
		{
			return i+1;
		}
	}

	return 0;
}

int get_animal_base_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t skill_id = *(uint32_t*)p->session;
	typedef struct animal_base_info {
		uint32_t animal_index;
		uint32_t animal_id;
		uint32_t star_id;
		uint32_t growth;
		uint32_t flag;
	}__attribute__((packed)) animal_base_info_t;

	animal_base_info_t *info = (animal_base_info_t *)buf;
	uint32_t level_id = 1;
	if (info->flag == 1)
	{
		level_id = 4;
	}
	else
	{
		level_id = cal_level_by_growth(info->animal_id, info->growth);
	}

	*(uint32_t*)(p->session + 4) = info->star_id;

	DEBUG_LOG("animal_id:%d, star_id:%d level_id:%d skill_id:%d",info->animal_id,info->star_id, level_id, skill_id);

	uint32_t animal_index = info->animal_id-ANIMAL_ID_MIN_VALUE;
	level_skills_t * p_level_skills = &animals_skills[animal_index].animal_stars[info->star_id].star_levels[level_id];

	uint32_t index = check_skill_use_enable(p_level_skills, skill_id);
	if (!index)
	{
		DEBUG_LOG("index:%d",index);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint32_t buff[5] = {0};
	buff[0] = info->animal_index;
	buff[1] = skill_id;
	buff[2] = p_level_skills->level_skills[index-1].cold_time;
	buff[3] = p_level_skills->level_skills[index-1].max_time;
	buff[4] = p_level_skills->level_skills[index-1].type;
	return  send_request_to_db(SVR_PROTO_FARM_ANIMAL_USE_SKILL, p, 20, buff, p->id);

}

int farm_animal_use_skill_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	typedef struct use_skill_info {
		uint32_t animal_index;
		uint32_t skill_id;
		uint32_t cold_time;
		uint32_t used_count;
		uint32_t type;
	}__attribute__((packed)) use_skill_info_t;
	use_skill_info_t *p_info = (use_skill_info_t *)buf;

	uint32_t star_id = *(uint32_t*)(p->session + 4);

	if (p_info->skill_id == 2 || p_info->skill_id == 1 || p_info->skill_id == 4)
	{
		DEBUG_LOG("notify map skill_id:%d", p_info->skill_id);
		notify_animal_use_skill_map(p, p_info->skill_id);
		uint32_t stars_hot_value[] = {0, 1, 2 ,4};
		uint32_t hot_value = stars_hot_value[star_id];

		*(uint32_t*)p->session = p_info->animal_index;
		*(uint32_t*)(p->session + 4) = p_info->skill_id;
		*(uint32_t*)(p->session + 8) = p_info->cold_time;
		*(uint32_t*)(p->session + 12) = p_info->used_count;
		*(uint32_t*)(p->session + 16) = p_info->type;

		return  send_request_to_db(SVR_PROTO_SYSARG_ADD_ANIMAL_SKILL_COUNT, p, 4, &hot_value, p->id);
	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, p_info->animal_index, l);
	PKG_UINT32(msg, p_info->skill_id, l);
	PKG_UINT32(msg, p_info->cold_time, l);
	PKG_UINT32(msg, p_info->used_count, l);
	PKG_UINT32(msg, p_info->type, l);
	if (p_info->skill_id == 3) //使用蛋雨技能
	{
		uint32_t itemid = 1270077;
		uint32_t count = 1;
		PKG_UINT32(msg, 1, l);
		PKG_UINT32(msg, itemid, l);
		PKG_UINT32(msg, count, l);
		DEBUG_LOG("notify all skill_id:%d", p_info->skill_id);
		notify_animal_use_skill_all(p, p_info->skill_id);
		db_exchange_single_item_op(p, 202, itemid, count, 0);
		uint32_t mapids[] = {59,7,6,10,28,8,9,4,80,61,5,33,37,109,47,83,68,77,120,112,41};
		uint32_t num = 5;
		uint32_t i = 0;
		for (i = 0; i < num; i++)
		{
			int index = rand()%(sizeof(mapids)/4);
			int mapid = mapids[index];

			DEBUG_LOG("egg rain mapid :%d", mapid);

			int j = 0;
			for(j = 0; j < maps_poss[mapid].pos_count ;j++)
			{
				if (maps_poss[mapid].pos_infos[j].itemid == 0)
				{
					uint32_t itemids[] = {1270076,1270076,1270076,1270077,1270077};
					int idx = rand()%(sizeof(itemids)/4);
					maps_poss[mapid].pos_infos[j].itemid = itemids[idx];
					break;
				}
			}
		}

	}
	else
	{
		PKG_UINT32(msg, 0, l);
		notify_animal_use_skill_map(p, p_info->skill_id);
	 }

	DEBUG_LOG("rsp client skill_id:%d", p_info->skill_id);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int animal_add_use_skill_count_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t total = 0;
	uint32_t prize_flag = 0;
	unpkg_host_uint32_uint32((uint8_t *)buf, &total, &prize_flag);
	if (prize_flag)
	{
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x04041410 + prize_flag - 1,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		DEBUG_LOG("notify all prize item prize_flag:%d", prize_flag);

		if (prize_flag < 10){
			prize_flag = 1;
		}
		else{
			prize_flag = 2;
		}
		notify_use_skill_prize_across_svr(p, &prize_flag, 4);
	}

	uint32_t animal_index =  *(uint32_t*)p->session;
	uint32_t skill_id = *(uint32_t*)(p->session + 4);
	uint32_t cold_time = *(uint32_t*)(p->session + 8);
	uint32_t used_count = *(uint32_t*)(p->session + 12);
	uint32_t type = *(uint32_t*)(p->session + 16);
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, animal_index, l);
	PKG_UINT32(msg, skill_id, l);
	PKG_UINT32(msg, cold_time, l);
	PKG_UINT32(msg, used_count, l);
	PKG_UINT32(msg, type, l);
	PKG_UINT32(msg, 0, l);

	DEBUG_LOG("rsp client skill_id:%d", skill_id);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}


int notify_animal_use_skill_map(sprite_t* p, uint32_t skill_id)
{
	uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, p->id, l);
	PKG_UINT32(n_msg, skill_id, l);

	init_proto_head(n_msg, PROTO_NOTIFY_ANIMAL_USED_SKILL, l);
	send_to_map_except_self(p, n_msg, l, 0);
	return 0;

}

int notify_animal_use_skill_all(sprite_t* p, uint32_t skill_id)
{
	uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, p->id, l);
	PKG_UINT32(n_msg, skill_id, l);

	init_proto_head(n_msg, PROTO_NOTIFY_ANIMAL_USED_SKILL, l);
	send_to_all_player_except_self(p, n_msg, l, 0);
	return 0;

}

int notify_all_use_skill_prize(sprite_t* p, uint32_t prize_flag)
{
	uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, prize_flag, l);

	DEBUG_LOG("notify cmd %d ", PROTO_NOTIFY_ALL_USE_SKILL_PRIZE);
	init_proto_head(n_msg, PROTO_NOTIFY_ALL_USE_SKILL_PRIZE, l);
	send_to_all_players(p, n_msg, l, 0);
	return 0;

}

int notify_one_use_skill_prize(sprite_t* p, uint32_t prize_flag)
{
	uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, prize_flag, l);

	DEBUG_LOG("notify cmd %d ", PROTO_NOTIFY_ALL_USE_SKILL_PRIZE);
	init_proto_head(n_msg, PROTO_NOTIFY_ALL_USE_SKILL_PRIZE, l);
	send_to_self(p, n_msg, l, 0);
	return 0;

}

static int parse_single_pos(map_pos_info_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Egg")))
		{
			DECODE_XML_PROP_INT (iut[j].posid, cur, "posid");
			DECODE_XML_PROP_INT_DEFAULT(iut[j].x, cur, "x", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[j].y, cur, "y", 0);
			DECODE_XML_PROP_INT_DEFAULT(iut[j].itemid, cur, "itemid", 0);
			j++;
		}
		cur = cur->next;
	}

	*cnt = j;

	return 0;
}


int load_rain_egg_map_pos_conf(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int ex_count;

	ex_count = 0;
	memset(maps_poss, 0, sizeof(maps_poss));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Map")))
		{
			DECODE_XML_PROP_INT(i, cur, "mapid");
			if (ex_count >= ANIMAL_RAIN_EGG_MAP_MAX - 1 || i >= ANIMAL_RAIN_EGG_MAP_MAX)
			{
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}

			maps_poss[i].mapid= i;

			chl = cur->xmlChildrenNode;
			if ( (parse_single_pos(maps_poss[i].pos_infos, &(maps_poss[i].pos_count), chl) != 0) )
				goto exit;

			ex_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load rain egg map pos file %s", file);

}

int get_map_rain_egg_info_cmd(sprite_t *p, uint8_t body[], int len)
{
	uint32_t mapid = 0;
	unpack(body, sizeof(mapid), "L", &mapid);
	if (mapid >= 200 || maps_poss[mapid].mapid != mapid)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint32_t cnt = 0;
	int index = sizeof(protocol_t);
	int old_index = index;
	PKG_UINT32(msg, maps_poss[mapid].pos_count, index);
	int i = 0;
	for (i = 0; i < maps_poss[mapid].pos_count; i++) {
		if(maps_poss[mapid].pos_infos[i].itemid != 0)
		{
			PKG_UINT32(msg, maps_poss[mapid].pos_infos[i].posid, index);
			PKG_UINT32(msg, maps_poss[mapid].pos_infos[i].x, index);
			PKG_UINT32(msg, maps_poss[mapid].pos_infos[i].y, index);
			PKG_UINT32(msg, maps_poss[mapid].pos_infos[i].itemid, index);
			cnt++;
		}
	}
	PKG_UINT32(msg, cnt, old_index);
	init_proto_head(msg, p->waitcmd, index);
	return send_to_self(p, msg, index, 1);

}

int gain_map_pos_egg_cmd(sprite_t *p, uint8_t body[], int len)
{
	uint32_t mapid = 0;
	uint32_t posid = 0;
	uint32_t itemid = 0;
	uint32_t flag = 0;
	unpack(body, sizeof(mapid)+sizeof(posid), "LL", &mapid,&posid);
	if (mapid >= 200 || maps_poss[mapid].mapid != mapid)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	int i = 0;
	for (i = 0; i < maps_poss[mapid].pos_count; i++)
	{
		if(maps_poss[mapid].pos_infos[i].posid == posid)
		{
			if(maps_poss[mapid].pos_infos[i].itemid != 0)
			{
				itemid = maps_poss[mapid].pos_infos[i].itemid;
				flag = 1;
			}
		}
	}
	if (!flag)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_farm_animal_get_map_pos_egg_over, 1);
	}

	*(uint32_t*)p->session = mapid;
	*(uint32_t*)(p->session + 4) = posid;
	*(uint32_t*)(p->session + 8) = itemid;

	return  db_set_sth_done(p, 40002, 5, p->id);
}

int gain_map_pos_egg(sprite_t *p)
{
	uint32_t mapid = *(uint32_t*)p->session;
	uint32_t posid = *(uint32_t*)(p->session + 4);
	uint32_t itemid = *(uint32_t*)(p->session + 8);
	int i = 0;
	for (i = 0; i < maps_poss[mapid].pos_count; i++)
	{
		if(maps_poss[mapid].pos_infos[i].posid == posid)
		{
			maps_poss[mapid].pos_infos[i].itemid = 0;
		}
	}

	db_exchange_single_item_op(p, 202, itemid, 1, 0);
	DEBUG_LOG("gain_map_pos_egg itemid:%d, posid:%d", itemid, posid);
	response_proto_uint32_uint32(p, p->waitcmd, itemid, posid, 0);
	return 0;

}


int gain_use_skill_prize_item_cmd(sprite_t *p, uint8_t body[], int len)
{
	return  db_set_sth_done(p, 40003, 10, p->id);
}

int gain_use_skill_prize_item(sprite_t *p)
{
	return  send_request_to_db(SVR_PROTO_SYSARG_DEC_ANIMAL_SKILL_COUNT, p, 0, NULL, p->id);
}

int animal_dec_use_skill_count_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t prize_flag = 0;
	unpkg_host_uint32((uint8_t *)buf, &prize_flag);
	uint32_t itemid = 0;
	uint32_t count = 0;
	uint32_t rand_value = rand()%100;
	if (prize_flag < 10)
	{
		if (rand_value >=0 && rand_value < 20)
		{
			itemid = 0;
			count = 100;
		}
		else if (rand_value >=20 && rand_value < 40)
		{
			itemid = 0;
			count = 300;
		}
		else if (rand_value >=40 && rand_value < 50)
		{
			itemid = 190840;
			count = 3;
		}
		else if (rand_value >=50 && rand_value < 60)
		{
			itemid = 190309;
			count = 5;
		}
		else if (rand_value >=60 && rand_value < 70)
		{
			itemid = 1230036;
			count = 1;
		}
		else if (rand_value >=70 && rand_value < 80)
		{
			itemid = 190809;
			count = 2;
		}
		else if (rand_value >=80 && rand_value < 90)
		{
			itemid = 1270076;
			count = 1;
		}
		else
		{
			itemid = 190602;
			count = 5;
		}

	}
	else
	{
		if (rand_value >=0 && rand_value < 20)
		{
			itemid = 0;
			count = 500;
		}
		else if (rand_value >=20 && rand_value < 40)
		{
			itemid = 1230052;
			count = 1;
		}
		else if (rand_value >=40 && rand_value < 60)
		{
			itemid = 190840;
			count = 5;
		}
		else if (rand_value >=60 && rand_value < 70)
		{
			itemid = 1230036;
			count = 2;
		}
		else if (rand_value >=70 && rand_value < 80)
		{
			itemid = 190602;
			count = 20;
		}
		else if (rand_value >=80 && rand_value < 90)
		{
			itemid = 1230068;
			count = 2;
		}
		else if (rand_value >=90 && rand_value < 95)
		{
			itemid = 16012;
			count = 5;
		}
		else
		{
			itemid = 1270078;
			count = 1;
		}
	}

	db_exchange_single_item_op(p, 202, itemid, count, 0);
	response_proto_uint32_uint32(p, p->waitcmd, itemid, count, 0);
	return 0;

}

int get_use_skill_prize_info_cmd(sprite_t *p, uint8_t body[], int len)
{
	return  send_request_to_db(SVR_PROTO_SYSARG_GET_USE_SKILL_INFO, p, 0, NULL, p->id);
}

int get_use_skill_prize_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t skill_count = 0;
	uint32_t prize_count = 0;
	uint32_t prize_flag = 0;
	unpkg_host_uint32_uint32_uint32((uint8_t *)buf, &skill_count,&prize_count,&prize_flag);
	response_proto_uint32_uint32_uint32(p, p->waitcmd, skill_count, prize_count,prize_flag, 0);
	return 0;
}


/* @brief 得到极品圣光兽的数目
 */
int get_very_good_animal_count_cmd(sprite_t *p, uint8_t body[], int len)
{
	uint32_t user_id = 0;
	unpack(body, sizeof(user_id), "L", &user_id);
	*(uint32_t*)p->session = user_id;
	return send_request_to_db (SVR_PROTO_GET_VERY_GOOD_ANIMAL_COUNT, p, 0, NULL, user_id);
}

/* @brief 得到圣光兽的DB返回处理函数
 */
int get_very_good_animal_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t user_id = *(uint32_t*)p->session;
	uint32_t num = *(uint32_t *)buf;
	response_proto_uint32(p, p->waitcmd, num, 0);
	if (user_id == p->id)
	{
		p->animal_count = num;
	}
	return 0;
}
