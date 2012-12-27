/**
 *============================================================
 *  @file      LowLvAI.cpp
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
#include "LowLvAI.hpp"
#include "../player.hpp"
#include "../stage.hpp"

using namespace taomee;

void
LowLvAI::wait(Player* player, int millisec)
{
	
	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
  	return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
	
	
	player->i_ai->target = 0;
	//get target
	if ( (player->i_ai->target = get_target(player)) != 0 ) {
		//if can attack
		const Player* target = player->i_ai->target;
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
			return;
		}
		
		// move
		if (player->mon_info->spd) {
			if (player->i_ai->hatred()) {
				player->i_ai->change_state(MOVE_STATE);
			} else {
				int rand_num = rand() % 100;
				if (rand_num < 70) {
					player->i_ai->change_state(MOVE_STATE);
				} else {
					player->i_ai->change_state(LINGER_STATE);
				}
			}
			return;
		}
		
	} else {
		// target out of visual_field
		/*calc_return_pos(player);
		player->i_ai->change_state(LINGER_STATE);*/
		
		if (calc_return_pos(player)) {
			player->i_ai->change_state(LINGER_STATE);
		}
		return;
	}
}

void 
LowLvAI::linger(Player* player, int millisec)
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
			//printf("tim;%u %u\n",player->i_ai->state_last_tm, player->i_ai->state_keep_tm);
			player->i_ai->set_state_keep_tm(rand() % 4 + 1);
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
LowLvAI::move(Player* player, int millisec)
{
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
			//printf("tim;%u %u\n",player->i_ai->state_last_tm, player->i_ai->state_keep_tm);
			player->i_ai->set_state_keep_tm(rand() % 3 + 1);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else if (player->mon_info->long_range_atk == 1 && player->i_ai->judge_update_tm(1000)) {
		if (player->select_skill(target)) {
			monster_stand(player);
			player->i_ai->target_pos.init();
			player->i_ai->change_state(ATTACK_STATE);
		}
	}
}

void
LowLvAI::evade(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
LowLvAI::attack(Player* player, int millisec)
{
	player_skill_t* skill = player->select_skill(player->i_ai->target);
	if (skill) {
		monster_attack(player, skill);
		player->i_ai->set_hatred(0);
	} 

	player->i_ai->set_state_keep_tm(rand() % 3 + 1);
	player->i_ai->change_state(WAIT_STATE);
}

void
LowLvAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->set_state_keep_tm(0);
		
		if (player->i_ai->target && player->i_ai->hatred() > 3) {
			attack(player, millisec);
			return;
		}
		player->i_ai->change_state(WAIT_STATE);
	}
}

void 
LowLvAI::defense(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}
