/**
 *============================================================
 *  @file      BossAI.cpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
: *============================================================
 */
 
extern "C" {
#include <libtaomee/log.h>
}

#include <libtaomee++/random/random.hpp>
#include <libtaomee++/inet/byteswap.hpp>

#include <libtaomee++/pathfinder/astar/astar.hpp>

#include "MonsterAI.hpp"
#include "HighLvAI.hpp"
#include "BossAI.hpp"
#include "../player.hpp"
#include "../stage.hpp"
#include "../player_status.hpp"
#include "../map_summon_object.hpp"
#include "../battle_impl.hpp"
using namespace taomee;


/****************************************************************
 * normal Boss
 ****************************************************************/
bool
BossAI::extra_wait_action(Player * player)
{
	//in serious injury
	if ( is_serious_injury(player) ) {
		//20% odds, to evade, 
		int rand_num = rand() % 100;
		if (rand_num < 20) {
			//TODO:20% odds
		} else if (rand_num < 60) {
			//TODO:50% odds, call other mons
		} else {
			//TODO:50% odds, braver attack
			if ((player->role_type == 13007 || player->role_type == 13026)) {
				if (use_buf_skill(player)) {
					return true;
				}
			}
		}
	}

	if (use_buf_skill(player)) {
		return true;
	}

	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {
		//buff skill
		if ((player->role_type == 13007 || player->role_type == 13026) &&
			player->hp == player->maxhp) {
			if (use_buf_skill(player)) {
				return true;
			}
		}
	}

	
	return false;
}

bool
BossAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * Pumpkin Boss
 ****************************************************************/
bool
PumpkinBossAI::extra_wait_action(Player * player)
{
	//teleport
	if (player->i_ai->common_flag_) {
		player->i_ai->common_flag_ = 0;
		player->invincible_time = 1;
		teleport(player, 1000, 250);
		player->i_ai->common_flag1_ = 1;
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}
	
	//launch skill1
	if (player->i_ai->common_flag1_ >= 1) {
		player->i_ai->change_state(ATTACK_STATE);
		player->i_ai->common_flag1_ = player->i_ai->common_flag1_ > 5 ? 0 : player->i_ai->common_flag1_;
		if (player->i_ai->common_flag1_ == 0) {
			uint32_t rand_num = rand() % 100;
			if (rand_num < 60) {
				player->i_ai->common_flag2_ = 1;
			} else {
				player->i_ai->common_flag3_ = 1;
			}
			player->i_ai->set_state_keep_tm(2);
		}
		return true;
	}
	
	//launch skill2, and then teleport
	if (player->i_ai->common_flag2_ >= 1) {
		player->i_ai->change_state(ATTACK_STATE);
		player->i_ai->common_flag2_ = player->i_ai->common_flag2_ > 3 ? 0 : player->i_ai->common_flag2_;
		if (player->i_ai->common_flag2_ == 0) {
			player->invincible_time = 0;
			teleport(player, player->i_ai->init_pos.x(), player->i_ai->init_pos.y());
			player->i_ai->set_state_keep_tm(2);
		}
		return true;
	}
	
	//launch skill3, and then teleport
	if (player->i_ai->common_flag3_ >= 1) {
		player->i_ai->change_state(ATTACK_STATE);
		player->i_ai->common_flag3_ = player->i_ai->common_flag3_ > 3 ? 0 : player->i_ai->common_flag3_;
		if (player->i_ai->common_flag3_ == 0) {
			player->invincible_time = 0;
			teleport(player, player->i_ai->init_pos.x(), player->i_ai->init_pos.y());
			player->i_ai->set_state_keep_tm(2);
		}
		return true;
	}
	
	//hatred
	if (player->i_ai->hatred() > 10) {
		player->i_ai->set_hatred(0);
		player->i_ai->common_flag_ = 1;
	}
	return false;
}

bool
PumpkinBossAI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;
	if (player->i_ai->common_flag1_ >= 1) {
		//get target
		if (player->i_ai->target == 0) {
			if ( (player->i_ai->target = get_target(player)) == 0 ) {
				player->i_ai->common_flag1_++;
				player->i_ai->change_state(WAIT_STATE);
				return true;
			}	
		}
		
		skill_id = 4120002;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill, player->i_ai->target->pos().x(), player->i_ai->target->pos().y());
			player->i_ai->common_flag1_++;
		}
		player->i_ai->set_state_keep_tm(2);
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	if (player->i_ai->common_flag2_ >= 1) {
		//get target
		if (player->i_ai->target == 0) {
			if ( (player->i_ai->target = get_target(player)) == 0 ) {
				player->i_ai->common_flag2_++;
				player->i_ai->change_state(WAIT_STATE);
				return true;
			}	
		}
		
		skill_id = 4120003;
		uint32_t pos_x[3] = {397, 1051, 1705};
		uint32_t pos_y[3] = {500, 500, 500};
		uint32_t i = player->i_ai->common_flag2_ % 3;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill, pos_x[i], pos_y[i]);
			player->i_ai->common_flag2_++;
		}
		player->i_ai->set_state_keep_tm(2);
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	if (player->i_ai->common_flag3_ >= 1) {
		//get target
		if (player->i_ai->target == 0) {
			if ( (player->i_ai->target = get_target(player)) == 0 ) {
				player->i_ai->common_flag3_++;
				player->i_ai->change_state(WAIT_STATE);
				return true;
			}	
		}
		
		skill_id = 4120001;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill, player->i_ai->target->pos().x(), player->i_ai->target->pos().y());
			player->i_ai->common_flag3_++;
		}
		player->i_ai->set_state_keep_tm(2);
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}

	return false;
}

bool
PumpkinBossAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			//used for teleport
			uint32_t rand_num = rand() % 100;
			if (rand_num < 50) {
				player->i_ai->set_hatred(0);
				player->i_ai->common_flag_ = 1;
			}
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}


/****************************************************************
 * BellCat Boss
 ****************************************************************/
bool
BellCatAI::extra_wait_action(Player * player)
{
	//evade for some times
	uint32_t count = player->role_type == 13036 ? 6 : 8;
	if (player->i_ai->common_flag_ < count && is_hp_range(player, 0, 20)) {
		player->i_ai->change_state(EVADE_STATE);
		player->i_ai->common_flag_++;
		if (player->i_ai->common_flag_ >= count) {
			player->i_ai->common_flag1_ = 1;
		}
		return true;
	}

	//after evade, call another monster
	if (player->i_ai->common_flag1_) {
		uint32_t mon_id = player->role_type == 13036 ? 13037 : 13038;
		player->call_monster(mon_id, player->pos().x(), player->pos().y());
		player->i_ai->set_state_keep_tm(1);
		
		player->i_ai->common_flag1_ = 0;
		player->i_ai->common_flag2_ = 1;
		return true;
	}

	//teleport to impossible place
	if (player->i_ai->common_flag2_) {
		teleport(player, 10000, 10000);
		player->i_ai->common_flag2_ = 0;
		player->i_ai->common_flag3_ = 1;
		return true;
	}

	//wait always
	if (player->i_ai->common_flag3_) {
		return true;
	}

	return false;
}

bool
BellCatAI::extra_attack_action(Player * player)
{
	return false;
}

bool
BellCatAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * Bitores Boss
 ****************************************************************/
bool
BitoresAI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag2_) {
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}

	if (!player->i_ai->common_flag_) {
		// to attack
		player->i_ai->change_state(ATTACK_STATE);
		player->i_ai->common_flag1_++;
		if (player->i_ai->common_flag1_ < 6 && player->i_ai->common_flag1_ % 2 == 0) {
			player->i_ai->common_flag_ = 1;
		} else if (player->i_ai->common_flag1_ >= 6) {
			//to weakness state
			player->i_ai->common_flag2_ = 1;
			player->i_ai->common_flag1_ = 0;
			player->i_ai->change_state(ATTACK_STATE);
		}
		return true;
	} else {
		// to evade
		player->i_ai->common_flag_ = 0;
		player->i_ai->change_state(EVADE_STATE);
		return true;
	}

	return false;
}

bool
BitoresAI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;

	//throw bomb
	if (player->i_ai->common_flag1_) {
		//get target
		if (player->i_ai->target == 0) {
			if ( (player->i_ai->target = get_target(player)) == 0 ) {
				player->i_ai->change_state(WAIT_STATE);
				return true;
			}	
		}
		
		skill_id = 4120008;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill, player->i_ai->target->pos().x(), player->i_ai->target->pos().y());
			///player->i_ai->common_flag1_++;
		}
		player->i_ai->set_state_keep_tm(2);
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	//weakness state
	if (player->i_ai->common_flag2_) {
		skill_id = 4120007;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag2_ = 0;
		}
		player->i_ai->set_state_keep_tm(5);
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	return false;
}

bool
BitoresAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * Balu Boss
 ****************************************************************/
bool
BaluAI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag_) {
		// to defence
		player->i_ai->change_state(ATTACK_STATE);
		player->i_ai->common_flag_ = 0;
		player->i_ai->common_flag1_ = 1;
		return true;
	}

	//defence
	if (player->i_ai->common_flag1_) {
		if (player->i_ai->hatred() > 4) {
			player->i_ai->set_hatred(0);
			player->i_ai->common_flag1_ = 0;
			player->i_ai->common_flag2_ = 1;
			player->hit_fly_flag = true;
			player->damage_ration = 0;
		}
		return true;
	}

	//5 attack
	if (player->i_ai->common_flag2_) {
		// to attack 5
		player->invincible_time = 1;
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}

	//hatred
	if (player->i_ai->hatred() > 6) {
		player->i_ai->set_hatred(0);
		player->i_ai->common_flag_ = 1;
	}
	return false;
}

bool
BaluAI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;

	//only denfence, 20% damage
	if (player->i_ai->common_flag1_) {
		//get target
		/*if (player->i_ai->target == 0) {
			if ( (player->i_ai->target = get_target(player)) == 0 ) {
				player->i_ai->change_state(WAIT_STATE);
				return true;
			}	
		}*/
		
		skill_id = 4120006;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			if (player->i_ai->target == 0) {
				player->i_ai->target = get_target(player);
			}
			monster_attack(player, skill);
			player->i_ai->set_hatred(0);
			player->hit_fly_flag = false;
			player->damage_ration = 20;
		}
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	//attack state
	if (player->i_ai->common_flag2_) {
		skill_id = 4120005;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag2_ = 0;
			player->invincible_time = 0;
		}
		player->i_ai->set_state_keep_tm(3);
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	return false;
}

bool
BaluAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
	
			player->i_ai->common_flag_ = 1;		
			//player->i_ai->set_state_keep_tm(2);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * Chifeng Boss
 ****************************************************************/
bool
ChifengAI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag_ && player->i_ai->ready_skill_id_ == 0) {
		if (player->i_ai->common_flag1_ == 0) {
			player->i_ai->ready_skill_id_ = 0;
			player->i_ai->common_flag_ = 0;
			return true;
		}
		
		player->i_ai->ready_skill_id_ = 4120011;
		player->i_ai->common_flag1_--;
		return true;
	}
	
	return false;
}

bool
ChifengAI::extra_attack_action(Player * player)
{
	return false;
}

bool
ChifengAI::extra_stuck_action(Player * player)
{
	if (player->i_ai->hatred() > 4) {
		player->i_ai->set_hatred(0);
		player->i_ai->common_flag_ = 1;
		//attack count
		if (is_hp_range(player, 80, 100)) {
			player->i_ai->common_flag1_ = 2;
		} else if (is_hp_range(player, 60, 80)) {
			player->i_ai->common_flag1_ = 4;
		} else if (is_hp_range(player, 40, 60)) {
			player->i_ai->common_flag1_ = 6;
		} else if (is_hp_range(player, 0, 40)) {
			player->i_ai->common_flag1_ = 8;
		}
	}
	
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * RabBro1 Boss
 ****************************************************************/
bool
RabBro1AI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag2_) {
		return true;
	}
	
	if (player->hp <= 1) {
		player->invincible_time = 1;
		player->i_ai->common_flag1_ = 1;
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}

	return false;
}

bool
RabBro1AI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;

	//comatose state
	if (player->i_ai->common_flag1_) {
		skill_id = 4120015;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag2_ = 1;
			
			check_other_bro(player);
		}
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	return false;
}

bool
RabBro1AI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

bool
RabBro1AI::check_other_bro(Player* player)
{
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) {
		Player* p = *it;
		if ( (p->role_type== 13039 || p->role_type== 13040 || p->role_type== 13041) && p->i_ai->common_flag2_ != 1) {
			return false;
		}
	}
	
	//call big rabbits
	uint32_t mon_id = 13042;
	player->call_monster(mon_id, player->pos().x(), player->pos().y());
	
	//teleport all rabbits
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) {
		Player* p = *it;
		if (p->role_type== 13039 || p->role_type== 13040 || p->role_type== 13041) {
			teleport(p, 10000, 10000);
		}
	}
	
	return true;
}
/****************************************************************
 * RabBro2 Boss
 ****************************************************************/
bool
RabBro2AI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag2_) {
		return true;
	}
	
	if (player->hp <= 1) {
		player->invincible_time = 1;
		player->i_ai->common_flag1_ = 1;
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}
		
	if (player->i_ai->common_flag_) {
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}
	
	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {
		player->i_ai->common_flag_ = 1;
		player->i_ai->change_state(EVADE_STATE);
		return true;
	}

	return false;
}

bool
RabBro2AI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;
	
	//comatose state
	if (player->i_ai->common_flag1_) {
		skill_id = 4120015;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag2_ = 1;
			
			check_other_bro(player);
		}

		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	//landmine skill
	if (player->i_ai->common_flag_) {
		skill_id = 4120014;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill && player->btl->get_map_summon_count(player->id) < 10) {
			monster_attack(player, skill);
		}
		player->i_ai->common_flag_ = 0;
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	return false;
}

bool
RabBro2AI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

bool
RabBro2AI::check_other_bro(Player* player)
{
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) {
		Player* p = *it;
		if ( (p->role_type== 13039 || p->role_type== 13040 || p->role_type== 13041) && p->i_ai->common_flag2_ != 1) {
			return false;
		}
	}
	
	//call big rabbits
	uint32_t mon_id = 13042;
	player->call_monster(mon_id, player->pos().x(), player->pos().y());
	
	//teleport all rabbits
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) {
		Player* p = *it;
		if (p->role_type== 13039 || p->role_type== 13040 || p->role_type== 13041) {
			teleport(p, 10000, 10000);
		}
	}
	
	return true;
}
/****************************************************************
 * RabBro3 Boss
 ****************************************************************/
bool
RabBro3AI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag2_) {
		return true;
	}
	
	if (player->hp <= 1) {
		player->invincible_time = 1;
		player->i_ai->common_flag_ = 0;
		player->i_ai->common_flag1_ = 1;
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}
		
	if (player->i_ai->common_flag_) {
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}
	
	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {
		player->i_ai->common_flag_ = 1;
		player->i_ai->change_state(EVADE_STATE);
		return true;
	}

	return false;
}

bool
RabBro3AI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;
	
	//comatose state
	if (player->i_ai->common_flag1_) {
		skill_id = 4120015;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag2_ = 1;
			
			check_other_bro(player);
		}
		
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	//launch buff skill
	if (player->i_ai->common_flag_) {
		skill_id = 4120012;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag_ = 0;
		} else {
			player->i_ai->common_flag_ = 0;
			return false;
		}
		
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	return false;
}

bool
RabBro3AI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

bool
RabBro3AI::check_other_bro(Player* player)
{
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) {
		Player* p = *it;
		if ( (p->role_type== 13039 || p->role_type== 13040 || p->role_type== 13041) && p->i_ai->common_flag2_ != 1) {
			return false;
		}
	}
	
	//call big rabbits
	uint32_t mon_id = 13042;
	player->call_monster(mon_id, player->pos().x(), player->pos().y());
	
	//teleport all rabbits
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) {
		Player* p = *it;
		if (p->role_type== 13039 || p->role_type== 13040 || p->role_type== 13041) {
			teleport(p, 10000, 10000);
		}
	}
	
	return true;
}
/****************************************************************
 * Rabbits Boss
 ****************************************************************/
bool
RabbitsAI::extra_wait_action(Player * player)
{
	//buff skill
	if (player->i_ai->common_flag_ && player->i_ai->ready_skill_id_ == 0) {
		if (player->i_ai->common_flag1_ == 1) {
			player->i_ai->ready_skill_id_ = 4035003;
			player->i_ai->common_flag1_++;
		} else if (player->i_ai->common_flag1_ == 2) {
			player->i_ai->ready_skill_id_ = 4120017;
			player->i_ai->common_flag1_++;
		} else if (player->i_ai->common_flag1_ == 3) {
			player->i_ai->ready_skill_id_ = 4120013;
			player->i_ai->common_flag_ = 0;
			player->i_ai->common_flag1_ = 0;
		}
		return true;
		
	}

	return false;
}

bool
RabbitsAI::extra_attack_action(Player * player)
{
	return false;
}

bool
RabbitsAI::extra_stuck_action(Player * player)
{
	if (player->i_ai->hatred() > 3 && !player->i_ai->common_flag_) {
		player->i_ai->set_hatred(0);
		player->i_ai->common_flag_ = 1;
		player->i_ai->common_flag1_ = 1;
	}
	
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * ChuanYin Boss
 ****************************************************************/
bool
ChuanYinAI::extra_wait_action(Player * player)
{
	if (!player->i_ai->common_flag_) {
		player->i_ai->common_flag_ = 1;
		//player->i_ai->common_flag1_ = 1;
		player->i_ai->set_event_tm(15000);
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}
	
	if (player->i_ai->check_event_tm()) {
		player->i_ai->common_flag1_ = 1;
		player->invincible_time = 1;
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}

	return false;
}

bool
ChuanYinAI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;

	//call 4 monster
	if (player->i_ai->common_flag1_) {
		player->invincible_time = 0;
		
		//get target
		if ( (player->i_ai->target = get_target(player)) == 0 ) {
			player->i_ai->set_event_tm(5000);
			player->i_ai->change_state(WAIT_STATE);
			player->i_ai->common_flag1_ = 0;
			return true;
		}	
		
		//check monster count in map
		uint32_t mon_id = 11208;
		uint32_t exits_cnt = get_monster_cnt(player, mon_id);
		if (exits_cnt >= 2) {
			//wait for 10s
			player->i_ai->set_event_tm(10000);
			player->i_ai->change_state(WAIT_STATE);
			player->i_ai->common_flag1_ = 0;
			return true;
		}
			
		//launch skill
		skill_id = 4120018;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag1_ = 0;
			
			//call monsters
			uint32_t create_cnt = 2 - exits_cnt;
			player->call_monsters(mon_id, create_cnt);
			
			//wait for 40s
			player->i_ai->set_event_tm(30000);
		}
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	return false;
}

bool
ChuanYinAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * HaBo Boss
 ****************************************************************/
bool
HaBoAI::extra_wait_action(Player * player)
{
	if (!player->i_ai->common_flag_) {
		player->i_ai->common_flag_ = 1;
		player->i_ai->set_event_tm(5000);
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}
	
	if (player->i_ai->check_event_tm()) {
		if (player->i_ai->target == 0) {
			if ( (player->i_ai->target = get_target(player)) == 0 ) {
				player->i_ai->set_event_tm(5000);
				return false;
			}
		}
		
		player->i_ai->common_flag1_++;
		player->i_ai->common_flag2_ = 1;
			
		player->invincible_time = 1;
		teleport(player, player->i_ai->target->pos().x(), player->i_ai->target->pos().y());
		
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}
	

	return false;
}

bool
HaBoAI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;

	if (player->i_ai->common_flag2_) {
		player->invincible_time = 0;
			
		//launch skill
		skill_id = 4120024;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack_without_noti(player, skill);
			
			if (player->i_ai->common_flag1_ < 3) {
				player->i_ai->set_event_tm(1500);
			} else {
				player->i_ai->set_event_tm(10000);
				player->i_ai->common_flag1_ = 0;
			}
			
			player->i_ai->common_flag2_ = 0;
		}
		
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	return false;
}

bool
HaBoAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * JiXuan Boss
 ****************************************************************/
bool
JiXuanAI::extra_wait_action(Player * player)
{
	if (!player->i_ai->common_flag_) {
		player->i_ai->common_flag_ = 1;
		player->i_ai->set_event_tm(10000);
		player->i_ai->set_event_tm(15000, 2);
		return true;
	}
	
	if (player->i_ai->check_event_tm()) {
		if (use_buf_skill(player)) {
			player->i_ai->set_event_tm(35000);
			return true;
		}
	}
	
	if (player->i_ai->check_event_tm(2)) {
		player->invincible_time = 1;
		player->i_ai->common_flag1_ = 1;
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}

	return false;
}

bool
JiXuanAI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;

	if (player->i_ai->common_flag1_) {
		player->invincible_time = 0;
			
		//launch skill
		skill_id = 4120022;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->set_event_tm(25000, 2);
		} else {
			player->i_ai->set_event_tm(5000, 2);
		}
		
		player->i_ai->common_flag1_ = 0;
		
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	player_skill_t* skill = player->select_skill(NULL, 4120089);
	if(skill){
		monster_attack(player, skill);
		player->call_monster(11302, player->pos().x(), player->pos().y());
		return true;	
	}
	return false;
}

bool
JiXuanAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * JiaCi Boss
 ****************************************************************/
bool
JiaCiAI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag1_ && player->i_ai->check_event_tm()) {
		//player->invincible_time = 0;
		player->i_ai->common_flag1_ = 0;
		return true;
	} else if (player->i_ai->common_flag1_) {
		return true;
	}
	
	if (player->i_ai->hatred() > 10) {
		player->i_ai->set_hatred(0);
		player->invincible_time = 1;
		player->i_ai->common_flag_ = 1;
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}

	return false;
}

bool
JiaCiAI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;

	if (player->i_ai->common_flag_) {
		player->invincible_time = 0;
			
		//launch skill
		skill_id = 4120019;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			//player->invincible_time = 1;
			player->i_ai->common_flag1_ = 1;
			player->i_ai->set_event_tm(15000);
		}
		
		player->i_ai->common_flag_ = 0;
		
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	return false;
}

bool
JiaCiAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	player->i_ai->change_state(WAIT_STATE);
	return true;
}

/****************************************************************
 * YouQi Boss
 ****************************************************************/
bool
YouQiAI::extra_wait_action(Player * player)
{
	if (!player->i_ai->common_flag_ && is_hp_range(player, 0, 30)) {
		player->invincible_time = 1;
		player->i_ai->common_flag1_ = 1;
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}

	return false;
}

bool
YouQiAI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;

	if (player->i_ai->common_flag1_) {
		player->invincible_time = 0;
			
		//launch skill
		skill_id = 4120030;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->set_run();
			player->i_ai->common_flag_ = 1;
			player->i_ai->common_flag1_ = 0;
		}
		
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	return false;
}

bool
YouQiAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * ShaKe Boss
 ****************************************************************/
bool
ShaKeAI::extra_wait_action(Player * player)
{
	if (!player->i_ai->common_flag_) {
		player->i_ai->set_event_tm(5000);
		player->i_ai->common_flag_ = 1;
		return true;
	}
	
	if (player->i_ai->check_event_tm()) {
		//player->invincible_time = 1;
		player->i_ai->common_flag1_++;
		if (player->i_ai->common_flag1_ % 2 == 0) {
			player->i_ai->ready_skill_id_ = 4120037;
		} else {
			player->i_ai->ready_skill_id_ = 4120038;
		}
		player->i_ai->set_event_tm(10000);
		return true;
	}

	return false;
}

bool
ShaKeAI::extra_attack_action(Player * player)
{
	/*uint32_t skill_id = 0;

	if (player->i_ai->common_flag1_) {
		player->invincible_time = 0;
			
		//launch skill
		if (player->i_ai->common_flag1_ % 2 == 0) {
			skill_id = 4120037;
		} else {
			skill_id = 4120038;
		}
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->set_event_tm(20000);
		}
		
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}*/
	
	return false;
}

bool
ShaKeAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * MangYa Boss
 ****************************************************************/
bool
MangYaAI::extra_wait_action(Player * player)
{
	if (!player->i_ai->common_flag_) {
		player->i_ai->set_event_tm(5000);
		player->i_ai->common_flag_ = 1;
		return true;
	}
	
	if (player->i_ai->check_event_tm()) {
		player->i_ai->ready_skill_id_ = 4120042;
		player->i_ai->set_event_tm(21000);
		return true;
	}

	return false;
}

bool
MangYaAI::extra_attack_action(Player * player)
{
	return false;
}

bool
MangYaAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

/****************************************************************
 * DaoLang Boss
 ****************************************************************/
bool
DaoLangAI::extra_wait_action(Player * player)
{
	if (!player->i_ai->common_flag_) {
		player->i_ai->set_event_tm(30000);
		player->i_ai->common_flag_ = 1;
		return true;
	}
	
	if (player->i_ai->common_flag2_) {
		player->i_ai->common_flag1_ = 1;
		player->i_ai->common_flag2_ = 0;
		player->i_ai->change_state(DEFENSE_STATE);
		return true;
	}
	
	if (player->i_ai->check_event_tm()) {
		player->invincible_time = 1;
		//player->i_ai->common_flag1_ = 1;
		//player->i_ai->change_state(DEFENSE_STATE);
		player->i_ai->common_flag2_ = 1;
		
		if (player->pos().x() > 1000) {
			teleport(player, 1372, 400);
		} else if (player->pos().x() <= 1000 && player->pos().x() > 420){
			teleport(player, 706, 219);
		} else {
			teleport(player, 233, 642);
		}
		
		return true;
	}

	return false;
}

bool
DaoLangAI::extra_attack_action(Player * player)
{
	return false;
}

bool
DaoLangAI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

bool
DaoLangAI::extra_defense_action(Player * player)
{
	//take a break
	if (player->i_ai->common_flag1_) {
		player_skill_t* skill = player->select_skill(0, 4120041);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag1_ = 0;
			player->invincible_time = 0;
			player->hit_fly_flag = false;
			player->i_ai->set_event_tm(10000);
			
			player->i_ai->set_hatred(0);
			
			monster_speak(player, 1);
			return true;
		} else {
			player->i_ai->common_flag1_ = 0;
			player->invincible_time = 0;
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	//break end
	if (player->i_ai->check_event_tm()) {
		player->i_ai->set_event_tm(30000);
		player->hit_fly_flag = true;
		
		monster_speak(player, 2);
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	//launch a skill if be attacked in it's rest
	if (player->i_ai->hatred() > 0) {
		//find target
		if (!player->i_ai->target && (player->i_ai->target = get_target(player)) == 0 ) {
			player->i_ai->set_hatred(0);
			return true;
		}
		
		//move to target's pos and launche skill
		Vector3D new_pos(player->i_ai->target->pos().x(), player->i_ai->target->pos().y());
		player->adjust_position_ex(new_pos);
		monster_stand(player);
		player_skill_t* skill = player->select_skill(player->i_ai->target, 4120040);
		if (skill) {
			monster_attack(player, skill);
		}
		
		player->i_ai->set_hatred(0);
		player->i_ai->set_event_tm(30000);
		player->hit_fly_flag = true;
		player->i_ai->change_state(WAIT_STATE);
		
		return true;
	}
	
	return true;
}

/****************************************************************
 *  * Moyi Boss
****************************************************************/

bool MoyiAI::extra_wait_action(Player * player)
{
	map_t* m  = player->cur_map;

	if( is_player_buff_exist( player, 538) )return true;

	if( get_target(player) != NULL && player->i_ai->common_flag_ == 0)
	{
		//召唤利齿
		player_skill_t* skill = player->select_skill(0, 4120049);
		if(skill == NULL )return true;

	    monster_attack(player, skill);
		for(uint32_t i =0; i< 4; i++)
		{
			player->call_monster(11214, 1700 - i*200, 275);
		}	
		teleport(player, 1290, 135);
		player->invincible_time = 1;
		player->i_ai->common_flag_ = 2;
		return true;
	}
	if( player->i_ai->common_flag_ == 2)
	{
		//陷阱召唤
		player_skill_t* skill = player->select_skill(0, 4120048);
		if(skill == NULL)return NULL;

		monster_attack(player, skill);
		for(uint32_t i =0; i< 3; i++)
		{
			player->call_map_summon(3, 1700 - i * 200, 280);
		}
		player->i_ai->common_flag_ = 3;
	}
	if( player->i_ai->common_flag_ == 3)//检查利齿是否被打死
	{
		int count = 0;
		for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it)
		{
			Player* p = *it;
			if (p->role_type == 11214) count++;
		}
		if(count == 0)
		{
			player->i_ai->common_flag_ = 4;
			//神圣绑定
			player->invincible_time = 0;
			teleport(player, 1700, 275);
			player_skill_t* skill = player->select_skill(0, 4120050);
			if(skill != NULL)
			{
				monster_attack(player, skill);
				player->call_monster(11215, 1400, 275);
				player->call_monster(11216, 1800, 300);
			}
		}		
		return true;
	}

	if(player->i_ai->common_flag_ == 4)//检查TT是否被打死
	{
		int flag1 = 0;
		int flag2 = 0;
		for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it)
		{
			Player* p = *it;
			if (p->role_type == 11215) {
				flag1 = 1;
				if( !is_player_buff_exist( player, 537))add_player_buff(player, 537, 0);
				
			}
			if(p->role_type == 11216){
				flag2 = 1;
				if( !is_player_buff_exist( player, 539))add_player_buff(player, 539, 0);
			}
		}

		if(flag1 == 1 && flag2 == 0)
		{
			del_player_buff(player, 539);
		}
		if(flag1 ==0 && flag2 == 1)
		{
			del_player_buff(player, 537);
		}

		if(flag1 == 0 && flag2 == 0)
		{
			del_player_buff(player, 539, 0);
			del_player_buff(player, 537, 0);
			player->i_ai->common_flag_ = 0;
			add_player_buff(player, 538, 0);
			return true;
		}
	}

	return false;
}


bool MoyiAI::extra_attack_action(Player * player)
{
	if( is_player_buff_exist( player, 538) )return true;
	return false;
}

bool MoyiAI::extra_stuck_action(Player* player)
{
	//SuperArmor
	if(player->super_armor){
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if(super_armor_skill){
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	return false;
}

bool MoyiAI::extra_defense_action(Player* player)
{
	return true;
}

/****************************************************************
 *  Hunfu Boss
****************************************************************/
void HunfuAI::move(Player* player, int millisec)
{
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, player->i_ai->ready_skill_id_)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}

	const Player* target = player->i_ai->target;
	if(target)
	{
		player->i_ai->common_flag2_ = target->pos().x();
		player->i_ai->common_flag3_ = target->pos().y();
	}
	if (monster_move(player, millisec)) 
	{
		if (player->select_skill(target, player->i_ai->ready_skill_id_)) 
		{
			player->i_ai->change_state(ATTACK_STATE);
		} 
		else 
		{
			player->i_ai->set_state_keep_tm(rand() % 1 + 1);
			player->i_ai->change_state(WAIT_STATE);
		}
	} 
	else 
	{
		if (player->i_ai->judge_update_tm(1500))
		{
			uint32_t  temp = 0;
			if( is_hp_range(player, 90, 100) || player->is_hp_full() )  
			{
				temp = 5;
			}
			else if( is_hp_range(player, 70, 89))
			{
				temp = 7;
			}
			else if( is_hp_range(player, 50, 69))
			{
				temp = 8;
			}
			else if( is_hp_range(player, 30, 49))
			{
				temp = 9;
			}
			else if( is_hp_range(player, 0, 29))
			{
				temp = 10;
			}

			if( player->i_ai->common_flag_ !=  temp ){
				player->i_ai->common_flag_ = temp;
				player->i_ai->common_flag1_ = 0;				
			}			
			if( player->i_ai->common_flag2_ != 0 && player->i_ai->common_flag3_ != 0 && player->i_ai->common_flag1_ < temp )
			{
				if (player->select_skill(target, 4120051)) {
					monster_stand(player);
					player->i_ai->target_pos.init();
					player->i_ai->change_state(ATTACK_STATE);
				}
							
			}
		}
	}
}
bool HunfuAI::extra_wait_action(Player* player)
{	
	return false;
}

bool HunfuAI::extra_attack_action(Player* player)
{
	
	if( player->i_ai->common_flag2_ != 0 && player->i_ai->common_flag3_ != 0 && player->i_ai->common_flag1_ < player->i_ai->common_flag_ )
	{
		player_skill_t* skill = player->select_skill(0, 4120051);
		if(skill == NULL)return false;
		player->i_ai->common_flag1_ ++;
		monster_attack(player, skill,  player->i_ai->common_flag2_,  player->i_ai->common_flag3_);
		return true;
	}
	return false;
}

bool HunfuAI::extra_stuck_action(Player* player)
{
	if(player->super_armor){
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if(super_armor_skill){
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	return false;
}
		    
bool HunfuAI::extra_defense_action(Player* player)
{
	return false;
}


/****************************************************************
 * * lichi touling Boss
****************************************************************/

bool lichi_toulingAI::check_call_monster_condition(Player* player)
{
	map_t*  m = player->cur_map;	
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it)
	{
		Player* p = *it;
		if( p->role_type == 11240 || p->role_type == 11241 || p->role_type == 11242 ){
			return false;
		}
	}
	return true;
}

bool lichi_toulingAI::extra_wait_action(Player* player)
{
	if(player->i_ai->common_flag1_ >= 13){
		return false;
	}
	
	if( player->i_ai->common_flag_ == 0){
		player->i_ai->set_event_tm(10000);
		player->i_ai->common_flag_ = 1;	
		player->i_ai->common_flag2_ = 0;	
	}

	if( player->i_ai->check_event_tm()){
		
		switch(player->i_ai->common_flag1_)
		{
			case 0:
			case 2:
			case 4:
			case 7:
			case 9:
			{
					player->call_monster(11240, 2588, 409);				
					player->call_monster(11240, 2537, 511);
					player->call_monster(11240, 2629, 604);
					player->call_monster(11240, 2358, 720);	
					player->call_monster(11240, 2336, 626);
					player->call_monster(11240, 2245, 585);	
			}
			break;

			case 1:
			case 3:
			case 5:
			case 8:
			case 10:
			{
					player->call_monster(11242, 593, 431);
					player->call_monster(11242, 411, 560);
					player->call_monster(11242, 434, 726);
					player->call_monster(11242, 640, 765);
					player->call_monster(11242, 430, 720);
					player->call_monster(11242, 433, 718);
			}
			break;

			case 6:
			case 11:
			{
				for(int i =0; i < 6; i++)
				{
					player->call_monster(11241, 1500 + i* 10, 600 + i*10);
				}
			}
			break;

			case 12:
			{
				teleport(player, 1500, 615);
			}
			break;
		}
		player->i_ai->common_flag1_ ++;
		player->i_ai->common_flag2_ = 1;
	}

	if( check_call_monster_condition(player) && player->i_ai->common_flag2_){
		player->i_ai->common_flag_ = 0;
		return true;
	}
	return false;
}

bool lichi_toulingAI::extra_attack_action(Player* player)
{
	return false;
}

bool lichi_toulingAI::extra_stuck_action(Player* player)
{
	if(player->super_armor){
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if(super_armor_skill){
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	return false;	
}

bool lichi_toulingAI::extra_defense_action(Player* player)
{
	return false;
}



/****************************************************************
 *  * * xili Boss
****************************************************************/

bool xili_AI::extra_wait_action(Player* player)
{
	if(  check_call_monster_condition(player) && player->i_ai->common_flag1_ == 1)
	{
		teleport(player, 563, 477);
		player->i_ai->common_flag1_ = 0;	        
	}
	return false;
}
    
bool xili_AI::extra_attack_action(Player* player)
{
	if( check_call_monster_condition(player))
	{
		if( player->i_ai->common_flag1_ == 0)	 
		{
			player_skill_t* skill = player->select_skill(0, 4120046);
			if(skill != NULL)
		 	{
				monster_attack(player, skill);
				player->call_monster(11145, player->pos().x(), player->pos().y()); 
		 		player->i_ai->common_flag1_ = 1;
				teleport(player, 2000, 2000);
				return true;	
			}
		}
	}
	return false;
}
	    
bool xili_AI::extra_stuck_action(Player* player)
{
	if(player->super_armor){
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if(super_armor_skill){
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	return false;
}
		    
bool xili_AI::extra_defense_action(Player* player)
{
	return false;
}

bool xili_AI::check_call_monster_condition(Player* player)
{
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it)
	{
		Player* p = *it;
		if( p->role_type == 11145 ){
			return false;
		}
	}
	return true;	
}


/****************************************************************
	* * huanying mowang Boss
****************************************************************/

int32_t huanying_AI::get_idol_count(Player* player)
{
	 map_t*  m = player->cur_map;
	 int32_t count = 0;
	 for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it)
	 {
		Player* p = *it;
		if( p->role_type == 11214){
			count++;
		}
	 }
	 return count;
}

Player* huanying_AI::get_current_player(Player* player)
{
	map_t*  m = player->cur_map;
	PlayerSet::iterator it = m->players.begin();
	return *it;
}

uint32_t huanying_AI::get_idol_id(Player* player)
{
	return 0;
}

bool huanying_AI::extra_wait_action(Player* player)
{
	if( player->i_ai->common_flag_ == 0){
		player->i_ai->common_flag_ = player->hp;
		player_skill_t* skill = player->select_skill(0, 4120053);
		if(skill != NULL){
			monster_attack(player, skill);
			
			player->call_monster(11243, 400, 200);
			player->call_monster(11243, 400, 250);
			player->call_monster(11243, 400, 300);
			player->call_monster(11243, 500, 350);
			player->call_monster(11243, 600, 300);
			player->call_monster(11243, 600, 250);
			player->call_monster(11243, 600, 200);
			
			
			player->i_ai->common_flag1_ = 1;
		}

		return false;		
	}

	if( get_idol_count(player) == 0 && player->i_ai->common_flag2_ == 1 ){
		//跳回原来的地方
		teleport(player, 400, 400);
		player->i_ai->common_flag2_ = 0;
		return false;
	}

	float temp_hp = player->i_ai->common_flag_  - player->hp;
	if(temp_hp > 0.0  &&  (temp_hp / player->max_hp()) >= 0.07 ){
		switch( player->i_ai->common_flag1_ )
		{
			case 0://召唤
			{
				player_skill_t* skill = player->select_skill(0, 4120053);
				if(skill != NULL){
					monster_attack(player, skill);
					player->call_monster(11243, 400, 200);
					player->call_monster(11243, 400, 250);
					player->call_monster(11243, 400, 300);
					player->call_monster(11243, 500, 350);
					player->call_monster(11243, 600, 300);
					player->call_monster(11243, 600, 250);
					player->call_monster(11243, 600, 200);
					
					
					player->i_ai->common_flag1_ = 1;
				}	
			}
			break;

			case 1://变形
			{
				player_skill_t* skill = player->select_skill(0, 4120054);
				if(skill != NULL){
					monster_attack(player, skill);
					player->call_monster(11214, player->pos().x(), player->pos().y());
					teleport(player, 2000, 2000);
					//清理掉身上所有BUFF
					del_player_all_buff(player);
					player->i_ai->common_flag2_ = 1;
					player->i_ai->common_flag1_ = 2;
				}
			}
			break;

			case 2://闪烁
			{
				player_skill_t* skill = player->select_skill(0, 4120055);
				if( skill != NULL){
					monster_attack(player, skill);
					Player* p = get_current_player(player);
					
					teleport(player, p->pos().x(),  p->pos().y());
					add_player_buff(p, 534, 0);
					player->i_ai->common_flag1_ = 0;
				}	
			}
			break;
		}
		player->i_ai->common_flag_ = player->hp;
	}
	return false;
}

bool huanying_AI::extra_attack_action(Player* player)
{
	return false;
}

bool huanying_AI::extra_stuck_action(Player* player)
{
	if(player->super_armor){
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if(super_armor_skill){
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	return false;
}
		     
bool huanying_AI::extra_defense_action(Player* player)
{
	return true;
}


/****************************************************************
 *   fumo Rabbits Boss
****************************************************************/
int32_t FumoRabbits_AI::check_other_rabbits_count(Player* player)
{
	map_t*  m = player->cur_map;	
	int32_t count = 0;
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) 
	{
		Player* p = *it;
		if( p->role_type == 13039 || p->role_type == 13040 || p->role_type == 13041)
		{
			count ++;
		}
	}
	return count;
}

bool FumoRabbits_AI::extra_wait_action(Player* player)
{
	if( player->i_ai->common_flag2_ == 0){
		if( check_other_rabbits_count(player) == 0){
			teleport(player, 500, 500);
			player->i_ai->common_flag2_ = 1;
			return false;
		}
		return true;
	}	
	return RabbitsAI::extra_wait_action(player);
}

bool FumoRabbits_AI::extra_attack_action(Player* player)
{
	return RabbitsAI::extra_attack_action(player);
}

bool FumoRabbits_AI::extra_stuck_action(Player* player)
{
	return RabbitsAI::extra_stuck_action(player);
}

bool FumoRabbits_AI::extra_defense_action(Player* player)
{
	return RabbitsAI::extra_defense_action(player);
}



/****************************************************************
 *  callcat Boss
****************************************************************/
bool CallCat_AI::extra_wait_action(Player* player)
{
	if( player->i_ai->common_flag_ == 2){
		return false;
	}
	if( check_call_condition(player)){
		switch(player->i_ai->common_flag_)
		{
			case 0:
			{
				player->call_monster(11245, 400, 400);
				player->i_ai->common_flag_ ++;
				return true;
			}
			case 1:
			{
				teleport(player, 400, 400);
				player->i_ai->common_flag_ ++;
				return false;
			}
		}
	}
	return false;
}

bool CallCat_AI::extra_attack_action(Player* player)
{
	return false;
}

bool CallCat_AI::extra_stuck_action(Player* player)
{
	return false;
}

bool CallCat_AI::extra_defense_action(Player* player)
{
	return false;
}

bool  CallCat_AI::check_call_condition(Player* player)
{
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it)
	{
		Player* p = *it;
		if( p->role_type ==  11245 || p->role_type == 11244){
			return false;
		}
	}
	return true;
}



/****************************************************************
 *  *  callcat Boss
****************************************************************/
bool YaoYaoBoss_AI::extra_wait_action(Player* player)
{
	if(get_stone_count(player) == 0){
		player->invincible_time = 0;
		Player* target = get_target(player);
		if(target){
			add_player_buff(target, 577, 0);
		}
	}
	return false;
}
bool YaoYaoBoss_AI::extra_attack_action(Player* player)
{
	if(get_stone_count(player) == 0){
		player->set_skill_cool_time(4120058, 60000);
	}
	return false;
}
bool YaoYaoBoss_AI::extra_stuck_action(Player* player)
{
	if(player->super_armor){
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if(super_armor_skill){
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	return false;
}
bool YaoYaoBoss_AI::extra_defense_action(Player* player)
{
	return false;
}

uint32_t YaoYaoBoss_AI::get_stone_count(Player* player)
{
	int count = 0;
	struct map_t* map = player->cur_map;
	PlayerSet::iterator pItr = map->barriers.begin();
	for( ; pItr != map->barriers.end(); ++pItr)
	{
		Player* p = *pItr;
		if( !p->is_dead() ){
			count++;
		}
	}
	return count;
}


/****************************************************************
 *  *  *  Joe_Step1 Boss
****************************************************************/
uint32_t Joe_Step1AI::get_batman_count( Player* player )
{
	uint32_t count = 0;
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it)
	{
		Player* p = *it;
		if( p->role_type == 11251){
			count++;
		}
	}
	return count;
}

uint32_t Joe_Step1AI::get_idol_count( Player* player )
{
	uint32_t count = 0;
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it)
	{
		Player* p = *it;
		if( p->role_type == 11250){
			count++;
		}
	}
	return count;
}


bool  Joe_Step1AI::extra_wait_action(Player* player)
{
	if( player->i_ai->common_flag_ == 2)
	{
		return true;
	}
	
	if( player->i_ai->common_flag_ == 0 && get_batman_count(player) == 0)
	{
		for(uint32_t i =0; i<3; i++)
		{
			player->call_monster(11251,  player->pos().x(),  player->pos().y());
		}
		
		player->i_ai->common_flag_ = 1;
		return false;
	}
	if( player->i_ai->common_flag_ == 1 && get_idol_count(player) == 0)	
	{
		player->call_monster(11250, player->pos().x(), player->pos().y());
		player->i_ai->common_flag_ = 0;
		return false;
	}
	if( is_hp_range(player,  0, 20) )
	{
		player->invincible_time	= 1;
		player->i_ai->common_flag_ = 2;
		player->call_monster(13072, player->pos().x(), player->pos().y());
		teleport(player, 2000, 2000);
		return false;
	}
	return false;
}

bool  Joe_Step1AI::extra_attack_action(Player* player)
{
	return false;
}

bool  Joe_Step1AI::extra_stuck_action(Player* player)
{
	if(player->super_armor){
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if(super_armor_skill){
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	return false;
}	

bool  Joe_Step1AI::extra_defense_action(Player* player)
{
	return false;
}

/****************************************************************
 *  *  *  *  Joe_Step2 Boss
****************************************************************/


bool Joe_Step2AI::extra_wait_action(Player* player)
{
	if( player->i_ai->common_flag1_ == 0){
		monster_speak(player, 2);
		player->i_ai->common_flag1_ = 1;		
	}
	if( is_hp_range(player,  0, 20) && player->i_ai->common_flag_ == 0)
	{
		player->recover_hp(50);
		player_skill_t* skill = player->select_skill(0, 4120064);
        if(skill != NULL){
			monster_attack(player, skill);
			player->i_ai->common_flag_ = 1;
		}		
	}
	return false;
}

bool Joe_Step2AI::extra_attack_action(Player* player)
{
	return false;
}
	    
bool Joe_Step2AI::extra_stuck_action(Player* player)
{
	if(player->super_armor){
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if(super_armor_skill){
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	return false;
}
		    
bool Joe_Step2AI::extra_defense_action(Player* player)
{
	return false;
}

/****************************************************************
 *  *  *  *  *  farui Boss
****************************************************************/

bool Farui_AI::extra_wait_action(Player* player)
{
	Player* target = get_target(player);
	if( target == NULL)return false;
	player_skill_t* skill = player->select_skill(player, 4120067);
	if(skill == NULL)return false;
	teleport(player, target->pos().x(), target->pos().y());
	monster_attack_without_noti(player, skill);
	return false;

}

bool Farui_AI::extra_attack_action(Player* player)
{
	return false;
}

bool Farui_AI::extra_stuck_action(Player* player)
{
	/*uint32_t on_hit_times = player->i_ai->common_flag1_;
	if (on_hit_times >= 5) {
		player->i_ai->change_state(DEFENSE_STATE);
		return true;
	} */ 

	if(player->super_armor){
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if(super_armor_skill){
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}

	player->i_ai->change_state(WAIT_STATE);
	return false;
}

bool Farui_AI::extra_defense_action(Player* player)
{
	/*
	if (player->i_ai->common_flag1_ >= 5) {
		int skill_id = 4029999;
		player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
		if (skill) {
			monster_attack(player, skill);
			ERROR_LOG("FARRUI DEFENSE USE BA TI!");
		}
		player->i_ai->common_flag1_ = 0;
	}*/

	player->i_ai->change_state(WAIT_STATE);
	return true;
}

/****************************************************************
 *  *  *  *  *  *  farui Boss
****************************************************************/
bool Lengyue_AI::extra_wait_action(Player* player)
{
	Player* target = get_target(player);
	if( target == NULL)return false;
	player_skill_t* skill = player->select_skill(player, 4120065);
	if(skill == NULL)return false;
	teleport(player, target->pos().x(), target->pos().y());
	monster_attack_without_noti(player, skill);
	return false;

}

bool Lengyue_AI::extra_attack_action(Player* player)
{
	return false;
}

bool Lengyue_AI::extra_stuck_action(Player* player)
{
	if(player->super_armor){
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if(super_armor_skill){
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	return false;
}

bool Lengyue_AI::extra_defense_action(Player* player)
{
	return false;
}

/****************************************************************
 *  *  *  *  *  *  *  fish Boss
****************************************************************/

bool Fish_AI::extra_wait_action(Player* player)
{	
	if( player->i_ai->common_flag_ == 0)
	{
		player->invincible_time = 1;
		del_player_all_buff(player, 2);
		if( !is_player_aura_exist(player, 2)){
			add_player_aura(player, 2, 0);
		}
	

		if( get_bonbon_count(player) > 0 )
		{
			uint32_t x =0, y = 0;
			if( get_random_bonbon_pos(player, x, y))
			{
				if( player->pos().x() != int32_t(x) || player->pos().y() != (int32_t)y){
					//teleport(player, x - 100, y);	
					player->i_ai->change_state(MOVE_STATE);
					return true;
				}
				player->dir = dir_right;
				monster_stand(player);
				player_skill_t* skill = player->select_skill(player, 4120074);
				if( skill ){
					monster_attack(player, skill);
				}		
			}
		}
		else
		{
			player->i_ai->common_flag_ = 1;
			return true;
		}
	}
	else if( player->i_ai->common_flag_ == 1)
	{
		player_skill_t* p_skill = player->select_skill(0, 4120075);

		if(p_skill == NULL)return true;
	
		if( player->i_ai->common_flag1_ == 6)
		{
			player->i_ai->common_flag_ = 2;
			player->i_ai->common_flag1_  = 0;
			return true;
		}
		if( player->i_ai->common_flag1_ %2 == 0)
		{
			if( player->pos().x() != 796 && player->pos().y() != 532)
			{
				teleport(player, 796, 532);
			}
			player->dir = dir_right;
			monster_stand(player); 
			monster_attack(player, p_skill);    
			player->i_ai->common_flag1_ ++;
			return true;
		}
		if( player->i_ai->common_flag1_ %2 == 1)
		{
			if( player->i_ai->common_flag1_ == 5){
				p_skill = player->select_skill(0, 4120077);
			}
			if(p_skill == NULL)return true;
			player->dir = dir_left;
			monster_stand(player);
			monster_attack(player, p_skill);
			player->i_ai->common_flag1_ ++;
			return true;
		}

		return true;
	}
	else if( player->i_ai->common_flag_ == 2)
	{
		//删除身上的防DEBUFF的东西
		del_player_aura(player, 2);
		player->invincible_time = 0;
		player_skill_t* p_skill = player->select_skill(0, 4120076);
		if(p_skill == NULL)return true;
		monster_attack(player, p_skill);
		add_player_buff(player, 575, 0);
		player->i_ai->common_flag_ = 3;
		return true;
	}
	else
	{
		player->call_monster(19127, 1483, 550, barrier_team);
		player->call_monster(19127, 833, 428, barrier_team);
		player->call_monster(19127, 1126, 678, barrier_team);
		player->i_ai->common_flag_ = 0;
		return true;
	}
	
		
	/*
	monster_stand(player);
	player_skill_t* p_skill = player->select_skill(0, 4020006);
	if(p_skill == NULL)return true;
	if( player->i_ai->common_flag_ == 0 )
	{
		teleport(player, 200, 511);
		player->dir = dir_right;
		monster_stand(player);
		monster_attack(player, p_skill);	
		player->i_ai->common_flag_ = 1;
	}
	else 
	{
		player->dir = dir_left;
		monster_stand(player);
		monster_attack(player, p_skill);
		player->i_ai->common_flag_ = 0;
	}
	*/
	return true;
}

bool Fish_AI::extra_attack_action(Player* player)
{
	return true;
}
	
bool Fish_AI::extra_stuck_action(Player* player)
{
	player->i_ai->change_state(WAIT_STATE);
	return true;
}

bool Fish_AI::extra_defense_action(Player* player)
{
	player->i_ai->change_state(WAIT_STATE);
	return true;
}

uint32_t Fish_AI::get_bonbon_count(Player* player)
{
	uint32_t count = 0;
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->barriers.begin(); it != m->barriers.end(); ++it)
	{
		Player* p = *it;
		if(p && p->role_type == 19127 && !p->is_dead()){
			count++;
		}
	}
	return count;	
}

bool Fish_AI::get_random_bonbon_pos(Player* player, uint32_t& x,  uint32_t& y)
{
	x = 0;
	y = 0;
	bool ret = false;
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->barriers.begin(); it != m->barriers.end(); ++it)
	{
		Player* p = *it;
		if(p && p->role_type == 19127 && !p->is_dead()){
			x = p->pos().x() - 100;
			y = p->pos().y();
			ret = true;
			break;
		}
	}
	return ret;
}

void Fish_AI::move(Player* player, int millisec)
{
	if( get_bonbon_count(player))
	{
		uint32_t x =0, y = 0;
		if( get_random_bonbon_pos(player, x, y))
		{
			if( player->pos().x() == (int32_t)x && player->pos().y() == (int32_t)y)
			{
				player->i_ai->change_state(WAIT_STATE);
				return;
			}
			//const KfAstar::Points* pts = player->cur_map->path->findpath(KfAstar::Point(player->pos().x(), player->pos().y()),
					            //KfAstar::Point( x  , y ) );
			KfAstar::Points* pts = NULL;
			KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
			if(p_star)
			{
				pts = (KfAstar::Points*)p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()), KfAstar::Point( x ,y));
			}

			if (pts){
				player->i_ai->target_path.assign(pts->begin(), pts->end());
				get_one_target_pos(player);
				monster_move_ex(player, millisec);				
			}

		}
	}
	else
	{
		player->i_ai->change_state(WAIT_STATE);
	}
	return ;
}



/****************************************************************
  *  *  *  *  * Balrog Boss
****************************************************************/

bool Balrog_AI::extra_wait_action(Player* player)
{
	if( player->get_be_hit_count() >= 5)
	{
		player_skill_t* p_skill = player->select_skill(0, 4120078);
		if(p_skill == NULL)return true;
		monster_attack(player, p_skill);
		player->reset_be_hit_count();
		return true;
	}
	call_volcano(player);
	return false;
}

bool Balrog_AI::extra_attack_action(Player* player)
{
	return false;
}

bool Balrog_AI::extra_stuck_action(Player* player)
{
	return false;
}

bool Balrog_AI::extra_defense_action(Player* player)
{
	return false;
}


bool Balrog_AI::call_volcano(Player* player)
{	
	player_skill_t* p_skill = player->select_skill(0, 4120079);
	if( p_skill == NULL)return false;

	monster_attack(player, p_skill);
	uint32_t monsters[] = { 19130, 19131, 19132 };	
	player->cur_map->del_barriers_from_map();

	for(uint32_t i =0; i< 10; i++)
	{
		uint32_t x = ranged_random(135, 1690);
		uint32_t y = ranged_random(489, 707);
		uint32_t monster_id = monsters[  ranged_random(0, 2) ];
		player->call_monster(monster_id, x, y, barrier_team);	
	}
	
	return true;
}


bool TuMars_AI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag2_) {
		if ((player->i_ai->target != 0) && !player->check_distance(player->i_ai->target, 150)) {//上次释放落石的目标还存在则继续释放
			uint32_t skill_id = 4120082;
			const Player * target = player->i_ai->target;
			player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
			if (skill) {
				monster_attack(player, skill, target->pos().x(), target->pos().y()); 
				player->i_ai->common_flag2_ --;
			}
			player->i_ai->set_state_keep_tm(1);
		} else {
			player->i_ai->common_flag2_ = 0;
		}
		player->invincible_time = player->i_ai->common_flag2_;
		return true;
	} 

	if (player->get_be_hit_count() >= 10) {
		player->invincible_time = 1;
		player->i_ai->change_state(DEFENSE_STATE);
		return true;
	}

	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		uint32_t cur_time = time(0);

		int rand_num = rand() % 10; 

		player_skill_t * skill = player->select_skill(target); 
		if (skill) {
			TRACE_LOG("TUMARS USE SKILL----%u", skill->skill_id);
			monster_attack(player, skill);
		} else if (player->i_ai->common_flag1_ + 8 < cur_time && rand_num > 5) {
			if (!player->check_distance(target, 200)) {
				int skill = 4120082;
				player_skill_t * pskill = player->select_skill(target, skill); 
				if (pskill) {
					player->i_ai->common_flag1_ = cur_time; 
					player->i_ai->common_flag2_ = 2;
					monster_attack(player, pskill, target->pos().x(), target->pos().y());
					player->invincible_time = player->i_ai->common_flag2_;
					player->i_ai->set_state_keep_tm(1);
					return true;
				}
			}
		} else { 
			player->i_ai->common_flag1_ += rand_num % 3;
			player->i_ai->change_state(MOVE_STATE);
		}
	}
	return true;
}


bool TuMars_AI::extra_attack_action(Player * player)
{
	if (player->i_ai->target == 0 ) {
		return true;
	}

	player->i_ai->change_state(WAIT_STATE);
	return false;
}

bool TuMars_AI::extra_stuck_action(Player * player)
{
	if (player->get_be_hit_count() >= 6 ) { //挨打超过5次进入防御状态
		player->i_ai->change_state(DEFENSE_STATE);
	} else { 
		player->i_ai->change_state(WAIT_STATE);
	}
	return true;
}

bool TuMars_AI::extra_defense_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) { //霸体
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}

	if (player->get_be_hit_count() >= 6) {//挨打次数超过一定次数则托马斯旋转
		int skill_id = 4120078;
		player_skill_t* skill = player->select_skill(0, skill_id);
		if (skill) {
			monster_attack(player, skill);
			TRACE_LOG("TUO MARS>>>>>>>&&&&&");
			player->i_ai->set_state_keep_tm(2);
		}
		player->reset_be_hit_count(); //重置被打次数
		player->invincible_time = 0;
	}

	player->i_ai->change_state(WAIT_STATE);
	return true;
}



bool Shark_AI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag1_) { //剩余攻击次数
		if (player->i_ai->common_flag3_) { //路径存在
			fish_attack(player);
		} else {
			if (player->i_ai->common_flag2_) {//鱼形态 
				int skill_id = 4120087; //出土
				player_skill_t* skill = player->select_skill(0, skill_id);
				if (skill) {
		//			ERROR_LOG("CHUCHUCHUC");
					monster_attack(player, skill);
					player->i_ai->common_flag2_ = 0;
					player->i_ai->set_state_keep_tm(2);
				}
			}
			else {
				get_next_pos(player);//寻找路径
			}
		}
	} else { 
		if (player->i_ai->common_flag2_) { //鱼形态
			int skill_id = 4120087; //出土
			player_skill_t* skill = player->select_skill(0, skill_id);
			if (skill) {
		//		ERROR_LOG("CHUCHUCHUC");
				monster_attack(player, skill);
				player->i_ai->common_flag2_ = 0;
				player->i_ai->set_state_keep_tm(2);
			}
			return true;
		}

		player->invincible = 0;
		uint32_t cur_time = time(0);
		if (player->i_ai->common_flag_ == 0) { //初始
			player->i_ai->common_flag_ = cur_time -  rand()%10;
		}

		if (player->i_ai->common_flag_ + 28 < cur_time) {
			player->i_ai->common_flag1_ = 4;
			player->i_ai->common_flag_ = cur_time;//store last skill time
			player->invincible = 1;
			return true;
		} 	

		player->i_ai->target = 0;
		if ((player->i_ai->target = get_target(player)) != 0) {
			const Player * target = player->i_ai->target;
			player_skill_t * skill = player->select_skill(target);
			if (skill) {
				monster_attack(player, skill);
			} else {
				player->i_ai->change_state(MOVE_STATE);
			}
		} 
	}
	return true;
	
}

bool Shark_AI::extra_attack_action(Player * player)
{
	return true;
}

bool Shark_AI::extra_stuck_action(Player * player)
{
	if (player->get_be_hit_count() >= 6 ) { //挨打超过5次进入防御状态
		player->i_ai->change_state(DEFENSE_STATE);
	} else { 
		player->i_ai->change_state(WAIT_STATE);
	}
	return true;
}

bool Shark_AI::extra_defense_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) { //霸体
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}

	if (player->get_be_hit_count() >= 5) {//挨打次数超过一定次数则横扫千军
		int skill_id = 4120078;
		player_skill_t* skill = player->select_skill(0, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->set_state_keep_tm(2);
			player->reset_be_hit_count(); //重置被打次数
		}
	}
	player->i_ai->change_state(WAIT_STATE);
	return true;
}

void  Shark_AI::fish_attack(Player * player)
{
	if (player->i_ai->common_flag2_) { //鱼形态
		int skill_id = 4120085;
		player_skill_t* skill = player->select_skill(0, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag1_ --; //剩余冲击次数
			player->i_ai->common_flag3_ = 0; //冲击路线用完
		}
	} else {  //人形态
		int skill_id = 4120086; //入土
		player_skill_t* skill = player->select_skill(0, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag2_ = 1;
		}
	}

}

void Shark_AI::get_next_pos(Player * player)
{
	int x;
	int y;

	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		if (target->pos().x() < 200) {
			x = target->pos().x() + 200;
		} else if (target->pos().x() > 800) {
			x = target->pos().x() - 200;
		} else {
			x = player->pos().x();
		}

		if ((target->pos().y() > 580 && target->pos().y() < 700) )  {
			if (target->pos().x() > 400 && target->pos().x() < 700) {
				x = 530 + rand() % 100; 
			}

		}else if (target->pos().y() > 300 && target->pos().y() < 400) {
			if (abs(target->pos().x() - player->pos().x()) > 200) {
				if (target->pos().x() > 700 ) {
					x = 730 + rand() % 100;
				} else {
					x = 200 + rand() % 200;
				}
			} else {
				if (target->pos().x() < 700 && player->pos().x() > 700) {
					x = 200 + rand() % 100;
				} else if (player->pos().x() < 700 && target->pos().x() > 700) {
					x = 730 + rand() % 100;
				}	
			}
		}	
	
		y = target->pos().y() - 20 + rand() % 40;
		if ( x < 100 &&  y > 100 && y < 500) {
			x += 100;//地图左上角存在某个不可行走区域
		}
		teleport(player, x, y);
		if (x >= target->pos().x()) {
			player->dir = dir_left;
		} else {
			player->dir = dir_right;
		} 

		monster_stand(player);
		player->i_ai->common_flag3_ = 1;
		player->i_ai->set_state_keep_tm(1);
	} else { //无目标清空所有状态
		player->i_ai->common_flag1_ = 0;
		player->i_ai->common_flag2_ = 0;
		player->i_ai->common_flag3_ = 0;
		player->i_ai->set_state_keep_tm(2);
	}
}

bool Dmhunter_AI::extra_wait_action(Player *player)
{
	uint32_t cur_time = time(0);
	if (player->i_ai->common_flag1_ == 0) {
		int rand_num = rand() % 7;
		player->i_ai->common_flag1_ = time(0) - rand_num;
	} else if (player->i_ai->common_flag1_ + 15 < cur_time) {
		if (!is_player_buff_exist(player, 27)) {
			add_player_buff(player, 27, 0, 7);
			player->i_ai->common_flag1_ = cur_time;
			return true;
		}
	}

	if (is_player_buff_exist(player, 27) && check_ghost_fire(player)) {
		player->call_map_summon(7, player->pos().x(), player->pos().y(), false);
	} 

	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
			ERROR_LOG("DMHUNTER SKILL %u", skill->skill_id);
		} else {
			player->i_ai->change_state(MOVE_STATE);
		}
	} else {
		player->i_ai->set_state_keep_tm(2);
	}

	return true;
}

bool Dmhunter_AI::extra_attack_action(Player *player)
{
	return false;
}

bool Dmhunter_AI::extra_stuck_action(Player *player)
{
	if (player->get_be_hit_count() >= 6 ) { //挨打超过5次进入防御状态
		player->i_ai->change_state(DEFENSE_STATE);
	} else { 
		player->i_ai->change_state(WAIT_STATE);
	}

	return true;
}

bool Dmhunter_AI::extra_defense_action(Player *player)
{
	//SuperArmor
	if (player->super_armor) { //霸体
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}

	if (player->get_be_hit_count() >= 5) {//挨打次数超过一定次数则横扫千军
		int skill_id = 4120078;
		player_skill_t* skill = player->select_skill(0, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->set_state_keep_tm(2);
			player->reset_be_hit_count(); //重置被打次数
		}
	}
	player->i_ai->change_state(WAIT_STATE);
	return true;

}

void Dmhunter_AI::move(Player * player, int millisec)
{
	uint32_t cur_time = time(0);
	if (player->i_ai->common_flag1_ + 15 < cur_time) {
		if (!is_player_buff_exist(player, 27)) {
			add_player_buff(player, 27, 0, 7);
			player->i_ai->common_flag1_ = cur_time; 
			return;
		}
	}

	if (is_player_buff_exist(player, 27) && check_ghost_fire(player)) {
		player->call_map_summon(7, player->pos().x(), player->pos().y(), false);
	}


	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, 4020004)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}

	const Player * target = player->i_ai->target;

	//move to target pos
	if (monster_move(player, millisec)) {
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
			ERROR_LOG("DMHUNATER SKILL %u", skill->skill_id);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:attack when monster close to player
		if (player->i_ai->judge_update_tm(1000)) {
				player->i_ai->change_state(WAIT_STATE);
		} 
	}
} 

bool Dmhunter_AI::check_ghost_fire(Player * player)
{
	for (std::list<map_summon_object *>::iterator it = player->btl->map_summon_list.begin();
			it != player->btl->map_summon_list.end(); ++ it) {
		if (player->check_distance(*it, 50)) {
			return false;
		}
	}
	return true;
}




/****************************************************************
 *  *  *  *  *  *  *  *  worm ai
****************************************************************/


/*******************************************
	元素炼狱Boss 

 * *****************************************/
bool YuanSu_AI::extra_wait_action(Player * player)
{
	uint32_t cur_time = time(0);
	if (player->i_ai->common_flag1_ == 0) { //初始化
		uint32_t rand_num = rand() % 2;
		uint32_t init_bufid = 33 + rand_num;
		player->i_ai->common_flag3_ = rand_num;
		add_player_buff(player, init_bufid, 0, 10);
	//	ERROR_LOG("YUAN ADD BUFF %u", init_bufid);
		player->i_ai->common_flag1_ = cur_time; 

	} else if (player->i_ai->common_flag1_ + 15 < cur_time) {
		if (player->i_ai->common_flag3_ == 0) 
		{ 
			add_player_buff(player, 33, 0, 15);
			player->i_ai->common_flag3_ = 1;
			player->i_ai->common_flag1_ = cur_time; 
		} else {
			add_player_buff(player, 34, 0, 15);
			player->i_ai->common_flag3_ = 0;
			player->i_ai->common_flag1_ = cur_time;
		}
	}

	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
		} else {
			player->i_ai->change_state(MOVE_STATE);
		}

	} else {
		player->i_ai->set_state_keep_tm(2);
	}	

	return true;
}

bool YuanSu_AI::extra_stuck_action(Player * player)
{
	if (player->get_be_hit_count() >= 6 ) { //挨打超过5次进入防御状态
		player->i_ai->change_state(DEFENSE_STATE);
	} else { 
		player->i_ai->change_state(WAIT_STATE);
	}
	return true;

}

bool YuanSu_AI::extra_defense_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) { //霸体
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}

	if (player->get_be_hit_count() >= 5) {//挨打次数超过一定次数则横扫千军
		int skill_id = 4120078;
		player_skill_t* skill = player->select_skill(0, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->set_state_keep_tm(2);
			player->reset_be_hit_count(); //重置被打次数
		}
	}
	player->i_ai->change_state(WAIT_STATE);
	return true;
}

void YuanSu_AI::move(Player * player, int millisec)
{
	if (player->i_ai->common_flag1_ + 15 < time(0)) {
		if (player->i_ai->common_flag3_ == 0) { 
			add_player_buff(player, 33, 0, 15);
			player->i_ai->common_flag3_ = 1;
		} else {
			add_player_buff(player, 34, 0, 15);
			player->i_ai->common_flag3_ = 0;
		}
		player->i_ai->common_flag1_ = time(0);
		return;
	}

	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, 4020004)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}

	const Player * target = player->i_ai->target;

	//move to target pos
	if (monster_move(player, millisec)) {
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:attack when monster close to player
		if (player->i_ai->judge_update_tm(1000)) {
				player->i_ai->change_state(WAIT_STATE);
		} 
	}

}

/*******************************************
	三段梅花流Boss 
 * *****************************************/
bool ThreeWoodBossL_AI::extra_wait_action(Player * player)
{
    uint32_t cur_time = time(0);
    if (player->i_ai->common_flag_ == 0) {

        if (!(player->is_monster_in_cur_map(11318))) {
            player->call_monster(11320, 300, 400);
            player->i_ai->common_flag_ = 1;
            //ERROR_LOG("trace L [%u] flag=[%u]", player->role_type, player->i_ai->common_flag_);
        }
    } else if (player->i_ai->common_flag_ == 1) {

        if ( !(player->is_monster_in_cur_map(11320)) ) {
            player->i_ai->common_flag_ = 2;
            teleport(player, 300, 400);
            //ERROR_LOG("trace L [%u] flag=[%u]", player->role_type, player->i_ai->common_flag_);
        }
    } else  {
        uint32_t pos[5][2] = {{300, 400}, {200, 280}, {400, 280}, {200, 550}, {400, 550}};
        //change time every 10 second
        if (player->i_ai->common_flag1_ == 0 || cur_time > (player->i_ai->common_flag1_ + 10) ) {
            player->i_ai->common_flag1_ = cur_time;
        }

        if (player->hp < (int)player->i_ai->common_flag3_) {
            if ( (rand() % 100) < 50 ) {
                int rand_num = rand();
                int x_offset = (rand_num % 50 + 350 * (rand_num % 2)) - 200;
                int y_offset = (rand_num % 100) - 100;
                call_monster_to_map(player->cur_map, player->btl, 11337 + (rand_num % 4), 
                    300 + x_offset, 400 + y_offset, neutral_team_1);
            }
        }
        player->i_ai->common_flag3_ = player->hp;
        if (cur_time < (player->i_ai->common_flag1_+3) && cur_time >= player->i_ai->common_flag1_) {
            if (player->i_ai->common_flag2_ == 0) {
                player->cur_map->set_special_monster_dead(11322);
                int rand_num = rand() % 5;
                teleport(player, pos[rand_num][0], pos[rand_num][1]); 
                //ERROR_LOG("pos BOSS [%u %u]", pos[rand_num][0], pos[rand_num][1]);
                player->invincible_time = true;
                player->cur_map->mon_lv = player->lv;
                int i = 0;
                for ( ; i < 5; i++) {
                    if (i != rand_num) {
                        call_monster_to_map(player->cur_map,player->btl,11322,pos[i][0],pos[i][1],barrier_team);
                        //ERROR_LOG("pos [%u %u]", pos[i][0], pos[i][1]);
                    }
                }
                uint32_t skill_id = 4120106;
                player_skill_t * skill = player->select_skill(0, skill_id);
                if (skill) {
                    monster_attack(player, skill);
                }
                player->i_ai->common_flag2_ = 1;
            }
        } else {
            if (player->i_ai->common_flag2_ == 1) {
                player->invincible_time = false;
                player->i_ai->common_flag2_ = 0;

            }
        }
    }
    return true;
}

void ThreeWoodBossL_AI::move(Player * player, int millisec)
{
    return ;
}
bool ThreeWoodBossR_AI::extra_wait_action(Player * player)
{
    uint32_t cur_time = time(0);
    if (player->i_ai->common_flag_ == 0) {
        if (!(player->is_monster_in_cur_map(11319))) {
            player->call_monster(11321, 900, 400);
            player->i_ai->common_flag_ = 1;
            //ERROR_LOG("trace R [%u] flag=[%u]", player->role_type, player->i_ai->common_flag_);
        }
    } else if (player->i_ai->common_flag_ == 1) {

        if ( !(player->is_monster_in_cur_map(11321)) ) {
            player->i_ai->common_flag_ = 2;
            teleport(player, 900, 400);
            //ERROR_LOG("trace R [%u] flag=[%u]", player->role_type, player->i_ai->common_flag_);
        }
    } else  {
        uint32_t pos[5][2] = {{900, 400}, {800, 280}, {1000, 280}, {800, 550}, {1000, 550}};
        if (player->i_ai->common_flag1_ == 0 || cur_time > (player->i_ai->common_flag1_ + 10)) {
            player->i_ai->common_flag1_ = cur_time;
        }

        if (player->hp < (int)player->i_ai->common_flag3_) {
            if ( (rand() % 100) < 50 ) {
                int rand_num = rand();
                int x_offset = (rand_num % 50 + 350 * (rand_num % 2)) - 200;
                int y_offset = (rand_num % 100) - 100;
                call_monster_to_map(player->cur_map, player->btl, 11337 + (rand_num % 4), 
                    900 + x_offset, 400 + y_offset, neutral_team_1);
            }
        }
        player->i_ai->common_flag3_ = player->hp;

        if (cur_time < (player->i_ai->common_flag1_+3) && cur_time >= player->i_ai->common_flag1_) {
            if (player->i_ai->common_flag2_ == 0) {
                player->cur_map->set_special_monster_dead(11323);
                int rand_num = rand() % 5;
                teleport(player, pos[rand_num][0], pos[rand_num][1]); 
                
                player->invincible_time = true;
                player->cur_map->mon_lv = player->lv;
                int i = 0;
                for ( ; i < 5; i++) {
                    if (i != rand_num) {
                        call_monster_to_map(player->cur_map,player->btl,11323,pos[i][0],pos[i][1],barrier_team);
                        //ERROR_LOG("pos [%u %u]", pos[i][0], pos[i][1]);
                    }
                }
                uint32_t skill_id = 4120106;
                player_skill_t * skill = player->select_skill(0, skill_id);
                if (skill) {
                    monster_attack(player, skill);
                }
                player->i_ai->common_flag2_ = 1;

/*
                map_t*  m = player->cur_map;
                for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) {
                    Player* p = *it;
                    if ( p->role_type == 11323) {
                        if ( i == rand_num ) i++;
                        teleport(p, pos[i][0], pos[i][1]);
                        ERROR_LOG("pos [%u] [%u %u]", p->role_type, pos[i][0], pos[i][1]);
                        i++;
                    }
                }*/ 
            }
        } else {
            if (player->i_ai->common_flag2_ == 1) {
                player->invincible_time = false;
                player->i_ai->common_flag2_ = 0;

            }
        }
    }
    return true;
}

void ThreeWoodBossR_AI::move(Player * player, int millisec)
{
    return ;
}

bool ConPP_AI::extra_wait_action(Player * player) 
{
	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
		} else {
			uint32_t cur_time = get_now_tv()->tv_sec;

			if (player->i_ai->common_flag1_ == 0) {
				player->i_ai->common_flag1_ = cur_time + rand()%4 + 2;
				player->i_ai->set_run(true);
			} else if (player->i_ai->common_flag1_ < cur_time) {
				player->i_ai->set_run(true);
			} else if (player->i_ai->common_flag1_ >= cur_time) {
				player->i_ai->set_run(false);
				player->i_ai->common_flag1_ = cur_time + rand() % 7 - 4;
			}

			player->i_ai->change_state(MOVE_STATE);
		}

	} else {
		player->i_ai->set_state_keep_tm(2);
	}	
	return true;
}


bool ConAoTian_AI::extra_wait_action(Player * player)
{
	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
		} else {
			uint32_t cur_time = get_now_tv()->tv_sec;

			if (player->i_ai->common_flag1_ == 0) {
				player->i_ai->common_flag1_ = cur_time + rand()%4 + 2;
				player->i_ai->set_run(true);
			} else if (player->i_ai->common_flag1_ < cur_time) {
				player->i_ai->set_run(true);
			} else if (player->i_ai->common_flag1_ >= cur_time) {
				player->i_ai->set_run(false);
				player->i_ai->common_flag1_ = cur_time + rand() % 7 - 4;
			}

			player->i_ai->change_state(MOVE_STATE);
		}

	} else {
		player->i_ai->set_state_keep_tm(2);
	}	
	return true;
}


bool ConYiEr_AI::extra_wait_action(Player * player)
{
	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
		//	ERROR_LOG("YI ER USE SKILL %u", skill->skill_id);
		} else {
			player->i_ai->change_state(MOVE_STATE);
		}

	} else {
		player->i_ai->set_state_keep_tm(2);
	}	
	return true;
}

void ConYiEr_AI::move(Player *player, int timediff)
{

	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, 0)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	const Player * target = player->i_ai->target;

	uint32_t cur_time = get_now_tv()->tv_sec;

	if (player->i_ai->common_flag1_ == 0) {
		player->i_ai->common_flag1_ = cur_time + rand()%4 + 2;
		player->i_ai->set_run(true);
	} else if (player->i_ai->common_flag1_ < cur_time) {
		player->i_ai->set_run(true);
	} else if (player->i_ai->common_flag1_ >= cur_time) {
		player->i_ai->set_run(false);
		player->i_ai->common_flag1_ = cur_time + rand() % 7 - 4;
	}

	//move to target pos
	if (monster_move(player, timediff)) {
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
		//	ERROR_LOG("YIER USE SKILL %u", skill->skill_id);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:attack when monster close to player
		if (player->i_ai->judge_update_tm(1000)) {
				player->i_ai->change_state(WAIT_STATE);
		} 
	}
}


bool ConDaZhu_AI::extra_wait_action(Player * player)
{
	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
		//	ERROR_LOG("YI ER USE SKILL %u", skill->skill_id);
		} else {
			uint32_t cur_time = get_now_tv()->tv_sec;
			if (player->i_ai->common_flag1_ == 0) {
				player->i_ai->common_flag1_ = cur_time + rand()%4 + 2;
				player->i_ai->set_run(true);
			} else if (player->i_ai->common_flag1_ < cur_time) {
				player->i_ai->set_run(true);
			} else if (player->i_ai->common_flag1_ >= cur_time) {
				player->i_ai->set_run(false);
				player->i_ai->common_flag1_ = cur_time + rand() % 7 - 4;
			}

			player->i_ai->change_state(MOVE_STATE);
		}

	} else {
		player->i_ai->set_state_keep_tm(2);
	}	

	return true;
}

/*******************************************
	淘汰赛专用三段梅花流Boss 
 * *****************************************/
bool TaoTaiBoss_AI::extra_wait_action(Player * player)
{
    uint32_t live_pos[2] = {260, 330};
    uint32_t monster_1 = 11341;
    uint32_t monster_2 = 11343;
    uint32_t monster_3 = 11345;
    int live_offset = 90;
    if ( abs(player->pos().x() - 100) > abs(player->pos().x() - 1400) ) {
        live_pos[0] = 1226;
        monster_1 = 11342;
        monster_2 = 11344;
        monster_3 = 11346;
        live_offset = -90;
    }
    uint32_t cur_time = time(0);
    if (player->i_ai->common_flag_ == 0) {

        if (!(player->is_monster_in_cur_map(monster_1))) {
            player->call_monster(monster_2, live_pos[0], live_pos[1], (live_offset > 0 ? 1:2));
            player->i_ai->common_flag_ = 1;
            ERROR_LOG("trace  [%u] pos[%u %u]", player->role_type, live_pos[0], live_pos[1]);
        }
    } else if (player->i_ai->common_flag_ == 1) {

        if ( !(player->is_monster_in_cur_map(monster_2)) ) {
            player->i_ai->common_flag_ = 2;
            teleport(player, live_pos[0], live_pos[1]);
            ERROR_LOG("trace [%u] pos[%u %u]", player->role_type, live_pos[0], live_pos[1]);
        }
    } else  {
        uint32_t pos[5][2] = {{0, 0},};//{{300, 400}, {200, 280}, {400, 280}, {200, 550}, {400, 550}};
        pos[0][0] = live_pos[0];
        pos[0][1] = live_pos[1];
        pos[1][0] = live_pos[0] - 100 + live_offset;
        pos[1][1] = live_pos[1] - 120;
        pos[2][0] = live_pos[0] + 100 + live_offset;
        pos[2][1] = live_pos[1] - 120;
        pos[3][0] = live_pos[0] - 100;
        pos[3][1] = live_pos[1] + 150;
        pos[4][0] = live_pos[0] + 100;
        pos[4][1] = live_pos[1] + 150;
        //change time every 10 second
        if (player->i_ai->common_flag1_ == 0 || cur_time > (player->i_ai->common_flag1_ + 10) ) {
            player->i_ai->common_flag1_ = cur_time;
        }

        if (player->hp < (int)player->i_ai->common_flag3_) {
            if ( (rand() % 100) < 25 ) {
                int rand_num = rand();
                int x_offset = (rand_num % 160 + 290);
                if (player->role_type == 13096) 
                    x_offset = -(rand_num % 160 + 290);
                int y_offset = (rand_num % 100) - 100;
                call_monster_to_map(player->cur_map, player->btl, 11337 + (rand_num % 4), 
                    live_pos[0] + x_offset, live_pos[1] + y_offset, neutral_team_1);
            }
        }
        player->i_ai->common_flag3_ = player->hp;
        if (cur_time < (player->i_ai->common_flag1_+3) && cur_time >= player->i_ai->common_flag1_) {
            if (player->i_ai->common_flag2_ == 0) {
                player->cur_map->set_special_monster_dead(monster_3);
                int rand_num = rand() % 5;
                teleport(player, pos[rand_num][0], pos[rand_num][1]); 
                //ERROR_LOG("pos BOSS [%u %u]", pos[rand_num][0], pos[rand_num][1]);
                player->invincible_time = true;
                player->cur_map->mon_lv = player->lv;
                int i = 0;
                for ( ; i < 5; i++) {
                    if (i != rand_num) {
                        call_monster_to_map(player->cur_map,player->btl,monster_3,pos[i][0],pos[i][1],barrier_team);
                        //ERROR_LOG("pos [%u %u]", pos[i][0], pos[i][1]);
                    }
                }
                uint32_t skill_id = 4120106;
                player_skill_t * skill = player->select_skill(0, skill_id);
                if (skill) {
                    monster_attack(player, skill);
                }
                player->i_ai->common_flag2_ = 1;
            }
        } else {
            if (player->i_ai->common_flag2_ == 1) {
                player->invincible_time = false;
                player->i_ai->common_flag2_ = 0;

            }
        }
    }
    return true;
}

void TaoTaiBoss_AI::move(Player * player, int millisec)
{
    return ;
}


////////////////////////////////////////////////////
/*
bool  ShadowRat_AI::extra_wait_action(Player* player)
{

	player_skill_t* skill = player->select_skill(NULL, 4020115);
	if (skill) {
		monster_attack(player, skill);
		return false;
	}

	skill = player->select_skill(NULL, 4020116);
	if (skill) {
		monster_attack(player, skill);
		return false;
	}

	Player* p_target = get_target(player);
	if(p_target)
	{
		skill = player->select_skill(NULL, 4020117);
		if(skill)
		{
			monster_attack(player, skill, p_target->pos().x(), p_target->pos().y());
			return false;
		}
		skill = player->select_skill(NULL, 4020118);
		if(skill)
		{
			monster_attack(player, skill, p_target->pos().x(), p_target->pos().y());
			return false;
		}
	}
	return false;
}*/
/*
void ShadowRat_AI::attack(Player* player, int millisec)
{
	return SummonAI::attack(player, millisec);
}
*/


bool PetMonster_AI::extra_wait_action(Player* player)
{
	for ( SkillMap::iterator it = player->skills_map.begin(); it != player->skills_map.end(); ++it ) 
	{
		player_skill_t* tmp_skill = &(it->second);
		skill_t* p_skill = g_skills->get_skill(tmp_skill->skill_id);
		if(p_skill->ai_control_flag)continue;
		if (p_skill->type == passive_skill) {
			continue;
		}
		if( ! player->can_use_skill(tmp_skill->skill_id, false))continue;
		monster_attack(player, tmp_skill);
		return false;	
	}
	return false;
}

bool FMDaZhu_AI::extra_wait_action(Player * player)
{
	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
		//	ERROR_LOG("YI ER USE SKILL %u", skill->skill_id);
		} else {
			player->i_ai->set_run(true);
			player->i_ai->change_state(MOVE_STATE);
		}

	} else {
		player->i_ai->set_state_keep_tm(2);
	}	

	return true;
}

bool FMPP_AI::extra_wait_action(Player * player) 
{
	if (!player->i_ai->common_flag1_) {
		uint32_t now = get_now_tv()->tv_sec;
		uint32_t monster_size = get_monster_cnt(player, 13123);
		if (player->i_ai->common_flag_ == 0) {
			player->i_ai->common_flag_ = now;
		} else if (player->i_ai->common_flag_ + 50 < now || monster_size == 0 ) { 
			player->i_ai->common_flag1_ = 1;
			teleport(player, 700, 500);
		} 
		return true;
	}

	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
		} else {
			player->i_ai->set_run(true);
			player->i_ai->change_state(MOVE_STATE);
		}

	} else {
		player->i_ai->set_state_keep_tm(2);
	}	
	return true;
}

bool FMAoTian_AI::extra_wait_action(Player * player) 
{
	if (!player->i_ai->common_flag1_) {
		uint32_t now = get_now_tv()->tv_sec;
		uint32_t monster_size = get_monster_cnt(player, 13122);
		if (player->i_ai->common_flag_ == 0) {
			player->i_ai->common_flag_ = now;
		} else if (player->i_ai->common_flag_ + 100 < now || monster_size == 0) { 
			player->i_ai->common_flag1_ = 1;
			teleport(player, 700, 400);
		} 
		return true;
	}

	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
		} else {
			player->i_ai->set_run(true);
			player->i_ai->change_state(MOVE_STATE);
		}

	} else {
		player->i_ai->set_state_keep_tm(2);
	}	
	return true;
}

bool FMYiEr_AI::extra_wait_action(Player * player) 
{
	if (!player->i_ai->common_flag1_) {
		uint32_t now = get_now_tv()->tv_sec;
		uint32_t monster_size = get_monster_cnt(player, 13121);
		if (player->i_ai->common_flag_ == 0) {
			player->i_ai->common_flag_ = now;
		} else if (player->i_ai->common_flag_ + 150 < now || monster_size == 0) { 
			player->i_ai->common_flag1_ = 1;
			teleport(player, 800, 400);
		} 
		return true;
	}

	player->i_ai->target = 0;
	if ((player->i_ai->target = get_target(player)) != 0) {
		const Player * target = player->i_ai->target;
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
		} else {
			player->i_ai->set_run(true);
			player->i_ai->change_state(MOVE_STATE);
		}

	} else {
		player->i_ai->set_state_keep_tm(2);
	}	
	return true;
}


void FMYiEr_AI::move(Player *player, int timediff)
{

	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, 0)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	const Player * target = player->i_ai->target;

	//move to target pos
	if (monster_move(player, timediff)) {
		player_skill_t * skill = player->select_skill(target);
		if (skill) {
			monster_attack(player, skill);
		//	ERROR_LOG("YIER USE SKILL %u", skill->skill_id);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:attack when monster close to player
		if (player->i_ai->judge_update_tm(1000)) {
				player->i_ai->change_state(WAIT_STATE);
		} 
	}
}

bool JinJi_AI::extra_wait_action(Player * player)
{
	if (this->use_buf_skill(player)) {
		return true;
	}
	
	return false;
}


bool JinJi_AI::extra_stuck_action(Player * player)
{
	if (player->get_be_hit_count() >= 6 ) { //挨打超过5次进入防御状态
		player->i_ai->change_state(DEFENSE_STATE);
	} else { 
		player->i_ai->change_state(WAIT_STATE);
	}
	return true;
}

bool JinJi_AI::extra_defense_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) { //霸体
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
		//	TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}

	if (player->get_be_hit_count() >= 6) {//挨打次数超过一定次数则托马斯旋转
		int skill_id = 4120078;
		player_skill_t* skill = player->select_skill(0, skill_id);
		if (skill) {
			monster_attack(player, skill);
		//	TRACE_LOG("TUO MARS>>>>>>>&&&&&");
			player->i_ai->set_state_keep_tm(2);
		}
		player->reset_be_hit_count(); //重置被打次数
		player->invincible_time = 0;
	}

	player->i_ai->change_state(WAIT_STATE);
	return true;
}

bool WoodsCold_AI::extra_wait_action(Player * player)
{
	uint32_t now = get_now_tv()->tv_sec;
	if (player->i_ai->common_flag_ && player->i_ai->common_flag_ + 15 < now) {
		int skill_id = 4120143;

		player->i_ai->target = 0;
		if ((player->i_ai->target = get_target(player)) != 0 ) {
			player_skill_t* skill = player->select_skill(0, skill_id);
			if (skill) {
				const Player * target = player->i_ai->target;
				uint32_t x = rand() % 200 - 100 + target->pos().x();
				uint32_t y = rand() % 200 - 100 + target->pos().y();
				player->call_map_summon(32, x, y);
				player->i_ai->common_flag_ = now;
				return true;
			}
		}
	} else if (!player->i_ai->common_flag_) {
		player->i_ai->common_flag_  = now;
	}
	return false;
}

//魔牙之怒
bool GhostTeeth_AI::extra_wait_action(Player * player)
{
	player_skill_t * skill = player->select_skill(0, 4120147);
	if (!skill) {
		return false;
	}

	uint32_t now = get_now_tv()->tv_sec;
	//can use change show skill
	if (player->i_ai->common_flag_ && player->i_ai->common_flag_ + 15 < now) {
		player->i_ai->common_flag_ = now;
		return false;
	} else if (player->i_ai->common_flag_ && player->i_ai->common_flag_ + 6 > now) {
		//in 5 s sec change state
		if (!player->show_state) {
			player->change_show(11444);
		}
		uint32_t teeth_atk = 4120148;

		//add self speed
		player_skill_t * skill_1 = player->select_skill(0, 4120149); 
		if (skill_1) {
			monster_attack(player, skill_1);
		}

		//remove player speed
		player_skill_t * skill_2 = player->select_skill(0, 4120150);
		if (skill_2) {
			monster_attack(player, skill_2);
		}

		if (player->i_ai->target == 0) {
			if ( (player->i_ai->target = get_target(player)) == 0 ) {
				player->i_ai->change_state(WAIT_STATE);
				return true;
			}	
		}

		const Player * target = player->i_ai->target;

		player_skill_t * skill_3 = player->select_skill(target, teeth_atk);
		if (skill_3) {
			monster_attack(player, skill_3);
		} else {
			player->i_ai->change_state(MOVE_STATE);
		}
		return true;

	} else if (player->i_ai->common_flag_ && player->i_ai->common_flag_ + 6 < now) {
		if (player->show_state) {
			player->change_show();
		}
	} else if (!player->i_ai->common_flag_) {
		player->i_ai->common_flag_ = now - rand() % 7;
	}

	return false;
}

bool GhostTeeth_AI::extra_stuck_action(Player * player)
{
	if (player->get_be_hit_count() >= 6 ) { //挨打超过5次进入防御状态
		player->i_ai->change_state(DEFENSE_STATE);
	} else { 
		player->i_ai->change_state(WAIT_STATE);
	}
	return true;
}

bool GhostTeeth_AI::extra_defense_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) { //霸体
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
		//	TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}

	return false;
}


void GhostTeeth_AI::move(Player * player, int millisec)
{

	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, 4020004)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}

	const Player * target = player->i_ai->target;

	//move to target pos
	uint32_t now = get_now_tv()->tv_sec;
	if (monster_move(player, millisec)) {
		
		player_skill_t * skill = 0;
	    if (player->i_ai->common_flag_ && player->i_ai->common_flag_ + 6 >= now) {
			skill = player->select_skill(target, 4120148); 
		} else {
			skill = player->select_skill(target);
		}

		if (skill) {
			monster_attack(player, skill);
			ERROR_LOG("DMHUNATER SKILL %u", skill->skill_id);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:attack when monster close to player
		if (player->i_ai->judge_update_tm(1000)) {
				player->i_ai->change_state(WAIT_STATE);
		} 
	}
}


bool YaoShi_AI::extra_wait_action(Player * player)
{
	uint32_t now = get_now_tv()->tv_sec;
	if (!player->i_ai->common_flag1_) {
		player->i_ai->common_flag1_ = now;
		return true;
	}

	if (player->i_ai->common_flag1_ + 10 < now) {
		if (player->show_state) {
			player->change_show();
		} else {
			uint32_t yaoshi_stats = 13133; 
			player->change_show(yaoshi_stats);
			add_player_buff(player, 1311, 0,  10);
		}
		player->i_ai->common_flag1_ = now;
	}

	if (player->show_state) {
		return true;
	}
	return false;
}

bool EggBoss_AI::extra_wait_action(Player * player)
{
//	if ((player->i_ai->target = get_target(player)) != 0) {
//		const Player * target = player->i_ai->target;
//		int skill_id = 4120183;
//		player_skill_t * skill = player->select_skill(target, skill_id); 
//		if (skill && player->check_distance(target, 500)) {
//			monster_attack(player, skill, target->pos().x(), target->pos().y());
//			return true;
//		}
//	}
	return false;
}


bool IceDragon_AI::extra_wait_action(Player * player)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (is_player_buff_exist( player, 1316)) {
		if (!player->i_ai->common_flag3_) { //冰盾buff存在没有加过最大生命效果
			player->i_ai->common_flag1_ = player->max_hp();
			player->i_ai->common_flag2_ = player->hp;
			player->hp += 50000;
			player->maxhp += 50000;
			player->noti_hpmp_to_btl();
			player->i_ai->common_flag3_ = cur_time;

			ERROR_LOG("MONSTER　NOW HP MP %u %u", player->hp, player->max_hp());
		}
	} else {
		if (player->i_ai->common_flag3_) { //buff消失生命加成取消
			if (player->hp >= static_cast<int>(player->i_ai->common_flag2_)) { //没有打掉50000的血量
				player->hp = player->i_ai->common_flag2_;
			} 
			player->maxhp = player->i_ai->common_flag1_;
			player->noti_hpmp_to_btl();
			player->i_ai->common_flag3_ = 0;
			ERROR_LOG("MONSTER　NOW HP MP %u %u", player->hp, player->max_hp());

		}
	}
	return false;
}

bool ShadowDragon_AI::extra_wait_action(Player * player)
{
	uint32_t copy_self_skill = 4120199;
	uint32_t copy_monster = 11508;
	player_skill_t * skill = player->select_skill(0, copy_self_skill);
	if (skill && !player->cur_map->get_one_monster(copy_monster)) {
		monster_attack(player, skill);
		call_monster_to_map(player->cur_map, player->btl, copy_monster, 
			   player->pos().x() + 50, player->pos().y() + 50, player->team);
		call_monster_to_map(player->cur_map, player->btl, copy_monster, 
			   player->pos().x() - 50, player->pos().y() - 50, player->team);
		return true;
	}
	return false;
}

bool FightDragon_AI::extra_wait_action(Player * player)
{
//  uint32_t skill_id = 41201223;
//  player->i_ai->target = NULL;
//  if ( player->i_ai->get_target() != NULL) {
//    Player * target = player->i_ai->target;
//    player_skill_t * skill = player->select_skill(NULL, skill_id);
//    if (skill&& target) {
//      monster_attack(player, skill, target->pos().x(), 
//		     target->pos().y());
//	  return true;
//    }
//  }
  return false;
}

bool DarkDragon_AI::extra_wait_action(Player * player)
{
	uint32_t cur_time = get_now_tv()->tv_sec; 
	if (player->i_ai->common_flag1_ == 0) {
		player->i_ai->common_flag1_ = cur_time;
		return false;
	} else if (player->i_ai->common_flag1_ + 10 < cur_time) {
		static int pos_x[4] = {325, 815, 1309, 1529};
		static int pos_y[4] = {408, 648, 578, 340};
		int idx = rand() % 4;
//	player->invincible_time = 1;
		teleport(player, pos_x[idx], pos_y[idx]);
//		ERROR_LOG("TELEORT TO %u %u", pos_x[idx], pos_y[idx]);
		player->i_ai->common_flag1_ = cur_time;
		player->i_ai->common_flag2_ = 1;
		return true;
	}

	if (player->i_ai->common_flag2_) {
		Player * p = player->cur_map->get_one_player();
		if (p && !p->is_dead() && is_valid_uid(p->id) && p->team != player->team) {
			add_player_buff(player, 1322, 0);
			add_player_buff(p, 1322, 0);
		}
		player->i_ai->common_flag2_ = 0;
	}
	return false;
} 

bool FuryDragon_AI::extra_wait_action(Player * player)
{
  return false;
}


bool XueYao_AI::extra_wait_action(Player * player)
{
//	uint32_t now_time = get_now_tv()->tv_sec;

	if (player->btl->common_flag4_) {
		return true;
	}
	/*
	//find target
	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {	
		//if can attack
		const Player* target = player->i_ai->target;
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
			return true;
		}
		
		// move to an attackable position
		if (player->mon_info->spd) {
			player->i_ai->change_state(MOVE_STATE);
			return true;
		}
	} else {
		//player->i_ai->change_state(LINGER_STATE);	
		if (calc_return_pos(player)) {
			player->i_ai->change_state(LINGER_STATE);
		}
	}
	*/
	return false;
}


bool
Fenshenkuilei_AI::extra_stuck_action(Player * player)
{
	//SuperArmor
	if (player->super_armor) {
		player->super_armor = false;
		player_skill_t* super_armor_skill = player->select_super_armor_skill();
		if (super_armor_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, super_armor_skill);
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}
	
	return false;
}

bool Fenshenkuilei_AI::extra_wait_action(Player * player)
{
	uint32_t now_time = get_now_tv()->tv_sec;
	
	// init 
	if (player->i_ai->common_flag2_ == 0) {
		player->i_ai->common_flag1_ = 0; //０休息，１分身
		player->i_ai->common_flag2_ = now_time; //休息开始时间
		player->i_ai->common_flag3_ = now_time; //分身开始时间

		call_monster_to_map(player->cur_map, player->btl, 13141, 10000, 10000);
		call_monster_to_map(player->cur_map, player->btl, 13141, 10000, 10000);
		call_monster_to_map(player->cur_map, player->btl, 13141, 10000, 10000);

		player->invincible = true;
		player->invincible_time = 10000;
	}

	if (player->i_ai->common_flag1_ == 0 && player->i_ai->common_flag2_ + 3 < now_time) {
	//休息结束
		uint32_t x = 300, y = 200;
		teleport(player, x, y);
		
		for (PlayerSet::iterator it = player->cur_map->monsters.begin(); it != player->cur_map->monsters.end(); ++it) {
			Player* boss = *it;
			if (boss->role_type == 13141) {
				teleport(boss, x, y);
				boss->i_ai->change_state(WAIT_STATE);
				boss->i_ai->reset();
				boss->invincible = true;
				boss->invincible_time = 10000;
			}
		}

		player->i_ai->common_flag3_ = now_time;
		player->i_ai->common_flag1_ = 1;
		player->invincible = false;
		player->invincible_time = 0;
	}

	if (player->i_ai->common_flag1_ == 1 && player->i_ai->common_flag3_ + 15 < now_time) {
	//分身结束
		//player->cur_map->set_monster_dead(13141);
		teleport(player, 270, 180);
		player->i_ai->common_flag1_ = 0;
		player->i_ai->common_flag2_ = now_time;
		player->invincible = true;
		player->invincible_time = 10000;

		for (PlayerSet::iterator it = player->cur_map->monsters.begin(); it != player->cur_map->monsters.end(); ++it) {
			Player* boss = *it;
			if (boss->role_type == 13141) {
				teleport(boss, 10000, 10000);
				boss->i_ai->change_state(WAIT_STATE);
				boss->i_ai->reset();
			}
		}

		player_skill_t * skill = player->select_skill(0, 4120205);
		if (skill) {
			monster_attack(player, skill);
		}
	}
	
	if (player->i_ai->common_flag1_ == 0) {
		return true;
	}	

	//find target
	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {	
		//if can attack
		const Player* target = player->i_ai->target;
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
			return true;
		}
		
		// move to an attackable position
		if (player->mon_info->spd) {
			player->i_ai->change_state(MOVE_STATE);
			return true;
		}
	} else {
		//player->i_ai->change_state(LINGER_STATE);	
		if (calc_return_pos(player)) {
			player->i_ai->change_state(LINGER_STATE);
		}
	}
	return true;
}

bool Fenshenkuilei_AI::extra_attack_action(Player* player)
{
	return false;
}


/****************************************************************
 * NineHeadDragon Boss
 ****************************************************************/
bool
NineHeadDragonAI::extra_wait_action(Player * player)
{
	Player * p = player->btl->get_some_player();
	if (p && p->score.cur_hit_num > 15 && player->i_ai->hatred() > 5) {
		player->i_ai->set_hatred(0);
		player_skill_t* p_tmp_skill = player->select_specifed_skill(4120216);
		if (p_tmp_skill) {
			TRACE_LOG("super_armor[%u]", player->id);
			monster_attack(player, p_tmp_skill);
			
			player->i_ai->change_state(WAIT_STATE);
			return true;
		}
	}

	// init 
	if (player->i_ai->common_flag1_ == 0) {
		player->i_ai->common_flag1_ = get_now_tv()->tv_sec;
	}
	if (get_now_tv()->tv_sec - player->i_ai->common_flag1_ > ranged_random(2, 4)) {
		uint32_t skill_arr[4] = {4120212, 4120213, 4120214, 4120215};
		uint32_t tmp_id = skill_arr[ranged_random(0, 4)];
		player_skill_t* p_tmp_skill = player->select_specifed_skill(tmp_id);
		if (p_tmp_skill) {
			TRACE_LOG("ATTACK [%u %u]", player->id, tmp_id);
			monster_attack(player, p_tmp_skill);
		}
		player->i_ai->common_flag1_ = get_now_tv()->tv_sec;
	}
	return true;

}

bool
NineHeadDragonAI::extra_attack_action(Player * player)
{
	return true;
}

bool
NineHeadDragonAI::extra_stuck_action(Player * player)
{
	return false;
}


bool
DaguiAI::extra_wait_action(Player * player)
{
	
	// init 
	if (player->hp < player->maxhp * 15 / 100) {
		if (player->i_ai->common_flag1_ == 0) {
			player->i_ai->common_flag1_ = get_now_tv()->tv_sec;
		} else {
			if (get_now_tv()->tv_sec - player->i_ai->common_flag1_ > 10) {
				if ((player->i_ai->target = get_target(player)) != 0) {
					const Player * target = player->i_ai->target;
					int skill_id = 4029327;
					player_skill_t * skill = player->select_skill(target, skill_id); 
					if (skill && player->check_distance(target, 100)) {
						monster_attack(player, skill, target->pos().x(), target->pos().y());
						PlayerVec::iterator it = player->btl->players_.begin();
						while (it != player->btl->players_.end()) {
							Player* p = *it;
							p->suicide();
							++it;
						}
						player->suicide();
						return true;
					} else {
						// move to an attackable position
						if (player->mon_info->spd) {
							player->i_ai->change_state(MOVE_STATE);
							return true;
						}
					}
				}
				return true;
			}
		}
	}

	if (player->i_ai->common_flag2_ < 4) {
		int skill_id = 4029326;
		player_skill_t * skill = player->select_skill(0, skill_id); 
		if (skill) {
			player->i_ai->common_flag2_ += 2;
			monster_attack(player, skill, player->pos().x(), player->pos().y());
			return true;
		}
		
	}
	int skill_id = 4029328;
	player_skill_t * skill = player->select_skill(0, skill_id); 
	if (skill) {
		TRACE_LOG("use %u", skill_id);
		monster_attack(player, skill, player->pos().x(), player->pos().y());
		return true;
	}
	skill_id = 4029329;
	skill = player->select_skill(0, skill_id); 
	if (skill) {
		TRACE_LOG("use %u", skill_id);
		monster_attack(player, skill, player->pos().x(), player->pos().y());
		return true;
	}
	
	return false;
}

bool
DaguiAI::extra_attack_action(Player * player)
{
	return false;
}

bool
DaguiAI::extra_stuck_action(Player * player)
{
	return false;
}

