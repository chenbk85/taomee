/**
 *============================================================
 *  @file      GuardAI.cpp
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
#include "GuardAI.hpp"
#include "../player.hpp"
#include "../stage.hpp"
#include "../battle_impl.hpp"
using namespace taomee;

void
GuardAI::wait(Player* player, int millisec)
{
	TRACE_LOG("wait state: uid=%u", player->id);

	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
         return;
	}
	
	//clear pre target
	player->i_ai->target_pos.init();
	player->i_ai->target_path.clear();
	
	
	if (!jugde_in_guard_range(player) && player->mon_info->spd) {
		player->i_ai->change_state(LINGER_STATE);
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
		if (player->mon_info->spd && jugde_in_guard_range(target)) {
			player->i_ai->change_state(MOVE_STATE);
			return;
		} else if (player->mon_info->spd /* && !jugde_in_guard_range(target) */) {
			player->i_ai->change_state(LINGER_STATE);
			return;
		}
	}
}

void 
GuardAI::linger(Player* player, int millisec)
{
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_guard_pos(player)) {
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
			//player->i_ai->set_state_keep_tm(rand() % 2);
			player->i_ai->change_state(WAIT_STATE);
		}
	}
}

void
GuardAI::move(Player* player, int millisec)
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
			player->i_ai->change_state(LINGER_STATE);
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
GuardAI::evade(Player* player, int millisec)
{
	//TRACE_LOG("evade state: uid=%u", player->id);
	//player->i_ai->set_trace_flag(false);
	player->i_ai->change_state(WAIT_STATE);
}

void
GuardAI::attack(Player* player, int millisec)
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
	
  //player->i_ai->set_state_keep_tm(rand() % 2 + 1);	
	player->i_ai->change_state(WAIT_STATE);
}

void
GuardAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		player->i_ai->set_state_keep_tm(0);
		
		player->i_ai->target_pos.init();
		
		//TODO:dodge
		int rand_num = rand() % 100;
		if (rand_num < 25) {
			attack(player, millisec);
			return;
		}
		
		//return to wait
		player->i_ai->change_state(WAIT_STATE);
	}
}

void 
GuardAI::defense(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

bool
GuardAI::jugde_in_guard_range(const Player* player)
{
	if (player->i_ai->guard_range == 0) {
		return true;
	}
	
	Vector3D guard_pos;
	if (player->i_ai->clientele) {
		guard_pos = player->i_ai->clientele->pos();
	} else {
		guard_pos = player->i_ai->init_pos;
	}
	
	if ( abs(player->pos().x() - guard_pos.x()) > player->i_ai->guard_range 
				|| abs(player->pos().y() - guard_pos.y()) > player->i_ai->guard_range ) {
		return false;
	}
	
	return true;
}

bool 
GuardAI::calc_guard_pos(const Player* player)
{
	static int sign[10] = { -1, 1, 1, 1, -1, -1, 1, -1, -1, 1 };
	
	if (!(player->i_ai->clientele)) {
		return calc_return_pos(player);
	}
	
	int guard_list = 200;
	int fine_adjust = 50;
	int dir = player->id % 4;
	
	int x, y;
	if (dir == 0) {//right
		x = player->i_ai->clientele->pos().x() + guard_list;
		y = player->i_ai->clientele->pos().y() + ranged_random(0, fine_adjust) * sign[rand() % 10];
	} else if (dir == 1) {//left
		x = player->i_ai->clientele->pos().x() - guard_list;
		y = player->i_ai->clientele->pos().y() + ranged_random(0, fine_adjust) * sign[rand() % 10];;
	} else if (dir == 2) {//down
		x = player->i_ai->clientele->pos().x() + ranged_random(0, fine_adjust) * sign[rand() % 10];;
		y = player->i_ai->clientele->pos().y() + guard_list / 2;
	} else if (dir == 3) {//up
		x = player->i_ai->clientele->pos().x() + ranged_random(0, fine_adjust) * sign[rand() % 10];;
		y = player->i_ai->clientele->pos().y() - guard_list / 2;
	}
	

	if (x < player->mon_info->len / 2) {
		x = player->mon_info->len / 2;
	}
	if (y < player->mon_info->height) {
		y = player->mon_info->height;
	}

	//const KfAstar::Points* pts = player->cur_map->path->findpath(KfAstar::Point(player->pos().x(), player->pos().y()),
	//																KfAstar::Point(x, y));

	KfAstar::Points* pts = NULL;
	KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
	if(p_star)
	{
		pts = (KfAstar::Points*)p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()), KfAstar::Point(x, y));
	}

	if (pts) {
		player->i_ai->target_path.assign(pts->begin(), pts->end());	
		get_one_target_pos(player);
	} else {
		return false;
	}

	TRACE_LOG("guard target: uid=%u to=%s v=%s", player->id, player->i_ai->target_pos.to_string().c_str(),
				player->velocity().to_string().c_str());
	
	return true;
}
	
	
