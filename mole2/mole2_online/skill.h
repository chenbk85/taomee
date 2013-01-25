#ifndef MOLE2_SKILL_H
#define MOLE2_SKILL_H

#include "benchapi.h"
#include "dbproxy.h"
#include "cli_proto.h"
#include "svr_proto.h"
#include "sprite.h"

#define PET_SKILL_BASE_ID		5001
#define PET_SKILL_MAX_ID		10000
#define PET_SPEC_SKILL_BASE_ID	20001
#define BATTLE_SKILL_BASE_ID	100
#define BATTLE_SKILL_MAX_ID		5000
#define LIFE_SKILL_BASE_ID		100001
#define LIFE_SKILL_MAX_ID		100100


#define IS_LIFE_SKILL(skill_id_) ((skill_id_) >= LIFE_SKILL_BASE_ID && (skill_id_) <= LIFE_SKILL_MAX_ID)
#define IS_BATTLE_SKILL(skill_id_) ((skill_id_) >= BATTLE_SKILL_BASE_ID && (skill_id_) <= BATTLE_SKILL_MAX_ID)
#define IS_NEW_PROF(prof) ((prof) >= prof_gedoushi && (prof)<= prof_wushushi)
int buy_skill_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int buy_skill_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int del_skill_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int del_skill_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

int buy_pet_skill_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int buy_pet_skill_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int del_pet_skill_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int del_pet_skill_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int unlock_pet_talent_skill_cmd(sprite_t *p, uint8_t *body, uint32_t len);

int get_skill_list_cmd(sprite_t *p, uint8_t *body, uint32_t len);

void init_all_skills();
void fini_all_skills();
int load_all_skills(xmlNodePtr cur);
skill_info_t* get_skill_info(uint32_t skillid);
uint32_t get_pet_skill_toplv(uint32_t skill_id);
pet_skill_p_t* get_pet_skill_price(uint32_t skillid, uint32_t lv);

int cache_update_skills(sprite_t* p, skill_t* apsi, uint32_t skill_cnt);
int cache_add_skill(sprite_t* p, skill_t* ps);
int cache_del_skill(sprite_t* p, uint32_t skill_id);
skill_t* cache_get_skill(sprite_t* p, uint32_t skill_id);
void cache_update_skill_level(sprite_t* p, skill_t* ps, skill_info_t* psi, uint32_t new_exp);
int db_del_pet_skill(sprite_t* p, uint32_t uid, uint32_t petid, uint32_t skillid);
int db_buy_pet_skill(sprite_t* p, uint32_t uid, pet_t* p_p, uint32_t skillid, uint32_t skilllv, int cost);
void add_skill_for_new_prof(sprite_t *p,uint32_t level);

static inline int db_update_skill_info(sprite_t* p, uint32_t id, skill_t* ps)
{
	uint8_t out[64];
	int i = 0;
	PKG_H_UINT32(out, 1, i);
	PKG_H_UINT32(out, ps->skill_id, i);
	PKG_H_UINT32(out, ps->skill_level, i);
	PKG_H_UINT32(out, ps->skill_exp, i);
	return send_request_to_db(p, id, proto_db_set_skill_lv, out, i);
}

static inline int db_set_default_skill_level(sprite_t* p, uint32_t id, uint32_t petid, uint32_t skillid, uint32_t defaultlv)
{
	mole2_set_skill_def_level_in db_in;
	db_in.petid = petid;
	db_in.skillid = skillid;
	db_in.def_level = defaultlv;
	return send_msg_to_db(p, id, mole2_set_skill_def_level_cmd, &db_in);
}

static inline int 
cache_add_pet_skill(sprite_t *p, pet_t* p_p, uint32_t skill_id, uint32_t skill_lv)
{
	for (int loop = 0; loop < p_p->skill_cnt; loop ++) {
		if (p_p->skills[loop].skill_id == skill_id) {
			p_p->skills[loop].skill_level = (skill_lv > p_p->skills[loop].skill_level) \
				? skill_lv : p_p->skills[loop].skill_level;
			p_p->skills[loop].default_lv = 0xFF;
			return 0;
		}
	}

	if (p_p->skill_cnt >= MAX_SKILL_BAR)
		return -1;

	p_p->skills[p_p->skill_cnt].skill_id = skill_id;
	p_p->skills[p_p->skill_cnt].skill_level = skill_lv;
	p_p->skills[p_p->skill_cnt].skill_exp = 0;
	p_p->skills[p_p->skill_cnt].default_lv = 0xFF;
	p_p->skill_cnt ++;

	return 0;
}


static inline int 
cache_del_pet_skill(pet_t* p_p, uint32_t skill_id)
{
	for (int loop = 0; loop < p_p->skill_cnt; loop ++) {
		if (p_p->skills[loop].skill_id == skill_id) {
			p_p->skill_cnt --;
			memcpy(&p_p->skills[loop], &p_p->skills[p_p->skill_cnt], sizeof(skill_t));
			memset(&p_p->skills[p_p->skill_cnt], 0, sizeof(skill_t));
			return 0;
		}
	}
	return 0;
}

static inline skill_t*
cache_get_pet_skill(pet_t* p_p, uint32_t skillid)
{
	for (int loop = 0; loop < p_p->skill_cnt; loop ++) {
		if (p_p->skills[loop].skill_id == skillid) {
			return &p_p->skills[loop];
		}
	}
	return NULL;
}

static inline int
response_user_add_skill(sprite_t* p, skill_t* ps, uint32_t cost,uint32_t complete)
{
	int i = sizeof(protocol_t);
	uint8_t out[64];
	PKG_UINT32(out, ps->skill_id, i);
	PKG_UINT8(out, ps->skill_level, i);
	PKG_UINT32(out, ps->skill_exp, i);
	PKG_UINT32(out, p->xiaomee, i);
	PKG_UINT32(out, cost, i);

	init_proto_head(out, p->waitcmd, i);
	return send_to_self(p, out, i, complete);
}

#endif
