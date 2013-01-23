#ifndef BATTLE_MANAGER_HPP_
#define BATTLE_MANAGER_HPP_

#include <algorithm>
#include <vector>
#include <list>
#include <boost/pool/object_pool.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/memory/mempool.hpp>

using namespace std;
using namespace taomee;

class Battle;
class Player;

class battle_mgr
{
private:	
	battle_mgr();
	~battle_mgr();
public:
	static battle_mgr* get_instance();
	static uint32_t    s_base_id;
public:
	void     on_battle_mgr_time( struct timeval cur_time );
	uint32_t alloc_battle_id();
	void     release_battle_id(uint32_t btl_id);
public:
   	bool 	add_battle(Battle* btl);
	bool 	del_battle(uint32_t battle_id);
	Battle* get_battle_by_id(uint32_t battle_id);
	bool    is_battle_exist(uint32_t battle_id);
public:
	void     clear_all_battle();
private:
	list<Battle*> 	btl_list;
	list<uint32_t>  btl_id_list;
};


class battle_factory
{
private:
	battle_factory(){}
	~battle_factory(){};
public:
	static	battle_factory* get_instance();
public:
	Battle* create_battle(Player* creater, uint32_t battle_mode, uint32_t stage_id,  uint32_t player_count);
	void    destroy_battle(Battle* btl);
};








#endif
