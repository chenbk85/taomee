/**
 *============================================================
 *  @file      battle_team.hpp
 *  @brief    declare class BattleTeam
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_BATTLE_TEAM_HPP_
#define KFBTL_BATTLE_TEAM_HPP_

#include <algorithm>
#include <vector>

#include <boost/pool/object_pool.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>

#include <libtaomee++/bitmanip/bitmanip.hpp>

//#include "mempool/mempool.hpp"
#include "moving_object.hpp"
#include "player.hpp"
#include "battle.hpp"



class BattleManager : public taomee::MemPool, public taomee::EventableObject {
	public:
		/**
		  * @brief a map to hold all the battle's  teams in a battle
		  */
		typedef std::map<uint32_t, Battle*>		BattleMap;

		typedef std::list<uint32_t>		BattleIDList;

	public:
		static BattleManager* get_instance()
			{ 
				static BattleManager instance;
				return &instance;
			}
		~BattleManager() { }

	public:
		void  battle_mgr_routing( struct timeval  next_tm )
		{
			BattleMap::iterator it = composite_battle.begin();
			for(; it != composite_battle.end(); ++it)
			{
				Battle* battle = it->second;
				battle->map_summon_routing(next_tm);
				battle->battle_section_routing(next_tm);
			}	
		}


		Battle* get_battle(uint32_t team_id, bool team_battle = true)
			{ 		
				BattleMap::iterator it = composite_battle.find(team_id);
				if (it == composite_battle.end()) {
					TRACE_LOG("add_battle %u %u %u", get_server_id(), team_id, (uint32_t)composite_battle.size());
					return 0;
				} 
				
				Battle* battle = it->second;
				TRACE_LOG("get_battle %u %u %u %u %u", team_id, it->second->id(), (uint32_t)composite_battle.size(), battle->is_team_btl(), team_battle);
		 		if( (battle->is_team_btl() ^ team_battle) == false)
				{
					return battle;
				}
				return NULL;
			}
		Battle* add_battle(Battle* p_btl)
			{ 
				all_battle[p_btl->stage()->id][p_btl->difficulty() - 1].insert(BattleMap::value_type(p_btl->id(), p_btl)); 
				composite_battle.insert(BattleMap::value_type(p_btl->id(), p_btl)); 
				TRACE_LOG("add_battle %u %u %u", get_server_id(), p_btl->id(), (uint32_t)composite_battle.size());
				return p_btl;	
		 	}
		void del_battle(Battle* p_btl)
			{ 
				TRACE_LOG("del_battle %u", p_btl->id());
				BattleMap* p_btmp = &(all_battle[p_btl->stage()->id][p_btl->difficulty() - 1]);
				BattleMap::iterator it = p_btmp->find(p_btl->id());
				if (it != p_btmp->end()) {
					p_btmp->erase(it); 
					BattleMap::iterator it2 = composite_battle.find(p_btl->id());
					composite_battle.erase(it2); 
				}
		 	}

		Battle* get_radom_battle(uint32_t stage_id, uint32_t difficulty, bool team_battle = true)
			{
				BattleMap* p_btmp = &(all_battle[stage_id][difficulty - 1]);
				BattleMap::iterator it = p_btmp->begin();
				for (; it != p_btmp->end(); ++it) {
					if (!(it->second->is_player_full()) && !(it->second->is_battle_started()) 
						&& it->second->is_permit_hot_join() &&  !(it->second->is_team_btl() ^ team_battle)) {
						TRACE_LOG("get radom battle %u", it->second->id());
						return it->second;
					}
				}
				TRACE_LOG("get_radom_battle %u %u %u %u", stage_id, difficulty, get_server_id(), (uint32_t)p_btmp->size());
				return 0;				
			}

		uint32_t produce_battle_id()
			{
				BattleIDList::iterator it = battle_id_list.begin();
				if (battle_id_list.end() == it) {
					return 0;
				}

				uint32_t id = battle_id_list.front();

				battle_id_list.pop_front();


				TRACE_LOG("produce battle id:%u ", id);

				return id;
			}
		
		void release_battle_id(uint32_t id)
			{
				battle_id_list.push_back(id);
				TRACE_LOG("release battle id:%u", id);
			}

	private:
		BattleManager() 
			{
				for (uint32_t i = 1; i < 0x000fffe ; i++) {
					battle_id_list.push_back(i);
				}
				TRACE_LOG("init battle_id_list size:%u", (uint32_t)battle_id_list.size());
			}
		
	private:
		BattleMap all_battle[max_stage_num][max_stage_difficulty_type];
		BattleMap composite_battle;
		BattleIDList battle_id_list;

};

#endif //~battle_team

