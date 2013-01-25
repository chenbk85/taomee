
#ifndef MOLE2_BATTER_H
#define MOLE2_BATTER_H

#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <async_serv/async_serv.h>

#include "util.h"
#include "benchapi.h"
#include "map.h"
#include "sns.h"
#include "svr_proto.h"
#include "cli_proto.h"

enum {
	standoff	= 0,
	victory		= 1,
	fail		= 2,
};
/*enum{*/
/*watch_start=1,*/
/*watch_cancel=2,*/
/*watch_error=3,*/
/*noti_user =4,*/
/*watch_succ =5,*/
/*};*/
enum {
	be_pk				= 0,
	be_fight_monster	= 1,
};

enum {
	team_accept			= 0,
	team_refuse			= 1,
};

enum {
	pk_off			= 0,
	pk_on			= 1,
};

enum {
	bt_state_init	= 0,
	bt_state_accept = 1,
	bt_state_create	= 2,
	bt_state_load	= 3,
	bt_state_fight	= 4,
};

#define MAX_TEMP_ITEM			100
#define MAX_TEMP_TASK_ITEM		60
#define MAX_BATTLE_ITEM			3
#define TIME_SPAN_FOR_MONSTER	5

typedef struct warrior_cloth {
	uint32_t			cloth_id;
	uint32_t			type_id;
	uint16_t 			clevel;
	uint16_t			duration;
	uint8_t				equip_pos;
}__attribute__((packed))warrior_cloth_t;

typedef struct warrior_skill {
	skill_t				skill_info;
	uint16_t			percent;
	uint16_t			cool_round;
}__attribute__((packed))warrior_skill_t;

typedef struct warrior {
	userid_t				id;
	uint32_t				petid;
	uint32_t				pettype;
	char					nick[USER_NICK_LEN];
	uint32_t				flag;
	uint32_t				viplv;
	uint32_t 				color;
	uint8_t					race;
	uint8_t					prof;
 	uint32_t				level;
	
	uint16_t				physique;
	uint16_t				strength;
	uint16_t				endurance;
	uint16_t				quick;
	uint16_t				intelligence;
	uint16_t				attr_addition;
	uint32_t				hp;
	uint32_t				mp;
	uint32_t				injury_lv;
	uint8_t					in_front;
	uint8_t					earth;
	uint8_t					water;
	uint8_t					fire;
	uint8_t					wind;
	//second level attr
	uint32_t				hp_max;
	uint32_t				mp_max;
	int16_t					attack;
	int16_t					mattack;
	int16_t					defense;
	int16_t					mdefense;
	int16_t					speed;
	int16_t					spirit;
	int16_t					resume;
	int16_t					hit_rate;
	int16_t					avoid_rate;
	int16_t					bisha;
	int16_t					fight_back;
	
	int16_t					rpoison;
	int16_t					rlithification;
	int16_t					rlethargy;
	int16_t					rinebriation;
	int16_t					rconfusion;
	int16_t					roblivion;

	uint8_t					need_protect_exp;
	
	uint8_t					cloth_cnt;
	warrior_cloth_t 		clothes[MAX_ITEMS_WITH_BODY];
	uint8_t					skill_cnt;
	warrior_skill_t			skills[MAX_SKILL_BAR];
	
	uint32_t				weapon_type;
	uint32_t				shield;

	uint8_t					pet_cnt_actual;		// just for user sprite
	uint16_t				catchable;			// just for beast
	uint32_t				handbooklv;			// just for beast	
	uint32_t				pet_contact_lv;		// just for user sprite
	uint8_t					action_state;		// just for user pet
	uint32_t				default_atk[2];
	uint8_t					default_atklv[2];
	uint32_t				autofight_count;
	 
	uint32_t 				change_race;
	uint32_t				change_petid;
}__attribute__((packed))warrior_t;

typedef struct skill_exp{
	uint32_t 	skill_id;
	uint32_t	skill_exp;
	uint32_t	skill_use_count;
} __attribute__((packed))skill_exp_t;

typedef struct cloth_duration {
	uint32_t	equip_pos;
	uint16_t	duration;
}__attribute__((packed))cloth_duration_t;

typedef struct beast_info {
	uint32_t	beast_id;
	uint32_t	beast_type;
	uint32_t	beast_lv;
}__attribute__((packed))beast_info_t;

typedef struct atk_attr{
	uint32_t 			uid;
	uint32_t			sorp;
	uint32_t 			hp;
	uint32_t 			mp;
	uint32_t			injury_lv;
	uint32_t			default_skills[2];
	uint32_t			default_levels[2];
	uint32_t			autofight;
	int					expri;
	uint32_t			protect_exp;
	skill_exp_t 		skill_expri[MAX_SKILL_BAR];
	cloth_duration_t	cloth_duration[MAX_ITEMS_WITH_BODY];
} __attribute__((packed))atk_attr_t;

typedef struct battle {
	battle_id_t		battle_id;
	uint8_t			pkapply;
	uint32_t		pkteam;
	uint8_t			battletype;
	uint32_t		toplv;
	uint32_t		average_lv;
	uint32_t		floorlv;
	uint32_t		enemy_lv;
	map_id_t		mapid;
	battle_id_t		team_id;
	uint8_t			state;
	uint8_t			count;
	sprite_t*		players[2 * MAX_PLAYERS_PER_TEAM];

	uint8_t			beast_cnt;
	beast_info_t	beasts[2 * MAX_PLAYERS_PER_TEAM];
	//for  watchers
	std::vector<uint32_t>* watchers;
	uint8_t			cachedata[8192];//for cache 1305's data
	uint16_t		datalen;
	uint8_t			load_result;//缓存1307战斗的加载结果 0:没有发1307 1:已经发给战斗1307通知fail 2:已经发给战斗1307通知succ
}__attribute__((packed))battle_t;

#define TEAM_LEADER(p_)	((p_)->id == ((p_)->btr_team)->teamid)
#define MAKE_BATTLE_ID(uid_) ((uid_) | get_now_tv()->tv_sec << 32)
#define GET_BATTLE_ID(uid_, btid_)	((uid_) | ((btid_) & 0xFFFFFFFF00000000))
#define IS_IN_BATTLE(p_) ((p_) && ((p_)->btr_info))
#define CHECK_TEAM_FULL(p_, lp_, addcnt_) \
		do { \
			if (!(p_)->btr_team && (p_)->npc_cnt + 1 + (addcnt_) > MAX_PLAYERS_PER_TEAM) { \
				KERROR_LOG(lp_->id, "team full\t[%u %u]", p_->id, addcnt_); \
				return send_to_self_error((lp_), (lp_)->waitcmd, cli_err_too_many_players, 1); \
			} else if ((p_)->btr_team) { \
				int loop, cnt = (p_)->btr_team->count; \
				for (loop = 0; loop < (p_)->btr_team->count; loop ++) { \
					cnt += (p_)->btr_team->players[loop]->npc_cnt; \
				} \
				if (cnt + (addcnt_) > MAX_PLAYERS_PER_TEAM) { \
					KERROR_LOG(lp_->id, "team full\t[%u %u]", p_->id, addcnt_); \
					return send_to_self_error((lp_), (lp_)->waitcmd, cli_err_too_many_players, 1); \
				} \
			} \
		} while (0)
#define CHECK_SELF_BATTLE_INFO(p, uid) \
		do { \
			if (p->btr_info && p->btr_info->state != bt_state_init) { \
				KERROR_LOG(p->id, "challeger fighting now\t[%u]", uid); \
				return send_to_self_error(p, p->waitcmd, cli_err_self_be_fighting, 1); \
			} else if (p->btr_info) { \
				KERROR_LOG(p->id, "er challenge now\t[%u]", uid); \
				return send_to_self_error(p, p->waitcmd, cli_err_self_chalk_other, 1); \
			} \
		} while (0)
#define CHECK_OTHER_BATTLE_INFO(p, lp) \
		do { \
			if (lp->btr_info && lp->btr_info->state != bt_state_init) { \
				KERROR_LOG(p->id, "challegee fighting now\t[%u]", lp->id); \
				return send_to_self_error(p, p->waitcmd, cli_err_other_be_fighting, 1); \
			} else if (lp->btr_info) { \
				KERROR_LOG(p->id, "ee challenge now\t[%u]", lp->id); \
				return send_to_self_error(p, p->waitcmd, cli_err_other_chalk_other, 1); \
			} \
		} while (0)
#define CHECK_SELF_TEAM_LEADER(p, uid, can_be_self) \
		do { \
			if ((!can_be_self && !p->btr_team) || (p->btr_team && !TEAM_LEADER(p))) {	\
				KERROR_LOG(p->id, "not team leader\t[%u]", uid); \
				return send_to_self_error(p, p->waitcmd, cli_err_self_not_leader, 1); \
			} \
		} while (0)

#define CHECK_SELF_JOIN_TEAM(p) \
		do { \
			if (p->btr_team) {	\
				KERROR_LOG(p->id, "already join team"); \
				return send_to_self_error(p, p->waitcmd, cli_err_alrdy_join_team, 1); \
			} \
		} while (0)

#define CHECK_TEAM_FIGHT_TIME(p) \
		do { \
			uint32_t timespan = get_now_tv()->tv_sec - p->last_fight_time; \
			if (timespan < TIME_SPAN_FOR_MONSTER) { \
				KERROR_LOG(p->id, "fight too often\t[%u %u]", p->last_fight_time, timespan); \
				return send_to_self_error(p, p->waitcmd, cli_err_fight_too_often, 1); \
			} \
		} while (0)

#define CHECK_USER_APPLY_PK(lp, p) \
		do { \
			if (lp->team_state >= apply_for_pk && lp->team_state <= apply_for_copy) { \
				return send_to_self_error(p, p->waitcmd, cli_err_applying_pk, 1); \
			} \
		} while (0)

#define CHECK_USER_HANGUP(p) \
		do { \
			if (p->seatid) { \
				KERROR_LOG(p->id, "hanging"); \
				return send_to_self_error(p, p->waitcmd, cli_err_hangup, 1); \
			} \
		} while (0)

	
#define CHECK_USER_IN_BATTLE(p, info) \
		do { \
			if (!p->btr_info) { \
				KERROR_LOG(p->id, "not challenge\t[%u]", info); \
				return send_to_self_error(p, p->waitcmd, cli_err_not_be_fighting, 1); \
			} \
		} while (0)

#define CHECK_TEAM_POS_VALID(p_, teamid, pos) \
	do { \
		if ((teamid != 0 && teamid != 1) || !(pos >= 0 && pos < 2 * MAX_PLAYERS_PER_TEAM)) { \
			return send_to_self_error(p_, p_->waitcmd, cli_err_team_pos_invalid, 1); \
		} \
	} while (0)

#define CHECK_TEAM_WAIT_ANIMATION(p_) \
	do { \
		if ((p_)->btr_team && TEAM_LEADER(p_)) \
			for (int loop = 0; loop < (p_)->btr_team->count; loop ++) { \
				sprite_t* s = (p_)->btr_team->players[loop]; \
				if (s && s != (p_) && s->team_state == wait_animation) { \
					KERROR_LOG((p_)->id, "wait animation\t[%u]", s->id); \
					return send_to_self_error((p_), (p_)->waitcmd, cli_err_team_wait_animation, 1); \
				} \
			} \
	} while(0)

#define CHECK_MAP_BEAST_DARK(p_) \
	do { \
		if ((p_)->tiles->check) { \
			KERROR_LOG((p_)->id, "beast not sure\t"); \
			return -1; \
		} \
	} while (0)

#define CHECK_HONOR_SATISFY(p_, honor_) \
	do { \
		if ((honor_) && (p_)->btr_team) { \
			for (int loop = 0; loop < (p_)->btr_team->count; loop ++) { \
				sprite_t* s = (p_)->btr_team->players[loop]; \
				if (!check_honor_exist(s, honor_)) { \
					KERROR_LOG((p_)->id, "honor not exsit\t[%u %u]", s->id, honor_); \
					return send_to_self_error((p_), (p_)->waitcmd, cli_err_honor_not_existed, 1); \
				} \
			} \
		} else if ((honor_) && !check_honor_exist(p_, honor_)) { \
			KERROR_LOG((p_)->id, "honor not exsit\t[%u %u]", (p_)->id, honor_); \
			return send_to_self_error((p_), (p_)->waitcmd, cli_err_honor_not_existed, 1); \
		} \
	} while (0)

#define MAX_PKPT_DAY(p_) (pow(p_->level, 5.0/6) * 18 + 0.5)	


void init_batter_teams();
void init_batter_infos();
batter_team_t* alloc_batter_team(sprite_t* p);
void change_batter_team_id(batter_team_t* pt, uint32_t new_temid);

int add_mbr_to_btr_team_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int apply_for_join_btr_team_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int leave_btr_team_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int kick_outof_btr_team_cmd(sprite_t *p, uint8_t *body, uint32_t len);
void notify_other_leader_challenging_state(sprite_t* challengee, battle_id_t btid, uint32_t state);
void notify_self_team_challenging(sprite_t *challenger, userid_t challengee_id);
int challenge_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int challenge_npc_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int accept_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int reject_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int cancel_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int load_battle_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int load_battle_result_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int round_info_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int battle_attack_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int battle_use_medical_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int battle_use_beast_card_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int battle_swap_pet_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int notify_teamleader_action_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int animation_finish_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int change_team_leader_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int apply_pk_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int cancel_pk_apply_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_last_kill_beastgrp_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int register_for_team_cmd(sprite_t *p, uint8_t *body, uint32_t len);
uint32_t get_avg_level(sprite_t* p) ;

void free_batter_team(void* pt);
void free_batter_info(void* pbi);
void del_user_battle_team(sprite_t* p);
void del_user_battle_info(sprite_t* p);
void noti_batsvr_user_offline(sprite_t* p);
int handle_batrserv_pkg(batrserv_proto_t* btrpkg, int len);
void free_batter_team_by_id(uint32_t teamid);
int kick_users_out_battle(int btidx);
void response_team_info(sprite_t* p);
void notify_teamers_enter_map(sprite_t* p, map_id_t mapid, uint16_t x, uint16_t y);
void notify_teamers_enter_map_done(sprite_t* p);
void notify_btrteam_state(battle_t* pb);
void notify_team_state(batter_team_t* pbt);

int kick_out_team_animation(void* owner, void* data);
int load_holiday_factor(xmlNodePtr cur);
uint32_t team_last_kill_beastgrp(sprite_t* p);
uint32_t get_exp_factor(sprite_t* p);

void init_challenger_battle(sprite_t* team_leader, int pkapply, int battletype);
void init_challengee_battle(sprite_t* team_leader, battle_id_t btid, int pkapply);
void batrserv_challenge_battle(sprite_t* challenger, userid_t challengee_id, uint32_t type, int cmd);
void batrserv_accept_battle(sprite_t* challengee);
int team_busy(sprite_t* p);

int add_user_to_watchers(sprite_t *p,uint32_t watcher);
int rm_user_from_watchers(sprite_t *p,uint32_t watcher);
int cancel_watch_ex(sprite_t* p);

static inline void send_to_battle(battle_t* abt, uint8_t* buffer, int len)
{
	for (int i = 0; i < abt->count; i++) {
		if (abt->players[i] && abt->players[i]->id > BEGIN_USER_ID && abt->players[i]->team_state != load_bt_fail)
			send_to_self(abt->players[i], buffer, len, 0);
	}
}

//发给战斗的观战者
static inline void send_to_battle_watchers(battle_t* abt, uint8_t* buffer, int len)
{
	std::vector<uint32_t>::iterator it; 
    for(it =abt->watchers->begin(); it != abt->watchers->end(); ){
		sprite_t *p=get_sprite(*it);
		if(p){
			KDEBUG_LOG(0,"battleinfo send to rcver=%u",p->id);
			send_to_self(p, buffer, len, 0);
			it++;
		}else{//用户下线了就把他从观战中去除
			it=abt->watchers->erase(it);
		}
	}
}

static inline void send_to_battle_except_uid(battle_t* abt, uint8_t* buffer, int len, uint32_t uid)
{
	for (int i = 0; i < abt->count; i++) {
		if (abt->players[i] && abt->players[i]->id != uid)
			send_to_self(abt->players[i], buffer, len, 0);
	}
}

static inline void send_to_btrtem(batter_team_t* ab, uint8_t* buffer, int len)
{
	for (int i = 0; i < ab->count; i++) {
		if (ab->players[i]) {
			send_to_self(ab->players[i], buffer, len, 0);
		}
	}
}

static inline int send_to_team(sprite_t* p, uint8_t* buffer, int len, int completed)
{
	if (p->btr_team) {
		send_to_btrtem(p->btr_team, buffer, len);
	} else {
		send_to_self(p, buffer, len, 0);
	}
	if (completed) {
		p->waitcmd = 0;
	}
	return 0;
}

static inline int send_msg_to_team(sprite_t* p, Cmessage* buffer,  int completed)
{
	if (p->btr_team) {
		for (int i = 0; i < p->btr_team->count; i++) {
			if (p->btr_team->players[i]) {
				send_msg_to_self(p->btr_team->players[i],p->waitcmd, buffer , completed);
			}
		}
	} else {
		send_msg_to_self(p,p->waitcmd, buffer,  completed);
	}
	return 0;
}

static inline void send_to_btrtem_except_uid(batter_team_t* ab, uint8_t* buffer, int len, uint32_t uid)
{
	int i;
	for (i = 0; i < ab->count; i++) {
		if (ab->players[i] && ab->players[i]->id != uid) {
			send_to_self(ab->players[i], buffer, len, 0);
		}
	}
}

static inline int get_user_in_bt(batter_team_t* pt, userid_t id)
{
	int idx;
	if(!pt)
		return -1;
	for (idx = 0; idx < pt->count; idx++) {
		if (pt->players[idx]->id == id)
			return idx;
	}

	return -1;
}

static inline int add_mbr_to_bt(sprite_t* adder, sprite_t* addee)
{
	if(!IS_SPRITE_VISIBLE(adder) || !IS_SPRITE_VISIBLE(addee)) return -1;

	batter_team_t* pt = adder->btr_team;
	if (!pt) {
		pt = alloc_batter_team(adder);
	}
	int indx = get_user_in_bt(pt, addee->id);
	if (indx != -1) {
		addee->btr_team = pt;
		ERROR_LOG("alrdy in team\t[%u %u]", pt->teamid, addee->id);
		return -1;
	}
	
	pt->players[pt->count] = addee;
	pt->count++;
	addee->btr_team = pt;

	if (pt->count == 5) {
		KDEBUG_LOG(adder->id, "FIVE MBR TEAM\t[id1=%u, ip1=%u, id2=%u, ip2=%u, id3=%u, ip3=%u, id4=%u, ip4=%u, id5=%u, ip5=%u]", \
			pt->players[0]->id, get_cli_ip(pt->players[0]->fdsess), \
			pt->players[1]->id, get_cli_ip(pt->players[1]->fdsess), \
			pt->players[2]->id, get_cli_ip(pt->players[2]->fdsess), \
			pt->players[3]->id, get_cli_ip(pt->players[3]->fdsess), \
			pt->players[4]->id, get_cli_ip(pt->players[4]->fdsess));
	}

	response_team_info(adder);
	return 0;
}

static inline void notify_batsvr_leave_team(sprite_t* p)
{
	KDEBUG_LOG(p->id, "1347 TO BTLSVR\t[%u %u]", high32_val(p->btr_info->battle_id), low32_val(p->btr_info->battle_id));
	send_to_batrserv(p, bt_battle_info_msg, 0, NULL, p->id, p->btr_info->battle_id, proto_cli_battle_leave_team);
}

static inline int batrserv_cancel_battle(sprite_t* p)
{	
	return send_to_batrserv(p, bt_battle_info_msg, 0, NULL, p->id, p->btr_info->battle_id, proto_cli_cancel_challenge);
}

static inline void cancel_battle_state(sprite_t* p)
{
	p->team_state = 0;
	KDEBUG_LOG(p->id, "CANCEL BATTLE WHEN LEAVE TEAM");
	p->busy_state = be_idle;
	notify_busy_state_update(p, sns_state, p->busy_state);
}

static inline int del_mbr_from_bt(batter_team_t* pt, userid_t id)
{
	int indx = get_user_in_bt(pt, id), loop;
	if (indx == -1) {
		return -1;
	}
	pt->count--;
	pt->players[indx]->btr_team = NULL;
	KDEBUG_LOG(pt->players[indx]->id, "MBR DEL STATE\t[%u %u]", pt->players[indx]->busy_state, pt->players[indx]->team_state);

	sprite_t* pl = get_sprite(pt->teamid);
	if (pl && pt->players[1]) {
		KDEBUG_LOG(id, "SET TEAMATE BOX INFO\t[%u]", pl->boxid);
		pt->players[1]->boxid = pl->boxid;
		pl->boxid = 0;
	}
	
	for (loop = indx; loop < pt->count; loop ++) {
		pt->players[loop] = pt->players[loop + 1];
	}
	pt->players[pt->count] = NULL;

	if (pt->count == 1) {
		KDEBUG_LOG(pt->teamid, "DEL TEAM\t[%u]", id);
		response_proto_uint32(pt->players[0], proto_cli_delete_batr_team, pt->teamid, 0, 1);
		free_batter_team_by_id(pt->teamid);
		free_batter_team(pt);
	} else {
		if (pt->teamid == id) {
			KDEBUG_LOG(pt->teamid, "CHAG TEAM\t[%u]", pt->players[0]->id);
			response_proto_uint32(pt->players[0], proto_cli_delete_batr_team, pt->teamid, 0, 1);
			change_batter_team_id(pt, pt->players[0]->id);
		} 
		response_team_info(pt->players[0]);
	}
	return 0;
}

static inline int
db_update_pkinfo(sprite_t* p, uint32_t uid, uint32_t pkpt_total, uint32_t pkpt_day, int wintype)
{
	int i = 0;
	uint8_t out[64];
	PKG_H_UINT32(out, pkpt_total, i);
	PKG_H_UINT32(out, pkpt_day, i);
	PKG_H_UINT32(out, get_now_tv()->tv_sec, i);
	PKG_H_UINT32(out, (wintype == 1), i);
	PKG_H_UINT32(out, 0, i);
	PKG_H_UINT32(out, (wintype == 0), i);
	PKG_H_UINT32(out, 0, i);

	return send_request_to_db(p, uid, proto_db_update_pk_info, out, i);
}

static inline int
db_set_winbossid(sprite_t* p, uint32_t uid, uint32_t bossid)
{
	return send_request_to_db(p, uid, proto_db_set_winbossid, &bossid, 4);
}

static inline int
db_set_task_svr_buf(sprite_t* p, uint32_t uid, task_db_t* ptr)
{
	uint8_t out[4 + TASK_SVR_BUF_LEN];
	int j = 0;
	PKG_H_UINT32(out, ptr->taskid, j);
	PKG_STR(out, ptr->svr_buf, j, TASK_SVR_BUF_LEN);
	return send_request_to_db(p, uid, proto_db_set_task_svr_buf, out, j);
}

#endif
