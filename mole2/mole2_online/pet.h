#ifndef MOLE2_PET_H
#define MOLE2_PET_H

#include "benchapi.h"
#include "dbproxy.h"
#include "util.h"
#include "proto/mole2_db.h"

enum pet_state{
	in_warehouse	= 0,
	just_in_bag		= 1,
	on_standby		= 2,
	rdy_fight		= 3,
	for_assist		= 4,
	pet_state_max	= 5,
	db_warehouse	= 10000,
	pet_del_state	= 2000000,
};

#define MAGIC_GROW_RATE	24	
#define IS_PET_RDY_FIGHT(state) ((state) == rdy_fight)
#define IS_PET_STANDBY(state) ((state) == rdy_fight || (state) == on_standby)
#define MAX_PET_STANDBY			3

enum {
	pet_follow_flag				= 0x04,
	pet_1_lv_flag				= 0x08,
	pet_catch_flag				= 0x10,
	pet_talent_flag				= 0x20,
	beast_elite_flag			= 0x40,
	beast_boss_flag				= 0x80,
	pet_add_attr				= 0x100,
};

enum {
	rand_diff_default			= 8,
	rand_base_default			= 11,
};

#define CHECK_PET_INBAG(p, pp, petid) \
		do { \
			if (!pp) { \
				KERROR_LOG(p->id, "pet not in bag\t[%u]", petid); \
				return send_to_self_error(p, p->waitcmd, cli_err_pet_not_inbag, 1); \
			} \
		} while (0)

#define CHECK_PET_LV_FIT(p_, pp_, minlv_, maxlv_) \
		do { \
			if ((pp_)->level < (minlv_) || (pp_)->level > (maxlv_)) { \
				KERROR_LOG((p_)->id, "lv not fit[%u %u %u]", (pp_)->level, minlv_, maxlv_); \
				return send_to_self_error((p_), (p_)->waitcmd, cli_err_level_not_fit, 1); \
			} \
		} while (0)

typedef struct pet_db {
	uint32_t			petid;
	uint32_t			pettype;
	uint8_t 			race;
	uint32_t			flag;
	char				nick[USER_NICK_LEN];
	uint32_t 			level;
	uint32_t 			experience;
	pet_attr_db_t		base_attr_db;
	uint16_t			attr_addition;
	uint32_t			hp;
	uint32_t			mp;
	uint8_t 			earth;
	uint8_t 			water;
	uint8_t 			fire;
	uint8_t 			wind;
	uint32_t			injury_lv;
	uint32_t			action_state;

	uint32_t			skill_cnt;
	skill_t				skills[];
}__attribute__((packed))pet_db_t;

int load_beasts(xmlNodePtr cur);
int load_pet_exchange(xmlNodePtr cur);
int load_pet_exchange_egg(xmlNodePtr cur);
//int load_g_book_gift_pet(xmlNodePtr cur);
void calc_pet_second_level_attr(pet_t* p);
beast_t* get_beast(uint32_t beastid);
beast_lvinfo_t* get_beast_lvinfo(uint32_t beastid, uint32_t lv);
pet_exchange_t* get_pet_exchange_info(uint32_t id);
int set_pet_state_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_pet_state_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int del_pet_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_pet_nick_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_pet_nick_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int set_pet_follow_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int set_pet_follow_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

int get_all_petcnt_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_pet_info_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_pet_info_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_other_pet_info_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_pet_list_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_pet_list_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_user_pet_list_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_pet_inhouse_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_pet_inhouse_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int update_pet_location_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int cure_pet_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int cure_pet_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int cure_pet_injury_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int cure_pet_injury_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int add_pet_base_attr_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int add_pet_base_attr_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int exchange_pet_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int exchange_pet_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int get_g_book_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_g_book_xiaomee_cmd(sprite_t *p, uint8_t *body, uint32_t len);

int pkg_pet_info(pet_t* p_p, uint8_t* buf);
void gen_pet_attr(pet_db_t* p_pd, beast_t* pb, uint32_t level, int growth, uint32_t rand_diff);
int db_add_pet_exp_level(sprite_t* p, pet_t* pp);
int user_add_pet(sprite_t* p, uint32_t pettype, uint32_t level, int must_inbag, int growth, uint32_t rand_diff);
void init_pet_skills(pet_db_t* ppd, beast_t* pba);

void calc_pet_5attr(pet_t* pet);
void notify_pet_follow(sprite_t* p, int complet);
int update_pet_from_db(sprite_t* p, pet_t *p_p, pet_db_t* p_pdb, bool resetskill);
int db_set_pet_flag(sprite_t* p,pet_t *p_p);

static inline int db_set_pet_state(sprite_t* p, uint32_t id, uint32_t petid, uint32_t state) {
	uint8_t out[8];
	int i = 0;
	PKG_H_UINT32(out, petid, i);
	PKG_H_UINT32(out, state, i);
	return send_request_to_db(p, id, proto_db_pet_set_state, out, i);
}

static inline int db_set_pet_injury(sprite_t* p, uint32_t id, uint32_t petid, uint32_t injury_lv)
{
	uint32_t out[2];
	out[0] = petid;
	out[1] = injury_lv;
	return send_request_to_db(p, id, proto_db_pet_set_injury, out, 8);
}

static inline void log_pet_all(uint32_t uid, pet_t* p_pet)
{
	KDEBUG_LOG(uid, "****************************pet_all info begin************************");
	KDEBUG_LOG(uid, "[petid=%u pettype=%u race=%u flag=%u nick=\"%s\" level=%u exp=%u] [attr_add: %u]", \
			p_pet->petid, p_pet->pettype, p_pet->race, p_pet->flag, p_pet->nick, p_pet->level, \
			p_pet->experience, p_pet->attr_addition);
	KDEBUG_LOG(uid, "[hp=%u mp=%u] [FIVE: %u %u %u %u %u] [FOUR: %u %u %u %u]", \
			p_pet->hp, p_pet->mp, p_pet->physi100, p_pet->stren100, p_pet->endur100, p_pet->quick100, p_pet->intel100, \
			p_pet->earth, p_pet->water, p_pet->fire, p_pet->wind);
	KDEBUG_LOG(uid, "[hp_max=%u mp_max=%u] [atk=%d def=%d speed=%d spirit=%d resume=%d hit=%d avoid=%d bisha=%d fback=%d]", \
			p_pet->hp_max, p_pet->mp_max, p_pet->attack, p_pet->defense, p_pet->speed, p_pet->spirit, \
			p_pet->resume, p_pet->hit_rate, p_pet->avoid_rate, p_pet->bisha, p_pet->fight_back);
	KDEBUG_LOG(uid, "[skills: %u (%u %u) (%u %u) (%u %u) (%u %u) (%u %u) (%u %u) (%u %u) (%u %u) (%u %u) (%u %u)]", \
			p_pet->skill_cnt, p_pet->skills[0].skill_id, p_pet->skills[0].skill_level, \
			p_pet->skills[1].skill_id, p_pet->skills[1].skill_level, \
			p_pet->skills[2].skill_id, p_pet->skills[2].skill_level, \
			p_pet->skills[3].skill_id, p_pet->skills[3].skill_level, \
			p_pet->skills[4].skill_id, p_pet->skills[4].skill_level, \
			p_pet->skills[5].skill_id, p_pet->skills[5].skill_level, \
			p_pet->skills[6].skill_id, p_pet->skills[6].skill_level, \
			p_pet->skills[7].skill_id, p_pet->skills[7].skill_level, \
			p_pet->skills[8].skill_id, p_pet->skills[8].skill_level, \
			p_pet->skills[9].skill_id, p_pet->skills[9].skill_level);
	KDEBUG_LOG(uid, "****************************pet_all info end**************************");
}

static inline pet_t* get_pet_inbag(sprite_t *p, uint32_t petid)
{
	int loop = 0;
	for (loop = 0; loop < p->pet_cnt; loop ++) {
		if (p->pets_inbag[loop].petid == petid)
			return &p->pets_inbag[loop];
	}
	return NULL;
}

static inline bool check_pet_inhouse(sprite_t *p, uint32_t petid)
{
	std::map<uint32_t,pet_simple_t>::iterator it=p->pets_inhouse->find(petid);
	if(it ==  p->pets_inhouse->end()){
		return false;
	}
	return true;	
}

static inline int del_pet_inhouse(sprite_t *p, uint32_t petid)
{
	KDEBUG_LOG(p->id,"del pet :%u",petid);
	std::map<uint32_t,pet_simple_t>::iterator it=p->pets_inhouse->find(petid);
	if(it != p->pets_inhouse->end()){
		p->pets_inhouse->erase(it);
	}
    db_set_pet_state(NULL, p->id, petid, get_now_tv()->tv_sec);
	KDEBUG_LOG(p->id,"del pet :%u",petid);
	return 0;	
}

static inline pet_t* get_pet_rdy_fight(sprite_t* p)
{
	for (int loop = 0; loop < p->pet_cnt; loop ++) {
		if (p->pets_inbag[loop].action_state == rdy_fight)
			return &p->pets_inbag[loop];
	}
	return NULL;
}

static inline void update_pet_skill_level(sprite_t* p, pet_t* pp)
{
	for (int loop = 0; loop < pp->skill_cnt; loop ++) {
		if (pp->skills[loop].skill_exp) {
			uint32_t pre_level = pp->skills[loop].skill_level;
			pp->skills[loop].skill_level = pp->level / 10 + 1;
			if (pre_level != pp->skills[loop].skill_level && pp->skills[loop].default_lv != 0xFF) {
				pp->skills[loop].default_lv = 0xFF;
				mole2_set_skill_def_level_in db_in;
				db_in.petid = pp->petid;
				db_in.skillid = pp->skills[loop].skill_id;
				db_in.def_level = 0xFF;
				send_msg_to_db(NULL, p->id, mole2_set_skill_def_level_cmd, &db_in);
			}
		}
	}
}

static inline int add_pet_to_bag(sprite_t* p, pet_db_t* p_pdb)
{
	KDEBUG_LOG(p->id, "ADD PET TO BAG\t[%u]", p_pdb->petid);
	if (p_pdb->action_state == in_warehouse \
		|| p->pet_cnt >= MAX_PETS_PER_SPRITE \
		|| p_pdb->action_state >= db_warehouse ) {
		return -1;
	}

	pet_t* p_p = &p->pets_inbag[p->pet_cnt];
	p->pet_cnt ++;
	if (IS_PET_STANDBY(p_pdb->action_state))
		p->pet_cnt_standby ++;
	if (p_pdb->action_state == for_assist)
		p->pet_cnt_assist ++;
	p->pet_cnt_actual = p->pet_cnt;
	update_pet_from_db(p, p_p, p_pdb, true);
	return 0;
}

static inline int pkg_pet_follow(const pet_t* p_pet, uint8_t* buf)
{
	int i = 0;
	if (p_pet == NULL) {
		PKG_UINT32(buf, 0, i);
		PKG_UINT32(buf, 0, i);
		PKG_UINT8(buf, 0, i);
		PKG_STR(buf, "", i, USER_NICK_LEN);
		PKG_UINT32(buf, 0, i);
		return i;
	}
	
	PKG_UINT32(buf, p_pet->petid, i);
	PKG_UINT32(buf, p_pet->pettype, i);
	PKG_UINT8(buf, p_pet->race, i);
	PKG_STR(buf, p_pet->nick, i, USER_NICK_LEN);
	PKG_UINT32(buf, p_pet->level, i);

	return i;
}

static inline int db_set_pet_follow(sprite_t* p, uint32_t uid, uint32_t petid)
{
	int i = 0;
	uint8_t out[64];
	PKG_H_UINT32(out, petid, i);
	return send_request_to_db(p, uid, proto_db_pet_follow_set, out, i);
}


static inline int del_pet_from_bag(sprite_t* p, pet_t* p_p)
{
	p->pet_cnt --;
	if (IS_PET_STANDBY(p_p->action_state))
		p->pet_cnt_standby --;
	if (p_p->action_state == for_assist)
		p->pet_cnt_assist --;
	memcpy(p_p, &p->pets_inbag[p->pet_cnt], sizeof(pet_t));
	memset(&p->pets_inbag[p->pet_cnt], 0, sizeof(pet_t));
	p->pet_cnt_actual = p->pet_cnt;
	if (p->p_pet_follow == p_p) {
		p->p_pet_follow = NULL;
		notify_pet_follow(p, 0);
		db_set_pet_follow(0, p->id, 0);
	}
	return 0;
}

static inline int update_user_pets(sprite_t* p, uint8_t* buf, uint32_t buflen, uint32_t pet_cnt)
{
	uint32_t len = 0, rdy_fight_cnt = 0;

	p->pet_cnt = 0;
	p->pet_cnt_standby = 0;
	p->pet_cnt_assist = 0;
	//DEBUG_LOG("xxxxtttt %u",pet_cnt);
	for (uint32_t loop = 0; loop < pet_cnt; loop++) {
		CHECK_BODY_LEN_GE(buflen, uint32_t(len + sizeof(pet_db_t)));
		pet_db_t* ppd = (pet_db_t *)(buf + len);
		
		len += sizeof(pet_db_t);

		CHECK_BODY_LEN_GE(buflen, uint32_t(len + ppd->skill_cnt * sizeof(skill_t)));
		len += sizeof(skill_t) * ppd->skill_cnt;
		if((ppd->action_state == for_assist && p->pet_cnt_assist >= 2) || 
				(ppd->action_state == on_standby && p->pet_cnt_standby >= MAX_PET_STANDBY)){
            ppd->action_state = db_warehouse;
			db_set_pet_state(NULL, p->id, ppd->petid, db_warehouse);
			continue;
		}
		//check db info
		if (p->pet_cnt >= MAX_PETS_PER_SPRITE || ppd->action_state == in_warehouse \
			|| (IS_PET_STANDBY(ppd->action_state) && p->pet_cnt_standby >= MAX_PET_STANDBY))
			continue;

		//set pet in battle
		if (ppd->action_state == on_standby || ppd->action_state == rdy_fight)
			p->pet_cnt_standby ++;
		
		if(ppd->action_state == for_assist){
			p->pet_cnt_assist ++;
		}

        if (ppd->action_state == rdy_fight) {
            if (rdy_fight_cnt){
                ppd->action_state = on_standby;
				db_set_pet_state(NULL,p->id,ppd->petid,on_standby);
			}
            rdy_fight_cnt ++;
        }

		//set pet follow 
		if (ppd->flag & pet_follow_flag) {
			p->p_pet_follow = &p->pets_inbag[p->pet_cnt];
		}

		//copy data from pet_db_t  to  pet_t
		update_pet_from_db(p, &p->pets_inbag[p->pet_cnt], ppd, true);
		p->pet_cnt ++;
	}

	p->pet_cnt_actual = p->pet_cnt;
	CHECK_BODY_LEN(buflen, len);

	return 0;
}


static inline int update_inbag_pet(sprite_t *p, pet_db_t* p_pdb)
{
	pet_t* p_p = get_pet_inbag(p, p_pdb->petid);
	if (p_pdb->action_state > pet_del_state)
			p->all_petcnt --;
	if (!p_p) {
		return add_pet_to_bag(p, p_pdb);
	} else if (p_pdb->action_state == db_warehouse || p_pdb->action_state > pet_del_state) {
		return del_pet_from_bag(p, p_p);
	}
	if (!IS_PET_STANDBY(p_pdb->action_state) && IS_PET_STANDBY(p_p->action_state)) {
		p->pet_cnt_standby --;	
	} else if (IS_PET_STANDBY(p_pdb->action_state) && !IS_PET_STANDBY(p_p->action_state)) {
		p->pet_cnt_standby ++;
	}
	
	if (p_pdb->action_state != for_assist && p_p->action_state == for_assist) {
		p->pet_cnt_assist --;
	} else if (p_pdb->action_state == for_assist && p_p->action_state != for_assist) {
		p->pet_cnt_assist ++;
	}

	update_pet_from_db(p, p_p, p_pdb, true);
	return 0;
}

static inline void cache_del_pet_inbag(sprite_t* p, uint32_t petid)
{
	for (int loop = 0; loop < p->pet_cnt; loop ++) {
		if (p->pets_inbag[loop].petid == petid) {
			uint32_t petfollowed = p->p_pet_follow ? p->p_pet_follow->petid : 0;
			if (p->p_pet_follow && petfollowed == petid) {
				p->p_pet_follow = NULL;
				notify_pet_follow(p, 0);
			}
			
			p->pet_cnt --;
			p->pet_cnt_actual --;
			if (p->pets_inbag[loop].action_state == on_standby \
				|| p->pets_inbag[loop].action_state == rdy_fight)
				p->pet_cnt_standby --;
			if (loop < p->pet_cnt) {
				memcpy(&p->pets_inbag[loop], &p->pets_inbag[p->pet_cnt], sizeof(pet_t));
				if (p->pets_inbag[loop].petid == petfollowed)
					p->p_pet_follow = &p->pets_inbag[loop];
			}
			memset(&p->pets_inbag[p->pet_cnt], 0, sizeof(pet_t));
			return;
		}
	}
}

static inline int calc_pet_attr_para10(int factor, int rand_diff)
{
	int para10 = (factor + (rand() % 11 - rand_diff)) * 10;
	return (para10 < 0 ? 0 : para10);
}

static inline int calc_beast_attr_para10(int factor)
{
	int para10 = (factor + (rand() % 11 - 8)) * 10;
	return (para10 < 0 ? 0 : para10);
}


static inline int pet_flag_set(pet_t* p_pet, uint32_t flag)
{
	if (p_pet != NULL) {
		p_pet->flag |= flag;
	}
	return 0;
}

static inline int pet_flag_unset(pet_t* p_pet, uint32_t flag)
{
	if (p_pet != NULL) {
		p_pet->flag &= ~flag;
	}
	return 0;
}
static inline pet_t* get_pet_follow(sprite_t *p)
{
	int loop = 0;
	for (loop = 0; loop < p->pet_cnt; loop ++) {
		if ((p->pets_inbag[loop].flag & pet_follow_flag) == pet_follow_flag){
			return &p->pets_inbag[loop];
		}
	}
	return NULL;
}

static inline void
msg_log_bus_mole2_killbeast(uint32_t userid, char* nick, uint32_t win_boss_id, uint32_t get_beast_id)
{
	beast_t *pb;
	uint8_t out[128] = {0};
	bus_proto_t* pbp = (bus_proto_t *)out;

	int i = sizeof(bus_proto_t);
	PKG_H_UINT32(out, get_now_tv()->tv_sec, i);
	PKG_STR(out, nick, i, USER_NICK_LEN);
	pb = get_beast(win_boss_id);
	if(pb) {
		PKG_STR(out, pb->nick, i, USER_NICK_LEN);
	} else {
		i += USER_NICK_LEN;
	}
	pb = get_beast(get_beast_id);
	if(pb) {
		PKG_STR(out, pb->nick, i, USER_NICK_LEN);
	} else {
		i += USER_NICK_LEN;
	}
	PKG_H_UINT32(out, get_beast_id, i);
	pbp->pkglen = i;
	pbp->userid = userid;
	pbp->cmdid = 5003;
	pbp->version = 1;
	msglog(statistic_logfile, 1, get_now_tv()->tv_sec, out, i);
}

#endif 

