/**
 *============================================================
 *  @file      SpecialMonAI.cpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */
 
extern "C" {
#include <libtaomee/log.h>
}

#include <libtaomee++/random/random.hpp>
#include <libtaomee++/inet/byteswap.hpp>

#include "MonsterAI.hpp"
#include "HighLvAI.hpp"
#include "MiddleLvAI.hpp"
#include "SpecialMonAI.hpp"
#include "NullAI.hpp"
#include "../player.hpp"
#include "../stage.hpp"
#include "../battle_impl.hpp"

using namespace taomee;

/****************************************************************
 * melon mole monster
 ****************************************************************/
void
MoleAI::wait(Player* player, int millisec)
{
	TRACE_LOG("wait state: uid=%u", player->id);

	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
         return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
	
	//find target
	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {
		//if can attack
		const Player* target = player->i_ai->target;
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
			return;
		}
		
		// move to an attackable position
		if (player->mon_info->spd) {
			player->i_ai->change_state(MOVE_STATE);
			return;
		}
	} else {
		if (calc_return_pos(player)) {
			player->i_ai->change_state(LINGER_STATE);
		}
		return;		
	}
}

void 
MoleAI::linger(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag(false);
	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_linger_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		//if can attack
		if (target && player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			player->i_ai->set_state_keep_tm(rand() % 3);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else if (player->i_ai->judge_update_tm(1000)) {
		if (target && player->select_skill(target)) {
				monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
		}
	}
}

void
MoleAI::move(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag();
	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		//if can attack
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			//player->i_ai->set_state_keep_tm(rand() % 2);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:
		if (player->i_ai->judge_update_tm(1000)) {
			//if can attack
			if (player->select_skill(target)) {
				monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
				return;
			}
		}
	}
}

void
MoleAI::evade(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
MoleAI::attack(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag();
	
	//TODO:normal attack & skill attack
	if (player->i_ai->target == 0) {
		player->i_ai->change_state(WAIT_STATE);
		return;
	}
	
	player_skill_t* skill = player->select_skill(player->i_ai->target);
	if (skill) {
		monster_attack(player, skill);
		//player->i_ai->set_hatred(0);
	} 
	
  //player->i_ai->set_state_keep_tm(rand() % 2);	
	player->i_ai->change_state(WAIT_STATE);
}

void
MoleAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->set_state_keep_tm(0);
		
		player->i_ai->target_pos.init();

		//TODO:
		attack(player, millisec);
	}
}

void 
MoleAI::defense(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

/****************************************************************
 * red eye monkey monster
 ****************************************************************/
void
RedEyeMonkeyAI::wait(Player* player, int millisec)
{
	TRACE_LOG("wait state: uid=%u", player->id);

	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
         return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
	
	//find target
	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {
		//if can attack
		const Player* target = player->i_ai->target;
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
			return;
		}
		
		// move to an attackable position
		if (player->mon_info->spd) {
			player->i_ai->change_state(EVADE_STATE);
			return;
		}
	} else {
		if (calc_return_pos(player)) {
			player->i_ai->change_state(LINGER_STATE);
		}
		return;
	}
}

void 
RedEyeMonkeyAI::linger(Player* player, int millisec)
{	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_linger_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		//if can attack
		if (target && player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			//player->i_ai->set_state_keep_tm(rand() % 3);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else if (player->i_ai->judge_update_tm(1000)) {
		if (target && player->select_skill(target)) {
				monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
		}
	}
}

void
RedEyeMonkeyAI::move(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
RedEyeMonkeyAI::evade(Player* player, int millisec)
{
	//calc evade pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_evade_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	if (monster_move(player, millisec)) {
		//if can attack
		const Player* target = player->i_ai->target;
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			//player->i_ai->set_state_keep_tm(rand() % 2);
			player->i_ai->change_state(WAIT_STATE);
		}
	}
}

void
RedEyeMonkeyAI::attack(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag();
	
	//TODO:normal attack & skill attack
	if (player->i_ai->target == 0) {
		player->i_ai->change_state(WAIT_STATE);
		return;
	}
	
	player_skill_t* skill = player->select_skill(player->i_ai->target);
	if (skill) {
		monster_attack(player, skill);
		player->i_ai->set_hatred(0);
	} 
	
  //player->i_ai->set_state_keep_tm(rand() % 2);
	player->i_ai->change_state(EVADE_STATE);
}

void
RedEyeMonkeyAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->set_state_keep_tm(0);
		
		player->i_ai->target_pos.init();

		//TODO:
		if (player->i_ai->hatred() > 3) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			player->i_ai->change_state(EVADE_STATE);
		}
	}
}

void 
RedEyeMonkeyAI::defense(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

/****************************************************************
 * Leopard monster
 ****************************************************************/
void
LeopardAI::wait(Player* player, int millisec)
{
	TRACE_LOG("wait state: uid=%u", player->id);

	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
         return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
	
	//find target
	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {
		//if can attack
		const Player* target = player->i_ai->target;
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
			return;
		}
		
		// move to an attackable position
		if (player->mon_info->spd) {
			player->i_ai->change_state(MOVE_STATE);
			return;
		}
	} else {
		if (calc_return_pos(player)) {
			player->i_ai->change_state(LINGER_STATE);
		}
		return;
	}
}

void 
LeopardAI::linger(Player* player, int millisec)
{	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_linger_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		//if can attack
		if (target && player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			//player->i_ai->set_state_keep_tm(rand() % 3);
			player->i_ai->change_state(WAIT_STATE);
		}
	}
}

void
LeopardAI::move(Player* player, int millisec)
{
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		//if can attack
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			//player->i_ai->set_state_keep_tm(rand() % 2);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:
		if (player->i_ai->judge_update_tm(1000)) {
			//if can attack
			if (player->select_skill(target)) {
				monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
				return;
			}
		}
	}
}

void
LeopardAI::evade(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
LeopardAI::attack(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag();
	
	//TODO:normal attack & skill attack
	if (player->i_ai->target == 0) {
		player->i_ai->change_state(WAIT_STATE);
		return;
	}
	
	player_skill_t* skill = 0;
	if ( (skill = select_crit_skill(player, player->i_ai->target)) == 0 )  {
		skill = player->select_skill(player->i_ai->target);
	}
	
	//player_skill_t* skill = player->select_skill(player->i_ai->target);
	if (skill) {
		monster_attack(player, skill);
	} 
	
  player->i_ai->set_state_keep_tm(rand() % 2);
	player->i_ai->change_state(WAIT_STATE);
}

void
LeopardAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->set_state_keep_tm(0);
		
		player->i_ai->target_pos.init();

		//TODO:
		attack(player, millisec);
	}
}

void 
LeopardAI::defense(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

player_skill_t*
LeopardAI::select_crit_skill(Player* player, const Player* target)
{
	player_skill_t* skill = 0;
	if (player->hp < player->maxhp / 2) {
		int rand_num = rand() % 100;
		int rand_odds = 0;
		int lv_diff = target->lv - player->lv;
		if (lv_diff <= 1) {
			rand_odds = 80;
		} else if (lv_diff == 2) {
			rand_odds = 70;
		} else if (lv_diff == 3) {
			rand_odds = 60;
		} else if (lv_diff == 4) {
			rand_odds = 50;
		} else {
			rand_odds = 50;
		}
		if (rand_num < rand_odds) {
			skill = player->select_skill(player->i_ai->target, 4020005);
		}
	}
	
	return skill;
}

/****************************************************************
 * JungleWolf monster
 ****************************************************************/
void
JungleWolfAI::wait(Player* player, int millisec)
{
	TRACE_LOG("wait state: uid=%u", player->id);

	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
         return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
	
	//find target
	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {
		const Player* target = player->i_ai->target;
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
			return;
		}
		
		if (player->mon_info->spd) {
			player->i_ai->change_state(MOVE_STATE);
			return;
		}
	} else {
		if (calc_return_pos(player)) {
			player->i_ai->change_state(LINGER_STATE);
		}
		return;
	}
}

void 
JungleWolfAI::linger(Player* player, int millisec)
{	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_linger_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	if (monster_move(player, millisec)) {
		player->i_ai->change_state(WAIT_STATE);
	}
}

void
JungleWolfAI::move(Player* player, int millisec)
{
	//calc target pos
	const Player* target = player->i_ai->target;
	if ( target && (player->dir == dir_left) && (target->pos().x() > player->pos().x()) ) {
		player->dir = dir_right;
	} else if ( target && (player->dir == dir_right) && (target->pos().x() < player->pos().x()) ) {
		player->dir = dir_left;
	}
	monster_stand(player);
	player->i_ai->set_state_keep_tm(rand() % 2);
	
	player->i_ai->change_state(WAIT_STATE);
}

void
JungleWolfAI::evade(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
JungleWolfAI::attack(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag();
	
	//TODO:normal attack & skill attack
	if (player->i_ai->target == 0) {
		player->i_ai->change_state(WAIT_STATE);
		return;
	}
	
	const Player* target = player->i_ai->target;
	player_skill_t* skill = player->select_skill(player->i_ai->target);
	if (skill) {
		if ( target && (player->dir == dir_left) && (target->pos().x() > player->pos().x()) ) {
			player->dir = dir_right;
		} else if ( target && (player->dir == dir_right) && (target->pos().x() < player->pos().x()) ) {
			player->dir = dir_left;
		}
		monster_stand(player);
		monster_attack(player, skill);
	} 
	
  //player->i_ai->set_state_keep_tm(rand() % 2);
	player->i_ai->change_state(WAIT_STATE);
}

void
JungleWolfAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->set_state_keep_tm(0);
		
		player->i_ai->target_pos.init();

		//TODO:
		attack(player, millisec);
	}
}

void 
JungleWolfAI::defense(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

/****************************************************************
 * gorilla monster
 ****************************************************************/
void
GorillaAI::wait(Player* player, int millisec)
{
	TRACE_LOG("wait state: uid=%u", player->id);

	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
         return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
	
	//find target
	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {
		//use full_hp buf
		if (!player->i_ai->common_flag_ && is_serious_injury(player)) {
			int rand_num = rand() % 100;
			if (rand_num > 50) {
				player->i_ai->common_flag_ = 1;
				return;
			}
			
			if (use_buf_skill(player)) {
				return;
			}
		}
		
		//if can attack
		const Player* target = player->i_ai->target;
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
			return;
		}
		
		// move to an attackable position
		if (player->mon_info->spd) {
			player->i_ai->change_state(MOVE_STATE);
			return;
		}
	} else {
		if (calc_return_pos(player)) {
			player->i_ai->change_state(LINGER_STATE);
		}
		return;		
	}
}

void 
GorillaAI::linger(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag(false);
	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_linger_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		//if can attack
		if (target && player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			player->i_ai->set_state_keep_tm(rand() % 3);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else if (player->i_ai->judge_update_tm(1000)) {
		if (target && player->select_skill(target)) {
				monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
		}
	}
}

void
GorillaAI::move(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag();
	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		//if can attack
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			//player->i_ai->set_state_keep_tm(rand() % 2);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:
		if (player->i_ai->judge_update_tm(1000)) {
			//if can attack
			if (player->select_skill(target)) {
				monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
				return;
			}
		}
	}
}

void
GorillaAI::evade(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
GorillaAI::attack(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag();
	
	//TODO:normal attack & skill attack
	if (player->i_ai->target == 0) {
		player->i_ai->change_state(WAIT_STATE);
		return;
	}
	
	player_skill_t* skill = player->select_skill(player->i_ai->target);
	if (skill) {
		monster_attack(player, skill);
		//player->i_ai->set_hatred(0);
	} 
	
  //player->i_ai->set_state_keep_tm(rand() % 2);	
	player->i_ai->change_state(WAIT_STATE);
}

void
GorillaAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->set_state_keep_tm(0);
		
		player->i_ai->target_pos.init();

		//TODO:
		attack(player, millisec);
	}
}

void 
GorillaAI::defense(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

/****************************************************************
 * gorilla monster
 ****************************************************************/
void
NoShadowCatAI::wait(Player* player, int millisec)
{
	TRACE_LOG("wait state: uid=%u", player->id);

	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
         return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
	
	//find target
	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {
		//if can attack
		const Player* target = player->i_ai->target;
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
			return;
		}
		
		// move to an attackable position
		if (player->mon_info->spd) {
			player->i_ai->change_state(MOVE_STATE);
			return;
		}
	} else {
		if (calc_return_pos(player)) {
			player->i_ai->change_state(LINGER_STATE);
		}
		return;		
	}
}

void 
NoShadowCatAI::linger(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag(false);
	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_linger_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		//if can attack
		if (target && player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			player->i_ai->set_state_keep_tm(rand() % 3);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else if (player->i_ai->judge_update_tm(1000)) {
		if (target && player->select_skill(target)) {
				monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
		}
	}
}

void
NoShadowCatAI::move(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag();
	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		//if can attack
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			//player->i_ai->set_state_keep_tm(rand() % 2);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:
		if (player->i_ai->judge_update_tm(1000)) {
			//if can attack
			if (player->select_skill(target)) {
				monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
				return;
			}
		}
	}
}

void
NoShadowCatAI::evade(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
NoShadowCatAI::attack(Player* player, int millisec)
{
	//player->i_ai->set_trace_flag();
	
	//TODO:normal attack & skill attack
	if (player->i_ai->target == 0) {
		player->i_ai->change_state(WAIT_STATE);
		return;
	}
	
	player_skill_t* skill = player->select_skill(player->i_ai->target);
	if (skill) {
		monster_attack(player, skill);
		//player->i_ai->set_hatred(0);
	} 
	
  //player->i_ai->set_state_keep_tm(rand() % 2);	
	player->i_ai->change_state(WAIT_STATE);
}

void
NoShadowCatAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->set_state_keep_tm(0);
		
		player->i_ai->target_pos.init();

		//TODO:
		int rand_num = rand() % 100;
		if (rand_num < 30) {
			use_buf_skill(player);
		}
	}
}

void 
NoShadowCatAI::defense(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

/****************************************************************
 * Villager AI
 ****************************************************************/
void
VillagerAI::wait(Player* player, int millisec)
{
	TRACE_LOG("wait state: uid=%u", player->id);

	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
  	return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
	
	player->i_ai->change_state(LINGER_STATE);
	return;
}

void 
VillagerAI::linger(Player* player, int millisec)
{	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_linger_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	//const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		player->i_ai->set_state_keep_tm(rand() % 3 + 2);
			player->i_ai->change_state(WAIT_STATE);
	}
}

void
VillagerAI::move(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
VillagerAI::evade(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
VillagerAI::attack(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
VillagerAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->set_state_keep_tm(0);
		
		player->i_ai->target_pos.init();
	}
	
	player->i_ai->change_state(WAIT_STATE);
}

void 
VillagerAI::defense(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

/****************************************************************
 * Raven AI
 ****************************************************************/
void
RavenAI::wait(Player* player, int millisec)
{
	TRACE_LOG("wait state: uid=%u", player->id);

	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
  	return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
	
	if (!player->i_ai->common_flag_) {
		player->i_ai->set_event_tm(3000);
		player->i_ai->common_flag_ = 1;
	}
	
	if (player->i_ai->check_event_tm()) {
		player->invincible_time = 1;
		player->i_ai->common_flag1_ = 1;
		player->i_ai->change_state(DEFENSE_STATE);
		return;
	}
	
	//get target
	player->i_ai->target = 0;
	if ( (player->i_ai->target = get_target(player)) != 0 ) {
		const Player* target = player->i_ai->target;
		//if can attack
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
			return;
		}
		
		// move
		if (player->mon_info->spd) {
			player->i_ai->change_state(MOVE_STATE);
			return;
		}
	} else {
		player->i_ai->change_state(LINGER_STATE);
	}
	return;
}

void 
RavenAI::linger(Player* player, int millisec)
{	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_linger_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	//const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		player->i_ai->set_state_keep_tm(rand() % 3 + 1);
		player->i_ai->change_state(WAIT_STATE);
	}
}

void
RavenAI::move(Player* player, int millisec)
{
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_interval_move_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	//const Player* target = player->i_ai->target;
	if (monster_move(player, millisec)) {
		player->i_ai->set_state_keep_tm(rand() % 2 + 1);
		player->i_ai->change_state(WAIT_STATE);
	}
}

void
RavenAI::evade(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
RavenAI::attack(Player* player, int millisec)
{
	//TODO:normal attack & skill attack
	if (player->i_ai->target == 0) {
		if ( (player->i_ai->target = get_target(player)) == 0 ) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	player_skill_t* skill = player->select_skill(player->i_ai->target, player->i_ai->ready_skill_id_);
	if (skill) {
		monster_attack(player, skill);
		player->i_ai->ready_skill_id_ = 0;
	} 
		
	player->i_ai->change_state(WAIT_STATE);
}

void
RavenAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->set_state_keep_tm(0);
		
		player->i_ai->target_pos.init();

		player->i_ai->change_state(WAIT_STATE);
	}
}

void 
RavenAI::defense(Player* player, int millisec)
{
	/*if ( !player->i_ai->target && (player->i_ai->target = get_target(player)) == 0 ) {
		player->i_ai->change_state(WAIT_STATE);
		return;
	}
	const Player* target = player->i_ai->target;*/
	
	if (player->i_ai->common_flag1_) {
		player_skill_t* skill = player->select_skill(0, 4120036);
		if (skill) {
			monster_speak(player, 1);
			monster_attack(player, skill);
			player->i_ai->common_flag1_ = 0;
			player->invincible_time = 0;
			player->i_ai->set_event_tm(5100);
		} else {
			player->i_ai->common_flag1_ = 0;
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	if (player->i_ai->check_event_tm()) {
		player->i_ai->set_event_tm(12000 + rand() % 3000);
		player->i_ai->change_state(WAIT_STATE);
		return;
	}
}

/****************************************************************
 * Pig monster
 ****************************************************************/
bool
PigAI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag2_) {
		if (player->i_ai->check_event_tm()) {
			teleport(player, 10000, 10000);
		}
		return true;
	}
	
	if (!player->i_ai->common_flag_ && is_hp_range(player, 0 ,20)) {
		//to weakness state
		player->i_ai->common_flag_ = 1;
		player->i_ai->common_flag1_ = 1;
		player->invincible_time = 1;
		player->i_ai->change_state(ATTACK_STATE);
		return true;
	}

	return false;
}

bool
PigAI::extra_attack_action(Player * player)
{
	uint32_t skill_id = 0;
	
	//weakness state
	if (player->i_ai->common_flag1_) {
		skill_id = 4120043;
		player->invincible_time = 0;
		player_skill_t* skill = player->select_skill(0, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag1_ = 0;
			player->i_ai->common_flag2_ = 1;
			player->i_ai->set_event_tm(30000);
		}
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	return false;
}

bool
PigAI::extra_stuck_action(Player * player)
{
	if (player->i_ai->common_flag2_) {
		uint32_t skill_id = 4120043;
		player_skill_t* skill = player->select_skill(0, skill_id);
		if (skill) {
			monster_attack(player, skill);
		}
		player->i_ai->change_state(WAIT_STATE);
		return true;
	}
	
	return false;
}


/****************************************************************
 * gear monster
****************************************************************/
void GearAI::wait(Player* player, int millisec)
{
	if( player->is_hp_full() && player->btl){
		player->btl->add_buff_to_all_players(21, 0);//免疫暗流所随机生成的BUFF
		player->btl->battle_section_data = 1;
		player->btl->del_buff_from_all_players(26);//删除暗流的图标
		player->i_ai->change_ai(NullAI::instance(), NULL_AI);	
	}
}
void GearAI::linger(Player* player, int millisec)
{
	return ;
}
void GearAI::move(Player* player, int millisec)
{
	return;
}
void GearAI::evade(Player* player, int millisec)
{
	return;
}
void GearAI::attack(Player* player, int millisec)
{
	return;
}
void GearAI::stuck(Player* player, int millisec)
{
	return;
}
void GearAI::defense(Player* player, int millisec)
{
	return;
}


/****************************************************************
 * BombAI
****************************************************************/
bool BombAI::extra_wait_action(Player* player)
{
	Player* p =  get_target(player);
	if( p ){
		if( player->check_distance(p, 100) )
		{
			player->suicide();
			return true;
		}
	}
	return false;
}

bool BombAI::extra_attack_action(Player* player)
{
	Player* p =  get_target(player);
	if( p )
	{
		if( player->check_distance(p, 100) )
		{
			player->suicide();
			return true;
		}
	}
	return false;
}


/****************************************************************
 * WormEggAI
****************************************************************/
bool WormEggAI::extra_wait_action(Player * player)
{
	if( player->i_ai->common_flag_ == 0){
		player->i_ai->set_event_tm(4000);
		player->i_ai->common_flag_  = 1;
		return true;
	}
	else if( player->i_ai->common_flag_ == 1 && player->i_ai->check_event_tm())
	{
		player_skill_t* skill = player->select_skill(NULL, 4120090);
		if(skill){
			monster_attack(player, skill);
			player->i_ai->set_event_tm(4000);
			player->i_ai->common_flag_  = 2;
		}
		return true;
	}
	else if( player->i_ai->common_flag_ ==  2 && player->i_ai->check_event_tm())
	{
		player_skill_t* skill = player->select_skill(NULL, 4120091);
		if(skill){
			monster_attack(player, skill);
			player->i_ai->set_event_tm(4000);
			player->i_ai->common_flag_ = 3;
		}
		return true;
	}
	else if( player->i_ai->common_flag_ ==  3 && player->i_ai->check_event_tm())
	{
		player->i_ai->common_flag_ = 4;
		player->suicide();
		return true;
	}	
	return true;
}

bool WormEggAI::extra_stuck_action(Player* player)
{
	player->i_ai->change_state(WAIT_STATE);
	return true;
}


/***************************************************************
 * Walnut ai  
***************************************************************/

bool WalnutAI::extra_wait_action(Player* player)
{
	player_skill_t* skill = player->select_skill(NULL, 4030049);
	if(skill){
		monster_attack(player, skill);
	}
	return true;
}

bool WalnutAI::extra_stuck_action(Player* player)
{
	player->i_ai->change_state(WAIT_STATE);
	return true;
}


bool JiGuan_AI::extra_wait_action(Player * player)
{
	uint32_t cur_time = time(0);
	player->dir = dir_right;
	player->invincible = true;
	player->invincible_time = 5;

	if (player->i_ai->common_flag1_ ==0) { //初始化
		player->i_ai->common_flag1_ = cur_time; 
		player->i_ai->common_flag2_ = cur_time;
		return true;
	}

	if (cur_time - player->i_ai->common_flag1_ < 60) { //第一状态
		if (cur_time - player->i_ai->common_flag2_ > 10) { //锯齿机关
			uint32_t skill_id = 4030050;
			player_skill_t * skill = player->select_skill(0, skill_id);
			if (skill) {
				monster_attack(player, skill);
				player->i_ai->common_flag2_ = cur_time;
			//	ERROR_LOG("JIGUAN 1 %u", cur_time);
			}
		}
	} else if ((cur_time - player->i_ai->common_flag1_) > 60
		   	&& (cur_time - player->i_ai->common_flag1_) <= 120) { //第二状态

		if (cur_time - player->i_ai->common_flag2_ > 7 ) { //锯齿机关
			uint32_t skill_id = 4030050;
			player_skill_t * skill = player->select_skill(0, skill_id);
			if (skill) {
				monster_attack(player, skill);
				player->i_ai->common_flag2_ = cur_time;
			//	ERROR_LOG("JIGUAN 2 %u", cur_time);
			}
		}
	} else if ((cur_time - player->i_ai->common_flag1_) > 120 
		    && (cur_time - player->i_ai->common_flag1_) < 180) { //第三状态
		if (cur_time - player->i_ai->common_flag2_ > 4) { //锯齿机关
			uint32_t skill_id = 4030050;
			player_skill_t * skill = player->select_skill(0, skill_id);
			if (skill) {
				monster_attack(player, skill);
				player->i_ai->common_flag2_ = cur_time;
			//	ERROR_LOG("JIGUAN 3 %u", cur_time);
			}
		}
	} else if (cur_time - player->btl->btl_start_tm > 180) {
		if (!player->cur_map->get_one_barriers(19141)) {
			call_monster_to_map(player->cur_map, player->btl, 19141, 500, 400, barrier_team);
		}
	}	
	return true;
}

bool ColorDoll_AI::extra_wait_action(Player * player)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (player->i_ai->common_flag1_ == 0) { //初次亮相
		uint32_t skill_id = 4120096;
		player_skill_t * skill = player->select_skill(0, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag1_ = cur_time;
			return true;
		}
	} else if (player->i_ai->common_flag1_ + 2 < cur_time) {
		player->invincible = false;
	} else if (player->i_ai->common_flag1_ + 2 > cur_time) { //2s内无敌
		player->invincible = true;
		return true;
	}

	if (player->i_ai->common_flag3_) {//在镜子中
		if (player->i_ai->common_flag3_  > cur_time - 10) {
			player->invincible = true; //无敌
		} else { //出镜子
			uint32_t skill_id = 4120099;
			player_skill_t * skill = player->select_skill(0, skill_id);
			if (skill) {
				monster_attack(player, skill);
				player->i_ai->common_flag3_ = 0;
				player->invincible = false;
				Player * mechanism = 0;
				if (player->pos().x() < 600) {
					mechanism = player->cur_map->get_mechanism_by_type(35000); //镜子1
				} else {
					mechanism = player->cur_map->get_mechanism_by_type(34999);//镜子2
				}

				player->hp = player->maxhp;
				player->noti_hpmp_to_btl();
				if (player->pos().x() < 600) {
					mechanism = player->cur_map->get_mechanism_by_type(35000); //镜子1
					teleport(player, player->pos().x(), mechanism->pos().y());
				} else {
					mechanism = player->cur_map->get_mechanism_by_type(34999);//镜子2
					teleport(player, player->pos().x(), mechanism->pos().y());
				}
				if (mechanism) {
					player->cur_map->mechansim_status_changed(mechanism->id, 0, 0);
				}

			}
		}
		return true;
	}

	if (is_injury(player)) { //受伤找镜子
		add_player_buff(player, 36, 0, 10);
		if (player->pos().x() < 660) {
			player->i_ai->common_flag2_ = 1;//找1号镜子
		} else {
			player->i_ai->common_flag2_ = 2;//找2号镜子
		}
		player->i_ai->change_state(MOVE_STATE);
		return true;
	}

	return false;
}

void ColorDoll_AI::move(Player * player, int timediff)
{

	if (player->i_ai->common_flag2_) {
		if (!is_player_buff_exist(player, 36)) {
			add_player_buff(player, 36, 0, 10);
		}
		const KfAstar::Points* pts = 0; 
		if (player->i_ai->common_flag2_ == 1) { //找1号镜子
			Player * mechanism = player->cur_map->get_mechanism_by_type(35000);
			if (mechanism) {
				if (mechanism->check_distance(player, 50)) {
					if (mechanism->mp != 0) {
						player->i_ai->common_flag2_ = 2; //换2号镜子
					} else {
						uint32_t skill_id = 4120098; //入镜子
						player_skill_t * skill = player->select_skill(0, skill_id);
						if (skill) {
							monster_attack(player, skill);
							player->i_ai->common_flag2_ = 0;
							player->i_ai->common_flag3_ = get_now_tv()->tv_sec;
							player->i_ai->change_state(WAIT_STATE);
							teleport(player, player->pos().x(), 10000);
							player->cur_map->mechansim_status_changed(mechanism->id, 3);
						}
					}
					return;
				} 
				else 
				{
					//pts = player->cur_map->path->findpath(KfAstar::Point(player->pos().x(), player->pos().y()), KfAstar::Point(mechanism->pos().x(), mechanism->pos().y()));
					KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
					if(p_star)
					{
						pts = p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()),  KfAstar::Point(mechanism->pos().x(),  mechanism->pos().y()));
					}

				}
			}
		} else if (player->i_ai->common_flag2_ == 2) {//找2号镜子
			Player * mechanism = player->cur_map->get_mechanism_by_type(34999);
			if (mechanism) {
				if (mechanism->check_distance(player, 50)) {
					if (mechanism->mp != 0) {
						player->i_ai->common_flag2_ = 1; //换1号镜子
					} else {
						uint32_t skill_id = 4120098; //入镜子
						player_skill_t * skill = player->select_skill(0, skill_id);
						if (skill) {
							monster_attack(player, skill);
							player->i_ai->common_flag2_ = 0;
							player->i_ai->common_flag3_ = get_now_tv()->tv_sec;
							teleport(player, player->pos().x(), 10000);
							player->i_ai->change_state(WAIT_STATE);
							player->cur_map->mechansim_status_changed(mechanism->id, 3);
						}
					}
					return;
				} 
				else 
				{
					//pts = player->cur_map->path->findpath(KfAstar::Point(player->pos().x(), player->pos().y()), KfAstar::Point(mechanism->pos().x(), mechanism->pos().y()));
					KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
					if(p_star)
					{
						 pts = p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()),  KfAstar::Point(mechanism->pos().x(), mechanism->pos().y()));
					}
				}
			}
		}
		if (pts){ //靠近镜子
        	player->i_ai->target_path.assign(pts->begin(), pts->end());
       		get_one_target_pos(player);
      	 	monster_move_ex(player, timediff);
   		 }
		return;
	}  

	//其他正常行走
	const Player* target = player->i_ai->target;
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, player->i_ai->ready_skill_id_)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	if (monster_move(player, timediff)) {
		if (player->select_skill(target, player->i_ai->ready_skill_id_)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			player->i_ai->set_state_keep_tm(rand() % 1 + 1);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		if (player->i_ai->judge_update_tm(1000)) {
			if (player->select_skill(target)) {
				monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
			}
		}
	}
}

bool GoldDoll_AI::extra_wait_action(Player *player)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (player->i_ai->common_flag1_ == 0) {//亮相
		uint32_t skill_id = 4120097;
		player_skill_t * skill = player->select_skill(0, skill_id);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->common_flag1_ = cur_time;
		}
		return true;
	} else if (player->i_ai->common_flag1_ + 2 < cur_time) {
		player->invincible = false;
	} else if (player->i_ai->common_flag1_ + 2 > cur_time){//2s内无敌
		player->invincible = true;
		return true;
	}	
	return false;
}

bool GoldDoll_AI::extra_stuck_action(Player * player)
{
	if (player->get_be_hit_count() >= 6 ) { //挨打超过5次进入防御状态
		player->i_ai->change_state(DEFENSE_STATE);
	} else { 
		player->i_ai->change_state(WAIT_STATE);
	}
	return true;
}

bool GoldDoll_AI::extra_defense_action(Player *player)
{
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

void GoldDoll_AI::move(Player * player, int millisec)
{
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, 4020003)) {
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

/////////////////good guy rabbit//////////////////////////////


bool GoodGuyRabbit::extra_wait_action(Player * player)
{
	if( player->i_ai->common_flag_ == 0)
	{
		player_skill_t * skill = player->select_skill( NULL, 4120102);
		if(skill){
			monster_attack(player, skill);
			player->i_ai->common_flag_ = 1;
		}
	}
	return true;
}


bool GoodGuyRabbit::extra_stuck_action(Player * player)
{
	player->i_ai->change_state(WAIT_STATE);
	return true;
}
		        

bool GoodGuyRabbit::extra_defense_action(Player * player)
{
	player->i_ai->change_state(WAIT_STATE);
	return true;
}


//////////////////////bad guy rabbit///////////////////////////
bool BadGuyRabbit::extra_wait_action(Player * player)
{
	if( player->i_ai->common_flag_ == 0)
	{
		player_skill_t * skill = player->select_skill( NULL, 4120101);
		if(skill){
			monster_attack(player, skill);
			player->i_ai->common_flag_ = 1;
		}
	}
	return true;
}

bool BadGuyRabbit::extra_stuck_action(Player * player)
{
	player->i_ai->change_state(WAIT_STATE);
	return true;
}


bool BadGuyRabbit::extra_defense_action(Player * player)
{
	player->i_ai->change_state(WAIT_STATE);
	return true;
}

/**********************************************
 *       三段梅花流 特殊怪
 ***********************************************/
bool ThreeWood1_AI::extra_wait_action(Player * player)                                                               
{
    uint32_t cur_time = time(0); 

    //BossAI excute after 2 second
    if (player->i_ai->common_flag_ == 0) {
        player->i_ai->common_flag_ = cur_time;
    }
    if (player->i_ai->common_flag_ + 2 > cur_time) {
        return true;
    }

    if (player->i_ai->common_flag1_ == 0 || cur_time >= (player->i_ai->common_flag1_ + 12) ) {
        player->i_ai->common_flag1_ = cur_time;                                                                      
    }                                                                

    if (player->hp < (int)player->i_ai->common_flag3_) {
        if ( (rand() % 100) < 30 ) {
            int rand_num = rand();
            int x_offset = (rand_num % 50) + (350 * (rand_num % 2)) - 200;
            if (player->role_type == 11341) {
                x_offset = rand_num % 160 + 290;
            } else if (player->role_type == 11342) {
                x_offset = -(rand_num % 160 + 290);
            }
            int y_offset = (rand_num % 100) - 100;
            call_monster_to_map(player->cur_map, player->btl, 11337 + (rand_num % 4),
                player->pos().x() + x_offset, player->pos().y() + y_offset, neutral_team_1);
            //ERROR_LOG("xxxxxxxxxxx [%u %u]", player->pos().x() + x_offset, player->pos().y() + y_offset);
        }
    }
    player->i_ai->common_flag3_ = player->hp;
    if (cur_time < (player->i_ai->common_flag1_+6) && cur_time >= player->i_ai->common_flag1_) { 
        if (player->i_ai->common_flag2_ == 0){
            player->invincible_time = true;                  
            uint32_t skill_id = 4120104;
            player_skill_t * skill = player->select_skill(0, skill_id);                                              
            if (skill) {                                                                                             
                monster_attack(player, skill);                                                                       
            } 
            player->i_ai->common_flag2_  = 1;
            if (player->role_type == 11341 || player->role_type == 11342) {
                player->call_map_summon(18, player->pos().x(), player->pos().y(), false);
                //ERROR_LOG("call_map_summon 18 [%u]", player->role_type);
            } else {
                player->call_map_summon(17, player->pos().x(), player->pos().y(), false);
            }
        }
    } else {                                                                                                         
        if (player->i_ai->common_flag2_ == 1) { 
            player->invincible_time = false;                      
            player->i_ai->common_flag2_  = 0;

        }                                                                                                            
    }                                                                                                                
    return true;                                                                                                     
}

bool ThreeWood2_AI::extra_wait_action(Player * player)
{
    uint32_t cur_time = time(0);
    if (player->i_ai->common_flag1_ == 0 || cur_time >= (player->i_ai->common_flag1_ + 12)) {
        player->i_ai->common_flag1_ = cur_time;
    }

    uint32_t map_summon_id = 18;
    if (player->hp < (int)player->i_ai->common_flag3_) {
        if ( (rand() % 100) < 30 ) {
            int rand_num = rand();
            int x_offset = (rand_num % 50) + (350 * (rand_num % 2)) - 200;
            if (player->role_type == 11343) {
                x_offset = rand_num % 160 + 290;
            } else if (player->role_type == 11344) {
                x_offset = -(rand_num % 160 + 290);
            }
            int y_offset = (rand_num % 100) - 100;
            call_monster_to_map(player->cur_map, player->btl, 11337 + (rand_num % 4),
                player->pos().x() + x_offset, player->pos().y() + y_offset, neutral_team_1);
            //ERROR_LOG("xxxxxxxxxxx [%u %u]", player->pos().x() + x_offset, player->pos().y() + y_offset);
        }
    }
    player->i_ai->common_flag3_ = player->hp;

    if (cur_time <= (player->i_ai->common_flag1_ + 6) && cur_time > player->i_ai->common_flag1_) {
        if (player->i_ai->common_flag_ == 0) {
            player->dir = dir_right;
            monster_stand(player);
            uint32_t skill_id = 4120105;
            player_skill_t * skill = player->select_skill(0, skill_id);                                              
            if (skill) {                                                                                             
                monster_attack(player, skill);                                                                       
            }
            player->i_ai->common_flag_  = 1;

            if (player->role_type == 11343 || player->role_type == 11344) {
                map_summon_id = 24;
            }
            //ERROR_LOG("call_map_summon [%u] [%u]", map_summon_id, player->role_type);
            for (int i = 0; i < 2; i++) {
                for (int j = (1 + i); j < (5 - i); j++) {
                    player->call_map_summon(map_summon_id, player->pos().x() - ((2 * i + 1) * 48), 
                        player->pos().y()-151 + (j * 65), false);
                    //player->call_map_summon(18, player->pos().x() - ((2 * i + 1) * 48), 249 + (j * 65), false);
                    //ERROR_LOG("pos l [%u %u]", player->pos().x()-((2*i+1)*48), player->pos().y()-151+(j*65));
                }
            }

        }
    } else {
        if (player->i_ai->common_flag_ == 1) {
            player->dir = dir_left;
            monster_stand(player);
            uint32_t skill_id = 4120105;
            player_skill_t * skill = player->select_skill(0, skill_id);                                              
            if (skill) {                                                                                             
                monster_attack(player, skill);                                                                       
            }

            if (player->role_type == 11343 || player->role_type == 11344) {
                map_summon_id = 24;
            }
            for (int i = 0; i < 2; i++) {
                for (int j = (1 + i); j < (5 - i); j++) {
                    player->call_map_summon(map_summon_id, player->pos().x() + ((2 * i + 1) * 48), 
                        player->pos().y()-151 + (j * 65), false);
                    //player->call_map_summon(18, player->pos().x() + ((2 * i + 1) * 48), 249 + (j * 65), false);
                    //ERROR_LOG("pos r [%u %u]", player->pos().x()+((2*i+1)*48), player->pos().y()-151+(j*65));
                }
            }
            player->i_ai->common_flag_ = 0;
        }
    }
    return true;
}

bool ThreeWoodItem_AI::extra_wait_action(Player * player)
{

	uint32_t cur_time = get_now_tv()->tv_sec;
	player->invincible_time = 5;
	Player * trigger_player = 0;
	if (player->i_ai->common_flag1_ == 0) {
		player->i_ai->common_flag1_ = cur_time;
	} else if (player->i_ai->common_flag1_ + 10 < cur_time) {
		player->set_dead();
		notify_delete_player_obj(player);
		return true;
	}


	for (PlayerSet::iterator iter = player->cur_map->players.begin(); 
			iter != player->cur_map->players.end(); ++iter) {
		if (player->check_distance((*iter), 35)) {
			trigger_player = (*iter);
			break;
		}
	}

	if (trigger_player) {
		switch (player->role_type) {
            case 11337: // 冰冻
                {
                    for (PlayerSet::iterator it = player->cur_map->players.begin();
                        it != player->cur_map->players.end(); ++it) {
                        if ( (*it)->id != trigger_player->id ) {
                            add_player_buff((*it), 701, 0, 6);
                        }
                    }
                }
                break;
            case 11338: //无敌
                {
                    bool is_left = false;
                    if (abs(trigger_player->pos().x() - 900) > abs(trigger_player->pos().x() - 300)) {
                        is_left = true;
                    }
                    for (PlayerSet::iterator it = player->cur_map->monsters.begin();
                        it != player->cur_map->monsters.end(); ++ it) {
                        //for 939 stage
                        if ((*it)->role_type == 11341 || (*it)->role_type == 11342 
                            || (*it)->role_type == 11343 || (*it)->role_type == 11344 
                            || ((*it)->role_type == 13095 && (*it)->pos().y() < 2000) 
                            || ((*it)->role_type == 13096 && (*it)->pos().y() < 2000) ) {
                            if (trigger_player->team == (*it)->team)
                                add_player_buff((*it), 28, 0, 8);
                            continue;
                        }

                        //for 931 stage
                        if ( is_left ) {
                            if ((*it)->role_type == 11319 || (*it)->role_type == 11321 
                                || ( (*it)->role_type == 13090 && (*it)->pos().y() < 2000) ) {
                                add_player_buff((*it), 28, 0, 8);
                                continue;
                            }
                        } else {
                            if ((*it)->role_type == 11318 
                                || (*it)->role_type == 11320 
                                || ((*it)->role_type == 13089 && (*it)->pos().y() < 2000) ) {
                                add_player_buff((*it), 28, 0, 8);
                                continue;
                            }
                        }
                    }
                }
                break;
            case 11339: // 击飞
                    player->call_map_summon(21, trigger_player->pos().x(), trigger_player->pos().y(), false);
                    //add_player_buff(trigger_player, 608, 0, 0);
                break;
            case 11340: //加血
                {
                    for (PlayerSet::iterator it = player->cur_map->monsters.begin();
                        it != player->cur_map->monsters.end(); ++ it) {
                        if ((*it)->role_type == 11318 || (*it)->role_type == 11319 
                            || (*it)->role_type == 11320 || (*it)->role_type == 11321 
                            || ((*it)->role_type == 13089 && (*it)->pos().y() < 2000) 
                            || ((*it)->role_type == 13090 && (*it)->pos().y() < 2000) ) {
                            //add_player_buff((*it), 585, 0, 0);
                            (*it)->recover_hp(30);
                            continue;
                        }

                        if ((*it)->role_type == 11341 || (*it)->role_type == 11342 
                            || (*it)->role_type == 11343 || (*it)->role_type == 11344 
                            || ((*it)->role_type == 13095 && (*it)->pos().y() < 2000) 
                            || ((*it)->role_type == 13096 && (*it)->pos().y() < 2000) ) {
                            (*it)->recover_hp(20);
                            continue;
                        }
                    }
                }
                break;
			default:
				break;
		}
		player->set_dead();
		notify_delete_player_obj(player);

	}

	return true;
}


bool Ghost_AI::extra_wait_action(Player * player)
{
	uint32_t cur_time = get_now_tv()->tv_sec;

	if (player->i_ai->common_flag1_ == 0)  {
		player->i_ai->common_flag1_ = cur_time + 30;
	} else if (player->i_ai->common_flag1_ < cur_time) {
		player->suicide();
		return true;
	}

	if ((player->btl->common_flag2_ > cur_time) || (player->i_ai->common_flag3_ > cur_time)) {
		if (is_player_buff_exist(player, 28)) { //取消无敌
			del_player_buff(player, 28, true);
		}
	} else {
		if (!is_player_buff_exist(player, 28)) { //加上无敌
			add_player_buff(player, 28, 0, 300);
		}
	}

	if (player->btl->common_flag3_ > cur_time) {
		if (!is_player_buff_exist(player, 701)) {
			add_player_buff(player, 701, 0, player->btl->common_flag3_ - cur_time);
		}
		return true;
	}

	if ((player->i_ai->target =  get_target(player)) != NULL) {
		player->i_ai->change_state(MOVE_STATE);
	}
	return true;
}

void Ghost_AI::move(Player * player, int timediff)
{

	uint32_t cur_time = get_now_tv()->tv_sec;

	if (player->i_ai->common_flag1_ < cur_time) {
		player->suicide();
		return;
	}


	if ((player->btl->common_flag2_ > cur_time) || (player->i_ai->common_flag2_ > cur_time)) {
		if (is_player_buff_exist(player, 28)) {
			del_player_buff(player, 28, true);
		}
	} else {
		if (!is_player_buff_exist(player, 28)) {
			add_player_buff(player, 28, 0, 300);
		}
	}

	if (player->btl->common_flag3_ > cur_time) {
		if (!is_player_buff_exist(player, 701)) {
			add_player_buff(player, 701, 0, player->btl->common_flag3_ - cur_time);
		}
		return;
	}


	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, 0)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}

	const Player * target = player->i_ai->target;

	if (player->check_distance(target, 40)) {
		player->suicide();
		return;
	}

	if ((rand() % 2 == 0) && monster_move(player, timediff)) {
		if (player->check_distance(target, 40)) {
			player->suicide();
			return;
		}
	} else { 
		if (player->i_ai->judge_update_tm(2000)) {
			player->i_ai->change_state(WAIT_STATE);
		} 
	}
}

bool Item_AI::extra_wait_action(Player * player)
{

	uint32_t cur_time = get_now_tv()->tv_sec;
	player->invincible_time = 5;
	Player * trigger_player = 0;
	if (player->i_ai->common_flag1_ == 0) {
		player->i_ai->common_flag1_ = cur_time;
	} else if (player->i_ai->common_flag1_ + 8 < cur_time) {
		player->set_dead();
		notify_delete_player_obj(player);
		return true;
	}


	for (PlayerSet::iterator iter = player->cur_map->players.begin(); 
			iter != player->cur_map->players.end(); ++iter) {
		if (player->check_distance((*iter), 35)) {
			trigger_player = (*iter);
			break;
		}
	}

	if (trigger_player) {
		switch (player->role_type) {
			case 11325: //获得5秒buff
				add_player_buff(trigger_player, 28, 0, 5);//28--无敌
				break;
			case 11327: //半径100的怪全死, 召唤一个半径100的，召唤物伤害 100%，
				{
				for (PlayerSet::iterator it = player->cur_map->monsters.begin(); 
							it != player->cur_map->monsters.end(); ++ it) {
					if ((*it)->role_type == 11324 && (*it)->check_distance(player, 100)) {
						if (is_player_buff_exist((*it), 28)) {
								del_player_buff((*it), 28);
							//	ERROR_LOG("NO>>>>>>>>>>>>>>>");
								(*it)->i_ai->common_flag2_ = cur_time + 2;
							}
						}
					}
				trigger_player->call_map_summon(19, player->pos().x(), player->pos().y(), false);
		//		ERROR_LOG("CALL SUMMON %u %u %u", 19, player->pos().x(), player->pos().y());
				}
				break;
			case 11326://小怪无敌取消5s
			//	ERROR_LOG("WUDI QUXIAO!");
				{
					player->btl->common_flag2_ = cur_time + 6;
			//		ERROR_LOG("no hit in 5s");
				}
				break;
			case 11328://小怪不能动5s
			//	ERROR_LOG("BUNENG DONG!");
				{
					player->btl->common_flag3_ = cur_time + 6;
				}
				break;
			default:
				break;
		}
		player->set_dead();
		notify_delete_player_obj(player);

	}

	return true;
}

bool Ghost_T_AI::extra_wait_action(Player * player)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (player->i_ai->common_flag1_ == 0) {
		player->i_ai->common_flag1_ = cur_time;
	} else if (player->i_ai->common_flag1_ + 10 < cur_time) {
		player->suicide();
		return true;
	}


	if ((player->i_ai->target =  get_target(player)) != NULL) {
		player->i_ai->change_state(MOVE_STATE);
	}

	return true;

}

void Ghost_T_AI::move(Player * player, int timediff)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (player->i_ai->common_flag1_ + 10 < cur_time) {
		player->suicide();
		return;
	}

	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, 0)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}

	if ((rand() % 2 == 0) && monster_move(player, timediff)) {

	} else { 
		if (player->i_ai->judge_update_tm(2000)) {
			player->i_ai->change_state(WAIT_STATE);
		} 
	}

}

/*---------------------------------------------*/
bool Tool_AI::extra_wait_action(Player* player)
{
	switch (player->role_type)
	{
		case 11352:
		{
			PlayerSet::iterator  pItr = player->cur_map->players.begin();
			for(; pItr != player->cur_map->players.end(); ++pItr)
			{
				Player* p = *pItr;
				if( player->check_distance(p, 35))
				{
					PlayerSet::iterator pItr2 = player->cur_map->monsters.begin();
					for(; pItr2 != player->cur_map->monsters.end(); ++pItr2)
					{
						Player* target = *pItr2;
						if( target->team == 3 && !target->is_dead() && target->pos().y() == player->pos().y())
						{
							player->call_map_summon(26, target->pos().x(), target->pos().y(), false );
						}
					}
					
					PlayerSet::iterator  pItr3 = player->cur_map->monsters.begin(); 
					for(; pItr3 != player->cur_map->monsters.end(); ++pItr3)
					{
						Player* p2 = *pItr3;
						if(p2->role_type == 11352 || p2->role_type == 11353 || p2->role_type == 11354 || p2->role_type == 11355)
						{
							p2->set_dead();
							notify_delete_player_obj(p2);
						}
					}
					break;
				}
			}
			
		}
		break;

		case 11353:
		{
			PlayerSet::iterator  pItr = player->cur_map->players.begin();
			for(; pItr != player->cur_map->players.end(); ++pItr)
			{
				Player* p = *pItr;
				if( player->check_distance(p, 35))
				{
					player->call_map_summon(27, player->pos().x(), player->pos().y(), false);
					//player->set_dead();
					//notify_delete_player_obj(player);
					PlayerSet::iterator  pItr3 = player->cur_map->monsters.begin(); 
					for(; pItr3 != player->cur_map->monsters.end(); ++pItr3)
					{
						Player* p2 = *pItr3;
						if(p2->role_type == 11352 || p2->role_type == 11353 || p2->role_type == 11354 || p2->role_type == 11355)
						{
							p2->set_dead();
							notify_delete_player_obj(p2);
						}
					}
					break;	
				}
			}
		}	
		break;

		case 11354:
		{
			PlayerSet::iterator  pItr = player->cur_map->players.begin();
			for(; pItr != player->cur_map->players.end(); ++pItr)
			{
				Player* p = *pItr;
				if (player->check_distance(p, 35))
				{	
					
					PlayerSet::iterator pItr2 = player->cur_map->monsters.begin();
					for(; pItr2 != player->cur_map->monsters.end(); ++pItr2)
					{
						Player* target = *pItr2;
						if( target->team == 3 && !target->is_dead())
						{
							player->call_map_summon(25, target->pos().x(), target->pos().y(), false);
						}	
					}
					PlayerSet::iterator  pItr3 = player->cur_map->monsters.begin(); 
					for(; pItr3 != player->cur_map->monsters.end(); ++pItr3)
					{
						Player* p2 = *pItr3;
						if(p2->role_type == 11352 || p2->role_type == 11353 || p2->role_type == 11354 || p2->role_type == 11355)
						{
							p2->set_dead();
							notify_delete_player_obj(p2);
						}
					}	
					break;
				}
			}
		}
		break;

		case 11355:
		{
			 PlayerSet::iterator  pItr = player->cur_map->players.begin();
			 for(; pItr != player->cur_map->players.end(); ++pItr) 
			 {
				Player* p = *pItr;
				if (player->check_distance(p, 35))
				{
					player->cur_map->add_all_monsters_buff(900, 3, NULL);
					
					PlayerSet::iterator  pItr3 = player->cur_map->monsters.begin();
					for(; pItr3 != player->cur_map->monsters.end(); ++pItr3)
					{
						Player* p2 = *pItr3;
						if(p2->role_type == 11352 || p2->role_type == 11353 || p2->role_type == 11354 || p2->role_type == 11355)
						{
							player->set_dead();
							notify_delete_player_obj(p2);
						}
					}
					break;	
				}
			 }
		}
		break;

	}	
	return true;
}



bool Box_943_AI::extra_wait_action(Player* player)
{
	uint32_t count = 0;
	PlayerSet::iterator  pItr = player->cur_map->monsters.begin();
	for(; pItr != player->cur_map->monsters.end(); ++pItr)
	{
		Player* p = *pItr;
		if(p->role_type != 11362)count++;
	}	
	
	if(player->i_ai->common_flag_ == 0 && count == 0)
	{
		teleport(player, 883, 604);
		player->i_ai->common_flag_++;
		return true;
	}

	return true;
}

bool DarkCurse_AI::extra_wait_action(Player * player)
{
	player->i_ai->target_path.clear();
	//find target
	player->i_ai->target = 0;
	//can't find any target
	if (!(player->i_ai->target = get_target(player))){
		return true;
	}

	const Player * target = player->i_ai->target;

	uint32_t now = get_now_tv()->tv_sec;

	switch (player->role_type) {
		case 11414: //for ice
		case 11419: //for fury
		{
			if (use_buf_skill(player) ) {
				return true;
			}
		}
		break;
		case 11421: //for summon
		{
			if (!player->i_ai->common_flag_) {
				player->i_ai->common_flag_ = now - 11;
			}

			if (player->i_ai->common_flag_ + 15 < now) {
				int x = (player->pos().x() + target->pos().x()) / 2;
				int y = (player->pos().y() + target->pos().y()) / 2;
				int x1 = x - 50;
				int y1 = y - 50;
				int x2 = x + 50;
				int y2 = y + 50;
				call_monster_to_map(player->cur_map, player->btl, 11424, x1, y1, player->team);
				call_monster_to_map(player->cur_map, player->btl, 11424, x2, y2, player->team);
				player->i_ai->common_flag_ = now;
				return true;
			}
		}
		break;
		default:
			break;
	}
	return false;
}

bool DarkWizard_AI::extra_wait_action(Player * player)
{
return false;
}

bool Worm_AI::extra_wait_action(Player * player)
{
	player->i_ai->target_path.clear();
	//find target
	player->i_ai->target = 0;
	//can't find any target
	if ((player->i_ai->target = get_target(player))){
		const Player * target = player->i_ai->target;
		if (player->pos().distance(target->pos()) < 75) {
			player->suicide();
		} else {
			player->i_ai->change_state(MOVE_STATE);
		}
	}
	return true;
}

void Worm_AI::move(Player * player, int tm_diff)
{
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, 0)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	const Player* target = player->i_ai->target;
	if (monster_move(player, tm_diff)) {
		if (player->pos().distance(target->pos()) < 75) {
			player->suicide();
		}
		//if can attack
	} else {
		//TODO:
		if (player->i_ai->judge_update_tm(1000)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}

}

bool WuSeng_AI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag2_ == 0) {
		if (player->pos().x() == 800) {
			player->i_ai->common_flag2_ = 1;
		} else if (player->pos().x() == 1050) {
			player->i_ai->common_flag2_ = 2;
		} else {
			player->i_ai->common_flag2_ = 3;
		}
		ERROR_LOG("WUSENG COMMON_FLAG2 %u", player->i_ai->common_flag2_);
	}
	//common_flag1_ for pre hp
	//uint32_t nondead_skill_id = 4120155;
	uint32_t recover_hp_id = 4120156;
	
	if (player->hp <= (player->max_hp() / 40)) {
			player->hp = player->max_hp();
			player->noti_hpmp_to_btl();
	} else if (player->hp <= (player->max_hp() / 2) && player->hp <(int) player->i_ai->common_flag1_) {
		if (!player->cur_map->get_one_monster(11476)) {
			uint32_t call_dargon_skill_id = 4120154;
			player_skill_t * skill = player->select_skill(0, call_dargon_skill_id);
			if (skill) {
				monster_attack(player, skill);
	 	  		call_monster_to_map(player->cur_map,
			              			player->btl,
						   			11476,
						   			player->pos().x(),
						   			player->pos().y(),
						   			player->team); 
			}
		}
	}

	player->i_ai->common_flag1_ = player->hp;

	player_skill_t * skill = player->select_skill(0, recover_hp_id);
	if (skill) {
		monster_attack(player, skill);
		player->hp += (player->max_hp() / 100);
		player->noti_hpmp_to_btl();
	}
	//true wuseng shouldn't dead before player when battle is over
	if (player->btl->common_flag2_) {
		teleport(player, 10000, 10000);
		player->btl->common_flag3_ = 1;
		ERROR_LOG("TRUE MOVE WUSENG!");
	}
	return false;
}

void WuSeng_AI::move(Player * player, int timediff)
{
	static uint32_t area_x[3] = { 800, 1050, 1350};
	static uint32_t area_y[3] = { 307, 600, 357};
//	uint32_t has_near_flag = 0;
//	//find a place far away other Wushengs
//	for (PlayerSet::iterator it = player->cur_map->monsters.begin(); 
//			it != player->cur_map->monsters.end(); ++it) {
//		Player * p = *it;	
//		if (player->pos().distance(p->pos()) < 200 ) {
//			has_near_flag++;
//			break;
//		}	
//	}
	uint32_t player_x = player->pos().x();
	uint32_t player_y = player->pos().y();
	uint32_t idx = player->i_ai->common_flag2_;
	if (::abs(player_x - area_x[idx - 1]) < 100 && ::abs(player_y - area_y[idx - 1]) < 100) {
		monster_stand(player);
		player->i_ai->change_state(WAIT_STATE);
		return;
	}

	player->i_ai->target = get_target(player);
	if (player->i_ai->target) {
		player_skill_t * skill = player->select_skill(player->i_ai->target);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}	
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();


	const KfAstar::Points* pts = 0; 
	KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
	if(p_star)
	{
		pts = p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()),  KfAstar::Point(area_x[idx - 1], area_y[idx - 1]));
	}

	if (pts){
		player->i_ai->target_path.assign(pts->begin(), pts->end());
		get_one_target_pos(player);
		monster_move_ex(player, timediff);
	}
	if (player->i_ai->judge_update_tm(1000)) {
		player->i_ai->change_state(WAIT_STATE);
	}

}


bool GuiseWuSeng_AI::extra_wait_action(Player * player)
{
	if (player->i_ai->common_flag2_ == 0) {
		if (player->pos().x() == 800) {
			player->i_ai->common_flag2_ = 1;
		} else if (player->pos().x() == 1050) {
			player->i_ai->common_flag2_ = 2;
		} else {
			player->i_ai->common_flag2_ = 3;
		}
		ERROR_LOG("GUI WUSENG FLAG2 %u", player->i_ai->common_flag2_);
	}
	
	//find a place far away other Wushengs

	uint32_t recover_hp_id = 4120156;
	uint32_t call_dargon_id = 4120157;

	if (player->hp <= (player->max_hp() / 2) && player->hp < (int)player->i_ai->common_flag1_) {
		if (player->cur_map->get_monster_cnt(11477) < player->cur_map->get_monster_cnt(11475)) {

			player_skill_t * skill = player->select_skill(0, call_dargon_id);
			if (skill) {
				monster_attack(player, skill);

       			call_monster_to_map(player->cur_map,
			         	  			player->btl,
							 		11477,
									player->pos().x(),
									player->pos().y(),
									player->team,
									0,
									65); 
			}
		}
	}

	player->i_ai->common_flag1_ = player->hp;	

	player_skill_t * skill = player->select_skill(0, recover_hp_id);
	if (skill) {
		monster_attack(player, skill);
		player->hp += (player->max_hp() / 100);
		player->noti_hpmp_to_btl();
	}

	return false;
}

void GuiseWuSeng_AI::move(Player * player, int timediff)
{
	static uint32_t area_x[3] = { 800, 1050, 1350};
	static uint32_t area_y[3] = { 307, 600, 357};
//	uint32_t has_near_flag = 0;
//	//find a place far away other Wushengs
//	for (PlayerSet::iterator it = player->cur_map->monsters.begin(); 
//			it != player->cur_map->monsters.end(); ++it) {
//		Player * p = *it;	
//		if (player->pos().distance(p->pos()) < 200 ) {
//			has_near_flag++;
//			break;
//		}	
//	}
	uint32_t player_x = player->pos().x();
	uint32_t player_y = player->pos().y();
	uint32_t idx = player->i_ai->common_flag2_;
	if (::abs(player_x - area_x[idx - 1]) < 100 && ::abs(player_y - area_y[idx - 1]) < 100) {
		monster_stand(player);
		player->i_ai->change_state(WAIT_STATE);
		return;
	}

	player->i_ai->target = get_target(player);
	if (player->i_ai->target) {
		player_skill_t * skill = player->select_skill(player->i_ai->target);
		if (skill) {
			monster_attack(player, skill);
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}	
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();


	const KfAstar::Points* pts = 0; 
	KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
	if(p_star)
	{
		pts = p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()),  KfAstar::Point(area_x[idx - 1], area_y[idx - 1]));
	}

	if (pts){
		player->i_ai->target_path.assign(pts->begin(), pts->end());
		get_one_target_pos(player);
		monster_move_ex(player, timediff);
	}
	if (player->i_ai->judge_update_tm(1000)) {
		player->i_ai->change_state(WAIT_STATE);
	}
}

bool PreHistory_Wuseng::extra_wait_action(Player * player)
{
	uint32_t now_time = get_now_tv()->tv_sec;

	if (!player->i_ai->common_flag1_) {
		player->i_ai->common_flag1_ = now_time;
		add_player_buff(player, 28, 0, 90, 0);
		return true;
	} 

	if (player->i_ai->common_flag1_ + 3 == now_time) {
		player_skill_t * skill = player->select_skill(0, 4120173);
		if (skill) {
			monster_attack(player, skill);
		}
		return true;
	}

	if (player->i_ai->common_flag1_ + 90 > now_time && !player->btl->common_flag3_) {
		player_skill_t * skill = player->select_skill(0, 4120174);
		if (skill) {
			monster_attack(player, skill);
		ERROR_LOG("WUSENG USE SKILL 4120174");

		}
		return true;
	} 

	if (is_player_buff_exist(player, 28)) { //取消无敌
		del_player_buff(player, 28, true);
	}

	player_skill_t * skill = player->select_skill(0, 4120176);
	if (skill) {
		ERROR_LOG("WUSENG USE SKILL 4120176");
		monster_attack(player, skill);
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

bool FireMon_AI::extra_wait_action(Player * player)
{
	uint32_t fire_skill_id = 4120200;
	player_skill_t * skill = player->select_skill(0, fire_skill_id);
	player->invincible = true;
	if (skill) {
		monster_attack(player, skill, player->pos().x(), player->pos().y());
//		ERROR_LOG("SPACE FIRE MON USE >>>>%u %u %u", 4120200, player->pos().x(), player->pos().y());
	}	
	player->invincible = true;
	return true;
}

bool ShouTouTuo_AI::extra_wait_action(Player * player)
{
	player->i_ai->target = 0;
	if (player->i_ai->common_flag1_ == 0) {
		player->i_ai->common_flag1_ = player->hp;
		player->i_ai->common_flag2_ = player->hp / 4;
	} else {
		if (player->hp + player->i_ai->common_flag2_ < player->i_ai->common_flag1_) {
			uint32_t hp_skill_id = 4120211;
			player_skill_t * skill = player->select_skill(NULL, hp_skill_id);
			if (skill) {
				monster_attack(player, skill);
				player->i_ai->common_flag1_ -= player->i_ai->common_flag2_;
				return true;
			}
		}
	}

	if ( (player->i_ai->target = get_target(player)) != 0 ) {	
		//if can attack
		const Player* target = player->i_ai->target;
		if (player->check_distance(target, 200) && !player->check_distance(target, 600)) {
			uint32_t far_skill_id = 4120210;
			player_skill_t * skill = player->select_skill(target, far_skill_id);
			if (skill) {
				monster_attack(player, skill);
			}
		} else {
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

/****************************************************************
 * TrackBombAI
****************************************************************/
bool TrackBombAI::extra_wait_action(Player* player)
{
    uint32_t track_bomb_cnt = get_monster_cnt(player, player->role_type);
    if (track_bomb_cnt == 3) {
        player->suicide();
        return true;
    }

    if (player->btl->is_battle_pve() && player->cur_map->get_monster_cnt() - track_bomb_cnt == 0) {
        player->suicide();
        return true;
    }

	Player* p =  get_target(player);
	if( p ){
		if( player->check_distance(p, 100) )
		{
            //player->call_map_summon(52, player->pos().x(), player->pos().y(), false);
			player->suicide();
			return true;
		}
	}
	return false;
}

bool TrackBombAI::extra_attack_action(Player* player)
{
	Player* p =  get_target(player);
	if( p )
	{
		if( player->check_distance(p, 100) )
		{
            //player->call_map_summon(52, player->pos().x(), player->pos().y(), false);
			player->suicide();
			return true;
		}
	}
	return false;
}

/****************************************************************
 * LampDefenceAI
****************************************************************/
bool LampDefenceAI::extra_wait_action(Player* player)
{
	if (player->i_ai->common_flag1_) {
		return false;
	}
	Player* target = player->cur_map->get_one_monster(11533);
	player->i_ai->target = target;
	if (target) {
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
			return true;
		}	
		if (player->mon_info->spd) {
			player->i_ai->change_state(MOVE_STATE);
			return true;
		}
	}
	return false;
}

bool LampDefenceAI::extra_attack_action(Player* player)
{
	return false;
}

bool LampDefenceAI::extra_move_action(Player* player)
{
	if (player->i_ai->common_flag1_ == 0) {
		Player* target = player->cur_map->get_one_monster(11533);
		if (target && player->check_distance(target, 200) ) {
			player->i_ai->common_flag1_ = 1;
			player->i_ai->change_state(WAIT_STATE);
		}
	}
	return false;
}

