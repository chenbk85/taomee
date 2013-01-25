#ifdef __cplusplus
extern "C"{
#endif
#include <fcntl.h>
#include <sys/mman.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
#include "libtaomee/project/stat_agent/msglog.h"
#include "utils.h"
/**************lua include file**********************/
#include <stdio.h>
#include <math.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
/**************lua include file**********************/
#ifdef __cplusplus
}
#endif

#include "bt_skill.h"
#include "bt_bat_check.h"
#include "global.h"
#include "beast_ai_api.h"


#define LUA_RETURN_NUMBER_1(l_, ret_) lua_pushnumber(l_, ret_); return 1

#define LUA_RETURN_NUMBER_2(l_, ret1_, ret2_) lua_pushnumber(l_, ret1_); lua_pushnumber(l_, ret2_); return 2

#define LUA_RETURN_NUMBER_4(l_, ret1_, ret2_, ret3_, ret4_) lua_pushnumber(l_, ret1_);lua_pushnumber(l_, ret2_);lua_pushnumber(l_, ret3_); lua_pushnumber(l_, ret4_); return 4

// check player has prop
// batid, pos, mark, propname
battle_info_t* check_lua_api_param(uint64_t batid, uint32_t mark, int32_t pos, battle_users_t** p_team)
{
	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("check_lua_api_param battle is not exist\t[%lu]", batid);
		return NULL;
	}
	
	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("check_lua_api_param param mark is not valid\t[%u]", mark);
		return NULL;
	}

	if (pos < 0 || pos >= (MAX_PLAYERS_PER_TEAM * 2)){
		ERROR_LOG("check_lua_api_param param pos is not valid\t[%d]", pos);
		return NULL;
	}
	
	warrior_t* lp = team->players_pos[pos];
	if (lp == NULL){
		ERROR_LOG("check_lua_api_param param pos is not valid\t[%d]", pos);
		return NULL;
	}

	if (p_team){
		*p_team = team;
	}

	return pbi;
}

// batid mark pos propname
int lua_chk_player_has_prop(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_chk_player_has_prop Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_chk_player_has_prop Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_chk_player_has_prop Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// propname
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_chk_player_has_prop Arg_4 must be number");
		lua_pushstring(L, "Arg_4 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int pos = (int)lua_tonumber(L, 3);
	uint32_t propname = (uint32_t)lua_tonumber(L, 4);

	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	switch(propname){
	case prop_hp:
	case prop_hp_max:
	case prop_mp:
	case prop_mp_max:
	case prop_weapon:
	case prop_prof:
	case prop_speed:
	case prop_level:
	case prop_typeid:
	case prop_injury:
	case prop_shield:
	case prop_spirit:
	case prop_resume:
	case prop_hit_rate:
	case prop_avoid_rate:
	case prop_bisha:
	case prop_fight_back:
	case prop_cloth_cnt:
	case prop_in_front:
	case prop_earth:
	case prop_water:
	case prop_fire:
	case prop_wind:
	case prop_attack_val:
	case prop_mattack_val:
	case prop_defense_val:
	case prop_mdefense_val:
	case prop_race:
	case prop_pet_cnt:
	case prop_catchable:
	case prop_handbooklv:
	case prop_pet_contract_lv:
	case prop_pet_state:
	case prop_id:
	case prop_pet_id:
	case prop_skill_count:
		LUA_RETURN_NUMBER_1(L, 1);
	}

	LUA_RETURN_NUMBER_1(L, 0);
}

// batid, mark, pos, propname
int lua_get_player_prop(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_prop Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_prop Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_player_prop Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// propname
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_get_player_prop Arg_4 must be number");
		lua_pushstring(L, "Arg_4 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int pos = (int)lua_tonumber(L, 3);
	uint32_t propname = (uint32_t)lua_tonumber(L, 4);

	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	warrior_t* lp = p_team->players_pos[pos];

	switch(propname){
	case prop_hp:
		LUA_RETURN_NUMBER_1(L, lp->hp);
	case prop_hp_max:
		LUA_RETURN_NUMBER_1(L, lp->hp_max);
	case prop_mp:
		LUA_RETURN_NUMBER_1(L, lp->mp);
	case prop_mp_max:
		LUA_RETURN_NUMBER_1(L, lp->mp_max);
	case prop_weapon:
		LUA_RETURN_NUMBER_1(L, lp->weapon_type);
	case prop_prof:
		LUA_RETURN_NUMBER_1(L, lp->prof);
	case prop_speed:
		LUA_RETURN_NUMBER_1(L, lp->speed);
	case prop_level:
		LUA_RETURN_NUMBER_1(L, lp->level);
	case prop_typeid:
		LUA_RETURN_NUMBER_1(L, lp->type_id);
	case prop_injury:
		LUA_RETURN_NUMBER_1(L, lp->injury_lv);
	case prop_shield:
		LUA_RETURN_NUMBER_1(L, lp->shield);
	case prop_spirit:
		LUA_RETURN_NUMBER_1(L, lp->spirit);
	case prop_resume:
		LUA_RETURN_NUMBER_1(L, lp->resume);
	case prop_hit_rate:
		LUA_RETURN_NUMBER_1(L, lp->hit_rate);
	case prop_avoid_rate:
		LUA_RETURN_NUMBER_1(L, lp->avoid_rate);
	case prop_bisha:
		LUA_RETURN_NUMBER_1(L, lp->bisha);
	case prop_fight_back:
		LUA_RETURN_NUMBER_1(L, lp->fight_back);
	case prop_cloth_cnt:
		LUA_RETURN_NUMBER_1(L, lp->cloth_cnt);
	case prop_in_front:
		LUA_RETURN_NUMBER_1(L, lp->in_front);
	case prop_earth:
		LUA_RETURN_NUMBER_1(L, lp->earth);
	case prop_water:
		LUA_RETURN_NUMBER_1(L, lp->water);
	case prop_fire:
		LUA_RETURN_NUMBER_1(L, lp->fire);
	case prop_wind:
		LUA_RETURN_NUMBER_1(L, lp->wind);
	case prop_attack_val:
		LUA_RETURN_NUMBER_1(L, lp->attack_value);
	case prop_mattack_val:
		LUA_RETURN_NUMBER_1(L, lp->mattack_value);
	case prop_defense_val:
		LUA_RETURN_NUMBER_1(L, lp->defense_value);
	case prop_mdefense_val:
		LUA_RETURN_NUMBER_1(L, lp->mdefense_value);
	case prop_race:
		LUA_RETURN_NUMBER_1(L, lp->race);
	case prop_pet_cnt:
		LUA_RETURN_NUMBER_1(L, lp->pet_cnt);
	case prop_catchable:
		LUA_RETURN_NUMBER_1(L, lp->catchable);
	case prop_handbooklv:
		LUA_RETURN_NUMBER_1(L, lp->handbooklv);
	case prop_pet_contract_lv:
		LUA_RETURN_NUMBER_1(L, lp->pet_contract_lv);
	case prop_pet_state:
		LUA_RETURN_NUMBER_1(L, lp->pet_state);
	case prop_id:
		LUA_RETURN_NUMBER_1(L, lp->userid);
	case prop_pet_id:
		LUA_RETURN_NUMBER_1(L, lp->petid);
	case prop_skill_count:
		LUA_RETURN_NUMBER_1(L, lp->skill_cnt);

	case prop_groupid:
		LUA_RETURN_NUMBER_1(L, lp->self_team->teamid);
	default:
		break;
	}

	LUA_RETURN_NUMBER_1(L, -1);
}

int lua_set_player_prop(lua_State* L)
{
#if 1
	LUA_RETURN_NUMBER_1(L, 0);
#else
// batid
if (!lua_isstring(L, 1)){
	ERROR_LOG("lua_set_player_prop Arg_1 must be string");
	lua_pushstring(L, "lua_set_player_prop Arg_1 must be number");
	lua_error(L);
}

// mark
if (!lua_isnumber(L, 2)){
	ERROR_LOG("lua_set_player_prop Arg_2 must be number");
	lua_pushstring(L, "lua_set_player_prop Arg_2 must be number");
	lua_error(L);
}

// pos
if (!lua_isnumber(L, 3)){
	ERROR_LOG("lua_set_player_prop Arg_3 must be number");
	lua_pushstring(L, "lua_set_player_prop Arg_3 must be number");
	lua_error(L);
}

// propname
if (!lua_isnumber(L, 4)){
	ERROR_LOG("lua_set_player_prop Arg_4 must be number");
	lua_pushstring(L, "lua_set_player_prop Arg_4 must be number");
	lua_error(L);
}

// propvalue
if (!lua_isnumber(L, 5)){
	ERROR_LOG("lua_set_player_prop Arg_5 must be number");
	lua_pushstring(L, "lua_set_player_prop Arg_5 must be number");
	lua_error(L);
}

uint64_t batid = atoll(lua_tostring(L, 1));
uint32_t mark = (uint32_t)lua_tonumber(L, 2);
int pos = (int)lua_tonumber(L, 3);
uint32_t propname = (uint32_t)lua_tonumber(L, 4);

battle_users_t* p_team = NULL;
battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
if (pbi == NULL){
	LUA_RETURN_NUMBER_1(L, -1);
}

warrior_t* lp = p_team->players_pos[pos];

DEBUG_LOG("lua_set_player_prop\t[batid=%lu batobj=%p globalloadtimer=%p globalfightertimer=%p]", pbi->batid, pbi, pbi->load_timer, pbi->fight_timer);

int32_t val = (int32_t)lua_tonumber(L, 5);
switch(propname){
	case prop_hp: { lp->hp = val; } break;
	case prop_hp_max: { lp->hp_max= val; } break;
	case prop_mp: lp->mp = val; break;
	case prop_mp_max: lp->mp_max = val; break;
	case prop_weapon: lp->weapon_type = val; break;
	case prop_prof: lp->prof = val; break;
	case prop_speed: lp->speed = val; break;
	case prop_level: lp->level = val; break;
	case prop_typeid: lp->typeid = val; break;
	case prop_injury: lp->injury_lv = val; break;
	case prop_shield: lp->shield = val; break;
	case prop_spirit: lp->spirit = val; break;
	case prop_resume: lp->resume = val; break;
	case prop_hit_rate: lp->hit_rate = val; break;
	case prop_avoid_rate: lp->avoid_rate = val; break;
	case prop_bisha: lp->bisha = val; break;
	case prop_fight_back: lp->fight_back = val; break;
	case prop_cloth_cnt: lp->cloth_cnt = val; break;
	case prop_in_front: lp->in_front = val; break;
	case prop_earth: lp->earth = val; break;
	case prop_water: lp->water = val; break;
	case prop_fire: lp->fire = val; break;
	case prop_wind: lp->wind = val; break;
	case prop_attack_val: lp->attack_value = val; break;
	case prop_mattack_val: lp->mattack_value = val; break;
	case prop_defense_val: lp->defense_value = val; break;
	case prop_mdefense_val: lp->mdefense_value = val; break;
	case prop_race: lp->race = val; break;
	case prop_pet_cnt: lp->pet_cnt = val; break;
	case prop_catchable: lp->catchable = val; break;
	case prop_handbooklv: lp->handbooklv = val; break;
	case prop_pet_contract_lv: lp->pet_contract_lv = val; break;
	case prop_pet_state: lp->pet_state = val; break;
	case prop_id: lp->userid = val; break;
	case prop_pet_id: lp->petid = val; break;
	case prop_skill_count: lp->skill_cnt = val; break;
	default:
		LUA_RETURN_NUMBER_1(L, -1);
}

LUA_RETURN_NUMBER_1(L, 0);
#endif
}


// batid, mark, pos, state
int lua_chk_player_state(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_chk_player_state Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_chk_player_state Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_chk_player_state Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// state
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_chk_player_state Arg_4 must be number");
		lua_pushstring(L, "Arg_4 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int pos = (int)lua_tonumber(L, 3);
	uint32_t state = (uint32_t)lua_tonumber(L, 4);

	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	warrior_t* lp = p_team->players_pos[pos];
	if (lp->check_state( state)){
		LUA_RETURN_NUMBER_1(L, 1);
	}

	LUA_RETURN_NUMBER_1(L, 0);
}

// batid, mark, pos, state, on_off
int lua_reset_player_state(lua_State* L)
{
#if 1
	DEBUG_LOG("lua_reset_player_state");
#else
// batid
if (!lua_isstring(L, 1)){
	ERROR_LOG("lua_reset_player_state Arg_1 must be string");
	lua_pushstring(L, "lua_reset_player_state Arg_1 must be number");
	lua_error(L);
}

// mark
if (!lua_isnumber(L, 2)){
	ERROR_LOG("lua_reset_player_state Arg_2 must be number");
	lua_pushstring(L, "lua_reset_player_state Arg_2 must be number");
	lua_error(L);
}

// pos
if (!lua_isnumber(L, 3)){
	ERROR_LOG("lua_reset_player_state Arg_3 must be number");
	lua_pushstring(L, "lua_reset_player_state Arg_3 must be number");
	lua_error(L);
}

// state
if (!lua_isnumber(L, 4)){
	ERROR_LOG("lua_reset_player_state Arg_4 must be number");
	lua_pushstring(L, "lua_reset_player_state Arg_4 must be number");
	lua_error(L);
}

// on/off
if (!lua_isnumber(L, 5)){
	ERROR_LOG("lua_reset_player_state Arg_5 must be number");
	lua_pushstring(L, "lua_reset_player_state Arg_5 must be number");
	lua_error(L);
}

uint64_t batid = atoll(lua_tostring(L, 1));
uint32_t mark = (uint32_t)lua_tonumber(L, 2);
int pos = (int)lua_tonumber(L, 3);
uint32_t state = (uint32_t)lua_tonumber(L, 4);
uint32_t set_on = (uint32_t)lua_tonumber(L, 5);

battle_users_t* p_team = NULL;
battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
if (pbi == NULL){
	LUA_RETURN_NUMBER_1(L, -1);
}

DEBUG_LOG("lua_reset_player_state\t[batid=%lu batobj=%p globalloadtimer=%p globalfightertimer=%p]", pbi->batid, pbi, pbi->load_timer, pbi->fight_timer);

warrior_t* lp = p_team->players_pos[pos];
if (set_on){
	SET_WARRIOR_STATE(lp->p_waor_state->state, state);
}else{
	RESET_WARRIOR_STATE(lp->p_waor_state->state, state);
}
#endif
	LUA_RETURN_NUMBER_1(L, 1);
}

// batid, mark
int lua_get_player_highspeed(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_highspeed Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_highspeed Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("lua_get_player_highspeed battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("lua_get_player_highspeed param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	int i = 0;
	int16_t speed = -1;
	int32_t ret_pos = -1;
	for (i = 0; i < MAX_PLAYERS_PER_TEAM * 2; i++){
		warrior_t* lp = team->players_pos[i];
		if (!lp || NONEED_ATTACK(lp)){
			continue;
		}

		if (lp->speed > speed){
			speed = lp->speed;
			ret_pos = i;
		}
	}

	LUA_RETURN_NUMBER_1(L, ret_pos);
}

static battle_users_t* get_enemy_player_team(battle_info_t* abi, battle_users_t* self_team)
{
	if (self_team == &abi->challger){
		return &abi->challgee;
	}

	return &abi->challger;
}

// batid, mark
int lua_get_player_least_hp(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_least_hp Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// self_mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_least_hp Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// self_pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_player_least_hp Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// enemy_mark
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_get_player_least_hp Arg_4 must be number");
		lua_pushstring(L, "Arg_4 must be number");
		lua_error(L);
	}

	uint32_t near_distance = 0;
	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t self_mark = (uint32_t)lua_tonumber(L, 2);
	uint32_t self_pos = (uint32_t)lua_tonumber(L, 3);
	uint32_t enemy_mark = (uint32_t)lua_tonumber(L, 4);
	
	// distance
	if (lua_isnumber(L, 5)){
		near_distance = (uint32_t)lua_tonumber(L, 5);
	}
	
	battle_users_t* p_self_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, self_mark, self_pos, &p_self_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	if (self_pos >= 0 && self_pos < MAX_PLAYERS_PER_TEAM){
		near_distance = 0;
	}

	battle_users_t* p_enemy_team = get_enemy_player_team(pbi, p_self_team);	
	if (enemy_mark == self_mark){
		near_distance = 0;
		p_enemy_team = p_self_team;
	}

	int i = 0;
	int32_t hp = -1;
	int32_t ret_pos = -1;
	uint8_t distance = 0;
	for (i = 0; i < (MAX_PLAYERS_PER_TEAM * 2); i++){
		warrior_t* lp = p_enemy_team->players_pos[i];
		if (!lp || NONEED_ATTACK(lp)){
			continue;
		}

		if (i >= 0 && i < MAX_PLAYERS_PER_TEAM){
			if (hp == -1){
				hp = lp->hp;
				ret_pos = i;
			}else{
				if (lp->hp < hp){
					hp = lp->hp;
					ret_pos = i;
				}
			}
			continue;
		}

		distance = 0;
		if (near_distance){
			warrior_t* lp_front = p_self_team->players_pos[self_pos - MAX_PLAYERS_PER_TEAM];
			if (lp_front && !CANNOT_ATTACK(lp_front))
				distance++;

			warrior_t* lp_enemy_front = p_enemy_team->players_pos[i - MAX_PLAYERS_PER_TEAM];
			if (lp_enemy_front && !NONEED_ATTACK(lp_enemy_front))
				distance++;
		}

		if (distance == 2){
			continue;
		}

		if (hp == -1){
			hp = lp->hp;
			ret_pos = i;
		}else{
			if (lp->hp < hp){
				hp = lp->hp;
				ret_pos = i;
			}
		}
	}

	LUA_RETURN_NUMBER_1(L, ret_pos);
}

// batid, mark, self pos
// param 1: batid
// param 2: mark
// param 3: self_pos
// param 4: enemy_mark
// param 5: near_distance [optional]
int lua_get_player_most_hp(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_most_hp Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// self_mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_most_hp Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// self_pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_player_most_hp Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// enemy_mark
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_get_player_most_hp Arg_4 must be number");
		lua_pushstring(L, "Arg_4 must be number");
		lua_error(L);
	}

	uint32_t near_distance = 0;
	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t self_mark = (uint32_t)lua_tonumber(L, 2);
	uint32_t self_pos = (uint32_t)lua_tonumber(L, 3);
	uint32_t enemy_mark = (uint32_t)lua_tonumber(L, 4);
	
	// distance[optional]
	if (lua_isnumber(L, 5)){
		near_distance = (uint32_t)lua_tonumber(L, 5);
	}
	
	battle_users_t* p_self_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, self_mark, self_pos, &p_self_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	if (self_pos >= 0 && self_pos < MAX_PLAYERS_PER_TEAM){
		near_distance = 0;
	}

	battle_users_t* p_enemy_team = get_enemy_player_team(pbi, p_self_team);	
	if (enemy_mark == self_mark){
		near_distance = 0;
		p_enemy_team = p_self_team;
	}

	int i = 0;
	int32_t hp = -1;
	int32_t ret_pos = -1;
	uint8_t distance = 0;
	for (i = 0; i < (MAX_PLAYERS_PER_TEAM * 2); i++){
		warrior_t* lp = p_enemy_team->players_pos[i];
		if (!lp || NONEED_ATTACK(lp)){
			continue;
		}

		if (i >= 0 && i < MAX_PLAYERS_PER_TEAM){
			if (lp->hp > hp){
				hp = lp->hp;
				ret_pos = i;
			}
			continue;
		}

		distance = 0;
		if (near_distance){
			warrior_t* lp_front = p_self_team->players_pos[self_pos - MAX_PLAYERS_PER_TEAM];
			if (lp_front && !CANNOT_ATTACK(lp_front))
				distance++;

			warrior_t* lp_enemy_front = p_enemy_team->players_pos[i - MAX_PLAYERS_PER_TEAM];
			if (lp_enemy_front && !NONEED_ATTACK(lp_enemy_front))
				distance++;
		}

		if (distance == 2){
			continue;
		}

		if (lp->hp > hp){
			hp = lp->hp;
			ret_pos = i;
		}
	}

	LUA_RETURN_NUMBER_1(L, ret_pos);
}

// batid, mark, prof
int lua_get_player_except_prof_rand(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("Arg_1 must be string");
		lua_pushstring(L, "lua_get_player_except_prof_rand Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("Arg_2 must be number");
		lua_pushstring(L, "lua_get_player_except_prof_rand Arg_2 must be number");
		lua_error(L);
	}

	// prof
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("Arg_3 must be number");
		lua_pushstring(L, "lua_get_player_except_prof_rand Arg_3 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	uint32_t prof = (uint32_t)lua_tonumber(L, 3);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	switch(prof){
	case prof_ancher:
	case prof_churchman:
	case prof_magician:
	case prof_swordman:
	case prof_none:
		break;
	default:
		ERROR_LOG("lua_get_player_except_prof_rand param prof is not valid\t[%u]", prof);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	int pos[2 * MAX_PLAYERS_PER_TEAM] = {0};
	int count = 0;
	for (int i = 0; i < 2 * MAX_PLAYERS_PER_TEAM; i++){
		warrior_t* lp = team->players_pos[i];
		if (!lp || NONEED_ATTACK(lp)){
			continue;
		}

		if (lp->prof != prof){
			pos[count++] = i;
			continue;
		}

		uint32_t prof1;
		if (lua_isnumber(L, 4)){
			prof1 = (uint32_t)lua_tonumber(L, 4);
			if (lp->prof != prof1){
				pos[count++] = i;
				continue;
			}

			if (lua_isnumber(L, 5)){
				prof1 = (uint32_t)lua_tonumber(L, 5);
				if (lp->prof != prof1){
					pos[count++] = i;
					continue;
				}

				if (lua_isnumber(L, 6)){
					prof1 = (uint32_t)lua_tonumber(L, 6);
					if (lp->prof != prof1){
						pos[count++] = i;
						continue;
					}
				}
			}
		}
	}

	if (count == 0){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	LUA_RETURN_NUMBER_1(L, pos[rand() % count]);

}

// batid, mark, prof
int lua_get_player_by_prof_rand(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_by_prof_rand Arg_1 must be string");
		lua_pushstring(L, "lua_get_player_by_prof_rand Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_by_prof_rand Arg_2 must be number");
		lua_pushstring(L, "lua_get_player_by_prof_rand Arg_2 must be number");
		lua_error(L);
	}

	// prof
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_player_by_prof_rand Arg_3 must be number");
		lua_pushstring(L, "lua_get_player_by_prof_rand Arg_3 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	uint32_t prof = (uint32_t)lua_tonumber(L, 3);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("lua_get_player_by_prof_rand battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("lua_get_player_by_prof_rand param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	switch(prof){
	case prof_ancher:
	case prof_churchman:
	case prof_magician:
	case prof_swordman:
	case prof_none:
		break;
	default:
		ERROR_LOG("lua_get_player_by_prof_rand param prof is not valid\t[%u]", prof);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	int pos[2 * MAX_PLAYERS_PER_TEAM] = {0};
	int i = 0;
	int count = 0;
	for (i = 0; i < 2 * MAX_PLAYERS_PER_TEAM; i++){
		warrior_t* lp = team->players_pos[i];
		if (!lp || NONEED_ATTACK(lp)){
			continue;
		}

		if (lp->prof == prof){
			pos[count++] = i;
			continue;
		}

		uint32_t prof1;
		if (lua_isnumber(L, 4)){
			prof1 = (uint32_t)lua_tonumber(L, 4);
			if (lp->prof == prof1){
				pos[count++] = i;
				continue;
			}

			if (lua_isnumber(L, 5)){
				prof1 = (uint32_t)lua_tonumber(L, 5);
				if (lp->prof == prof1){
					pos[count++] = i;
					continue;
				}

				if (lua_isnumber(L, 6)){
					prof1 = (uint32_t)lua_tonumber(L, 6);
					if (lp->prof == prof1){
						pos[count++] = i;
						continue;
					}
				}
			}
		}
	}

	if (count == 0){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	LUA_RETURN_NUMBER_1(L, pos[rand() % count]);
}

// batid, mark, typeid
int lua_get_beast_by_typeid(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_beast_by_typeid Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_beast_by_typeid Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// typeid
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_beast_by_typeid Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	uint32_t type_id = (uint32_t)lua_tonumber(L, 3);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("lua_get_beast_by_typeid battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("lua_get_beast_by_typeid param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	int pos[2 * MAX_PLAYERS_PER_TEAM] = {0};
	int i = 0;
	int count = 0;
	for (i = 0; i < 2 * MAX_PLAYERS_PER_TEAM; i++){
		warrior_t* lp = team->players_pos[i];
		if (!lp){
			continue;
		}

		if (lp->type_id == type_id){
			pos[count++] = i;
		}
	}

	if (count == 0){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	LUA_RETURN_NUMBER_1(L, pos[rand() % count]);
}


// batid, mark, state
int lua_get_player_by_state_rand(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_by_state_rand Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_by_state_rand Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// state
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_player_by_state_rand Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	uint32_t state = (uint32_t)lua_tonumber(L, 3);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("lua_get_player_by_state_rand battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("lua_get_player_by_state_rand param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	int pos[2 * MAX_PLAYERS_PER_TEAM] = {0};
	int i = 0;
	int count = 0;
	for (i = 0; i < 2 * MAX_PLAYERS_PER_TEAM; i++){
		warrior_t* lp = team->players_pos[i];
		if (!lp){
			continue;
		}

		if (lp->check_state( state)){
			pos[count++] = i;
		}
	}

	if (count == 0){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	LUA_RETURN_NUMBER_1(L, pos[rand() % count]);
}

// batid, mark, state
int lua_get_player_by_rand(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_by_rand Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_by_rand Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("lua_get_player_by_rand battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("lua_get_player_by_rand param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	int pos[2 * MAX_PLAYERS_PER_TEAM] = {0};
	int i = 0;
	int count = 0;
	for (i = 0; i < 2 * MAX_PLAYERS_PER_TEAM; i++){
		warrior_t* lp = team->players_pos[i];
		if (!lp || NONEED_ATTACK(lp)){
			continue;
		}

		pos[count++] = i;
	}

	if (count == 0){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	LUA_RETURN_NUMBER_1(L, pos[rand() % count]);
}

int lua_get_player_by_rand_distance(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_by_rand_distance Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// self_mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_by_rand_distance Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// self_pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_player_by_rand_distance Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// enemy_mark
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_get_player_by_rand_distance Arg_4 must be number");
		lua_pushstring(L, "Arg_4 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t self_mark = (uint32_t)lua_tonumber(L, 2);
	uint32_t self_pos = (uint32_t)lua_tonumber(L, 3);
	uint32_t enemy_mark = (uint32_t)lua_tonumber(L, 4);
	uint32_t near_distance = 0;
	if (lua_isnumber(L, 5)){
		near_distance = (uint32_t)lua_tonumber(L, 5);
	}

	battle_users_t* p_self_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, self_mark, self_pos, &p_self_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	if (self_pos >= 0 && self_pos < MAX_PLAYERS_PER_TEAM){
		near_distance = 0;
	}

	battle_users_t* p_enemy_team = get_enemy_player_team(pbi, p_self_team);	
	if (enemy_mark == self_mark){
		near_distance = 0;
		p_enemy_team = p_self_team;
	}

	int pos[2 * MAX_PLAYERS_PER_TEAM] = {0};
	int i = 0;
	int count = 0;
	for (i = 0; i < 2 * MAX_PLAYERS_PER_TEAM; i++){
		warrior_t* lp = p_enemy_team->players_pos[i];
		if (!lp || NONEED_ATTACK(lp)){
			continue;
		}

		if (i >= 0 && i < MAX_PLAYERS_PER_TEAM){
			pos[count++] = i;
			continue;
		}

		if (near_distance == 0){
			pos[count++] = i;
			continue;
		}

		uint8_t disance = 0;
		warrior_t* lp_self_front = p_self_team->players_pos[self_pos - MAX_PLAYERS_PER_TEAM];
		if (!lp_self_front || CANNOT_ATTACK(lp_self_front)){
		}else{
			disance++;
		}
		
		warrior_t* lp_enemy_front = p_enemy_team->players_pos[i - MAX_PLAYERS_PER_TEAM];
		if (!lp_enemy_front || NONEED_ATTACK(lp_enemy_front)){
		}else{
			disance++;
		}

		if (disance != 2){
			pos[count++] = i;
		}
	}

	if (count == 0){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	LUA_RETURN_NUMBER_1(L, pos[rand() % count]);
}


// batid, mark
int lua_get_player_fangyu_rand_distance(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_fangyu_rand_distance Arg_1 must be string");
		lua_pushstring(L, "lua_get_player_fangyu_rand_distance Arg_1 must be number");
		lua_error(L);
	}

	// self_mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_fangyu_rand_distance Arg_2 must be number");
		lua_pushstring(L, "lua_get_player_fangyu_rand_distance Arg_2 must be number");
		lua_error(L);
	}

	// self_pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_player_fangyu_rand_distance Arg_3 must be number");
		lua_pushstring(L, "lua_get_player_fangyu_rand_distance Arg_3 must be number");
		lua_error(L);
	}

	// enemy_mark
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_get_player_by_rand_distance Arg_4 must be number");
		lua_pushstring(L, "lua_get_player_fangyu_rand_distance Arg_4 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t self_mark = (uint32_t)lua_tonumber(L, 2);
	uint32_t self_pos = (uint32_t)lua_tonumber(L, 3);
	uint32_t enemy_mark = (uint32_t)lua_tonumber(L, 4);
	uint32_t near_distance = 0;
	if (lua_isnumber(L, 5)){
		near_distance = (uint32_t)lua_tonumber(L, 5);
	}

	battle_users_t* p_self_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, self_mark, self_pos, &p_self_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_2(L, -1, -1);
	}

	warrior_t* lp_self = p_self_team->players_pos[self_pos];

	if (self_pos >= 0 && self_pos < MAX_PLAYERS_PER_TEAM){
		near_distance = 0;
	}

	battle_users_t* p_enemy_team = get_enemy_player_team(pbi, p_self_team);	
	if (enemy_mark == self_mark){
		near_distance = 0;
		p_enemy_team = p_self_team;
	}

	int pos[2 * MAX_PLAYERS_PER_TEAM] = {0};
	int i = 0;
	int count = 0;
	for (i = 0; i < 2 * MAX_PLAYERS_PER_TEAM; i++){
		warrior_t* lp = p_enemy_team->players_pos[i];
		if (!lp || NONEED_ATTACK(lp)){
			continue;
		}

		// first attack must set fangyu before battle
		if (lp->check_state( fangyu_bit) || 
			lp->check_state( huandun_bit)){
		}else{
			if (lp_self->speed < lp->speed){
				if (lp->atk_info[1].atk_type == skill_pd_pet_fangyu || 
					lp->atk_info[1].atk_type == skill_pd_pet_huandun){
				}else{
					continue;
				}
			}else{
				continue;
			}
		}

		if (i >= 0 && i < MAX_PLAYERS_PER_TEAM){
			pos[count++] = i;
			continue;
		}

		if (near_distance == 0){
			pos[count++] = i;
			continue;
		}

		uint8_t disance = 0;
		warrior_t* lp_self_front = p_self_team->players_pos[self_pos - MAX_PLAYERS_PER_TEAM];
		if (!lp_self_front || CANNOT_ATTACK(lp_self_front)){
		}else{
			disance++;
		}
		
		warrior_t* lp_enemy_front = p_enemy_team->players_pos[i - MAX_PLAYERS_PER_TEAM];
		if (!lp_enemy_front || NONEED_ATTACK(lp_enemy_front)){
		}else{
			disance++;
		}

		if (disance != 2){
			pos[count++] = i;
		}
	}

	if (count == 0){
		LUA_RETURN_NUMBER_2(L, 0, -1);
	}

	LUA_RETURN_NUMBER_2(L, count, pos[rand() % count]);
}


// batid self_mark self_pos enemy_mark enemy_pos
int lua_can_reach_distance(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_can_reach_distance Arg_1 must be string");
		lua_pushstring(L, "lua_can_reach_distance Arg_1 must be number");
		lua_error(L);
	}

	// self_mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_can_reach_distance Arg_2 must be number");
		lua_pushstring(L, "lua_can_reach_distance Arg_2 must be number");
		lua_error(L);
	}

	// self_pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_can_reach_distance Arg_3 must be number");
		lua_pushstring(L, "lua_can_reach_distance Arg_3 must be number");
		lua_error(L);
	}

	// enemy_mark
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_can_reach_distance Arg_4 must be number");
		lua_pushstring(L, "lua_can_reach_distance Arg_4 must be number");
		lua_error(L);
	}

	// enemy_pos
	if (!lua_isnumber(L, 5)){
		ERROR_LOG("lua_can_reach_distance Arg_5 must be number");
		lua_pushstring(L, "lua_can_reach_distance Arg_5 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t self_mark = (uint32_t)lua_tonumber(L, 2);
	uint32_t self_pos = (uint32_t)lua_tonumber(L, 3);
	uint32_t enemy_mark = (uint32_t)lua_tonumber(L, 4);
	uint32_t enemy_pos = (uint32_t)lua_tonumber(L, 5);

	battle_users_t* p_self_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, self_mark, self_pos, &p_self_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* p_enemy_team = NULL;
	pbi = check_lua_api_param(batid, enemy_mark, enemy_pos, &p_enemy_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	if (enemy_mark == self_mark){
		LUA_RETURN_NUMBER_1(L, 1);
	}

	if (self_pos >= 0 && self_pos < MAX_PLAYERS_PER_TEAM){
		LUA_RETURN_NUMBER_1(L, 1);
	}

	if (enemy_pos >= 0 && enemy_pos < MAX_PLAYERS_PER_TEAM){
		LUA_RETURN_NUMBER_1(L, 1);
	}

	warrior_t* lp_front = p_self_team->players_pos[self_pos - MAX_PLAYERS_PER_TEAM];
	if (!lp_front || CANNOT_ATTACK(lp_front)){
		LUA_RETURN_NUMBER_1(L, 1);
	}

	warrior_t* lp_ene_front = p_enemy_team->players_pos[enemy_pos - MAX_PLAYERS_PER_TEAM];
	if (!lp_ene_front || NONEED_ATTACK(lp_ene_front)){
		LUA_RETURN_NUMBER_1(L, 1);
	}

	LUA_RETURN_NUMBER_1(L, 0);
}

// batid, mark, pos, index, prop
int lua_get_player_skill_prop(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_skill_prop Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_skill_prop Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_player_skill_prop Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// skill_index
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_get_player_skill_prop Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// skill_prop
	if (!lua_isnumber(L, 5)){
		ERROR_LOG("lua_get_player_skill_prop Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int32_t pos = (int32_t)lua_tonumber(L, 3);
	int32_t index = (int32_t)lua_tonumber(L, 4);
	uint32_t skill_prop = (uint32_t)lua_tonumber(L, 5);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("lua_get_player_skill_prop battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("lua_get_player_skill_prop param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	warrior_t* lp = team->players_pos[pos];
	if (!lp){
		ERROR_LOG("lua_get_player_skill_prop param pos is not valid\t[%d]", pos);
		LUA_RETURN_NUMBER_1(L, -1);
	}
	
	if (index < 0 || index >= lp->skill_cnt){
		ERROR_LOG("lua_get_player_skill_prop param index is not valid\t[%d]", index);
		LUA_RETURN_NUMBER_1(L, -1);
	}
	
	skill_info_t* skill = &lp->skills[index];
	switch(skill_prop){
	case skill_prop_id:
		LUA_RETURN_NUMBER_1(L, skill->skill_id);
	case skill_prop_level:
		LUA_RETURN_NUMBER_1(L, skill->skill_level);
	case skill_prop_rate:
		LUA_RETURN_NUMBER_1(L, skill->rand_rate);
	case skill_prop_exp:
		LUA_RETURN_NUMBER_1(L, skill->skill_exp);
	}

	LUA_RETURN_NUMBER_1(L, -1);
}

// batid, mark, pos, index, prop
int lua_get_player_equip_prop(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_equip_prop Arg_1 must be string");
		lua_pushstring(L, "lua_get_player_equip_prop Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_equip_prop Arg_2 must be number");
		lua_pushstring(L, "lua_get_player_equip_prop Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_player_equip_prop Arg_3 must be number");
		lua_pushstring(L, "lua_get_player_equip_prop Arg_3 must be number");
		lua_error(L);
	}

	// equip_index
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_get_player_equip_prop Arg_3 must be number");
		lua_pushstring(L, "lua_get_player_equip_prop Arg_3 must be number");
		lua_error(L);
	}

	// equip_prop
	if (!lua_isnumber(L, 5)){
		ERROR_LOG("lua_get_player_equip_prop Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int32_t pos = (int32_t)lua_tonumber(L, 3);
	int32_t index = (int32_t)lua_tonumber(L, 4);
	uint32_t equip_prop = (uint32_t)lua_tonumber(L, 5);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("lua_get_player_equip_prop battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("lua_get_player_equip_prop param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	warrior_t* lp = team->players_pos[pos];
	if (!lp){
		ERROR_LOG("lua_get_player_equip_prop param pos is not valid\t[%d]", pos);
		LUA_RETURN_NUMBER_1(L, -1);
	}
	
	if (index < 0 || index >= lp->cloth_cnt){
		ERROR_LOG("lua_get_player_equip_prop param index is not valid\t[%d]", index);
		LUA_RETURN_NUMBER_1(L, -1);
	}
	
	body_cloth_t* cloth = &lp->clothes[index];
	switch(equip_prop){
	case equip_prop_id:
		LUA_RETURN_NUMBER_1(L, cloth->cloth_id);
	case equip_prop_typeid:
		LUA_RETURN_NUMBER_1(L, cloth->type_id);
	case equip_prop_clevel:
		LUA_RETURN_NUMBER_1(L, cloth->clevel);
	case equip_prop_durable_val:
		LUA_RETURN_NUMBER_1(L, cloth->durable_val);
	case equip_prop_equip_type:
		LUA_RETURN_NUMBER_1(L, cloth->equip_type);
	}

	LUA_RETURN_NUMBER_1(L, -1);
}

int lua_set_player_equip_prop(lua_State* L)
{
	DEBUG_LOG("lua_set_player_equip_prop");
	LUA_RETURN_NUMBER_1(L, 0);
}

// batid, mark
int lua_get_player_count(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_count Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_count Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("lua_get_player_count battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("lua_get_player_count param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	LUA_RETURN_NUMBER_1(L, team->count_ex);
}

// batid, mark, prof
int lua_get_player_of_prof_count(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_of_prof_count Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_of_prof_count Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// prof
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_player_of_prof_count Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	uint32_t prof = (uint32_t)lua_tonumber(L, 3);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("lua_get_player_of_prof_count battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("lua_get_player_of_prof_count param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	switch(prof){
	case prof_ancher:
	case prof_churchman:
	case prof_magician:
	case prof_swordman:
	case prof_none:
		break;
	default:
		ERROR_LOG("lua_get_player_of_prof_count param prof is not valid\t[%u]", prof);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	int i = 0;
	int count = 0;
	for (i = 0; i < (MAX_PLAYERS_PER_TEAM * 2); i++){
		warrior_t* lp = team->players_pos[i];
		if (!lp){
			continue;
		}

		if (lp->prof == prof){
			count++;
		}
	}

	LUA_RETURN_NUMBER_1(L, count);
}

// batid, mark
int lua_get_player_count_alive(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_count_alive Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_count_alive Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("lua_get_player_count_alive battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("lua_get_player_count_alive param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	if (mark == mark_challenger){
		LUA_RETURN_NUMBER_1(L, pbi->challger.alive_count);
	}

	LUA_RETURN_NUMBER_1(L, pbi->challgee.alive_count);
}

// batid, mark, prof
int lua_get_player_of_prof_count_alive(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_player_of_prof_count_alive Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_player_of_prof_count_alive Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// prof
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_player_of_prof_count_alive Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	uint32_t prof = (uint32_t)lua_tonumber(L, 3);

	battle_info_t* pbi = (battle_info_t*)g_hash_table_lookup(battles, &(batid));
	if (pbi == NULL){
		ERROR_LOG("lua_get_player_of_prof_count_alive battle is not exist\t[%lu]", batid);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	battle_users_t* team = NULL;
	if (mark == mark_challenger){
		team = &pbi->challger;
	}
	
	if (mark == mark_challengee){
		team = &pbi->challgee;
	}
	
	if (team == NULL){
		ERROR_LOG("lua_get_player_of_prof_count_alive param mark is not valid\t[%u]", mark);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	switch(prof){
	case prof_ancher:
	case prof_churchman:
	case prof_magician:
	case prof_swordman:
	case prof_none:
		break;
	default:
		ERROR_LOG("lua_get_player_of_prof_count_alive param prof is not valid\t[%u]", prof);
		LUA_RETURN_NUMBER_1(L, -1);
	}

	int i = 0;
	int count = 0;
	for (i = 0; i < (2 * MAX_PLAYERS_PER_TEAM); i++){
		warrior_t* lp = team->players_pos[i];
		if (!lp || NONEED_ATTACK(lp)){
			continue;
		}

		if (lp->prof == prof){
			count++;
		}
	}

	LUA_RETURN_NUMBER_1(L, count);
}

// batid, mark, pos
int lua_chk_player_can_attack(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_chk_player_can_attack Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_chk_player_can_attack Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_chk_player_can_attack Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int pos = (int)lua_tonumber(L, 3);

	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	warrior_t* lp = p_team->players_pos[pos];

	if (CANNOT_ATTACK(lp)){
		LUA_RETURN_NUMBER_1(L, 0);
	}

	LUA_RETURN_NUMBER_1(L, 1);
}

// batid, mark, pos
int lua_chk_plater_need_attack(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_chk_plater_need_attack Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_chk_plater_need_attack Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_chk_plater_need_attack Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int pos = (int)lua_tonumber(L, 3);

	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, 0);
	}

	warrior_t* lp = p_team->players_pos[pos];

	if (NONEED_ATTACK(lp)){
		LUA_RETURN_NUMBER_1(L, 0);
	}

	LUA_RETURN_NUMBER_1(L, 1);
}

// batid, mark, pos, atk_seq
// return mark, pos, atk_type, atk_level
int lua_get_play_atk_target(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_get_play_atk_target Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_get_play_atk_target Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_get_play_atk_target Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// atk_seq
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_get_play_atk_target Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int pos = (int)lua_tonumber(L, 3);
	uint32_t atk_seq = (uint32_t)lua_tonumber(L, 4);

	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_4(L, -1, -1, -1, -1);
	}

	switch(atk_seq){
	case 0:
	case 1:
		break;
	default:
		ERROR_LOG("lua_get_play_atk_target param atk_seq is not valid\t[%u]", atk_seq);
		LUA_RETURN_NUMBER_4(L, -1, -1, -1, -1);
	}
	warrior_t* lp = p_team->players_pos[pos];
	atk_info_t* atk_info = &lp->atk_info[atk_seq];
	LUA_RETURN_NUMBER_4(L, atk_info->atk_mark, atk_info->atk_pos, atk_info->atk_type, atk_info->atk_level);
}

// calc hit rate
// batid, mark_g, pos_g, mark_f, pos_f
int lua_calc_attack_hit_rate(lua_State* L)
{
	DEBUG_LOG("lua_calc_attack_hit_rate");
	LUA_RETURN_NUMBER_1(L, 0);
}

// batid, mark_g, pos_g, mark_f, pos_f, skill_index
int lua_calc_phy_atk_hurthp_1(lua_State* L)
{
	DEBUG_LOG("lua_calc_phy_atk_hurthp_1");
	LUA_RETURN_NUMBER_1(L, 0);
}

// batid, mark_g, pos_g, mark_f, pos_f, atk_seq
int lua_calc_phy_atk_hurthp_2(lua_State* L)
{
	DEBUG_LOG("lua_calc_phy_atk_hurthp_2");
	LUA_RETURN_NUMBER_1(L, 0);
}

// batid, mark_g, pos_g, mark_f, pos_f, skill_index
int lua_calc_mp_atk_hurthp_1(lua_State* L)
{
	DEBUG_LOG("lua_calc_mp_atk_hurthp_1");
	LUA_RETURN_NUMBER_1(L, 0);
}

// batid, mark_g, pos_g, mark_f, pos_f, atk_seq
int lua_calc_mp_atk_hurthp_2(lua_State* L)
{
	DEBUG_LOG("lua_calc_mp_atk_hurthp_2");
	LUA_RETURN_NUMBER_1(L, 0);
}

// batid, mark_g, pos_g, atk_pos, atk_mark, atk_seq, atk_type, atk_level
int lua_set_atk_skill(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_set_atk_skill Arg_1 must be string");
		lua_pushstring(L, "lua_set_atk_skill Arg_1 must be number");
		lua_error(L);
	}

	// mark_g
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_set_atk_skill Arg_2 must be number");
		lua_pushstring(L, "lua_set_atk_skill Arg_2 must be number");
		lua_error(L);
	}

	// pos_g
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_set_atk_skill Arg_3 must be number");
		lua_pushstring(L, "lua_set_atk_skill Arg_3 must be number");
		lua_error(L);
	}

	// mark_f
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_set_atk_skill Arg_4 must be number");
		lua_pushstring(L, "lua_set_atk_skill Arg_4 must be number");
		lua_error(L);
	}

	// pos_f
	if (!lua_isnumber(L, 5)){
		ERROR_LOG("lua_set_atk_skill Arg_5 must be number");
		lua_pushstring(L, "lua_set_atk_skill Arg_5 must be number");
		lua_error(L);
	}

	// atk_seq
	if (!lua_isnumber(L, 6)){
		ERROR_LOG("lua_set_atk_skill Arg_6 must be number");
		lua_pushstring(L, "lua_set_atk_skill Arg_6 must be number");
		lua_error(L);
	}

	// atk_type
	if (!lua_isnumber(L, 7)){
		ERROR_LOG("lua_set_atk_skill Arg_7 must be number");
		lua_pushstring(L, "lua_set_atk_skill Arg_7 must be number");
		lua_error(L);
	}

	// atk_level
	if (!lua_isnumber(L, 8)){
		ERROR_LOG("lua_set_atk_skill Arg_8 must be number");
		lua_pushstring(L, "lua_set_atk_skill Arg_8 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark_g = (uint32_t)lua_tonumber(L, 2);
	int pos_g = (int)lua_tonumber(L, 3);
	uint32_t mark_f = (uint32_t)lua_tonumber(L, 4);
	int pos_f = (int)lua_tonumber(L, 5);
	int atk_seq = (int)lua_tonumber(L, 6);
	uint32_t atk_type = (uint32_t)lua_tonumber(L, 7);
	uint32_t atk_level = (uint32_t)lua_tonumber(L, 8);

	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark_g, pos_g, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}
	
	warrior_t* lp_g = p_team->players_pos[pos_g];

	switch(atk_seq){
	case 0:
		if (lp_g->atk_info[0].atk_type != 0){
			ERROR_LOG("lua_set_atk_skill param atk_seq already set[%u] ", atk_seq);
			LUA_RETURN_NUMBER_1(L, 0);
		}
		break;
	case 1:
		if (lp_g->atk_info[0].atk_type == 0){
			ERROR_LOG("lua_set_atk_skill param atk_seq = 1 is not set[%u] ", atk_seq);
			LUA_RETURN_NUMBER_1(L, 0);
		}

		if (lp_g->atk_info[1].atk_type != 0){
			ERROR_LOG("lua_set_atk_skill param atk_seq is already set[%u] ", atk_seq);
			LUA_RETURN_NUMBER_1(L, 0);
		}
		break;
	default:
		ERROR_LOG("lua_set_atk_skill param atk_seq is not valid\t[%d]", atk_seq);
		LUA_RETURN_NUMBER_1(L, 0);
	}

	if (atk_level <= 0){
		atk_level = 1;
	}

	if (atk_level > 10){
		atk_level = 10;
	}
	lp_g->atk_info[atk_seq].atk_level = atk_level;
	lp_g->atk_info[atk_seq].atk_mark = (mark_g != mark_f);
	lp_g->atk_info[atk_seq].atk_pos = pos_f;
	lp_g->atk_info[atk_seq].atk_seq = atk_seq;
	lp_g->atk_info[atk_seq].atk_type = atk_type;

	DEBUG_LOG("BEAST ATK INFO-AI\t[batid=%lu uid=%u petid=%u atk_type=%u atk_level=%u atk_pos=%d ark_mark=%u]", pbi->batid, 
		lp_g->userid, lp_g->petid, atk_type, atk_level, pos_f, lp_g->atk_info[atk_seq].atk_mark);
	LUA_RETURN_NUMBER_1(L, 1);
}

// batid, mark, pos, atk_type
// return -1 not exist
int lua_has_atk_type(lua_State* L)
{
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_has_atk_type Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_has_atk_type Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_has_atk_type Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// type
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_has_atk_type Arg_4 must be number");
		lua_pushstring(L, "Arg_4 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int pos = (int)lua_tonumber(L, 3);
	uint32_t atk_type = (uint32_t)lua_tonumber(L, 4);

	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	DEBUG_LOG("lua_has_atk\t[batid=%lu batobj=%p globalloadtimer=%p globalfightertimer=%p]", pbi->batid, pbi, pbi->load_timer, pbi->fight_timer);
	
	warrior_t* lp = p_team->players_pos[pos];
	int i = 0;
	for (i = 0; i < lp->skill_cnt; i++){
		if (lp->skills[i].skill_id == atk_type){
			LUA_RETURN_NUMBER_1(L, i);
		}
	}

	LUA_RETURN_NUMBER_1(L, 0);
}

// batid, mark, pos, skill_index
int lua_calc_skill_cost_mp_1(lua_State* L)
{
	DEBUG_LOG("lua_calc_skill_cost_mp_1");
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_calc_skill_cost_mp_1 Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_calc_skill_cost_mp_1 Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_calc_skill_cost_mp_1 Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// skill_index
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_calc_skill_cost_mp_1 Arg_4 must be number");
		lua_pushstring(L, "Arg_4 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int pos = (int)lua_tonumber(L, 3);
	uint32_t skill_index = (uint32_t)lua_tonumber(L, 4);
	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}
	
	warrior_t* lp = p_team->players_pos[pos];
	if (skill_index < 0 || skill_index >= lp->skill_cnt){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	skill_mp_exp_t* psu = get_skill_mp_exp(lp->skills[skill_index].skill_id, lp->prof);
	if (!psu || !psu->id) {
		LUA_RETURN_NUMBER_1(L, -1);
	}
	int res_mp = (psu->mp_a * lp->skills[skill_index].skill_level+ psu->mp_b);
	LUA_RETURN_NUMBER_1(L, res_mp);
}

// batid, mark, pos, atk_seq
int lua_calc_skill_cost_mp_2(lua_State* L)
{
	DEBUG_LOG("lua_calc_skill_cost_mp_2");
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_calc_skill_cost_mp_2 Arg_1 must be string");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_calc_skill_cost_mp_2 Arg_2 must be number");
		lua_pushstring(L, "Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_calc_skill_cost_mp_2 Arg_3 must be number");
		lua_pushstring(L, "Arg_3 must be number");
		lua_error(L);
	}

	// atk_seq
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_calc_skill_cost_mp_2 Arg_4 must be number");
		lua_pushstring(L, "Arg_4 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int pos = (int)lua_tonumber(L, 3);
	uint32_t atk_seq = (uint32_t)lua_tonumber(L, 4);
	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}
	
	warrior_t* lp = p_team->players_pos[pos];
	if (atk_seq != 0 && atk_seq != 1){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	atk_info_t* pai = &lp->atk_info[atk_seq];
	if (pai->atk_type == 0){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	skill_mp_exp_t* psu = get_skill_mp_exp(pai->atk_type, lp->prof);
	if (!psu || !psu->id) {
		LUA_RETURN_NUMBER_1(L, -1);
	}
	int res_mp = (psu->mp_a * pai->atk_level + psu->mp_b);
	LUA_RETURN_NUMBER_1(L, res_mp);
}

// batid, mark, pos, atk_type, atk_level
int lua_calc_skill_cost_mp_3(lua_State* L)
{
	DEBUG_LOG("lua_calc_skill_cost_mp_3");
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_calc_skill_cost_mp_3 Arg_1 must be string");
		lua_pushstring(L, "lua_calc_skill_cost_mp_5 Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_calc_skill_cost_mp_3 Arg_2 must be number");
		lua_pushstring(L, "lua_calc_skill_cost_mp_5 Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_calc_skill_cost_mp_3 Arg_3 must be number");
		lua_pushstring(L, "lua_calc_skill_cost_mp_5 Arg_3 must be number");
		lua_error(L);
	}

	// atk_type
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_calc_skill_cost_mp_4 Arg_4 must be number");
		lua_pushstring(L, "lua_calc_skill_cost_mp_5 Arg_4 must be number");
		lua_error(L);
	}

	// atk_level
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_calc_skill_cost_mp_5 Arg_4 must be number");
		lua_pushstring(L, "lua_calc_skill_cost_mp_5 Arg_5 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int pos = (int)lua_tonumber(L, 3);
	uint32_t atk_type = (uint32_t)lua_tonumber(L, 4);
	uint32_t atk_level = (uint32_t)lua_tonumber(L, 5);
	
	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}
	
	warrior_t* lp = p_team->players_pos[pos];
	skill_mp_exp_t* psu = get_skill_mp_exp(atk_type, lp->prof);
	if (!psu || !psu->id) {
		LUA_RETURN_NUMBER_1(L, -1);
	}
	int res_mp = (psu->mp_a * atk_level + psu->mp_b);
	LUA_RETURN_NUMBER_1(L, res_mp);
}

// batid, mark, pos, atk_type, weapon
int lua_chk_skill_by_weapon(lua_State* L)
{
	DEBUG_LOG("lua_chk_skill_by_weapon");
	// batid
	if (!lua_isstring(L, 1)){
		ERROR_LOG("lua_chk_skill_by_weapon Arg_1 must be string");
		lua_pushstring(L, "lua_chk_skill_by_weapon Arg_1 must be number");
		lua_error(L);
	}

	// mark
	if (!lua_isnumber(L, 2)){
		ERROR_LOG("lua_chk_skill_by_weapon Arg_2 must be number");
		lua_pushstring(L, "lua_chk_skill_by_weapon Arg_2 must be number");
		lua_error(L);
	}

	// pos
	if (!lua_isnumber(L, 3)){
		ERROR_LOG("lua_chk_skill_by_weapon Arg_3 must be number");
		lua_pushstring(L, "lua_chk_skill_by_weapon Arg_3 must be number");
		lua_error(L);
	}

	// atk_type
	if (!lua_isnumber(L, 4)){
		ERROR_LOG("lua_chk_skill_by_weapon Arg_4 must be number");
		lua_pushstring(L, "lua_chk_skill_by_weapon Arg_4 must be number");
		lua_error(L);
	}

	// weapon
	if (!lua_isnumber(L, 5)){
		ERROR_LOG("lua_chk_skill_by_weapon Arg_5 must be number");
		lua_pushstring(L, "lua_chk_skill_by_weapon Arg_5 must be number");
		lua_error(L);
	}

	uint64_t batid = atoll(lua_tostring(L, 1));
	uint32_t mark = (uint32_t)lua_tonumber(L, 2);
	int pos = (int)lua_tonumber(L, 3);
	uint32_t atk_type = (uint32_t)lua_tonumber(L, 4);
	uint32_t weapon = (uint32_t)lua_tonumber(L, 5);
	
	battle_users_t* p_team = NULL;
	battle_info_t* pbi = check_lua_api_param(batid, mark, pos, &p_team);
	if (pbi == NULL){
		LUA_RETURN_NUMBER_1(L, -1);
	}

	DEBUG_LOG("lua_chk_skill_by_weapon\t[batid=%lu batobj=%p globalloadtimer=%p globalfightertimer=%p]", pbi->batid, pbi, pbi->load_timer, pbi->fight_timer);
	
//	warrior_t* lp = p_team->players_pos[pos];
	skill_attr_t* psu = get_skill_attr(atk_type, weapon);
	if (!psu || !psu->id){
		LUA_RETURN_NUMBER_1(L, 0);
	}
	LUA_RETURN_NUMBER_1(L, 1);
}

// mod
// return 1/0
int lua_calc_rand_rate(lua_State* L)
{
	// mark
	if (!lua_isnumber(L, 1)){
		ERROR_LOG("lua_calc_rand_rate Arg_1 must be number");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	uint32_t rate = (uint32_t)lua_tonumber(L, 1);
	
	if (RAND_NUM(100,rate)){
		LUA_RETURN_NUMBER_1(L, 1);
	}

	LUA_RETURN_NUMBER_1(L, 0);
}

// create a rand number
int lua_cert_rand_num(lua_State* L)
{
	// mark
	if (!lua_isnumber(L, 1)){
		ERROR_LOG("lua_calc_rand_rate Arg_1 must be number");
		lua_pushstring(L, "Arg_1 must be number");
		lua_error(L);
	}

	uint32_t rate = (uint32_t)lua_tonumber(L, 1);
	if (rate == 0){
		LUA_RETURN_NUMBER_1(L, -1);
	}
	
	uint32_t ret = rand() % rate;
	
	LUA_RETURN_NUMBER_1(L, ret);
}


