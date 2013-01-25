#ifndef ONLINE_BENCHAPI_H_
#define ONLINE_BENCHAPI_H_

#include <time.h>
#include <stdint.h>
#include <glib.h>
#include <map>
#include <set>
#include <libtaomee++/proto/proto_base.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/timer.h>
#include <libtaomee/list.h>
#ifdef __cplusplus
}
#endif


#ifndef likely
#define likely(x)  __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif

#define MAX_PLAYERS_PER_TEAM	5
#define BATTLE_POS_CNT			(2 * MAX_PLAYERS_PER_TEAM)
#define MAX_PETS_PER_SPRITE		6
#define MAX_ITEMS_WITH_BODY		8
#define USER_NICK_LEN			16
#define TEAM_NICK_LEN			16
#define MAX_STR_LEN				64

#define BEAST_COMMON_RAND_BASE	1000u
#define RAND_COMMON_RAND_BASE	1000u
#define CLOTH_COMMON_RAND_BASE	10000u

#define BEGIN_USER_ID			50000

#define MAX_BEAST_TYPE			4000
#define MAX_NPC_TYPE			5000
#define MAX_RACE_TYPE			10

#define IS_BEAST_ID(uid_)  		(uid_ < BEGIN_USER_ID && uid_ != 0)
#define IS_NORMAL_ID(uid_)  	((uid_ >= BEGIN_USER_ID) && !IS_GUEST_ID(uid_))

#define MAX_SKILL_BAR			10

#define TASK_SVR_BUF_LEN		20
#define TASK_CLI_BUF_LEN		128

#define VIP_BUFF_LEN			128

#define BEGIN_CLEAN_BOSS		2501
#define END_CLEAN_BOSS			2511
#define DRAW_GIFT_CNT			3

#define BATTLE_USER_ID(btid_)	((btid_) & 0xFFFFFFFF)
#define MAX_SKILL_CACHE 		10


enum {
	skill_resist_lithification		= 1501,
	skill_resist_oblivion			= 1502,
	skill_resist_confusion			= 1503,
	skill_resist_commination		= 1504,
	skill_resist_poison				= 1505,
	skill_resist_lethargy			= 1506,
	
	skill_life_pet_contact			= 100001,
	skill_life_pet_raise			= 100002,
	skill_life_medical_mix			= 100003,
	skill_life_food_cook			= 100004,
	skill_life_weapon_make			= 100005,
	skill_life_armor_make			= 100006,
	skill_life_collect				= 100007,
	skill_life_cut					= 100008,
	skill_life_hunt					= 100009,
	skill_life_equipment_repair		= 100010,
};

enum {
	attr_id_hpmax = 1,
	attr_id_mpmax,
	attr_id_spirit,
	attr_id_resume,
	attr_id_dodge,
	attr_id_hit,
	attr_id_avoid,
	attr_id_mdef,
	attr_id_rconfusion,
	attr_id_rpoison,
	attr_id_rlethargy,
	attr_id_roblivion,
	attr_id_rcommination,
	attr_id_rlithification,
	attr_id_max,
};

enum equip_part {
	part_hat				= 1,
	part_cloth				= 2,
	part_glove				= 3,
	part_shoe				= 4,
	part_weapon				= 5,
	part_shield				= 6,
	part_guajian			= 7,
	part_crystal			= 8,
};

enum buffid {
	buffid_exp			= 0,
	buffid_skill_exp	= 1,
	buffid_pet_exp		= 2,
	buffid_auto_fight	= 3,
	buffid_hpmp			= 4,
	buffid_max_vip,

	buffid_beast_lv10	= 1000,
	buffid_beast_lv20	= 1001,
	buffid_beast_lv35	= 1002,
	buffid_max_beast,
};

typedef uint64_t map_id_t;
typedef uint32_t userid_t;
typedef uint64_t battle_id_t;

typedef struct cloth_attr {
	uint32_t			clothtype;
	uint32_t			grid;
	uint16_t 			cloth_level;
	uint16_t			duration_max;
	uint16_t			duration;

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
	uint32_t			crystal_attr;
	uint32_t			bless_type;
}__attribute__((packed))cloth_attr_t;

typedef struct body_cloth {
	uint32_t			clothid;
	uint32_t			clothtype;
	uint32_t			grid;
	uint16_t 			clothlv;
	uint16_t			duration_max;
	uint16_t			duration;

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
	uint32_t			crystal_attr;
	uint32_t			bless_type;
}__attribute__((packed)) body_cloth_t;

typedef struct item {
	uint32_t		itemid;
	uint32_t		count;
}__attribute__((packed))item_t;

typedef struct id_cnt {
	uint32_t		id;
	uint32_t		count;
}__attribute__((packed)) id_cnt_t;

typedef struct skill {
	uint32_t		skill_id;
	uint8_t			skill_level;
	uint8_t			default_lv;
	uint32_t		skill_exp;
}__attribute__((packed))skill_t;

typedef struct pet_attr_db {
	uint16_t			physi_init100;
	uint16_t			stren_init100;
	uint16_t			endur_init100;
	uint16_t			quick_init100;
	uint16_t			intel_init100;
	uint16_t			physi_para10;
	uint16_t			stren_para10;
	uint16_t			endur_para10;
	uint16_t			quick_para10;
	uint16_t			intel_para10;
	uint16_t			physi_add;
	uint16_t			stren_add;
	uint16_t			endur_add;
	uint16_t			quick_add;
	uint16_t			intel_add;
}__attribute__((packed))pet_attr_db_t;

typedef struct pet {
	uint32_t			petid;
	uint32_t			pettype; // not use now
	uint8_t 			race;
	uint32_t			flag;  // beast not use, only for pet
	
	char				nick[USER_NICK_LEN];
	uint32_t			level;
	uint32_t			experience; // beast not use, only for pet
	pet_attr_db_t		base_attr_db;
	uint32_t			physi100;
	uint32_t			stren100;
	uint32_t			endur100;
	uint32_t			quick100;
	uint32_t			intel100;
	uint16_t			attr_addition;
	uint32_t			hp;
	uint32_t			mp;
	uint8_t				earth;
	uint8_t				water;
	uint8_t				fire;
	uint8_t				wind;
	uint32_t			injury_lv;
	uint32_t			action_state;

	uint8_t				skill_cnt;
	skill_t				skills[MAX_SKILL_BAR];
		
	uint32_t			default_skills[2];
	uint32_t			default_levels[2];

	//second level attr
	uint32_t			hp_max;
	uint32_t			mp_max;
	int16_t				attack;
	int16_t				defense;
	int16_t				speed;
	int16_t				spirit;
	int16_t				resume;
	int16_t				hit_rate;
	int16_t				avoid_rate;
	int16_t				bisha;
	int16_t				fight_back;
	//for cal
	/*int16_t				attack_for_cal;*/
	/*int16_t				defense_for_cal;*/
	/*int16_t				speed_for_cal;*/
}__attribute__((packed))pet_t;

typedef struct base_attr {
	uint32_t			id;
	uint32_t			minlv;
	uint32_t			maxlv;
	uint16_t			lvtype;
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
	uint16_t			leaderlv[2];
	uint16_t			percent;
	uint32_t			itemid;
	uint16_t			itemcnt;
	uint16_t			rate;
	uint16_t			double_atk_flag;
}__attribute__((packed))base_attr_t;

typedef struct task_fin {
	uint32_t			taskid;
	uint8_t				fin;
	uint8_t				everfin;
}__attribute__((packed)) task_fin_t;

typedef struct beast_kill {
	uint32_t			beastid;
	uint32_t			state;
	uint32_t			count;
}__attribute__((packed)) beast_kill_t;

typedef struct map_info {
	uint32_t			mapid;
	uint32_t			state;
}__attribute__((packed)) map_info_t;

typedef struct task_db {
	uint32_t		taskid;
	uint32_t		nodeid;
	uint32_t		state;
	uint32_t		optdate;//设置buf的时间
	uint32_t		fintime;//完成任务时间
	uint32_t		finnum;
	uint8_t			cli_buf[TASK_CLI_BUF_LEN];
	uint16_t		svr_buf[TASK_SVR_BUF_LEN / 2];
}__attribute__((packed))task_db_t;

// sizeof(loop_db_t) == sizeof(task_db_t)
typedef struct loop_db {
	uint32_t		loopid;  
	uint32_t		taskcnt;
	uint32_t		state;  // always ON
	uint32_t		optdate;
	uint32_t		finidx;
	uint32_t		finnum;
	uint32_t		ids[(TASK_CLI_BUF_LEN + TASK_SVR_BUF_LEN) / 4];
}__attribute__((packed)) loop_db_t;

#define LOOP_GRP_CNT		(buffid_max_beast - buffid_beast_lv10)
#define LOOP_GRP_BEAST_CNT	5
#define LOOP_GRP_MAX_DAY	3
#define LOOP_GRP_VERSION	1

typedef struct grp_loop {
	uint32_t		version;
	uint32_t		uptime;
	uint32_t		draw_total;
	uint32_t		day_draw_cnt;
	uint32_t		beastcnt;
	id_cnt_t		beasts[LOOP_GRP_BEAST_CNT];
	uint8_t			pad[VIP_BUFF_LEN - 60];
}__attribute__((packed)) grp_loop_t;


#define MAX_RELATION_USER		5

typedef struct relation_user {
	uint32_t		uid;
	uint32_t		day;
	uint32_t		intimacy;
}__attribute__((packed)) relation_user_t;

typedef struct relation {
	uint32_t		flag;
	uint32_t		exp;
	uint32_t		graduation;
	uint32_t		intimacy;
	uint32_t		count;
	relation_user_t	users[MAX_RELATION_USER];
}__attribute__((packed)) relation_t;

typedef struct trade_item{
	uint32_t tradeid;
	uint32_t itemid;		
	int32_t itemcnt;
	uint32_t price;
	uint32_t tax;
}__attribute__((packed)) trade_item_t;

typedef struct trade_cloth{
	uint32_t tradeid;
	uint32_t clothid;		
	uint32_t clothtype;
	uint32_t price;
	uint32_t tax;
}__attribute__((packed)) trade_cloth_t;

typedef struct shape_shifting_info{
	uint32_t itemid;
	uint32_t pettype;
	uint32_t lefttime;
}__attribute__((packed)) shape_shifting_info_t;

typedef struct trade_record{
	uint32_t trade_time;
	uint32_t itemid;		
	uint32_t itemcnt;
	uint32_t price;
	uint32_t tax;
}__attribute__((packed)) trade_record_t;

typedef struct pet_simple{
	uint32_t race;		
	uint32_t level;
}__attribute__((packed)) pet_simple_t;


typedef struct sprite {
	int			fd;
	struct fdsession*	fdsess;
	uint32_t			last_sent_tm;
	int32_t				seq;

	userid_t			id;
	uint32_t			pettype;
	char				nick[USER_NICK_LEN];
	char				signature[32];
	uint32_t			flag;	
	uint32_t 			color;
	uint32_t			register_time;
	uint8_t				race;
	uint8_t				prof;	// 0: none, 1:swordsman 2:ancher 3:magician 4:churchman
	uint32_t			prof_lv;
	uint32_t			honor;
	uint32_t			xiaomee;
	uint32_t			level;
	uint32_t			experience;
	uint16_t			physique;
	uint16_t			strength;
	uint16_t			endurance;
	uint16_t			quick;
	uint16_t			intelligence;
	uint16_t			attr_addition;
	uint32_t			hp;
	uint32_t			mp;
	uint32_t			injury_lv;		// 0: healthy 1:white 2:yellow 3:red
	uint8_t				in_front;		// 1: sprite in front;   0: pet in front
	uint32_t			max_cloth_cnt;
	uint32_t			max_m_cnt;
	uint32_t			max_c_cnt;

	userid_t			parent_id;
	int32_t				pkpoint_day;
	uint32_t			pkpoint_total;

	uint32_t			vacancy_cnt;

	int					cur_fish_cnt;

	uint32_t			medal_devote;
	uint32_t			hero_team;

	uint32_t			flag2;	
	uint32_t			vip_auto;
	uint32_t			viplv;
	uint32_t			vip_base_exp;
	uint32_t			vip_exp_ex;
	uint32_t			vipexp;
	uint32_t			vip_begin;
	uint32_t			vip_end;
  
	uint32_t			vip_time_1;
	uint32_t			vip_time_2;
	uint32_t			vip_time_3;//下架 vip_activity4
	//第5次活动vip冲的天数
	uint32_t			vip_day_5;
	uint32_t			vip_noti_flag_5;
	//第6次活动vip冲的天数
	uint32_t			vip_day_6;
	uint32_t			vip_noti_flag_6;
	//翻牌次数
	uint32_t			draw_cnt_gift;//vip_activity3
	
	uint32_t			vip_dragon_cnt;
	uint32_t			vip_dragon_2;

	uint32_t			draw_max_cnt;
	uint32_t			draw_cnt_day;
	uint32_t			draw_cnt_total;
	uint32_t			vip_draw_cnt_day;
	uint32_t			vip_draw_cnt_total;
	uint32_t			vip_draw_cnt_gift_use;
	//pvp分数
	uint32_t			pvp_scores;
	//每天pk失败 次数
	uint32_t			pvp_pk_fail_times[2];
	uint32_t			pvp_type;//0: 单人单精灵赛” ;1:“单人多精灵赛”
	//pvp 可以得到物品 :1：胜利, 2:失败,3:失败(用于加物品)
	uint32_t			pvp_pk_can_get_item;
//勇士的修行计划
	uint32_t  			trainning_start_time;
	uint32_t  			trainning_last_time;
	uint32_t  			trainning_least_time;

	uint32_t 			watchid;//观战选手id

	uint32_t			expbox;
	uint32_t			skill_expbox;
	uint32_t			energy;
	uint32_t			uiflag;
	uint32_t			hangup_rewards;
	uint32_t			pumpkin_lantern_cnt;
	uint8_t				pumpkin_lantern_activity_type; 
	// attire
	GHashTable*			body_cloths;
	GHashTable*			all_cloths;
	GHashTable*			cloth_uplist;
	GHashTable*			cloth_timelist;

	uint16_t			collection_grid;
	uint16_t			chemical_grid;
	uint16_t			card_grid;
	GHashTable*			bag_items;

	GHashTable*			recvtasks;
	GHashTable*			fintasks;
	GHashTable*			looptasks;
	
	uint8_t				skill_cnt;
	skill_t				skills[MAX_SKILL_BAR];
	uint8_t				life_skill_cnt;
	skill_t				life_skills[MAX_SKILL_BAR];
					
	uint32_t			default_skills[2];
	uint32_t			default_levels[2];
	uint32_t			cache_skill_exp;
	timer_struct_t*		p_add_energy;

	timer_struct_t*		p_lucky_star;//幸运星活动定时器
	uint32_t 			luckystar_count;//幸运星数量

	uint8_t				open_box_times;
	uint32_t			hangup_activity_time;//在线礼箱挂机时间
	//std::map<uint32_t,uint32_t>  *item_cnt_map;//存储物品-数量map  id在item.h定义  
	//for map
	struct map*			tiles;
	map_id_t			portal;
	uint16_t			startX;
	uint16_t			startY;
	uint16_t			posX;
	uint16_t			posY;
	uint32_t			base_action;
	uint32_t			advance_action;
	uint32_t			direction;
	uint32_t			seatid;
	timer_struct_t*		p_hangup_timer;
	uint32_t			recv_map;
	uint32_t			boxid;
	uint32_t			box_get_rand;
	GHashTable*			beast_handbook;
	GHashTable*			item_handbook;
	GHashTable*			map_infos;
	GHashTable*			day_limits;
	GHashTable*			user_honors;

	uint32_t			firework_beast;
	GHashTable*			fireworks;

	uint32_t			vip_buffs[buffid_max_vip][VIP_BUFF_LEN / 4];

	id_cnt_t*			pgrp;
	uint32_t			grpid;			
	grp_loop_t			loop_grp[LOOP_GRP_CNT];
	int16_t				pos_in_line;
	
	//second level attr
	uint32_t			hp_max;
	uint32_t			mp_max;
	int16_t				attack;
	int16_t				mattack;
	int16_t				defense;
	int16_t				mdefense;
	int16_t				speed;
	int16_t				spirit;
	int16_t				resume;
	int16_t				hit_rate;
	int16_t				avoid_rate;
	int16_t				bisha;
	int16_t				fight_back;
	uint8_t				earth;
	uint8_t				water;
	uint8_t				fire;
	uint8_t				wind;
	uint16_t			rpoison;
	uint16_t			rlithification;
	uint16_t			rlethargy;
	uint16_t			rinebriation;
	uint16_t			rconfusion;
	uint16_t			roblivion;
	char				team_nick[TEAM_NICK_LEN];
	uint8_t				team_registerd;
	struct batter_team*	btr_team;
	struct battle*		btr_info;
	uint32_t			team_state;
	uint8_t				pk_switch;
	uint32_t			busy_state;
	//pet 
	pet_t*				p_pet_follow;
	uint32_t			all_petcnt;
	uint8_t				pet_cnt;
	uint8_t				pet_cnt_actual;
	uint8_t				pet_cnt_standby;
	uint8_t				pet_cnt_assist;
	struct pet			pets_inbag[MAX_PETS_PER_SPRITE];
	std::map<uint32_t, pet_simple_t>* pets_inhouse;
	//新职业用来战斗时计算的
	int16_t				attack_for_cal;
	int16_t				defense_for_cal;
	int16_t				speed_for_cal;

	/**************trade*************/
	uint32_t			roomid;	
	char				shopname[USER_NICK_LEN];
	uint8_t				shop_state;
	uint8_t				shop_id;
	uint32_t			register_shop_time;	//抢占店铺的时间（今天过了多少秒） 
	uint32_t			add_item_timestamp;	//上架物品的时间戳
	timer_struct_t*		p_open_shop_timer;//定时器
	//uint8_t				trade_flag;//0 not in trading 1 in trading
	std::multimap <uint32_t,trade_item_t> *trade_item_info;	
	std::multimap <uint32_t,trade_cloth_t> *trade_cloth_info;	
	std::multimap <uint32_t,trade_record_t> *trade_record_info;	
	/*******************************/

	shape_shifting_info_t  shapeshifting;//变身相应信息
	uint8_t				npc_cnt;
	struct sprite*		battle_npc[MAX_PLAYERS_PER_TEAM - 1];
	uint8_t 			challenge_3061;//挑战3061
	relation_t			relationship;
	
	uint32_t			login_time;
	uint32_t			login_day;
	uint32_t			online_time;
	uint32_t			double_exptime;
	uint32_t			start_dexp_time;

	uint32_t			user_timestamp;
	uint32_t			sys_timestamp;

	uint32_t			hang_exp_time;
	uint32_t			hang_pet_time;
	uint32_t			hang_skill_time;

	uint32_t			total_time;
	uint32_t			levelup_time;

	uint32_t			game_start_time;
	uint32_t			game_type;
	uint32_t			illegal_times;
	
	list_head_t			hash_list;
	list_head_t			timer_list;
	list_head_t			map_list;

	uint32_t            item_in_use;
	uint32_t 			stone_in_use;
	uint8_t				stone_cnt;
	uint8_t				skill_id_cache_cnt;
	uint32_t			skill_id_cache[MAX_SKILL_CACHE];
	uint32_t 			petid_in_use;

	uint16_t			recover_energy;
	uint8_t				times_for_hunting_rarebeast;
	//cmd process system
	uint16_t			waitcmd; //current wait cmd
	GQueue* 			pending_cmd;
	list_head_t			busy_sprite_list;
	uint32_t			last_attack_time;
	uint8_t				round_attack_cnt;
	uint32_t			last_fight_time;
	uint32_t			start_walk_time;
	uint32_t			walktime;
	uint32_t			last_fight_step;
	uint32_t			last_beastgrp;
	uint16_t			left_step;
	uint32_t			update_flag;
	uint32_t			last_use_medical_id;//每回合第一次使用的药品
	uint16_t			sess_len;
	uint8_t				session[];
	bool	pk_switch_is_on();
	bool	is_in_war_map();
	//返回实际加入的个数
	uint32_t add_item(uint32_t itemid,uint32_t count );

} __attribute__ ((packed)) sprite_t;

typedef struct batter_team {
	uint32_t		teamid;
	uint8_t			count;
	sprite_t*		players[MAX_PLAYERS_PER_TEAM];
	list_head_t		timer_list;
}__attribute__((packed))batter_team_t;


#define SPRITE_SESS_AVAIL_LEN	(SPRITE_STRUCT_LEN - sizeof(sprite_t))

typedef struct protocol {
	uint32_t	len;
	uint16_t	cmd;
	userid_t	id;
	int32_t		seq;
	uint32_t	ret;
	uint8_t		body[];
} __attribute__((packed)) protocol_t;

#endif
