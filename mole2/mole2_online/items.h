
#ifndef MOLE2_ITEMS_H
#define MOLE2_ITEMS_H

#include "benchapi.h"
#include "dbproxy.h"
#include "cli_proto.h"
#include "svr_proto.h"
#include "sprite.h"
#include "mail.h"
#include "proto/mole2_db.h"

#define BASE_BODY_TYPE		10000

#define START_GRID_CLOTH_WAREHOUSE		20000
#define GRID_FOR_CLOTH_WAREHOUSE(grid_) ((grid_) >= START_GRID_CLOTH_WAREHOUSE)

enum item_type {
	item_special			= 1,
	item_normal				= 2,
	item_cloth				= 3,
};

enum {
	normal_item_medical		= 1,
	normal_item_collection	= 2,
	normal_item_task		= 3,
	normal_item_card		= 4,
	normal_item_vip			= 5,
};

enum {
	item_id_xiaomee			= 50001,
	item_id_exp				= 50002,
	item_id_pet_expbox		= 50003,
	item_id_skill_exp		= 50004,
	item_id_skill_expbox	= 50005,
	item_id_energy			= 50006,
	item_id_pkpt			= 50007,
	item_id_help_clean		= 50008,
	item_id_help_boss		= 50009,
	item_id_double_exp		= 50010,
	item_id_exp_box			= 50011,

	item_id_hang_exp		= 50012,
	item_id_hang_pet		= 50013,
	item_id_hang_skill		= 50014,
	//变身卡
	item_id_shape_shifting   = 50015,//变身卡id
	item_id_shapeshifting_time   = 50016,//剩余时间
	//lucky star
	item_id_lucky_star		= 50017,

	item_id_vacancy			= 51000,
	item_id_novip_draw		= 51001,
	item_id_vip_draw_day	= 51002,
	item_id_vip_draw_give	= 51003,
	item_id_draw_max_cnt    = 51004,
	/* special item id for activity, can be delete from db sometime */
	item_id_max,
};

enum {
	ssid_fishing_cnt		= 60001,
	ssid_vip_dragon_active	= 60002,
	ssid_pet_for_item		= 60003,
	ssid_vip_dragon_2		= 60004,

	ssid_pet_skill_reward_1	= 60005,
	ssid_pet_skill_reward_2	= 60006,
	ssid_pet_skill_reward_3	= 60007,
	ssid_grass_open_cnt		= 60008,
	ssid_global_beast_kill	= 60009,
	ssid_enter_copy_map_cnt	= 60010,
	ssid_add_energy			= 60011,
	ssid_precious_box		= 60012,
	ssid_hangup_activity_time = 60013,
	ssid_hangup_reward		= 60014,
	ssid_hunt_rarebeast_times = 60015,
	ssid_trainning_last_time  = 60017,
	ssid_pumpkin_lantern_cnt = 60018,
	ssid_pvp_pk_fail_times 		= 	60019,//每天单人单精灵赛pk 的次数限制
	ssid_pvp_pk_fail_times_2 	= 	60020,//每天单人多精灵赛pk 的次数限制
	ssid_shop_open_time 	= 60021,
	ssid_try_vip_activity 	= 60022,
	ssid_luckystar_hangup_time = 60023,
	ssid_pet_exchange_rare_egg = 60024,//玩家兑换4级精灵蛋的数量 
//for mail	
	ssid_mail_noti_seer   =61001,

	//每周清理一次的
	ssid_week_pvp_scores 	= 65000,
	// 两周清理一次
	ssid_trainning_least_time  = 66000,
	//每月清理一次
	ssid_reset_user_cnt   = 67000, 
	ssid_reset_pet_cnt   = 67001, 

	ssid_total_base 		= 80000,
	survey_id_age_sex		= 80001,
	survey_id_201108		= 80002,
	survey_id_pet_skill		= 80003,

	ssid_moqi_ride			= 89003,
	ssid_exchange_base		= 100000,
};

enum item_function {
	item_no_use					= 0,
	item_for_hpmp				= 1,
	item_for_reset_attr			= 2,
	item_for_handbook			= 3,
	item_for_exchange_normal	= 4,
	item_for_pet_exchange		= 5,
	item_for_pet_catch			= 6,
	item_for_battle				= 7,
	item_for_exchange_spec		= 8,
	item_for_set_color			= 9,
	item_for_broadcast			= 10,
	item_for_pet_egg			= 11,
	item_for_energy				= 13,
	item_for_exp_buff			= 14,
	item_for_pet_buff			= 15,
	item_for_skill_buff			= 16,
	item_for_exchange_rand		= 17,
	item_for_auto_hpmp			= 18,
	item_for_auto_fight			= 19,
	item_for_pet_param			= 21,
	item_for_pet_init			= 22,
	item_for_race_skill			= 23,
	item_for_recover_duration	= 25,
	item_for_precious_stone		= 26,
	item_for_recover_mduration	= 27,
	item_for_comp_clothes		= 28,
	item_for_box_key			= 29,
	item_for_exchange_rand_prof	= 30,
	item_for_shapeshifting	    = 31,
	item_for_pet_exchange_egg	= 32,
};

enum {
	item_target_user			= 0,
	item_target_self			= 1,
	item_target_pet_self		= 2,
	item_target_self_all		= 3,
	item_target_team_user		= 4,
	item_target_team_pet		= 5,
	item_target_team			= 6,
	item_target_team_all		= 7,
	item_target_pet_all			= 8,
};

enum cloth_type {
	weapon_jian				= 1,
	weapon_gong				= 2,
	weapon_zhang			= 3,
	weapon_biao				= 4,
	weapon_qiang			= 5,
};

enum rand_type {
	only_item				= 1,
	only_cloth				= 2,
	mix_item_cloth			= 3,
};

#define IS_ITEM_SPECIAL(itemid) \
	((itemid) >= item_id_xiaomee && (itemid) < item_id_max)

#define IS_ITEM_HANDBOOK_ITEM(itemid) \
		((itemid) / 10000 == 19) || ((itemid) / 10000 == 33)

#define IS_ITEM_HANDBOOK_BEAST(itemid) ((itemid) / 10000 == 25)
#define IS_ITEM_KEY(itemid) ((itemid) / 10000 == 28)

#define ITEM_ID_PRENSET			300004
#define ITEM_ID_DRAGON_EGG		350012
#define ITEM_ID_DRAGON_EGG_2	350015
#define ITEM_ID_DRAGON_EGG_3	350030	
#define ITEM_ID_DRAGON_EGG_5	89004	
#define ITEM_ID_DRAGON_EGG_6	350038	
#define ITEM_ID_ROCK_CHIP		290011
#define ITEM_CHIP				290013
#define ITEM_ID_TICKET			290014
#define ITEM_ID_XXXX			300002
#define ITEM_ID_SIGNAL			320004

#define CHECK_ITEM_VALID(p, pni, itemid) \
		do { \
			if (!pni) { \
				KERROR_LOG(p->id, "item id invalid\t[%u]", itemid); \
				return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1); \
			} \
		} while (0)
		
#define CHECK_ITEM_FUNC(p, pni, function_) \
		do { \
			if (pni->function != function_) { \
				KERROR_LOG(p->id, "item func invalid\t[%u %u]", pni->function, function_); \
				return send_to_self_error(p, p->waitcmd, cli_err_item_id_invalid, 1); \
			} \
		} while (0)


#define CHECK_ITEM_EXIST(p,itemid, count) \
		do { \
			if (!cache_item_have_cnt(p, (itemid), (count))) { \
				KERROR_LOG(p->id, "item no exist\t[%u %u]", (itemid), (count)); \
				return send_to_self_error(p, p->waitcmd, cli_err_havnot_this_item, 1); \
			} \
		} while (0)

#define CHECK_ITEM_EXIST_NEW(p,lp,itemid, count) \
		do { \
			if (!cache_item_have_cnt(lp, (itemid), (count))) { \
				KERROR_LOG(p->id, "item no exist\t[%u %u]", (itemid), (count)); \
				return send_to_self_error(p, p->waitcmd, cli_err_havnot_this_item, 1); \
			} \
		} while (0)

#define CHECK_CLOTH_EXIST(p,clothid) \
		do { \
			if (!cache_get_bag_cloth(p,clothid )) {\
				KERROR_LOG(p->id, "cloth no exist\t[%u]", (clothid)); \
				return send_to_self_error(p, p->waitcmd, cli_err_havnot_this_item, 1);\
			}\
		} while (0)


#define CHECK_CLOTH_EXIST_NEW(p,lp, clothid) \
		do { \
			if (!cache_get_bag_cloth(lp,clothid )) {\
				KERROR_LOG(p->id, "cloth no exist\t[%u]", (clothid)); \
				return send_to_self_error(p, p->waitcmd, cli_err_havnot_this_item, 1);\
			}\
		} while (0)


#define CHECK_CLOTH_LV_VALID(p, pcloth, clothid, lv) \
		do { \
			if (!pcloth || lv >= MAX_CLOTH_LEVL_CNT || !pcloth->clothlvs[lv].valid) { \
				KERROR_LOG(p->id, "cloth lv invalid\t[%u %u]", clothid, lv); \
				return send_to_self_error(p, p->waitcmd, cli_err_cloth_id_invalid, 1); \
			} \
		} while (0)
#define CHECK_CLOTH_PROD_VALID(p, pcloth, clothid, lv) \
		do { \
			if (!pcloth || lv >= PRODUCT_LV_CNT || !pcloth->pro_attr[lv].valid) { \
				KERROR_LOG(p->id, "cloth pro invalid\t[%u %u]", clothid, lv); \
				return send_to_self_error(p, p->waitcmd, cli_err_cloth_id_invalid, 1); \
			} \
		} while (0)

#define CHECK_CAN_HAVE_MORE_ITEM(p_, pni_, cnt_)	\
		do { \
			uint32_t item_cnt = get_item_cnt(p_, pni_); \
			if ((cnt_) < 0 || (cnt_) + item_cnt > pni_->total_max) { \
				return send_to_self_error (p_, p_->waitcmd, cli_err_item_cnt_max, 1); \
			} \
			if (!can_have_item(p_, pni_, get_need_grid(pni_, item_cnt, item_cnt + (cnt_)))) { \
				return send_to_self_error(p_, p_->waitcmd, cli_err_bag_full, 1); \
			} \
		} while (0)

#define IS_GRASS_GAME_TYPE(rid_) ((rid_)>=9050&&(rid_)<=9050)
#define IS_ROULETTE_GAME_TYPE(rid_) ((rid_)>=9001&&(rid_)<=9024)
#define IS_PUMPKIN_GAME_TYPE(rid_) ((rid_)>=720&&(rid_)<=720)
#define IS_PUMPKIN_QUESTION_TYPE(rid_) ((rid_)>=719&&(rid_)<=719)

#define itm_buyable(tradability) (tradability & 1)
#define itm_vipbuyable(tradability) (tradability & 1)
#define itm_salable(tradability) (tradability & 2)
#define itm_vipsalable(tradability) (tradability & 2)

#define db_del_cloth(p, uid, clothid) \
		send_request_to_db(p, uid, proto_db_del_cloth, &(clothid), 4)
#define db_get_packet_cloth_list(p) \
		send_request_to_db( p, p->id, proto_db_get_pakt_clothes, NULL, 0)

#define MIN_CLOTH_ID		50001
#define MAX_CLOTH_ID		149999
#define	IS_ITEM(rid_) ((rid_)>=150000 && (rid_)<=500000)
#define	IS_CLOTHES(rid_) ((rid_)<=MAX_CLOTH_ID&&(rid_)>=MIN_CLOTH_ID)


void init_all_items();
void fini_all_items();
void init_all_clothes();
void fini_all_clothes();
void init_rand_infos();
void fini_rand_infos();

int load_clothes(xmlNodePtr cur);
int load_items(xmlNodePtr cur);
int load_rand_item(xmlNodePtr cur);
int load_gather_info(xmlNodePtr cur);
int load_exchange_info(xmlNodePtr cur);
int load_suit(xmlNodePtr cur);
int load_handbook(xmlNodePtr cur);

cloth_t* get_cloth(uint32_t clothtype);
int buy_cloth_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int buy_cloth_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

int sell_cloths_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_pakt_clothes_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_pakt_clothes_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_body_clothes_cmd(sprite_t *p, uint8_t *body, uint32_t len);

int set_clothes_where_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int del_cloth_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int del_cloth_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int repair_cloth_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int repair_cloths_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int buy_normal_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int buy_normal_item_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int use_medical_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int add_normal_item_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int use_handbook_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int use_handbook_item_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int use_energy_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int use_buff_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_normal_item_list_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int sell_normal_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int add_item_list_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_house_item_list_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_house_item_list_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_house_cloth_list_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int item_in_out_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int item_in_out_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int cloth_in_out_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int cloth_in_out_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int rand_info_update_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int check_rand_info_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int rand_info_check_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int del_normal_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int del_house_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int check_team_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int composite_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int composite_cloth_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_item_day_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int exchange_gift_normal_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int exchange_gift_spec_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int exchange_gift_pet_cmd(sprite_t *p, uint8_t *body, uint32_t len);

int add_item_once_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int db_get_item_feed_count(sprite_t* p, userid_t uid,uint32_t itemid,uint32_t type);
int db_add_item_feed_count(sprite_t* p, userid_t uid,uint32_t itemid,uint32_t type,int count);
int add_item_feed_count_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_sth_done_for_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_sth_done_for_item_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int add_item_feed_count_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_item_feed_count_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);


int cache_update_body_cloths(sprite_t* p, body_cloth_t* apc, uint32_t cloth_cnt);
void cache_set_cloth_attr(sprite_t* p);
void cache_set_body_attr_temp(sprite_t* p, body_cloth_t* pac, uint32_t count);
int add_user_bag_cloths(sprite_t* p, body_cloth_t* apbc, uint32_t cloth_cnt);

int gen_cloth_attr(cloth_t* pc, uint16_t lv, cloth_lvinfo_t* pclv, cloth_attr_t* p_attr, uint32_t validday);
int gen_cloth_attr_new(cloth_attr_t* p_attr,body_cloth_t *pbc);
int db_add_item_list(sprite_t* p, uint32_t id, item_t* items, uint32_t item_cnt);
int db_reduce_item_list(sprite_t* p, uint32_t id, item_t* items, uint32_t item_cnt);
rand_info_t* get_rand_info(uint32_t rand_id);
normal_item_t* get_item(uint32_t itemid);
exchange_info_t* get_exchange_info(uint32_t id);

int update_cloth_duration(void* owner, void* data);
int item_enough_del(sprite_t* p, uint32_t itemid, uint32_t count);

void cache_update_kind_item(sprite_t* p, uint32_t itemid, uint32_t count);
int cache_reduce_kind_item(sprite_t* p, uint32_t itemid, uint32_t count);
int cache_add_kind_item(sprite_t* p, uint32_t itemid, uint32_t count);
int cache_init_bag_items(sprite_t* p, item_t* api, uint32_t item_cnt);
int cache_item_have_cnt(sprite_t* p, uint32_t itemid, uint32_t itemcnt);

int shapeshifting_start_effect(sprite_t* p);
int shapeshifting_lose_effect(sprite_t* p);

int item_enough_check(sprite_t* p, uint32_t itemid, uint32_t count);
static inline item_t *cache_get_bag_item(sprite_t* p, uint32_t itemid)
{
	return(item_t *)g_hash_table_lookup(p->bag_items, &itemid);
}

static inline int pkg_card_race(uint8_t* buf, normal_item_t* p_ni)
{
	int i = 0;
	PKG_H_UINT32(buf, p_ni->item_id, i);
	PKG_H_UINT32(buf, p_ni->petcatch.cardlv, i);
	PKG_H_UINT32(buf, p_ni->petcatch.racecnt, i);
	for (int loop = 0; loop < p_ni->petcatch.racecnt; loop ++) {
		PKG_H_UINT32(buf, p_ni->petcatch.races[loop], i);
	}
	return i;
}

static inline void noti_all_get_item(sprite_t* p, sysinfo_t* psys)
{
	uint8_t out[256];
	int i = sizeof(protocol_t);

	PKG_UINT32(out, 1, i);
	PKG_UINT32(out, p->id, i);
	PKG_STR(out, p->nick, i, USER_NICK_LEN);

	PKG_UINT32(out, sys_info_get_item, i);
	PKG_UINT32(out, psys->val1, i);
	PKG_UINT32(out, 0, i);
	PKG_UINT32(out, 0, i);

	KDEBUG_LOG(p->id, "SEND ALL GET ITEM");

	init_proto_head(out, proto_cli_sys_info, i);
	send_to_all(out, i);
}


static inline int db_add_item(sprite_t* p, uint32_t id, uint32_t itemid, int32_t item_cnt) 
{
	int8_t out[32];
	int k = 0;

	if (item_cnt > 0) {
		msg_log_vip_item_get(id, itemid);
	} else if (item_cnt < 0){
		msg_log_vip_item_use(id, itemid, -item_cnt);
	}

	if (itemid == 290016 && item_cnt > 0)
		msg_log_mooncake(item_cnt);

	msg_log_tuzhi(itemid,item_cnt);

	//normal_item_t* pni = get_item(itemid);
	uint32_t type = 0;
	PKG_H_UINT32(out, type, k);
	PKG_H_UINT32(out, itemid, k);
	PKG_H_UINT32(out, item_cnt, k);
	KDEBUG_LOG(id, "DB ADD ITEM\t[id=%u %u cnt=%d]",  itemid, type, item_cnt);
	return send_request_to_db(p, id, proto_db_add_normal_item, out, k);
}

static inline int db_add_item_to_house(sprite_t* p, uint32_t id, uint32_t itemid, int32_t item_cnt) 
{
	int8_t out[32];
	int k = 0;

	PKG_H_UINT32(out, itemid, k);
	PKG_H_UINT32(out, item_cnt, k);
	KDEBUG_LOG(id, "DB ADD ITEM TO HOUSE\t[id=%u cnt=%d]",  itemid, item_cnt);
	return send_request_to_db(p, id, proto_db_add_house_item, out, k);
}

static inline int
db_day_set_ssid_cnt(sprite_t* p, uint32_t uid, uint32_t ssid, uint32_t count, uint32_t totalcnt)
{
	mole2_day_set_ssid_count_in out;
	out.ssid=ssid;
	out.count=count;
	out.totalcnt=totalcnt;
	KDEBUG_LOG(uid, "DB SET SSID CNT\t[%u %u %u]", ssid, count, totalcnt);
	return send_msg_to_db(p,uid,mole2_day_set_ssid_count_cmd,&out);
}

static inline int
db_day_add_ssid_cnt(sprite_t* p, uint32_t uid, uint32_t ssid, uint32_t count, uint32_t limit)
{
	uint8_t out[64];
	int k = 0;

	PKG_H_UINT32(out, ssid, k);
	PKG_H_UINT32(out, count, k);
	PKG_H_UINT32(out, limit, k);
	KDEBUG_LOG(uid, "DB ADD SSID CNT\t[%u %u %u]", ssid, count, limit);
	return send_request_to_db(p, uid, proto_db_day_add_ssid_cnt, out, k);
}

static inline int
db_sub_ssid_totalcnt(sprite_t* p, uint32_t uid, uint32_t ssid, uint32_t count)
{
	uint8_t out[32];
	int k = 0;
	PKG_H_UINT32(out, ssid, k);
	PKG_H_UINT32(out, count, k);
	KDEBUG_LOG(uid, "DB SUB SSID TOTAL CNT\t[%u %u]", ssid, count);
	return send_request_to_db(p, uid, proto_db_sub_ssid_cnt, out, k);
}

static inline int
db_sub_ssid_count(sprite_t* p, uint32_t uid, uint32_t ssid, uint32_t count)
{
	uint8_t out[32];
	int k = 0;
	PKG_H_UINT32(out, ssid, k);
	PKG_H_UINT32(out, count, k);
	KDEBUG_LOG(uid, "DB SUB SSID COUNT\t[%u %u]", ssid, count);
	return send_request_to_db(p, uid, day_sub_ssid_count_cmd, out, k);
}


static inline int db_set_item_handbook(sprite_t* p, uint32_t id, uint32_t itemid, uint32_t item_cnt)
{
	int8_t out[8];
	int k = 0;
	
	PKG_H_UINT32(out, itemid, k);
	PKG_H_UINT32(out, item_cnt, k);
	return send_request_to_db(p, id, proto_db_set_item_handbook, out, k);
}

static inline int db_set_monster_handbook(sprite_t* p, uint32_t id, uint32_t beastid, uint32_t state, uint32_t itemid, int itemcnt)
{
	int bytes = 0;
	uint8_t buf[64];
	PKG_H_UINT32(buf, beastid,bytes);
	PKG_H_UINT32(buf, state, bytes);
	PKG_H_UINT32(buf, itemid, bytes);
	PKG_H_UINT32(buf, itemcnt, bytes);
	return send_request_to_db(p, id, proto_db_use_beast_hb_item, buf, bytes);
}

static inline int db_check_rand_info(sprite_t* p, uint32_t id, uint32_t randid, uint32_t repeattype, uint32_t max_cnt)
{
	uint8_t out[32];
	int i = 0;
	PKG_H_UINT32 (out, randid, i);
	PKG_H_UINT32 (out, repeattype, i);
	PKG_H_UINT32 (out, max_cnt, i);

	return send_request_to_db(p, id, proto_db_rand_info_check, out, i);
}

static inline int db_set_energy(sprite_t* p, uint32_t id, uint32_t energy)
{
	return send_request_to_db(p, id, proto_db_set_energy, &energy, 4);
}

static inline int db_log_vip_item(sprite_t* p, uint32_t id, uint32_t logtype, uint32_t itemid, uint32_t count)
{
	uint32_t buf[3] = {logtype, itemid, count};
	return send_request_to_db(p, id, log_vip_item_cmd, buf, sizeof(buf));
}

static inline int pkg_cloth(body_cloth_t* ac, uint8_t* pbuf)
{
	int j = 0;
	PKG_UINT32(pbuf, ac->clothid, j);
	PKG_UINT32(pbuf, ac->clothtype, j);
	PKG_UINT32(pbuf, ac->grid, j);
	PKG_UINT16(pbuf, ac->clothlv, j);

	PKG_UINT32(pbuf, ac->quality, j);
	PKG_UINT32(pbuf, ac->validday, j);

	PKG_UINT16(pbuf, ac->duration_max, j);
	PKG_UINT16(pbuf, ac->duration, j);
	PKG_UINT32(pbuf, ac->hp_max, j);
	PKG_UINT32(pbuf, ac->mp_max, j);
	PKG_UINT16(pbuf, ac->attack, j);
	PKG_UINT16(pbuf, ac->mattack, j);
	PKG_UINT16(pbuf, ac->defense, j);
	PKG_UINT16(pbuf, ac->mdefense, j);
	PKG_UINT16(pbuf, ac->speed, j);
	PKG_UINT16(pbuf, ac->spirit, j);
	PKG_UINT16(pbuf, ac->resume, j);
	PKG_UINT16(pbuf, ac->hit, j);
	PKG_UINT16(pbuf, ac->dodge, j);
	PKG_UINT16(pbuf, ac->crit, j);
	PKG_UINT16(pbuf, ac->fightback, j);
	PKG_UINT16(pbuf, ac->rpoison, j);
	PKG_UINT16(pbuf, ac->rlithification, j);
	PKG_UINT16(pbuf, ac->rlethargy, j);
	PKG_UINT16(pbuf, ac->rinebriation, j);
	PKG_UINT16(pbuf, ac->rconfusion, j);
	PKG_UINT16(pbuf, ac->roblivion, j);
	PKG_UINT32(pbuf, ac->crystal_attr, j);
	cloth_t* psc = get_cloth(ac->clothtype);
	if (psc && ac->crystal_attr && ac->bless_type && (ac->bless_type & 0xFFFF) < attr_id_max) {
		PKG_UINT32(pbuf, ac->bless_type & 0xFFFF, j);
		PKG_UINT32(pbuf, !!(ac->bless_type &0x10000), j);
		PKG_UINT32(pbuf, crystal_attr[(ac->bless_type & 0xFFFF) - 1][ac->quality][psc->minlv / 5], j);
	} else {
		PKG_UINT32(pbuf, 0, j);
		PKG_UINT32(pbuf, 0, j);
		PKG_UINT32(pbuf, 0, j);
	}

	PKG_UINT32(pbuf, 0, j);		//hole cnt
	
	return j;
}

static inline void log_cloth(uint32_t uid, const body_cloth_t* pc)
{
	KDEBUG_LOG(uid, "CLOTH:[%u %u %u %u %u]\t[%u %u %u %u %u %u]\t[%u %u %u %u %u %u %u]\t", \
		pc->clothid, pc->clothtype, pc->clothlv, pc->duration_max, pc->duration, \
		pc->hp_max, pc->mp_max, pc->attack, pc->mattack, pc->defense, pc->mdefense, \
		pc->speed, pc->spirit, pc->resume, pc->hit, pc->dodge, pc->crit, pc->fightback);
}

static inline int db_add_cloth(sprite_t* p, uint32_t uid, uint32_t cost, cloth_t* pc, uint16_t lv, cloth_lvinfo_t* pclv)
{
	uint8_t out[256] = {0};
	int i = 0;
	PKG_H_UINT32(out, -cost, i);
	cloth_attr_t* pca = (cloth_attr_t *)(out + i);
	gen_cloth_attr(pc, lv, pclv, pca, 0);
	i += sizeof(cloth_attr_t);
	KDEBUG_LOG(uid, "DB ADD CLOTH\t[type=%u lv=%u cost=%d]", pca->clothtype, pca->cloth_level, cost);
	return send_request_to_db(p, uid, proto_db_add_cloth, out, i);
}

static inline int db_add_cloth_new(sprite_t* p, uint32_t uid, uint32_t cost, body_cloth_t *pbc)
{
	uint8_t out[256] = {0};
	int i = 0;
	PKG_H_UINT32(out, -cost, i);
	cloth_attr_t* pca = (cloth_attr_t *)(out + i);
	gen_cloth_attr_new(pca, pbc);
	i += sizeof(cloth_attr_t);
	KDEBUG_LOG(uid, "DB ADD CLOTH NEW\t[type=%u lv=%u cost=%d]", pca->clothtype, pca->cloth_level, cost);
	return send_request_to_db(p, uid, proto_db_add_cloth, out, i);
}

static inline int db_repair_one_cloth(sprite_t* p, uint32_t uid, uint32_t price,uint32_t clothid, uint32_t mduration, uint32_t duration)
{
	uint8_t buf[128];
	int j = 0;
	PKG_H_UINT32(buf, -price, j);
	PKG_H_UINT32(buf, 1, j);
	PKG_H_UINT32(buf, clothid, j);
	PKG_H_UINT16(buf, mduration, j);
	PKG_H_UINT16(buf, duration, j);
	return send_request_to_db(p, uid, proto_db_repair_cloth_list, buf, j);
}

static inline void set_body_cloth(sprite_t* p, body_cloth_t* pc)
{
	//log_cloth(pc);
	if (pc->duration_max == 0) {
		pc->duration_max = pc->duration;
		pc->duration_max = 1;
		KERROR_LOG(p->id, "CLOTH DURATION INVALID\t[%u %u]", pc->clothid, pc->duration_max);
	}
	if (pc->duration > pc->duration_max) 
		pc->duration = pc->duration_max;
	int ratio = (pc->duration * 10 / pc->duration_max) * 2 + 1;
	ratio = ratio > 10 ? 10 : ratio;

	p->hp_max += pc->hp_max * ratio / 10;
	p->mp_max += pc->mp_max * ratio / 10;
	p->attack += pc->attack * ratio / 10;
	p->mattack += pc->mattack * ratio / 10;
	p->defense += pc->defense * ratio / 10;
	p->mdefense += pc->mdefense * ratio / 10;
	p->speed += pc->speed * ratio / 10;
	p->spirit += pc->spirit * ratio / 10;
	p->resume += pc->resume * ratio / 10;
	p->hit_rate += pc->hit * ratio / 10;
	p->avoid_rate += pc->dodge * ratio / 10;
	p->bisha += pc->crit * ratio / 10;
	p->fight_back += pc->fightback * ratio / 10;
	p->rpoison += pc->rpoison * ratio / 10;
	p->rlithification += pc->rlithification * ratio / 10;
	p->rlethargy += pc->rlethargy * ratio / 10;
	p->rinebriation += pc->rinebriation * ratio / 10;
	p->rconfusion += pc->rconfusion * ratio / 10;
	p->roblivion += pc->roblivion * ratio / 10;
	
	p->attack_for_cal += pc->attack * ratio / 10 /5;
	p->defense_for_cal += pc->defense * ratio / 10 /5;
	p->speed_for_cal += pc->speed * ratio / 10 /5;

	KDEBUG_LOG(p->id,"monitor_sprite_add_attr:%u %u %u %u %u %u %u %u %u"
			,pc->attack,pc->defense,pc->speed	
			,p->attack,p->defense,p->speed,p->attack_for_cal,p->defense_for_cal,p->speed_for_cal);
}

static inline void noti_cloth_disappear(sprite_t* p, uint32_t clothid)
{
	response_proto_uint32(p, proto_cli_cloth_disappear, clothid, 0, 0);
}

static inline void notify_user_clothes(sprite_t* p)
{
	cli_set_body_clothes_out cli_out;
	cli_out.id = p->id;
	cli_out.flag = p->flag;
	cli_out.title = p->honor;
	
	GList* pcloths = g_hash_table_get_values(p->body_cloths);
	GList* head = pcloths;
	while (pcloths) {
		body_cloth_t* pc = (body_cloth_t*)pcloths->data;
		stru_cloth_simple_t simple_cloth;
		simple_cloth.clothid = pc->clothid;
		simple_cloth.clothtype = pc->clothtype;
		simple_cloth.clothlv = pc->clothlv;
		cli_out.cloth_list.push_back(simple_cloth);
		pcloths = pcloths->next;
	}
	g_list_free(head);

	send_msg_to_map(p, cli_set_body_clothes_cmd, &cli_out, 0, 1);
}

static inline int try_disappear_cloth_before(sprite_t* p, body_cloth_t* pc)
{
	uint32_t nowtime = get_now_tv()->tv_sec;
	if (pc->validday && nowtime > pc->clothid && (nowtime - pc->clothid) >= pc->validday * 3600) {
		db_del_cloth(0, p->id, pc->clothid);
		send_dbmail(p, 29);
		return 1;
	}
	return 0;
}

 static inline gboolean try_disappear_cloth_after(gpointer key, gpointer value, gpointer user_data)
{
	sprite_t* p = (sprite_t *)user_data;
	body_cloth_t* pc = (body_cloth_t *)value;
	uint32_t nowtime = get_now_tv()->tv_sec;
	if (pc->validday && nowtime > pc->clothid && (nowtime - pc->clothid) >= pc->validday * 3600) {
		db_del_cloth(0, p->id, pc->clothid);
		g_hash_table_remove(p->cloth_uplist, &pc->clothid);
		g_hash_table_remove(p->all_cloths, &pc->clothid);
		noti_cloth_disappear(p, pc->clothid);
		if (pc->grid > BASE_BODY_TYPE) {
			g_hash_table_remove(p->body_cloths, &pc->grid);
			cache_set_cloth_attr(p);
			notify_user_clothes(p);
		}
		
		send_dbmail(p, 29);
		g_slice_free1(sizeof(body_cloth_t), pc);
		return TRUE;
	}
	return FALSE;
}


static inline uint32_t get_max_grid(sprite_t* p)
{
	return ISVIP(p->flag) ? 80 : 48;
}

static inline uint32_t get_bag_cloth_cnt(sprite_t* p)
{
	return g_hash_table_size(p->all_cloths) - g_hash_table_size(p->body_cloths);
}

static inline int get_need_grid(normal_item_t* pni, uint32_t old_cnt, uint32_t new_cnt)
{
	uint32_t overlaymax = pni->overlay_max ? pni->overlay_max : 1;
	int need_grid_old = old_cnt / overlaymax + (old_cnt % overlaymax ? 1 : 0);
	int need_grid_new = new_cnt / overlaymax + (new_cnt % overlaymax ? 1 : 0);
	return need_grid_new - need_grid_old;
}

static inline uint32_t get_item_cnt(sprite_t* p, normal_item_t* pni)
{
	item_t* pitem = cache_get_bag_item(p, pni->item_id);
	return pitem ? pitem->count : 0;
}

static inline int can_have_item(sprite_t* p, normal_item_t* pni, uint32_t need_grid)
{
	return ((pni->type == normal_item_medical && p->chemical_grid + need_grid <= get_max_grid(p)) \
		|| (pni->type == normal_item_collection && p->collection_grid + need_grid <= get_max_grid(p)) \
		|| (pni->type == normal_item_card && p->card_grid + need_grid <= get_max_grid(p)) \
		|| (pni->type == normal_item_vip));
}

static inline int can_have_more_item(sprite_t* p, uint32_t itemid, uint32_t count)
{
	normal_item_t* pni = get_item(itemid);
	if (!pni) return 0;
	if (pni->type == normal_item_vip)
		return 1;
	
	uint32_t item_cnt = get_item_cnt(p, pni);
	if (item_cnt + count >= pni->total_max) return 0;
	uint32_t need_grid = get_need_grid(pni, item_cnt, item_cnt + count);
	return can_have_item(p, pni, need_grid);
}

static inline int cache_add_bag_cloth(sprite_t* p, body_cloth_t* pc)
{
	/*
	if (try_disappear_cloth_before(p, pc))
		return 0;
	*/
	
	if (get_bag_cloth_cnt(p) >= get_max_grid(p)) {
		KERROR_LOG(p->id, "ADD CLOTH REACH MAX\t[%u]", pc->clothid);
		return -1;
	}
	body_cloth_t* pcloth = (body_cloth_t *)g_slice_alloc(sizeof(body_cloth_t));
	memcpy(pcloth, pc, sizeof(body_cloth_t));
	g_hash_table_insert(p->all_cloths, &pcloth->clothid, pcloth);
	if (pc->validday)
		g_hash_table_insert(p->cloth_timelist, &pcloth->clothid, pcloth);
	return 0;
}

static inline body_cloth_t* cache_get_cloth(sprite_t* p, uint32_t clothid)
{
	return (body_cloth_t *)g_hash_table_lookup(p->all_cloths, &clothid);
}

static inline void db_sync_cloth_duration(sprite_t* p)
{
	uint8_t out[1024];
	int i = 4, count = 0;
	GList* pcloths = g_hash_table_get_values(p->cloth_uplist);
	GList* head = pcloths;
	while (pcloths) {
		body_cloth_t* pbc = (body_cloth_t *)pcloths->data;
		PKG_H_UINT32(out, pbc->clothid, i);
		PKG_H_UINT16(out, pbc->duration, i);
		KDEBUG_LOG(p->id, "UPDATE CLOTH\t[%u %u]", pbc->clothid, pbc->duration);
		count ++;
		pcloths = pcloths->next;
	}
	g_list_free(head);

	KDEBUG_LOG(p->id, "SYNC DURATION\t[%u]", count);
	if (count) {
		int j = 0;
		PKG_H_UINT32(out, count, j);
		send_request_to_db(NULL, p->id, proto_db_set_cloth_duration, out, i);
		g_hash_table_remove_all(p->cloth_uplist);
	}
}

static inline void log_cloth_new(sprite_t *p, const body_cloth_t* pbc,const char* type)
{
	KF_LOG(type,p->id,"clothtype:%u grid:%u clothlv:%u duration_max:%u duration:%u hp_max:%u mp_max:%u attack:%u mattack:%u defense:%u mdefense:%u speed:%u spirit:%u resume:%u hit:%u dodge:%u crit:%u fightback:%u rpoison:%u rlithification:%u rlethargy:%u rinebriation:%u rconfusion:%u roblivion:%u quality:%u validday:%u crystal_attr:%u bless_type:%u",
				pbc->clothtype, pbc->grid, pbc->clothlv, pbc->duration_max, pbc->duration, pbc->hp_max, pbc->mp_max,
			   	pbc->attack, pbc->mattack, pbc->defense, pbc->mdefense, pbc->speed, pbc->spirit, pbc->resume, 
				pbc->hit, pbc->dodge, pbc->crit, pbc->fightback, pbc->rpoison, pbc->rlithification, pbc->rlethargy, 
				pbc->rinebriation, pbc->rconfusion, pbc->roblivion, pbc->quality, pbc->validday, pbc->crystal_attr, 
				pbc->bless_type);

}

static inline int cache_del_bag_cloth(sprite_t* p, uint32_t cloth_id)
{
	body_cloth_t* pbc = cache_get_cloth(p, cloth_id);
	if (pbc) {
		DEBUG_LOG("cache_del_bag_cloth %u",pbc->clothtype);
		db_sync_cloth_duration(p);
		g_hash_table_remove(p->body_cloths, &pbc->grid);
		g_hash_table_remove(p->cloth_timelist, &pbc->clothid);
		g_hash_table_remove(p->all_cloths, &cloth_id);
		g_slice_free1(sizeof(body_cloth_t), pbc);
		//log_cloth_new(p,pbc,"del_bag_cloth");
	}
	return 0;
}


static inline body_cloth_t* 
cache_get_bag_cloth(sprite_t* p, uint32_t cloth_id)
{
	body_cloth_t* pc = (body_cloth_t *)g_hash_table_lookup(p->all_cloths, &cloth_id);
	return pc && pc->grid < BASE_BODY_TYPE ? pc : NULL; 
}

static inline body_cloth_t* cache_get_body_cloth(sprite_t* p, uint32_t grid)
{
	return (body_cloth_t *)g_hash_table_lookup(p->body_cloths, &grid);
}

static inline item_t* cache_get_day_limits(sprite_t* p, uint32_t ssid)
{
	item_t* pday = (item_t *)g_hash_table_lookup(p->day_limits, &ssid);
	if (!pday) {
		pday = (item_t *)g_slice_alloc(sizeof(item_t));
		pday->itemid = ssid;
		pday->count = 0;
		g_hash_table_insert(p->day_limits, &(pday->itemid), pday);
	}
	return pday;
}

static inline uint32_t cache_item_limit_add(sprite_t* p, uint32_t itemid, uint32_t count)
{
	item_t* pday = cache_get_day_limits(p, itemid);
	normal_item_t* pni = get_item(itemid);
	if (!pni) return 0;
	if (pday->count + count > pni->day_max)
		count = pni->day_max > pday->count ? pni->day_max - pday->count : 0;
	return count;
}

static inline void item_limit_update(sprite_t* p, uint32_t itemid, uint32_t count)
{
	if (!count) return;
	item_t* pday = cache_get_day_limits(p, itemid);
	pday->count += count;
	normal_item_t* pni = get_item(itemid);
	db_day_add_ssid_cnt(NULL, p->id, itemid, count, pni ? pni->day_max : -1); 
}

static inline int get_roulette_outitem_idx(uint32_t itemid)
{
	if(itemid <= 80060)
		return itemid-80057;
	else if(itemid <= 210002 && itemid >= 210001)
		return itemid-210001+4;
	else if(itemid <= 220009 && itemid >= 220028)
		return itemid-220009+6;
	else if(itemid <= 230002 && itemid >= 230001)
		return itemid-230001+27;
	else if(itemid == 290001)
		return 29;
	else if(itemid == 300001)
		return 30;
	else if(itemid == 310005)
		return 31;
	else if(itemid == 310032)
		return 32;
	return -1;
}

#endif


