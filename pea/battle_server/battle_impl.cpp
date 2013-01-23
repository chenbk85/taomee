#include <cstdio>

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <libtaomee++/random/random.hpp>
#include <math.h>

extern "C" 
{
#include <glib.h>
#include <assert.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee++/random/random.hpp>
}


#include "battle_round.hpp"
#include "battle_impl.hpp"
#include "battle_manager.hpp"
#include "player.hpp"
#include "cli_proto.hpp"
#include "map_data.hpp"
#include "attack_obj.hpp"
#include "player_attribute.hpp"


struct player_turn_value_compare
{
	bool operator()(Player* a, Player* b)
	{
		return a->get_turn_value() < b->get_turn_value();
	}	
};

Battle::Battle(Player* creater, uint32_t battle_mode, uint32_t round_time, uint32_t stage_id, uint32_t player_count)
{
	btl_id = battle_mgr::get_instance()->alloc_battle_id();
	btl_mode = battle_mode;
	btl_state = battle_no_state;
	set_creater(creater);
	btl_map_data = NULL;
	btl_stage_data = NULL;
	btl_wait_load_time = 0;
	cur_turn_player = NULL;
	last_turn_player = NULL;
	max_player_count = player_count;
	cur_round_num = 0;
	init_phy_container();
	init_attack_obj_list();
	init_btl_players();
	init_battle_statistics_data();
	if(stage_id != 0)
	{
		init_btl_stage_data(stage_id);
		init_btl_map_data(stage_id);
	}
}

Battle::~Battle()
{
	battle_mgr::get_instance()->release_battle_id(btl_id);
	btl_id = 0;
	btl_mode = 0;
	btl_state = battle_no_state;
	//btl_round_time = 0;
	btl_wait_load_time = 0;
	set_creater(NULL);
	cur_turn_player = NULL;
	last_turn_player = NULL;
	max_player_count = 0;
	cur_round_num = 0;
	final_btl_map_data();
	final_phy_container();
	final_attack_obj_list();
	final_btl_players();
	final_battle_statistics_data();
	final_btl_stage_data();
}

stage_data*  Battle::get_stage_data()
{
	return btl_stage_data;	
}

bool Battle::init_btl_stage_data( uint32_t battle_map_id)
{
	btl_stage_data = stage_data_mgr::get_instance()->get_stage_data_by_id(battle_map_id);
	if(btl_stage_data == NULL && battle_map_id != 0)
	{
		ERROR_TLOG("invalid stage_map_id = %u", battle_map_id);	
	}
	return true;
}

bool Battle::final_btl_stage_data()
{
	btl_stage_data = NULL;
	return true;
}


bool Battle::init_phy_container()
{
	container.set_acceleration(GRAVITY_ACCELERATION, 0.0);	
	return true;
}
    
bool Battle::final_phy_container()
{
	container.clear_phy_models();
	return true;
}

bool Battle::add_phy_model_obj(phy_model* p_model)
{
	return container.add_phy_model(p_model);
}

bool Battle::del_phy_model_obj(phy_model* p_model)
{
	return container.del_phy_model(p_model);
}

void Battle::phy_container_routing(struct timeval cur_time)
{
	uint32_t millisecond = 0;
	if(container.to_next_time(cur_time, millisecond))
	{
		container.simulate(millisecond);
		attack_obj_routing(cur_time);
	}
}

bool Battle::init_btl_map_data( uint32_t battle_map_id)
{
	map_data* p_map = map_data_mgr::getInstance()->get_map_data(battle_map_id);
	if(p_map == NULL && battle_map_id != 0)
	{
		ERROR_TLOG("init btl map data error map_id = %u",battle_map_id);
		return false;
	}
	btl_map_data = new map_data(*p_map);
	return true;
}

bool Battle::final_btl_map_data()
{
	if(btl_map_data)
	{
		delete btl_map_data;
		btl_map_data = NULL;
	}
	return true;
}

bool Battle::random_init_btl_stage_map()
{
	int32_t map_id = map_data_mgr::getInstance()->gen_random_map_id();	
	if(map_id == -1)
	{
		ERROR_TLOG("gen random_map_id error map_id = -1");
		return false;
	}
	init_btl_stage_data(map_id);
	init_btl_map_data(map_id);
	return true;
}

map_data* Battle::get_btl_map_data()
{
	return btl_map_data;	
}

uint32_t Battle::get_btl_map_id()
{
	if(btl_map_data == NULL)return 0;
	return btl_map_data->map_id;
}

bool Battle::init_attack_obj_list()
{
	attack_obj_list.clear();	
	return true;	
}

bool Battle::final_attack_obj_list()
{
	std::list<attack_obj*>::iterator pItr = attack_obj_list.begin();
	for(; pItr != attack_obj_list.end(); ++pItr)
	{
		attack_obj* obj = *pItr;
		destroy_attack_obj(obj);
		obj = NULL;
	}
	attack_obj_list.clear();
	
	return true;
}

bool Battle::del_attack_obj_by_owner_id(uint32_t owner_id)
{
	std::list<attack_obj*>::iterator pItr = attack_obj_list.begin();
	while(pItr != attack_obj_list.end())
	{
		attack_obj* obj = *pItr;
		if(obj->owner->id == owner_id)
		{
			pItr = attack_obj_list.erase(pItr);
			destroy_attack_obj(obj);
			obj = NULL;
		}
		else
		{
			++pItr;	
		}
	}
	return true;
}

bool Battle::add_attack_obj(attack_obj* obj)
{
	attack_obj_list.push_back(obj);
	return true;	 
}

bool Battle::del_attack_obj(attack_obj* obj)
{
	std::list<attack_obj*>::iterator pItr = std::find( attack_obj_list.begin(),  attack_obj_list.end(), obj);
	if(pItr != attack_obj_list.end())
	{
		attack_obj_list.erase(pItr);	
	}
	return true;
}

void Battle::attack_obj_routing( struct timeval cur_time)
{
	/*
	std::list<attack_obj*>::iterator pItr = attack_obj_list.begin();
	while(pItr != attack_obj_list.end())
	{
		attack_obj* p_obj = *pItr;
		if(p_obj->is_attack_obj_finish( cur_time))
		{
			pItr = attack_obj_list.erase(pItr);
			if(p_obj->get_hit_result())
			{
				p_obj->owner->inc_total_hit_count();
			}
			destroy_attack_obj(p_obj);
			p_obj = NULL;
			ERROR_TLOG("delete attack_obj_list----------------------");
			continue;
		}
		else if( p_obj->is_need_init_first_bullets(cur_time) )
		{
			p_obj->init_attack_obj(); 			
			ERROR_TLOG("first_bullets-----------------------");
			++pItr;
			continue;
		}
		else if( p_obj->is_need_init_next_bullets(cur_time))
		{
			p_obj->init_attack_obj();
			ERROR_TLOG("next_bullets-----------------------");
			++pItr;
			continue;
		}
		else if(p_obj->is_need_init_after_timer())
		{
			ERROR_TLOG("init after timer--------------------");
			p_obj->init_after_attack_timer( cur_time );
			++pItr;
			continue;
		}
		else if(p_obj->is_need_init_next_timer())
		{
			ERROR_TLOG("init next timer-------------------");
			p_obj->init_next_bullet_timer( cur_time );
			++pItr;
			continue;
		}
		else if( p_obj->is_attack_obj_fly(cur_time))
		{
			ERROR_TLOG("attack_obj_timer----------------");
			attack_obj_timer(p_obj, cur_time);
			++pItr;
			continue;
		}
		++pItr;
		*/
		/*
		if(p_obj->is_attack_obj_finish( cur_time) )//
		{
			pItr = attack_obj_list.erase(pItr);
			if(p_obj->get_hit_result())
			{
				p_obj->owner->inc_total_hit_count();
			}
			destroy_attack_obj(p_obj);
			p_obj = NULL;
			ERROR_TLOG("delete attack_obj_list----------------------");
			continue;
		}
		else if( p_obj->is_next_bullets(cur_time))
		{
			p_obj->init_attack_obj();
			ERROR_TLOG("next_bullets-----------------------");
			++pItr;
			continue;
		}
		else if(p_obj->is_bullets_empty() && (!p_obj->check_after_attack_timer()) && p_obj->remain_bullet_times == 0 )
		{
			ERROR_TLOG("init after timer--------------------");
			p_obj->init_after_attack_timer( cur_time );
			++pItr;
			continue;
		}
		else if(p_obj->is_bullets_empty() && (!p_obj->check_next_bullet_timer()) && p_obj->remain_bullet_times == 1 )
		{
			ERROR_TLOG("init next timer-------------------");
			p_obj->init_next_bullet_timer( cur_time );
			++pItr;
			continue;
		}
		else if ( !p_obj->is_bullets_empty() && p_obj->is_attack_obj_fly(cur_time))
		{
			ERROR_TLOG("attack_obj_timer----------------");
			attack_obj_timer(p_obj, cur_time);
			++pItr;
			continue;
		}
		*/
	//}
}

void Battle::player_routing(struct timeval cur_time)
{
	std::list<Player*>::iterator pItr = playerlist.begin();
	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;
		player_timer(p, cur_time);
	}
}

bool Battle::init_btl_players()
{
	return true;
}

bool Battle::final_btl_players()
{
	creater = NULL;
	max_player_count = 0;
	std::list<Player*>::iterator pItr = playerlist.begin();
	for(; pItr != playerlist.end(); ++pItr)
	{	
		Player* p = *pItr;
		if( get_cur_turn_player() ==  p)  
		{   
			set_cur_turn_player(NULL);  
		} 
		if( get_last_turn_player() == p)
		{
			set_last_turn_player(NULL);
		}
		p->dettach_battle();
		::del_player(p);
		p = NULL;
	}
	playerlist.clear();
	btl_turn_queue.clear();
	return true;
}

bool Battle::check_creater(Player* p)
{
	return creater == p;
}

bool    Battle::add_player(Player* p)
{
	if(is_player_exist(p->id))return false;
	playerlist.push_back(p);
	p->attach_battle(this);
	return true;
}

bool    Battle::player_fall_down(Player* p)
{
	
	int32_t begin_x = p->get_x() - p->get_width()/2;		
	int32_t end_x = begin_x + p->get_width();
	bool fall_down = false;

	while(1)
	{
		int32_t cur_y = p->get_y() + 1;
		if( check_map_data_fall_down(btl_map_data, begin_x, end_x, cur_y) )
		{
			p->set_y(cur_y);
			fall_down = true;
			ERROR_TLOG("player fall down p = %u, x = %u, y = %d----------", p->id, (uint32_t)p->get_x(), cur_y);		
			if(out_of_area(btl_map_data, p))
			{	
				ERROR_TLOG("player fall down p = %u, out of area", p->id);
				p->set_dead(true);
				del_player_from_queue(p->id);
				return true;
			}
			continue;
		}
		break;
	}
	return fall_down;
}

void Battle::all_fall_down(uint32_t bullet_syn_number, uint32_t bullet_times)
{
	std::list<Player*>::iterator pItr = playerlist.begin();
	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;
		if( player_fall_down(p) )
		{
			notify_all_fall_down_delay(p, bullet_syn_number, bullet_times);
			YJ_DEBUG_LOG("player fall down result p = %u, x = %u, y= %u", p->id, (uint32_t)p->get_x(), (uint32_t)p->get_y());
			if(p->is_dead())
			{
				p->btl->notify_all_player_dead_delay(p, bullet_syn_number, bullet_times);	
			}
		}
	}
}

bool Battle::check_move_able(Player* p, int32_t new_x, int32_t new_y)
{
	int32_t x = p->get_x();
	int32_t y = p->get_y();

	if(x == new_x && y == new_y )return true;

	if(x == new_x && y < new_y )return false;

	if( x != new_x && y > new_y)
	{
		double user_tan = abs(new_y - y) / abs(new_x -x);
		double limit_tan = tan(angle2radian(60));
		return limit_tan >= user_tan;
	}
	return true;
}

bool    Battle::del_player(uint32_t player_id)
{
	std::list<Player*>::iterator pItr = playerlist.begin(); 
	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;
		if(p->id == player_id )
		{
			del_attack_obj_by_owner_id(p->id);
			del_player_from_queue(p->id);
			if( get_cur_turn_player() ==  p)
			{
				set_cur_turn_player(NULL);	
			}
			p->dettach_battle();
			max_player_count --;
			pItr = playerlist.erase(pItr);	
			::del_player(p);
			p = NULL;
			return true;
		}
	}
	return false;
}

bool    Battle::is_player_exist(uint32_t player_id)
{
	std::list<Player*>::iterator pItr = playerlist.begin();
	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;	
		if(p->id == player_id)return true;
	}
	return false;
}

Player* Battle::get_player_by_id(uint32_t player_id)
{
	std::list<Player*>::iterator pItr = playerlist.begin(); 
	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;
		if(p->id == player_id)return p;
	}
	return NULL;
}

void    Battle::set_creater(Player* p)
{
	creater = p;	
}

Player* Battle::get_creater()
{
	return creater;
}

uint32_t  Battle::get_cur_players_count()
{
	return playerlist.size();	
}

void Battle::init_btl_begin_time()
{
	struct timeval  cur_time = *get_now_tv();
	btl_begin_time = cur_time;
	container.init_base_trigger_timer(cur_time, PHY_MODEL_SIMULATE_TIME);
}

void Battle::init_round_delay_timer(struct timeval cur_time, uint32_t delay_time)
{
	round_delay_timer.init_base_duration_timer(cur_time, delay_time);	
}

bool Battle::check_round_delay_not_finish( struct timeval cur_time)
{
	return 	round_delay_timer.check_timer_flag() && !round_delay_timer.is_timer_finish(cur_time);
}

void Battle::goto_next_round(struct timeval cur_time, uint32_t delay_time)
{
	set_cur_turn_player(NULL);
	init_round_delay_timer(cur_time, delay_time);
	set_all_player_round_state(battle_round_start);
}

void Battle::inc_round_num()
{
	cur_round_num++;
}

void Battle::set_all_player_round_state(uint32_t state)
{
	std::list<Player*>::iterator pItr = playerlist.begin();
	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;
		p->set_btl_round_state(state);
	}
}

bool Battle::check_all_player_round_state(uint32_t state)
{
	std::list<Player*>::iterator pItr = playerlist.begin();
	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;
		if( p->get_btl_round_state() != state)
		{
			return false;	
		}
	}
	return true;
}

void Battle::on_battle_timer( struct timeval cur_time )
{
	Player* turn = NULL;
	if(get_btl_state() == battle_running_state)
	{
		player_routing(cur_time);
	}
	
	if (check_round_delay_not_finish(cur_time))
	{
		return;		
	}

	if( get_btl_state() == battle_end_state)
	{
		ERROR_TLOG("btl end change to destroy and notify all");
		change_btl_state(battle_destroy_state);
		calc_battle_statistics_data();
		notify_all_battle_end();
		return;
	}


	turn = get_cur_turn_player();
	//战斗回合
	if(turn == NULL)
	{
		//每次出手都有50%几率触发连击
		turn = calc_player_turn_again();
		if(turn)
		{
			set_cur_turn_player(turn);
			set_last_turn_player(turn);
			turn->ready_attack_timer.init_base_duration_timer(cur_time, BATTLE_READY_ATTACK_TIME);
			notify_all_player_turn_again(turn);
		}
		else//否则按照正常出手顺序
		{
			turn = calc_player_turn();
			set_cur_turn_player(turn);
			set_last_turn_player(turn);
			if(turn)
			{
                //因为里面时间单位是回合
                //所以玩家每次出手前触发各类定时器
				turn->process_timer(cur_time, player_action_step_before_atk);
				turn->ready_attack_timer.init_base_duration_timer(cur_time, BATTLE_READY_ATTACK_TIME);
				notify_all_player_turn(turn);
			
			}
		}
	}
	else 
	{
		if(turn->ready_attack_timer.check_timer_flag() && turn->ready_attack_timer.is_timer_finish(cur_time))
		{
			//如果准备攻击计时器超时，则轮到下个人出手
			goto_next_round(cur_time, BATTLE_ROUND_DELAY_TIME);
		}
		else if(turn->attack_timer.check_timer_flag() && turn->attack_timer.is_timer_finish(cur_time))
		{
			//如果攻击调整计时器超时，则轮到下个人出手
			goto_next_round(cur_time, BATTLE_ROUND_DELAY_TIME);
		}
		else if( !turn->attack_timer.check_timer_flag() && !turn->ready_attack_timer.check_timer_flag())
		{
			goto_next_round(cur_time, BATTLE_ROUND_DELAY_TIME);
		}
		
	}
}


bool	Battle::check_player_turn(Player* p)
{
	if(p == NULL)return false;
	return p == cur_turn_player;
}

bool    Battle::del_player_from_queue(uint32_t uid)
{
	list<Player*>::iterator pItr = btl_turn_queue.begin();	
	for(; pItr != btl_turn_queue.end(); ++pItr)
	{
		Player* p = *pItr;
		if(p->id == uid)
		{
			btl_turn_queue.erase(pItr);
			return true;
		}
	}
	return false;
}

Player* Battle::calc_player_turn_again()
{
	if(last_turn_player == NULL)return NULL;
	int value = ranged_random(1, 100);
	if(value >= 200)
	{
		return last_turn_player;	
	}
	return NULL;
}


Player* Battle::calc_player_turn()
{
	Player* p = NULL;
	if(btl_turn_queue.empty())
	{
		std::list<Player*>::iterator pItr = playerlist.begin();
		for(; pItr != playerlist.end(); ++pItr)
		{
			Player* p = *pItr;
			if(p->is_dead())continue;
			btl_turn_queue.push_back(p);
		}
		
		
		btl_turn_queue.sort(player_turn_value_compare());
		inc_round_num();
	}

	if(btl_turn_queue.empty())return NULL;
	
	p = btl_turn_queue.front();	
	btl_turn_queue.pop_front();	
	return p;
}

void Battle::set_last_turn_player(Player* p)
{
	last_turn_player = p;
}

Player* Battle::get_last_turn_player()
{
	return last_turn_player;
}

void Battle::set_cur_turn_player(Player* p)
{
	cur_turn_player = p;
}

Player* Battle::get_cur_turn_player()
{
	return cur_turn_player;	
}

bool Battle::check_syn_players()
{
	if( max_player_count == playerlist.size() && max_player_count != 0)
	{
		return true;
	}
	return false;
}

bool Battle::check_load_resource_players()
{
	std::list<Player*>::iterator pItr = playerlist.begin();
	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;
		if( !p->check_btl_player_state(btl_player_load_ok_state) )
		{
			return false;
		}
	}
	return true;
}


void Battle::change_all_btl_player_state(uint32_t state)
{
	std::list<Player*>::iterator pItr = playerlist.begin();
	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;
		p->change_btl_player_state(state);
	}	
}


bool Battle::check_battle_end()
{
	//改函数会被子类的函数覆盖
	return true;	
}

bool Battle::statistics_player_dead(Player* atker, Player* dead)
{
	//改函数会被子类的函数覆盖
	return true;	
}

bool Battle::statistics_player_damage(Player* atker, Player* dead, uint32_t damage)
{
	//改函数会被子类的函数覆盖
	return true;	
}

bool Battle::calc_battle_statistics_data()
{
	//改函数会被子类的函数覆盖
	return true;	
}

bool Battle::init_battle_statistics_data()
{
	win_team = 0;
	return true;	
}

bool Battle::final_battle_statistics_data()
{
	win_team = 0;
	return true;	
}


/////////////////////////////////////////////////////////



void Battle::broadcast_to_all(void* pkg, uint32_t len, Player* clear_waitcmd)
{
	std::list<Player*>::iterator pItr = playerlist.begin();
	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = (*pItr);
		
		if(clear_waitcmd != NULL && p == clear_waitcmd )
		{
			send_to_player(p, pkg, len, 1);	
		}
		else
		{
			send_to_player(p, pkg, len, 0);
		}	
	}
}

void Battle::notify_all_load_resouce()
{
	int idx =sizeof(btl_proto_t) + sizeof(cli_proto_t);	
	pack(pkgbuf, btl_id, idx);
	pack(pkgbuf, get_btl_map_id(), idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_load_resource_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);
}

void Battle::notify_all_battle_start()
{
	int idx =sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, btl_id, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_battle_start_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);
}

void Battle::notify_all_player_move(Player* p)
{
	int idx =sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, (uint32_t)p->get_x(), idx);
	pack(pkgbuf, (uint32_t)p->get_y(), idx);
	pack(pkgbuf, p->get_dir(), idx);
	pack(pkgbuf, p->get_angle(), idx);
	pack(pkgbuf, p->get_gun_angle(), idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_btl_player_move_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx, p);
}

void Battle::notify_all_player_turn(Player* p)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, p->btl->cur_round_num, idx);
	pack(pkgbuf, (uint32_t)(0), idx);
	pack(pkgbuf, (uint32_t)(0), idx);

	ERROR_TLOG("btl_id = %u round_num = %u,  turn id = %u", btl_id, p->btl->cur_round_num,  p->id);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_player_turn_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx, p);
}


void Battle::notify_all_player_turn_again(Player* p)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, p->id, idx);

	ERROR_TLOG("btl_id = %u turn again id = %u", btl_id, p->id);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_player_turn_again_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx, p);

}


void Battle::notify_all_player_attack(Player* p, uint32_t skill_id, uint32_t skill_lv, uint32_t gun_angle, uint32_t muzzle_velocity)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, skill_id, idx);
	pack(pkgbuf, skill_lv, idx);
	pack(pkgbuf, gun_angle, idx);
	pack(pkgbuf, muzzle_velocity, idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_player_attack_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx, p);
}

void Battle::notify_all_player_ready_attack(Player* p)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, p->id, idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_player_ready_attack_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx, p);
}

void Battle::notify_all_player_dead(Player* p)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_player_dead_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);	
}

void Battle::notify_all_player_dead_delay(Player* p, uint32_t syn_number, uint32_t bullet_times)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, syn_number, idx);
	pack(pkgbuf, bullet_times, idx);
	pack(pkgbuf, p->id, idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_player_dead_delay_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);
}

void Battle::notify_player_battle_end(Player* p)
{
	uint32_t count = playerlist.size();
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, btl_id, idx);
	pack_h(pkgbuf, p->player_win_flag, idx);
	pack_h(pkgbuf, count, idx);

	std::list<Player*>::iterator pItr = playerlist.begin();
	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p_player = *pItr;
		pack_h(pkgbuf, p_player->id, idx);
		pack_h(pkgbuf, p_player->team, idx);
		pack(pkgbuf,   p_player->nick, sizeof(p->nick), idx);

		uint32_t hit_rate = 0;
		if(p_player->total_atk_count != 0)
		{
			hit_rate = p_player->total_hit_count * 100 / p_player->total_atk_count;	
		}
		pack_h(pkgbuf, hit_rate, idx);
		
		pack_h(pkgbuf, p_player->total_damage, idx);
		pack_h(pkgbuf, p_player->total_exp, idx);
		pack_h(pkgbuf, p_player->total_extern_exp, idx);
	}

	init_btl_proto_head(pkgbuf, online_proto_battle_end_cmd, idx);
	send_to_player(p, pkgbuf, idx, 0);
}

void Battle::notify_all_battle_end()
{
	std::list<Player*>::iterator pItr = playerlist.begin();

	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;
		notify_player_battle_end(p);
	}

	ERROR_TLOG("--------notify all battle end----------");
}

void Battle::notify_all_obj_attr_chg_delay(Player* p, uint64_t type, uint32_t syn_number, uint32_t bullet_times, uint32_t common_flag)
{
	uint32_t type_count = 0;
	int idx =sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, syn_number, idx);
	pack(pkgbuf, bullet_times, idx);
	pack(pkgbuf, p->id, idx);
	int idx2 = idx;
	pack(pkgbuf, type_count, idx);

	if(check_type(type, OBJ_ATTR_HP))
	{
		type_count++;
		pack(pkgbuf, (uint32_t)(OBJ_ATTR_HP), idx);
		pack(pkgbuf, p->get_merge_attr_value(OBJ_ATTR_HP), idx);
		pack(pkgbuf, common_flag, idx);
	}
	YJ_DEBUG_LOG("ATTR CHG %u type:%u value:%u", p->id, type, p->get_merge_attr_value(OBJ_ATTR_HP));
	pack(pkgbuf, type_count, idx2);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_obj_attr_chg_delay_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);
}

void Battle::notify_all_bullet_hit_delay(uint32_t bullet_id, uint32_t bullet_syn_number, uint32_t bullet_times, uint32_t x, uint32_t y)
{
	int idx =sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, bullet_id, idx);
	pack(pkgbuf, bullet_syn_number, idx);
	pack(pkgbuf, bullet_times, idx);
	pack(pkgbuf, x, idx);
	pack(pkgbuf, y, idx);

	YJ_DEBUG_LOG("BULLET HIT %u %u,%u", bullet_id, x, y);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_bullet_hit_delay_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);
}

void Battle::notify_all_progress_bar(uint32_t userid, uint32_t progress)
{
	int idx =sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, userid, idx);
	pack(pkgbuf, progress, idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_progress_bar_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);
}


void Battle::notify_all_player_attack_end(Player* p)
{
	int idx =sizeof(btl_proto_t) + sizeof(cli_proto_t);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_player_attack_end_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);
}

void Battle::notify_all_fall_down(Player* p)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, (uint32_t)p->get_x(), idx);
	pack(pkgbuf, (uint32_t)p->get_y(), idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_player_fall_down_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);
}

void Battle::notify_all_leave_battle(uint32_t uid)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, uid, idx);
	
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_leave_battle_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);
}

void Battle::notify_all_fall_down_delay(Player* p, uint32_t syn_number, uint32_t bullet_times)
{
	int idx =sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, syn_number, idx);
	pack(pkgbuf, bullet_times, idx);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, (uint32_t)p->get_x(), idx);
	pack(pkgbuf, (uint32_t)p->get_y(), idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_player_fall_down_delay_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);
}

void Battle::notify_all_select_skill(Player* p, uint32_t skill_id, uint32_t skill_lv)
{
	int idx =sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, skill_id, idx);
	pack(pkgbuf, skill_lv, idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_select_skill_cmd,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, online_proto_transmit_only_cmd, idx);
	broadcast_to_all(pkgbuf, idx);
}
