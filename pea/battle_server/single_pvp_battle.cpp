#include "single_pvp_battle.hpp"
#include "player.hpp"

void single_pvp_battle::on_battle_timer( struct timeval cur_time )
{
	Battle::on_battle_timer( cur_time );	
}

bool single_pvp_battle::check_battle_end()
{		
	std::list<Player*> red_list;
	std::list<Player*> blue_list;
	
	uint32_t red_dead_count = 0;
	uint32_t blue_dead_count = 0;

	std::list<Player*>::iterator pItr = playerlist.begin();

	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;
		if(p->team == TEAM_ONE)
		{
			red_list.push_back(p);	
			if(p->is_dead())red_dead_count++;
		}
		else if(p->team == TEAM_TWO)
		{
			blue_list.push_back(p);	
			if(p->is_dead())blue_dead_count++;
		}
	}
	
	if(red_list.size() == 0 && blue_list.size() > 0) 
	{
		set_win_team(TEAM_TWO);
		return true;
	}

	if(blue_list.size() == 0 && red_list.size() > 0)
	{
		set_win_team(TEAM_ONE);	
		return true;
	}


	if(blue_list.size() == blue_dead_count && red_list.size() > red_dead_count)
	{
		set_win_team(TEAM_ONE);
		return true;
	}

	if(red_list.size() == red_dead_count && blue_list.size() > blue_dead_count)
	{
		set_win_team(TEAM_TWO);
		return true;
	}

	set_win_team(TEAM_NO);
	return false;
}

bool single_pvp_battle::calc_battle_statistics_data()
{
	std::list<Player*>::iterator pItr = playerlist.begin();

	for(; pItr != playerlist.end(); ++pItr)
	{
		Player* p = *pItr;
		if(p->team == get_win_team() )
		{
			p->set_player_win_flag(btl_player_win);
		}
		else
		{
			p->set_player_win_flag(btl_player_lose);	
		}
	
	}
	return true;	
}

bool single_pvp_battle::statistics_player_dead(Player* atker, Player* dead)
{
	return true;	
}
    
bool single_pvp_battle::statistics_player_damage(Player* atker, Player* dead, uint32_t damage)
{

	return true;	
}
