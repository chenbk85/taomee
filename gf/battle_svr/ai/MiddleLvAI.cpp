/**
 *============================================================
 *  @file      MiddleLvAI.cpp
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
#include "MiddleLvAI.hpp"
#include "../player.hpp"
#include "../stage.hpp"

using namespace taomee;

void
MiddleLvAI::wait(Player* player, int millisec)
{

	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
         return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
	
	//extra action for boss
	if (extra_wait_action(player)) {
		return;
	}
	
	//30% odds to evade:be at death's door
	if ( is_near_dead(player) ) {
		int rand_num = rand() % 100;
		if (rand_num < 30) {
			if (player->i_ai->target) {
				int dist = player->pos().squared_distance(player->i_ai->target->pos());
				if (dist < player->mon_info->visual_field) {
					player->i_ai->change_state(EVADE_STATE);
				}
			}
			return;
		}
	}
	
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
			//TODO:
			if ( get_near_target_cnt(player) > 3) {
				player->i_ai->change_state(LINGER_STATE);
				return;			
			}
			player->i_ai->change_state(MOVE_STATE);
			return;
		}
	} else {
		// target out of visual_field
		/*if (player->i_ai->hatred() == 0) {
				calc_return_pos(player);
		}
		player->i_ai->change_state(LINGER_STATE);*/
		
		if (calc_return_pos(player)) {
			player->i_ai->change_state(LINGER_STATE);
		}
		return;		
	}
}

void 
MiddleLvAI::linger(Player* player, int millisec)
{
	player->i_ai->set_trace_flag(false);
	
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
			player->i_ai->set_state_keep_tm(rand() % 3 + 1);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else if (player->mon_info->long_range_atk == 1 && player->i_ai->judge_update_tm(1000)) {
		if (target && player->select_skill(target)) {
				monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
		}
	}
}

void
MiddleLvAI::move(Player* player, int millisec)
{
	player->i_ai->set_trace_flag();
	
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_interval_move_pos(player)) {
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
			player->i_ai->set_state_keep_tm(rand() % 3 + 1);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:take a rest
		if (player->i_ai->move_dist_ > player->mon_info->visual_field_sqrt / 2) {
			int rand_num = rand() % 100;
			if (rand_num > 15) {
				return;
			}
			monster_stand(player);
			player->i_ai->set_state_keep_tm(rand() % 1 + 1);
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
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
MiddleLvAI::evade(Player* player, int millisec)
{
	//TRACE_LOG("evade state: uid=%u", player->id);
	player->i_ai->set_trace_flag(false);
	
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
			player->i_ai->set_state_keep_tm(rand() % 4 + 1);
			player->i_ai->change_state(WAIT_STATE);
		}
	}
}

void
MiddleLvAI::attack(Player* player, int millisec)
{
	if (extra_attack_action(player)) {
		player->i_ai->change_state(WAIT_STATE);
		return;
	}
	
	player->i_ai->set_trace_flag();
	
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
	
  player->i_ai->set_state_keep_tm(rand() % 2 + 1);	
	player->i_ai->change_state(WAIT_STATE);
}

void
MiddleLvAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->set_state_keep_tm(0);
		
		player->i_ai->target_pos.init();
		
		//extra action for boss
		if (extra_stuck_action(player)) {
			return;
		}
		
		//TODO:
		if (player->i_ai->target && player->i_ai->hatred() > 3) {
			attack(player, millisec);
			return;
		}
		
		//TODO:dodge
		int rand_num = rand() % 100;
		if (rand_num < 25 && calc_dodge_pos(player)) {
			player->i_ai->change_state(EVADE_STATE);
			return;
		} else {
			//clear pre target
			attack(player, millisec);
			return;
		}
		
		//return to wait
		player->i_ai->change_state(WAIT_STATE);
	}
}

void 
MiddleLvAI::defense(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}
