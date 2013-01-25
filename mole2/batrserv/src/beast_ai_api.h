#ifndef __BATRSER_BEAST_AI_API_H__
#define __BATRSER_BEAST_AI_API_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <libxml/tree.h>
#include <async_serv/net_if.h>

#include "utils.h"
#include "proto.h"
#ifdef __cplusplus
}
#endif


#include "bt_online.h"

enum
{
	prop_hp      = 0,       // 当前血量
	prop_hp_max  = 1,   // 最大血量值
	prop_mp      = 2,       // 当前魔法
	prop_mp_max  = 3,   // 最大魔法值
	prop_weapon  = 4,   // 武器类型
	prop_prof    = 5,     // 职业
	prop_speed   = 6,    // 速度
	prop_level   = 7,    // 等级
	prop_typeid  = 8,   // type id
	prop_injury  = 9,   // 受伤
	prop_shield  = 10,  // 盾牌
	prop_spirit  = 11,  // 精神
	prop_resume  = 12, // resume
	prop_hit_rate = 13, // 命中率(战斗中实际命中率的计算参数)
	prop_avoid_rate = 14, // 回避率(注意实际战斗中回避率是通过命中率反向计算得出，这个值是否有用还得问online)
	prop_bisha = 15, // 必杀(计算必杀概率的一个参数)
	prop_fight_back = 16, // 反击
	prop_cloth_cnt = 17, // 装备数量
	prop_in_front = 18,  // 前后排(宠物的前后排通过人的属性来判断)
	prop_earth = 19, // 地属性
	prop_water = 20, // 水属性
	prop_fire  = 21, // 火属性
	prop_wind  = 22, // 风属性
	prop_attack_val = 23, // 基础物理攻击值
	prop_mattack_val = 24, // 基础魔法攻击值
	prop_defense_val = 25, // 物理防御值
	prop_mdefense_val = 26, // 魔法防御值
	prop_race         = 27, // 种族
	prop_pet_cnt      = 28, // 宠物数量
	prop_catchable    = 29, // 宠物是否可抓
	prop_handbooklv   = 30, // 宠物图鉴
	prop_pet_contract_lv = 31, // 精灵契约
	prop_pet_state       = 32, // 宠物状态(等待，出战等等)
	prop_id              = 33, // id
	prop_pet_id          = 34, // 宠物id
	prop_skill_count     = 35, // 技能数量
	prop_groupid  		= 40, // 得到groupid 
};

enum
{
	skill_prop_id    = 0,     // 技能id
	skill_prop_level = 1,  // 技能等级
	skill_prop_rate  = 2,   // 技能概率
	skill_prop_exp   = 3,    // 技能经验
};

enum
{
	equip_prop_id = 0,     // 装备id
	equip_prop_typeid = 1, // 装备类型id
	equip_prop_clevel = 2, // 装备等级
	equip_prop_durable_val = 3, // 耐久值
	equip_prop_equip_type = 4,  // 装备类型
};

enum
{
	mark_challenger = 0, // 主动挑战方
	mark_challengee = 1, // 挑战接受方
};

// check player has prop
// batid, mark, pos, propname
int lua_chk_player_has_prop(lua_State* L);

// batid, pos, mark, propname
int lua_get_player_prop(lua_State* L);
int lua_set_player_prop(lua_State* L);

// 校验状态
int lua_chk_player_state(lua_State* L);
int lua_reset_player_state(lua_State* L);

// batid, mark, typeid
int lua_get_beast_by_typeid(lua_State* L);

// batid, mark, pos, index, prop
int lua_get_player_skill_prop(lua_State* L);

// batid, mark, pos, index, prop
int lua_get_player_equip_prop(lua_State* L);

// batid, mark, pos, index, prop, propval
int lua_set_player_equip_prop(lua_State* L);

// batid, mark
int lua_get_player_highspeed(lua_State* L);

// batid, mark
int lua_get_player_least_hp(lua_State* L);

// batid, mark
int lua_get_player_most_hp(lua_State* L);

// batid, mark, prof
int lua_get_player_by_prof_rand(lua_State* L);

// batid, mark, prof
int lua_get_player_except_prof_rand(lua_State* L);

// batid, mark, state
int lua_get_player_by_state_rand(lua_State* L);

// batid, mark,
int lua_get_player_by_rand(lua_State* L);
int lua_get_player_by_rand_distance(lua_State* L);
int lua_get_player_fangyu_rand_distance(lua_State* L);


// batid self_mark self_pos enemy_mark enemy_pos
int lua_can_reach_distance(lua_State* L);

// batid, mark
int lua_get_player_count(lua_State* L);

// batid, mark, prof
int lua_get_player_of_prof_count(lua_State* L);

// batid, mark
int lua_get_player_count_alive(lua_State* L);

// batid, mark, prof
int lua_get_player_of_prof_count_alive(lua_State* L);

// batid, mark, pos
int lua_chk_player_can_attack(lua_State* L);

// batid, mark, pos
int lua_chk_plater_need_attack(lua_State* L);

// batid, mark, pos, atk_seq
// return mark, pos, atk_type, atk_level
int lua_get_play_atk_target(lua_State* L);

// calc hit rate
// batid, mark_g, pos_g, mark_f, pos_f
int lua_calc_attack_hit_rate(lua_State* L);

// batid, mark_g, pos_g, mark_f, pos_f, skill_index
int lua_calc_phy_atk_hurthp_1(lua_State* L);

// batid, mark_g, pos_g, mark_f, pos_f, atk_seq
int lua_calc_phy_atk_hurthp_2(lua_State* L);

// batid, mark_g, pos_g, mark_f, pos_f, skill_index
int lua_calc_mp_atk_hurthp_1(lua_State* L);

// batid, mark_g, pos_g, mark_f, pos_f, atk_seq
int lua_calc_mp_atk_hurthp_2(lua_State* L);

// batid, mark_g, pos_g, atk_pos, atk_mark, atk_type, atk_level
int lua_set_atk_skill(lua_State* L);

// batid, mark, pos, atk_type
// return -1 not exist
int lua_has_atk_type(lua_State* L);

// batid, mark, pos, skill_index
int lua_calc_skill_cost_mp_1(lua_State* L);

// batid, mark, pos, atk_seq
int lua_calc_skill_cost_mp_2(lua_State* L);

// batid, mark, pos, atk_type, atk_level
int lua_calc_skill_cost_mp_3(lua_State* L);

// batid, mark, pos, atk_type, weapon
int lua_chk_skill_by_weapon(lua_State* L);

// mod
// return 1/0
int lua_calc_rand_rate(lua_State* L);

// create a rand number
int lua_cert_rand_num(lua_State* L);

// 这个函数不用注册，仅仅是api回调的时候校验参数
battle_info_t* check_lua_api_param(uint64_t batid, uint32_t mark, int32_t pos, battle_users_t** p_team);
#endif

