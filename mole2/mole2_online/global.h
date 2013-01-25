#ifndef MOLE2_GLOBAL_H
#define MOLE2_GLOBAL_H

#include "benchapi.h"
#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/log.h>
#include <netinet/in.h>
#ifdef __cplusplus
}
#endif

#define MAX_SPRITE_LEVEL	100
#define MAX_USER_LEVEL		65
#define MAX_PET_LEVEL		(MAX_USER_LEVEL+5)	
#define MAX_TEMP_EXP_EX		(MAX_USER_LEVEL+1) * (MAX_USER_LEVEL+1) * (MAX_USER_LEVEL+1) * (MAX_USER_LEVEL+1)
#define MAX_PET_EXP			(MAX_USER_LEVEL+6) * (MAX_USER_LEVEL+6) * (MAX_USER_LEVEL+6) * (MAX_USER_LEVEL+6)
#define	MAX_PROC_FUNCS		10000
#define MAX_AUTO_FIGHT_CNT  150//自动战斗次数

typedef int (*func_t)(sprite_t* p, uint8_t* pkg, uint32_t bodylen);
typedef struct cli_handle {
	func_t 		func;
	uint32_t	len;
	uint8_t		no_guest;
	uint8_t		cmp_method;
}cli_handle_t;

extern uint8_t msg[1<<20];
extern list_head_t g_busy_sprite_list;
extern cli_handle_t	cli_handles[MAX_PROC_FUNCS];

/**
  * @brief hold timer list
  */
typedef struct timer_head {
	list_head_t	timer_list;
}timer_head_t;

extern timer_head_t g_events;

extern char* statistic_logfile;
extern int idc_type;


#define DAYS_PER_WEEK			7
#define HOURS_PER_DAY			24

/**
  * @brief data type for handlers that handle protocol packages from dbproxy
  */
typedef int (*dbproto_func_t)(sprite_t* p, userid_t id, uint8_t* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief data type for handles that handle protocol packages from dbproxy
  */
typedef struct dbproto_handle {
	dbproto_func_t		func;
	uint32_t			len;
	uint8_t				cmp_method;
}dbproto_handle_t;

/*! save handles for handling protocol packages from dbproxy */
extern dbproto_handle_t db_handles[65536];
/**
  * @brief data type for handlers that handle protocol packages from codesvr
  */
typedef int (*code_func_t)(sprite_t* p, uint8_t* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief data type for handles that handle protocol packages from dbproxy
  */
typedef struct code_handle {
	code_func_t			func;
	uint32_t			len;
	uint8_t				cmp_method;
}code_handle_t;

extern int udp_report_fd;
extern struct sockaddr_in udp_report_addr;
extern int udp_post_fd;
extern struct sockaddr_in udp_post_addr;
extern int chat_svr_fd;
extern struct sockaddr_in udp_chat_svr_addr;


/*! save handles for handling protocol packages from dbproxy */
extern code_handle_t code_handles[65536];
extern code_handle_t spacetime_handles[65536];
extern code_handle_t mall_handles[65536];


extern GHashTable*	batter_teams;
extern uint32_t			batter_team_cnt;
extern GHashTable*	batter_infos;
extern uint32_t			batter_info_cnt;
typedef struct exp_factor {
	uint32_t	timetype;
	uint32_t	half_hour_ratio;
	uint32_t	two_hour_ratio;
	uint32_t	five_hour_ratio;
}__attribute__((packed))exp_factor_t;

extern exp_factor_t holidy_factor[DAYS_PER_WEEK];

#define MAX_ONE_REFRESH_GRP		10
#define MAX_ONE_REFRESH_MAP		10
typedef struct refresh_group {
	uint32_t			id;
	uint32_t			refresh_type;
	uint32_t			refresh_time;
	uint32_t			refresh_speed;
	uint8_t				timeout;
	uint32_t			day;
	uint32_t			start_time;
	uint32_t			end_time;
	uint32_t			disapear;
	uint32_t			count;
	uint8_t				grp_cnt;
	uint32_t			private_refresh_cnt;
	uint32_t			grpids[MAX_ONE_REFRESH_GRP];
	uint32_t			minlist[MAX_ONE_REFRESH_GRP];
	int					startx[MAX_ONE_REFRESH_GRP];
	int					starty[MAX_ONE_REFRESH_GRP];
	uint8_t				map_cnt;
	uint32_t			mapids[MAX_ONE_REFRESH_MAP];
	uint32_t			total_cnt;
	int					counts[MAX_ONE_REFRESH_GRP][MAX_ONE_REFRESH_MAP];
}__attribute__((packed)) refresh_group_t;

#define MAX_REFRESH_GROUP_CNT		10
extern uint32_t refresh_group_cnt;
extern refresh_group_t refresh_grps[MAX_REFRESH_GROUP_CNT];


#define MAX_DROP_ITEM_TYPE		10
#define MAX_MAPID_CNT  5

typedef struct beast_datk{
	uint32_t	beastid;
	uint16_t	double_atk;
}__attribute__((packed))beast_datk_t;

typedef struct beast_group {
	uint32_t			grpid;
	uint32_t			honor_id;
	uint32_t			gethonor;
	int32_t				limit_exp;
	uint32_t			allowteam;;
	uint8_t				count;
	base_attr_t			saset[BATTLE_POS_CNT];
	uint32_t			elite_type[BATTLE_POS_CNT];
	uint32_t			type;
	uint32_t			mapid;
	uint32_t			bossid;
	uint32_t			beasttype;
	uint32_t			repeatcnt;
	uint32_t			npcid;

	uint32_t			minlv;
	uint32_t			maxlv;

	uint32_t			for_fish;

	uint32_t			mincnt;
	uint32_t			maxcnt;
	uint8_t				idcnt;
	base_attr_t			idlist[BATTLE_POS_CNT];
	uint32_t			percents[BATTLE_POS_CNT];
	uint32_t			minlist[BATTLE_POS_CNT];
	uint32_t			maxlist[BATTLE_POS_CNT];

	uint16_t			cntlist[BATTLE_POS_CNT];
	int					posinfo[BATTLE_POS_CNT][BATTLE_POS_CNT];

	uint32_t			drop_item_cnt;
	uint32_t			drop_items[MAX_DROP_ITEM_TYPE];
	uint32_t			certain_drop_item;
	uint32_t			certain_drop_cnt;
	uint32_t			rates[MAX_DROP_ITEM_TYPE];
	uint32_t			counts[MAX_DROP_ITEM_TYPE];

	uint32_t			needitem;
	uint32_t			needcnt;

	uint32_t			time_valid[DAYS_PER_WEEK];
//记录双击怪物的信息
	beast_datk_t	datk_info[BATTLE_POS_CNT];
	uint8_t			beastcnt;
//返回给客户端的beastid
	uint32_t			typeids[BATTLE_POS_CNT];
	uint8_t				typeidcnt;
}__attribute__((packed)) beast_group_t;
extern GHashTable* beast_grp;

extern GHashTable* npcs;

#define MAX_SHOP_CNT			1210
typedef struct shop {
	uint32_t			shop_id;
	GHashTable*			item_list;
	GHashTable*			skill_list;
	GHashTable*			pet_skill_list;
	GHashTable*			item_map_list;
	GHashTable*			skill_map_list;
}__attribute__((packed)) shop_t;

extern shop_t shops[MAX_SHOP_CNT];

#define MAX_PRE_HONOR_NUM	4
#define MAX_HONOR_NUM		4000
typedef struct honor_attr {
	uint32_t		honor_id;
	uint32_t		prof;
	uint32_t		proflv;
	uint32_t		pre_honor_cnt;
	uint32_t		clear_type;
	uint32_t		pre_honor_id[MAX_PRE_HONOR_NUM];
	base_attr_t		attr;
}__attribute__((packed)) honor_attr_t;
extern honor_attr_t honors[MAX_HONOR_NUM];

#define MAX_CLOTH_LEVL_CNT	10
#define MAX_RAND_ITEM_TYPE	25
#define MAX_MATERIAL_CNT	10
#define PRODUCT_LV_CNT		4
#define MAX_PROF_CNT		10
#define MAX_RAND_ITEM_MAPID	4


typedef struct cloth_lvinfo {
	uint8_t 			valid;
	int					duration[2];

	int32_t				hp_max;
	int32_t				mp_max;
	int16_t				attack;
	int16_t				mattack;
	int16_t				defense;
	int16_t				mdefense;
	int16_t				speed;
	int16_t				spirit;
	int16_t				resume;
	int16_t				hit;
	int16_t				dodge;
	int16_t				crit;
	int16_t				fightback;
	int16_t				rpoison;
	int16_t				rlithification;
	int16_t				rlethargy;
	int16_t				rinebriation;
	int16_t				rconfusion;
	int16_t				roblivion;
	uint32_t			quality;
	uint32_t			validday;

	uint32_t			r_hp_max;
	uint32_t			r_mp_max;
	uint16_t			r_attack;
	uint16_t			r_mattack;
	uint16_t			r_defense;
	uint16_t			r_mdefense;
	uint16_t			r_speed;
	uint16_t			r_spirit;
	uint16_t			r_resume;
	uint16_t			r_hit;
	uint16_t			r_dodge;
	uint16_t			r_crit;
	uint16_t			r_fightback;
	uint16_t			r_rpoison;
	uint16_t			r_rlithification;
	uint16_t			r_rlethargy;
	uint16_t			r_rinebriation;
	uint16_t			r_rconfusion;
	uint16_t			r_roblivion;

	uint8_t				earth;
	uint8_t				water;
	uint8_t				fire;
	uint8_t				wind;

	uint32_t 			price;
	uint32_t			sell_price;
	uint32_t			repair_price;
	uint32_t			tradablity;
	uint32_t			vip_tradablity;
	uint32_t			del_forbid;
	uint16_t			vendibility;
}__attribute__((packed)) cloth_lvinfo_t;

typedef struct cloth {
	uint32_t			id;
	uint32_t			prof;
	uint32_t			proflv;
	uint32_t 			minlv;
	uint16_t			itemlv;
	uint32_t			lv;
	uint32_t			equip_part;
	uint32_t			weapon_type;
	uint32_t			honor_id;
	uint32_t			del_forbid;
	uint32_t			activatecnt;
	uint32_t			activateid[MAX_PROF_CNT];

	uint8_t				suitcnt;
	uint32_t			suitid[MAX_ITEMS_WITH_BODY];

	uint8_t				material_cnt;
	item_t				materials[MAX_MATERIAL_CNT];
	
	uint32_t			pro_rate[PRODUCT_LV_CNT];
	cloth_lvinfo_t		pro_attr[PRODUCT_LV_CNT];
	
	cloth_lvinfo_t		clothlvs[MAX_CLOTH_LEVL_CNT];
}__attribute__((packed))cloth_t;

#define CLOTH_LV_PHASE		21
extern uint32_t drop_clothes[CLOTH_LV_PHASE][100];
extern uint32_t cloth_drop_rate[CLOTH_LV_PHASE][PRODUCT_LV_CNT + 1];
#define CLOTH_QUALITY_CNT		6
extern uint32_t crystal_attr[attr_id_max][CLOTH_QUALITY_CNT][CLOTH_LV_PHASE];

#define MAX_GIFT_CNT		10

typedef struct medical_info {
	uint32_t			hp_up;
	uint32_t			mp_up;
}__attribute__((packed))medical_info_t;

typedef struct pet_info {
	uint32_t			beastid;
	uint32_t			beastlv;
	uint32_t			feed_type;
	uint32_t			feed_count;
	int					rand_growth;
	uint32_t			rand_diff;
}__attribute__((packed))pet_info_t;

typedef struct pet_catch {
	uint8_t				racecnt;
	int					races[MAX_RACE_TYPE];
	uint32_t			cardlv;
}__attribute__((packed))pet_catch_t;

typedef struct gift_info {
	uint8_t				itemcnt;
	uint32_t			itemids[MAX_GIFT_CNT];
	uint32_t			item_cnt[MAX_GIFT_CNT];
	uint8_t				clothcnt;	
	uint16_t			tag;
	uint32_t			cloth_type[MAX_GIFT_CNT];
	uint32_t			cloth_lv[MAX_GIFT_CNT];
}__attribute__((packed))gift_info_t;

#define RAND_GIFT_MAX_TYPE		12
#define RAND_PROF_GIFT_MAX_TYPE		20

typedef struct rand_gift {
	uint32_t			grp_cnt;
	gift_info_t			gifts[RAND_GIFT_MAX_TYPE];
	uint32_t			percents[RAND_GIFT_MAX_TYPE];
}__attribute__((packed)) rand_gift_t;

typedef struct rand_prof_gift {
	uint32_t			grp_cnt;
	gift_info_t			gifts[RAND_PROF_GIFT_MAX_TYPE];
	uint32_t			prof[RAND_PROF_GIFT_MAX_TYPE];
}__attribute__((packed)) rand_prof_gift_t;

typedef struct gift_spec {
	uint8_t			count;
	uint32_t		rates[MAX_RAND_ITEM_TYPE];
	uint32_t		ids[MAX_RAND_ITEM_TYPE];
	uint32_t		min[MAX_RAND_ITEM_TYPE];
	uint32_t		max[MAX_RAND_ITEM_TYPE];
	uint16_t		quantities;//quantities==1表示可以批量处理 
}__attribute__((packed))gift_spec_t;

typedef struct attr_info {
	int16_t			physique;
	int16_t			strength;
	int16_t			endurance;
	int16_t			quick;
	int16_t			intelligence;
	int16_t			attr_add;
}__attribute__((packed)) attr_info_t;

typedef struct broadcast_usage {
	uint32_t		taskid;
	uint32_t		itemid;
	uint32_t		grpid;
}__attribute__((packed)) broadcast_usage_t;

typedef struct race_skill_type_info {
	uint32_t        raceid;
	uint32_t		skillid[3];
	uint32_t		percent[3];
}__attribute__((packed)) race_skill_type_info_t;

#define MAX_RACE_SKILL_TYPE			3

typedef struct race_skill_useage {
	uint32_t        order[3];
	race_skill_type_info_t type_info[MAX_RACE_SKILL_TYPE];
}__attribute__((packed)) race_skill_useage_t;


typedef struct buff_factor {
	uint32_t		factor;
	uint32_t		count;
}__attribute__((packed)) buff_factor_t;

typedef struct auto_hpmp {
	uint32_t		hpfactor;
	uint32_t		mpfactor;
}__attribute__((packed)) auto_hpmp_t;

typedef struct comp_clothes {
	uint32_t			proc_rate[PRODUCT_LV_CNT];
	uint32_t			limit_lv[2];
}__attribute__((packed)) comp_clothes_t;

typedef struct shapeshifting_attr {
	uint32_t			pettype;
	uint32_t			race;
	base_attr_t		attr;
}__attribute__((packed)) shapeshifting_attr_t;


typedef struct normal_item {
	uint32_t			item_id;
	uint32_t			type;
	uint32_t			overlay_max;
	uint32_t			total_max;
	uint32_t			day_max;
	uint32_t			price;
	uint32_t			sale_price;
	uint32_t			tradablity;
	uint32_t			vip_tradablity;
	uint32_t			minlv;
	uint32_t			maxlv;
	uint32_t			del_forbid;
	uint32_t			function;
	uint32_t			target;
	uint32_t			for_fish;
	uint16_t			itemlv;
	uint16_t			vip_only;
	uint16_t			vendibility;//trade in the market
	union {
		medical_info_t		medinfo;
		pet_info_t			petinfo;
		pet_catch_t			petcatch;
		gift_info_t			giftinfo;
		rand_gift_t			randgift;
		rand_prof_gift_t	rand_prof_gift;
		uint32_t			handbook_id;
		gift_spec_t			specgift;
		uint32_t			color;
		attr_info_t			attrinfo;
		broadcast_usage_t	cast_usage;
		uint32_t			energy_factor;
		buff_factor_t		exp_factor;
		auto_hpmp_t			hpmp_facotr;
		uint32_t			auto_fight_cnt;
		race_skill_useage_t race_skill_info;
		uint32_t			rate;
		int32_t				duration;
		comp_clothes_t		comp_clothes_info;
		shapeshifting_attr_t 		shapeshifting_info;
		uint32_t 				add_times;
	};

	uint8_t				material_cnt;
	item_t				materials[MAX_MATERIAL_CNT];
}__attribute__((packed))normal_item_t;



#define MAX_EXCHANGE_ITEM_TYPE		10
#define MAX_EXCHANGE_INFO_CNT		20100
typedef struct exchange_info {
	uint32_t			id;
	uint32_t			minlv;
	uint32_t			day_limit;
	uint32_t			just_once;
	uint32_t			start_time;
	uint32_t			end_time;
	uint8_t				type;
	uint8_t				paycnt;
	item_t				payitems[MAX_EXCHANGE_ITEM_TYPE];
	uint8_t				getcnt;
	item_t				getitems[MAX_EXCHANGE_ITEM_TYPE];
	uint32_t			extents[MAX_EXCHANGE_ITEM_TYPE];
	uint32_t			limit_cnt;
}__attribute__((packed)) exchange_info_t;

typedef struct rand_item {
	uint32_t			item_id;
	uint32_t			item_cnt;
	uint32_t			get_rate;
}__attribute__((packed)) rand_item_t;

typedef struct rand_info {
	uint32_t		rand_id;
	uint8_t			rand_type;
	uint32_t		type_cnt;
	uint32_t		rates[MAX_RAND_ITEM_TYPE];
	rand_item_t		items[MAX_RAND_ITEM_TYPE];
	uint32_t		needitem;
	int				needcnt;
	uint16_t		speccnt;
	uint16_t		minlv;
	uint16_t		maxlv;
	int				max_cnt;
	uint32_t		repeattype;
	uint32_t		startday;
	uint8_t		mapcnt;
	uint32_t		mapid[MAX_RAND_ITEM_MAPID];
	uint32_t		beastgrp;
	uint32_t		redirectlv;
	uint32_t		redirectid;
}__attribute__((packed))rand_info_t;

extern GHashTable*	all_clothes;
extern GHashTable*	all_items;
extern GHashTable* rand_infos;
extern exchange_info_t exchange_infos[MAX_EXCHANGE_INFO_CNT];

typedef struct vip_item {
	uint32_t		vid;
	uint8_t			rand_type;
	uint32_t		type_cnt;
	uint32_t		rates[MAX_RAND_ITEM_TYPE];
	rand_item_t		items[MAX_RAND_ITEM_TYPE];
	uint16_t		minlv;
	uint16_t		maxlv;
	uint16_t		viplv;
	int				repeatcnt;
	uint32_t		repeattype;
	uint32_t		startday;
}__attribute__((packed))vip_item_t;
extern GHashTable* vip_items;

typedef struct {
	uint32_t		id;
	uint32_t		itemid;
	uint8_t			type;
	uint8_t			level;
	uint32_t		validday;
}__attribute__((packed))product_t;

extern GHashTable* all_products;

#define MAX_SUIT_NUM		100
typedef struct suit {
	uint32_t		sid;
	uint32_t		prof;
	uint32_t		clothcnt;
	uint32_t		clothids[MAX_ITEMS_WITH_BODY];
	base_attr_t		attr;
}__attribute__((packed)) suit_t;

extern suit_t suits[MAX_SUIT_NUM];

typedef struct drop_item {
	uint32_t			itemid;
	uint32_t			rate;
}drop_item_t;

typedef struct beast_skill {
	uint32_t			skillid;
	uint32_t			percent;
}beast_skill_t;

typedef struct beast_lvinfo {
	uint32_t			id;
	uint32_t			level;
	uint32_t			catchable;
	uint8_t				item_cnt;
	drop_item_t			items[MAX_DROP_ITEM_TYPE];
}beast_lvinfo_t;


#define MAX_SPEC_CATCH_TYPE		10
#include  <map>
typedef struct beast {
	uint32_t			id;
	uint8_t 			race;
	uint32_t			style;
	uint32_t			price;
	uint32_t			sell_price;
	uint32_t 			tradability;
	uint32_t			vip_tradability;
	uint32_t			handbooklv;
	uint32_t			skillbar;
	uint8_t				spec_ccnt;
	int					spec_catch[MAX_SPEC_CATCH_TYPE];
	
	char				nick[USER_NICK_LEN];
	uint16_t			physi_factor;
	uint16_t			stren_factor;
	uint16_t			endur_factor;
	uint16_t			quick_factor;
	uint16_t			intel_factor;
	uint8_t				earth;
	uint8_t				water;
	uint8_t				fire;
	uint8_t				wind;

	uint8_t				talent_skill_cnt;
	uint32_t			talent_skill_ids[MAX_SKILL_BAR];
	uint32_t			talent_skill_percent[MAX_SKILL_BAR];
	
	uint8_t				skill_cnt;
	uint8_t				skill_cnt_all;
	beast_skill_t		skills[MAX_SKILL_BAR];
	std::map<uint32_t ,beast_lvinfo_t> levels;
	//beast_lvinfo_t levels[MAX_SPRITE_LEVEL];
} beast_t;

extern beast_t beasts_attr[MAX_BEAST_TYPE];

extern uint32_t valid_beast_cnt[LOOP_GRP_CNT];
extern uint32_t valid_beasts[LOOP_GRP_CNT][MAX_BEAST_TYPE];

#define MAX_PET_EXCHANGE_CNT	20000
#define MAX_PET_EXCHANGE_ITEM	5
typedef struct pet_exchange {
	uint32_t			id;
	uint32_t			typecnt;
	uint32_t			types[MAX_PETS_PER_SPRITE];
	uint32_t			cnts[MAX_PETS_PER_SPRITE];
	uint32_t			lvs[MAX_PETS_PER_SPRITE];
	uint32_t			getcnt;
	uint32_t			gettypes[MAX_PETS_PER_SPRITE];
	uint32_t			getcnts[MAX_PETS_PER_SPRITE];
	uint32_t			itemcnt;
	item_t				items[MAX_PET_EXCHANGE_ITEM];
	uint32_t			raretype;
	uint32_t			can_mix;
}__attribute__((packed)) pet_exchange_t;

extern pet_exchange_t pet_exchanges[MAX_PET_EXCHANGE_CNT];
#define     MAX_PET_EX_EGG_CNT 4
#define     MAX_CONTAIN_EGG_CNT  30
typedef struct exchange_egg_info{
	uint32_t itemid;	
	uint32_t race;
}__attribute__((packed)) exchange_egg_info_t;

typedef struct pet_exchange_egg {
	uint32_t id;
	uint32_t rate;
	exchange_egg_info_t egg_info[MAX_CONTAIN_EGG_CNT];	
	uint32_t egg_rate[MAX_CONTAIN_EGG_CNT];
	uint32_t egg_cnt;
}__attribute__((packed)) pet_exchange_egg_t;

extern pet_exchange_egg_t exchange_egg[MAX_PET_EX_EGG_CNT]; 
#define MAP_HASH_SLOT_NUM			500
#define MAX_RETMAP_PER_MAP			5
#define MAX_ADJACENTMAP_PER_MAP		15
#define NORMAL_MAP_NUM					60000
#define MAX_TASK_BEAST_NUM			4
#define MAX_SPEC_BEAST_PER_MAP		4

typedef struct entry_pos {
	map_id_t	mapid;
	uint32_t	cur_x;
	uint32_t	cur_y;
	uint32_t	des_x;
	uint32_t	des_y;
	uint32_t	grpid;
}entry_pos_t;

typedef struct task_beast{
	uint32_t	grpid;
	uint32_t	task_cnt;
	uint32_t	tasks[MAX_TASK_BEAST_NUM];
	uint32_t	startns[MAX_TASK_BEAST_NUM];
	uint32_t	endns[MAX_TASK_BEAST_NUM];
	uint32_t	probability;
}__attribute__((packed))task_beast_t;

typedef struct spec_beast{
	uint32_t	grpid;
	uint32_t	probability;
	uint32_t	start_time;
	uint32_t	end_time;
	uint32_t	clothid;
}__attribute__((packed))spec_beast_t;

#define MAX_STATE_CNT		3
typedef struct map_state {
	uint32_t	id;
	uint32_t	state;
	uint32_t	to_state;
	int			trans[MAX_STATE_CNT][MAX_STATE_CNT];
}__attribute__((packed))map_state_t;

#define STATE_CNT_PER_MAP		5
#define MAX_BEAST_TYPE_PER_MAP	16

typedef struct elite_beast {
	uint32_t	beastid;
	uint32_t	count[2];
}__attribute__((packed)) elite_beast_t;

typedef struct map {
	map_id_t		id;
	uint8_t			flag;
	uint32_t		area;
	uint32_t		chocobo;
	uint32_t		recv_map;
	uint16_t		home_ret;
	uint32_t		transmap;
	uint32_t		transid;
	uint16_t		check;

	uint32_t		leaderlv;

	int				sprite_num;
	list_head_t		sprite_list_head;
	list_head_t		hash_list;
	list_head_t		timer_list;

	int 			min_beast_cnt;
	int 			max_beast_cnt;
	uint8_t			beastcnt;
	beast_lvinfo_t* apbl[MAX_BEAST_TYPE_PER_MAP];
	uint32_t		beastrate[MAX_BEAST_TYPE_PER_MAP];
	uint32_t		elite_cnt;
	elite_beast_t	ebeasts[MAX_BEAST_TYPE_PER_MAP];
	int				task_beast_cnt;
	task_beast_t	task_beasts[MAX_SPEC_BEAST_PER_MAP];
	int				spec_beast_cnt;
	spec_beast_t	spec_beasts[MAX_SPEC_BEAST_PER_MAP];
	uint8_t			maze_beastcnt;
	uint32_t		maze_beasts[MAX_BEAST_TYPE_PER_MAP];
	uint32_t		maze_beastrates[MAX_BEAST_TYPE_PER_MAP];
	uint32_t		base_step;
	uint32_t		rand_step;
	uint32_t		maze_id;
	uint32_t		shop_id;

	int 			retmap_cnt;
	entry_pos_t		ret_maps[MAX_RETMAP_PER_MAP];

	int				adjacent_cnt;
	entry_pos_t		adjacent_maps[MAX_ADJACENTMAP_PER_MAP];	
	uint32_t		tasks[2];
	uint32_t		nodes[2];
	uint32_t		fin_task;
	uint32_t		fin_task_node;
	uint32_t		on_task;
	uint32_t		on_task_node;

	uint8_t			state_cnt;
	uint8_t			team;
	map_state_t		states[STATE_CNT_PER_MAP];
} map_t;

extern list_head_t map_hash_slots[MAP_HASH_SLOT_NUM];
extern map_t normal_map[NORMAL_MAP_NUM];

#define MAX_SKILL_PHASE			6
#define MAX_SKILL_PROFS			10	
#define MAX_SKILL_LEVEL			10

typedef struct skill_prof {
	uint32_t		prof_id;
	uint32_t		add_exp;
	int				phaselv[MAX_SKILL_PHASE];
}__attribute__((packed))skill_prof_t;

typedef struct skill_info {
	uint32_t		skill_id;
	uint32_t		professtion;
	uint32_t		rolelv;
	uint32_t		price;
	uint8_t			studyable;
	uint32_t		base_exp;
	skill_prof_t	skill_prof[MAX_SKILL_PROFS];
}__attribute__((packed))skill_info_t;

typedef struct pet_skill_p {
	uint8_t			skill_lv;
	uint64_t		skill_key;
	uint32_t		pet_lv;
	uint32_t		price;
	uint8_t			studyable;
}__attribute__((packed))pet_skill_p_t;

typedef struct pet_skill_info {
	uint32_t		skill_id;
	uint8_t			skill_lv_cnt;
	pet_skill_p_t	psp[MAX_SKILL_LEVEL];
}__attribute__((packed))pet_skill_info_t;

extern GHashTable* sprite_skills;
extern GHashTable* pet_skills;


#define HASH_SLOT_NUM	200

extern GHashTable*	all_sprites;
extern GHashTable*	war_sprites;

extern list_head_t	idslots[HASH_SLOT_NUM];
extern uint32_t	sprites_count;
extern int injury_effect_ratio[4];
extern uint32_t exp_lv[MAX_SPRITE_LEVEL + 1];
extern uint32_t exp_hang_base[MAX_SPRITE_LEVEL + 1];

extern uint32_t relation_level[10][2];

#define MAX_HOMESERV_NUM	1
#define MAX_BATRSERV_NUM	3

enum {
	/*! max acceptable length in bytes for each client package */
	switch_proto_max_len	= 32 * 1024,
	db_proto_max_len		= 32 * 1024,
	bt_proto_max_len		= 32 * 1024,
	hm_proto_max_len		= 32 * 1024,
	code_prot_max_len		= 32 * 1024,
};

enum {
	idc_type_dx				= 1,
	idc_type_wt				= 2,
	idc_type_internal		= 3,
};

/*! socket fd that is connecting to dbproxy */
extern int proxysvr_fd;
/*! socket fd that is connecting to batrserv */
extern int batrserv_fd[MAX_BATRSERV_NUM];
/*! socket fd that is connecting to homeserv */
extern int home_serv_fd[MAX_HOMESERV_NUM];
/*! socket fd that is connecting to switch */
extern int switch_fd;
/*! socket fd that is connecting to cross */
extern int magic_fd;
extern int spacetime_fd;
extern int mall_fd;
extern int vipserv_fd;

/*! for packing protocol data and send to dbproxy */
extern uint8_t dbbuf[db_proto_max_len];

#define MAX_TASK_OUT_OPT		10
#define MAX_TASK_OUT_ITEM_NUM	10

typedef struct task_item {
	uint32_t		type;
	uint32_t		prof;
	uint32_t 		itemid;
	uint32_t		skillid;
	uint32_t		count;
	uint32_t		level;
}__attribute__((packed))task_item_t;

typedef struct task_option {
	uint32_t		out_id;
	uint32_t		out_cnt;
	task_item_t		items[MAX_TASK_OUT_ITEM_NUM];
}__attribute__((packed))task_option_t;

#define MAX_PARENT_TASK_CNT		10
#define MAX_TASK_NODE_CNT		10	

typedef struct item_target {
	uint32_t		itemid;
	uint32_t		count;
	uint32_t		beastid;
	uint16_t		dropodds;
	uint16_t		del;
	uint16_t		check;
	uint16_t		cntlimit;
	uint32_t		dropratio[3];
}__attribute__((packed)) item_target_t;

typedef struct beast_target {
	uint32_t		beastid;
	uint32_t		count;
	uint16_t		win;
	uint16_t		type;
	uint16_t		help;
	uint32_t		mapid;
}__attribute__((packed)) beast_target_t;


typedef struct pk_target {
	uint32_t		count;
	uint16_t		win;
}__attribute__((packed)) pk_target_t;

typedef struct pet_target {
	uint32_t		petcnt;
	uint32_t		pettype[MAX_PETS_PER_SPRITE];
	uint32_t		count[MAX_PETS_PER_SPRITE];
}__attribute__((packed)) pet_target_t;

typedef struct task_node {
	uint8_t			nodeid;
	uint32_t		isend;
	item_t			usetarget;
	item_target_t	itarget;
	beast_target_t	btarget;
	pk_target_t		ptarget;
	pet_target_t	pettarget;
	uint32_t		optcnt;
	item_t			skilltarget;
	task_option_t	options[MAX_TASK_OUT_OPT + 1];	/* out_opts[0] for out item first time */
	uint32_t 		needtime;
}__attribute__((packed))task_node_t;

typedef struct task {
	uint32_t		taskid;	
	uint16_t		prelation;	
	uint8_t			ptaskcnt;
	uint32_t		ptasklist[MAX_PARENT_TASK_CNT];
	uint32_t		repeat;	
	uint32_t		repeat_cnt;
	uint32_t		startday;
	uint32_t		need_lv;		/* level needed to recv task */
	uint32_t		max_lv;
	uint32_t		need_skill;
	uint32_t		min_skill_lv;
	uint32_t		max_skill_lv;
	uint32_t		need_job;	/*  career need to recv task (professtion) */
	uint32_t		need_joblv;	/* career level needed to recv task */	
	uint8_t			node_cnt;
	task_node_t		nodes[MAX_TASK_NODE_CNT];
	uint32_t		time_valid[DAYS_PER_WEEK];
	uint32_t		type;
	uint32_t		loopid;
}__attribute__((packed))task_t;
extern GHashTable* all_tasks;

#define MAX_TASK_LOOP_CNT			10
#define MAX_TASK_PER_LOOP			100
typedef struct task_loop {
	uint32_t		loopid;
	uint32_t		taskcnt;
	uint32_t		tasks[MAX_TASK_PER_LOOP];
	uint32_t		mincnt;
	uint32_t		maxcnt;
	uint32_t		loopcnt;
	uint32_t		maxsame;
	uint32_t		startid;
	uint32_t		endid;
}__attribute__((packed)) task_loop_t;
extern task_loop_t task_loops[MAX_TASK_LOOP_CNT];

extern GHashTable*	maps;
extern int home_serv_fd[MAX_HOMESERV_NUM];

#define MAX_BOX_ITEM_CNT		16
#define MAX_BOX_CNT				150
#define MAX_ACTIVE_BOX_CNT		16
#define MAX_BOX_POS_CNT			100
#define MAX_BOX_CLOTH_CNT      5
typedef struct box_pos {
	map_t*			p_m;
	uint16_t		posx;
	uint16_t		posy;
	uint8_t			hasbox;
}__attribute__((packed))box_pos_t;

enum {
	box_refresh_type_box,
	//>100
	box_refresh_type_grass,
};
typedef struct box {
	uint32_t		id;
	uint32_t		refresh_type;
	uint32_t		type;
	uint32_t		refreshtime;
	uint32_t		activate_time;
	uint32_t		loading_time;
	uint8_t			beastcnt;
	uint32_t		beastids[MAX_BEAST_TYPE_PER_MAP];
	uint32_t		beastrate[MAX_BEAST_TYPE_PER_MAP];
	uint32_t		beastrand;
	uint8_t			itemcnt;
	uint32_t		itemid[MAX_BOX_ITEM_CNT];
	uint32_t		itemrate[MAX_BOX_ITEM_CNT];
	uint32_t		cntmax;
	uint32_t		cntmin;
	uint32_t		pos_cnt;
	box_pos_t*		pos[MAX_BOX_POS_CNT];
	box_pos_t*		pbpactive;
	beast_group_t	box_beast;
	list_head_t		active_list;
	uint32_t 		last_mapid;	
}__attribute__((packed))box_t;

extern box_t all_boxes[MAX_BOX_CNT];
extern uint32_t active_box_cnt;
extern uint32_t active_box[MAX_ACTIVE_BOX_CNT];
extern box_pos_t box_pos[MAX_BOX_POS_CNT];
extern list_head_t active_box_list;

#define PIC_SRV_MAX_NUM		128

typedef struct ipport{
	char		ip[16];
	uint16_t	port;
	uint32_t    ip_int;
}__attribute__ ((packed))ipport_t;

extern ipport_t pic_srv[PIC_SRV_MAX_NUM];
extern int pic_srv_num;

/*! mail */

#define MAX_MAIL_COUNT	64

#define MAIL_ITEM_LIST_LEN	64
#define MAIL_ID_LIST_LEN	32
#define MAIL_TITLE_LEN		64
#define MAIL_SEND_NICK_LEN		64
#define MAIL_MESSAGE_LEN	256
#define MAIL_IDX_CNT		4


enum {
	mail_sys_xm			= 1,
	mail_sys_login		= 2,
	mail_sys_level		= 3,
	mail_sys_task		= 4,
	mail_sys_clothes	= 5,
	mail_sys_noti		= 6,
	mail_sys_max,
};

typedef struct mail_op {
	uint64_t key;
	uint32_t value1;
	uint32_t value2;
	uint32_t idx[MAIL_IDX_CNT];
}__attribute__((packed))mail_op_t;

typedef struct mail {
	uint32_t	indexid;
	uint32_t	themeid;
	char	send_nick[MAIL_SEND_NICK_LEN];
	char	title[MAIL_TITLE_LEN];
	char	message[MAIL_MESSAGE_LEN];
	char	itmlist[MAIL_ITEM_LIST_LEN];
}__attribute__((packed))mail_t;

extern mail_op_t sz_mail_op[mail_sys_max][MAX_MAIL_COUNT];

extern mail_t	sz_mail[MAX_MAIL_COUNT];

extern GHashTable* all_mails;

#define SYS_INFO_MSG_LEN		64
typedef struct sysinfo {
	uint64_t		key;
	uint32_t		type;
	uint32_t		val1;
	uint32_t		val2;
	char			message[SYS_INFO_MSG_LEN];
}__attribute__((packed)) sysinfo_t;
extern GHashTable* sys_infos;

#define MAX_LAYERS		8
#define MAX_TEMPLATES	16
#define MAX_MAZE_MAPS	256

typedef struct {
	int16_t		x;
	int16_t		y;
	uint32_t	flag;
}	maze_point_t;

typedef struct{
	uint32_t 		mapid;
	uint16_t	 	useds;
	uint16_t	 	count;
	maze_point_t	point[8];	 
}	maze_map_t;

typedef struct {
	uint16_t	map_idx[8];
	uint16_t	map_rate[8];
	uint32_t	shop_id[4];	
	uint16_t	shop_rate[4];
}	layer_tpl_t;

typedef struct {
	uint8_t  		tag_id;
	uint8_t  		layers;
	uint8_t  		limits;	
	uint8_t  		actives;
	uint32_t 		refresh;
	uint32_t 		start_time;	
	uint32_t 		close_time;
	layer_tpl_t		layers_tpl[MAX_LAYERS];
	list_head_t		mazes_head;
}	maze_tpl_t;

typedef struct {
	uint16_t	map_count;
	uint16_t	tpl_count;
	maze_tpl_t	pmaze_tpl[MAX_TEMPLATES];
	maze_map_t	pmaze_map[MAX_MAZE_MAPS];
}	maze_config_t;

typedef struct {
	map_id_t	mapid;//new mapid
	uint16_t	index;//src map index
	uint8_t		iback;//
	uint8_t		inext;
	uint32_t	shopid:24;
	uint32_t	ishopxy:8;
}	maze_layer_t;

typedef struct {
	uint32_t 		index;
	maze_tpl_t*		p_type;
	maze_layer_t	layers[MAX_LAYERS];
	list_head_t		mzlist;
}	maze_t;

extern maze_config_t maze_config;

extern int maze_next;
extern maze_t maze_all[256];

extern int total_water_cnt;
extern int last_min_water_cnt;
extern int auto_water_cnt;

typedef struct gift_pet {
	uint32_t			id;
	uint32_t			pettype;
	uint32_t			checkcnt;
	uint32_t			checktypes[MAX_PETS_PER_SPRITE];
}__attribute__((packed)) gift_pet_t;

extern gift_pet_t g_gook_gift_pets[1];

typedef struct init_prof_info {
	uint16_t			profid;
	uint32_t			honorid;
	uint16_t			physique;
	uint16_t			strength;
	uint16_t			endurance;
	uint16_t			quick;
	uint16_t			intelligence;
	uint32_t			cloth_cnt;
	uint32_t			cloths[MAX_ITEMS_WITH_BODY];
	uint32_t			clothpatch;
}__attribute__((packed)) init_prof_info_t;

extern init_prof_info_t init_prof_infos[MAX_PROF_CNT];

#define SEAT_NUM		48
#define SEAT_TYPE		3
extern uint32_t hangup_uids[SEAT_TYPE * SEAT_NUM];

extern item_t rewards[3][4];

#define MAX_KILL_BEAST_CNT		5000
extern int beast_cnt;

#define MAX_GET_SKILL 			2

#define COPY_BEAST_MAX_TYPE			4

typedef struct copy_beast {
	uint32_t			order;
	uint32_t			beast_type;
	uint32_t			grpids[COPY_BEAST_MAX_TYPE];
	uint32_t			validcnts[COPY_BEAST_MAX_TYPE];
	uint32_t			needkillcnts[COPY_BEAST_MAX_TYPE];
}__attribute__((packed)) copy_beast_t;

#define COPY_BEAST_MAX_ORDER		4

typedef struct copy_layer {
	uint32_t			layerid;
	int					parentlayer;
	uint32_t			templatemap;
	uint32_t			needitem;
	uint32_t			needitemcnt;
	uint32_t			beast_orders;
	copy_beast_t		beasts[COPY_BEAST_MAX_ORDER];
}__attribute__((packed)) copy_layer_t;

#define MAX_COPY_LAYER				6
#define MAX_MAP_COPY				5000

typedef struct map_copy_config {
	uint32_t			copyid;
	uint32_t			day_limit;
	uint32_t			minilv;
	int					layer_cnt;
	copy_layer_t		layers[MAX_COPY_LAYER];
	uint32_t			instanceid[MAX_MAP_COPY];
}__attribute__((packed)) map_copy_config_t;


#define MAX_COPY_CONFIG_CNT 3
extern map_copy_config_t map_copy_configs[MAX_COPY_CONFIG_CNT];

typedef struct map_copy_instance {
	map_id_t			mapid;
	map_copy_config_t*	p_copy_config;
	uint32_t			killedcnts[MAX_COPY_LAYER][COPY_BEAST_MAX_ORDER][COPY_BEAST_MAX_TYPE];
	uint32_t			itemdel[MAX_COPY_LAYER];
	uint8_t				usercnt;
	uint32_t			uids[MAX_PLAYERS_PER_TEAM];
	uint32_t			ustate[MAX_PLAYERS_PER_TEAM];
	list_head_t 		timer_list;
}__attribute__((packed)) map_copy_instance_t;

extern GHashTable*	 map_copys; 
typedef struct user_wait_info{
	uint32_t 		userid;
	uint16_t		lv;
	list_head_t 	timer_list;
}__attribute__((packed)) user_wait_info_t;
#define MAX_SKILL_DIFF 	10
#define MAX_GVG_WAIT   100
#define MAX_USER_WAIT  	((uint32_t)(MAX_SPRITE_LEVEL/MAX_SKILL_DIFF))
extern user_wait_info_t user_for_team[MAX_USER_WAIT];
extern uint32_t gvg_groups[MAX_GVG_WAIT];

#define MAX_RECOVER_ERNERGY			300
#define MAX_VIP_LV				 	10	
extern	uint8_t max_openbox_times;
#define PRECIOUS_BOX_TYPE		8
extern uint32_t hangup_rewards[4];
extern	uint32_t g_luckystar_exchange;
#define HANGUP_START_TIME		13*3600
#define IRON_REWARD_BOX			3600/2
#define COPPER_REWARD_BOX		3600
#define SILVER_REWARD_BOX		3600*2
#define GOLD_REWARD_BOX			3600*3
#define MAX_HANGUP_TIME			3600*3

#define GRASS_GAME_TYPE			9050
#define LV_REWARD_TYPE			10001
#define MAP_COPY_MINI_LV		30
#define MAX_GVG_CHALLENGE 		3
#define MAX_USER_RESET_CNT 1
#define MAX_PET_RESET_CNT 2
#define  TRAIN_TASK_ID 36544 
#define  MAX_TRADE_ROOM_CNT	1000
#define  HALF_HOUR_TIME    1800
#define  ONE_HOUR_TIME   3600 
#define  VIP_EXP_FACTOR_PER_DAY   20
#define  VIP_TITLE 	2007	
#define KF_LOG(type_str,uid,fmt,args...) \
			SIMPLY(log_lvl_notice, 0,"%s\t%s\t%u\t"fmt,get_datetime(time(NULL)) ,type_str,uid, ##args)
//NOTI_LOG("%s\t%u\t%s",get_datetime(time(NULL)) ,uid,type_str,fmt,args...)
#endif
