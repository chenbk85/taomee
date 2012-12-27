/**
 *============================================================
 *  @file      SummonMonAI.cpp
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
#include "SummonMonAI.hpp"
#include "../player.hpp"
#include "../battle.hpp"
#include "../stage.hpp"

using namespace taomee;

/****************************************************************
 * summon SummonAI
 ****************************************************************/
void
SummonAI::wait(Player* player, int millisec)
{

//	ERROR_LOG("SUMMON AI WAIT %u", player->summon_info.owner->id);
	//wait some time
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
         return;
	}

    player->invincible = false;
    //clear pre target
    player->i_ai->target_pos.init();
    player->i_ai->target_path.clear();

	//if have unique skill
	if (player->summon_info.trigger_skill_id != 0) {
		launch_unique_skill(player, player->summon_info.trigger_skill_id);
		player->summon_info.trigger_skill_id = 0;
		return;
	}

	//add status buf
	if (this->extra_status_action(player)) {
		return;
	}	

	//use buf skill
	if (this->use_buf_skill(player)) {
		return;
	}

	if (this->extra_wait_action(player)) {
		return;
	}

	//find target
	player->i_ai->target = 0;
    player->i_ai->target = get_summon_target(player);

    const Player* owner = player->summon_info.owner;

	if ( (player->pos().distance(owner->pos()) < 450) && (player->i_ai->target != 0) ) {
        //if can attack 
        const Player* target = player->i_ai->target;
        if (player->select_skill(target)) {
            player->i_ai->change_state(ATTACK_STATE);
            return;
        }

        if (player->mon_info->spd) { 
            player->i_ai->change_state(MOVE_STATE);
            return;
        }

	} else if (player->pos().distance(owner->pos()) > 150)  {
        player->i_ai->target_pos = owner->pos();
        player->i_ai->change_state(LINGER_STATE);
    }
}

bool
SummonAI::extra_wait_action(Player * player)
{

//	ERROR_LOG("SUMMON AI EXTRA WAIT ACTION %u", player->summon_info.owner->id);
;
	if (pick_item(player)) {
		return true;
	}
	return false;
}

void 
SummonAI::linger(Player* player, int millisec)
{

//	ERROR_LOG("SUMMON AI LINGER %u", player->summon_info.owner->id);
	if(extra_status_action(player))
	{
		return;
	}

	if (pick_item(player)) {
		return;
	}

	int next_x = player->pos().x();
	int next_y = player->pos().y();

	const Player * owner = player->summon_info.owner;
	if (player->pos().distance(owner->pos()) > 600) {
		next_x = owner->pos().x() + rand() % 150 - rand() % 150;
		next_y = owner->pos().y();
		teleport(player, next_x, next_y); 
		player->i_ai->change_state(WAIT_STATE);
		return;
	}	

	if (player->pos().distance(owner->pos()) < 150) {
		if (owner->pos().x() > player->pos().x()) {
			player->dir = dir_right;
		} else {
			player->dir = dir_left;
		}	
		monster_stand(player);
		player->i_ai->change_state(WAIT_STATE);
		return;	
	}

	KfAstar::Points* pts = NULL;
	KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
	if(p_star)
	{
		pts = (KfAstar::Points*)p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()), KfAstar::Point(owner->pos().x(), owner->pos().y()));
	}

	if (pts){
		player->i_ai->target_path.assign(pts->begin(), pts->end());
		get_one_target_pos(player);
		monster_move_ex(player, millisec);				
	}
}

bool SummonAI::extra_status_action(Player * p)
{

//	ERROR_LOG("SUMMON AI EXTRA STATUS ACTION %u", p->summon_info.owner->id);
	uint32_t buff_default = 1101; //default
	uint32_t buff_fury = 1102; //fury;
	uint32_t buff_wood = 1103; //wood
	
	if (is_player_buff_exist(p, buff_fury)) {
		del_player_buff(p, buff_fury);
	}

	if (is_player_buff_exist(p, buff_wood)) {
		del_player_buff(p, buff_wood);
	}

	if (!is_player_buff_exist(p, buff_default)) {
		add_player_buff(p, buff_default, 0, 9000, 0);
		return true;
	}

	return false;
}

void
SummonAI::move(Player* player, int millisec)
{


//	ERROR_LOG("SUMMON AI MOVE %u", player->summon_info.owner->id);
	if(extra_status_action(player))
	{
		return;
	}
	
    const Player* target = player->i_ai->target;
	if (player->i_ai->target_pos.is_zero()) {
		if (!calc_target_pos(player, 0)) {
			player->i_ai->change_state(WAIT_STATE);
			return;
		}
	}
	//move to target pos
	if (monster_move(player, millisec)) {
		//if can attack
		if (player->select_skill(target)) {
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			//player->i_ai->set_state_keep_tm(rand() % 1 + 1);
			player->i_ai->change_state(WAIT_STATE);
		}
	} else {
		//TODO:attack when monster close to player
		if (player->i_ai->judge_update_tm(1000)) {
			//if can attack
			if (player->select_skill(target)) {
		//		monster_stand(player);
				player->i_ai->target_pos.init();
				player->i_ai->change_state(ATTACK_STATE);
			} else {
                player->i_ai->change_state(WAIT_STATE);
            }
		}
		
	}
}

void
SummonAI::evade(Player* player, int millisec)
{

//	ERROR_LOG("SUMMON AI EVADE %u", player->summon_info.owner->id);
	player->i_ai->change_state(WAIT_STATE);
}

void
SummonAI::attack(Player* player, int millisec)
{

//	ERROR_LOG("SUMMON AI ATTACK %u", player->summon_info.owner->id);
	if(extra_status_action(player))
	{
		return;
	}
	
//	NOTI_LOG(" AI attack state: uid=%u", player->id);
	
	//TODO:normal attack & skill attack
	if (player->i_ai->target == 0) {
		player->i_ai->change_state(WAIT_STATE);
		return;
	}
	
	//set_summon_atk_pos(player);
	player_skill_t* skill = player->select_skill(player->i_ai->target);
	if (skill) {
		//summon_attack(player, skill);
        monster_attack(player, skill);
	} 
	
	player->i_ai->change_state(WAIT_STATE);
}

void
SummonAI::stuck(Player* player, int millisec)
{

//	ERROR_LOG("SUMMON AI STUCK %u", player->summon_info.owner->id);
	player->i_ai->change_state(WAIT_STATE);
}

void 
SummonAI::defense(Player* player, int millisec)
{

//	ERROR_LOG("SUMMON AI DEFENSE %u", player->summon_info.owner->id);
	player->i_ai->change_state(WAIT_STATE);
}

void 
SummonAI::launch_unique_skill(Player* player, uint32_t skill_id)
{
	if (skill_id == 0) {
		return;
	}

	const Player* owner = player->summon_info.owner;
	Vector3D atk_pos(owner->pos().x(), owner->pos().y());
	player->set_pos(atk_pos);

	player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
	if (skill) {
		//summon_attack(player, skill);
		monster_attack(player, skill);
        player->i_ai->set_state_keep_tm(2);
        player->invincible = true;
	}

	player->i_ai->change_state(WAIT_STATE);
}

bool SummonAI::pick_item(Player * player)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (player->i_ai->item_flag && player->i_ai->common_flag1_ + 2 > cur_time) {
		Player* owner = player->summon_info.owner;
		uint32_t flag_ = player->i_ai->item_flag;
		ItemDropMap::iterator iter = player->cur_map->item_drops.find(flag_);
		if (iter != player->cur_map->item_drops.end() && iter->second.item_state() == can_pick 
				&& owner->pos().squared_distance(iter->second.pos()) < 500)
		{
			summon_pick_item(player, flag_);
			return true;
		}  
	} else {
		player->i_ai->item_flag = 0;
	}	

	if (player->select_skill(0, 4020081) && player->i_ai->common_flag1_ + 2 < cur_time) {
		if (this->get_item_drop_target(player)) {
			return true;
		}
	}
	return false;
}

ItemDrop *
SummonAI::get_item_drop_target(Player * player)
{ 
	Player* owner = player->summon_info.owner;
	if (!owner) {
		return 0;
	}

	uint32_t dist = player->mon_info->visual_field;
	ItemDropMap::iterator iter =  player->cur_map->item_drops.begin();
	uint32_t unqiue_id = 0;
	ItemDrop * item = 0;
	while (iter != player->cur_map->item_drops.end()) {
		if (iter->second.get_owner_id() && iter->second.get_owner_id() != owner->id) {
			++iter;
			continue;
		}
		uint32_t t_di = player->pos().squared_distance(iter->second.pos());
		if (t_di < dist) {
			dist = player->pos().squared_distance(iter->second.pos());
			unqiue_id = iter->first;
			item = &(iter->second);
		}
		++iter;
	}

	if (unqiue_id && item && (player->pos().distance(item->pos()) < 500)) {
		//int dir = 0;
		if (owner->pos().x() > item->pos().x()) {
		//	dir = 1;
			player->dir = dir_left;
		} else {
		//	dir = -1;
			player->dir = dir_right;
		}	

		uint32_t cur_time = get_now_tv()->tv_sec;

		player->i_ai->common_flag1_ = cur_time;
		player->i_ai->item_flag = unqiue_id;

		player->set_pos(item->pos());
		summon_pick_item(player, unqiue_id);
		return item;
	}

	player->i_ai->item_flag = 0;

	return item;
} 

bool SumFury_AI::extra_wait_action(Player * player)
{

//	ERROR_LOG("SUMMON AI FURY EXTRA WAIT ACTION %u", player->summon_info.owner->id);
	if (pick_item(player)) {
		return true;
	}

	//find target
	player->i_ai->target = 0;
    player->i_ai->target = get_summon_target(player);

	if (player->i_ai->target) {
		player_skill_t* skill = player->select_skill(player->i_ai->target);
		if (skill) {
			//summon_attack(player, skill);
   		    monster_attack(player, skill);
		 	player->i_ai->ready_skill_id_ = 0;
		} else {
			player->i_ai->change_state(MOVE_STATE);
		}	
     } else if (player->pos().distance(player->summon_info.owner->pos()) > 150) {
		 player->i_ai->change_state(LINGER_STATE);
	 }

	return true;
} 

bool SumFury_AI::extra_status_action(Player * p)
{

//	ERROR_LOG("SUMMON AI FURY EXTRA STATUS ACTION %u", p->summon_info.owner->id);
	uint32_t buff_default = 1101; //default
	uint32_t buff_fury = 1102; //fury;
	uint32_t buff_wood = 1103; //wood
	
	if (is_player_buff_exist(p, buff_default)) {
		del_player_buff(p, buff_default);
	}

	if (is_player_buff_exist(p, buff_wood)) {
		del_player_buff(p, buff_wood);
	}

	if (!is_player_buff_exist(p, buff_fury)) {
		add_player_buff(p, buff_fury, 0, 9000, 0);
		return true;
	}

	return false;

}

bool SumWood_AI::extra_wait_action(Player * player)
{

//	ERROR_LOG("SUMMON AI WOOD EXTRA WAIT ACTION %u", player->summon_info.owner->id);
	//find target
	player->i_ai->target = 0;
    player->i_ai->target = get_summon_target(player);

	if (player->i_ai->target) {
		player_skill_t* skill = player->select_skill(player->i_ai->target);
		if (skill) {
			//summon_attack(player, skill);
   		    monster_attack(player, skill);
		 	player->i_ai->ready_skill_id_ = 0;
		} 
     }

	return true;
} 

bool SumWood_AI::extra_status_action(Player * player)
{

//	ERROR_LOG("SUMMON AI WOOD EXTRA ACTION %u", player->summon_info.owner->id);
	uint32_t buff_default = 1101; //default
	uint32_t buff_fury = 1102; //fury;
	uint32_t buff_wood = 1103; //wood
	
	if (is_player_buff_exist(player, buff_fury)) {
		del_player_buff(player, buff_fury);
	}

	if (is_player_buff_exist(player, buff_default)) {
		del_player_buff(player, buff_default);
	}

	if (!is_player_buff_exist(player, buff_wood)) {
		add_player_buff(player, buff_wood, 0, 9000, 0);
		return true;
	}
	return false;
}

uint32_t get_summon_seq_skill(Player * p, uint32_t seq)
{
	const Player * owner = p->summon_info.owner;
	if (seq < owner->summon_info.skills_cnt - 1) {
		const summon_skill_info_t* p_skill = &(owner->summon_info.skills[seq]);
		return p_skill->skill_id;
	} else {
		return 0;
	}
}


KfAstar::Point get_summon_target_pos(Player * player)
{
	//p1
	if (player->summon_info.owner->pos().y() == 320) {
		if (player->i_ai->common_flag1_) {
			return KfAstar::Point(80, 320);
		} else {
			return KfAstar::Point(870, 320);
		}
	} else {	
		//p2
		if (player->i_ai->common_flag1_) {
			return KfAstar::Point(80, 370);
		} else {
			return KfAstar::Point(870, 370);
		}
	}	
}

bool SpeedSum_AI::extra_wait_action(Player * player)
{

//	ERROR_LOG("SUMMON AI SPEED EXTRA WAIT %u", player->summon_info.owner->id);
	uint32_t buff_speed = 1340;
	if (!is_player_buff_exist(player, buff_speed)) {
		add_player_buff(player, buff_speed, 0, 900);
		return true;
	}
	return false;
}

bool  judge_dragon_ship_btl_over(Player * player) {
	Battle * btl = player->btl;
	if (btl->pvp_lv() == pvp_dragon_ship && !btl->is_battle_ended()) {
		//failed 
		Player * player_1 = btl->get_one_player();

		if (player_1 && player_1->my_summon) {
			if (player_1->pos().x() + 10 >= player_1->my_summon->pos().x() &&
					player_1->my_summon->i_ai->common_flag1_) {
				end_pvp_on_player_dead(btl, player_1, player_1->team);
				return true;
			}
		}

		Player* player_2 =  btl->get_one_other_player(player_1);
		if (player_2 && player_2->my_summon) {
			if (player_2->pos().x() + 10 >= player_2->my_summon->pos().x() && 
					player_2->my_summon->i_ai->common_flag1_) {
				end_pvp_on_player_dead(btl, player_2, player_2->team);
				return true;
			}
		}
	}
	return false;
}


void SpeedSum_AI::wait(Player * player, int millisec)
{

//	ERROR_LOG("SUMMON AI SPEED  WAIT %u", player->summon_info.owner->id);
	if ( player->p_cur_skill || player->i_ai->judge_state_in_tm() ) {
         return;
	}

  //clear pre target
    player->i_ai->target_pos.init();
    player->i_ai->target_path.clear();

	if (this->extra_wait_action(player)) {
		return;
	}


	KfAstar::Points* pts = NULL;
	KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );

	if(p_star)
	{
		pts = (KfAstar::Points*)p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()),  get_summon_target_pos(player));
	}

	if (pts){
		player->i_ai->target_path.assign(pts->begin(), pts->end());
		get_one_target_pos(player);
		monster_move_ex(player, millisec);				
		if (judge_dragon_ship_btl_over(player)) {
			return;
		}	
	}

	if (player->pos().x() > 860 && player->i_ai->common_flag1_ == 0) {
		player->i_ai->common_flag1_ = 1;
	}
}

void SpeedSum_AI::move(Player * player, int millisec)
{

//	ERROR_LOG("SUMMON AI SPEED MOVE  WAIT %u", player->summon_info.owner->id);
	player->i_ai->change_state(WAIT_STATE);
}

void SpeedSum_AI::stuck(Player * player, int timediff)
{

//	ERROR_LOG("SUMMON AI SPEED STUCK  WAIT %u", player->summon_info.owner->id);
	player->i_ai->set_state_keep_tm(1);
	player->i_ai->change_state(WAIT_STATE);
}

bool  SOneSum_AI::extra_wait_action(Player * player)
{
//	ERROR_LOG("SUMMON AI STONE EXTRA WAIT %u", player->summon_info.owner->id);
	uint32_t buff_speed = 1340;
	if (is_player_buff_exist(player, buff_speed)) {
		del_player_buff(player, buff_speed);
		return true;
	}

	uint32_t skill_id = get_summon_seq_skill(player, 1);
	if (skill_id) {
		player->i_ai->target = 0;
		player->i_ai->target = get_summon_target(player);

		if (player->i_ai->target) {
			//if can attack 
			player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
			if (skill == NULL) {
				skill = player->select_buff_skill(player->i_ai->target, skill_id);
			} 
			if (skill && (skill->skill_info->is_atk_skill() || skill->skill_info->is_buff_skill())) {
				monster_attack(player, skill);
				return true;
			}
		}
	}
	return false;
}

bool STwoSum_AI::extra_wait_action(Player * player)
{
//	ERROR_LOG("SUMMON AI STWO EXTRA WAIT %u", player->summon_info.owner->id);
	uint32_t buff_speed = 1340;
	if (is_player_buff_exist(player, buff_speed)) {
		del_player_buff(player, buff_speed);
	}

	uint32_t skill_id = get_summon_seq_skill(player, 2);
	if (skill_id) {
		player->i_ai->target = 0;
		player->i_ai->target = get_summon_target(player);
		//if can attack 
		if (player->i_ai->target) {
			player_skill_t* skill = player->select_skill(player->i_ai->target, skill_id);
			if (skill == NULL) {
				skill = player->select_buff_skill(player->i_ai->target, skill_id);
			} 

			if (skill && (skill->skill_info->is_atk_skill() || skill->skill_info->is_buff_skill())) {
				monster_attack(player, skill);
				return true;
			}
		}
	}
	return false;
}




