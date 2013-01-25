#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <glib.h>

#include "npc.h"
#include "sprite.h"
#include "center.h"
#include "battle.h"
#include "items.h"
#include "sns.h"
#include "pet.h"
#include "global.h"
#include "homemap.h"
#include "task_new.h"
#include  <libtaomee++/utils/strings.hpp>

#include "maze.h"
#include "./proto/mole2_home.h"
#include "./proto/mole2_home_enum.h"

void traverse_sprites(void (*action)(void* key, void* spri, void* userdata), void* data)
{
	g_hash_table_foreach(all_sprites, action, data);
}

static inline void
do_free_sprite(void* p)
{
	uint32_t uid = ((sprite_t *)p)->id;
	g_slice_free1(SPRITE_STRUCT_LEN, p);
	--sprites_count;
	KDEBUG_LOG(uid, "FREE USER SUCC\t[%u]", sprites_count);
}

void init_sprites()
{
	all_sprites = g_hash_table_new(g_int_hash, g_int_equal);
	war_sprites = g_hash_table_new(g_int_hash, g_int_equal);
	sprites_count = 0;

	for (int i = 0; i < HASH_SLOT_NUM; i++)
		INIT_LIST_HEAD (&idslots[i]);
	INIT_LIST_HEAD (&g_busy_sprite_list);
}

static gboolean free_sprite_by_key(gpointer key, gpointer spr, gpointer userdata)
{
	do_free_sprite(spr);
	return TRUE;
}

void fini_sprites()
{
	KDEBUG_LOG(0, "FREE ALL SPRITE");
	g_hash_table_remove_all(war_sprites);
	g_hash_table_destroy(war_sprites);
	g_hash_table_foreach_remove(all_sprites, free_sprite_by_key, 0);
	g_hash_table_destroy(all_sprites);
	//destroy_npc_system();
}

void init_exp_lv()
{
	int loop;
	for (loop = 0; loop <= MAX_SPRITE_LEVEL; loop ++) {
		exp_lv[loop] = loop * loop * loop * loop;
	}
	exp_lv[1] = 0;
}

uint32_t get_lv_exp(uint32_t level)
{
	if (level >= MAX_SPRITE_LEVEL) {
		KERROR_LOG(0, "why this mole so niu XXXX\t[%u]", level);
		level = MAX_SPRITE_LEVEL;
	}
	return exp_lv[level];
}

int get_injury_effect(uint32_t injury_lv) 
{
	injury_lv = injury_lv > INJURY_RED ? INJURY_RED : injury_lv;
	return injury_effect_ratio[injury_lv];
}

sprite_t *get_sprite (uint32_t id)
{
	sprite_t *p;
	list_for_each_entry (p, &idslots[id % HASH_SLOT_NUM], hash_list) {
		if (p->id == id)
			return p;
	}
	return NULL;
}

sprite_t* get_sprite_by_fd(int fd)
{
	sprite_t* p = (sprite_t *)g_hash_table_lookup(all_sprites, &fd);
	if ( !p || IS_BEAST_ID(p->id) ) {
		return 0;
	}
	return p;
}

static inline void
free_sprite(sprite_t *p, int del_node)
{
	if (del_node) {
		int fd = p->fd;
		KDEBUG_LOG(p->id, "FREE WITH REMOVE\t[%d]", fd);
		delete p->trade_item_info;
		delete p->trade_cloth_info;
		delete p->trade_record_info;
		delete p->pets_inhouse;
		//delete p->item_cnt_map;
		g_hash_table_remove(all_sprites, &p->fd);
		do_free_sprite(p);
	}
}

static inline sprite_t*
alloc_sprite(int fd)
{
	sprite_t* p = (sprite_t *)g_slice_alloc(SPRITE_STRUCT_LEN);
	KDEBUG_LOG(0, "ALLOC SPRITE\t[%u]", fd);

	p->fd = fd;
	g_hash_table_insert(all_sprites, &(p->fd), p);
	++sprites_count;

	return p;
}

static inline void
send_login_exit_info(uint32_t uid, int login_flag, uint32_t online_id, uint32_t login_time)
{
	uint32_t buff_tmp[3];

	buff_tmp[0] = login_flag;
	buff_tmp[1] = online_id;
	buff_tmp[2] = login_time;

	send_udp_login_report_to_db(NULL, uid, proto_db_login_report, buff_tmp, sizeof buff_tmp);
}

sprite_t* add_sprite(sprite_t* v)
{
	sprite_t* p = alloc_sprite(v->fd);
	*p = *v;

	p->login_time = now.tv_sec;
	INIT_LIST_HEAD(&p->hash_list);
	INIT_LIST_HEAD(&p->map_list);
	INIT_LIST_HEAD(&p->timer_list);
	
	//make the hook point to it self, convenient to judge empty or not
	INIT_LIST_HEAD(&p->busy_sprite_list);
	//alloc a pending cmd queue head
	p->pending_cmd = g_queue_new(); // must succ, or program dies
	p->fintasks = g_hash_table_new(g_int_hash, g_int_equal);
	p->looptasks = g_hash_table_new(g_int_hash, g_int_equal);
	p->recvtasks = g_hash_table_new(g_int_hash, g_int_equal);
	p->bag_items = g_hash_table_new(g_int_hash, g_int_equal);
	p->beast_handbook = g_hash_table_new(g_int_hash, g_int_equal);
	p->map_infos = g_hash_table_new(g_int_hash, g_int_equal);
	p->day_limits = g_hash_table_new(g_int_hash, g_int_equal);
	p->user_honors = g_hash_table_new(g_int_hash, g_int_equal);
	p->fireworks = g_hash_table_new(g_int_hash, g_int_equal);
	p->all_cloths = g_hash_table_new(g_int_hash, g_int_equal);
	p->cloth_uplist = g_hash_table_new(g_int_hash, g_int_equal);
	p->cloth_timelist = g_hash_table_new(g_int_hash, g_int_equal);
	p->body_cloths = g_hash_table_new(g_int_hash, g_int_equal);
	p->trainning_start_time=get_now_sec();	
	p->trade_item_info=new std::multimap<uint32_t,trade_item_t>;
	p->trade_cloth_info=new std::multimap<uint32_t,trade_cloth_t>;
	p->trade_record_info=new std::multimap<uint32_t,trade_record_t>;
	p->pets_inhouse= new std::map<uint32_t, pet_simple_t>;
	//p->item_cnt_map=new std::map<uint32_t,uint32_t>;
	//p->seq=250;
	//p->gvg_wait_pos=-1;
	list_add_tail(&p->hash_list, &idslots[p->id % HASH_SLOT_NUM]);
	ADD_ONLINE_TIMER(p, n_send_cli_keepalive_pkg, 0, 1);
	ADD_ONLINE_TIMER(p, n_update_cloth_duration, NULL, 600);

	notify_user_login(p, 1);
	send_login_exit_info(p->id, 70, get_server_id(), get_now_tv()->tv_sec);

	KDEBUG_LOG(p->id, "ADD SPRITE COUNT\t[%u]", sprites_count);
	return p;
}

static void free_cmd_queue_elem(gpointer data, gpointer user_data)
{
	cached_pkg_t* cpt = (cached_pkg_t*)data;
	if ( likely(data != NULL) ) {
		KDEBUG_LOG(0, "FREE CMD BUF\t[%u %u]", cpt->len, ntohl(((protocol_t *)cpt->pkg)->id));
		g_slice_free1(cpt->len, data);
	}
}

static gboolean free_fintask(gpointer key, gpointer ptask, gpointer userdata)
{
	g_slice_free1(sizeof(task_fin_t), ptask);
	return TRUE;
}

static gboolean free_looptask(gpointer key, gpointer ptask, gpointer userdata)
{
	g_slice_free1(sizeof(loop_db_t), ptask);
	return TRUE;
}


static gboolean free_recvtask(gpointer key, gpointer ptask, gpointer userdata)
{
	g_slice_free1(sizeof(task_db_t), ptask);
	return TRUE;
}


static gboolean free_bag_item(gpointer key, gpointer pitem, gpointer userdata)
{
	g_slice_free1(sizeof(item_t), pitem);
	return TRUE;
}

static gboolean free_beast_info(gpointer key, gpointer pbeast, gpointer userdata)
{
	g_slice_free1(sizeof(beast_kill_t), pbeast);
	return TRUE;
}

static gboolean free_map_info(gpointer key, gpointer pmap, gpointer userdata)
{
	g_slice_free1(sizeof(map_info_t), pmap);
	return TRUE;
}

static gboolean free_day_limits(gpointer key, gpointer pday, gpointer userdata)
{
	g_slice_free1(sizeof(item_t), pday);
	return TRUE;
}

void db_add_user_login_info(sprite_t* p)
{
	typedef struct login_info {
	   uint16_t channel_id;
	   char verify_info[32];
	   uint32_t gameid;
	   uint32_t login_time;
	   uint32_t logout_time;
	} __attribute__ ((packed)) login_info_t;

    login_info_t info;
	info.gameid = 7;

    unsigned char src[100];
    uint16_t channel_id = config_get_intval("channel_id", 0);
    uint32_t security_code = config_get_intval("security_code", 0);
    DEBUG_LOG("userid %d channel_id %d security_code %d", p->id, channel_id, security_code);

    info.channel_id = channel_id;
	info.login_time = p->login_time;
	info.logout_time = get_now_tv()->tv_sec;
    
	int len = snprintf((char*)src, sizeof src, "channelId=%d&securityCode=%u&data=", channel_id, security_code);
    *(uint32_t*)(src + len) = info.gameid; 
    *(uint32_t*)(src + len + 4) = p->fdsess->remote_ip; 
	utils::MD5 md5;

	md5.reset();
	md5.update ( (void*)src, len + 8 ); 
    
    memcpy(info.verify_info, md5.toString().c_str() , 32);
	send_request_to_db(NULL, p->id, proto_db_add_user_login_info, &info, sizeof(info));
}

int del_sprite(sprite_t *p, int del_node)
{
	KDEBUG_LOG(p->id, "BEGIN DEL USER\t[flg=0x%x fd=%d]", p->flag, p->fd);
	
	REMOVE_TIMERS(p);
	msg_log_user_off(p->id);
	msg_log_user_xiaomee(p->id, p->xiaomee);
	msg_log_online_time(get_now_tv()->tv_sec - p->login_time);
	monitor_sprite(p, "LOG OUT");
	
    if(p->pos_in_line !=-1){
		user_for_team[p->pos_in_line].userid = 0;
		user_for_team[p->pos_in_line].lv = 0;
		REMOVE_TIMERS(&(user_for_team[p->pos_in_line]));
    }
	if( p->watchid ){
		cancel_watch_ex(p);
	}
	//记录排行榜任务的持续时间
	if(cache_get_task_info(p, TRAIN_TASK_ID) && p->trainning_start_time < get_now_sec()){
		uint32_t count=get_now_sec()-p->trainning_start_time;
		db_day_add_ssid_cnt(0,p->id,ssid_trainning_last_time,count,-1);	
	}

	/*
	if (p->is_in_war_map()&&p->pvp_pk_can_get_item==2 ){
		KDEBUG_LOG(p->id, "PVA FAIL del_sprite" );
		//DB.失败者失败次数+1
		db_day_add_ssid_cnt(NULL, p->id, ssid_pvp_pk_fail_times, 1, -1);
	}
	*/
//delete user's cache item when user log out  for func cli_train_race_skill
	if (p->item_in_use) {
	    cache_reduce_kind_item(p,p->item_in_use,1);
		db_add_item(0, p->id, p->item_in_use,-1);
	}
	uint32_t hangup_time=get_hangup_activity_time(p),add_time=0;
	add_time=hangup_time >= MAX_HANGUP_TIME?
		(MAX_HANGUP_TIME-p->hangup_activity_time):(hangup_time-p->hangup_activity_time);
	db_day_add_ssid_cnt(NULL, p->id, ssid_hangup_activity_time, add_time,MAX_HANGUP_TIME);
	//if(p->gvg_challenge_tag)
		//db_day_add_ssid_cnt(0, p->id, ssid_gvg_challenge_times, 1, MAX_GVG_CHALLENGE);
	noti_batsvr_user_offline(p);
	del_user_battle_team(p);
	map_id_t mapid = p->tiles ? p->tiles->id : 0;
	uint32_t x = 0, y = 0;
	if (IS_NORMAL_MAP(mapid)) {
		if (!p->tiles->retmap_cnt) {
			mapid = p->tiles->id; 
			x = p->posX;
			y = p->posY;
		} else {
			mapid = p->tiles->ret_maps[0].mapid; 
			x = p->tiles->ret_maps[0].des_x; 
			y = p->tiles->ret_maps[0].des_y;
		}
	} else if(IS_MAZE_MAP(mapid)){
		maze_t *mz = get_maze(p->tiles->maze_id);
		if(mz) {
			maze_map_t *map = get_maze_map(mz->layers[0].index);
			if(map) {
				mapid = mz->layers[0].mapid; 
				x = map->point[mz->layers[0].inext].x; 
				y = map->point[mz->layers[0].inext].y;
			}
		}
	} else if (IS_COPY_MAP(mapid)) {
		x = p->posX;
		y = p->posY;
	} else if (IS_WAR_MAP(mapid)) {
		mapid = 11218;
		x=680;
		y=280;
	} else if (IS_TRADE_MAP(mapid)) {
		mapid = 11205;
		x=360;
		y=308;
	}else {
		mapid = 11202;
		x = 337;
		y = 997;
	}

	if (p->seatid)
		hangup_uids[p->seatid - 1] = 0;
	
	db_add_user_login_info(p);

	db_set_user_map_info(0, p->id, get_now_tv()->tv_sec, mapid, x ,y, p->recv_map);
	uint32_t all_dexp_time = holidy_factor[get_now_tm()->tm_wday].timetype ? 3600*3 : 1800;
	if (p->double_exptime != 0xFFFFFFFF && p->double_exptime && p->double_exptime < all_dexp_time) {
		uint32_t dexp_time = get_now_tv()->tv_sec - p->start_dexp_time;
		if (dexp_time + p->double_exptime > all_dexp_time)
			dexp_time = all_dexp_time - p->double_exptime;
		db_day_add_ssid_cnt(NULL, p->id, item_id_double_exp, dexp_time, all_dexp_time);
	}

	if (test_bit_on32(p->update_flag, update_flag_hp_mp_bit)) {
		db_add_exp_level(p);
		//db_set_user_injury(0, p->id, p->injury_lv);
		for (int loop = 0; loop < p->pet_cnt; loop ++) {
			pet_t* p_p = &p->pets_inbag[loop];
			db_add_pet_exp_level(p, p_p);
			//db_set_pet_injury(NULL, p->id, p_p->petid, p_p->injury_lv);
		}
	}
	//幸运星活动挂机时间
	item_t* ptime = cache_get_day_limits(p, ssid_luckystar_hangup_time);
	db_day_set_ssid_cnt(NULL,p->id, ssid_luckystar_hangup_time,ptime->count,ptime->count);
	
	db_sync_cloth_duration(p);

	leave_map(p, 0);
	list_del(&p->hash_list);
	
	notify_user_login(p, 0);
	send_login_exit_info(p->id, 71, get_server_id(), get_now_tv()->tv_sec);
	
	if (!g_queue_is_empty(p->pending_cmd)) {  
		g_queue_foreach(p->pending_cmd, free_cmd_queue_elem, NULL);  
	}
	g_queue_free(p->pending_cmd);
	g_hash_table_foreach_remove(p->fintasks, free_fintask, 0);
	g_hash_table_destroy(p->fintasks);
	g_hash_table_foreach_remove(p->looptasks, free_looptask, 0);
	g_hash_table_destroy(p->looptasks);
	g_hash_table_foreach_remove(p->recvtasks, free_recvtask, 0);
	g_hash_table_destroy(p->recvtasks);
	g_hash_table_foreach_remove(p->bag_items, free_bag_item, 0);
	g_hash_table_destroy(p->bag_items);
	g_hash_table_foreach_remove(p->beast_handbook, free_beast_info, 0);
	g_hash_table_destroy(p->beast_handbook);
	g_hash_table_foreach_remove(p->map_infos, free_map_info, 0);
	g_hash_table_destroy(p->map_infos);
	g_hash_table_foreach_remove(p->day_limits, free_day_limits, 0);
	g_hash_table_destroy(p->day_limits);
	g_hash_table_foreach_remove(p->fireworks, free_fireworks, 0);
	g_hash_table_destroy(p->fireworks);
	g_hash_table_remove_all(p->user_honors);
	g_hash_table_destroy(p->user_honors);
	g_hash_table_remove_all(p->body_cloths);
	g_hash_table_destroy(p->body_cloths);
	g_hash_table_remove_all(p->cloth_timelist);
	g_hash_table_destroy(p->cloth_timelist);
	g_hash_table_destroy(p->cloth_uplist);
	g_hash_table_foreach_remove(p->all_cloths, free_body_cloth, 0);
	g_hash_table_destroy(p->all_cloths);
	list_del_init(&p->busy_sprite_list);
	g_hash_table_remove(war_sprites, &p->id);
	free_sprite(p, del_node);
	return 0;
}

void calc_sprite_second_level_attr(sprite_t* p)
{
	int effect_ratio = 10; //get_injury_effect(p->injury_lv);
	p->hp_max 	= (8 * p->physique + 2 * p->strength + 3 * p->endurance + 3 * p->quick + p->intelligence) * effect_ratio / 10 + 20;
	p->mp_max 	= (p->physique + 2 * p->strength + 2 * p->endurance + 2 * p->quick + 10* p->intelligence) * effect_ratio / 10 + 20;
	p->attack	= (p->physique + 20 * p->strength + 2 * p->endurance + 2 * p->quick + p->intelligence) * effect_ratio / 10 / 10 + 20;
	p->defense 	= (p->physique + 2 * p->strength + 20 * p->endurance + 2 * p->quick + p->intelligence) * effect_ratio / 10 / 10 + 20;
	p->speed 	= (p->physique + 2 * p->strength + 2 * p->endurance + 20 * p->quick + p->intelligence) * effect_ratio / 10 / 10 + 20;
	p->spirit 	= (-3 * p->physique - p->strength + 2 * p->endurance - p->quick + 8 * p->intelligence) / 10 + 100;
	p->resume	= (8 * p->physique - p->strength - p->endurance + 2 * p->quick - 3 * p->intelligence) / 10 + 100;

	p->spirit = (p->spirit > 0) ? p->spirit : 1;
	p->resume = (p->resume > 0) ? p->resume : 1;

	p->hit_rate = 0;
	p->avoid_rate = 0;
	p->mattack = 0;
	p->mdefense = 0;
	p->fight_back = 0;
	p->bisha = 0;
	p->rpoison = 0;
	p->rconfusion = 0;
	p->rinebriation = 0;
	p->rlethargy = 0;
	p->rlithification = 0;
	p->roblivion = 0;

	p->hp_max *= 5;
//生命、攻击、防御、速度运算值:属性点换算
	p->attack_for_cal = p->attack;
	p->defense_for_cal = p->defense;
	p->speed_for_cal = p->speed;
	//KDEBUG_LOG(p->id,"monitor_sprite_attr:%u %u %u %u %u %u"
			//,p->attack,p->defense,p->speed,p->attack_for_cal,p->defense_for_cal,p->speed_for_cal);
}

void calc_base_second_attr(base_second_attr_t* p, base_5_attr_t* p_b5a, uint32_t injury_lv)
{
	DEBUG_LOG("calc_base_second_attr");
	int effect_ratio = 10; //get_injury_effect(injury_lv);
	p->hp_max 	= (8 * p_b5a->physique + 2 * p_b5a->strength + 3 * p_b5a->endurance + 3 * p_b5a->quick + p_b5a->intelligence) * effect_ratio / 10 + 20;
	p->mp_max 	= (p_b5a->physique + 2 * p_b5a->strength + 2 * p_b5a->endurance + 2 * p_b5a->quick + 10* p_b5a->intelligence) * effect_ratio / 10 + 20;
	p->attack	= (p_b5a->physique + 20 * p_b5a->strength + 2 * p_b5a->endurance + 2 * p_b5a->quick + p_b5a->intelligence) * effect_ratio / 10 / 10 + 20;
	p->defense 	= (p_b5a->physique + 2 * p_b5a->strength + 20 * p_b5a->endurance + 2 * p_b5a->quick + p_b5a->intelligence) * effect_ratio / 10 / 10 + 20;
	p->speed 	= (p_b5a->physique + 2 * p_b5a->strength + 2 * p_b5a->endurance + 20 * p_b5a->quick + p_b5a->intelligence) * effect_ratio / 10 / 10 + 20;
	p->spirit 	= (-3 * p_b5a->physique - p_b5a->strength + 2 * p_b5a->endurance - p_b5a->quick + 8 * p_b5a->intelligence) / 10 + 100;
	p->resume	= (8 * p_b5a->physique - p_b5a->strength - p_b5a->endurance + 2 * p_b5a->quick - 3 * p_b5a->intelligence) / 10 + 100;
	p->hp_max = p->hp_max*5;
	p->spirit = (p->spirit > 0) ? p->spirit : 1;
	p->resume = (p->resume > 0) ? p->resume : 1;

	p->hit_rate = 0;
	p->avoid_rate = 0;

}

void update_base_second_attr(base_second_attr_t* p, base_5_attr_t* p_b5a_new, base_5_attr_t* p_b5a_old, uint32_t injury_lv_new, uint32_t injury_lv_old)
{
	DEBUG_LOG("update_base_second_attr");
	base_second_attr_t old_attr, new_attr;
	calc_base_second_attr(&old_attr, p_b5a_old, injury_lv_old);
	calc_base_second_attr(&new_attr, p_b5a_new, injury_lv_new);
	
	p->hp_max 		+= new_attr.hp_max - old_attr.hp_max;
	p->mp_max 		+= new_attr.mp_max - old_attr.mp_max;
	p->attack		+= new_attr.attack - old_attr.attack;
	p->defense	 	+= new_attr.defense - old_attr.defense;
	p->speed 		+= new_attr.speed - old_attr.speed;
	p->spirit 		+= new_attr.spirit - old_attr.spirit;
	p->resume		+= new_attr.resume - old_attr.resume;
	p->hit_rate		+= new_attr.hit_rate - old_attr.hit_rate;
	p->avoid_rate   += new_attr.avoid_rate - old_attr.avoid_rate;

	p->spirit = (p->spirit > 0) ? p->spirit : 1;
	p->resume = (p->resume > 0) ? p->resume : 1;

}


void update_user_second_attr(sprite_t* p, base_5_attr_t* p_b5a_new, uint32_t injury_lv)
{
	DEBUG_LOG("update_user_second_attr");
	base_second_attr_t old_attr, new_attr;
	calc_base_second_attr(&old_attr, (base_5_attr_t *)&p->physique, p->injury_lv);
	calc_base_second_attr(&new_attr, p_b5a_new, injury_lv);

	p->physique = p_b5a_new->physique;
	p->strength = p_b5a_new->strength;
	p->endurance = p_b5a_new->endurance;
	p->quick = p_b5a_new->quick;
	p->intelligence = p_b5a_new->intelligence;
	p->injury_lv = injury_lv;
	
	p->hp_max 		+= new_attr.hp_max - old_attr.hp_max;
	p->mp_max 		+= new_attr.mp_max - old_attr.mp_max;
	p->attack 		+= new_attr.attack - old_attr.attack;
	p->defense 		+= new_attr.defense - old_attr.defense;
	p->speed 		+= new_attr.speed - old_attr.speed;
	p->spirit 		+= new_attr.spirit - old_attr.spirit;
	p->resume		+= new_attr.resume - old_attr.resume;
	p->hit_rate		+= new_attr.hit_rate - old_attr.hit_rate;
	p->avoid_rate 	+= new_attr.avoid_rate - old_attr.avoid_rate;

	p->attack_for_cal += new_attr.attack - old_attr.attack;
	p->defense_for_cal += new_attr.defense - old_attr.defense;
	p->speed_for_cal += new_attr.speed - old_attr.speed;

	p->spirit = (p->spirit > 0) ? p->spirit : 1;
	p->resume = (p->resume > 0) ? p->resume : 1;

	p->hp = (p->hp > p->hp_max) ? p->hp_max : p->hp;
	p->mp = (p->mp > p->mp_max) ? p->mp_max : p->mp;
}

void add_second_attr(sprite_t* p, base_attr_t* p_sa,uint32_t type)
{
	p->hp_max += p_sa->hp_max;
	p->mp_max += p_sa->mp_max;
	p->attack += p_sa->attack;
	p->mattack += p_sa->mattack;
	p->defense += p_sa->defense;
	p->mdefense += p_sa->mdefense;
	p->speed += p_sa->speed;
	p->spirit += p_sa->spirit;
	p->resume += p_sa->resume;
	p->hit_rate += p_sa->hit;
	p->avoid_rate += p_sa->dodge;
	p->bisha += p_sa->crit;
	p->fight_back += p_sa->fightback;
	p->rpoison += p_sa->rpoison;
	p->rlithification += p_sa->rlithification;
	p->rlethargy += p_sa->rlethargy;
	p->rinebriation += p_sa->rinebriation;
	p->rconfusion += p_sa->rconfusion;
	p->roblivion += p_sa->roblivion;

	KDEBUG_LOG(p->id,"monitor_sprite_add_attr:hp_max=%u mp_max=%u attack=%u mattack=%u defense=%u mdefense=%u speed=%u spirit=%u resume=%u hit=%u  dodge=%u crit=%u fightback=%u rpoison=%u rlithification=%u rlethargy=%u rinebriation=%u rconfusion=%u roblivion=%u"
, p_sa->hp_max , p_sa->mp_max , p_sa->attack , p_sa->mattack , p_sa->defense , p_sa->mdefense , p_sa->speed , p_sa->spirit
	, p_sa->resume , p_sa->hit , p_sa->dodge , p_sa->crit , p_sa->fightback , p_sa->rpoison , p_sa->rlithification
	, p_sa->rlethargy , p_sa->rinebriation , p_sa->rconfusion , p_sa->roblivion);


	p->attack_for_cal += p_sa->attack/5;
	p->defense_for_cal += p_sa->defense/5;
	p->speed_for_cal += p_sa->speed/5;
	//KDEBUG_LOG(p->id,"monitor_sprite_add_attr:%u %u %u %u %u %u %u %u %u"
			//,p_sa->attack,p_sa->defense,p_sa->speed	
			//,p->attack,p->defense,p->speed,p->attack_for_cal,p->defense_for_cal,p->speed_for_cal);
}

void reduce_second_attr(sprite_t* p, base_attr_t* p_sa, uint32_t type)
{
	p->hp_max -= p_sa->hp_max;
	p->mp_max -= p_sa->mp_max;
	p->attack -= p_sa->attack;
	p->mattack -= p_sa->mattack;
	p->defense -= p_sa->defense;
	p->mdefense -= p_sa->mdefense;
	p->speed -= p_sa->speed;
	p->spirit -= p_sa->spirit;
	p->resume -= p_sa->resume;
	p->hit_rate -= p_sa->hit;
	p->avoid_rate -= p_sa->dodge;
	p->bisha -= p_sa->crit;
	p->fight_back -= p_sa->fightback;
	p->rpoison -= p_sa->rpoison;
	p->rlithification -= p_sa->rlithification;
	p->rlethargy -= p_sa->rlethargy;
	p->rinebriation -= p_sa->rinebriation;
	p->rconfusion -= p_sa->rconfusion;
	p->roblivion -= p_sa->roblivion;

	p->attack_for_cal -= p_sa->attack/5;
	p->defense_for_cal -= p_sa->defense/5;
	p->speed_for_cal -= p_sa->speed/5;
	KDEBUG_LOG(p->id,"monitor_sprite_reduce_attr:hp_max=%u mp_max=%u attack=%u mattack=%u defense=%u mdefense=%u speed=%u spirit=%u resume=%u hit=%u  dodge=%u crit=%u fightback=%u rpoison=%u rlithification=%u rlethargy=%u rinebriation=%u rconfusion=%u roblivion=%u"
, p_sa->hp_max , p_sa->mp_max , p_sa->attack , p_sa->mattack , p_sa->defense , p_sa->mdefense , p_sa->speed , p_sa->spirit
	, p_sa->resume , p_sa->hit , p_sa->dodge , p_sa->crit , p_sa->fightback , p_sa->rpoison , p_sa->rlithification
	, p_sa->rlethargy , p_sa->rinebriation , p_sa->rconfusion , p_sa->roblivion);


	//KDEBUG_LOG(p->id,"monitor_sprite_reduce_attr:%u %u %u %u %u %u"
			//,p->attack,p->defense,p->speed,p->attack_for_cal,p->defense_for_cal,p->speed_for_cal);
}

int pkg_body_clothes_simple(const sprite_t* p, uint8_t *buf)
{
	int j = 0;
	PKG_UINT8(buf, g_hash_table_size(p->body_cloths), j);

	GList* pcloths = g_hash_table_get_values(p->body_cloths);
	GList* head = pcloths;
	while (pcloths) {
		body_cloth_t* pc = (body_cloth_t *)pcloths->data;
		PKG_UINT32 (buf, pc->clothid, j);
		PKG_UINT32 (buf, pc->clothtype, j);
		PKG_UINT16 (buf, pc->clothlv, j);
		pcloths = pcloths->next;
	}
	g_list_free(head);
	
	return j;
}

int pkg_body_clothes_detail(const sprite_t* p, uint8_t *buf)
{
	int j = 0;
	PKG_UINT8(buf, g_hash_table_size(p->body_cloths), j);

	GList* pcloths = g_hash_table_get_values(p->body_cloths);
	GList* head = pcloths;
	while (pcloths) {
		body_cloth_t* pc = (body_cloth_t *)pcloths->data;
		j += pkg_cloth(pc, buf + j);
		pcloths = pcloths->next;
	}
	g_list_free(head);
	
	return j;
}


int pack_activity_info(const sprite_t* p, uint8_t* buf)
{
	int i = 0;
	PKG_UINT32(buf, p->login_day, i);
	return 32;
}

/*
	affect proto: 1001 1002 1004 1005,  
	related func: pkg_sprite_ol_simply pkg_home_sprite_info , change together with them
*/
int pkg_sprite_simply(const sprite_t *p, uint8_t *buf)
{
	int i = 0;

	PKG_UINT32(buf, p->id, i);
	PKG_STR(buf, p->nick, i, 16);
	PKG_UINT32(buf, p->flag, i);
	PKG_UINT32(buf, p->viplv, i);
	PKG_UINT32(buf, p->vipexp, i);
	PKG_UINT32(buf, p->vip_begin, i);
	PKG_UINT32(buf,	p->vip_end, i);
	PKG_UINT32(buf, p->hero_team, i);
	PKG_UINT32(buf, p->color, i);
	PKG_UINT8(buf, p->race, i);
	PKG_UINT8(buf, p->prof, i);
	PKG_UINT32(buf, p->prof_lv, i);
	PKG_UINT32(buf, p->honor, i);
	PKG_UINT32(buf, p->pkpoint_total, i);
	PKG_UINT32(buf, p->level, i);
	PKG_UINT32(buf, p->hp, i);
	PKG_UINT32(buf, p->mp, i);
	PKG_UINT32(buf, p->hp_max, i);
	PKG_UINT32(buf, p->mp_max, i);
	PKG_UINT32(buf, p->injury_lv, i);

	PKG_UINT32(buf, p->shapeshifting.pettype, i);
	//
	PKG_MAP_ID(buf, (p->tiles ? p->tiles->id : 0), i);
	PKG_UINT32(buf, p->posX, i);
	PKG_UINT32(buf, p->posY, i);
	PKG_UINT32(buf, p->base_action, i);
	PKG_UINT32(buf, p->advance_action, i);
	PKG_UINT8(buf, p->direction, i);
	if (p->btr_team) {
		PKG_UINT32(buf, p->btr_team->teamid, i);
		PKG_UINT32(buf, get_user_in_bt(p->btr_team, p->id), i);
		PKG_UINT32(buf, p->btr_team->players[0]->team_registerd, i);
		PKG_STR(buf, p->btr_team->players[0]->team_nick, i, TEAM_NICK_LEN);
	} else {
		PKG_UINT32(buf, 0, i);
		PKG_UINT32(buf, 0, i);
		PKG_UINT32(buf, p->team_registerd, i);
		PKG_STR(buf, p->team_nick, i, TEAM_NICK_LEN);
	}
	PKG_UINT8(buf, team_switch_state(p), i);
	PKG_UINT8(buf, p->pk_switch, i);
	PKG_UINT8(buf, p->busy_state, i);
	i += pack_activity_info(p, buf + i);
	i += pkg_body_clothes_simple(p, buf + i);
	i += pkg_pet_follow(p->p_pet_follow, buf + i);
	
	return i;
}

//called by 1005
int pkg_sprite_detail(const sprite_t* p, uint8_t* buf, int self)
{
	int i = 0;

	PKG_UINT32(buf, p->id, i);
	PKG_STR(buf, p->nick, i, 16);
	PKG_UINT32(buf, p->flag, i);
	PKG_UINT32(buf, p->viplv, i);
	PKG_UINT32(buf, p->vipexp, i);
	PKG_UINT32(buf, p->vip_begin, i);
	PKG_UINT32(buf, p->vip_end, i);
	PKG_UINT32(buf, p->hero_team, i);
	PKG_UINT32(buf, p->color, i);
	PKG_UINT32(buf, p->register_time, i);
	PKG_UINT8(buf, p->race, i);
	PKG_UINT8(buf, p->prof, i);
	PKG_UINT32(buf, p->prof_lv, i);
	PKG_UINT32(buf, p->honor, i);
	PKG_UINT32(buf, p->xiaomee, i);
	PKG_UINT32(buf, IS_ADMIN(p) ? 1203 : p->pkpoint_total, i);
	PKG_UINT32(buf, p->energy, i);
	PKG_UINT32(buf, p->level, i);
	PKG_UINT32(buf, p->experience, i);
	PKG_UINT16(buf, p->physique, i);
	PKG_UINT16(buf, p->strength, i);
	PKG_UINT16(buf, p->endurance, i);
	PKG_UINT16(buf, p->quick, i);
	PKG_UINT16(buf, p->intelligence, i);
	PKG_UINT16(buf, p->attr_addition, i);
	PKG_UINT32(buf, p->hp, i);
	PKG_UINT32(buf, p->mp, i);
	if (self) {
		PKG_UINT8(buf, p->earth, i);
		PKG_UINT8(buf, p->water, i);
		PKG_UINT8(buf, p->fire, i);
		PKG_UINT8(buf, p->wind, i);
	} else {
		PKG_UINT8(buf, 0, i);
		PKG_UINT8(buf, 0, i);
		PKG_UINT8(buf, 0, i);
		PKG_UINT8(buf, 0, i);
	}
	
	PKG_UINT32(buf, p->injury_lv, i);
	//shape
	PKG_UINT32(buf, p->shapeshifting.pettype, i);

	PKG_MAP_ID(buf, (p->tiles ? p->tiles->id : 0), i);
	PKG_UINT32(buf, p->posX, i);
	PKG_UINT32(buf, p->posY, i);
	PKG_UINT32(buf, p->base_action, i);
	PKG_UINT32(buf, p->advance_action, i);
	PKG_UINT8(buf, p->direction, i);
	PKG_UINT8(buf, p->in_front, i);
	PKG_UINT32(buf, (p->btr_team ? p->btr_team->teamid: 0), i);
	PKG_UINT32(buf, (p->btr_team ? get_user_in_bt(p->btr_team, p->id) : 0), i);

	PKG_UINT32(buf, p->hp_max, i);
	PKG_UINT32(buf, p->mp_max, i);
	PKG_UINT16(buf, p->attack, i);
	PKG_UINT16(buf, p->defense, i);
	PKG_UINT16(buf, p->mdefense, i);
	PKG_UINT16(buf, p->speed, i);
	PKG_UINT16(buf, p->spirit, i);
	PKG_UINT16(buf, p->resume, i);
	PKG_UINT16(buf, p->hit_rate < 0 ? 0 : p->hit_rate, i);
	PKG_UINT16(buf, p->avoid_rate < 0 ? 0 : p->avoid_rate, i);
	PKG_UINT16(buf, p->bisha < 0 ? 0 : p->bisha, i);
	PKG_UINT16(buf, p->fight_back < 0 ? 0 : p->fight_back, i);
	PKG_UINT16(buf, p->rpoison, i);
	PKG_UINT16(buf, p->rlithification, i);
	PKG_UINT16(buf, p->rlethargy, i);
	PKG_UINT16(buf, p->rinebriation, i);
	PKG_UINT16(buf, p->rconfusion, i);
	PKG_UINT16(buf, p->roblivion, i);
	
	i += pack_activity_info(p, buf + i);
	
	i += pkg_body_clothes_detail(p, buf + i);
	return i;
}
// called by 1015
int pkg_sprite_attr(const sprite_t* p, uint8_t* buf)
{
	int i = 0;

	PKG_STR(buf, p->nick, i, 16);
	PKG_UINT32(buf, p->flag, i);
	PKG_UINT32(buf, p->prof, i);
	PKG_UINT32(buf, p->color, i);
	PKG_UINT32(buf, p->register_time, i);
	PKG_UINT16(buf, p->physique, i);
	PKG_UINT16(buf, p->strength, i);
	PKG_UINT16(buf, p->endurance, i);
	PKG_UINT16(buf, p->quick, i);
	PKG_UINT16(buf, p->intelligence, i);
	PKG_UINT16(buf, p->attr_addition, i);
	PKG_UINT32(buf, p->hp, i);
	PKG_UINT32(buf, p->mp, i);
	PKG_UINT8(buf, p->earth, i);
	PKG_UINT8(buf, p->water, i);
	PKG_UINT8(buf, p->fire, i);
	PKG_UINT8(buf, p->wind, i);
	PKG_UINT32(buf, p->injury_lv, i);
	//
	PKG_UINT32(buf, p->shapeshifting.pettype, i);

	PKG_MAP_ID(buf, (p->tiles ? p->tiles->id : 0), i);
	PKG_UINT32(buf, p->posX, i);
	PKG_UINT32(buf, p->posY, i);

	PKG_UINT32(buf, p->hp_max, i);
	PKG_UINT32(buf, p->mp_max, i);

	PKG_UINT16(buf, p->attack, i);
	PKG_UINT16(buf, p->mattack, i);
	PKG_UINT16(buf, p->defense, i);
	PKG_UINT16(buf, p->mdefense, i);
	PKG_UINT16(buf, p->speed, i);
	PKG_UINT16(buf, p->spirit, i);
	PKG_UINT16(buf, p->resume, i);
	PKG_UINT16(buf, p->hit_rate < 0 ? 0 : p->hit_rate, i);
	PKG_UINT16(buf, p->avoid_rate < 0 ? 0 : p->avoid_rate, i);
	PKG_UINT16(buf, p->bisha < 0 ? 0 : p->bisha, i);
	PKG_UINT16(buf, p->fight_back < 0 ? 0 : p->fight_back, i);
	PKG_UINT16(buf, p->rpoison, i);
	PKG_UINT16(buf, p->rlithification, i);
	PKG_UINT16(buf, p->rlethargy, i);
	PKG_UINT16(buf, p->rinebriation, i);
	PKG_UINT16(buf, p->rconfusion, i);
	PKG_UINT16(buf, p->roblivion, i);
	
	return i;
}

void modify_sprite_second_attr(sprite_t* p)
{
	p->attack = p->attack < 1 ? 1 : p->attack;
	p->mattack = p->mattack < 1 ? 1 : p->mattack;
	p->defense = p->defense < 1 ? 1 : p->defense;
	p->mdefense = p->mdefense < 1 ? 1 : p->mdefense;
	p->speed = p->speed < 1 ? 1 : p->speed;
	p->spirit = p->spirit < 1 ? 1 : p->spirit;
	p->resume = p->resume < 1 ? 1 : p->resume;
	p->hit_rate = p->hit_rate < 0 ? 0 : p->hit_rate;
	p->avoid_rate = p->avoid_rate < 0 ? 0 : p->avoid_rate;
	p->bisha = p->bisha < 0 ? 0 : p->bisha;
	p->fight_back = p->fight_back < 0 ? 0 : p->fight_back;
	p->hp = p->hp > p->hp_max ? p->hp_max : p->hp;
	p->hp = p->hp ? p->hp : 1;
	p->mp = p->mp > p->mp_max ? p->mp_max : p->mp;
}

void send_varpkg(void* key, void* spri, void* userdata)
{
	var_pkg_t* vpkg = (var_pkg_t *)userdata;
	sprite_t* p = (sprite_t *)spri;

	if (vpkg->idle_time < 0) {
		send_to_self(p, (uint8_t *)vpkg->pkg, vpkg->pkg_len, 0);
	} else {
		const struct timeval* tv = get_now_tv();
		if ( (tv->tv_sec - p->last_sent_tm) >= vpkg->idle_time ) {
			send_to_self(p, (uint8_t *)vpkg->pkg, vpkg->pkg_len, 0);
		}
	}
}

void notify_user_exit(sprite_t* p, uint32_t reason)
{
	response_proto_uint32(p, proto_cli_kick_user_offline, reason, 0, 0);
}

static inline void do_kick_user(void* key, void* spri, void* userdata)
{
	sprite_t* p = (sprite_t*)spri;
	if (p) {	
		notify_user_exit(p, KICK_MIDNIGHT);

		uint32_t* fds = (uint32_t *)(msg + 2048);
		KDEBUG_LOG(p->id, "MIDNIGHT KICK USER OFFLINE\t[%u %u]", fds[0], p->fd);
		//close_client_conn(p->fd);
		
		fds[++ fds[0]] = p->fd;
	}
}

int send_cli_keepalive_pkg(void* owner, void* data)
{
	sprite_t* p = (sprite_t*)owner;
	if (p) {
		int i = sizeof(protocol_t);
		uint8_t out[128];
		i += pkg_ol_time(p, out + i);
		PKG_UINT32(out, get_now_tv()->tv_sec - p->login_time, i);
		init_proto_head(out, proto_cli_keep_sprite_alive, i);
		send_to_self(p, out, i, 0);

		ADD_ONLINE_TIMER(p, n_send_cli_keepalive_pkg, 0, 60);
	}
	return 0;
}


int kick_user_timeout(void* owner, void* data)
{
	sprite_t* p = (sprite_t*)owner;
	if (p) {
		notify_user_exit(p, KICK_TIMEOUT);
		KDEBUG_LOG(p->id, "TIMEOUT KICK USER OFFLINE\t[%u]", p->fd);
		close_client_conn(p->fd);
	}
	return 0;
}

int kick_all_users_offline(void* owner, void* data)
{
	uint32_t * fds = (uint32_t *)(msg + 2048);
	fds[0] = 0;
	traverse_sprites(do_kick_user, data);
	
	for (uint32_t loop = 0; loop < fds[0]; loop ++) {
		close_client_conn(fds[loop + 1]);
	}
	remove_all_across_svr_sprites();
	//send_to_central_online(COCMD_kick_all_offline, 0, 0, 0, 0);
	
	DEBUG_RETURN( ("KICK ALL USERS OFFLINE"), 0 );
}

int start_kick_all_users(void* owner, void* data)
{
	ADD_ONLINE_TIMER(&g_events, n_start_kick_all_users, 0, 86400);
	ADD_ONLINE_TIMER(&g_events, n_kick_all_users_offline, 0, 60);
	DEBUG_RETURN( ("START KICK ALL USERS"), 0 );
}

init_prof_info_t* get_init_prof_info_no_chk(uint32_t profid)
{
	return !profid || profid > MAX_PROF_CNT ? NULL : &init_prof_infos[profid - 1];
}

init_prof_info_t* get_init_prof_info(uint32_t profid)
{
	init_prof_info_t* pipi = get_init_prof_info_no_chk(profid);
	return pipi && pipi->profid ? pipi : NULL;
}

int load_init_prof_info(xmlNodePtr cur)
{
	memset(init_prof_infos, 0, sizeof(init_prof_infos));
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Prof")) {
			uint32_t profid;
			DECODE_XML_PROP_UINT32(profid, cur, "ID");
			CHECK_PROF_VALID(0, profid);
			init_prof_info_t* pipi = get_init_prof_info_no_chk(profid);
			if (!pipi || pipi->profid) {
				ERROR_RETURN(("invalid or duplicate prof\t[%u]", profid), -1);
			}
			pipi->profid = profid;
			DECODE_XML_PROP_UINT32(pipi->honorid, cur, "TitleID");
			DECODE_XML_PROP_UINT32(pipi->physique, cur, "Physique");
			DECODE_XML_PROP_UINT32(pipi->strength, cur, "Strength");
			DECODE_XML_PROP_UINT32(pipi->endurance, cur, "Endurance");
			DECODE_XML_PROP_UINT32(pipi->quick, cur, "Quick");
			DECODE_XML_PROP_UINT32(pipi->intelligence, cur, "Intelligence");
			pipi->cloth_cnt = decode_xml_prop_arr_int_default((int *)pipi->cloths, MAX_ITEMS_WITH_BODY, cur, "Clothes", 0);
			for (uint32_t loop = 0; loop < pipi->cloth_cnt; loop ++) {
				if (!get_cloth(pipi->cloths[loop])) {
					ERROR_RETURN(("invalid clothid\t[%u %u]", profid, pipi->cloths[loop]), -1);
				}
			}
			DECODE_XML_PROP_UINT32(pipi->clothpatch, cur, "ClothesPatch");
		}

		cur = cur->next;
	}
	return 0;
}


bool sprite::pk_switch_is_on( ){
		return this->pk_switch== pk_on || user_is_in_war_map(this ) ;
}


bool sprite::is_in_war_map( ){
		return user_is_in_war_map(this ) ;
}

uint32_t sprite::add_item(uint32_t itemid,uint32_t count )
{
	uint32_t real_add_count=0;
	real_add_count=cache_add_kind_item(this,itemid,count );
	db_add_item(NULL,this->id,itemid,real_add_count );
	return  real_add_count ;
}
