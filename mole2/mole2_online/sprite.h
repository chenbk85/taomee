#ifndef MOLE2_SPRITE_H
#define MOLE2_SPRITE_H

#include "benchapi.h"
#include "util.h"
#include "statistic.h"

enum {
	INJURY_NONE		=	0,
	INJURY_WHITE,
	INJURY_YELLOW,
	INJURY_RED,
	INJURY_MAX,
};

enum {
	KICK_MIDNIGHT	= 0,
	KICK_TIMEOUT	= 1,
};

enum {
	flag_vip_bit		= 0,
	flag_init_bit		= 1,
	flag_guide_bit		= 2,		// no more use
	flag_rookie_bit		= 3,		// no more use
	flag_mogu_bit		= 4,		// no more use
	flag_20lv_bit		= 5,
	flag_team_sw		= 6,
	flag_chaola_bit		= 7,
	flag_vip_ever_bit	= 8,
	flag_vip_1st_on		= 9,
	flag_vip_1mail		= 10,
	flag_vip_2mail		= 11,
	flag_vip_3mail		= 12,
	flag_vip_4mail		= 13,
	flag_hero_cup_bit	= 14,

	flag_fashion_bit	= 15,
	flag_cli_16 		= 16,  

	flag_vip_item_tip	= 17,		// no more use
	flag_herocup_item	= 18,
	flag_herocup_new	= 19,
	flag_herocup_reward	= 20,
	flag_signin_bit		= 21,
	flag_vip_item_tip_2	= 22,
	flag_cli_1			= 23,
	flag_cli_2			= 24,
	flag_cli_3			= 25,
	flag_cli_5			= 26,

	flag_vip_charged	= 27,
	flag_vip_item_got	= 28,
	flag_cli_4			= 29,

	flag_hide_bit		= 30,
	flag_cli_31			= 31,
	flag_auto_fight		= 32,// 是否自动战斗
	flag_fire_acitivity	= 33,//天火活动
	flag_reset_attr  	= 34,//是否自动分配属性点
	flag_cli_35  		= 35,
	flag_try_vip  		= 36,//vip 使用活动
	flag_try_vip2  		= 37,//第二次 vip使用
	flag_try_vip3  		= 38,//新手赠送vip
};

enum {
	update_flag_hp_mp_bit = 1,
	update_flag_pet_bit,
};

enum {
	prof_swordman = 1,
	prof_ancher,
	prof_magician,
	prof_churchman,
	prof_ninja,
	prof_gedoushi,
	prof_heimodaoshi,
	prof_shengyanshi,
	prof_wushushi,
};


#define CHECK_PROF_VALID(id_, prof_) \
	if ((prof_) < prof_swordman || (prof_) > prof_ninja) { \
		KERROR_LOG(id_, "prof invalid\t[%u]", prof_); \
		return -1; \
	}
	
#define IS_PROF_MATCH(p, prof_) ((1 << (p)->prof) & (prof_))
#define IS_WIZARD(p) (p->prof == prof_magician || p->prof == prof_churchman)

#define BE_INITED(flag_)	((flag_) & 1 << flag_init_bit)
#define ISVIP(flag_)			((flag_) & (1 << flag_vip_bit))
#define IS_EVER_VIP(p_)			((p_)->flag & (1 << flag_vip_ever_bit))
#define IS_SPRITE_VISIBLE(p_)	!((p_)->flag & (1 << flag_hide_bit))
//#define IS_ADMIN(p_)			((p_)->flag & (1 << flag_admin_bit))
#define IS_ADMIN(p_)			(0)

#define SPRITE_STRUCT_LEN	10240
#define SPRITE_FRIENDS_MAX	200

#define GUEST_SEQ_NUM		2000000000
#define IS_GUEST_ID(x)		(x >= GUEST_SEQ_NUM)

#define EXP_PER_BATTER_MAX	99999
#define MAX_EXP				(MAX_SPRITE_LEVEL * MAX_SPRITE_LEVEL * MAX_SPRITE_LEVEL * MAX_SPRITE_LEVEL)

#define LIMIT_BATTER_EXP(exp_,uid_) \
	do {	\
		if ((exp_) > EXP_PER_BATTER_MAX) {	\
			KERROR_LOG((uid_), "exp single battle max\[%u]", (exp_)); \
			(exp_) = EXP_PER_BATTER_MAX;	\
		}	\
	} while(0)

#define LIMIT_MAX_EXP(exp_,uid_) \
	do {	\
		if ((exp_) > MAX_EXP) {	\
			KERROR_LOG((uid_), "exp over max\[%u]", (exp_)); \
			(exp_) = MAX_EXP;	\
		}	\
	} while(0)

#define LIMIT_PET_MAX_EXP(exp_,uid_) \
	do {	\
		if ((exp_) >= MAX_PET_EXP) {	\
			KERROR_LOG((uid_), "pet exp over max\[%u]", (exp_)); \
			(exp_) = MAX_PET_EXP-1;	\
		}	\
	} while(0)

#define LIMIT_USER_MAX_EXP(exp_,uid_) \
	do {	\
		if ((exp_) >= MAX_TEMP_EXP_EX) {	\
			KERROR_LOG((uid_), "user exp over max\[%u]", (exp_)); \
			(exp_) = MAX_TEMP_EXP_EX-1;	\
		}	\
	} while(0)



#define CHECK_VALID_ID(id)	if (!id||IS_BEAST_ID(id)||IS_GUEST_ID(id))ERROR_RETURN (("error id=%u", id), -1)

#define CHECK_TRUE_FALSE_VALID(p_, val_) \
	do { \
		if ((val_) != 0 && (val_) != 1) { \
			KERROR_LOG(p_->id, "must 0/1\t[%u]", val_); \
			return -1; \
		} \
	} while (0)

#define CHECK_LV_FIT(p_, lp_, minlv_, maxlv_) \
	do { \
		if ((lp_)->level < (minlv_) || (lp_)->level > (maxlv_)) { \
			KERROR_LOG((p_)->id, "lv not fit[%u %u %u %u]", (lp_)->id, (lp_)->level, minlv_, maxlv_); \
			return send_to_self_error((p_), (p_)->waitcmd, cli_err_level_not_fit, 1); \
		} \
	} while (0)

#define CHECK_USER_VIP(p_) \
	do { \
		if (!ISVIP(p_->flag)) { \
			KERROR_LOG((p_)->id, "not vip" ); \
			return send_to_self_error((p_), (p_)->waitcmd, cli_err_not_vip, 1); \
		} \
	} while (0)

#define CHECK_PROF_FIT(p_, prof_, proflv_) \
	do { \
		if (!IS_PROF_MATCH(p_, prof_) || p_->prof_lv < proflv_) { \
			KERROR_LOG((p_)->id, "prof not fit[%u %u %u %u]", (p_)->prof, p_->prof_lv, prof_, proflv_); \
			return send_to_self_error((p_), (p_)->waitcmd, cli_err_level_not_fit, 1); \
		} \
	} while (0)


#define CHECK_XIAOMEE_ENOUGH(p_, xiaomee_) \
	do { \
		if ((p_)->xiaomee < (xiaomee_)) { \
			KERROR_LOG((p_)->id, "xiaomee not enough[%u %u]", (p_)->xiaomee, xiaomee_); \
			return send_to_self_error((p_), (p_)->waitcmd, cli_err_xiaomee_not_enough, 1); \
		} \
	} while (0)

typedef struct VarPkg {
	void*   pkg;
	int     pkg_len;
	time_t  idle_time;
} var_pkg_t;

typedef struct base_second_attr {
	int32_t 			hp_max;
	int32_t 			mp_max;
	int16_t 			attack;
	int16_t				mattack;
	int16_t 			defense;
	int16_t				mdefense;
	int16_t 			speed;
	int16_t 			spirit;
	int16_t 			resume;
	int16_t 			hit_rate;
	int16_t 			avoid_rate;
	int16_t 			bisha;
	int16_t 			fight_back;
}__attribute__((packed))base_second_attr_t;

typedef struct base_5_attr {
	uint16_t			physique;
	uint16_t			strength;
	uint16_t			endurance;
	uint16_t			quick;
	uint16_t			intelligence;
}__attribute__((packed))base_5_attr_t;

void send_varpkg(void* key, void* spri, void* userdata);
void traverse_sprites(void (*action)(void* key, void* spri, void* userdata), void* data);
void init_sprites();
void fini_sprites();
void init_exp_lv();
uint32_t get_lv_exp(uint32_t level);
int get_injury_effect(uint32_t injury_lv);
sprite_t *get_sprite (uint32_t id);
sprite_t* get_sprite_by_fd(int fd);
int del_sprite(sprite_t* p, int del_node);
sprite_t* add_sprite(sprite_t* v);
void calc_sprite_second_level_attr(sprite_t* p);
void calc_base_second_attr(base_second_attr_t* p, base_5_attr_t* p_b5a, uint32_t injury_lv);
void update_user_second_attr(sprite_t* p, base_5_attr_t* p_b5a_new, uint32_t injury_lv);
void update_base_second_attr(base_second_attr_t* p, base_5_attr_t* p_b5a_new, \
	base_5_attr_t* p_b5a_old, uint32_t injury_lv_new, uint32_t injury_lv_old);
void add_second_attr(sprite_t* p, base_attr_t* p_sa,uint32_t type);
void reduce_second_attr(sprite_t* p, base_attr_t* p_sa, uint32_t type);
int pkg_body_clothes_simple(const sprite_t* p, uint8_t *buf);
int pkg_body_clothes_detail(const sprite_t* p, uint8_t *buf);
int pack_activity_info(const sprite_t* p, uint8_t* buf);
int pkg_sprite_simply(const sprite_t *p, uint8_t *buf);
int pkg_sprite_detail(const sprite_t *p, uint8_t *buf, int self);
int pkg_sprite_attr(const sprite_t *p, uint8_t *buf);
int kick_all_users_offline(void* owner, void* data);
int start_kick_all_users(void* owner, void* data);
int kick_user_timeout(void* owner, void* data);
int send_cli_keepalive_pkg(void* owner, void* data);
void modify_sprite_second_attr(sprite_t* p);

static inline void reset_common_action(sprite_t *p)
{
	p->direction = 0;
	p->base_action= 0;
	p->advance_action= 0;
}


static inline uint32_t get_vip_level(uint32_t vipexp)
{
	if (!vipexp) return 0;
	uint32_t vipexps[10] = {150, 450, 900, 1800, 2700, 4500, 6300, 9000, 11700, 15300};
	for (int loop = 0; loop < MAX_VIP_LV; loop ++) {
		if (vipexps[loop] >= vipexp)
			return loop + 1;
	}
	return 10;
}

static inline void recalc_vip_exp_level(sprite_t* p)
{
	uint32_t nowtv = get_now_tv()->tv_sec;
	/*if (p->vip_end <= nowtv) {*/
	/*p->flag &= ~1u;*/
	/*KDEBUG_LOG(p->id, "NO MORE VIP\t");*/
	/*}*/
	
	if (ISVIP(p->flag)) {
		//是vip
		//p->vipexp = p->vip_base_exp + p->vip_exp_ex + 5 * get_day_interval(nowtv, p->vip_begin);
		//changed by francisco 2012-5-18 reason:超能勇士VIP升级点数，每天5点调整为20点（升级速度提升400%）
		p->vipexp = p->vip_base_exp + p->vip_exp_ex + VIP_EXP_FACTOR_PER_DAY * get_day_interval(nowtv, p->vip_begin);
		p->vipexp = p->vipexp ? p->vipexp : 1;
	} else if (IS_EVER_VIP(p)) {
		//曾经时vip
		//uint32_t lost_exp = 5 * get_day_interval(nowtv, p->vip_end);
		//changed by francisco 2012-5-18 reason:VIP过期后，每天损失10点VIP活跃度。损失减少50%
		uint32_t lost_exp = 10 * get_day_interval(nowtv, p->vip_end);
		p->vipexp = p->vip_base_exp + p->vip_exp_ex >= lost_exp ? (p->vip_base_exp + p->vip_exp_ex - lost_exp) : 0;
	} else {
		//不是vip
		p->vip_base_exp = p->vipexp = p->vip_exp_ex = 0;
	}

	p->viplv = get_vip_level(p->vipexp);
	KDEBUG_LOG(p->id, "VIP UPDATE\t[%u %u %u %u %u %u %u]", (p->flag & 0x101), p->vip_auto, p->vip_base_exp, p->vipexp, p->viplv, p->vip_begin, p->vip_end);
}

static inline uint32_t get_left_double_exp_time(sprite_t *p, uint32_t online_time) 
{
	if (p->double_exptime == 0XFFFFFFFF) 
		return p->double_exptime;

	uint32_t double_exp_time = holidy_factor[get_now_tm()->tm_wday].timetype ? 3600*3 : 1800;
	uint32_t cur_dtime = p->double_exptime + get_now_tv()->tv_sec - p->start_dexp_time;
	double_exp_time = cur_dtime > double_exp_time ? 0 : (double_exp_time - cur_dtime);
	uint32_t left_online_time = online_time > 18000 ? 0 : 18000 - online_time;
	double_exp_time = double_exp_time > left_online_time ? left_online_time : double_exp_time;
	return double_exp_time;
}

static inline uint32_t get_ol_time(sprite_t* p)
{
	return p->online_time + (get_now_tv()->tv_sec - p->login_time);
}

static inline int pkg_ol_time(sprite_t *p, uint8_t* buf)
{
	int i = 0;
	uint32_t online_time = 0;
	uint32_t double_exp_time = holidy_factor[get_now_tm()->tm_wday].timetype ? 3600*3: 1800;

	if (!IS_ADMIN(p)) {
		online_time = get_ol_time(p);
		double_exp_time = get_left_double_exp_time(p, online_time);
	}
	
	PKG_UINT32(buf, get_now_tv()->tv_sec, i);
	PKG_UINT32(buf, holidy_factor[get_now_tm()->tm_wday].timetype, i);
	PKG_UINT32(buf, online_time, i);
	PKG_UINT32(buf, double_exp_time, i);
	KDEBUG_LOG(p->id, "OL TIME\t[%u %u %u]", p->fd, online_time, double_exp_time);
	return i;
}

static inline void update_user_flag(sprite_t* p, int bit, int flag)
{
	if (flag)
		p->flag |= (1 << bit);
	else
		p->flag &= ~(1 << bit);
}

static inline uint8_t team_switch_state(const sprite_t* p)
{
	return !!(p->flag & (1 << flag_team_sw));
}

static inline uint32_t max_user_energy(sprite_t* p)
{
	return (100 + 4 * (p->level - 1));
}

static inline gboolean free_body_cloth(gpointer key, gpointer pc, gpointer userdata)
{
	g_slice_free1(sizeof(body_cloth_t), pc);
	return TRUE;
}

static inline gboolean free_fireworks(gpointer key, gpointer pday, gpointer userdata)
{
	g_slice_free1(sizeof(item_t), pday);
	return TRUE;
}

static inline void send_to_all(void* buf, int len)
{
	var_pkg_t vpkg = { buf, len, -1 };
	traverse_sprites(send_varpkg, &vpkg);
}

static inline void send_to_all_idle(void* buf, int len, time_t idle_time)
{
	var_pkg_t vpkg = { buf, len, idle_time };
	traverse_sprites(send_varpkg, &vpkg);
}

static inline void log_sprite_skill(sprite_t* p, int type)
{
	int len = 0;
	char buf[256] = {0};
	skill_t* ps = NULL;
	uint32_t skill_cnt = 0, loop;
	switch(type) {
		case 0:
			ps = p->skills;
			skill_cnt = p->skill_cnt;
			len += sprintf(buf + len, "BATTLE SKILL: %u", skill_cnt);
			break;
		case 1:
			ps = p->life_skills;
			skill_cnt = p->life_skill_cnt;
			len += sprintf(buf + len, "LIFE SKILL: %u", skill_cnt);
			break;
		default:
			return;
	}
	for (loop = 0; loop < skill_cnt; loop ++) {
		len += sprintf(buf + len, " (%u %u %u)", ps[loop].skill_id, ps[loop].skill_level, ps[loop].skill_exp);
	}
	KDEBUG_LOG(p->id, "%s", buf);
}

static inline void log_sprite(sprite_t* p)
{
	KDEBUG_LOG(p->id, "****************************sprite info begin************************");
	KDEBUG_LOG(p->id, "[%u(\"%s\" %u %u %u) prof(%u %u) xm=%u lv(%u %u %u)]\t[%u/%u %u/%u %u]", \
			p->id, p->nick, p->flag, p->honor, p->race, p->prof, p->prof_lv, p->xiaomee, p->level, \
			p->experience, p->attr_addition, p->hp, p->hp_max, p->mp, p->mp_max, p->injury_lv);
	KDEBUG_LOG(p->id, "[FIVE: %u %u %u %u %u]\t[FOUR: %u %u %u %u]\t[atk(%d %d) def(%d %d) speed=%d spirit=%d resume=%d]", \
			p->physique, p->strength, p->endurance, p->quick, p->intelligence, p->earth, p->water, p->fire, p->wind, \
			p->attack, p->mattack, p->defense, p->mdefense, p->speed, p->spirit, p->resume);
	KDEBUG_LOG(p->id, "[hit=%d avoid=%d bisha=%d fightback=%d]\t[%u %u %u %u %u %u]", \
			p->hit_rate, p->avoid_rate, p->bisha, p->fight_back, p->rpoison, p->rlithification, \
			p->rlethargy, p->rinebriation, p->rconfusion, p->roblivion);
	KDEBUG_LOG(p->id, "BAG\t[%u %u %u %u %u]", p->collection_grid, p->chemical_grid, p->card_grid, \
			g_hash_table_size(p->body_cloths), g_hash_table_size(p->all_cloths));
	log_sprite_skill(p, 0);
	log_sprite_skill(p, 1);
	KDEBUG_LOG(p->id, "****************************sprite info end**************************");
}

static inline void monitor_sprite(sprite_t* p, const char* info)
{
	KDEBUG_LOG(p->id, "SPRITE MONITOR\t[%s][lv=%u exp=%u xiaomee=%u pkpt_total=%u pkpt_day=%d]", \
		info, p->level, p->experience, p->xiaomee, p->pkpoint_total, p->pkpoint_day);
}

int load_init_prof_info(xmlNodePtr cur);
init_prof_info_t* get_init_prof_info(uint32_t profid);

#define  IS_OLD_PROF(prof) ((prof)<=5) 
#endif
