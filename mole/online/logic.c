#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "central_online.h"
#include "communicator.h"
#include "dll.h"
#include "message.h"

#include "homeinfo.h"
#include "mole_homeland.h"
#include "mole_pasture.h"
#include "spring.h"
#include "event.h"

#include "logic.h"
#include "mole_class.h"
#include "mole_car.h"
#include "cache_svr.h"
#include "game_pk.h"
#include "game_bonus.h"
#include "dbproxy.h"
#include "mole_angel.h"
#include "mole_dungeon.h"
#include "final_boss_2011.h"
#include "mole_cutepig.h"
#include "fire_cup.h"
#include "lanterns_day.h"


#undef  MIN
#define MIN(a, b)	((a) > (b) ? (b) : (a))

#define MAX_NORMAL_MAP_FOR_PET 20
uint32_t mapforpet[MAX_NORMAL_MAP_FOR_PET]={2,3,4,6,7,8,9,10,41,47};
int mapforpet_num = 10;

int do_buy_item(sprite_t* p, uint32_t itemid, int nitems, int free, int ret_needed)
{
	if (nitems > 100 || nitems <= 0)
		ERROR_RETURN(("error count=%d, uin=%u", nitems, p->id), -1);

	item_t* itm_prop = NULL;
	itm_prop = get_item_prop(itemid);
	if (!itm_prop)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}


	if (!itm_prop || !itm_buyable(p, itm_prop) || p->vip_level < itm_prop->vip_gift_level) {
		ERROR_LOG("Item %u not found or Unbuyable: uid=%u itmflag=0x%X %u",
						itemid, p->id, (itm_prop ? itm_prop->tradability : 0), p->vip_level);
		return send_to_self_error(p, p->waitcmd, -ERR_cannot_buy_itm, 1);
	}

	item_kind_t* kind = find_kind_of_item(itemid);
	if (!kind || kind->kind == PET_ITEM_KIND || kind->kind == POSTCARD_ITEM_KIND || (kind->kind >= ATTR_ITEM_KIND && (kind->kind <= PET_ITEM_HONOR_KIND))) {
		ERROR_LOG("error item kind, item=%u, uin=%u", itemid, p->id);
		return send_to_self_error(p, p->waitcmd, -ERR_cannot_buy_itm, 1);
	}

	if (kind->kind == HOMELAND_SEED_KIND || itemid == 1220117) {
		if (itm_prop->zhongzi_youzai_level > calculation_level_from_skill(p->cultivate_skill)) {
			return send_to_self_error(p, p->waitcmd, -ERR_zhongzhi_level_not_enough, 1);
		}
	}
	if (kind->kind == ANIMAL_BABY_ITEM) {
		if (itm_prop->zhongzi_youzai_level > calculation_level_from_skill(p->breed_skill)) {
			return send_to_self_error(p, p->waitcmd, -ERR_yangzhi_level_not_enough, 1);
		}
	}

	if(kind->kind == CLASS_ITEM_KIND) {
		*(int*)p->session = - itm_prop->price * nitems;
		*(uint32_t*)(p->session + 4)= itemid;
		*(uint32_t*)(p->session + 8)= nitems;
		return buy_itm_class(p, itemid, nitems, itm_prop->max);
	}

	if( ( kind->kind == CUTE_PIG_KIND ||
		kind->kind == CUTE_PIG_ITEM_KIND ||
		kind->kind == CUTE_PIG_CLOTHES_KIND ) &&
		itm_prop->u.cp_base_info_tag.buy_level > cutepig_get_cur_level(p->cp_exp)
		)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_cannot_buy_itm, 1);
	}

	if(((kind->kind == OCEAN_ANIMAL_ITEM_KIND || kind->kind == OCEAN_ITEM_ITEM_KIND)) && itemid != 1633000){
		return db_buy_items_by_ocean_shells(p, itm_prop, nitems, kind, free, ret_needed);
	}
	return db_buy_items(p, itm_prop, nitems, kind, free, ret_needed);
}

#define calc_attr(x, y, inc) { \
	(inc) = rate * (x) / 100; \
	(y) += (inc); \
	PKG_H_UINT32(p->session, (inc), i); \
}

int do_game_score(sprite_t* p, uint32_t rate, uint32_t score)
{
	if ( rate > 100 ) {
		ERROR_RETURN( ("error ratio=%u score=%u id=%u", rate, score, p->id), -1 );
	}

	p->sess_len = 73;
	game_t* g = p->group->game;
	int i = sizeof(protocol_t);
	PKG_H_UINT32(p->session, g->id, i);

	char outbuf[8], inbuf[8];
	//session: time + ip + time + score + userid + game_id
	// time
	memcpy(inbuf, &now.tv_sec, 4);
	// ip
	uint32_t ipaddr = get_ip2(p);
	memcpy(inbuf + 4, &ipaddr, 4);
	des_encrypt(DES_GAME_SCORE_KEY, inbuf, outbuf);
	PKG_STR(p->session, outbuf, i, sizeof outbuf);
	// score
	//int score = rate * g->score / 100;
	memcpy(inbuf + 4, &score, 4);
	des_encrypt(DES_GAME_SCORE_KEY, inbuf, outbuf);
	PKG_STR(p->session, outbuf, i, sizeof outbuf);
	// userid & gameid
	memcpy(inbuf, &p->id, 4);
	memcpy(inbuf + 4, &g->id, 4);
	des_encrypt(DES_GAME_SCORE_KEY, inbuf, outbuf);
	PKG_STR(p->session, outbuf, i, sizeof outbuf);
	//score
	PKG_UINT32(p->session, score, i);
	int exp, strong, iq, lovely, yxb;
	if (!credit_cond_met(p)) {
		if (rate != 0) {
			uint32_t msg_info[] = {1, p->id};
			msglog(statistic_logfile, 0x04020302, get_now_tv()->tv_sec, msg_info, sizeof(msg_info));
			rate = 0;
		}
	}

	int modulus = 100;
	const struct tm* cur_time = get_now_tm();

	if (p->followed && g->id == 59)
		modulus = 120;

	if (is_holiday(cur_time->tm_wday)) {
		modulus = modulus * 2;
	}

	if ((get_now_tv()->tv_sec - p->stamp + p->oltoday) >= FIVE_HOUR_SEC) {
		modulus = 0;
	} else {
		if ((get_now_tv()->tv_sec - p->stamp + p->oltoday) >= TWO_HOUR_SEC) {
			if (is_holiday(cur_time->tm_wday)) {
				modulus = modulus / 2;
			} else {
				modulus = 0;
			}
		}
	}

	DEBUG_LOG("MODULUS\t[%u %u %u %u %u %u]", p->id, (uint32_t)get_now_tv()->tv_sec, (uint32_t)p->stamp, p->oltoday, modulus, rate);

	calc_attr(g->exp * modulus / 100, p->exp, exp);
	calc_attr(g->strong * modulus / 100, p->strong, strong);
	calc_attr(g->IQ * modulus / 100, p->iq, iq);
	calc_attr(g->lovely * modulus / 100, p->lovely, lovely);

	if(g->aux_item && is_wear_item(p, g->aux_item)){
		calc_attr( ((g->aux_yxb * modulus) / 100), p->yxb, yxb);
	}
	else{
		calc_attr( ((g->yxb * modulus) / 100), p->yxb, yxb);
	}
	if (g->pk)
		db_update_game_grade(0, g->id, score, p->id);

	uint32_t itmid = 0;
	if (g->bonus_cnt && (score > g->bonus_cond[0]) && rate != 0) {
		int idx = rand() % g->bonus_cnt;
		if ( (g->bonus_cond[1] && (score > g->bonus_cond[1])) || ((rand() % g->bonus_rate[idx]) < g->bonus_rate_numerator[idx]) ) {
			itmid = g->itembonus[idx];
		}
	}

	if(g->id == 59 && !ISVIP(p->flag)) {
		itmid = 0;
	}
	if (g->id == 8 && itmid == 1270009) {
		if (!ISVIP(p->flag) || !g->aux_item || !is_wear_item(p, g->aux_item))
			itmid = 0;
	}

	DEBUG_LOG("game_bonus_id\t[%u %u %u %u %u]", p->id, g->game_bonus_id, itmid, modulus, score);
	uint32_t tmp_bonus_id = g->game_bonus_id;

	if((tmp_bonus_id || itmid) && modulus) {
		if(score > g->score) score = g->score;
		pack_client_itmid_accord_id_score(p, p->session, &i, tmp_bonus_id, score, itmid);
	} else {
		PKG_UINT32(p->session, 0, i);
	}

	init_proto_head(p->session, PROTO_GAME_SCORE, i);
	uint8_t reason = LEAVE_GAME_BY_OVER;
	leave_game_cmd(p, &reason, 1);


	if(g->id == 85) {
		uint32_t msg_info[1] = {1};
		uint32_t stage = rate / 11;
		if(stage > 0) {
			if(stage > 9) stage = 9;
			msglog(statistic_logfile,0x04030101 + stage - 1, get_now_tv()->tv_sec, msg_info, sizeof(msg_info));
		}
	}

	p->waitcmd = PROTO_GAME_SCORE;
	return exchg_item_callback(p, p->id, 0, 0);
}

#undef calc_attr

int do_find_item(sprite_t *p, uint32_t itemid)
{
	item_kind_t *kind = find_kind_of_item (itemid);
	item_t *itm = get_item_prop (itemid);

	if (!itm || !kind || kind->kind != SPECIAL_ITEM_KIND || !itm_buyable(p, itm))
		ERROR_RETURN (("not exists item=%d, id=%u", itemid, p->id), -1);

	return db_single_item_op(p, p->id, itemid, 1, 1);
}

int do_leave_map(sprite_t* p)
{
	if (!p->tiles) {
		ERROR_RETURN(("no tile found, id=%u", p->id), -1);
	}
	if (p->group) {
		uint8_t reason = LEAVE_GAME_BY_REQUEST;
		leave_game_cmd(p, &reason, 1);
	}

	if ( !IS_NPC_ID(p->id) ) {
		DEBUG_LOG("LEAVE MAP\t[%u %lu %d]", p->id, p->tiles->id, p->tiles->sprite_num);
	}

	leave_map(p, 1);
	return 0;
}

void pet_go_outside(sprite_t* p)
{
	int i;
	//map_t* map = p->tiles;
	map_t* map = get_map(p->id);
	for (i = 0; i != map->pet_cnt; ++i) {
		pet_t* pet = &(map->pets[i]);
		if ( (pet != p->followed) && !PET_IS_SUPER_LAHM(pet)
				&& PET_NATURAL(pet) && (get_pet_level(pet) > 1) ) {
			char txt[192];
			uint32_t map_id = mapforpet[rand()%mapforpet_num];
			if( -1 == add_pet_to_map(pet, map_id))
				return;

			int out_reason = 0;
			int loop;
			for (loop = 0; loop < pet->suppl_info.pti.count && loop < PET_TASK_MAX_CNT; loop++) {
				if (pet->suppl_info.pti.taskinfo[loop].taskid == 102) {
					out_reason = 102;
					break;
				}
				if (pet->suppl_info.pti.taskinfo[loop].taskid == 104) {
					out_reason = 104;
					break;
				}
			}

			if (out_reason == 102) {
			#ifdef TW_VER
			snprintf(txt, sizeof txt, "   親愛的%.16s，我要出門展現一下我的力量！不用著急找我啦，馬上就會回來的！", p->nick);
			#else
			snprintf(txt, sizeof txt, "   亲爱的%.16s，我要出门展现一下我的力量！不用着急找我啦，马上就会回来的！", p->nick);
			#endif
				send_postcard(pet->nick, 0, p->id, 1000086, txt, map_id);
			} else if (out_reason == 104) {
			#ifdef TW_VER
			snprintf(txt, sizeof txt, "   親愛的%.16s，我決定去探險，說不定會有驚喜發現哦！不用著急找我啦，馬上就會回來的！", p->nick);
			#else
			snprintf(txt, sizeof txt, "   亲爱的%.16s，我决定去探险，说不定会有惊喜发现哦！不用着急找我啦，马上就会回来的！", p->nick);
			#endif
				send_postcard(pet->nick, 0, p->id, 1000084, txt, map_id);
			} else if (pet->suppl_info.pti.count != 0) {
			#ifdef TW_VER
			snprintf(txt, sizeof txt, "   親愛的%.16s，我一個人待在家裏好無聊哦，想出去逛逛，如果你發現我不在家的話不用太著急，我很快就回來哦！", p->nick);
			#else
			snprintf(txt, sizeof txt, "   亲爱的%.16s，我一个人呆在家里好无聊哦，想出去逛逛，如果你发现我不在家的话不用太着急，我很快就回来哦！", p->nick);
			#endif
				send_postcard(pet->nick, 0, p->id, 1000075, txt, map_id);
			} else {
			#ifdef TW_VER
			snprintf(txt, sizeof txt, "   親愛的%.16s，看到別人都去拉姆學院上課啦！我也想去看看！不用著急哦，我馬上就回來。", p->nick);
			#else
			snprintf(txt, sizeof txt, "   亲爱的%.16s，看到别人都去拉姆学院上课啦！我也想去看看！不用着急哦，我马上就回来！", p->nick);
			#endif
				send_postcard(pet->nick, 0, p->id, 1000080, txt, map_id);
			}
			send_attr_update_noti(0, 0, p->id, 3);
			//
			SET_PET_GO_OUT(pet);
			pet->trust_end_tm = get_now_tv()->tv_sec + ((rand() % 10) + 1) * 60;
			//pet->trust_end_tm = get_now_tv()->tv_sec + 30 * 60;
			pet->suppl_info.mapid = map_id;
			//db_pet_op(0, pet, p->id);
			db_set_flag_endtime(0, pet->flag, pet->trust_end_tm, pet->id, p->id);
			respond_pet_to_map(pet, PET_OUT);
			DEBUG_LOG("PET GO OUT\t[uid=%u pid=%u endtm=%d mapid=%u]", p->id, pet->id, pet->trust_end_tm, map_id);
			break;
		}
	}
}

void pet_visit_other(sprite_t* p)
{
	int i;
	map_t* map = p->tiles;
	for (i = 0; i != map->pet_cnt; ++i) {
		pet_t* pet = &(map->pets[i]);
		if ( (pet != p->followed) && !PET_IS_SUPER_LAHM(pet)
				&& PET_NATURAL(pet) && (get_pet_level(pet) > 1) ) {

			char txt[192];
			sprite_t* tmp;
			if(-1 == select_sprite(p, &tmp))
			{
				return;
			}
			if( -1 == add_pet_to_map(pet, tmp->id))
			{
				return;
			}

#ifdef TW_VER
			snprintf(txt, sizeof txt, "親愛的%.16s，我現在正在%.16s家裏做客忘了回家的路了，如果你收到這封信的話，記得要來接我回家哦！", p->nick, tmp->nick);
#else
			snprintf(txt, sizeof txt, "亲爱的%.16s，我现在正在%.16s家里做客忘了回家的路了，如果你收到这封信的话，记得要来接我回家哦！", p->nick, tmp->nick);
#endif
			send_postcard(pet->nick, 0, p->id, 1000020, txt, tmp->id);

			SET_PET_VISIT_OTHER(pet);
			pet->trust_end_tm = get_now_tv()->tv_sec + 30 * 60;
			pet->suppl_info.mapid = tmp->id;
			//db_pet_op(0, pet, p->id);
			db_set_flag_endtime(0, pet->flag, pet->trust_end_tm, pet->id, p->id);
			respond_pet_to_map(pet, PET_OUT);
			DEBUG_LOG("PET VISIT OTHER\t[uid=%u pid=%u endtm=%d oid=%u]", p->id, pet->id, pet->trust_end_tm, tmp->id);
			break;
		}
	}
}

void pet_be_infected(map_t* map, uint32_t uid)
{
	int i;
	for (i = 0; i != map->pet_cnt; ++i) {
		pet_t* pet = &(map->pets[i]);
		if ( !PET_IS_SUPER_LAHM(pet) && (pet->sick_type == 0)
				&& PET_NATURAL(pet) && (get_pet_level(pet) > 1) ) {
			infect_pet(pet, rand()%PET_SICK_MAX);
			uint32_t uid = GET_UID_IN_HOUSE(map->id);
			db_infect_pet(0, pet, uid);
			char txt[256];
#ifdef TW_VER
			snprintf(txt, sizeof txt, "你的拉姆不小心感染了黑森林怪病，快帶它去拉姆治療室看看吧！");
			send_postcard("克勞神父", 0, uid, 1000067, txt, 0);
#else
			snprintf(txt, sizeof txt, "你的拉姆不小心感染了黑森林怪病，快带它去拉姆救治室看看吧！");
			send_postcard("克劳神父", 0, uid, 1000067, txt, 0);
#endif
			send_attr_update_noti(0, 0, GET_UID_IN_HOUSE(map->id), 3);

		}
	}
}

int do_enter_map(sprite_t* p, map_id_t newmap,map_id_t oldmap,uint32_t gdst,uint32_t gsrc)
{
	if (p->tiles || (newmap == oldmap && gdst == gsrc) || newmap <= 0 || oldmap < 0 )
		ERROR_RETURN(("error map, tiles=%p, new=%lu, old=%lu, id=%u", p->tiles, newmap, oldmap, p->id), -1);

	int err = enter_map(p, newmap, oldmap, gdst);
	if (!err) {
		// Pet Event
		map_t* map = p->tiles;

		if (map->id == 148) {
			uint32_t ding_room[] = {p->id, 0};
			msglog(statistic_logfile, 0x02020005, get_now_tv()->tv_sec, ding_room, sizeof(ding_room));
		}
		if (newmap == 209) {
			uint32_t msgbuf[] = {p->id, 1};
			msglog(statistic_logfile, 0x0409B404, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));
		}
		
		sprite_t* who;
		broadcast_sprite_info(p);
		response_proto_get_sprite(p, PROTO_MAP_ENTER, 0, NULL);
		//tell the home's owner that somebody is in his home
		if (IS_JY_MAP(newmap) && (who = get_sprite(GET_UID_IN_JY(newmap)))) {
			int home_sprite_num = get_all_house_sprite_num(GET_UID_IN_JY(newmap));
			if (p->tiles->sprite_num + home_sprite_num == 1) {
				notify_home_users_change(who, 1, 0);
			}

		}
		//¼Ӕ°
		if(IS_JY_MAP(newmap)) {
			if(JY_MAP(p->id) != newmap){
				db_add_jy_visitors(p, GET_UID_IN_JY(newmap));
				db_add_hot(p, GET_UID_IN_JY(newmap));
				cs_set_friend_home_hot(0, NULL, 0, GET_UID_IN_JY(newmap));
				//DEBUG_LOG("add homeland vistor[%u] owner:[%u]", p->id, GET_UID_IN_JY(newmap));
			}
			uint32_t msgbuff[2] = {p->id, 1};
			msglog(statistic_logfile, 0x0405BEA1, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

			db_add_hot_intem(p, GET_UID_IN_JY(newmap), home_hot);
		}
		//Ł³¡
		if(IS_PASTURE_MAP(newmap)) {
			if(PASTURE_MAP(p->id) != newmap){
				db_add_pasture_visitors(p, GET_UID_IN_PASTURE(newmap), PASTURE_VISITOR_LIST);
				//DEBUG_LOG("add pasture vistor[%u] owner:[%u]", p->id, GET_UID_IN_PASTURE(newmap));
				//db_add_hot(p, GET_UID_IN_JY(newmap));
			}
			uint32_t msgbuff[2] = {p->id, 1};
			msglog(statistic_logfile, 0x0405BCA1, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

			db_add_hot_intem(p, GET_UID_IN_PASTURE(newmap), farm_hot);
		}
		//ͬʹV԰
		if(IS_PARADISE_MAP(newmap) ) {
			if(PARADISE_MAP(p->id) != newmap){
				db_add_paradise_visitors(p, GET_UID_IN_PARADISE(newmap), 300);
				//DEBUG_LOG("add PARADISE vistor[%u] owner:[%u]", p->id, GET_UID_IN_PARADISE(newmap));
			}
			uint32_t msgbuff[2] = {p->id, 1};
			msglog(statistic_logfile, 0x0405CAA2, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

			db_add_hot_intem(p, GET_UID_IN_PARADISE(newmap), angel_hot);
		}
		//²Ό�
		if(IS_SHOP_MAP(newmap)) {
			//DEBUG_LOG("add restaurant vistor[%u] owner:[%u]", p->id, GET_UID_SHOP_MAP(newmap));
			uint32_t msgbuff[2] = {p->id, 1};
			msglog(statistic_logfile, 0x0405C1A1, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

			db_add_hot_intem(p, GET_UID_SHOP_MAP(newmap), restaurant_hot);
		}
		//½͊Њ		i
		if(IS_CLASSROOM_MAP(newmap)) {
			//DEBUG_LOG("add class vistor[%u] owner:[%u]", p->id, GET_UID_IN_CLASSID(newmap));
			uint32_t msgbuff[2] = {p->id, 1};
			msglog(statistic_logfile, 0x0405C3A1, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

			db_add_hot_intem(p, GET_UID_CLASSROOM_MAP(newmap), classroom_hot);
		}
		/**/
		//Сϝ
		if(IS_HOUSE_MAP(newmap)){
			//db_add_hot_intem(p, GET_UID_CLASSROOM_MAP(newmap), house_hot);
			uint32_t msgbuff[2] = {p->id, 1};
			msglog(statistic_logfile, 0x0405C5A1, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}

		if(IS_EXHIBIT_MAP(newmap))
		{
			if(GET_EXHIBIT_MAP(p->id) != newmap)
			{
				dungeon_add_exhibit_visitors(p, GET_UID_IN_EXHIBIT(newmap));
			}
		}

	}

	//create npc when enter map 1
#ifdef LUA_TEST
	if ( !IS_NPC_ID(p->id) ) { //is USER
		DEBUG_LOG("IS USER");
		if ( newmap == 1 ) {
			DEBUG_LOG("ENTER MAP 1, CREATE NPC");
			create_npc_test();
		}
	}
#endif
	return err;

}

int do_maps_user(sprite_t* p)
{
	int i, k, count;
	map_t* map;

	i = sizeof(protocol_t) + 4;
	count = 0;
	for (k = 0; k < NORMAL_MAP_NUM; k++) {
		map = &normal_map[k];
		if (map->id == 0)
			continue;
		PKG_UINT32(msg, map->id, i);
		PKG_UINT32(msg, map->sprite_num, i);
		count++;
	}

	init_proto_head(msg, PROTO_MAPS_USER, i);
	k = sizeof(protocol_t);
	PKG_UINT32(msg, count, k);

	send_to_self(p, msg, i, 1);
	return 0;
}

int do_map_info(sprite_t *p, map_id_t mapid)
{
	map_t* m = get_map(mapid);
	if (!m)	ERROR_RETURN( ("error map=%lu, id=%u", mapid, p->id), -1 );

	int i = sizeof(protocol_t);
	PKG_MAP_ID(msg, mapid, i);
	PKG_STR(msg, m->name, i, sizeof(m->name));
	PKG_UINT32(msg, m->party_period, i);
	PKG_UINT32(msg, m->item_count, i);
	init_proto_head(msg, PROTO_MAP_INFO, i);
	return send_to_self(p, msg, i, 1);
}

typedef struct PackUserBuf {
	uint8_t** buf;
	int*      idx;
} pack_user_buf_t;

static void pack_across_svr_user(void* key, void* sinfo, void* data)
{
	pack_user_buf_t* buf = data;

	sprite_t* p = sinfo;
	//if (p->login && IS_SPRITE_VISIBLE(p)) {
		int pkgsize = pkg_sprite_simply(p, *(buf->buf));
		*(buf->buf) += pkgsize;
		++(*(buf->idx));
	//}
}

int do_list_user(sprite_t* p, map_id_t mapid, uint32_t grid)
{
	if (mapid != p->tiles->id ) {
		WARN_LOG("uid=%u request map=%lu while in map=%lu", p->id, mapid, p->tiles->id);
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, 1, i);
		i += pkg_sprite_simply(p, msg + i);
		init_proto_head(msg, p->waitcmd, i);
		return send_to_self(p, msg, i, 1);
	}

	sprite_t* l;
	list_head_t* t;
	int i = 0, pkgsize;
	uint8_t* tmp = msg + sizeof(protocol_t) + 4;
	list_for_each(t, &p->tiles->sprite_list_head) {
		l = list_entry(t, sprite_t, map_list);
		if (l->login && IS_SPRITE_VISIBLE(l) && grid == l->sub_grid) {
			pkgsize = pkg_sprite_simply(l, tmp);
			tmp += pkgsize;
			++i;
		}
	}

	DEBUG_LOG("LIST USER COUNT = %d",i);

//-----------------------------------------
	pack_user_buf_t pbuf = { &tmp, &i };
	traverse_across_svr_sprites(mapid, pack_across_svr_user, &pbuf);
//-----------------------------------------
	pkgsize = tmp - msg;
	*(uint32_t *)(msg + sizeof(protocol_t)) = htonl(i);
	init_proto_head(msg, PROTO_USER_LIST, pkgsize);
	send_to_self(p, msg, pkgsize, 1);

	DEBUG_LOG("LIST USER COUNT = %d",i);

	return 0;
}

int do_action(sprite_t* p, uint32_t action, uint8_t direction)
{
	if (p->action > ACTION_MAX) {
		WARN_LOG("%u do action %u while transformed to be %u", p->id, action, p->action);
	}
	time_t temp = -1;
	switch (action) {
	case ACTION_levelup:
	case ACTION_gotmedal:
		temp = get_now_tv()->tv_sec;
		if ( temp < p->last_action_time + 30 )
			return send_to_self_error(p, p->waitcmd, -ERR_client_not_proc, 1);
		p->last_action_time = temp;
	case ACTION_IDLE:
	case ACTION_WAVE:
	case ACTION_PAOPAO:
		p->action    = ACTION_IDLE;
		p->direction = 0;
		break;
	case ACTION_SIT:
	case ACTION_DANCE:
		p->action    = action;
		p->direction = direction;
		break;
	default:
		ERROR_RETURN( ("error action=%d, id=%u", action, p->id), -1 );
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, action, i);
	PKG_UINT8(msg, direction, i);
	init_proto_head(msg, PROTO_ACTION, i);
	send_to_map(p, msg, i, 1);

	return 0;
}

int do_paopao(sprite_t *p)
{
	if (p->action > ACTION_MAX)
		ERROR_RETURN(("error paopao, action=%d, id=%u", p->action, p->id), -1);

	srand(now.tv_sec);

	const int loop = 100;
	int x[loop], y[loop];
	x[0] = p->posX;
	y[0] = p->posY;

	int idx;
	float arc;

// TODO - start with idx set to be 1 would be better
	for ( idx = 0; idx < loop - 1; idx++) {
		arc = 1 + (int)( M_PI * (rand() / (RAND_MAX + 1.0)) );

		x[idx + 1] = x[idx] + 20 * cos(arc) / 1.5;
		y[idx + 1] = y[idx] + 20 * sin(arc);

		if (arc > 3.05 || y[idx + 1] >= 560 ) break;
	}

	int n, j;
	n = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, n);
	PKG_UINT8(msg, idx, n);
	for (j = 0; j < idx; j++) {
		PKG_UINT32(msg, x[j], n);
		PKG_UINT32(msg, y[j], n);
	}
	init_proto_head(msg, PROTO_PAOPAO, n);
	send_to_map(p, msg, n, 1);

	return 0;
}

int do_walk(sprite_t* p, uint32_t xpos, uint32_t ypos, uint32_t item)
{
	reset_common_action (p);
	p->posX      = xpos;
	p->posY      = ypos;

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, p->posX, i);
	PKG_UINT32(msg, p->posY, i);
	PKG_UINT32(msg, item, i);
	PKG_UINT32(msg, p->sub_grid, i);

	init_proto_head(msg, PROTO_WALK, i);
	send_to_map(p, msg, i, 1);
	return 0;
}

int do_reply_invite(sprite_t* p, uint32_t id, map_id_t map_id, uint8_t accept)
{
	int i;
	char txt[256];

	response_proto_head(p, PROTO_RSP_FRIEND_INVITE, 0);

	sprite_t* who = get_sprite(id);
	if (!who ) return 0;

	if (!accept) {
#ifdef TW_VER
		i = sprintf(txt, "%.16s拒絕了您的邀請.", p->nick);
#else
		i = sprintf(txt, "%.16s拒绝了您的邀请.", p->nick);
#endif
	} else {
#ifdef TW_VER
		i = sprintf(txt, "%.16s接受了您的邀請.", p->nick);
#else
		i = sprintf(txt, "%.16s接受了您的邀请.", p->nick);
#endif
	}
	i = text_notify_pkg(msg, PROTO_RSP_FRIEND_INVITE, p->id, map_id, p->sub_grid, p->nick, !!accept, i, txt);
	return send_to_self(who, msg, i, 0);
}

int do_invite_friend(sprite_t* p, uint32_t id, map_id_t map_id)
{
	map_t* map = get_map(map_id);
	if (!map) {
		ERROR_RETURN(("not exists map=%lu, id=%u", map_id, p->id), -1);
	}

	sprite_t* rep = get_sprite(id);
	if (!rep) {
		return send_to_self_error(p, PROTO_FRIEND_INVITE, -ERR_user_offline, 1);
	}

	char txt[256];
	int i = 0;
	sprite_t* mp = NULL;
#ifdef TW_VER
	if (IS_HOUSE_MAP(map_id) || IS_HOUSE1_MAP(map_id) || IS_HOUSE2_MAP(map_id)) {
		if (p->id == GET_UID_IN_HOUSE(map_id)) {
			i = sprintf(txt, "%s邀請您到%s去，您是否願意？", p->nick, "他/她的小屋" );
		} else if ((mp = get_sprite(GET_UID_IN_HOUSE(map_id)))) {
			i = sprintf(txt, "%s邀請您到%s的小屋去，您是否願意？", p->nick, mp->nick );
		} else {
			i = sprintf(txt, "%s邀請您到%d的小屋去，您是否願意？", p->nick, (GET_UID_IN_HOUSE(map_id)));
		}
	} else if (IS_JY_MAP(map_id)) {
		if (p->id == GET_UID_IN_JY(map_id)) {
			i = sprintf(txt, "%s邀請您到%s去，您是否願意？", p->nick, "他/她的家园" );
		} else if ((mp = get_sprite(GET_UID_IN_JY(map_id)))) {
			i = sprintf(txt, "%s邀請您到%s家园去，您是否願意？", p->nick, mp->nick);
		} else {
			i = sprintf(txt, "%s邀請您到%d的家园去，您是否願意？", p->nick, GET_UID_IN_JY(map_id));
		}
	} else if (IS_CLASS_MAP(map_id)) {
		if (p->id == GET_UID_IN_CLASSID(map_id)) {
			i = sprintf(txt, "%s邀請您到%s去，您是否願意？", p->nick, "他/她的班級" );
		} else if ((mp = get_sprite(GET_UID_IN_CLASSID(map_id)))) {
			i = sprintf(txt, "%s邀請您到%s班級去，您是否願意？", p->nick, mp->nick);
		} else {
			i = sprintf(txt, "%s邀請您到%d的班級去，您是否願意？", p->nick, GET_UID_IN_CLASSID(map_id));
		}
	} else if (IS_PASTURE_MAP(map_id)) {
		DEBUG_LOG("INVITE TO PASTURE MAP [%d %d]", p->id, (uint32_t)map_id);
		if (p->id == GET_UID_IN_CLASSID(map_id)) {
			i = sprintf(txt, "%s邀請您到%s去，您是否願意？", p->nick, "他/她的牧場" );
		} else if ((mp = get_sprite(GET_UID_IN_CLASSID(map_id)))) {
			i = sprintf(txt, "%s邀請您到%s的牧場去，您是否願意？", p->nick, mp->nick);
		} else {
			i = sprintf(txt, "%s邀請您到%d的牧場去，您是否願意？", p->nick, GET_UID_IN_CLASSID(map_id));
		}
	} else {
		i = sprintf(txt, "%s邀請您到%s去，您是否願意？", p->nick, map->name);
	}
	//goto classes
#else
	if (IS_HOUSE_MAP(map_id) || IS_HOUSE1_MAP(map_id) || IS_HOUSE2_MAP(map_id)) {
		DEBUG_LOG("INVITE TO HOUSE MAP [%d %d]", p->id, (uint32_t)map_id);
		if (p->id == GET_UID_IN_HOUSE(map_id)) {
			i = sprintf(txt, "%s邀请您到%s去，您是否愿意？", p->nick, "他/她的小屋" );
		} else if ((mp = get_sprite(GET_UID_IN_HOUSE(map_id)))) {
			i = sprintf(txt, "%s邀请您到%s的小屋去，您是否愿意？", p->nick, mp->nick );
		} else {
			i = sprintf(txt, "%s邀请您到%d的小屋去，您是否愿意？", p->nick, (GET_UID_IN_HOUSE(map_id)));
		}
	} else if (IS_JY_MAP(map_id)) {
		DEBUG_LOG("INVITE TO JY MAP [%d %d]", p->id, (uint32_t)map_id);
		if (p->id == GET_UID_IN_JY(map_id)) {
			i = sprintf(txt, "%s邀请您到%s去，您是否愿意？", p->nick, "他/她的家园" );
		} else if ((mp = get_sprite(GET_UID_IN_JY(map_id)))) {
			i = sprintf(txt, "%s邀请您到%s家园去，您是否愿意？", p->nick, mp->nick);
		} else {
			i = sprintf(txt, "%s邀请您到%d的家园去，您是否愿意？", p->nick, GET_UID_IN_JY(map_id));
		}
	} else if (IS_CLASS_MAP(map_id)) {
		DEBUG_LOG("INVITE TO CLASS MAP [%d %d]", p->id, (uint32_t)map_id);
		if (p->id == GET_UID_IN_CLASSID(map_id)) {
			i = sprintf(txt, "%s邀请您到%s去，您是否愿意？", p->nick, "他/她的班级" );
		} else if ((mp = get_sprite(GET_UID_IN_CLASSID(map_id)))) {
			i = sprintf(txt, "%s邀请您到%s班级去，您是否愿意？", p->nick, mp->nick);
		} else {
			i = sprintf(txt, "%s邀请您到%d的班级去，您是否愿意？", p->nick, GET_UID_IN_CLASSID(map_id));
		}
	} else if (IS_PASTURE_MAP(map_id)) {
		DEBUG_LOG("INVITE TO PASTURE MAP [%d %d]", p->id, (uint32_t)map_id);
		if (p->id == GET_UID_IN_CLASSID(map_id)) {
			i = sprintf(txt, "%s邀请您到%s去，您是否愿意？", p->nick, "他/她的牧场" );
		} else if ((mp = get_sprite(GET_UID_IN_CLASSID(map_id)))) {
			i = sprintf(txt, "%s邀请您到%s的牧场去，您是否愿意？", p->nick, mp->nick);
		} else {
			i = sprintf(txt, "%s邀请您到%d的牧场去，您是否愿意？", p->nick, GET_UID_IN_CLASSID(map_id));
		}
	} else {
		i = sprintf(txt, "%s邀请您到%s去，您是否愿意？", p->nick, map->name);
	}
	//goto classes
#endif
	i = text_notify_pkg(msg, PROTO_FRIEND_INVITE, p->id, map_id, p->sub_grid, p->nick, 0, i, txt);
	send_to_self(rep, msg, i, 0);

	response_proto_head(p, PROTO_FRIEND_INVITE, 0);
	return 0;
}

int do_exchange_item(sprite_t* p, int idx, uint32_t item_num)
{
	const exchange_item_t* exitm = get_exchg_entry(idx);
	return (item_num ? db_exchg_collected_items(p, exitm, item_num) : db_exchange_items(p, exitm));
}

int show_item_use_expired (void* owner, void* data)
{
	int i;
	sprite_t* p = owner;

	i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, p->action, i);
	init_proto_head(msg, PROTO_ITEM_SHOW_EXPIRE, i);
	send_to_map(p, msg, i, 0);

	DEBUG_LOG("SHOW EXPIRED\t[%u %d]", p->id, p->action);
	if (p->action > ACTION_MAX)
		p->action = 0;
	return 0; // returns 0 means the `timer scanner` should remove this event
}

int do_post_blackboard(sprite_t* p, uint32_t board, uint32_t msg_len, uint8_t* content)
{
	char buf[msg_len + 28];
	int i = 0;

	PKG_H_UINT32 (buf, board, i);
	PKG_H_UINT32 (buf, p->color, i);
	PKG_STR (buf, p->nick, i, 16);
	PKG_H_UINT32 (buf, msg_len, i);
	PKG_STR (buf, content, i, msg_len);

	return send_request_to_db(SVR_PROTO_ADD_BOARD, p, i, buf, p->id);
}

int do_show_item_use_callback(sprite_t* p, uint32_t itemid)
{
#define PASSIVE_FLASH 1
#define ACTIVE_FLASH  2
#define ALL_FLASH	  3

	uint32_t changeid = 0;
	uint32_t flashtag = 0;
	int j = 0;
	userid_t userid;

	UNPKG_H_UINT32(p->session, userid, j);
	p->sess_len = 0;
	sprite_t* who = get_sprite(userid);
	if (!who && !(who = get_across_svr_sprite(userid, p->tiles->id))) {
		return send_to_self_error(p, PROTO_SHOW_ITEM_USE, -ERR_user_offline, 1);
	}
	p->last_show_item = get_now_tv()->tv_sec;  //init at the beginning

	if ( !IS_ADMIN_ID(userid) ) {
		if(is_wear_item(who, 12247)){
			if(is_wear_item(p, 12247)){
				//don't change, and two gays flash
				flashtag = ALL_FLASH;
			}else{
				//change self, the passive gay flash
				who = p;
				changeid = p->id;
				flashtag = PASSIVE_FLASH;
			}
		}else{
			//change passive
			changeid = userid;
		}
	}

	if(changeid){
		
		//元宵节投掷元宵特别活动
		//攻击已经变身成元宵的玩家，玩家元宵被击破，并回复常规状态
		if( who->action == 150017 && itemid == who->action )
		{
			//统计 击破汤圆变身
			{
				uint32_t msgbuff[2]= {p->id, 1};
				msglog(statistic_logfile, 0x0409C319, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
			}
			who->action = 0;
			//create award to attacker and notice to map
			ld_attack_player( p, who );
		}
		else
		{
			//统计 投掷汤圆变身
			if( itemid == 150017 )
			{
				uint32_t msgbuff[2]= {p->id, 1};
				msglog(statistic_logfile, 0x0409C318, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
			}

			who->action = itemid;
			uint32_t time_lasts = 20;
			if( itemid == 150017 )
			{
				time_lasts = 60;
				//notice db
				{
					int sendlen = 0;
					uint8_t buff[128];
					item_kind_t* ik = find_kind_of_item( 1351300 );
					item_t* it = get_item( ik, 1351300 );
					PKG_H_UINT32( buff, 0, sendlen );
					PKG_H_UINT32( buff, 1, sendlen );
					PKG_H_UINT32( buff, 203, sendlen );
					PKG_H_UINT32( buff, 0, sendlen );
					pkg_item_kind( p, buff, 1351300, &sendlen );
					PKG_H_UINT32( buff, 1351300, sendlen );
					PKG_H_UINT32( buff, 1, sendlen );
					PKG_H_UINT32( buff, it->max, sendlen );
					send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );
				}
				//send to player
				{
					int msg_len = sizeof(protocol_t);
					//reason 1--attack 2--be attack
					PKG_UINT32( msg, 1, msg_len );
					PKG_UINT32( msg, 1351300, msg_len );
					PKG_UINT32( msg, 1, msg_len );
					init_proto_head( msg, PROTO_LD_GET_AWARD, msg_len );
					send_to_self( p, msg, msg_len, 0 );
				}
			}
			if (who->fdsess) {
				add_event(&(who->timer_list), show_item_use_expired, who, 0, now.tv_sec + time_lasts, ADD_EVENT_REPLACE_UNCONDITIONALLY);
			}
		}
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, userid, i);
	PKG_UINT32(msg, itemid, i);
	PKG_UINT32(msg, changeid, i);
	PKG_UINT32(msg, flashtag, i);
	init_proto_head(msg, PROTO_SHOW_ITEM_USE, i);
	send_to_map(p, msg, i, 1);
#undef PASSIVE_FLASH
#undef ACTIVE_FLASH
#undef ALL_FLASH

	return 0;
}

int do_list_items(sprite_t* p, uint32_t userid, uint32_t itemtype, uint8_t flag, uint8_t kind_flag)
{
	if (IS_NPC_ID(userid)) {
		sprite_t *npc = get_npc(userid);
		if (!npc) ERROR_RETURN(("error id, type=%u, id=%u,%u", itemtype, userid, p->id), -1);

		return response_proto_list_items(p, userid, npc->items, npc->item_cnt);
	} else if (IS_GUEST_ID (userid)) {
		return response_proto_list_items(p, userid, NULL, 0);
	}

	if (userid == p->id && (flag & ITEM_ON_BODY_FLAG)) {
		return response_proto_list_items(p, p->id, p->items, p->item_cnt);
	}
	return db_get_items(p, itemtype, flag, userid, kind_flag);
}

int do_get_group(sprite_t* p, uint32_t groupid)
{
	game_group_t* group = get_game_group(groupid);
	if (!group)
		ERROR_RETURN(("not exists groupid=%d, id=%u", groupid, p->id), -1);

	int i = sizeof(protocol_t), j;
	PKG_UINT32(msg, group->game->id, i);
	PKG_UINT8(msg, group->type, i);
	PKG_UINT32(msg, group->count, i);
	for (j = 0; j < group->count; j++)
		PKG_UINT32(msg, group->players[j]->id, i);
	init_proto_head(msg, PROTO_GROUP_INFO, i);
	send_to_self(p, msg, i, 1);

	return 0;
}

int do_get_user(sprite_t* p, uint32_t uid)
{
	if (!uid) ERROR_RETURN(("error user id=%u, %u", p->id, uid), -1);

	if (IS_NPC_ID(uid)) {
		sprite_t* npc = get_npc(uid);
		if (!npc) ERROR_RETURN(("error user=%d, id=%u", uid, p->id), -1);

		response_proto_get_sprite(npc, p->waitcmd, p->waitcmd == PROTO_USER_DETAIL, p);
		return 0;
	}

	sprite_t* who = get_sprite(uid);
	if ( who || (who = get_across_svr_sprite(uid, p->tiles->id)) ) {
		response_proto_get_sprite(who, p->waitcmd, p->waitcmd == PROTO_USER_DETAIL, p);
	} else {
		return db_get_sprite(p, uid);
	}

	return 0;
}

#define MAX_LIMIT_TIME_OF_TALK		60
#define TALK_TIME_INTERVAL 			3
#define BAN_USER_TALK_TIME			300

int do_talk(sprite_t* p, const uint8_t mesg[], int msglen, uint32_t recvrid)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg + 4, p->id, i);
	PKG_STR(msg + 4, p->nick, i, sizeof(p->nick));
	PKG_UINT32(msg + 4, recvrid, i);
	PKG_UINT32(msg + 4, msglen, i);
	PKG_STR(msg + 4, mesg, i, msglen);
	init_proto_head(msg + 4, PROTO_TALK, i);

	sprite_t* receiver;
	if (recvrid == 0) {
		if (p->group && INTERACT_SINGLEPLAYER_GAME(p->group)) {
			send_to_group(p->group, msg + 4, i);
			p->waitcmd = 0;
		} else {

		    send_chat_monitor(SVR_PROTO_CHATLOG, p, recvrid, mesg, msglen);
		    
			uint32_t now_time = get_now_tv()->tv_sec;
			//DEBUG_LOG("user talk time state: uid[%u] flag[%u] now_time[%u] ban_time[%u]", p->id, p->user_talk_flag, now_time, p->ban_talk_time);
			//用户被禁言5分钟
			if (p->user_talk_flag == 1) {
				if (now_time - p->ban_talk_time >= BAN_USER_TALK_TIME) {
					p->user_talk_flag = 0;
				} else {
					return send_to_self_error(p, p->waitcmd, -ERR_cannot_talk_has_been_ban, 1);
				}
			}

			//3秒钟重复说话
			if (now_time - p->talk_time[p->cur_talk_idx] < TALK_TIME_INTERVAL) {
				DEBUG_LOG("CANNOT TALK TO MAP: uid[%u] now_time[%u] last_talk_time[%u]", p->id, now_time, p->talk_time[p->cur_talk_idx]);
				return send_to_self_error(p, p->waitcmd, -ERR_cannot_talk_time_limit, 1);
			}

			//1分钟10次说话
			if (p->talk_time[p->cur_talk_idx] != 0) {
				int next_idx = (p->cur_talk_idx + 1) % MAX_TALK_COUNT_IN_LIMIT_TIME;
				if (now_time - p->talk_time[next_idx] < MAX_LIMIT_TIME_OF_TALK) {
					return send_to_self_error(p, p->waitcmd, -ERR_cannot_talk_count_limit, 1);
				}
				p->talk_time[next_idx] = now_time;
				p->cur_talk_idx = next_idx;
			} else {
				p->talk_time[p->cur_talk_idx] = now_time;
			}

			send_to_map(p, msg + 4, i, 1);
		}
	} else if ((receiver = get_sprite(recvrid))) {
		send_to_self(receiver, msg + 4, i, 0);
		send_to_self(p, msg + 4, i, 1);
	} else {
		*(uint32_t*)msg = p->id;
		chat_across_svr(recvrid, msg, i + 4);
		send_to_self(p, msg + 4, i, 1);
	}

	return 0;
}

int do_throw_item (sprite_t *p, uint32_t itemid, uint32_t x, uint32_t y)
{
	item_t *itm;

	if (!(itm = get_item_prop (itemid)))
		ERROR_RETURN (("can't find the item=%d, id=%u", itemid, p->id), -1);

	//2011年火神杯boss水弹攻击
	uint32_t water_bomb_id = 15001;
	if( water_bomb_id == itemid )
	{
		fb_water_bomb_attack( p, x, y );
	}

	if( itemid == 150015 || itemid == 150016 )
	{
		//统计投掷 冬之花 和春之花的人数
		{
			uint32_t msg_id = 0x0409C2FC + ( itemid - 150015 );
			uint32_t msgbuff[2]= { p->id, 1 };
			msglog(statistic_logfile, msg_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}
	if(itemid == 150019 || itemid == 150020){
		uint32_t msg_id = 0x0409C3E9 + ( itemid - 150019 );
		uint32_t msgbuff[2]= { p->id, 1 };
		msglog(statistic_logfile, msg_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	p->sess_len = pkg_proto_throw_item (p->session, p, itemid, x, y);
	//投掷元宵时，同时使用发送变身道具使用消息，为避免扣除两次，则投掷时不扣除
	if (itm->price == 0 || itemid == 150017) {
		send_to_map (p, p->session, p->sess_len, 1);
		p->sess_len = 0;
		return 0;
	}

	return db_single_item_op(p, p->id, itemid, 1, 0);
}

static inline void
pack_online_pkg(sprite_t* p, uint32_t friend_cnt)
{
	int idx = sizeof(protocol_t);
	if (friend_cnt) {
		PKG_UINT16(p->session, 1, idx); // online cnt
		PKG_UINT16(p->session, 0, idx); // online id == 0 --> same svr as p
		PKG_UINT32(p->session, friend_cnt, idx);
	} else {
		PKG_UINT16(p->session, 0, idx); // online cnt
		p->sess_len = idx;
	}
}

int do_users_online(sprite_t* p, int count, uint32_t* uin, int return_all)
{
	int i, k = 4;
	uint8_t buf[(4 * SPRITE_FRIENDS_MAX) + 4];
	uint32_t friend_cnt = 0;
	p->sess_len = sizeof(protocol_t) + 8;
	for (i = 0; i != count; ++i) {
		if ( get_sprite(uin[i]) ) {
			PKG_UINT32(p->session, uin[i], p->sess_len);
			PKG_UINT8(p->session, 1, p->sess_len);
			++friend_cnt;
		} else {
			if (return_all) {
				PKG_UINT32(p->session, uin[i], p->sess_len);
				PKG_UINT8(p->session, 0, p->sess_len);
				++friend_cnt;
			}
			PKG_H_UINT32(buf, uin[i], k);
		}
	}

	pack_online_pkg(p, friend_cnt);
	if ( k > 4 ) {
		*((uint32_t*)buf) = (k - 4) / 4;
		chk_online_across_svr(p, buf, k);
	} else {
		chk_online_across_svr_callback(p, p->id, 0, 0);
	}

	return 0;
}

int pkg_useritem(sprite_t* p, struct user_itm_use_buf* itm, uint32_t itemid, uint8_t is_used)
{
	if (!(itm->prop = get_item_prop(itemid))) {
		ERROR_RETURN(("error item id=%u, uin=%u", itemid, p->id), -1);
	}
	itm->id = itemid;

	if (!is_used) { // put off clothes
		if (!is_wear_item(p, itm->id)) {
			ERROR_RETURN( ("not use suit item, id=%u, item=%d", p->id, itm->id), -1 );
		}
		return (itm->mask = USED_ITEM_MASK);
	} else { // put on clothes
		int unwearable = is_wear_item(p, itm->id)
							+ (itm_vip_only(itm->prop) && !ISVIP(p->flag));
		if (unwearable) {
			ERROR_RETURN( ("item unwearable: uid=%u, item=%u flag=%d", p->id, itemid, unwearable), -1 );
		}
		return (itm->mask = ADD_ITEM_MASK | USED_ITEM_MASK);
	}
}

void req_add_friend_notifier(sprite_t* p, sprite_t* friend, uint32_t id)
{
	char txt[256];
#ifdef TW_VER
	int j = sprintf(txt, "%s請求將您加為好友，您是否同意？", p->nick);
#else
	int j = sprintf(txt, "%s请求将您加为好友，您是否同意？", p->nick);
#endif

	j = text_notify_pkg(msg + 4, PROTO_FRIEND_ADD, p->id, 0, p->sub_grid, p->nick, 0, j, txt);
	if (friend) {
		send_to_self(friend, msg + 4, j, 0);
	} else {
		*(uint32_t*)msg = p->id;
		chat_across_svr(id, msg, j + 4);
	}
}

int text_notify_pkg_pic(uint8_t buf[], int cmd, uint32_t id, map_id_t map, uint32_t grid,
			const char nick[], int accept, uint32_t pic, int len, const char txt[])
{
	int i = sizeof(protocol_t);
	PKG_UINT32(buf, cmd, i);
	PKG_MAP_ID(buf, map, i);
	PKG_UINT32(buf, grid, i);
	PKG_UINT32(buf, id, i);
	PKG_STR(buf, nick, i, 16);
	PKG_UINT32(buf, accept, i);
	PKG_UINT32(buf, now.tv_sec, i);
	PKG_UINT32(buf, pic, i);
	PKG_UINT32(buf, len, i);

	PKG_STR(buf, txt, i, len);
	init_proto_head(buf, PROTO_TEXT_NOTIFY, i);
	return i;
}

int pkg_items(int count, const uint32_t *p, uint8_t *buf, int detail)
{
	int i, j;

	j = 0;
	PKG_UINT8 (buf, count, j);
	for (i = 0; i < count; i++) {
		PKG_UINT32 (buf, p[i], j);
	}

	return j;
}

int pack_activity_info(sprite_t* p, uint8_t* buf)
{
	int i = 0;
	uint32_t team_id = p->team_id;
	DEBUG_LOG("team_id: %u  notify client teamid info", team_id);
	PKG_UINT32(buf, team_id, i);

	return 32;
}
/*
	affect 204
*/
int pkg_sprite_detail( sprite_t* p, uint8_t* buf)
{
	int j;
	uint8_t st;

	j = 0;
	PKG_UINT32 (buf, p->id, j);
	PKG_STR (buf, p->nick, j, 16);
	PKG_UINT32(buf, p->inviter_id, j);
	PKG_UINT32(buf, p->old_invitee_cnt, j);
	PKG_UINT32(buf, p->invitee_cnt, j);
	PKG_UINT32 (buf, p->color, j);

	PKG_UINT32 (buf, p->flag, j);
	PKG_UINT32 (buf, p->birth, j);
	PKG_UINT32 (buf, p->exp, j);

	PKG_UINT32 (buf, p->strong, j);
	PKG_UINT32 (buf, p->iq, j);

	PKG_UINT32 (buf, p->lovely, j);
	PKG_UINT32 (buf, p->game_king, j);
	PKG_UINT32 (buf, p->yxb, j);
	PKG_UINT32 (buf, p->engineer, j);
	PKG_UINT32 (buf, calculation_level_from_fashion(p->fashion), j);
	PKG_UINT32 (buf, calculation_level_from_skill(p->cultivate_skill), j);
	PKG_UINT32 (buf, calculation_level_from_skill(p->breed_skill), j);
	PKG_UINT32 (buf, p->dining_flag, j);
	PKG_UINT32 (buf, p->dining_level, j);
	PKG_UINT32 (buf, p->vip_level, j);
	PKG_UINT32 (buf, p->vip_month, j);
	PKG_UINT32 (buf, p->vip_value, j);
	PKG_UINT32 (buf, p->vip_end, j);
	PKG_UINT32 (buf, p->vip_autopay, j);

	if (p->tiles)
		PKG_MAP_ID(buf, p->tiles->id, j);
	else
		PKG_MAP_ID(buf, 0, j);

	st = get_sprite_status(p);
	PKG_UINT8 (buf, st, j);
	PKG_UINT32 (buf, p->action, j);
	PKG_UINT8 (buf, p->direction, j);

	PKG_UINT32 (buf, p->posX, j);
	PKG_UINT32 (buf, p->posY, j);
	PKG_UINT32 (buf, p->sub_grid, j);
		//
	j += pack_activity_info(p, buf + j);
	PKG_UINT32(buf, p->dragon_id, j);
	PKG_STR (buf, p->dragon_name, j, DRAGON_NICK_LEN);
	PKG_UINT32(buf, p->dragon_growth, j);

	uint32_t dungeon_lvl = dungeon_get_level_by_exp(p->dungeon_explore_exp);
	PKG_UINT32(buf, dungeon_lvl, j);

	PKG_UINT32(buf, p->car_status, j);
	if(p->car_status){
		PKG_UINT32(buf, p->id, j);
		pack_car_info(buf, &(p->car_driving), &j);
	}
	PKG_UINT32(buf, p->animal_nbr, j);
	if(p->animal_nbr){
		PKG_UINT32(buf, p->id, j);
		pkg_animal_info(buf, &(p->animal_follow_info.tag_animal), &j);
	}

	j += pkg_items(p->item_cnt, p->items, buf + j, 0);
	PKG_UINT32(buf, p->sg_guidering_flag, j);

	return j;
}

/*
	affect 203401405
*/
int pkg_sprite_simply( sprite_t *p, uint8_t *buf)
{
	int j;
	uint8_t st;

	j = 0;
	PKG_UINT32(buf, p->id, j);
	PKG_STR(buf, p->nick, j, 16);
	PKG_UINT32(buf, p->inviter_id, j);
	PKG_UINT32(buf, p->old_invitee_cnt, j);
	PKG_UINT32(buf, p->invitee_cnt, j);
	PKG_UINT32(buf, p->color, j);
	PKG_UINT32(buf, p->flag, j);
	if (p->tiles)
		PKG_MAP_ID(buf, p->tiles->id, j);
	else
		PKG_MAP_ID(buf, 0, j);

	st = get_sprite_status(p);
	PKG_UINT8(buf, st, j);
	PKG_UINT32(buf, p->action, j);
	PKG_UINT32(buf, p->lamu_action, j);
	PKG_UINT8(buf, p->direction, j);

	PKG_UINT32(buf, p->posX, j);
	PKG_UINT32(buf, p->posY, j);

	PKG_UINT32(buf, p->sub_grid, j);

	PKG_UINT32(buf, p->action2, j);

	//pet info
	if (p->followed) {
		uint8_t level = get_pet_level(p->followed);
		PKG_UINT32(buf, p->followed->id, j);
		PKG_STR(buf,p->followed->nick,j,sizeof(p->followed->nick));
		PKG_UINT32(buf, p->followed->color, j);
		PKG_UINT8(buf, level, j);
		PKG_UINT32(buf, p->followed->skill, j);
		PKG_UINT32(buf, p->followed->sick_type, j);
		PKG_UINT32(buf, p->followed->skill_ex[0], j);
		PKG_UINT32(buf, p->followed->skill_ex[1], j);
		PKG_UINT32(buf, p->followed->skill_ex[2], j);
		PKG_UINT32(buf, p->followed->skill_bits, j);
		PKG_UINT32(buf, p->followed->skill_value, j);
		PKG_UINT8(buf, p->followed->skill_hot[0], j);
		PKG_UINT8(buf, p->followed->skill_hot[1], j);
		PKG_UINT8(buf, p->followed->skill_hot[2], j);

		PKG_UINT32(buf, p->followed->suppl_info.pet_clothes[0], j);
		PKG_UINT32(buf, p->followed->suppl_info.pet_honor[0], j);
		PKG_UINT32(buf, pet_has_learned(p->followed, 11), j);
	} else {
		memset(buf + j, 0, 68);
		j += 68;
	}

	j += pack_activity_info(p, buf + j);
	PKG_UINT32(buf, p->dragon_id, j);
	PKG_STR (buf, p->dragon_name, j, DRAGON_NICK_LEN);
	PKG_UINT32(buf, p->dragon_growth, j);

	uint32_t dungeon_lvl = dungeon_get_level_by_exp(p->dungeon_explore_exp);
	PKG_UINT32(buf, dungeon_lvl, j);

	PKG_UINT32(buf, p->car_status, j);
	if(p->car_status){
		PKG_UINT32(buf, p->id, j);
		pack_car_info(buf, &(p->car_driving), &j);
	}
	//DEBUG_LOG("ANIMAL\t[%u %u]", p->id, p->animal_nbr);
	PKG_UINT32(buf, p->animal_nbr, j);
	if(p->animal_nbr){
		PKG_UINT32(buf, p->id, j);
		pkg_animal_info(buf, &(p->animal_follow_info.tag_animal), &j);
	}
	j += pkg_items(p->item_cnt, p->items, buf + j, 0);
	PKG_UINT32(buf, p->sg_guidering_flag, j);
	return j;
}

static void pkg_home_list(void* key, void* spri, void* userdata)
{
	map_t*    map;
	sprite_t* p    = spri;
	int*      idx  = userdata;
	int i = sizeof (protocol_t) + *idx * 28 + 4;
	if (IS_GUEST_ID(p->id) || IS_NPC_ID(p->id) || (p->flag & SPRITE_HOME_PUBLIC_FLAG))
		return;

	PKG_UINT32(msg, p->id, i);
	PKG_STR(msg, p->nick, i, 16);
	PKG_UINT32(msg, ISVIP(p->flag), i);
	if ( !(map = get_map(p->id)) ) {
		PKG_UINT32(msg, 0, i);
	} else {
		PKG_UINT32(msg, map->sprite_num + get_across_svr_sprite_cnt(p->id), i);
	}

	++(*idx);
}

int do_get_home_list (sprite_t *p)
{
	int i, count = 0;

	traverse_sprites(pkg_home_list, &count);
	i = sizeof(protocol_t);
	PKG_UINT32(msg, count, i);

	i = count * 28 + 4 + sizeof(protocol_t);
	init_proto_head(msg, PROTO_GET_HOME_LIST, i);
	//DEBUG_LOG("GET HOME LIST[uid=%u msglen=%u]", p->id, i);
	return send_to_self(p, msg, i, 1);
}

int do_promotion_tips(sprite_t* p, uint16_t level)
{
	static char logic_buf[256];

	if (level > 120) {
		return 0;
	}

	int len;
	switch (level) {
	case 5:
#ifdef TW_VER
		len = snprintf(logic_buf, sizeof logic_buf,
				"恭喜你升級了，你現在的等級是 %d 級，可以參加 SMC（超級摩爾俱樂部）的小考驗咯！",
				level);
#else
		len = snprintf(logic_buf, sizeof logic_buf,
				"恭喜你升级了，你现在的等级是 %d 级，可以参加 SMC（超级摩尔俱乐部）的小考验咯！",
				level);
#endif
		break;
	case 10:
#ifdef TW_VER
		len = snprintf(logic_buf, sizeof logic_buf,
				"恭喜你升級了，你現在的等級是 %d 級，已經可以獲取 SMC 的系列任務咯！",
				level);
#else
		len = snprintf(logic_buf, sizeof logic_buf,
				"恭喜你升级了，你现在的等级是 %d 级，已经可以获取 SMC 的系列任务咯！",
				level);
#endif
		break;
	default:
#ifdef TW_VER
		len = snprintf(logic_buf, sizeof logic_buf,
				"恭喜你升級了，你現在的等級是 %d 級，摩爾莊園還有更多精彩的內容等著你發現哦！",
				level);
#else
		len = snprintf(logic_buf, sizeof logic_buf,
				"恭喜你升级了，你现在的等级是 %d 级，摩尔庄园还有更多精彩的内容等着你发现哦！",
				level);
#endif
		break;
	}

	notify_system_message(p, logic_buf, len);
	return 0; // returns 0 means the `timer scanner` should remove this event
}

int del_mul_friends_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t friend_cnt;
	int i = 0;
	UNPKG_UINT32(body, friend_cnt, i);

	if (friend_cnt == 0 || friend_cnt > 20) {
		return send_to_self_error(p, p->waitcmd, -ERR_del_mul_friend_error, 1);
	}

	CHECK_BODY_LEN(len, friend_cnt * 4 + 4);

	uint32_t db_buff[32];
	uint32_t uid;
	int idx;
	int cnt = 1;
	for (idx = 0; idx < friend_cnt; idx++) {
		UNPKG_UINT32(body, uid, i);
		CHECK_VALID_ID(uid);
		if (uid == p->id) {
			return send_to_self_error(p, p->waitcmd, -ERR_del_mul_friend_error, 1);
		}

		int j = 1;
		for (j = 1; j < cnt && db_buff[j] != uid; j++);
		if (j == cnt) {	//判断是否有重复的
			db_buff[cnt++] = uid;
		} else {
			return send_to_self_error(p, p->waitcmd, -ERR_del_mul_friend_error, 1);
		}
	}
	db_buff[0] = cnt - 1;

	return send_request_to_db(SVR_PROTO_DEL_MUL_FRIENDS, p, cnt * 4, db_buff, p->id);
}

int del_mul_friends_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t state;
		uint32_t left_cnt;
	}__attribute__((packed)) del_mul_friends_cbk_pack_t;

	CHECK_BODY_LEN(len, sizeof(del_mul_friends_cbk_pack_t));
	del_mul_friends_cbk_pack_t* cbk_pack = (del_mul_friends_cbk_pack_t*)buf;

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->state, l);
	PKG_UINT32(msg, cbk_pack->left_cnt, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}
