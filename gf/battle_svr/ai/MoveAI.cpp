/**
 *		*============================================================
 *   		@file      LowLvAI.hpp
 *      	@brief    all states of a player are defined here
 *       
 *      	compiler   gcc4.1.2
 *       	platform   Linux
 *                
 *        	copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *       *============================================================
**/


extern "C" 
{
#include <libtaomee/log.h>
}

#include <libtaomee++/random/random.hpp>
#include <libtaomee++/inet/byteswap.hpp>

#include "MonsterAI.hpp"
#include "LowLvAI.hpp"
#include "../player.hpp"
#include "../stage.hpp"
#include "../battle_impl.hpp"
#include "../player_status.hpp"
#include "HighLvAI.hpp"
#include "BossAI.hpp"
#include "MoveAI.hpp"
#include <libtaomee++/pathfinder/astar/astar.hpp>
using namespace taomee;

void MoveAI::get_next_point(Player* player, uint32_t& x,  uint32_t& y)
{
}


bool MoveAI::check_battle_finish( Player* player)
{
	return false;
}

bool MoveAI::check_change_ai(Player* player)
{
	Player* target = get_target(player);
	if( target && player->check_distance(target,  100) ){
		player->i_ai->change_ai(HighLvAI::instance(), HIGH_LV_AI);
		return true;
	}
	return false;
}

void MoveAI::wait(Player* player, int millisec)
{
	player->i_ai->change_state(MOVE_STATE);
	
	uint32_t x =0, y = 0;
	get_next_point(player, x, y);


	//const KfAstar::Points* pts = player->cur_map->path->findpath(KfAstar::Point(player->pos().x(), player->pos().y()),
	//		KfAstar::Point( x  , y ) );
	
	KfAstar::Points* pts = NULL;
	KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
	if(p_star)
	{
		pts = (KfAstar::Points*)p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()), KfAstar::Point( x  , y ));
	}

	if (pts) {
		player->i_ai->target_path.assign(pts->begin(), pts->end());
		get_one_target_pos(player);
	}
}

void MoveAI::linger(Player* player, int millisec)
{
}

void MoveAI::move(Player* player, int millisec)
{
	uint32_t x =0, y = 0;
	get_next_point(player, x, y);

	if( check_change_ai(player) ){
		return;
	}
	if( check_battle_finish(player) ){
		return ;
	}	

	monster_move_ex(player, millisec);
	if( player->pos().x() ==(int32_t)x && player->pos().y() ==(int32_t)y){
		player->i_ai->common_flag_ ++;
		player->i_ai->change_state(WAIT_STATE);
	}
}

void MoveAI::evade(Player* player, int millisec)
{

}

void MoveAI::attack(Player* player, int millisec)
{

}

void MoveAI::stuck(Player* player, int millisec)
{
	player->i_ai->change_state(MOVE_STATE);
}

void MoveAI::defense(Player* player, int millisec)
{

}




/*---------------------------------------------------------------------*/
void YaoYaoMoveAI::get_next_point(Player* player, uint32_t& x,  uint32_t& y)
{
	player->invincible_time = 1;
	int a[] = { 1124, 1291, 1478,  1069, 698, 750};
	int b[] = { 759, 695, 543,  370, 550, 640};

	player->i_ai->common_flag_ = player->i_ai->common_flag_ % (sizeof( a )/ sizeof(a[0]));
	x = a[player->i_ai->common_flag_];
	y = b[player->i_ai->common_flag_];
}

bool YaoYaoMoveAI::check_change_ai(Player* player)
{
	Player* target = get_target(player);
	if( target && player->check_distance(target,  100) ){
		player->i_ai->change_ai( YaoYaoBoss_AI::instance(), BOSS_AI);
		return true;
	}
	return false;
}


/*---------------------------------------------------------------------*/

void BoxWatcherAI1::get_next_point(Player* player, uint32_t& x,  uint32_t& y)
{
	int a[] = {1489, 1420, 1619};
	int b[] = {402,  532,  742};
	player->i_ai->common_flag_ = player->i_ai->common_flag_ % (sizeof( a )/ sizeof(a[0]));
	x = a[player->i_ai->common_flag_];
	y = b[player->i_ai->common_flag_];
}

/*---------------------------------------------------------------------*/

void BoxWatcherAI2::get_next_point(Player* player, uint32_t& x,  uint32_t& y)
{
	int a[] = { 837, 918, 577};
	int b[] = { 326, 438, 683};
	player->i_ai->common_flag_ = player->i_ai->common_flag_ % (sizeof( a )/ sizeof(a[0]));
	x = a[player->i_ai->common_flag_];
	y = b[player->i_ai->common_flag_];
}

/*---------------------------------------------------------------------*/

void CorpseAI::get_next_point(Player* player, uint32_t& x,  uint32_t& y)
{
	x = 110;
	y = player->pos().y();	
}

void CorpseAI::move(Player* player, int millisec)
{
	if( abs(player->pos().x() - 110)< 80 ) 
	{
			player->suicide();
			return ;	
	}
	MoveAI::move(player, millisec);
}

void CorpseAI::stuck(Player* player, int millisec)
{
	player->i_ai->change_state(MOVE_STATE);
}

bool CorpseAI::check_change_ai(Player* player)
{
	return false;
}
/*-----------------------------------------------------------------------*/
void CorpseBossAI::get_next_point(Player* player, uint32_t& x,  uint32_t& y)
{
	x = 110;
	y = player->pos().y();
}

void CorpseBossAI::move(Player* player, int millisec)
{
	if( abs(player->pos().x() - 110)< 200 )
	{
		player_skill_t* skill = player->select_skill(NULL, 4120094);
		if (skill) {
			monster_attack(player, skill);
		}
		return ;
	}
	MoveAI::move(player, millisec);
}

void CorpseBossAI::stuck(Player* player, int millisec)
{
	player->i_ai->change_state(MOVE_STATE);
}

bool CorpseBossAI::check_change_ai(Player* player)
{
	return false;
}


/*----------------------------------------------------------------------*/
void MoveCloseAttackAI::get_next_point(Player* player, uint32_t& x,  uint32_t& y)
{
	if(player->pos().y() == 206 && !check_wall1_exist(player))
	{
		x = 250;
		y = player->pos().y();
		return ;
	}
	if(player->pos().y() == 319 && !check_wall2_exist(player))
	{
		x = 250;
		y = player->pos().y();
		return;
	}
	if(player->pos().y() == 433 && !check_wall3_exist(player))
	{
		x = 250;
		y = player->pos().y();
		return;
	}
	x = 380;
	y = player->pos().y();
}

void MoveCloseAttackAI::move(Player* player, int millisec)
{
	if(player->pos().x() <= 250)
	{
		player->btl->on_btl_over(player, true);
		return;
	}
	
	if(player->pos().y() == 206 && !check_wall1_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);
	}
	if(player->pos().y() == 319 && !check_wall2_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);
	}
	if(player->pos().y() == 433 && !check_wall3_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);
	}
	
	if( abs(player->pos().x() - 110)< 300 )
	{
		player_skill_t* skill = player->select_skill(NULL, 4020002);
		if (skill) {
			monster_stand(player);
			monster_attack(player, skill);
		}
		return ;
	}
	MoveAI::move(player, millisec);
}

void MoveCloseAttackAI::stuck(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

bool MoveCloseAttackAI::check_change_ai(Player* player)
{
	return false;
}


bool MoveCloseAttackAI::check_wall1_exist(Player* player)
{
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it)
	{
		Player* p = *it;
		if(p->role_type == 11356 && p->pos().y() == 206)
		{
			return true;
		}	
	}
	return false;
}

bool MoveCloseAttackAI::check_wall2_exist(Player* player)
{
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it)
	{
		Player* p = *it;
		if(p->role_type == 11356 && p->pos().y() == 319)  
		{
			return true;
		}
	}
	return false;
}

bool MoveCloseAttackAI::check_wall3_exist(Player* player)
{
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->monsters.begin(); it != m->monsters.end(); ++it)
	{
		Player* p = *it;
		if(p->role_type == 11356 && p->pos().y() == 433)  
		{
			return true;
		}
	}
	return false;
}
/*---------------------------------------------------------------------------*/

void MoveBombAI::get_next_point(Player* player, uint32_t& x,  uint32_t& y)
{
	MoveCloseAttackAI::get_next_point(player, x, y);
}

void MoveBombAI::move(Player* player, int millisec)
{
	if(player->pos().x() <= 250)
	{
		player->btl->on_btl_over(player, true);
		return;
	}
	
	if(player->pos().y() == 206 && !check_wall1_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);	
	}
	if(player->pos().y() == 319 && !check_wall2_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);
	}
	if(player->pos().y() == 433 && !check_wall3_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);
	}
	
	
	if( abs(player->pos().x() - 110)< 300 )
    {
		player->suicide();
		return ;
	}
    MoveAI::move(player, millisec);
}


void MoveBombAI::stuck(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

bool MoveBombAI::check_change_ai(Player* player)
{
	return false;
}

/*--------------------------------------------------------------*/
void MoveBossAI::get_next_point(Player* player, uint32_t& x,  uint32_t& y)
{
	MoveCloseAttackAI::get_next_point(player, x, y);
}

void MoveBossAI::move(Player* player, int millisec)
{
	if(player->pos().x() <= 250)
	{
		player->btl->on_btl_over(player, true);
		return;
	}

	if(player->pos().y() == 206 && !check_wall1_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);
	}
	if(player->pos().y() == 319 && !check_wall2_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);
	}
	if(player->pos().y() == 433 && !check_wall3_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);
	}

	if( abs(player->pos().x() - 110)< 300 )
	{
		player_skill_t* skill = player->select_skill(NULL, 4020004);
		if (skill) {
			monster_attack(player, skill);
		}
		return ;
	}
	MoveAI::move(player, millisec);
}

void MoveBossAI::stuck(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

bool MoveBossAI::check_change_ai(Player* player)
{
	return false;
}

/*-----------------------------------------------------------*/

void MoveFarAttackAI::get_next_point(Player* player, uint32_t& x,  uint32_t& y)
{
	MoveCloseAttackAI::get_next_point(player, x, y);
}

void MoveFarAttackAI::move(Player* player, int millisec)
{
	if(player->pos().x() <= 250)
	{
		player->btl->on_btl_over(player, true);
		return;
	}

	if(player->pos().y() == 206 && !check_wall1_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);
	}
	if(player->pos().y() == 319 && !check_wall2_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);
	}
	if(player->pos().y() == 433 && !check_wall3_exist(player))
	{
		MoveAI::wait(player, millisec);
		return MoveAI::move(player, millisec);
	}

	player_skill_t* skill = player->select_skill(NULL, 4030045);
	if(skill){
		monster_stand(player);
		monster_attack(player, skill);
		player->i_ai->change_state(WAIT_STATE);
		return;		
	}
	MoveAI::move(player, millisec);
}

void MoveFarAttackAI::stuck(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}
		
bool MoveFarAttackAI::check_change_ai(Player* player)
{
	return false;
}


void RunStone_AI::get_next_point(Player* player, uint32_t& x,  uint32_t& y)
{

	if (player->pos().x() > 100) {
		x = player->pos().x() - 100;
		y = player->pos().y();

	} else {
		x = player->pos().x();
		y = player->pos().y();
	}

}

void RunStone_AI::move(Player* player, int millisec)
{
	map_t*  m = player->cur_map;
	for (PlayerSet::iterator it = m->players.begin(); it != m->players.end(); ++it)
	{
		Player* p = *it;
		if (p->pos().distance(player->pos()) < 100 && player->team != p->team) {
			player->suicide();
			return;
		}
	}

	uint32_t x = 0;
	uint32_t y = 0;

	this->get_next_point(player, x, y);

	KfAstar::Points* pts = NULL;
	KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
	if(p_star)
	{
		pts = (KfAstar::Points*)p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()), KfAstar::Point(x, y));
	}

	Vector3D pos = player->pos();

	if (pts){
		player->i_ai->target_path.assign(pts->begin(), pts->end());
		get_one_target_pos(player);
		monster_move_ex(player, millisec);
	}

	if (pos == player->pos()) {
		player->suicide();
	}


}

void RunStone_AI::wait(Player * player, int millisec)
{

	player->i_ai->change_state(MOVE_STATE);

}

void RunStone_AI::stuck(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

bool RunStone_AI::check_change_ai(Player* player)
{
	return false;
}

