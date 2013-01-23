#include "battle_manager.hpp"
#include "battle_impl.hpp"
#include "player.hpp"
#include "single_pvp_battle.hpp"

uint32_t battle_mgr::s_base_id = 0;

battle_mgr::battle_mgr()
{
	for(uint32_t i = 1; i< 0xfffe; i++)
	{
		btl_id_list.push_back(i);	
	}
}

battle_mgr::~battle_mgr()
{
	btl_id_list.clear();	
}

battle_mgr* battle_mgr::get_instance()
{
	static battle_mgr obj;
	return &obj;
}

void     battle_mgr::clear_all_battle()
{
	std::list<Battle*>::iterator pItr = btl_list.begin();
	for(;  pItr != btl_list.end(); ++pItr)
	{
		Battle* btl = *pItr;
		battle_factory::get_instance()->destroy_battle(btl);
		btl = NULL;
	}
	btl_list.clear();
}

void     battle_mgr::on_battle_mgr_time( struct timeval cur_time )
{
	std::list<Battle*>::iterator pItr = btl_list.begin();
	
	while(pItr != btl_list.end())
	{
		Battle* btl = *pItr;	
		if(btl->check_btl_state(battle_destroy_state))
		{
			pItr = btl_list.erase(pItr);
			battle_factory::get_instance()->destroy_battle(btl);
			continue;
		}
		if(btl->check_btl_state(battle_running_state) || btl->check_btl_state(battle_end_state))
		{
			btl->on_battle_timer( cur_time );
			++pItr;
			continue;
		}
		++pItr;
	}

}

uint32_t battle_mgr::alloc_battle_id()
{
	std::list<uint32_t>::iterator pItr = btl_id_list.begin();
	if(pItr == btl_id_list.end())return 0;
	uint32_t btl_id = btl_id_list.front();
	btl_id_list.pop_front();
	return btl_id;
}

void  battle_mgr::release_battle_id(uint32_t btl_id)
{
	btl_id_list.push_back(btl_id);
}

bool    battle_mgr::add_battle(Battle* btl)
{
	if(is_battle_exist(btl->get_battle_id()))return false;
	btl_list.push_back(btl);
	return true;
}

bool    battle_mgr::del_battle(uint32_t battle_id)
{
	std::list<Battle*>::iterator pItr = btl_list.begin();
	for(; pItr != btl_list.end(); ++pItr)
	{
		Battle* btl = *pItr;
		if(btl->get_battle_id() == battle_id)
		{
			pItr = btl_list.erase(pItr);
			return true;
		}
	}
	return false;
}

Battle*  battle_mgr::get_battle_by_id(uint32_t battle_id)
{
	std::list<Battle*>::iterator pItr = btl_list.begin();
	for(; pItr != btl_list.end(); ++pItr)
	{
		Battle* btl = *pItr;
		if(btl->get_battle_id() == battle_id)
		{
			return btl;
		}
	}
	return NULL;
}

bool    battle_mgr::is_battle_exist(uint32_t battle_id)
{
	std::list<Battle*>::iterator pItr = btl_list.begin();
	for(; pItr != btl_list.end(); ++pItr)
	{
		Battle* btl = *pItr;
		if(btl->get_battle_id() == battle_id)return true;
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////////

battle_factory* battle_factory::get_instance()
{
	static battle_factory obj;
	return &obj;
}

Battle* battle_factory::create_battle(Player* creater, uint32_t battle_mode, uint32_t stage_id, uint32_t player_count)
{
	Battle* btl = NULL;
	switch(battle_mode)
	{
		case single_pvp_mode:
		{
			btl = new single_pvp_battle(creater, battle_mode, stage_id,  player_count);
			btl->init_battle();
		}
		break;

		case team_pvp_mode:
		{
			btl = new team_pvp_battle(creater, battle_mode, stage_id, player_count);
			btl->init_battle();
		}
		break;

		case single_pve_mode:
		{
			btl = new single_pve_battle(creater, battle_mode, stage_id, player_count);
			btl->init_battle();
		}
		break;

		case team_pve_mode:
		{
			btl = new team_pve_battle(creater, battle_mode, stage_id, player_count);
			btl->init_battle();
		}
		break;

		default :
			return btl;
	}
	return btl;
}

void    battle_factory::destroy_battle(Battle* btl)
{
	btl->final_battle();
	delete btl;
}
