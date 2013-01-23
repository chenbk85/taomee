#ifndef _SINGLE_PVP_BATTLE_HPP
#define _SINGLE_PVP_BATTLE_HPP

#include <algorithm>
#include <vector>
#include <list>
#include <boost/pool/object_pool.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/memory/mempool.hpp>
#include "battle_impl.hpp"

class single_pvp_battle: public Battle
{
public:
	single_pvp_battle(Player* creater, uint32_t battle_mode, uint32_t stage_id,  uint32_t player_count)
	:Battle(creater, battle_mode, 0, stage_id, player_count)
	{

	}
	~single_pvp_battle()
	{
		
	}
public:
	bool init_battle(){return true;}
	bool final_battle(){return true;}
public:
	void on_battle_timer( struct timeval cur_time );
public:
	bool check_battle_end();
	bool calc_battle_statistics_data();
	bool statistics_player_dead(Player* atker, Player* dead);
	bool statistics_player_damage(Player* atker, Player* dead, uint32_t damage);
};


























#endif
