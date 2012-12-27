/**
 *============================================================
 *  @file      MonsterAI.cpp
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

using namespace taomee;

#include "MonsterAI.hpp"
#include "NullAI.hpp"
#include "LowLvAI.hpp"
#include "MiddleLvAI.hpp"
#include "HighLvAI.hpp"
#include "BossAI.hpp"
#include "SummonMonAI.hpp"

#include "LongRangeAtkAI.hpp"
#include "GuardAI.hpp"
#include "SpecialMonAI.hpp"
#include "MoveAI.hpp"
#include "LuaAI.hpp"
#include "../battle_impl.hpp"
#include "../stage.hpp"

//----------------------------------------------------------
// MonsterAI class
//----------------------------------------------------------
Player*
MonsterAI::get_target(const Player* player)
{
	map_t*  m = player->cur_map;
	Player* target = 0;
	int dist = 2000000000;
	int sum_dist = 2000000000;

	if( player->team == neutral_team_1)return NULL;

	 for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) {
		 Player* p = *it;
		 if(p->in_battle() && !is_summon_mon(p->role_type) && p->team != player->team && p->team != neutral_team_1){
			 int tmp = player->pos().squared_distance(p->pos());
			 if(tmp < dist){
				 target = p;
				 dist = tmp;
			 }
		 }
	 }
	

	if( player->team != neutral_team_1) 
	{
		for (PlayerSet::iterator it = m->players.begin(); it != m->players.end(); ++it) 
		{
			Player* p = *it;
			if (p->in_battle() && p->team != player->team) {
                if ((p->my_summon) && !(p->my_summon->is_dead())) {
                    sum_dist = player->pos().squared_distance(p->my_summon->pos());
                }
				int tmp = player->pos().squared_distance(p->pos());

                if (tmp <= sum_dist && tmp < dist) {
                    target = p;
                    dist   = tmp;
                } else if (tmp > sum_dist && sum_dist < dist) {
                    target = p->my_summon;
                    //NOTI_LOG(" AI [%u] attack [%u]", player->role_type, target->role_type);
                    dist   = sum_dist;
                }
			}
		}
	}
	
	if (dist < player->mon_info->visual_field) {
		return target;
	}
	
	return 0;
}
/**
 * @brief search monster in visual field
 */
Player* 
MonsterAI::get_summon_target(const Player* summon)
{
	map_t*  m = summon->cur_map;
	//Player* owner = summon->summon_info.owner;
	Player* target = 0;
	//int dist = 20000;//summon->mon_info->visual_field;
	//for pvp	
	if (summon->btl->battle_mode() == 	btl_mode_lv_matching) {	
		 if (m->players.empty()) {
       		 return target;
   		 }

   		PlayerSet::iterator it = m->players.begin();
        //target = *it;
	    for ( ; it != m->players.end(); ++it) {
	     	Player* p = *it;
		    if( p->team == summon->team){
		  	  continue;
		     }
		     if( p->team == neutral_team_1){
			  continue;
		     }

			 if (p->team == barrier_team || p->team == barrier_team_1) {
			  continue;
			 }

		     if(p->team == summon->team ){
			   continue;	
			 } 

        
       		 if (p->in_battle()) {
            	if (target == NULL) {
               		target = p;
            	}

            	int tmp = summon->pos().squared_distance(p->pos());
            	if (summon->pos().squared_distance(target->pos()) > tmp) {
             	  	target = p;
            	}

				if (p->my_summon && !p->my_summon->is_dead()) {
					tmp = summon->pos().squared_distance(p->my_summon->pos());
					if (summon->pos().squared_distance(target->pos()) > tmp) {
						target = p->my_summon;
					}
				}
        	}
		}

   	 	it = m->monsters.begin();
		for (; it != m->monsters.end(); ++it) {
			Player * monster = *it;
			if (monster->team == monster_team && monster->in_battle()) {
				if (target == NULL) {
					target = monster;
				}
				int tmp = summon->pos().squared_distance(target->pos());
            	
				if (monster->pos().squared_distance(monster->pos()) < tmp) {
					target = monster;
				}

			}
		}

		return target;
	}

    if (m->monsters.empty()) {
        return target;
    }

    PlayerSet::iterator it = m->monsters.begin();
    //target = *it;
	for ( ; it != m->monsters.end(); ++it) {
		Player* p = *it;
		//tmp,villager
		if (p->role_type == 11211) {
			continue;
		}
		if( p->team == summon->team){
			continue;
		}
		if( p->team == neutral_team_1){
			continue;
		}
		if( p->team == player_team_1){
			continue;
		}
        
        if (p->in_battle()) {
            if (target == NULL) {
                target = p;
                continue;
            }
            int tmp = summon->pos().squared_distance(p->pos());
            if ( summon->pos().squared_distance(target->pos()) > tmp) {
                target = p;
            }
        }
/*
		if (p->in_battle()) {
			int tmp = owner->pos().squared_distance(p->pos());
			TRACE_LOG("summonai:[%u %u]",tmp, dist);
			if (tmp < dist) {
				target = p;
				break;
			}
		}*/
	}
	
	return target;
}

ItemDrop *
MonsterAI::get_item_drop_target(Player * summon)
{
	return NULL;
}

uint32_t 
MonsterAI::get_near_target_cnt(const Player* player)
{
uint32_t mon_cnt = 0;
map_t*  m = player->cur_map;

//TODO:
for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) {
	Player* p = *it;
	/*if ( p!= player && (p->i_ai->is_in_state(MOVE_STATE) || p->i_ai->is_in_state(ATTACK_STATE)) ) {
		mon_cnt++;
	}*/
	if (p!= player && player->i_ai->judge_is_trace()) {
		mon_cnt++;
	}
}

return mon_cnt;
}

uint32_t 
MonsterAI::get_monster_cnt(const Player* player, uint32_t monster_id)
{
	uint32_t mon_cnt = 0;
	map_t*  m = player->cur_map;
	
	//TODO:
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it) {
		Player* p = *it;
		if (p->role_type == monster_id) {
			mon_cnt++;
		}
	}
	
	return mon_cnt;
}

void
MonsterAI::move_to_target_pos(Player* player, Vector3D& new_pos, const Vector3D& topos, uint32_t millisec)
{
	if (player->i_ai->get_one_pos_flag) {
		//adjust player dir
		if (topos.x() > player->pos().x()) {
			player->dir = dir_right;
		} else {
			player->dir = dir_left;
		}
		
		//send to map the monster target pos
		uint8_t move_type = player->i_ai->if_monster_run() ? 3 : 2;
		send_monster_move_noti(player, topos, move_type);
		
		player->i_ai->get_one_pos_flag = false;
		//TRACE_LOG("send move1 pack!");
	}
	
	
	const Vector3D& curpos = player->pos();

	//Vector3D pos;
	Vector3D s = player->velocity() * millisec / 1000;
	if ((::abs(curpos.x() - topos.x()) <= ::abs(s.x()) 
		 && ::abs(curpos.y() - topos.y()) <= ::abs(s.y())) || 
		curpos.squared_distance(topos) < s.squared_length()) {
		new_pos = topos;
	} else {
		new_pos = curpos + s;
		player->i_ai->move_dist_ += s.length();
	}

	//TRACE_LOG(" AI monster move: uid=%u tm=%d v=%s pos=%s %s", player->id, millisec,
	//			player->velocity().to_string().c_str(),	player->pos().to_string().c_str(), new_pos.to_string().c_str());

	player->adjust_position_ex(new_pos);

	//get next pos
	if (new_pos == topos && player->i_ai->target_path.size()) {
		get_one_target_pos(player);
	}
}

void
MonsterAI::get_one_target_pos(const Player* player)
{
	if (player->i_ai->target_path.size() == 0) {
		return;
	}
	
	KfAstar::Point& pt = player->i_ai->target_path.back();
	Vector3D newpos(pt.x, pt.y);
	player->i_ai->set_target_pos(newpos);
	
	player->i_ai->target_path.pop_back();
	
	
	player->i_ai->get_one_pos_flag = true;
}

void
MonsterAI::monster_stand(const Player* player)
{
	//TRACE_LOG("exit move state: uid=%u", player->id);

	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, player->pos().x(), idx);
	pack(pkgbuf, player->pos().y(), idx);
	pack(pkgbuf, player->dir, idx);

	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), player->id, cli_proto_stand, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	player->cur_map->send_to_map(pkgbuf, idx);
	
	//NOTI_LOG("stand: uid=%u %u, [%u %u %u]", player->id, player->i_ai->cur_state(), player->pos().x(), player->pos().y(), player->dir);
}

void
MonsterAI::monster_speak(const Player* player, uint32_t word_idx)
{
	//TRACE_LOG("exit move state: uid=%u", player->id);

	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, word_idx, idx);

	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), player->id, cli_proto_monster_speak, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	player->cur_map->send_to_map(pkgbuf, idx);
	
	//NOTI_LOG("stand: uid=%u %u, [%u %u %u]", player->id, player->i_ai->cur_state(), player->pos().x(), player->pos().y(), player->dir);
}


void
MonsterAI::send_monster_move_noti(const Player* player, const Vector3D& new_pos, uint8_t move_type)
{
	uint16_t* plag;
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, new_pos.x(), idx);
	pack(pkgbuf, new_pos.y(), idx);
	pack(pkgbuf, new_pos.z(), idx);
	/*pack(pkgbuf, player->i_ai->target_pos2.x(), idx);
	pack(pkgbuf, player->i_ai->target_pos2.y(), idx);
	pack(pkgbuf, player->i_ai->target_pos2.z(), idx);*/
	pack(pkgbuf, player->dir, idx);
	pack(pkgbuf, move_type, idx);
	//NOTI_LOG("stand: uid=%u , [%u %u %u %u %u %u]", player->id, new_pos.x(), new_pos.y(), player->i_ai->target_pos2.x(), player->i_ai->target_pos2.y(), player->dir);

	plag = reinterpret_cast<uint16_t*>(pkgbuf + idx);
	idx += sizeof(*plag);

	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), player->id, cli_proto_npc_move, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	
	

	for (PlayerSet::const_iterator it = player->cur_map->players.begin(); it != player->cur_map->players.end(); ++it) {
		Player* p = *it;
		*plag = bswap(static_cast<uint16_t>(player->lag + p->lag));

		send_to_player(p, pkgbuf, idx, 0);
		TRACE_LOG("TEST send move pack! %u %u", new_pos.x(), new_pos.y());
	}
	
	//NOTI_LOG("move: uid=%u %u, [%u %u %u]->[%u %u %u]", player->id, player->i_ai->cur_state(), player->pos().x(), player->pos().y(), player->dir,
	//					new_pos.x(), new_pos.y(), new_pos.z());
}

bool
MonsterAI::calc_target_pos(Player* player, uint32_t assign_skill_id)
{
	static int sign[10] = { -1, 1, 1, 1, -1, -1, 1, -1, -1, 1 };

	player->i_ai->move_dist_ = 0;
	const Player* target = player->i_ai->target;
	attack_region_t rgn; 
	//get monster attack region
	if (assign_skill_id == 0) {
		rgn.x = 50;
		rgn.y = 50;
	} else {
		rgn = player->get_attack_region(assign_skill_id);
	}
	if ((rgn.x == 0) && (rgn.y == 0)) {
		//player->state_machine->change_state(WaitState::instance());
		player->i_ai->change_state(WAIT_STATE);
		return false;
	}

	int lx = rgn.x - rgn.x / 2;
	rgn.x -= rgn.x / 4;
	rgn.y -= rgn.y / 4;

	int dir = player->id % 2 == 0 ? -1 : 1;
	if ( !(player->i_ai->find_path_flag_) ) {
		dir = sign[rand() % 10];
	}
	int x = target->pos().x() + ranged_random(lx, rgn.x) * dir;
	//int x = target->pos().x() + ranged_random(lx, rgn.x) * sign[rand() % 10];
	int y = target->pos().y() + ranged_random(0, rgn.y) * sign[rand() % 10];

	if (x < player->mon_info->len / 2) {
		x = player->mon_info->len / 2;
	}
	if (y < player->mon_info->height) {
		y = player->mon_info->height;
	}

	//const KfAstar::Points* pts = player->cur_map->path->findpath(KfAstar::Point(player->pos().x(), player->pos().y()),
	//					KfAstar::Point(x, y));
	
	 KfAstar::Points* pts = NULL;
	 KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
	 if(p_star)
	 {
		 pts = (KfAstar::Points*)p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()), KfAstar::Point(x, y));
	 }


	if (pts) {
		player->i_ai->target_path.assign(pts->begin(), pts->end());
		get_one_target_pos(player);
	/*	
        TRACE_LOG(" AI [%u] path ========== dump ==============", player->id); 
        KfAstar::Point& pt = pts->begin();
        for (; pt != pts->end(); ++pt) {
            TRACE_LOG(" AI pos [%u %u]", pt.x, pt.y);
        }*/
		player->i_ai->find_path_flag_ = true;
	} else {
		player->i_ai->find_path_flag_ = false;
		player->i_ai->change_state(WAIT_STATE);
		return false;
	}

				
	return true;
}

bool
MonsterAI::calc_long_atk_pos(Player* player)
{
	static int sign[10] = { -1, 1, 1, 1, -1, -1, 1, -1, -1, 1 };

	player->i_ai->move_dist_ = 0;
	const Player* target = player->i_ai->target;

	//get monster attack region
	attack_region_t rgn = player->get_attack_region();
	if ((rgn.x == 0) && (rgn.y == 0)) {
		return false;
	}

	int lx = rgn.x - rgn.x / 3;
	//rgn.x -= rgn.x / 4;
	rgn.y -= rgn.y / 4;

	int dir = player->id % 2 == 0 ? -1 : 1;
	if ( !(player->i_ai->find_path_flag_) ) {
		dir = sign[rand() % 10];
	}
	
	int x, y;
	int diff_x = target->pos().x() - player->pos().x();
	int diff_y = target->pos().y() - player->pos().y();
	if (abs(diff_y) > 75) {
		x = target->pos().x() + ranged_random(lx, rgn.x) * dir;
		//y = target->pos().y() + ranged_random(0, rgn.y) * sign[rand() % 10];	
		y = target->pos().y();	
	} else {
		if ( (dir == 1 && diff_x < 0) 
			|| (dir == -1 && diff_x > 0) ) {
			x = target->pos().x() + ranged_random(lx, rgn.x) * dir;
			y = target->pos().y() + ranged_random(0, rgn.y) * sign[rand() % 10];	
		} else {
			x = player->pos().x();
			y = player->pos().y() + ranged_random(75, 125) * dir;
		}
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
		
		player->i_ai->find_path_flag_ = true;
	} else {
		player->i_ai->find_path_flag_ = false;
		return false;
	}

	//TRACE_LOG("target: uid=%u to=%s v=%s", player->id, player->i_ai->target_pos.to_string().c_str(),
	//			player->velocity().to_string().c_str());
				
	return true;
}

bool
MonsterAI::calc_interval_move_pos(const Player* player)
{
	//long_range_attack monster
	if (player->mon_info->long_range_atk == 1) {
		return calc_target_pos(const_cast<Player*>(player));
	}
	
	static int sign[10] = { -1, 1, 1, 1, -1, -1, 1, -1, -1, 1 };

	player->i_ai->move_dist_ = 0;
	const Player* target = player->i_ai->target;

	int x, y;
	int abs_diff_x = abs(target->pos().x() - player->pos().x());
	int abs_diff_y = abs(target->pos().y() - player->pos().y());
	
	int dir = player->id % 2 == 0 ? -1 : 1;
	if ( !(player->i_ai->find_path_flag_) ) {
		dir = sign[rand() % 10];
	}
	
	int x_rng_start = 50;
	int x_rng_end = 125;
	if (player->i_ai->ai_type() == MIDDLE_LV_AI) {
		if ( abs_diff_x > abs_diff_y * 2 ){
			//x = target->pos().x() + ranged_random(0, 100) * sign[rand() % 10];
			x = target->pos().x() + ranged_random(x_rng_start, x_rng_end) * dir;
			y = player->pos().y();
		} else if ( abs_diff_y > abs_diff_x * 2 ) {
			x = player->pos().x();
			y = target->pos().y() + ranged_random(0, 100) * sign[rand() % 10];
		} else {
			//x = target->pos().x() + ranged_random(0, 100) * sign[rand() % 10];
			x = target->pos().x() + ranged_random(x_rng_start, x_rng_end) * dir;
			y = target->pos().y() + ranged_random(0, 100) * sign[rand() % 10];
		}
	} else {
		if ( abs_diff_x > abs_diff_y ){
			//x = target->pos().x() + ranged_random(0, 100) * sign[rand() % 10];
			x = target->pos().x() + ranged_random(x_rng_start, x_rng_end) * dir;
			y = player->pos().y();
		} else {
			x = player->pos().x();
			y = target->pos().y() + ranged_random(0, 100) * sign[rand() % 10];
		}
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
		player->i_ai->find_path_flag_ = true;
	} else {
		player->i_ai->find_path_flag_ = false;
		return false;
	}

	//TRACE_LOG("target: uid=%u to=%s v=%s", player->id, player->i_ai->target_pos.to_string().c_str(),
	//			player->velocity().to_string().c_str());
	
	return true;
}

bool
MonsterAI::calc_evade_pos(const Player* player)
{
	player->i_ai->move_dist_ = 0;
	const Player* target = player->i_ai->target;
	if (!target) {
		return false;
	}
	
	int evade_dist = player->mon_info->visual_field_sqrt + rand() % 100;
	int abs_diff_x = abs(target->pos().x() - player->pos().x());
	int abs_diff_y = abs(target->pos().y() - player->pos().y());
	//already evade
	if (abs_diff_x >= evade_dist || abs_diff_y >= evade_dist){
		return false;
	}
	
	//const KfAstar::Points* pts = player->cur_map->path->find_escape_path(KfAstar::Point(player->pos().x(), player->pos().y()), 
	//	                                                           KfAstar::Point(target->pos().x(), target->pos().y()), evade_dist);
	
	KfAstar::Points* pts = NULL;
	KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
	if(p_star)
	{
		pts = (KfAstar::Points*)p_star->find_escape_path(KfAstar::Point(player->pos().x(), player->pos().y()),  KfAstar::Point(target->pos().x(), target->pos().y()), evade_dist);
	}


	if (pts) {
		player->i_ai->target_path.assign(pts->begin(), pts->end());		
		get_one_target_pos(player);
	} else {
		return false;
	}

	
	return true;
}

bool
MonsterAI::calc_new_evade_pos(const Player* player)
{
	player->i_ai->move_dist_ = 0;
	const Player* target = player->i_ai->target;
	if (!target) {
		return false;
	}
	
	int evade_dist = player->mon_info->visual_field_sqrt + rand() % 100;
	int abs_diff_x = abs(target->pos().x() - player->pos().x());
	int abs_diff_y = abs(target->pos().y() - player->pos().y());
	//already evade
	if (abs_diff_x >= evade_dist || abs_diff_y >= evade_dist){
		return false;
	}
	
	int x = 0, y = 0;
	const KfAstar::Points* pts = 0;
	
	//x dirction, away
	x = player->pos().x() + (target->pos().x() > player->pos().x() ? -evade_dist : evade_dist);
	y = player->pos().y();
	if (x < player->mon_info->len / 2) {
		x = player->mon_info->len / 2;
	}
	pts =  player->cur_map->path->find_linear_path(KfAstar::Point(player->pos().x(), player->pos().y()),
																	KfAstar::Point(x, y));
	if (pts) {
		if ( abs((int)(target->pos().x() - (*pts)[0].x)) >= player->mon_info->visual_field_sqrt / 2 ) {
			goto ret;
		}
	}
	
	//x dirction, toward
	if (abs_diff_y >= player->mon_info->visual_field_sqrt / 3) {
		x = player->pos().x() + (target->pos().x() > player->pos().x() ? evade_dist : -evade_dist);
		y = player->pos().y();
		if (x < player->mon_info->len / 2) {
			x = player->mon_info->len / 2;
		}
		pts =  player->cur_map->path->find_linear_path(KfAstar::Point(player->pos().x(), player->pos().y()),
																		KfAstar::Point(x, y));
		if (pts) {
			if ( abs((int)(target->pos().x() - (*pts)[0].x)) >= player->mon_info->visual_field_sqrt / 2) {
				goto ret;
			}
		}
	}
	
	//y dirction, away
	x = player->pos().x();
	y = player->pos().y() + (target->pos().y() > player->pos().y() ? -evade_dist : evade_dist);
	if (y < player->mon_info->height) {
		y = player->mon_info->height / 2;
	}
	pts =  player->cur_map->path->find_linear_path(KfAstar::Point(player->pos().x(), player->pos().y()),
																	KfAstar::Point(x, y));
	if (pts) {
		if (abs((int)(target->pos().y() - (*pts)[0].y)) >= player->mon_info->visual_field_sqrt / 2) {
			goto ret;
		}
	}
	
	//y dirction, toward
	x = player->pos().x();
	y = player->pos().y() + (target->pos().y() > player->pos().y() ? evade_dist : -evade_dist);
	if (y < player->mon_info->height) {
		y = player->mon_info->height / 2;
	}
	pts =  player->cur_map->path->find_linear_path(KfAstar::Point(player->pos().x(), player->pos().y()),
																	KfAstar::Point(x, y));
	
	
	
ret:
	if (pts) {
		player->i_ai->target_path.assign(pts->begin(), pts->end());
		get_one_target_pos(player);
	} else {
		return false;
	}

	//TRACE_LOG("target: uid=%u to=%s v=%s", player->id, player->i_ai->target_pos.to_string().c_str(),
	//			player->velocity().to_string().c_str());
	
	return true;
}


bool
MonsterAI::calc_dodge_pos(const Player* player)
{
	player->i_ai->move_dist_ = 0;
	const Player* target = player->i_ai->target;
	if (!target) {
		////printf("no target!\n");
		return false;
	}
	
	int safe_dist = 100 + rand() % 50;
	int x, y;
	int abs_diff_x = abs(target->pos().x() - player->pos().x());
	int abs_diff_y = abs(target->pos().y() - player->pos().y());
	//already safe
	if (abs_diff_x >= safe_dist || abs_diff_y >= safe_dist){
		return false;
	}
	
	//get rand dir
	int rand_dir = rand() % 100;
	if (rand_dir > 25) {
		x = player->pos().x() + (target->pos().x() > player->pos().x() ? -safe_dist : safe_dist);
		y = player->pos().y();
	} else {
		x = player->pos().x();
		y = player->pos().y() + (target->pos().y() > player->pos().y() ? -safe_dist : safe_dist);
	}
	
	if (x < player->mon_info->len / 2) {
		x = player->mon_info->len / 2;
	}
	if (y < player->mon_info->height) {
		y = player->mon_info->height;
	}
	
	//find path
	player->i_ai->target_path.clear();
	//const KfAstar::Points* pt =  player->cur_map->path->find_linear_path(KfAstar::Point(player->pos().x(), player->pos().y()),KfAstar::Point(x, y));
	
	KfAstar::Points* pt = NULL;
	KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
	if(p_star)
	{
		pt = (KfAstar::Points*)p_star->find_linear_path(KfAstar::Point(player->pos().x(), player->pos().y()), KfAstar::Point(x, y));
	}
	
	if (pt) {
		player->i_ai->target_path.assign(pt->begin(), pt->end());
		get_one_target_pos(player);
	} else {
		return false;
	}

	return true;
}

bool
MonsterAI::calc_linger_pos(const Player* player)
{
	player->i_ai->move_dist_ = 0;
	int field = player->mon_info->visual_field_sqrt / 2;
	if (field <= 0) {
		return false;
	}
	int linger_dist = field + rand() % field;
	int dir = rand() % 4;
	
	int x = 0, y = 0;
	if (dir == 0) {//right
		x = player->pos().x() + linger_dist;
		y = player->pos().y();
	} else if (dir == 1) {//left
		x = player->pos().x() - linger_dist;
		y = player->pos().y();
	} else if (dir == 2) {//down
		x = player->pos().x();
		y = player->pos().y() + linger_dist / 2;
	} else if (dir == 3) {//up
		x = player->pos().x();
		y = player->pos().y() - linger_dist / 2;
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

	
	return true;
}

bool
MonsterAI::calc_return_pos(const Player* player)
{
	player->i_ai->move_dist_ = 0;
	int x = player->i_ai->init_pos.x();
	int y = player->i_ai->init_pos.y();
	
	if (player->pos().x() == x && player->pos().y() == y) {
		return false;
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
	return true;
}

void
MonsterAI::set_summon_atk_pos(Player* player)
{
	const Player* target = player->i_ai->target;
	const Player* owner = player->summon_info.owner;

	
	int dir = 0;
		
	if (owner->pos().x() > target->pos().x()) {
		dir = 1;
		player->dir = dir_left;
	} else {
		dir = -1;
		player->dir = dir_right;
	}

	int x = target->pos().x() + ranged_random(50, 100) * dir;
	int y = target->pos().y();

	if (x < player->mon_info->len / 2) {
		x = player->mon_info->len / 2;
	}
	if (y < player->mon_info->height) {
		y = player->mon_info->height;
	}
	
	TRACE_LOG("summonai:xy[%u %u]",x, y);
	Vector3D atk_pos(x, y);
	player->set_pos(atk_pos);

}

bool MonsterAI::monster_move_ex(Player* player, int millisec)
{
	Vector3D new_pos;
	Vector3D topos = player->i_ai->target_pos;
	
	move_to_target_pos(player, new_pos, topos, millisec);
	if (new_pos == topos && player->i_ai->target_path.size() == 0) {
		monster_stand(player);
		return true;
	}
	return false;
}

bool 
MonsterAI::monster_move(Player* player, int millisec)
{
	//move to target pos
	Vector3D new_pos;
	Vector3D& topos  = player->i_ai->target_pos;
	move_to_target_pos(player, new_pos, topos, millisec);

    //TRACE_LOG(" AI monster [%u] move pos from-[%u %u] to-[%u %u] [%u]", 
    //    player->id, new_pos.x(), new_pos.y(), topos.x(), topos.y(), player->i_ai->target_path.size());
	//move finish
	const Player* target = player->i_ai->target;
	if (new_pos == topos && player->i_ai->target_path.size() == 0) {
		//adjust player dir
		if ( target && (player->dir == dir_left) && (target->pos().x() > new_pos.x()) ) {
			player->dir = dir_right;
		} else if ( target && (player->dir == dir_right) && (target->pos().x() < new_pos.x()) ) {
			player->dir = dir_left;
		}
		
		monster_stand(player);
		player->i_ai->target_pos.init();
		return true;	
	}
	return false;
}

void 
MonsterAI::monster_attack(Player* player, player_skill_t* skill, uint32_t assign_x, uint32_t assign_y)
{
	//boss speak
	/*if (player->mon_info->type == 1 && skill->skill_id != 4029999) {
		monster_speak(player, 1);
	}*/
    
    //cut mp
    if (!player->can_use_skill(skill->skill_id, true)) {
        //send_header_to_player(player, player->waitcmd, cli_err_cool_time, 1);
        //player->notify_launch_skill_fail(skill->skill_id, skill->lv); 
        return ;
    }

	//
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, static_cast<uint32_t>(player->team), idx);
	pack(pkgbuf, player->role_type, idx);
	pack(pkgbuf, skill->skill_id, idx);
	pack(pkgbuf, skill->lv, idx);
	pack(pkgbuf, player->pos().x(), idx);
	pack(pkgbuf, player->pos().y(), idx);
	pack(pkgbuf, player->pos().z(), idx);
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), player->id, cli_proto_player_attack, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	player->cur_map->send_to_map(pkgbuf, idx);

	player->launch_skill(skill, assign_x, assign_y);
}

void 
MonsterAI::monster_attack_without_noti(Player* player, player_skill_t* skill, uint32_t assign_x, uint32_t assign_y)
{
	player->launch_skill(skill, assign_x, assign_y);
}

void 
MonsterAI::summon_attack(Player* player, player_skill_t* skill, uint32_t assign_x, uint32_t assign_y)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	//pack(pkgbuf, static_cast<uint32_t>(player->team), idx);
	//pack(pkgbuf, player->role_type, idx);
	uint32_t delay_tm = 500;
	pack(pkgbuf, skill->skill_id, idx);
	pack(pkgbuf, skill->lv, idx);
	pack(pkgbuf, player->pos().x(), idx);
	pack(pkgbuf, player->pos().y(), idx);
	pack(pkgbuf, player->dir, idx);
	pack(pkgbuf, delay_tm, idx);
	TRACE_LOG("summon_attack[%u %u %u %u %u]",skill->skill_id, skill->lv, player->pos().x(), player->pos().y(), player->dir);
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), player->id, cli_proto_summon_attack, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	player->cur_map->send_to_map(pkgbuf, idx);
	
	player->launch_skill(skill, assign_x, assign_y);
}

void 
MonsterAI::summon_pick_item(Player * player, uint32_t unqiue_id)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	//pack(pkgbuf, static_cast<uint32_t>(player->team), idx);
	//pack(pkgbuf, player->role_type, idx);
	uint32_t delay_tm = 500;
	uint32_t skill_id = 4020081;
	uint32_t lv = 1;

	Player * owner = player->summon_info.owner;
	if (!owner) {
		return;
	}
	pack(pkgbuf, skill_id, idx);
	pack(pkgbuf, lv, idx);
	pack(pkgbuf, player->pos().x(), idx);
	pack(pkgbuf, player->pos().y(), idx);
	pack(pkgbuf, player->dir, idx);
	pack(pkgbuf, delay_tm, idx);
	pack(pkgbuf, unqiue_id, idx);

	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), player->id, cli_proto_summon_pick_item, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_player(owner, pkgbuf, idx, 0);
	DEBUG_LOG("SUMMON PICK ITEM MSG!");
}

int 
MonsterAI::use_buf_skill(Player* player, uint32_t buf_type)
{
	player_skill_t* buff_skill = player->select_buff_skill(player->i_ai->target, buf_type);
	if (buff_skill) {
		TRACE_LOG("buff skill launch[%u]", player->id);
		monster_attack(player, buff_skill);
		player->i_ai->change_state(WAIT_STATE);
		return 1;
	}
	
	return 0;
}

	

void
MonsterAI::teleport(Player* player, uint32_t x, uint32_t y)
{
	Vector3D new_pos(x, y);
	player->adjust_position_ex(new_pos);
	send_monster_move_noti(player, new_pos, 4);
	
	player->i_ai->set_state_keep_tm(rand() % 1 + 1);
	//player->i_ai->change_state(WAIT_STATE);
}


//----------------------------------------------------------
// AIFactory class
//----------------------------------------------------------
MonsterAI*
AIFactory::select_ai(uint32_t ai_type, uint32_t mon_id)
{
	MonsterAI* ai;
	
	switch (ai_type) {
	case BOSS_AI:
		{
			switch (mon_id) {
				case 13097:
					ai = MoveBossAI::instance();
				break;
				
				case 13035:
					ai = PumpkinBossAI::instance();
					break;
				case 13036:
				case 13037:
					ai = BellCatAI::instance();
					break;
				case 13005:
					ai = ChifengAI::instance();
					break;
				case 13007:
					ai = JiXuanAI::instance();
					break;
				case 13008:
					ai = BaluAI::instance();
					break;
				case 13009: 
					ai = BitoresAI::instance();
					break;
				case 13010:
					ai = HaBoAI::instance();
					break;
				case 13011:
					ai = JiaCiAI::instance();
					break;
				case 13017:
					ai = ShaKeAI::instance();
					break;
				case 13031:
					ai = ChuanYinAI::instance();
					break;
				case 13039:
					ai = RabBro1AI::instance();
					break;
				case 13040:
					ai = RabBro2AI::instance();
					break;
				case 13041:
					ai = RabBro3AI::instance();
					break;
				case 13042:
					ai = RabbitsAI::instance();
					break;
				case 13050:
					ai = YouQiAI::instance();
					break;
				case 13052:
					ai = DaoLangAI::instance();
					break;
				case 13053:
					ai = MoyiAI::instance();
					break;
				case 13043:
					ai = HunfuAI::instance();
					break;
				case 13055:
					ai = lichi_toulingAI::instance();
					break;
				case 13054:
					ai = NullAI::instance();
					break;
				case 13028:
					ai = xili_AI::instance();
					break;
				case 13062:
					ai = huanying_AI::instance();
					break;
				case 13067:
					ai = CallCat_AI::instance();
					break;
				case 13069:
					ai = FumoRabbits_AI::instance();
					break;
				case 13070:
					ai = YaoYaoMoveAI::instance();
					break;
				case 13071:
					ai = Joe_Step1AI::instance();
					break;
				case 13072:
					ai = Joe_Step2AI::instance();
					break;
				case 13086:
					ai = Farui_AI::instance();
					break;
				case 13077:
					ai = Fish_AI::instance();
					break;
				case 13078:
					ai = Balrog_AI::instance();
					break;
				case 13016:
					ai = TuMars_AI::instance();
					break;
				case 13079:
					ai = Shark_AI::instance();
					break;
				case 13080:
					ai = Dmhunter_AI::instance();
					break;
				case 13081:
					ai = JinJi_AI::instance();
					break;
				case 13082:
					ai = GhostTeeth_AI::instance();
					break;
				case 13083:
					ai = CorpseBossAI::instance();
					break;
				case 13084:
					ai = WalnutAI::instance();
					break;
				case 13087: 
					ai = YuanSu_AI::instance();
					break;
				case 13089:
           			ai = ThreeWoodBossL_AI::instance();
					break;
				case 13090:
          			ai = ThreeWoodBossR_AI::instance();
					break;
				case 13091:
				case 13098:
				case 13102:
					ai = ConYiEr_AI::instance();
					break;
				case 13092:
				case 13099:
				case 13103:
	
					ai = ConAoTian_AI::instance();
					break;
				case 13093:
				case 13100:
				case 13104:
					ai = ConPP_AI::instance();
					break;
				case 13094:
				case 13101:	
				case 13105:
					ai = ConDaZhu_AI::instance();
					break;
                case 13095:
                case 13096:
                    ai = TaoTaiBoss_AI::instance();
                    break;
				case 13120:
					ai = FMYiEr_AI::instance();
					break;
				case 13121:
					ai = FMAoTian_AI::instance();
					break;
				case 13122:
					ai = FMPP_AI::instance();
					break;
				case 13123:
					ai = FMDaZhu_AI::instance();
					break;
				case 13026:
					ai = WoodsCold_AI::instance();
					break;
				case 13131:
					ai = YaoShi_AI::instance();
					break;
				case 13137:
					ai = EggBoss_AI::instance();
					break;
				case 13148:
					ai = IceDragon_AI::instance();
					break;
				case 13149:
					ai = ShadowDragon_AI::instance();
					break;
				case 13151:
					ai = DarkDragon_AI::instance();
					break;
				case 13139:
					ai = XueYao_AI::instance();
					break;
				case 11517:
					ai = NineHeadDragonAI::instance();
					break;
				case 13162:
					ai = DaguiAI::instance();
					break;
				default:
					ai = BossAI::instance();
					break;
			}
		}
		break;
	case LOW_LV_AI:
		ai = LowLvAI::instance();
		break;
	case MIDDLE_LV_AI:
		ai = MiddleLvAI::instance();
		break;
	case HIGH_LV_AI:
		ai = HighLvAI::instance();
		break;
			
	case LONG_RANGE_ATK_AI:
		ai = LongRangeAtkAI::instance();
		break;
		
	case SUMMON_MON_AI:
		if (mon_id == 1) {
//			ERROR_LOG("SUMMON CHANGE --->FURY AI");
			ai = SumFury_AI::instance();
		} else if (mon_id == 2) {
//			ERROR_LOG("SUMMON CHANGE --->WOOD AI");
			ai = SumWood_AI::instance();
		} else if (mon_id == 3) {
//			ERROR_LOG("SUMMON CHANGE --> SPEED AI");
			ai = SpeedSum_AI::instance();
		} else if (mon_id == 4) {
//			ERROR_LOG("SUMMON CHANGE --> SONE AI");
			ai = SOneSum_AI::instance();
		} else if (mon_id == 5) {
//			ERROR_LOG("SUMMON CHANGE --> STWO AI");
			ai = STwoSum_AI::instance();
		} else {
//			ERROR_LOG("SUMMON CHANGE --> DEFAULT AI");
			ai = SummonAI::instance();
		}  
		break;
			
	case GUARD_AI:
		ai = GuardAI::instance();
		break;
			
	case SPECIAL_MON_AI:

		switch (mon_id) {
			case 11362:
				ai = Box_943_AI::instance();
				break;

			case 11046: 
				ai = MoleAI::instance();
				break;
			case 11049: 
				ai = RedEyeMonkeyAI::instance();
				break;
			case 11085: 
			case 11087: 
				ai = LeopardAI::instance();
				break;
			case 11086:
				ai = JungleWolfAI::instance();
				break;
			case 11127:
			case 11129:
				ai = GorillaAI::instance();
				break;
			case 11128: 
			case 11130: 
				ai = NoShadowCatAI::instance();
				break;
			case 11211:
				ai = VillagerAI::instance();
				break;
			case 11212:
				ai = RavenAI::instance();
				break;
			case 11213: 
				ai = PigAI::instance();
				break;
			case 11247:
				ai = BoxWatcherAI1::instance();
				break;
			case 11248:
				ai = BoxWatcherAI2::instance();
				break;
			case 11300:
				ai = GearAI::instance();	
				break;
			case 11304:
			case 11303:
				ai = BombAI::instance();
				break;
			case 11302:
				ai = WormEggAI::instance();
				break;
			case 11305:
			case 11306:
			case 11307:
				ai = CorpseAI::instance();	
				break;
			case 11313:
				ai = JiGuan_AI::instance();
				break;
			case 11316:
				ai = ColorDoll_AI::instance();
				break;
			case 11317:
				ai = GoldDoll_AI::instance();
				break;

			case 11314:
				ai = GoodGuyRabbit::instance();
				break;
			case 11315:
				ai = BadGuyRabbit::instance();
				break;
			case 11318:
			case 11319:
            case 11341:
            case 11342:
           		ai = ThreeWood1_AI::instance();
				break;
			case 11320:
			case 11321:
            case 11343:
            case 11344:
          	 	ai = ThreeWood2_AI::instance();
				break;
			case 11324:
				ai = Ghost_AI::instance();
				break;
			case 11329:
				ai = Ghost_T_AI::instance();
				break;
			case 11325:
			case 11326:
			case 11327:
			case 11328:
				ai = Item_AI::instance();
				break;
			case 11337:
			case 11338:
			case 11339:
			case 11340:
           		ai = ThreeWoodItem_AI::instance();
				break;
			case 11348:
				ai = MoveCloseAttackAI::instance();
				break;
			case 11350:
			case 11351:
				ai = MoveBombAI::instance();
				break;
			case 11349:
				ai = MoveFarAttackAI::instance();
				break;

			case 11352:
			case 11353:
			case 11354:
			case 11355:
				ai = Tool_AI::instance();
				break;
			case 11414:
			case 11415:
			case 11416:
			case 11417:
			case 11418:
			case 11419:
			case 11420:
			case 11421:
			case 11422:
				ai = DarkCurse_AI::instance();
				break;
			case 11423:
				ai = RunStone_AI::instance();
				break;
			case 11424:
				ai = Worm_AI::instance();
				break;
			case 11433:
			case 11434:
				ai = DarkWizard_AI::instance();
				break;
			case 11474:
				ai = WuSeng_AI::instance();
				break;
			case 11475:
				ai = GuiseWuSeng_AI::instance();
				break;
			case 13132:
				ai = PreHistory_Wuseng::instance();
				break;
			case 11494:
				ai = GoldDoll_AI::instance();
				break;
			case 11509:
				ai = FireMon_AI::instance();
				break;
			case 11512:
			case 11515:
			case 11516:
				ai = NullAI::instance();
				break;
			case 13139:
				ai = XueYao_AI::instance();
				break;
			case 13140:
				ai = Fenshenkuilei_AI::instance();
				break;
			case 13142:
				ai = BossAI::instance();
				break;
			case 13143:
				ai = ShouTouTuo_AI::instance();
				break;
			case 11517:
				ai = NineHeadDragonAI::instance();
				break;
            case 11521:
                ai = TrackBombAI::instance();
                break;
			case 11523:
			case 11524:
			case 11525:
			case 11526:
			case 11527:
			case 11528:
			case 11529:
			case 11530:
			case 11531:
			case 11532:

			case 13165:
			case 13166:
			case 13167:
			case 13168:
			case 13169:
			case 13170:
			case 13171:
			case 13172:
			case 13173:
			case 13174:
                ai = LampDefenceAI::instance();
                break;
			default:
			 	ai = HighLvAI::instance();
				break;
		}
		break;
	default:
		ai = NullAI::instance();
	}
	
	return ai;
}

MonsterAI*
AIFactory::select_lua_ai(uint32_t ai_type)
{
	MonsterAI* ai = 0;
	
	switch (ai_type) {
//		case BOSS_AI:
//			ai = BossLuaAI::instance();
//			break;

		default:
			ai = NullAI::instance();
	}

	return ai;
}


