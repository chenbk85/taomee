// =====================================================================================
//
//       Filename:  online.hpp
// 
//    	 Description: specify the communication between online and switch 
// 
//       Version:  1.0
//       Created:  12/26/2011 08:35:48 PM CST
//       Compiler:  g++
// 
//       Company:  TAOMEE
// 
// =====================================================================================


#ifndef GROUP_HPP_
#define GROUP_HPP_

#include <map>
#include <libtaomee++/memory/mempool.hpp>

using namespace std;

extern "C" 
{
#include <libtaomee/log.h>
}
#include "object.hpp"
// =====================================================================================

class Player;



class TalkGroup : public Object {
	public:
		TalkGroup(uint32_t group_id) {
			group_id_ = group_id;
		}
		virtual ~TalkGroup() {
		}
typedef std::map<uint32_t, Player*> PlayerMap;
	public:
		Player* add_talk_obj(uint32_t id, Player* p_obj) {
			DEBUG_LOG("ADD TALK OBJ  %u ", id);
			PlayerMap::iterator it = talk_obj_map.find(id);
			if (it == talk_obj_map.end()) {
				talk_obj_map.insert(PlayerMap::value_type(id, p_obj));
			} else {
				del_talk_obj(id);
				talk_obj_map.insert(PlayerMap::value_type(id, p_obj));
			}
			return p_obj;
		}
		void del_talk_obj(uint32_t id) {
			DEBUG_LOG("DEL TALK OBJ %u", id);
			talk_obj_map.erase(id);
		}

		void send_to_group(void* pkg, uint32_t len);


		uint32_t group_id() {
			return group_id_;
		}
		uint32_t group_size() {
			return talk_obj_map.size();
		}
	private:
		uint32_t group_id_;
		PlayerMap talk_obj_map;
};

class TalkGroupSet : public Object {
	public:
		TalkGroupSet() {
		}
		virtual ~TalkGroupSet() {
		}
typedef std::map<uint32_t, TalkGroup*> GroupMap;
		virtual TalkGroup* add_group(uint32_t group_id) {
			GroupMap::iterator it = group_map.find(group_id);
			if (it == group_map.end()) {
				TalkGroup* p_tmp = new TalkGroup(group_id);
				group_map.insert(GroupMap::value_type(group_id, p_tmp));
				DEBUG_LOG("ADD NEW GROUP %u ", group_id);
				return p_tmp;
			}
			return it->second;
		}
		virtual void del_group(uint32_t group_id) {
			GroupMap::iterator it = group_map.find(group_id);
			if (it != group_map.end()) {
				DEBUG_LOG("DEL GROUP %u", group_id);
				delete it->second;
				group_map.erase(group_id);
			}
			return;

		}
		virtual TalkGroup* get_group(uint32_t group_id) {
			GroupMap::iterator it = group_map.find(group_id);
			if (it == group_map.end()) {
				return 0;
			} 
			return it->second;
		}
	private:
		GroupMap group_map;	
};

class TalkGroupMrg : public Object {
	public:
typedef std::map<uint32_t, TalkGroupSet*> TalkGroupMap;
		TalkGroupMrg() {
		}
		~TalkGroupMrg() {
		}		

		TalkGroup* get_talk_group(uint32_t group_type, uint32_t group_id) {
			if (group_type == talk_type_fight_team) {
				return fight_team_talk_group.get_group(group_id);
			}
			return 0;
		}

		Player* add_talk_obj(uint32_t group_type, uint32_t group_id, uint32_t id, Player* p_obj) {
			if (group_type == talk_type_fight_team) {
				TalkGroup* p_group = add_talk_group(group_type, group_id);
				if (p_group) {
					p_group->add_talk_obj(id, p_obj);
					return p_obj;
				}
			}
			return 0;
		}
		void del_talk_obj(uint32_t group_type, uint32_t group_id, uint32_t id) {
			if (group_type == talk_type_fight_team) {
				TalkGroup* p_group = add_talk_group(group_type, group_id);
				if (p_group) {
					p_group->del_talk_obj(id);
				}
				if (!(p_group->group_size())) {
					del_talk_group(group_type, group_id);
				}
			}
		}
		
		TalkGroup* add_talk_group(uint32_t group_type, uint32_t group_id) {
			if (group_type == talk_type_fight_team) {
				return fight_team_talk_group.add_group(group_id);;
			}
			return 0;
		}
		void del_talk_group(uint32_t group_type, uint32_t group_id) {
			if (group_type == talk_type_fight_team) {
				fight_team_talk_group.del_group(group_id);;
			}
		}

		void send_to_group(uint32_t group_type, uint32_t group_id, void* pkg, int len) {
			TalkGroupSet* p_group_set = get_talk_group_set(group_type);
			if (p_group_set) {
				TalkGroup* p_group = p_group_set->get_group(group_id);
				if (p_group) {
					p_group->send_to_group(pkg, len);
				}
			}
		}

		TalkGroupSet* get_talk_group_set(uint32_t group_type) {
			if (group_type == talk_type_fight_team) {
				return &fight_team_talk_group;
			}
			return 0;
		}
	private:
		TalkGroupSet fight_team_talk_group;	
};

#endif // GROUP_HPP

