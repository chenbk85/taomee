
#include <libtaomee/dataformatter/bin_str.h>

#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "homeinfo.h"
#include "cache_svr.h"
#include "small_require.h"
#include "mole_pasture.h"

#include "mole_homeland.h"

static bg_seed_grid_t bg_grid[MAX_BG_NUM] = {
					{1220001, {{306, 416}, {390, 416}, {477, 416}, {560, 416}}},
       				{1220002, {{265, 421}, {504, 418}, {618, 415}, {382, 419}, {392, 326}, {500, 325}}},
					{1220011, {{150, 356}, {112, 429}, {834, 414}, {793, 340}, {737, 418}}},
					{1220017, {{797, 428}, {760, 331}, {125, 428}, {164, 334}}},
					{1220041, {{170, 427}, {498, 416}, {813, 417}}},
					{1220059, {{121, 417}, {470, 445}, {566, 445}, {376, 445}, {154, 335}, {760, 249}, {800, 327}, {844, 403}}},
					{1220073, {{110, 328}, {65, 427}, {789, 331}, {734, 428}, {848, 425}}},
					{1220083, {{441, 426}, {134, 353}, {184, 426}, {320, 444}, {486, 349}, {714, 417}, {832, 417}, {813, 317}}},
					{1220084, {{413, 371}, {126, 362}, {608, 368}, {730, 353}, {844, 357}, {832, 265}}},
					{1220102, {{101, 405}, {120, 326}, {220, 434}, {337, 434}, {605, 439}, {724, 434}}},
					{1220110, {{824, 430}, {802, 342}, {717, 434}, {712, 344}, {509, 436}, {617, 434}}},
					{1220117, {{790, 454}, {790, 364}, {669, 454}, {668, 366}, {427, 456}, {551, 454}, {311, 364}, {311, 454}, {189, 366}, {189, 454}}},
					{1220123, {{816, 432}, {790, 326}, {689, 432}, {429, 432}, {551, 432}, {295, 432}, {195, 326}, {165, 432}}},
					{1220124, {{858, 458}, {790, 378}, {749, 464}, {531, 468}, {639, 466}, {425, 468}, {567, 380}, {681, 376}}},
					{1220138, {{724, 360}, {612, 364}, {617, 456}, {383, 456}, {501, 454}, {269, 456}, {389, 360}, {501, 360}}},
					{1220154, {{765, 369}, {640, 369}, {168, 443}, {294, 443}, {423, 442}}},
					{1220159, {{582, 463}, {287, 463}, {159, 358}, {281, 360}, {154, 463},{709,463},{836,465},{831,352}}},
					{1220171, {{559, 443}, {446, 443}, {107, 443}, {221, 443}, {333, 442},{109, 331}}},
					{1220172, {{787, 358}, {727, 442}, {232, 442}, {157, 358}, {257, 291},{688, 291}}},
					{1220187, {{713, 369}, {600, 420}, {220, 378}, {345, 414}, {470, 440}}},
					{1220182, {{705, 374}, {569, 441}, {102, 398}, {246, 442}, {408, 459}, {794, 278}}},
					{1220194, {{579, 336}, {553, 422}, {376, 336}, {400, 422}, {477, 355}, {476, 286}}},
					{1220195, {{831, 346}, {81, 358}, {176, 462}, {735, 448}}},
					{1220211, {{620, 447}, {144, 348}, {225, 402}, {327, 445},{729, 423},{814, 369},{876, 308}}},
					{1220212, {{535, 439}, {147, 365}, {278, 421}, {673, 424},{801, 382},{853, 309}}},
					{1220236, {{531, 436}, {98, 256}, {208, 439}, {623, 388},{673, 313},{60,327},{108, 401}}},
					{1220241, {{562, 419}, {95, 276}, {243, 420}, {670, 419},{778, 421},{60, 345},{136, 402}}},
					{1220242, {{667, 447}, {211, 385}, {767, 426}, {852, 372},{294, 440}}},
					{1220247, {{720, 381}, {504, 390}, {617, 430}, {815, 332},{703, 286}}},
					{1220252, {{816, 412}, {285, 438}, {698, 446}, {873, 321},{410, 461},{558, 464}}},
					{1220257, {{632, 440}, {201, 440}, {529, 440}, {732, 440},{306, 440},{407, 440}}},
					{1220262, {{601, 391}, {168, 347}, {493, 386}, {761, 374},{245, 438},{365, 402},{889, 358},{836, 279}}},
};

int  is_grid_valid(uint32_t itmid, uint32_t x, uint32_t y)
{
	int loop;
	for (loop = 0; loop < sizeof(bg_grid)/(sizeof(bg_seed_grid_t)); loop++) {
		DEBUG_LOG("itmid=%u", bg_grid[loop].itmid);
		if (bg_grid[loop].itmid == itmid) {
			int i;
			for (i = 0; i < MAX_GRID_NUM; i++){
				if (bg_grid[loop].grid[i].x == x && bg_grid[loop].grid[i].y == y) {
					return 1;
				}
				DEBUG_LOG("itmid=%u x=%u y=%u", bg_grid[loop].itmid, bg_grid[loop].grid[i].x, bg_grid[loop].grid[i].y);
			}
		}
	}
	return 0;
}

#define db_get_homeland_item(p_, id) \
		send_request_to_db(SVR_PROTO_GET_HOMELAND_ITEM, p_, 0, NULL, id)
#define db_homeland_plant(p_, buf) \
		send_request_to_db(SVR_PROTO_HOMELAND_PLANT, p_, 12, buf, (p_)->id)
#define db_root_out_plant(p_, buf) \
		send_request_to_db(SVR_PROTO_ROOT_OUT_PLANT, p_, 4, buf, (p_)->id)
#define db_water_plant(p_, buf, id) \
		send_request_to_db(SVR_PROTO_WATER_PLANT, p_, 8, buf, id)
#define db_kill_bug(p_, buf, id) \
		send_request_to_db(SVR_PROTO_KILL_BUG, p_, 8, buf, id)
#define db_get_one_plant(p_, buf, id) \
		send_request_to_db(SVR_PROTO_GET_ONE_PLANT, p_, 4, buf, id)
#define db_harvest_fruits(p_, buf) \
		send_request_to_db(SVR_PROTO_HARVEST_FRUITS, p_, 4, buf, (p_)->id)
#define db_get_homeland_item_in_box(p_, buf) \
		send_request_to_db(SVR_PROTO_GET_HOMELAND_BOX_ITEM, p_, 8, buf, (p_)->id)
#define db_set_homeland_used_itm(p_, uid_, buf_, len_) \
		send_request_to_db(SVR_PROTO_SET_JY_USED_ITM, (p_), (len_), (buf_), (uid_))
#define db_chk_if_user_exist(p_, uid_) \
		send_request_to_db(SVR_PROTO_CHK_IF_USER_EXIST, (p_), 0, NULL, (uid_))
#define db_thief_fruit(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_THIEF_FRUIT, (p_), 8, buf_, (uid_))
#define db_dec_pollinate_cnt(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_DEC_POLLINATE_CNT, (p_), 4, buf_, (uid_))
#define db_pollinate_flower(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_POLLINATE_FLOWER, (p_), 4, buf_, (uid_))


int enter_homeland_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t id;
	if (unpkg_uint32(body, len, &id) == -1) return -1;

	CHECK_VALID_ID(id);

	return db_get_homeland_item(p, id);
}

int get_homeland_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 12);

	uint32_t home_bg_itm, itm_count, plant_count;
	int j = 0;
	UNPKG_H_UINT32(buf, home_bg_itm, j);
	UNPKG_H_UINT32(buf, itm_count, j);
	UNPKG_H_UINT32(buf, plant_count, j);

	CHECK_BODY_LEN(len, 12 + itm_count * 16 + plant_count * sizeof(plant_attr_t));

	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, id, i);
	if (p->id == id) {
		PKG_STR(msg, p->nick, i, sizeof p->nick);
		PKG_UINT32 (msg, 1, i);
	} else {
		sprite_t* op = get_sprite(id);
		if (!op)
			op = get_across_svr_sprite(id, id);

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

	PKG_UINT32(msg, home_bg_itm, i);
	PKG_UINT32(msg, itm_count, i);
	PKG_UINT32(msg, plant_count, i);
	int loop;
	for (loop = 0; loop < itm_count; loop++) {
		uint32_t itemid;
		uint16_t x, y;

		UNPKG_H_UINT32(buf, itemid, j);
		PKG_UINT32(msg, itemid, i);

		UNPKG_H_UINT16(buf, x, j);
		PKG_UINT16(msg, x, i);

		UNPKG_H_UINT16(buf, y, j);
		PKG_UINT16(msg, y, i);

		memcpy (msg + i, buf + j, 8);
		i += 8;
		j += 8;
		//DEBUG_LOG("itmid=%u %u %u %u %u %u %u %u", itemid, x, y, *(msg+i-8), *(msg+i-7), *(msg+i-6), *(msg+i-5), *(msg+i-4));
	}

	for (loop = 0; loop < plant_count; loop++) {

		plant_attr_t* plant = (plant_attr_t*)(buf + j);
		PKG_UINT32(msg, plant->number, i);
		PKG_UINT32(msg, plant->itemid, i);
		PKG_UINT16(msg, plant->x, i);
		PKG_UINT16(msg, plant->y, i);
		PKG_UINT32(msg, plant->value, i);
		PKG_UINT32(msg, plant->sickflag, i);
		PKG_UINT32(msg, plant->fruit_num, i);
		PKG_UINT32(msg, plant->fruit_status, i);
		PKG_UINT32(msg, plant->mature_time, i);
		PKG_UINT32(msg, plant->diff_mature_time, i);
		PKG_UINT32(msg, plant->cur_grow_rate, i);
		PKG_UINT32(msg, plant->earth, i);
		PKG_UINT32(msg, plant->pollinate_num, i);
		uint32_t thiefable = (get_now_tv()->tv_sec > plant->mature_time + 7*24*60*60 && p->id != id) ? 1 : 0;
		if (plant->itemid == 1230032 && plant->mature_time)
			thiefable = 1;
		/*如果是孢子花，不能偷取*/
		if (plant->itemid == 1230055 || plant->itemid == 1230056) {
			thiefable = 0;
		}
		PKG_UINT32(msg, thiefable, i);
		j += sizeof(plant_attr_t);

	}

	init_proto_head (msg, PROTO_GET_HOMELAND_INFO, i);
	DEBUG_LOG ("USER HOMELAND\t[%u %u %u %u %u]", p->id, id, home_bg_itm, itm_count, plant_count);
	return send_to_self (p, msg, i, 1);

}

static int
unpkg_set_homeland_item(uint8_t* body, int len, int *used, int *not_used)
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


int set_homeland_items_cmd(sprite_t* p, uint8_t* body, int len)
{
	int used, not_used;
	CHECK_VALID_ID(p->id);

	if (unpkg_set_homeland_item (body, len, &used, &not_used) == -1)
		return -1;

	DEBUG_LOG("SET HOMELAND\t[%u %d %d]", p->id, used, not_used);
	cs_set_jy_and_pet_state(0, 1, get_now_tv()->tv_sec, p->id);
	return db_set_homeland_item(p, body, len);

}

int get_homeland_box_items_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);

	uint32_t tmp_buf[2]; // start itemid  and end itemid
	get_item_kind_range (HOMELAND_SEED_KIND, &tmp_buf[0],  &tmp_buf[1]);

	DEBUG_LOG("GET HOMELAND BOX ITEM\t[uid=%u start=%u end=%u", p->id, tmp_buf[0], tmp_buf[1]);
	return db_get_homeland_item_in_box(p, tmp_buf);
}

int get_homeland_box_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
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
		//DEBUG_LOG ("GET HOMELAND BOX ITEM\t[%u %u]", p->id, itm);
	}

	init_proto_head (msg, p->waitcmd, j);

	DEBUG_LOG ("GET HOMELAND BOX ITEM\t[%u %d]", p->id, count);
	return send_to_self (p, msg, j, 1);

}

int homeland_plant_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 16);

	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in JY\t[uid=%u]", p->id), -1);

	uint32_t tmp_buf[4]={0};;
	int j = 0;
	UNPKG_UINT32(body, tmp_buf[0], j);  // background id
	UNPKG_UINT32(body, tmp_buf[1], j);	// itemid
	UNPKG_UINT32(body, tmp_buf[2], j);  // x  grid
	UNPKG_UINT32(body, tmp_buf[3], j);  // y  grid

	if (!is_grid_valid(tmp_buf[0], tmp_buf[2], tmp_buf[3])) {
		ERROR_RETURN(("error grid\t[uid=%u bg=%u itmid=%u x=%u y=%u]", p->id, tmp_buf[0], tmp_buf[1], tmp_buf[2], tmp_buf[3]), -1);
	}

	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0405BEA2, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	DEBUG_LOG("PLANT SEED\t[uid=%u bakgrid=%u itemid=%u x=%u y=%u]", p->id, tmp_buf[0], tmp_buf[1], tmp_buf[2], tmp_buf[3]);
	return db_homeland_plant(p, &tmp_buf[1]);
}

static inline int
rsp_plant_attr(sprite_t* p, plant_attr_t* plant, uint32_t uid)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, plant->number, i);
	PKG_UINT32(msg, plant->itemid, i);
	PKG_UINT16(msg, plant->x, i);
	PKG_UINT16(msg, plant->y, i);
	PKG_UINT32(msg, plant->value, i);
	PKG_UINT32(msg, plant->sickflag, i);
	PKG_UINT32(msg, plant->fruit_num, i);
	PKG_UINT32(msg, plant->fruit_status, i);
	PKG_UINT32(msg, plant->mature_time, i);
	PKG_UINT32(msg, plant->diff_mature_time, i);
	PKG_UINT32(msg, plant->cur_grow_rate, i);
	PKG_UINT32(msg, plant->earth, i);
	PKG_UINT32(msg, plant->pollinate_num, i);
	uint32_t thiefable = (plant->mature_time && get_now_tv()->tv_sec > plant->mature_time + 7*24*60*60 && p->id != uid) ? 1 : 0;
	if (plant->itemid == 1230032 && plant->mature_time)
		thiefable = 1;
	/*如果是孢子花，不能偷取*/
	if (plant->itemid == 1230055 || plant->itemid == 1230056) {
		thiefable = 0;
	}

	PKG_UINT32(msg, thiefable, i);

	init_proto_head (msg, p->waitcmd, i);
	return send_to_self (p, msg, i, 1);
}

int homeland_plant_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(plant_attr_t));

	add_cultivate_exp(p, 2);

	plant_attr_t* plant = (plant_attr_t*)(buf);
	DEBUG_LOG ("HOMELAND PLANT CALLBACK\t[%u %u %u]", p->id, id, plant->number);

	msglog(statistic_logfile, 0x04040109,get_now_tv()->tv_sec, &(p->id), 4);
	return rsp_plant_attr(p, plant, id);
}

int homeland_root_out_plant_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 8);


	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in JY\t[uid=%u]", p->id), -1);

	uint32_t number, root_type;
	int j = 0;
	UNPKG_UINT32(body, number, j);
	UNPKG_UINT32(body, root_type, j);
	*(uint32_t*)p->session = number;
	*(uint32_t*)(p->session + 4) = root_type;
	DEBUG_LOG("ROOT OUT\t[%u number=%u type=%u]", p->id, number, root_type);
	if (root_type != 0 && root_type != 1) {
		ERROR_RETURN(("Invalid type"), -1);
	}
	return db_root_out_plant(p, &number);
}

int homeland_root_out_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t itms[] = {0, 1230004, 1230007, 1230009, 1230020,1230012};
	uint32_t nbr = *(uint32_t*)p->session;
	uint32_t root_type = *(uint32_t*)(p->session + 4);
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, nbr, i);  // number
	init_proto_head(msg, PROTO_PLANT_DISAPPEAR, i);
	send_to_map(p, msg, i, 0);

	if (root_type == 1 && (rand() % 2) < 1) {
		int indx = rand() % (sizeof(itms)/sizeof(itms[0]));
		int count = 1;
		if (indx == 0) {
			db_change_xiaome(0, 100, 0, 0, p->id);
			p->yxb += 100;
			count = 100;
		} else {
			db_single_item_op(0, p->id, itms[indx], 1, 1);
		}
		int j = sizeof(protocol_t);
		PKG_UINT32(msg, nbr, j);
		PKG_UINT32(msg, 1, j);
		PKG_UINT32(msg, itms[indx], j);
		PKG_UINT32(msg, count, j);
		init_proto_head(msg, p->waitcmd, j);
		send_to_self(p, msg, j, 1);
	} else {
		response_proto_uint32_uint32(p, p->waitcmd, nbr, 0, 0);
	}
	if (root_type == 1)
		add_cultivate_exp(p, 2);

	return 0;
}

int homeland_water_plant_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 8);

	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in JY\t[uid=%u]", p->id), -1);

	userid_t userid;
	uint32_t tmp_buf[2];
	int j = 0;
	UNPKG_UINT32(body, userid, j);
	UNPKG_UINT32(body, tmp_buf[0], j);  // which number
	tmp_buf[1] = p->id;

	DEBUG_LOG("WATER PLANT\t[%u %u number=%u]", p->id, userid, tmp_buf[0]);
	return db_water_plant(p, tmp_buf, userid);
}

int homeland_water_plant_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(plant_attr_t));

	add_cultivate_exp(p, 2);
	plant_attr_t* plant = (plant_attr_t*)buf;
	DEBUG_LOG ("HOMELAND WATER CALLBACK\t[%u %u %u]", p->id, id, plant->number);

	return rsp_plant_attr(p, plant, id);
}

int homeland_kill_bug_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 8);

	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in JY\t[uid=%u]", p->id), -1);

	userid_t userid;
	uint32_t tmp_buf[2];
	int j = 0;
	UNPKG_UINT32(body, userid, j);
	UNPKG_UINT32(body, tmp_buf[0], j);  //which number
	tmp_buf[1] = p->id;

	DEBUG_LOG("KILL BUG\t[%u %u number=%u]", p->id, userid, tmp_buf[0]);
	return db_kill_bug(p, tmp_buf, userid);
}

int homeland_kill_bug_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(plant_attr_t));

	add_cultivate_exp(p, 2);
	plant_attr_t* plant = (plant_attr_t*)buf;
	DEBUG_LOG ("HOMELAND KILL BUG CALLBACK\t[%u %u %u]", p->id, id, plant->number);

	return rsp_plant_attr(p, plant, id);
}

int homeland_get_one_plant_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 8);

	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in JY\t[uid=%u]", p->id), -1);

	userid_t userid;
	uint32_t number;
	int j = 0;
	UNPKG_UINT32(body, userid, j);
	UNPKG_UINT32(body, number, j);

	DEBUG_LOG("GET ONE PLANT\t[%u %u number=%u]", p->id, userid, number);
	return db_get_one_plant(p, &number, userid);
}

int homeland_get_one_plant_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(plant_attr_t));

	plant_attr_t* plant = (plant_attr_t*)buf;
	DEBUG_LOG ("GET ONE PLANT CALLBACK\t[%u %u %u]", p->id, id, plant->number);

	return rsp_plant_attr(p, plant, id);
}

int homeland_harvest_fruits_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 4);

	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in JY\t[uid=%u]", p->id), -1);

	uint32_t number;
	int j = 0;
	UNPKG_UINT32(body, number, j);

	*(uint32_t*)p->session = number;
	DEBUG_LOG("HARVEST FRUITS\t[%u number=%u]", p->id, number);
	return db_harvest_fruits(p, &number);
}

int homeland_harvest_fruits_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);
	uint32_t number = *(uint32_t*)p->session;
	uint32_t old_cultivate_skill = calculation_level_from_skill(p->cultivate_skill);
	int cultiv_diff = *(uint32_t*)buf - p->cultivate_skill;
	notify_skill_change(p, cultiv_diff, 0);
	DEBUG_LOG("qianshu %u %u", cultiv_diff, p->id);
	p->cultivate_skill = *(uint32_t*)buf;
	uint32_t new_cultivate_skill = calculation_level_from_skill(p->cultivate_skill);
	level_change_and_send_postcard(new_cultivate_skill, old_cultivate_skill, 1000109, p->id);
	uint32_t itmid = *(uint32_t*)(buf + 4);
	uint32_t count = *(uint32_t*)(buf + 8);

	DEBUG_LOG("HARVEST FRUITS CALLBACK\t[%u itmid=%u count=%u]", p->id, itmid, count);
	msglog(statistic_logfile, 0x04040109,get_now_tv()->tv_sec, &(p->id), 4);

	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0405BEA3, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, number, i);
	PKG_UINT32(msg, itmid, i);
	PKG_UINT32(msg, count, i);


	init_proto_head(msg, p->waitcmd, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

int sell_fruits_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);

	uint32_t itmid, itmcnt;
	int i = 0;
	UNPKG_UINT32(body, itmid, i);
	UNPKG_UINT32(body, itmcnt, i);

	if (itmcnt > 100 || itmcnt == 0) {
		return send_to_self_error(p,p->waitcmd,-ERR_can_not_sell_too_many,1);
	}

	item_t* itm_prop = get_item_prop(itmid);
	if (!itm_prop || !itm_sellable(itm_prop)) {
		ERROR_RETURN( ("Item %u not found or not sellable: uid=%u itmflag=0x%X",
						itmid, p->id, (itm_prop ? itm_prop->tradability : 0)), -1 );
	}

	item_kind_t* kind = find_kind_of_item(itmid);
	if (!kind || !(kind->kind == SPECIAL_ITEM_KIND || kind->kind == FIGHT_CARD_KIND))
		ERROR_RETURN(("error item kind, item=%u, uid=%u", itmid, p->id), -1);

	char buf[20];
	int j = 0;
	pkg_item_kind(p, (uint8_t*)buf, itmid, &j);
	//PKG_H_UINT32(buf, 0, j);  // 0 for cloth
	PKG_H_UINT32(buf, itmid, j);
	PKG_H_UINT32(buf, itmcnt, j);
	PKG_H_UINT32(buf, itm_prop->max, j);
	PKG_H_UINT32(buf, itm_prop->sell_price * itmcnt, j);
	DEBUG_LOG("SELL FRUITS\t[uid=%u itm=%u cnt=%u price=%u]", p->id, itmid, itmcnt, itm_prop->sell_price);

	return send_request_to_db(SVR_PROTO_SELL_ITEM, p, 20, buf, p->id);

}

/*
 *@ list recent 50 visitors
 */
int get_recent_jy_visitors_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	int j = 0;
	userid_t uid = 0;
	UNPKG_UINT32(body, uid, j);

	return db_list_recent_jy_visitors(p, uid);
}

/*
 *@ get recent visitors information from db, handle db's reply
 */
int get_recent_jy_visitors_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	visitor_list_res_t* vp = (visitor_list_res_t*)buf;
	CHECK_BODY_LEN(len, vp->cnt * sizeof(visitor_jy_t) + 4);

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

static int
unpkg_homeland_used_item(uint8_t* body, int len, int *used)
{
	int j, i;
	home_item_t *hi;

	if (len < 4)
		ERROR_RETURN (("error len=%d", len), -1);

	i = 0;
	j = 0;
	UNPKG_UINT32 (body, *used, i);
	PKG_H_UINT32 (body, *used, j);

	CHECK_BODY_LEN (len, 4 + (*used) * sizeof (home_item_t));

	for (j = 0; j < *used; j++) {
		hi = (home_item_t*) (body + 4 + j * sizeof (home_item_t));
		hi->itemid = ntohl (hi->itemid);
		hi->x = ntohs (hi->x);
		hi->y = ntohs (hi->y);

		DEBUG_LOG("itmid=%u %u %u %u %u %u %u", hi->itemid, hi->x, hi->y, hi->way, hi->visible, hi->layer, hi->type);
		if (!get_item_prop (hi->itemid))
			ERROR_RETURN (("error item id=%d", hi->itemid), -1);
	}

	return 0;
}


int set_homeland_used_itms_cmd(sprite_t* p, uint8_t* body, int len)
{
	int used;
	CHECK_VALID_ID(p->id);

	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in JY\t[uid=%u]", p->id), -1);

	if (unpkg_homeland_used_item (body, len, &used) == -1)
		return -1;

	DEBUG_LOG("SET USED\t[%u %d]", p->id, used);
	return db_set_homeland_used_itm(p, p->id, body, len);
}

int chk_if_user_exist_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);

	userid_t uid;
	int j = 0;
	UNPKG_UINT32(body, uid, j);

	return db_chk_if_user_exist(p, uid);
}

int chk_if_user_exist_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t* exist = (uint32_t*)buf;   // 1 exist , 0 not exist
	response_proto_uint32(p, p->waitcmd, *exist, 0);
	return 0;
}

/*
 *@ get fruit free
 */
int get_fruit_free_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID(p->id);

	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in homeland\t[uid=%u]", p->id), -1);

	int j = 0;
	uint32_t nbr;
	UNPKG_UINT32(body, nbr, j);

	/*
	item_kind_t* kind = find_kind_of_item(itemid);
	if ( !kind || (SPECIAL_ITEM_KIND != kind->kind) || itemid == 190534) {
		ERROR_RETURN(("Wrong Item: itmid=%u uid=%u", itemid, p->id), -1);
	}
	*/
	p->sess_len = 0;
	PKG_H_UINT32(p->session, nbr, p->sess_len);

	return db_set_sth_done(p, 115, 10, p->id);
}

int add_culti_breed_exp_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	p->cultivate_skill = *(uint32_t*)buf;

	uint32_t itmid = *(uint32_t*)p->session;
	response_proto_uint32(p, p->waitcmd, itmid, 0);
	return 0;
}

int get_fruit_from_nbr_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t itmid = *(uint32_t*)buf;

	item_kind_t* kind = find_kind_of_item(itmid);
	if ( !kind || (SPECIAL_ITEM_KIND != kind->kind)) {
		itmid = 190028;
	}

	if (itmid == 190807)
	{
	    ERROR_RETURN(("error itmid \t[uid=%u]", p->id), -1);
	}

	return do_get_fruit_free(p, itmid);
}

/*
 *@ get fruit free when rabbit apper
 */
int do_get_fruit_free(sprite_t* p, uint32_t itmid)
{
	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in homeland\t[uid=%u]", p->id), -1);

	userid_t userid = GET_UID_IN_JY(p->tiles->id);
	sprite_t* ps = get_sprite(userid);
	map_t* mp = get_map(userid);
	if (mp) {
		int iloop;
		for (iloop = 0; iloop < MAX_PETS_PER_SPRITE; iloop++) {
			pet_t* pet = &(mp->pets[iloop]);
			char txt[128];
			int out_reason = 0;
			if (pet_has_learned(pet, 103)) {
				out_reason = 103;
				break;
			}
			if (out_reason == 103) {
			#ifdef TW_VER
				if (ps)
					snprintf(txt, sizeof txt, "　　親愛的%s，你不在家的時候啊，有壞兔子來偷菜哦！不過放心，我已經趕跑它了。我們的家園有我守護，你就放心吧！", ps->nick);
				else
					snprintf(txt, sizeof txt, "　　親愛的%d，你不在家的時候啊，有壞兔子來偷菜哦！不過放心，我已經趕跑它了。我們的家園有我守護，你就放心吧！", userid);
			#else
				if (ps)
					snprintf(txt, sizeof txt, "　　亲爱的%s，你不在家的时候啊，有坏兔子来偷菜哦！不过放心，我已经赶跑它了。我们的家园由我守护，你就放心吧！", ps->nick);
				else
					snprintf(txt, sizeof txt, "　　亲爱的%d，你不在家的时候啊，有坏兔子来偷菜哦！不过放心，我已经赶跑它了。我们的家园由我守护，你就放心吧！", userid);
			#endif
				send_postcard(pet->nick, 0, userid, 1000090, txt, 0);
				break;
			}
		}
	}
	//uint32_t itmid = *(uint32_t*)p->session;
	db_single_item_op(0, p->id, itmid, 1, 1);
	response_proto_uint32(p, p->waitcmd, itmid, 0);
	add_cultivate_exp(p, 2);
	return 0;
}

int homeland_thief_fruit_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 4);

	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in homeland\t[uid=%u]", p->id), -1);

	userid_t userid = GET_UID_IN_JY(p->tiles->id);
	uint32_t tmp_buf[2];
	int j = 0;
	UNPKG_UINT32(body, tmp_buf[0], j);  // which number
	tmp_buf[1] = p->id;

	DEBUG_LOG("THIEF FRUIT\t[%u %u number=%u]", p->id, userid, tmp_buf[0]);
	return db_thief_fruit(p, tmp_buf, userid);
}

static void rsp_plant_attr_to_map(sprite_t* p, plant_attr_t* plant)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, plant->number, i);
	PKG_UINT32(msg, plant->itemid, i);
	PKG_UINT16(msg, plant->x, i);
	PKG_UINT16(msg, plant->y, i);
	PKG_UINT32(msg, plant->value, i);
	PKG_UINT32(msg, plant->sickflag, i);
	PKG_UINT32(msg, plant->fruit_num, i);
	PKG_UINT32(msg, plant->fruit_status, i);
	PKG_UINT32(msg, plant->mature_time, i);
	PKG_UINT32(msg, plant->diff_mature_time, i);
	PKG_UINT32(msg, plant->cur_grow_rate, i);
	PKG_UINT32(msg, plant->earth, i);
	PKG_UINT32(msg, plant->pollinate_num, i);

	// may be not right,  thiefable is to one person, but the message be sent to every body in this map
	uint32_t thiefable = (plant->mature_time && get_now_tv()->tv_sec > plant->mature_time + 7*24*60*60) ? 1 : 0;
	if (plant->itemid == 1230032 && plant->mature_time)
		thiefable = 1;
	/*如果是孢子花，不能偷取*/
	if (plant->itemid == 1230055 || plant->itemid == 1230056) {
		thiefable = 0;
	}
	PKG_UINT32(msg, thiefable, i);
	/*
	DEBUG_LOG("uid=%u, %u %u %u %u %u %u %u %u %u", p->id, plant->number, plant->value, \
		plant->sickflag, plant->fruit_num, \
		plant->update_time, plant->water_time, \
		plant->kill_bug_time, plant->fruit_status, plant->mature_time);
	*/
	init_proto_head (msg, PROTO_UPDATE_PLANT_INFO, i);
	send_to_map(p, msg, i, 0);
}

int homeland_thief_fruit_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4 + sizeof(plant_attr_t));

	uint32_t itemid = *(uint32_t*)buf;
	DEBUG_LOG("THIEF FRUIT CALLBACK\t[%u %u %u]", p->id, id, itemid);
	db_single_item_op(0, p->id, itemid, 1, 1);
	response_proto_uint32(p, p->waitcmd, itemid, 0);

	plant_attr_t* plant = (plant_attr_t*)(buf + 4);
	rsp_plant_attr_to_map(p, plant);
	return 0;
}
//PROTO_GIFT_DRAW
int gift_draw_cmd(sprite_t* p, uint8_t* body, int len)
{
	return db_single_item_op(p, p->id, 190460, 1, 0);
}

int do_draw(sprite_t *p)
{
	uint32_t items[] = {13053, 160516, 160517, 160518, 888888};
	int indx = rand()%5;
	if (indx != 0) {
		if (indx != 4)
			db_buy_itm_freely(0, p->id, items[indx], 1);
		response_proto_uint32(p, p->waitcmd, items[indx], 0);
		return 0;
	}
	return db_chk_itm_cnt(p, items[0], items[0] + 1);
}

int add_cultivate_exp(sprite_t* p, int cultivt_exp)
{
	uint32_t old_cultivate_level = calculation_level_from_skill(p->cultivate_skill);
	if (p->cult_day_limit + cultivt_exp > 20)
		cultivt_exp = 20 - p->cult_day_limit;

	p->cult_day_limit += cultivt_exp;
	p->cultivate_skill += cultivt_exp;
	notify_skill_change(p, cultivt_exp, 0);
	DEBUG_LOG ("ADD CULTIVATE EXP\t[%u %u %u]", p->id, p->cult_day_limit, p->cultivate_skill);
	uint32_t new_cultivate_level = calculation_level_from_skill(p->cultivate_skill);
	level_change_and_send_postcard(new_cultivate_level, old_cultivate_level, 1000109, p->id);
	return db_add_culti_breed_exp(0, cultivt_exp, 1, p->id);
}

int homeland_pollinate_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 4);

	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in JY\t[uid=%u]", p->id), -1);
	if (!p->animal_nbr)
		return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);

	uint32_t plant_nbr;
	int j = 0;
	UNPKG_UINT32(body, plant_nbr, j);

	*(uint32_t*)p->session = plant_nbr;
	DEBUG_LOG("POLLINATE PLANT\t[%u animal=%u flower=%u]", p->id, p->animal_nbr, plant_nbr);
	return db_dec_pollinate_cnt(p, &p->animal_nbr, p->id);
}

int homeland_dec_pollinate_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in JY\t[uid=%u]", p->id), -1);

	p->animal_follow_info.tag_animal.remaind_pollinate_cnt--;
	return db_pollinate_flower(p, p->session, GET_UID_IN_JY(p->tiles->id));
}

int homeland_pollinate_flower_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(plant_attr_t));

	plant_attr_t* plant = (plant_attr_t*)buf;
	DEBUG_LOG ("POLLINATE PLANT CALLBACK\t[%u %u %u]", p->id, id, plant->number);

	return rsp_plant_attr(p, plant, id);
}

int homeland_fertilize_plant_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 4);

	if (!p->tiles || !IS_JY_MAP(p->tiles->id))
		ERROR_RETURN(("not in homeland\t[uid=%u]", p->id), -1);

	uint32_t* tmp_buf = (uint32_t*)p->session;
	int j = 0;
	tmp_buf[0] = p->id;
	UNPKG_UINT32(body, tmp_buf[1], j);  // which number
	tmp_buf[2] = 2;

	return db_single_item_op(p, p->id, 190574, 1, 0);
}

int homeland_fertilize_plant_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, sizeof(plant_attr_t));

	DEBUG_LOG("FERTILIZE PLANT CALLBACK\t[%u %u]", p->id, id);

	response_proto_head(p, p->waitcmd, 0);
	add_cultivate_exp(p, 2);
	plant_attr_t* plant = (plant_attr_t*)buf;
	rsp_plant_attr_to_map(p, plant);
	return 0;
}

int get_jy_anmimal_items_cmd(sprite_t* p, uint8_t* body, int len)
{
    CHECK_BODY_LEN(len, 4);
    int j = 0;
	uint32_t type = 0;
	UNPKG_UINT32(body, type, j);
	if (type < 1 || type > 2)
	{
	    ERROR_RETURN(("type error \t[uid=%u, type=%d]", p->id, type), -1);
	}

	return send_request_to_db(SVR_PROTO_GET_JY_ANIMAL_ITEMS, p, 4, &type, p->id);
}

int get_jy_anmimal_items_callback(sprite_t* p, uint32_t id, char* buf, int len)
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
	}

	init_proto_head (msg, p->waitcmd, j);
	return send_to_self (p, msg, j, 1);

}


