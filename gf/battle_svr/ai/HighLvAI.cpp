/**
 *============================================================
 *  @file      HighLvAI.cpp
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
#include "../player.hpp"
#include "../stage.hpp"

using namespace taomee;

void
HighLvAI::wait(Player* player, int millisec)
{
	//TRACE_LOG("HighLvAI State: wait state: uid=%u", player->id);
	
	//TODO:wait some time
	if (player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
  	return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
//	if (use_buf_skill(player)) {
//		return;
//	}
	
	//extra action for boss
	if (extra_wait_action(player)) {
		return;
	}
	if (use_buf_skill(player)) {
		return;
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
			player->i_ai->change_state(MOVE_STATE);
			return;
		}
	} else {
		//player->i_ai->change_state(LINGER_STATE);	
		if (calc_return_pos(player)) {
			player->i_ai->change_state(LINGER_STATE);
		}
	}
}

void 
HighLvAI::linger(Player* player, int millisec)
{
	//TRACE_LOG("HighLvAI State: linger state: uid=%u", player->id);
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_linger_pos(player)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	
	//move to target pos
	if (monster_move(player, millisec)) {
		//if can attack
		const Player* target = player->i_ai->target;
		if (target && player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			player->i_ai->set_state_keep_tm(rand() % 4 + 1);
			player->i_ai->change_state(WAIT_STATE);
		}
	}
}

void
HighLvAI::move(Player* player, int millisec)
{
	//TRACE_LOG("HighLvAI State: move state: uid=%u", player->id);
	//calc target pos
	//

	const Player* target = player->i_ai->target;

	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, player->i_ai->ready_skill_id_)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}

	if (extra_move_action(player)) {
		return;
	}
	
	//move to target pos
	if (monster_move(player, millisec)) {
		//if can attack
		if (player->select_skill(target, player->i_ai->ready_skill_id_)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			player->i_ai->set_state_keep_tm(rand() % 1 + 1);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:attack when monster close to player
		if (player->i_ai->judge_update_tm(1000)) {
			//if can attack
			if (player->select_skill(target)) {
				monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
			}
		}
		
		//TODO:adjust monster move dir
		/*if () {
			
		}*/
	}
}

void
HighLvAI::evade(Player* player, int millisec)
{
	//TRACE_LOG("HighLvAI State: evade state: uid=%u", player->id);
	
	//calc evade pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_new_evade_pos(player)) {
			//player->i_ai->change_state(WAIT_STATE);
			player->i_ai->change_state(LINGER_STATE);
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
			player->i_ai->set_state_keep_tm(rand() % 3 + 1);
			player->i_ai->change_state(WAIT_STATE);
		}
	}
}

void
HighLvAI::attack(Player* player, int millisec)
{
	//TRACE_LOG("HighLvAI State: attack state: uid=%u", player->id);
	//extra action for boss
	if (extra_attack_action(player)) {
		player->i_ai->change_state(WAIT_STATE);
		return;
	}
	
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
		
	//player->i_ai->set_state_keep_tm(rand() % 2);
	player->i_ai->change_state(WAIT_STATE);
}

void
HighLvAI::stuck(Player* player, int millisec)
{
	//TRACE_LOG("HighLvAI State: stuck state: uid=%u", player->id);
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->target_pos.init();
		player->i_ai->target_path.clear();

		//extra action for boss
		if (extra_stuck_action(player)) {
			return;
		}
		
		attack(player, millisec);
		//player->i_ai->change_state(WAIT_STATE);
	}
}

void 
HighLvAI::defense(Player* player, int millisec)
{
	//extra defense for boss
	if (extra_defense_action(player)) {
		return;
	}
	
	player->i_ai->change_state(WAIT_STATE);
}

void 
HighLvAI::call_monster(Player* player)
{
	map_t*  m = player->cur_map;
	
	//TODO:
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) {
		Player* p = *it;
		//TODO:
		if (p->role_type == 11058) {
			p->i_ai->select_ai(GUARD_AI);
			p->i_ai->clientele = player;
			p->i_ai->guard_range = player->mon_info->visual_field_sqrt / 2;
		}
	}
}

