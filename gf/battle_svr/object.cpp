#include "object.hpp"
#include "player.hpp"
#include "skill.hpp"
#include "battle_manager.hpp"

//#define PERFORMAN_STATIS
using namespace taomee;

#ifdef PERFORMAN_STATIS
static struct timeval func_start_tm = *get_now_tv();
static struct timeval func_end_tm = *get_now_tv();
#endif


/*! this is the next valid ID. Each time an Object is instantiated, this value is updated */
uint32_t Object::s_next_id_ = base_guest_id;


//---------------------------------------------------


void update_all_objects()
{
#ifdef PERFORMAN_STATIS
	static uint32_t call_cnt1 = 0;
	static uint32_t consume_tm1 = 0;
	gettimeofday(&func_start_tm,NULL);
#endif

	static struct timeval s_mon_update_tv   = *get_now_tv();
	static struct timeval s_skill_update_tv = *get_now_tv();
	static struct timeval s_player_update_tv = *get_now_tv();

	const timeval* tv = get_now_tv();
		

	int time_elapsed = timediff2(*tv, s_player_update_tv);
	// update players
	if(time_elapsed > 100){

		//TRACE_LOG("player buff routing");
		s_player_update_tv = *tv;
		player_map_routing(*tv);
		//map summon object routing
		//BattleManager::get_instance()->battle_mgr_routing(*tv);	
	}




	time_elapsed = timediff2(*tv, s_mon_update_tv);
	// update monsters
#ifndef UPDATE_NOR_PROXY
	//if (time_elapsed > (230) / MonlistMrg::max_monlist_arr_cnt) {
	if (time_elapsed > 99) {
		s_mon_update_tv = *tv;

		g_monlist_mrg.update_monlist(time_elapsed);
	}
#else 
	if (time_elapsed > 230) {
		s_mon_update_tv = *tv;
		MonList::iterator nx = g_monlist.begin();
		for (MonList::iterator it = nx; it != g_monlist.end(); it = nx) {
			++nx;
			if (!it->is_dead()) {
				it->update(time_elapsed);

			} else {
				delete &(*it);
			}
		}
	}		
#endif

	// update skills
	time_elapsed = timediff2(*tv, s_skill_update_tv);
	if (time_elapsed > 90) {
		
		BattleManager::get_instance()->battle_mgr_routing(*tv);   

		s_skill_update_tv = *tv;

		ObjList::iterator it = g_skill_list.begin();
		while (it != g_skill_list.end()) {
			if (!(*it)->is_dead()) {
				(*it)->update(time_elapsed);
				++it;
			} else {
				delete *it;
				it = g_skill_list.erase(it);
			}
		}
	}

#ifdef PERFORMAN_STATIS
	gettimeofday(&func_end_tm,NULL);
	call_cnt1++;
	consume_tm1 += timediff2(func_end_tm, func_start_tm);
#endif
}

