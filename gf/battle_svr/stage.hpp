/**
 *============================================================
 *  @file      stage.hpp
 *  @brief    stage & map definitions
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_BATTLE_STAGE_HPP_
#define KFBTL_BATTLE_STAGE_HPP_

#include <set>
#include <vector>

extern "C" {
#include <stdint.h>
}

#include <libtaomee++/conf_parser/xmlparser.hpp>

#include "fwd_decl.hpp"
#include "cli_proto.hpp"
#include "monster.hpp"
#include "player.hpp"
#include "player_status.hpp"


enum {
	map_base_id			= 1000000,
	stage_max_num		= 1100,
	stage_max_map_num	= 20,
	stage_base_id		= 100,
	
	stage_mode_easy = 1,
	stage_mode_nor	= 2,
	stage_mode_hard	= 3,
	stage_mode_hell	= 4,
	stage_mode_legend = 5,
	stage_mode_epic = 6,
	

	stage_mode_tower = 11,
	max_stage_type	= 11,

	stage_tower_base_floor = 800,
};



inline int get_stage_id(uint32_t mid)
{
	uint32_t id = (mid - map_base_id) / stage_base_id;
	if (id < stage_max_num) {
		return id;
	}

	return -1;
}

inline int get_stage_map_id(uint32_t mid)
{
	uint32_t id = ((mid - map_base_id) % stage_base_id) - 1;
	if (id < stage_max_map_num) {
		return id;
	}

	return -1;
}
inline bool is_tower_stage(uint32_t stageid) {
	return false;
}

struct tomaps_t {
	tomaps_t()
		{ id = 0; init_x = 0; init_y = 0;}

	uint16_t	id;
	uint16_t	init_x;
	uint16_t	init_y;
};

struct map_monster_t {
	uint16_t	id;
	uint16_t	x;
	uint16_t	y;
	uint8_t		call_flg;
	uint8_t 	rt_birth_flg;
};

struct rand_monster_t {
	uint32_t    id;
	uint32_t    x_aur[2];
	uint32_t    y_aur[2];
	uint32_t    odds;
};

struct map_t {
public:
	typedef std::vector<map_monster_t>		MapMonVec;
	typedef std::vector<rand_monster_t>     RanMonVec;
public:
	~map_t()
	{
		for (PlayerSet::iterator it = monsters.begin(); it != monsters.end(); ++it) {
			(*it)->cur_map = 0;
			delete *it;
		}

		for (PlayerSet::iterator it = barriers.begin(); it != barriers.end(); ++it) {
			(*it)->cur_map = 0;
			delete *it;
		}

		for (PlayerSet::iterator it = pre_barriers.begin(); it != pre_barriers.end(); ++ it) {
			(*it)->cur_map = 0;
			delete *it;
		}

		for (PlayerSet::iterator it = pre_monsters.begin(); it != pre_monsters.end(); ++ it) {
			(*it)->cur_map = 0;
			delete *it;
		}
	}

	void init()
	{
		delete [] to;
		delete mons;
		delete rt_birth_mons;
		delete path;
		delete rand_mons;
		id     = 0;
        mon_lv = 0;
		to     = 0;
		mons   = 0;
		path   = 0;
		visited_flg = false;
	}

	void add_all_monsters_buff(uint32_t buff_id, uint32_t team, Player* except_player)
	{
		for (PlayerSet::iterator it = monsters.begin(); it != monsters.end(); ++it) 
		{
			Player* p = *it;
			if(p->team == team && p != except_player)
			{
				add_player_buff(p, buff_id, 0);
			}
		}
	}

	void set_monster_dead(uint32_t mon_type);
	
	void clear_old_barriers()
	{
	}

	const tomaps_t* get_tomap(uint16_t mid)
	{
		if ((mid < stage_max_map_num) && to[mid].id) {
			return &(to[mid]);
		}

		return 0;
	}

	uint32_t get_monster_cnt()
	{
		return monsters.size();
	}

	uint32_t get_monster_cnt(uint32_t mon_type)
	{
		uint32_t cnt = 0;
		for (PlayerSet::iterator it = monsters.begin(); it != monsters.end(); ++it) {
			Player* mon = *it;
			if (mon->role_type == mon_type) {
				cnt ++;
			}
		}
		return cnt;
	}

    bool is_all_monster_dead()
	{
		for (PlayerSet::iterator it = monsters.begin(); it != monsters.end(); ++it) {
			Player* mon = *it;
			if (!mon->is_dead()) {
                return false;
			}
		}
		return true;
	}

	uint32_t get_dead_pre_barriers_cnt(uint32_t mon_type)
	{
		uint32_t cnt = 0;
		for (PlayerSet::iterator it = pre_barriers.begin(); it != pre_barriers.end(); ++it) {
			Player* mon = *it;
			if (mon->role_type == mon_type && mon->mp == 0) {
				
				cnt ++;
			}
		}
		return cnt;
	}

	uint32_t get_enemy_cnt(Player* p)
	{
		uint32_t cnt = 0;
		for (PlayerSet::iterator it = monsters.begin(); it != monsters.end(); ++it) {
			Player* mon = *it;
			if (mon->team != p->team && mon->team != neutral_team_1) {
				cnt ++;
			}
		}
		return cnt;
	}

	Player* get_boss()
	{
		for (PlayerSet::iterator it = monsters.begin(); it != monsters.end(); ++it) {
			Player* mon = *it;
			if (mon->mon_info->type == 1 && !(mon->is_dead())) {
				return mon;
			}
		}
		return 0;
	}

    bool is_monster_exist(uint32_t mon_id)
    {
        for (PlayerSet::iterator it = monsters.begin(); it != monsters.end(); ++it) {
            if ((*it)->role_type == mon_id) {
                return true;
            }
        }
        return false;
    }

	void activate_monster(Player* mon)
	{
		update_challenge_attr(mon, mon->btl);
#ifndef UPDATE_NOR_PROXY
		g_monlist_mrg.insert_monlist(*mon);
#else
		g_monlist.push_back(*mon);
#endif
		if (mon->role_type == 11309) { //火怪
			add_player_buff(mon, 33, 0, 6000, false);
		} 

		if (mon->role_type == 11310) { //冰晶蜗牛
			add_player_buff(mon, 34, 0, 6000, true);
		}

	}

	// 特殊机关触发,出怪物
	void activate_pre_monsters()
	{
		for (PlayerSet::iterator it = pre_monsters.begin(); it != pre_monsters.end(); ++it) {
			Player* p = *it;
			p->do_enter_map(p->cur_map, false);
			update_challenge_attr(p, p->btl);

#ifndef UPDATE_NOR_PROXY
		g_monlist_mrg.insert_monlist(**it);
#else
		g_monlist.push_back(**it);
#endif			
			

			int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
			idx += p->pack_rt_birth_mon_basic_info(pkgbuf + idx);
			init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_realtm_birth_mon_info, idx - sizeof(btl_proto_t));
			init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
			send_to_map(pkgbuf, idx, 0);
		}

		pre_monsters.clear();
	}

	void activate_monsters()
	{
		for (PlayerSet::iterator it = monsters.begin(); it != monsters.end(); ++it) {

#ifndef UPDATE_NOR_PROXY
			g_monlist_mrg.insert_monlist(**it);
#else
			g_monlist.push_back(**it);
#endif			
			Player * mon = *it;
			if (mon->role_type == 11309) { //火怪
				add_player_buff(mon, 33, 0, 6000, false);
			} 

			if (mon->role_type == 11310) { //冰晶蜗牛
				add_player_buff(mon, 34, 0, 6000, true);
			}
		}
	}


	void set_all_monsters_dead()
	{
		for (PlayerSet::iterator it = monsters.begin(); it != monsters.end(); ++it) {
			Player* p = *it;
			if ( !(p->is_dead()) ) {
				p->set_dead();
			}
		}

	}

    void set_special_monster_dead(uint32_t mon_id)
    {
        for (PlayerSet::iterator it = monsters.begin(); it != monsters.end(); ++it) {
            if (!((*it)->is_dead()) && (*it)->role_type == mon_id) {
                (*it)->set_dead();
                notify_delete_player_obj(*it);
            }
        }
    }

	void send_to_map(void* pkg, uint32_t len, Player* p = 0)
	{
		//Battle * ptl = 0;
		for (PlayerSet::iterator it = players.begin(); it != players.end(); ++it) {
			send_to_player(*it, pkg, len, 0);
		//	ptl = (*it)->btl;
		}

		if (p) {
			p->waitcmd = 0;
		}
	}

	void send_to_map_except_player(void* pkg, uint32_t len, uint32_t except_id,  Player* p = 0)
	{
		for (PlayerSet::iterator it = players.begin(); it != players.end(); ++it) 
		{
			if( (*it)->id == except_id ) continue;
			send_to_player(*it, pkg, len, 0);
		}

		if (p) {
			p->waitcmd = 0;
		}
	}

	void send_players_info(Player* p)
	{
		int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

		idx += pack_players_basic_info(p, pkgbuf + idx);

		init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_players_info, idx - sizeof(btl_proto_t));
		init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

		if (p) {
			send_to_player(p, pkgbuf, idx, 0);
		} else {
			send_to_map(pkgbuf, idx, 0);
		}
	}
	void send_player_info(Player* p, uint32_t except_player = 0)
	{
		int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
				
		idx += p->pack_rt_birth_mon_basic_info(pkgbuf + idx);
		init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_realtm_birth_mon_info, idx - sizeof(btl_proto_t));
		init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
		if (except_player) {
			send_to_map_except_player(pkgbuf, idx, except_player, 0);
		} else {
			send_to_map(pkgbuf, idx, 0);
		}
	}

	void send_players_status_info(Player* p, bool except_self = true);

	int pack_players_basic_info(Player* p_ent, uint8_t* buf)
	{
		int idx = 4, idx2 = 0;
		int cnt = players.size() + monsters.size();

		for (PlayerSet::iterator it = players.begin(); it != players.end(); ++it) {
			Player* p = *it;
			idx += p->pack_basic_info(buf + idx);
		}
        //deal summon 获取自己的灵兽 
        if (p_ent && p_ent->my_summon) {
            p_ent->my_summon->set_pos(p_ent->pos());
            cnt++;
            idx += p_ent->my_summon->pack_basic_info(buf + idx);
        }
        //获取地图上其他玩家的灵兽
        for (PlayerSet::iterator it = players.begin(); it != players.end(); ++it) {
            Player* p = *it;
            if (p->my_summon) {
                if ( (p_ent) && (p_ent->id == p->id) ) {
                    continue;
                } else {
                    cnt++;
                    idx += p->my_summon->pack_basic_info(buf + idx);
                }
            }
        }
		//获取地图上神的信息

		if (p_ent && p_ent->my_numen) {
            cnt++;
            idx += p_ent->my_numen->pack_basic_info(buf + idx);
        }
        for (PlayerSet::iterator it = players.begin(); it != players.end(); ++it) {
			Player* p = *it;
			if (p->my_numen) {
				 if ( (p_ent) && (p_ent->id == p->id) ) {
                    continue;
                } else {
					cnt++;
					idx += p->my_numen->pack_basic_info(buf + idx);
                }
			}
		}
		for (PlayerSet::iterator it = monsters.begin(); it != monsters.end(); ++it) {
			Player* p = *it;
		//	ERROR_LOG("MONSTER HP = %u MAXHP=%u", p->hp, p->max_hp());
			idx += p->pack_basic_info(buf + idx);
		}
		for (PlayerSet::iterator it = barriers.begin(); it != barriers.end(); ++it) {
			Player* p = *it;
			if (!p->is_dead()) {
				idx += p->pack_basic_info(buf + idx);
				++cnt;
			}
		}
		for (PlayerSet::iterator it = pre_barriers.begin(); it != pre_barriers.end(); ++ it) {
			Player * p = * it;
			if (!p->is_dead()) {
				idx += p->pack_basic_info(buf + idx);
				++cnt;
			//	ERROR_LOG(":LOAD PRE BARRIESR!...........");
			}
		}
		taomee::pack(buf, cnt, idx2);

		return idx;
	}

	void send_all_item_drop(Player* p)
	{
		if (item_drops.size() == 0) {
			return;
		}
		
		int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
		int idx_tmp = pack_all_drop_items_info(pkgbuf + idx, p);
		if (idx_tmp <= 4) {
			return;
		}
		idx += idx_tmp;
		init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), p->id, cli_proto_monster_item_drop, 0, idx - sizeof(btl_proto_t));
		init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	
		send_to_player(p, pkgbuf, idx, 0);
	}
	
	int pack_all_drop_items_info(uint8_t* buf, Player* p)
	{
		int idx = 4;
		int idx2 = 0;
		uint32_t count = 0;

		ItemDropMap::iterator it;
		for ( it = item_drops.begin(); it != item_drops.end(); ++it ) {
			if (it->second.can_be_saw_by_player(p->id)) {
				idx += it->second.pack_item_info(buf + idx);
				count++;
			}
		}
		taomee::pack(buf, count, idx2);
		
		return idx;
	}

	Player * get_one_barriers(uint32_t barr_type)
	{
		PlayerSet::iterator it = barriers.begin();
		while (it != barriers.end()) {
			if ((*it)->role_type == barr_type) {
				return (*it);
			}
			++it;
		}
		return 0;
	}

	int  get_barriers_count()
	{
		PlayerSet::iterator it = barriers.begin();
		uint32_t count = 0;
		for ( ; it != barriers.end(); ++it) {
			if ((*it)->is_dead()) {
				++count; 
			}
		}
		return count;
	}


	void del_barriers_from_map()
	{
		for (PlayerSet::iterator it = barriers.begin(); it != barriers.end(); ++it) 
		{
			    Player* p = *it;
				if(!p)continue;
				if(p->cur_map)
				{
					notify_delete_player_obj(p);
				}
				p->cur_map = NULL;
				delete p;
				p = NULL;				
		}		
		barriers.clear();
	}

	Player *get_mechanism(uint32_t id) {
		PlayerSet::iterator it = pre_barriers.begin();
	   	while  (it != pre_barriers.end()) {
			if ((*it)->id == id) {
				return *it;
			}
			++it;
		}	

		return 0;
	}

	uint32_t  get_mechanism_id_by_type(uint32_t role_type) {
		PlayerSet::iterator it = pre_barriers.begin();
	   	while  (it != pre_barriers.end()) {
			if ((*it)->role_type == role_type) {
				return (*it)->id;
			}
			++it;
		}	
		return 0;

	}

	Player * get_mechanism_by_type(uint32_t role_type)
	{
		PlayerSet::iterator it = pre_barriers.begin();
	   	while  (it != pre_barriers.end()) {
			if ((*it)->role_type == role_type) {
				return (*it);
			}
			++it;
		}	
		return 0;
	}

	void del_mechanism_from_map(uint32_t id) 
	{
		PlayerSet::iterator it = pre_barriers.begin();
		
		while (it != pre_barriers.end()) {
			if ((*it)->id == id) {
				Player * mechanism = *it;
				if(mechanism->cur_map)
				{
					notify_delete_player_obj(mechanism);
				}
				mechanism->cur_map = NULL;
				delete mechanism;
				pre_barriers.erase(it);
				return;
			}
			++it;
		}
	}

	Player* get_one_player()
		{
			PlayerSet::iterator it = players.begin();
			if (it != players.end()) {
				return *it;
			} 
			return 0;
		}
	Player * get_one_monster(uint32_t mon_type)
	{
			PlayerSet::iterator it = monsters.begin();
			while (it != monsters.end()) {
				if ( (*it)->role_type == mon_type) {
					return (*it);
				}
				++it;
			} 
			return 0;
	}

	void mechansim_status_changed(int mechansim_id, uint32_t status, Player * p = 0)
	{
		Player * mechansim = get_mechanism(mechansim_id);
		if (mechansim) {
			mechansim->mp = status;
		}

		uint32_t id = 0;
		if (p) {
			id = p->id;
		}
		int idx = sizeof(btl_proto_t); 
		taomee::pack_h(pkgbuf, id, idx); 
		taomee::pack_h(pkgbuf, mechansim_id, idx);
		taomee::pack_h(pkgbuf, status, idx);
		init_btl_proto_head(pkgbuf,  btl_change_mechanism, idx); 
		send_to_map(pkgbuf, idx, p);
	}

public:
	uint32_t	id;
	bool		visited_flg;
    uint16_t    mon_lv;

	PlayerSet	players;
	PlayerSet	monsters;
	PlayerSet	barriers;
	PlayerSet   pre_barriers;//中立无敌机关类
	PlayerSet   pre_monsters;//需要机关触发才出现的怪物


	ItemDropMap item_drops;

	tomaps_t*	to;
	MapMonVec*	mons;
	MapMonVec*  rt_birth_mons;
	RanMonVec*  rand_mons;

	KfAstar*	path;
};

struct stage_t {
public:
	typedef std::set<const monster_t*>		StgMonSet;

public:
	void init()
	{
		id      = 0;
		lv[0]   = 0;
		lv[1]   = 0;		
		map_num = 0;
		for (int i = 0; i != stage_max_map_num; ++i) {
			maps[i].init();
		}
	}

	~stage_t()
	{
		init();
	}

public:
	uint16_t	id;
	int16_t		lv[2];

	uint16_t	init_x;
	uint16_t	init_y;
    uint16_t    sec_x;
    uint16_t    sec_y;

	uint16_t	map_num;
	map_t		maps[stage_max_map_num];
	uint16_t	init_map_id;
	uint32_t    boss_id;
	uint32_t	max_lv;
	StgMonSet	mon_set;
};

struct finder_t {
	finder_t(uint32_t inval): m_val(inval) {}
	
	bool operator()(Player *p)
	{
		return (m_val == p->role_type && !p->is_dead());
	}
	userid_t m_val;
};

const stage_t* get_stage(uint16_t sid, uint32_t stage_mode = 0);



struct punish_info_t {
	uint32_t stage_id;
	uint32_t protect_lv;
	uint32_t exp_per;
	uint32_t duration_per;
};


bool is_punish_stage(uint32_t stageid);
uint32_t get_duration_punish_percent(uint32_t stageid);
uint32_t get_exp_punish_percent(uint32_t stageid);

/**
  * @brief load stage configs from "./conf/stages"
  * @return 0 on success, -1 on error
  */
 void load_all_stages();
void load_stages(uint32_t stage_mode);
void unload_stages();

int load_punish_stage(xmlNodePtr cur);

enum {
	max_diff_cnt = 12,
	max_challenge_cnt = 100,
};

struct challenge_arr_t {
	uint32_t diff_id;
	uint32_t cool_dec;
	uint32_t hp_add;
	uint32_t atk_add;
	uint32_t lv_add;
	uint32_t drop_add;
	uint32_t low_cnt;
	uint32_t mid_cnt;
	uint32_t high_cnt;
	uint32_t boss_cnt;
};

struct challenge_t {
	uint32_t stage_id;
	uint32_t diff_cnt;
	challenge_arr_t challenges[max_diff_cnt];

	challenge_t()
	{
		stage_id = 0;
		diff_cnt = 0;
		memset(challenges, 0, sizeof(challenges));
	}
	
};

const challenge_t  * get_challenge(const uint32_t stageid);

int load_challenges(xmlNodePtr cur);

#endif // KFBTL_BATTLE_STAGE_HPP_
