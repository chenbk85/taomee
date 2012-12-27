extern "C" {
#include <libtaomee/log.h>
}

#include <libtaomee++/pathfinder/astar/astar.hpp>

#include "player.hpp"
#include "player_status.hpp"
#include "stage.hpp"
#include "battle_impl.hpp"

#include "monster.hpp"
#include "item.hpp"
#include "ai/MonsterAI.hpp"

using namespace taomee;


//static monster_t all_monsters[monster_max_num];
static MonstersMap* all_monsters[max_stage_type];

static uint32_t quality_drop_odds[max_stage_type][max_drop_lv][max_quality];

SpecialDropsMrg special_drop_mrg;



//static int pack_item_drop(Player* p, uint32_t item_id, uint8_t* buf, const KfAstar::Points* points, userid_t owner_uid= 0);

inline uint32_t get_quality_drop_odds(uint32_t drop_lv, uint32_t  quality_lv, uint32_t stage_mode)
{
	if (drop_lv == 0 || stage_mode == 0) {
		WARN_LOG("get_quality_drop_odds error:[%u %u]",drop_lv, stage_mode);
		return 0;
	}
	TRACE_LOG("[%u %u %u]:[%u]", stage_mode, drop_lv, quality_lv, quality_drop_odds[stage_mode - 1][drop_lv - 1][quality_lv]);
	return quality_drop_odds[stage_mode - 1][drop_lv - 1][quality_lv];
}

static bool check_item_exits(uint32_t item_id)
{
	const GfItem* pItem = items->get_item(item_id);
	if (!pItem) {
		ERROR_LOG("check item no exits[%u]", item_id);
		return false;
	}
	return true;
}


inline bool is_monsters(uint32_t role_type)
{
	if (role_type >= 11001 && role_type <= 19000) {
		return true;
	}
	return false;
}

//------------------------------------------------------------------
/*
static void set_monster_assign_attr(Player* mon, Player* player, uint32_t radio)
{
	if (!mon || !player) {
		return;
	}

	float radio2 = 1.0;
	if (mon->mon_info->type != 1) {
		radio2 = 0.7;
	}

	mon->lv = player->lv;
	mon->atk = player->atk * radio * radio2;
	mon->hp = player->hp * radio * radio2;
	mon->mp = player->mp * radio * radio2;
	mon->maxhp = player->hp * radio * radio2;
	mon->maxmp = player->mp * radio * radio2;

	if (mon->mon_info->type == 1) {
		mon->exp = 640 + player->lv * 32;
	} else {
		mon->exp = 40 + player->lv * 2;
	}
	
	mon->hit_rate = player->hit_rate;
	mon->def_rate = 1;
	mon->def_threshold_value = player->def_threshold_value;
	mon->dodge_rate = player->dodge_rate;

}
*/
void set_monster_attr(Player* p, const monster_t* mon, Battle* btl, uint16_t mon_lv)
{
	p->role_type   = mon->id;
	memcpy(p->nick, mon->name, max_nick_size);
	p->lv          = mon->lv;

	p->hp          = mon->cur_hp != 0 ?  mon->cur_hp : mon->hp;
	p->atk         = mon->atk;
	p->exp         = mon->exp;
	p->maxhp       = mon->hp;
	p->mp          = mon->mp;
	p->maxmp       = mon->mp;

	p->hit_rate    = mon->hit_rate;
	p->def_rate    = 1 - mon->def_rate;
	p->def_threshold_value = mon->def_threshold_value;
	p->atk_threshold_value = mon->atk_threshold_value;
	p->dodge_rate  = 1 - mon->dodge_rate;
	p->skill_point = mon->sp;
	p->undead_flag = mon->undead == 0 ? false : true;
	p->mon_info    = mon;
	p->team = mon->team;
	p->speed = mon->spd;
	p->set_region(mon->len, mon->width, mon->height);
	p->set_battle_status(player_in_battle);
	//p->set_pos(Vector3D(it->x, it->y));
	p->btl = btl;
	p->rigidity_factor = mon->rigidity_factor;
	p->dead_call_summon = mon->dead_call_summon;
	p->suicide_call_summon = mon->suicide_call_summon;
		
	if (mon->id < 19001) {
		p->team = mon->team;
	} else  if ( mon->team == barrier_team_1){
		p->team = barrier_team_1;
	} else {
		p->team = barrier_team;
	}	
	

	for (int i = 0; i != mon->skill_num; ++i) {
		player_skill_t skill(mon->skills[i].skill_id, mon->skills[i].lv);
		p->skills_map.insert(SkillMap::value_type(mon->skills[i].skill_id, skill));
	}
	
	//select ai
	if (mon->type == 1) {
		p->i_ai->select_ai(BOSS_AI, mon->id);
	} else if (mon->type == 4) {
		p->i_ai->select_ai(HIGH_LV_AI);
	} else if (mon->type == 2) {
		p->i_ai->select_ai(LOW_LV_AI);
	} else if (mon->type == 3) {
		p->i_ai->select_ai(MIDDLE_LV_AI);
	} else if (mon->type == 5){
		p->i_ai->select_ai(SPECIAL_MON_AI, mon->id);
	} else if (mon->type == 6) {
		p->i_ai->select_ai(BOSS_AI, mon->id);
	} else {
		p->i_ai->select_ai(NULL_AI);
	}
	
	if (mon->type != 1 && mon->long_range_atk == 1) {
		//printf("[%u],",mon->id);
		p->i_ai->select_ai(LONG_RANGE_ATK_AI);
	}

	TRACE_LOG("mon: id %u   diff %u %u", mon->id, mon->stage_diff, p->btl->difficulty());
	if (mon->stage_diff || mon->activity_stage || 
		(p->btl->difficulty() > 4 && p->btl->difficulty() < 11 && is_monsters(p->role_type))) {
		Player* player = btl->get_some_player();
		calc_mon_attr(0, p, player->lv, mon->stage_diff ? mon->stage_diff : btl->difficulty(), mon->attr_type ? mon->attr_type : 1);
		//set_monster_assign_attr(p, player, mon->radio);
	}
    if (btl->is_battle_tower()) {
		Player* player = btl->get_some_player();
		calc_mon_attr(0, p, mon_lv, mon->stage_diff ? mon->stage_diff : btl->difficulty(), mon->attr_type ? mon->attr_type : 4);
        p->lv = player->lv;
    }

    if (btl->is_battle_pvp()) {
        if (btl->pvp_lv() == pvp_monster_) {
            if (p->role_type == 11318 || p->role_type == 11320 || p->role_type == 13089) {
                mon_lv = btl->get_left_player_lv();
            } else if (p->role_type == 11319 || p->role_type == 11321 || p->role_type == 13090){
                mon_lv = btl->get_right_player_lv();
            }
            p->lv = mon_lv;
            //ERROR_LOG("SET pvp_monster_ [%u] [%u]", p->role_type, p->lv);
            if ( mon->type != NULL_AI ) {
                calc_mon_attr(0, p, mon_lv, 4, 3);
            }
        }
        if (btl->pvp_lv() == pvp_monster_game || btl->pvp_lv() == pvp_monster_practice) {
            Player *pvp_player = 0;
            if (p->team == 1) {
                pvp_player = btl->get_player_by_team(2);
            } else {
                pvp_player = btl->get_player_by_team(1);
            }
            mon_lv = pvp_player->lv;
            p->lv = mon_lv;
            //ERROR_LOG("SET pvp_monster_game [%u] [%u]", p->role_type, p->lv);
            if ( mon->type != NULL_AI ) {
                calc_mon_attr(0, p, mon_lv, 1, 4);
            }
        }
    }
	if( mon->aura_id)
	{
		add_player_aura(p, mon->aura_id, 0, false);
	}

	update_challenge_attr(p, btl);
}

int create_monsters(map_t* m, Battle* btl)
{
	map_t::MapMonVec* mons = m->mons;
	for (map_t::MapMonVec::iterator it = mons->begin(); it != mons->end(); ++it) {
		if (it->call_flg == 1) {
			continue;
		}

		const monster_t* mon = get_monster(it->id, btl->difficulty());
		Player* p = new Player;
		p->set_pos(Vector3D(it->x, it->y));

        set_monster_attr(p, mon, btl, m->mon_lv);
		p->do_enter_map(m);
	
		if( p->role_type == 13054 || p->role_type == 13071)//如果是宝典则加上一个BUFF
		{
			add_player_buff(p, 573, 0, 0, false);
		}
		//for challenge stage
		update_challenge_attr(p, btl);

	}

	//create rand monsters
	map_t::RanMonVec * r_mon = m->rand_mons;
	for (map_t::RanMonVec::iterator it = r_mon->begin(); it != r_mon->end(); ++it) {
		const rand_monster_t mon = *it;
		uint32_t odds = rand()%100;
		if (odds < mon.odds) {
			int x = 0;
			int y = 0;
			if ( mon.x_aur[0] >= mon.x_aur[1] ) {
				x = mon.x_aur[1];
			} else {
			    x =	rand() % (mon.x_aur[1] - mon.x_aur[0]) + mon.x_aur[0];
			}
			if ( mon.y_aur[0] >= mon.y_aur[1] ) {
				y = mon.y_aur[1];
			} else {
				y = rand() % (mon.y_aur[1] - mon.y_aur[0]) + mon.y_aur[0];
			}

			const monster_t* mon_attr = get_monster(it->id, btl->difficulty());
			Player* p = new Player;
			p->set_pos(Vector3D(x, y));
       		set_monster_attr(p, mon_attr, btl, m->mon_lv);
			p->do_enter_map(m);
			btl->raninfo.insert(Battle::RanMonInfo::value_type(mon_attr->id, mon_attr));
		}

	}
	

	return m->monsters.size();
}

int call_monster_to_map(map_t* m, Battle* btl, uint32_t mon_id, int x, int y, int team, int add_buff_id, int add_aura_id)
{
	if (!m || !btl) {
		return 0;
	}

	if (!mon_id) {
		map_t::MapMonVec* mons = m->mons;
		for (map_t::MapMonVec::iterator it = mons->begin(); it != mons->end(); ++it) {
			const monster_t* mon = get_monster(it->id, btl->difficulty());
			if (mon->type == 1) {
				continue;
			}
			mon_id = it->id;
			x = it->x;
			y = it->y;
			break;
		}
	}

	TRACE_LOG("call mons %u %u %u", mon_id, x, y);

	//create monster
	const monster_t* mon = get_monster(mon_id, btl->difficulty());
	Player* p = new Player;
	p->set_pos(Vector3D(x, y));
	set_monster_attr(p, mon, btl, m->mon_lv);
	p->monster_enter_map(m);
	if (p->role_type < 19001) {
		m->activate_monster(p);
	}
	p->team = team;

	if(add_buff_id > 0){
		add_player_buff(p, add_buff_id, 0);
	}

	if(add_aura_id > 0){
		add_player_aura(p, add_aura_id, 0);
	}


	return 0;
}

/*bool summon_skill_disabled(const summon_mon_t* p_info, uint32_t skill_id) 
{
	if (!p_info) {
		return false;
	}

	for (uint32_t i = 0; i < max_summon_skills; i++) {
		if (p_info->disable_skills[i] == skill_id) {
			TRACE_LOG("[%u]",skill_id);
			return true;
		}
	}

	return false;
}*/

Player* create_summon(Player* owner) 
{
	const monster_t* mon = get_monster(owner->summon_info.mon_type);
	if (owner->btl->stage_->id == 986) {
		return 0;
	}
	Player* summon = new Player;
	
    summon->role_tm     = owner->summon_info.mon_tm;
	summon->role_type   = owner->summon_info.mon_type;
	memcpy(summon->nick, owner->summon_info.nick, max_nick_size);
	summon->lv          = owner->summon_info.mon_lv;
	summon->exp		    = owner->summon_info.mon_exp;

	summon->hp          = mon->hp;
	summon->maxhp       = mon->hp;
	summon->mp          = mon->mp;
	summon->maxmp       = mon->mp;
	summon->atk         = mon->atk;
	summon->hit_rate    = mon->hit_rate;

	summon->mon_info    = mon;
	summon->set_region(mon->len, mon->width, mon->height);
	summon->set_battle_status(player_in_battle);
	summon->set_pos(Vector3D(owner->pos().x(), owner->pos().y()));
    summon->speed       = mon->spd;

	if (owner->btl->pvp_lv() == pvp_dragon_ship) {
		summon->speed = 100;
	}

	summon->btl         = owner->btl;
	summon->team 		= owner->team;
	summon->cur_map 	= owner->cur_map;
	
	summon->summon_info.owner = owner;
	summon->summon_info.mon_type = summon->role_type;
	summon->summon_info.mon_tm = owner->summon_info.mon_tm;
	//summon->summon_info.fight_value = owner->summon_info.fight_value;
	summon->summon_info.max_fight_value = 100;
	summon->summon_info.fight_value = 100;
	summon->summon_info.anger_value = 0;
	summon->summon_info.active_flag = false;	
	summon->summon_info.mon_lv = owner->summon_info.mon_lv;
	summon->summon_info.attr_type = owner->summon_info.attr_type;
	summon->summon_info.attr_per = owner->summon_info.attr_per;
	
	summon->summon_info.trigger_skill_id = 0;	

	summon->lucky_cnt = owner->lucky_cnt;

	for (int i = 0; i != owner->summon_info.skills_cnt; ++i) {
		if (i >= max_summon_skills) {
			break;
		}

		summon_skill_info_t* p_skill = &(owner->summon_info.skills[i]);
		if (p_skill->used_flag == 0) {
			continue;
		}

		player_skill_t skill(p_skill->skill_id, p_skill->skill_lv);
		summon->skills_map.insert(SkillMap::value_type(p_skill->skill_id, skill));
	}

	summon->calc_summon_attr();
    summon->mp = summon->summon_info.fight_value;
	if (owner->btl->pvp_lv() == pvp_dragon_ship && owner->btl->stage()->id == 734) {
		summon->i_ai->select_ai(SUMMON_MON_AI, 3);
	} else {
		summon->i_ai->select_ai(SUMMON_MON_AI, summon->role_type);
	}
		
	return summon;
}


Player* create_numen(Player* owner) 
{
	if (!(owner->numen_info.numen_id)) {
		return 0;
	}
	//const monster_t* mon = get_monster(owner->numen_info.numen_id);
	
	Player* numen = new Player;

	numen->numen_info.owner = owner;
	numen->role_type   = owner->numen_info.numen_id;
	memcpy(numen->nick, owner->numen_info.numen_nick, max_nick_size);

	return numen;
}

//------------------------------------------------------------------

const monster_t* get_monster(uint32_t id, uint32_t diffculty)
{
	MonstersMap::iterator it = all_monsters[diffculty - 1]->find(id);
	if (it == all_monsters[diffculty - 1]->end()) {
		return 0;
	}
	return &(it->second);
}

void monster_killed_task(Player* p, Player* killer)
{
	if ( killer->task_mon_map.size() > 0 ) {
	}
}

int monster_task_drop(Player* p, Player* killer, uint8_t* buf, const KfAstar::Points* points, uint32_t& drop_num)
{
	int idx = 0;
	if (!killer->stage_task_list.empty()) {
		TaskItemList::iterator it = killer->stage_task_list.begin();
		while (it != killer->stage_task_list.end()) {
			uint32_t rand_odds = rand() % (drop_odds_multiple * 100);
			if (rand_odds <= it->drop_odds) {
				idx += pack_item_drop(p, it->item_id, buf, points, killer->id);
				drop_num++;
				it->rest_cnt --;
				if (it->rest_cnt == 0) {
					killer->stage_task_list.erase(it++);
				}
			}
			++it;
		}
	}

	if ( killer->task_item_map.size() > 0 ) {
		TaskItemMap::iterator beg = killer->task_item_map.lower_bound(p->mon_info->id);
		TaskItemMap::iterator end = killer->task_item_map.upper_bound(p->mon_info->id);
		while ( beg != end ) {
			uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
			TRACE_LOG("task:rand_odds[%u]", rand_odds);
			if ( rand_odds <= beg->second.drop_odds ) {
				idx += pack_item_drop(p, beg->second.item_id, buf, points, killer->id);
				drop_num++;
				
				beg->second.rest_cnt--;
				TRACE_LOG("task item rest cnt:[%u %u]",killer->id, beg->second.rest_cnt);
				if (beg->second.rest_cnt == 0) {
					TRACE_LOG("monster_task_drop %u %u", beg->second.item_id, beg->second.monster_id);
					killer->task_item_map.erase(beg++);
					continue;
				}
			}
			++beg;
		}

		//temporarily drop at date.20101111 version
		TRACE_LOG("temporarily drop %u %u", p->is_boss(), (uint32_t)killer->btl->players_.size());
		if (p->is_boss() && is_valid_uid(killer->id) && killer->btl->players_.size() > 1) {
			PlayerVec::iterator it = killer->btl->players_.begin();
			PlayerVec::iterator it1 = it;
			PlayerVec::iterator it2 = ++it;
			TRACE_LOG("temporarily drop %u %u", (*it1)->show_state, (*it2)->show_state);
			if ((*it1)->show_state && (*it2)->show_state && (*it1)->show_state != (*it2)->show_state) {
				TaskItemMap::iterator beg = killer->task_item_map.lower_bound(1);
				TaskItemMap::iterator end = killer->task_item_map.upper_bound(1);
				while ( beg != end ) {
					uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
					TRACE_LOG("task boss:rand_odds[%u]", rand_odds);
					if ( rand_odds <= beg->second.drop_odds ) {
						idx += pack_item_drop(p, beg->second.item_id, buf, points, killer->id);
						drop_num++;
						
						beg->second.rest_cnt--;
						TRACE_LOG("task item rest cnt:[%u %u]",killer->id, beg->second.rest_cnt);
						if (beg->second.rest_cnt == 0) {
							killer->task_item_map.erase(beg++);
							continue;
						}
					}
					++beg;
				}
			}
		}
	}

	return idx;
}

int monster_unique_items_drop(Player* p, Player* killer, uint8_t* buf, const KfAstar::Points* points, uint32_t& drop_num)
{
	int idx = 0;
	const item_drop_t* p_drops = &(p->mon_info->drops);
	if ( p_drops->unique_items_drop_num > 0 ) {
	//	uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
		for (uint32_t i = 0; i < p_drops->unique_items_drop_num; i++) {
			const unique_item_drop_t & unqiue_drop = p_drops->unique_items_drop[i];
			if ( (!unqiue_drop.role_type || unqiue_drop.role_type == killer->role_type) 
					&& !taomee::test_bit_on(killer->unique_item_bit, unqiue_drop.bit_pos)
					&& judge_luck(unqiue_drop.drop_odds_lower,
					   			  unqiue_drop.drop_odds_upper,
								  drop_odds_multiple * unqiue_drop.max_odds,
								  killer->lucky_cnt)) {
				uint32_t item_id = unqiue_drop.item_id; 
				if ( !item_id ) {
					break;
				}
				TRACE_LOG("quality unique item_id[%u][%u]", item_id, p->id);
				idx += pack_item_drop(p, item_id, buf, points, killer->id);
				drop_num++;
				break;
			}
		}
	}	

	return idx;
}

int monster_buff_drop(Player* p, Player* killer)
{
	const item_drop_t* p_drops = &(p->mon_info->drops);
	if ( p_drops->buffs_drop_num > 0 ) {
	//	uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
	//	TRACE_LOG("buffs:rand_odds[%u]", rand_odds);
		for (uint32_t i = 0; i < p_drops->buffs_drop_num; i++) {
		//	if ( rand_odds >= p_drops->buff_drop[i].drop_odds_lower && 
		//			rand_odds <= p_drops->buff_drop[i].drop_odds_upper ) {
				//printf("drop buff[%u %u]\n", killer->id, p_drops->buff_drop[i].buff_id);
			if (judge_luck(p_drops->buff_drop[i].drop_odds_lower,
						   p_drops->buff_drop[i].drop_odds_upper,
					   	   drop_odds_multiple * p_drops->buff_drop[i].max_odds)) {	   
				add_player_buff(killer, p_drops->buff_drop[i].buff_id, 0);
				break;
			}
		}
	}
	
	return 0;
}

int monster_material_drop(Player* p, uint8_t* buf, const KfAstar::Points* points, uint32_t& drop_num)
{
	int idx = 0;
	const item_drop_t* p_drops = &(p->mon_info->drops);
	if ( p_drops->material_drop_num > 0 ) {
		uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
		//uint32_t rand_odds = rand() % drop_odds_multiple;
		TRACE_LOG("material:rand_odds[%u]", rand_odds);
		for (uint32_t i = 0; i < p_drops->material_drop_num; i++) {
			if ( rand_odds >= p_drops->material_drop[i].drop_odds_lower && 
					rand_odds <= p_drops->material_drop[i].drop_odds_upper ) {
				idx += pack_item_drop(p, p_drops->material_drop[i].item_id, buf, points);
				drop_num++;
                //stat_buf[4]++;
				break;
			}
		}
	}
	
	return idx;
}


bool mechanism_drop_to_all_players(Player* p_monster)
{
	do_stat_item_log_2(stat_log_open_box, p_monster->role_type - 39000, 0, 1);	
	vector<uint32_t> item_ids;

	const item_drop_t* p_drops = &(p_monster->mon_info->drops);	
	mechanism_drop_data* p_data = p_drops->p_data;
	if(p_data)
	{
		for(uint32_t j =0; j< p_data->repeat_count; j++)
		{
			uint32_t rand_odds = rand() % 1000;		
			for(uint32_t i =0; i< p_data->monster_drops.size(); i++)
			{
				if( rand_odds >= p_data->monster_drops[i].begin_odds &&
					rand_odds < p_data->monster_drops[i].end_odds)
				{
					p_monster->call_monster(p_data->monster_drops[i].monster_id,  p_monster->pos().x(), p_monster->pos().y());
					do_stat_item_log_2(stat_log_open_box_monster, p_data->monster_drops[i].monster_id - 11400, 0, 1);
				}
			}
			for(uint32_t i =0; i< p_data->item_drops.size(); i++)
			{
				if( rand_odds >= p_data->item_drops[i].begin_odds && 
					rand_odds < p_data->item_drops[i].end_odds )
				{
					item_ids.push_back(p_data->item_drops[i].item_id);	
				}
			}
		}
	}	

	if( item_ids.size() > 0)
	{
		int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
		uint32_t count = item_ids.size();
		pack(pkgbuf, count, idx);
		for(uint32_t i=0; i< item_ids.size(); i++)
		{
			ItemDrop itm_drop(item_ids[i], 0);
			itm_drop.set_region(100,100);
			Vector3D pt(p_monster->pos().x(), p_monster->pos().y() + 20);
			itm_drop.set_pos(pt);
			p_monster->cur_map->item_drops.insert(ItemDropMap::value_type(itm_drop.id(),itm_drop));

			pack(pkgbuf, itm_drop.id(), idx);
			pack(pkgbuf, item_ids[i], idx);
			pack(pkgbuf, itm_drop.pos().x(), idx);
			pack(pkgbuf, itm_drop.pos().y(), idx);
		}
		init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_monster_item_drop, idx - sizeof(btl_proto_t));
		init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
		p_monster->btl->send_to_btl(pkgbuf, idx, 0, 0);
	}	
	return true;
}

int monster_item_drop(Player* p, uint8_t* buf, const KfAstar::Points* points, uint32_t& drop_num)
{
	int idx = 0;
	const item_drop_t* p_drops = &(p->mon_info->drops);
	if ( p_drops->items_drop_num > 0 ) {
		uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
		TRACE_LOG("items:rand_odds[%u]", rand_odds);
		for (uint32_t i = 0; i < p_drops->items_drop_num; i++) {
			if ( rand_odds >= p_drops->items_drop[i].drop_odds_lower && 
					rand_odds <= p_drops->items_drop[i].drop_odds_upper ) {
				uint32_t item_id = items->get_drop_itemid(p_drops->items_drop[i].drop_lv); 
				TRACE_LOG("quality item_id[%u %u]", item_id, p_drops->items_drop[i].drop_lv);
				if ( !item_id ) {
					break;
				}
				idx += pack_item_drop(p, item_id, buf, points);
				drop_num++;
                /*if (item_id > 1300000 && item_id < 1400000) {
                    stat_buf[3]++;
                } else {
                    stat_buf[4]++;
                }*/
				break;
			}
		}
	}

	return idx;
}

int monster_lucky_drop(Player* p, uint8_t* buf, const KfAstar::Points* points, uint32_t& drop_num)
{
	int idx = 0;
	const item_drop_t* p_drops = &(p->mon_info->drops);
	if ( p_drops->lucky_drop_num > 0 ) {
		//uint32_t rand_odds = rand() % drop_odds_multiple;
		uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
		for (uint32_t i = 0; i < p_drops->lucky_drop_num; i++) {
		TRACE_LOG("lucky:rand_odds[%u]", rand_odds);
			if ( rand_odds >= p_drops->lucky_drop[i].drop_odds_lower && 
					rand_odds <= p_drops->lucky_drop[i].drop_odds_upper ) {
				idx += pack_item_drop(p, p_drops->lucky_drop[i].item_id, buf, points);
				drop_num++;
                //stat_buf[5]++;
				break;
			}
		}
	}

	return idx;
}

int monster_clothes_drop(Player* p, uint8_t* buf, const KfAstar::Points* points, uint32_t& drop_num)
{
	int idx = 0;
	const item_drop_t* p_drops = &(p->mon_info->drops);
	if ( p_drops->clothes_drop_num > 0 ) {
		uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
		TRACE_LOG("clothes:rand_odds[%u]", rand_odds);
		for (uint32_t i = 0; i < p_drops->clothes_drop_num; i++) {
			if ( rand_odds >= p_drops->clothes_drop[i].drop_odds_lower && 
					rand_odds <= p_drops->clothes_drop[i].drop_odds_upper ) {
				uint32_t drop_lv = p_drops->clothes_drop[i].drop_lv;
				uint32_t drop_id = p_drops->clothes_drop[i].drop_id;
				uint32_t quality = 0;
				uint32_t quality_odds = rand() % ( drop_odds_multiple * 100 );
				TRACE_LOG("quality:rand_odds[%u]",quality_odds);
				for (uint32_t j = 0; j < max_quality; j++) {
					//uint32_t odds_lower = ( j == 0 ) ? 0 : get_quality_drop_odds(drop_lv, j - 1, p->btl->difficulty());
					//uint32_t odds_upper = get_quality_drop_odds(drop_lv, j, p->btl->difficulty());
					uint32_t odds_lower = ( j == 0 ) ? 0 : get_quality_drop_odds(drop_id, j - 1, p->btl->difficulty());
					uint32_t odds_upper = get_quality_drop_odds(drop_id, j, p->btl->difficulty());
					TRACE_LOG("quality info[%u %u %u]",drop_lv, odds_lower, odds_upper);
					if ( quality_odds >= odds_lower && quality_odds < odds_upper ) {
						quality = j + 1;
						break;
					}
				}
				if ( !quality ) {
					break;
				}

				uint32_t item_id = items->get_drop_clothesid(drop_lv, quality); 
				TRACE_LOG("quality item_id[%u %u %u]", item_id, drop_lv, quality);
				if ( !item_id ) {
					break;
				}
				idx += pack_item_drop(p, item_id, buf, points);
				drop_num++;
                /*if (quality < 4) {
                    stat_buf[quality - 1]++;
                }*/
				break;
			}
		}
	}
	
	return idx;
}

int monster_holiday_drop(Player* p, uint8_t* buf, const KfAstar::Points* points, uint32_t& drop_num)
{
	int idx = 0;
	for (uint32_t i = 0; i < max_holiday_drop_type; i++) {
		holiday_drop_t* p_holiday_drop = &(holiday_drop_arr[i]);
		if (!(p_holiday_drop->cnt)) {
			break;
		}
		if (p_holiday_drop->start_flg) {
			for (uint32_t j = 0; j < p_holiday_drop->cnt; j++) {
				holiday_drop_item_t* p_drop_item = &(p_holiday_drop->drop_item[j]);
				//test level
				if (p->mon_info->lv < p_drop_item->mon_lv[0]
					|| p->mon_info->lv > p_drop_item->mon_lv[1]) {
					continue;
				}
				//test odds
				uint32_t rand_odds = rand() % 100;
				if (rand_odds >= p_drop_item->drop_odds) {
					continue;
				}
				
				TRACE_LOG("quality unique item_id[%u][%u]", p_drop_item->itemid, p->id);
				idx += pack_item_drop(p, p_drop_item->itemid, buf, points);
				drop_num++;
			}
		}	
	}

	return idx;
}

void monster_drop_amber_to_player(Player* killer, int x, int y)
{
	map_t *map = killer->cur_map;
	uint32_t item_id = 0;
	switch(killer->show_state)
	{
		case 1:
		{
			item_id = 1500303;
		}
		break;

		case 2:
		{
			item_id = 1500304;
		}
		break;

		case 3:
		{
			item_id = 1500305;
		}
		break;

		default:
			return;
		break;
	}

	if( rand() % 100 >= 15 ){
		return ;
	}

	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	//const KfAstar::Points* points = map->path->find_surrounding_points(KfAstar::Point(x, y));
	const KfAstar::Points* points = NULL;
	KfAstar* p_star = killer->btl->get_cur_map_path( map );
	if(p_star)
	{
		points = p_star->find_surrounding_points(KfAstar::Point(x, y));
	}


	pack(pkgbuf, 1, idx);
	idx += pack_item_drop(map, item_id, pkgbuf + idx, points);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_monster_item_drop, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	send_to_player(killer, pkgbuf, idx, 0);
}

void monster_drop_item_to_player(Player* p, Player* killer)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t) + 4;
	uint32_t drop_num = 0;
	
	//const KfAstar::Points* points = p->cur_map->path->find_surrounding_points(KfAstar::Point(p->pos().x(), p->pos().y()));
	const KfAstar::Points* points = NULL;
	KfAstar* p_star = killer->btl->get_cur_map_path( p->cur_map );
	if(p_star)
	{
		points = p_star->find_surrounding_points(KfAstar::Point(p->pos().x(), p->pos().y()));
	}

	idx += monster_task_drop(p, killer, pkgbuf + idx, points, drop_num);
	idx += monster_unique_items_drop(p, killer, pkgbuf + idx, points, drop_num);

		//stage_score
	killer->score.all_drop_cnt += drop_num;

	//send to map
	int idx_tmp = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, drop_num, idx_tmp);
	TRACE_LOG("drop_num =[%u]",drop_num);
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), p->id, cli_proto_monster_item_drop, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	if (!is_valid_uid(killer->id)) {
		killer->cur_map->send_to_map(pkgbuf, idx);
	} else {
		send_to_player(killer, pkgbuf, idx, 0);
	}
}

int pack_temp_drop(Player* p, uint8_t* buf, const KfAstar::Points* points, uint32_t& drop_num)
{
	int idx = 0;
	if (p->btl->stage()->id == 933 && p->mon_info->id == 19144) {
		time_t  timeval = get_now_tv()->tv_sec;
		tm* tm_ptr = localtime(&timeval);
		uint32_t hour = tm_ptr->tm_hour;
		uint32_t week = tm_ptr->tm_wday;

		
/*		uint32_t rand_odds = rand() % 11;
		if (rand_odds == 0 ) {
			rand_odds = 8;
		}
		for (uint32_t i = 0; i < rand_odds; i++) {
			idx += pack_item_drop(p, 1500359, buf + idx, points);
			drop_num++;
		}*/
		//1500359
		
		if ((week > 0 && week < 5 &&
			((hour >= 11 && hour < 13) ||
			  (hour >= 19 && hour < 21))) ||
			  
			  ((week > 4 || week== 0) &&
			((hour >= 9 && hour < 11) ||
			  (hour >= 14 && hour < 16) ||
			  (hour >= 19 && hour < 21)))){
			  uint32_t rand_odds = rand() % 10000;
			  if (rand_odds < 3000) { //3000) {
			  //1500357
			  	idx += pack_item_drop(p, 1500357, buf + idx, points);
			  	drop_num++;
			  }
			  rand_odds = rand() % 10000;
			  if (rand_odds < 1) { //1) {
			  //1500358
			  	idx += pack_item_drop(p, 1500358, buf + idx, points);
			  	drop_num++;
			  } 
		}
		
	}
	return idx;
}


bool judge_if_stage_can_drop(Player* p, Player* killer)
{
	if (p->btl->stage()->id == 926 ||
		p->btl->stage()->id == 927 ||
		p->btl->stage()->id == 928 ||
		p->btl->stage()->id == 929 ||
		p->btl->stage()->id == 930 ||
		p->btl->stage()->id == 932) {
		
        if (killer->is_have_bless_buff(1012) || 
            killer->is_have_bless_buff(1013) || 
            killer->is_have_bless_buff(1014)) {
            return true;
        }

		time_t  timeval = get_now_tv()->tv_sec;
		tm* tm_ptr = localtime(&timeval);
		uint32_t hour = tm_ptr->tm_hour;
		//uint32_t week = tm_ptr->tm_wday;
		//if ((hour >= 12 && hour < 15) ||
		//	  (hour >= 19 && hour < 21)) {
		if (hour >=12 && hour < 21) {
			  return true;
		}
		
		return false;
	}

	return true;
	
}

void monster_drop_item(Player* p, Player* killer)
{
	TRACE_LOG("monster[%u] drop item", p->mon_info->id);
	const item_drop_t* p_drops = &(p->mon_info->drops);
	
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t) + 4;
	uint32_t drop_num = 0;
    uint32_t stat_buf[6] = {0};
	vector<uint32_t>  roll_item_ids;

	if (!judge_if_stage_can_drop(p, killer)) {
		return ;
	}
	//find drop point
	//const KfAstar::Points* points = p->cur_map->path->find_surrounding_points(KfAstar::Point(p->pos().x(), p->pos().y()));
	KfAstar::Points* points = NULL;
	KfAstar* p_star = killer->btl->get_cur_map_path( p->cur_map );
	if(p_star)
	{
		points = (KfAstar::Points*)p_star->find_surrounding_points(KfAstar::Point(p->pos().x(), p->pos().y()));
	}

	//HARD CODE TEMPORARILY:  test special drop 
	if (p->mon_info->id == 13085 || p->mon_info->id == 13086) {
		time_t  timeval = get_now_tv()->tv_sec;
		tm* tm_ptr = localtime(&timeval);
		uint32_t hour = tm_ptr->tm_hour;

		if (hour < 17 && hour >= 14) {
			if (judge_luck(0, 30, 100)) {
				if (p->mon_info->id == 13085) {
					idx += pack_item_drop(p, 1700019, pkgbuf + idx, points);
				} else {
					idx += pack_item_drop(p, 1700020, pkgbuf + idx, points);
				}
				drop_num++;
			}
		}
	}

	idx += special_drop_mrg.pack_special_drop(p, killer, points, pkgbuf + idx, drop_num);
	if ( p_drops->material_drop_num > 0 ) {
		for (uint32_t i = 0; i < p_drops->material_drop_num; i++) {
			const other_drop_t & material_drop = p_drops->material_drop[i];
			if (judge_luck(material_drop.drop_odds_lower,
						   material_drop.drop_odds_upper,
						   drop_odds_multiple * material_drop.max_odds,
						   killer->lucky_cnt)) {
				if (p->role_type == 13049 && material_drop.item_id == 1500265) {
						time_t  timeval = get_now_tv()->tv_sec;
						tm* tm_ptr = localtime(&timeval);
						uint32_t month = tm_ptr-> tm_mon + 1;
						uint32_t day = tm_ptr-> tm_mday;
						//uint32_t weekday = tm_ptr-> tm_wday;
						char daystr1[40] = {0};
						char daystr2[40] = {0};
						time_t tmptime1;
						time_t tmptime2;

						sprintf(daystr1, "2010-%02d-%02d 19:00:00", month, day);
						sprintf(daystr2, "2010-%02d-%02d 20:59:59", month, day);
						date2timestamp(daystr1, "%Y-%m-%d %H:%M:%S", tmptime1);
						date2timestamp(daystr2, "%Y-%m-%d %H:%M:%S", tmptime2);
						TRACE_LOG("now:%u %s:%u %s:%u", (uint32_t)timeval, daystr1, (uint32_t)tmptime1, daystr2, (uint32_t)tmptime2);
						if (timeval < tmptime1 || timeval > tmptime2) {
							continue;
						}
					
				}

				//特殊物品特殊处理
				if ( (material_drop.item_id == 1500339 
							|| material_drop.item_id == 1300313) 
						&& p->btl->stage()->id == 925) {//妖桃之心
					//ERROR_LOG("T>>>>>>>><<<<<<<! ");
					int idx = sizeof(btl_proto_t);
					pack_h(pkgbuf, material_drop.item_id, idx);
					init_btl_proto_head(pkgbuf, btl_extra_item_drop_proc, idx);
					killer->cur_map->send_to_map(pkgbuf, idx);
					do_stat_item_log_5(stat_log_peach_got, 0, 0, 1);
					continue;
				}

				idx += pack_item_drop(p, material_drop.item_id, pkgbuf + idx, points);
				drop_num++;
                stat_buf[4]++;
				break;
			}
		}
	}
	
	//test lucky drop
	if ( p_drops->lucky_drop_num > 0 ) {
		//uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
		for (uint32_t i = 0; i < p_drops->lucky_drop_num; i++) {
		//	uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
		//	TRACE_LOG("lucky:rand_odds[%u]", rand_odds);
			const other_drop_t & luck_drop = p_drops->lucky_drop[i];
			if (judge_luck(luck_drop.drop_odds_lower,
						   luck_drop.drop_odds_upper,
						   drop_odds_multiple * luck_drop.max_odds,
						   killer->lucky_cnt)) {

				uint32_t item_id = luck_drop.item_id;
				const GfItem* pItem = items->get_item(item_id);
				//如果装备品质大于等于3则开始roll,同时在组队模式
				if( pItem->quality_lv >=2  && killer->btl->is_team_btl())
				{
					roll_item_ids.push_back(item_id);
					continue;
				}
				idx += pack_item_drop(p, luck_drop.item_id, pkgbuf + idx, points);
				drop_num++;
                stat_buf[5]++;
				//break;
				continue;
			}
		}
	}

	if (p_drops->card_drop_num > 0) {
		for (uint32_t i = 0; i < p_drops->card_drop_num; i++) {
			const card_drop_t & card_drop = p_drops->card_drop[i];
			if (judge_luck(card_drop.drop_odds_lower,
						   card_drop.drop_odds_upper,
						   drop_odds_multiple * card_drop.max_odds,
						   killer->lucky_cnt)) {

				//uint32_t item_id = card_drop.item_id;

				//const GfItem* pItem = items->get_item(item_id);
				idx += pack_item_drop(p, card_drop.item_id, pkgbuf + idx, points);
				drop_num++;
                stat_buf[5]++;
				//break;
				continue;
			}
		}
	}

	//test clothes drop
	if ( p_drops->clothes_drop_num > 0 ) {
	//	uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
	//	TRACE_LOG("clothes:rand_odds[%u]", rand_odds);
		for (uint32_t i = 0; i < p_drops->clothes_drop_num; i++) {
			const clothes_drop_t & clothes_drop = p_drops->clothes_drop[i];
			if (judge_luck(clothes_drop.drop_odds_lower,
						   clothes_drop.drop_odds_upper,
						   drop_odds_multiple * 100,
						   killer->lucky_cnt)) {
				uint32_t drop_lv = clothes_drop.drop_lv;
				uint32_t drop_id = clothes_drop.drop_id;
				uint32_t quality = 0;

				uint32_t quality_odds = rand() % ( drop_odds_multiple * 100 );
				TRACE_LOG("quality:rand_odds[%u]",quality_odds);

				for (uint32_t j = 0; j < max_quality; j++) {

					uint32_t odds_lower = ( j == 0 ) ? 0 : get_quality_drop_odds(drop_id, j - 1, p->btl->difficulty());
					uint32_t odds_upper = get_quality_drop_odds(drop_id, j, p->btl->difficulty());

					TRACE_LOG("quality info[%u %u %u]",drop_id, odds_lower, odds_upper);

					if (judge_luck(odds_lower,
							  	   odds_upper,
								   drop_odds_multiple * 100,
								   killer->lucky_cnt) ) {

						quality = j + 1;
						break;
					}
				}

				if ( !quality ) {
					break;
				}

				uint32_t item_id = items->get_drop_clothesid(drop_lv, quality); 
				TRACE_LOG("quality item_id[%u %u %u]", item_id, drop_lv, quality);
				if ( !item_id ) {
					break;
				}
				const GfItem* pItem = items->get_item(item_id);

				if( pItem->quality_lv >=2  && killer->btl->is_team_btl())//如果装备品质大于等于3则开始roll,同时在组队模式
				{
					roll_item_ids.push_back(item_id);
					continue;
				}
				idx += pack_item_drop(p, item_id, pkgbuf + idx, points);
				drop_num++;
                if (quality < 4) {
                    stat_buf[quality - 1]++;
                }
				break;
			}
		}
	}
	
	//test items drop
	if ( p_drops->items_drop_num > 0 ) {
//		uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
		//TRACE_LOG("items:rand_odds[%u]", rand_odds);
		for (uint32_t i = 0; i < p_drops->items_drop_num; i++) {
			const items_drop_t & item_drop = p_drops->items_drop[i];
			if (judge_luck(item_drop.drop_odds_lower,
						   item_drop.drop_odds_upper,
						   drop_odds_multiple * item_drop.max_odds,
						   killer->lucky_cnt)) {
				uint32_t item_id = items->get_drop_itemid(item_drop.drop_lv); 
				TRACE_LOG("quality item_id[%u %u]", item_id, item_drop.drop_lv);
				if ( !item_id ) {
					break;
				} 

				idx += pack_item_drop(p, item_id, pkgbuf + idx, points);
				drop_num++;
                if (item_id > 1300000 && item_id < 1400000) {
                    stat_buf[3]++;
                } else {
                    stat_buf[4]++;
                }
				break;
			}
		}
	}

	//test buffs drop
	if ( p_drops->buffs_drop_num > 0 ) {
//		uint32_t rand_odds = rand() % ( drop_odds_multiple * 100 );
	//	TRACE_LOG("buffs:rand_odds[%u]", rand_odds);
		for (uint32_t i = 0; i < p_drops->buffs_drop_num; i++) {
			const buff_drop_t & buff_drop = p_drops->buff_drop[i];
			if (judge_luck(buff_drop.drop_odds_lower,
						   buff_drop.drop_odds_upper,
						   drop_odds_multiple * buff_drop.max_odds)) {
				add_player_buff(killer, p_drops->buff_drop[i].buff_id, 0);
				break;
			}
		}
	}
	
	if (!(p->mon_info->no_world_drop)) {
		//test holiday drop
		for (uint32_t i = 0; i < max_holiday_drop_type; i++) {
			holiday_drop_t* p_holiday_drop = &(holiday_drop_arr[i]);
			if (!(p_holiday_drop->cnt)) {
				break;
			}

			if (p_holiday_drop->start_flg) {
				for (uint32_t j = 0; j < p_holiday_drop->cnt; j++) {
					holiday_drop_item_t* p_drop_item = &(p_holiday_drop->drop_item[j]);
					//test level
					if (p->mon_info->lv < p_drop_item->mon_lv[0]
						|| p->mon_info->lv > p_drop_item->mon_lv[1]) {
						continue;
					}

					if (judge_luck(0,
							   	   p_drop_item->drop_odds * drop_odds_multiple,
								   1000 * drop_odds_multiple)) {
								   
					
						TRACE_LOG("quality unique item_id[%u][%u]", p_drop_item->itemid, p->id);
						idx += pack_item_drop(p, p_drop_item->itemid, pkgbuf + idx, points);
						drop_num++;
					} else {
						continue;
					}
				}
			}	
		}
	}

	
	//stage_score
	killer->score.all_drop_cnt += drop_num;

	//send to map
	int idx_tmp = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, drop_num, idx_tmp);
	TRACE_LOG("drop_num =[%u]",drop_num);
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), p->id, cli_proto_monster_item_drop, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
		  
  	p->cur_map->send_to_map(pkgbuf, idx);
    //statistics boss drop item
    if (p->role_type > 13002 && p->role_type < 14000) {
        do_stat_item_log_4(stat_log_boss_drop, p->role_type - 11001, 0, stat_buf, sizeof(stat_buf));
    }

	Battle* btl = killer->btl;
	if (special_drop_mrg.is_in_time(6, get_now_tv()->tv_sec) && btl && btl->is_team_btl() && btl->difficulty() == 6 && p->is_boss()) {
	//史诗级别的boss怪都掉陆半仙宝珠
		roll_item_ids.push_back(1500574);
	}
	if (btl && btl->is_team_btl() && p->role_type == 13137) {
		roll_item_ids.push_back(1500578);
	}
	for(uint32_t i = 0; i< roll_item_ids.size(); i++)
	{
		battle_roll_data *data = new battle_roll_data();
		data->set_battle(btl);  
		data->add_map_player(killer->cur_map);
		data->add_player_roll_data(btl->get_next_roll_id(), roll_item_ids[i], 1);
		if(btl->m_roll_datas.size() == 0)
		{
			btl->add_roll_data(killer->cur_map, data);
		}
		else
		{
			btl->m_prepare_roll_datas.push_back(data);
		}
	}
	//////////////////////////////////////////////////
}

void treasure_box_drop_items(Player* p, uint32_t item_id, uint32_t item_cnt)
{
	//find drop point
	//const KfAstar::Points* points = p->cur_map->path->find_surrounding_points(KfAstar::Point(p->pos().x(), p->pos().y()));
	KfAstar::Points* points = NULL;
	KfAstar* p_star = p->btl->get_cur_map_path( p->cur_map );
	if(p_star)
	{
		points = (KfAstar::Points*)p_star->find_surrounding_points(KfAstar::Point(p->pos().x(), p->pos().y()));
	}

	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, item_cnt, idx);
	for (uint32_t i = 0; i < item_cnt; i++) {
		idx += pack_item_drop(p, item_id, pkgbuf + idx, points);
	}
	
	//send to map
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), p->id, cli_proto_monster_item_drop, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
		  
  	p->cur_map->send_to_map(pkgbuf, idx);
}

int pack_item_drop(Player* p, uint32_t item_id, uint8_t* buf, const KfAstar::Points* points, userid_t owner_uid) 
{
	int idx = 0;

	ItemDrop itm_drop(item_id, owner_uid);
	itm_drop.set_region(100,100);
	//set drop point
	int i = rand() % (points->size());
	Vector3D pt((*points)[i].x, (*points)[i].y);
	itm_drop.set_pos(pt);
	//itm_drop.set_pos_offset(p->pos(), offset_x, offset_y);
	p->cur_map->item_drops.insert(ItemDropMap::value_type(itm_drop.id(),itm_drop));
	
	idx += itm_drop.pack_item_info(buf + idx);
	return idx;
}

int pack_item_drop(map_t* map, uint32_t item_id, uint8_t* buf, const KfAstar::Points* points, userid_t owner_uid)
{
	int idx = 0;

	ItemDrop itm_drop(item_id, owner_uid);
	itm_drop.set_region(100,100);
	int i = rand() % (points->size());
	Vector3D pt((*points)[i].x, (*points)[i].y);
	itm_drop.set_pos(pt);
	map->item_drops.insert(ItemDropMap::value_type(itm_drop.id(),itm_drop));

	idx += itm_drop.pack_item_info(buf + idx);
	return idx;
}
//------------------------------------------------------------------
int
ItemDrop::pack_item_info(uint8_t* buf)
{
	int idx = 0;
	pack(buf, id(), idx);
	pack(buf, item_id_, idx);
	pack(buf, pos().x(), idx);
	pack(buf, pos().y(), idx);

	TRACE_LOG("[%u %u %u %u]",id(), item_id_, pos().x(), pos().y());
	return idx;
}

int
pack_mon_attr(uint32_t monid, void* buf)
{
	int idx = 0;
	

	const monster_t* mon = get_monster(monid);
	

	pack(buf, static_cast<uint32_t>(mon->lv), idx);

	pack(buf, static_cast<uint32_t>(mon->hp), idx);

	pack(buf, static_cast<uint32_t>(mon->mp), idx);

	pack(buf, static_cast<uint32_t>(mon->atk), idx);

	pack(buf, static_cast<uint32_t>(mon->hit_rate * 1000), idx);

	pack(buf, static_cast<uint32_t>((1 - mon->def_rate) * 1000), idx);
	//p->dodge_rate  = 1;
	pack(buf, static_cast<uint32_t>(1000), idx);

	return idx;
}


//------------------------------------------------------------------

static int load_monster_skills(xmlNodePtr cur, monster_t* mon)
{
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Skill"))) {
			uint32_t id;
			get_xml_prop(id, cur, "ID");
			if (g_skills->get_skill(id) == 0) {
				ERROR_LOG("cannot find skill %u", id);
				return -1;
			}

			mon->skills[mon->skill_num].skill_id = id;
			get_xml_prop_def(mon->skills[mon->skill_num].lv, cur, "Lv", 1);
			++(mon->skill_num);
		}
		cur = cur->next;	
	}

	return 0;
}

void clear_monsters(uint32_t type)
{
	if (all_monsters[type - 1]) {
		all_monsters[type - 1]->clear();
		all_monsters[type - 1] = 0;
	} else {
		all_monsters[type - 1] = new MonstersMap;
	}
}

void calc_mon_attr(monster_t* p_out_mon, Player* p_out_player, uint16_t mon_lv, uint32_t stage_diff, uint32_t attr_type)
{
	TRACE_LOG("%u %u %u %u", p_out_mon ? p_out_mon->id : 0, mon_lv, stage_diff, attr_type);
#define role_type_monkey_load_monsters 1
	uint16_t std_atk = calc_atk(calc_strength[role_type_monkey_load_monsters](mon_lv));
	uint16_t std_hp  = calc_hp[role_type_monkey_load_monsters](calc_body_quality[role_type_monkey_load_monsters](mon_lv));
	static uint16_t diff_atk_coef[11] = { 20, 12, 8, 6, 5, 4, 0, 0, 0, 0, 5};
	static uint16_t diff_hp_coef[11]  = { 4, 6, 8, 15, 18, 25, 0, 0, 0, 0, 20 };
	static uint16_t attr_atk_coef[5] = { 100, 170, 100, 250, 100};
	static uint16_t attr_hp_coef[5]  = { 100, 50, 200, 2000, 100};

	uint32_t atk = std_hp / diff_atk_coef[stage_diff - 1] * attr_atk_coef[attr_type - 1] / 100;
	uint32_t hp = 1.4 * std_atk * diff_hp_coef[stage_diff - 1] * attr_hp_coef[attr_type - 1] / 100;
	float def_rate = 0;
	float dodge_rate = 0.1;
	float hit_rate = 0.9;
	
	if (p_out_mon) {
		p_out_mon->lv = mon_lv;
		p_out_mon->atk = atk;
		p_out_mon->hp = hp;
		p_out_mon->def_rate = def_rate;
		p_out_mon->dodge_rate = dodge_rate;
		p_out_mon->hit_rate = hit_rate;
		
		TRACE_LOG("attr mon1:%u %u %u %u %u %u %u %u", 
			p_out_mon->id, stage_diff, std_hp, attr_type, 
			attr_atk_coef[attr_type - 1], calc_atk(calc_strength[role_type_monkey_load_monsters](40)),
			p_out_mon->atk, p_out_mon->hp);
		TRACE_LOG("%u %u %u", std_atk, diff_hp_coef[stage_diff - 1], attr_hp_coef[attr_type - 1] );
	}
	if (p_out_player) {
		p_out_player->lv = mon_lv;
		p_out_player->atk = atk;
		p_out_player->hp = hp;
		p_out_player->maxhp = hp;
		p_out_player->def_rate = def_rate;
		p_out_player->def_rate = 1 - p_out_player->def_rate;
		p_out_player->dodge_rate = dodge_rate;
		p_out_player->dodge_rate = 1 - p_out_player->dodge_rate;
		p_out_player->hit_rate = hit_rate;
		
		TRACE_LOG("attr mon2:%u %u %u %u %u %u %u %u", 
			p_out_player->id, stage_diff, std_hp, attr_type, 
			attr_atk_coef[attr_type - 1], calc_atk(calc_strength[role_type_monkey_load_monsters](40)),
			p_out_player->atk, p_out_player->hp);
	}
}

bool SpecialDropInfo::CanDropByMonster(Player * p)
{
	if (mon_id && p->role_type != mon_id) {
		return false;
	}	

	if (mon_type && p->mon_info->type != mon_type) {
		return false;
	}
	return true;
}


int SpecialDropsMrg::pack_special_drop(Player * p, Player* killer, const  KfAstar::Points * points, uint8_t * buf, uint32_t& drop_num)
{

	int idx = 0;
	uint32_t stage_id = p->btl->stage()->id;
	uint32_t difficulty = p->btl->difficulty();
	uint32_t mon_id = p->mon_info->id;
	std::map<uint32_t, SpecialDrop>::iterator it = special_drop_map.find(difficulty);
	if (it != special_drop_map.end()) {
		SpecialDrop* pDrop = &(it->second);
		std::vector<SpecialDropInfo>::iterator it2 = pDrop->drop_info_arr.begin();
		for (; it2 != pDrop->drop_info_arr.end(); ++it2) {
			SpecialDropInfo* pDropInfo = &(*it2);
			if ((!(pDropInfo->stage_id) || pDropInfo->stage_id == stage_id) &&
				(!(pDropInfo->mon_id) || pDropInfo->mon_id == mon_id)) {
				if (is_in_time(pDropInfo->drop_time_id, get_now_tv()->tv_sec) || killer->is_have_bless_buff(pDropInfo->buff_id) ) {
					uint32_t random_num = rand() % 10000;
					if (random_num < pDropInfo->drop_odds && pDropInfo->CanDropByMonster(p)) {

						//printf("%u %u %u\n", pDropInfo->item_id, pDropInfo->drop_odds, random_num);
						idx += pack_item_drop(p, pDropInfo->item_id, buf + idx, points);
						drop_num++;
					}
				}
			}
			
		}
	}
	return idx;
}

void SpecialDrop::init(const char* xml_file)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml_file);
	if(doc == NULL){
		return ;
	}
	
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		return ;
	}

	load_special_drop(root);
}

int SpecialDrop::load_special_drop(xmlNodePtr cur)
{
	drop_info_arr.clear();
	SpecialDropInfo drop_info;
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Stage"))) {
			get_xml_prop(drop_info.stage_id, cur, "ID");
			get_xml_prop(drop_info.mon_id, cur, "Monster");
			get_xml_prop(drop_info.drop_time_id, cur, "DropTimeID");
			get_xml_prop(drop_info.item_id, cur, "ItemID");
			get_xml_prop(drop_info.drop_odds, cur, "DropOdds");
			get_xml_prop_def(drop_info.buff_id, cur, "BuffID", 0);
			get_xml_prop_def(drop_info.mon_type, cur, "MonType", 0);

			drop_info_arr.push_back(drop_info);
		}
		cur = cur->next;	
	}
	return 0;
}

void SpecialDropsMrg::initSpecialDrops(uint32_t stage_mode)
{
	const char* drop_file = 0;
	SpecialDrop easy_stage_drop;
	if (stage_mode == stage_mode_easy) {
		drop_file = "./conf/stage_easy/special_drop.xml";
		easy_stage_drop.init(drop_file);
	}
	if (stage_mode == stage_mode_nor) {
		drop_file = "./conf/stage_normal/special_drop.xml";
		easy_stage_drop.init(drop_file);
	}
	if (stage_mode == stage_mode_hard) {
		drop_file = "./conf/stage_hard/special_drop.xml";
		easy_stage_drop.init(drop_file);
	}
	if (stage_mode == stage_mode_hell) {
		drop_file = "./conf/stage_hell/special_drop.xml";
		easy_stage_drop.init(drop_file);
	}
	if (stage_mode == stage_mode_tower) {
		drop_file = "./conf/stage_tower/special_drop.xml";
		easy_stage_drop.init(drop_file);
	}
	special_drop_map.insert(std::map<uint32_t, SpecialDrop>::
			value_type(stage_mode, easy_stage_drop));
}

/*
int SpecialDropsMrg::load_easy_special_drop(xmlNodePtr cur)
{
	return load_special_drop(cur, stage_mode_easy);
}

int SpecialDropsMrg::load_nor_special_drop(xmlNodePtr cur)
{
	return load_special_drop(cur, stage_mode_nor);
}

int SpecialDropsMrg::load_hard_special_drop(xmlNodePtr cur)
{
	return load_special_drop(cur, stage_mode_hard);
}

int SpecialDropsMrg::load_hell_special_drop(xmlNodePtr cur)
{
	return load_special_drop(cur, stage_mode_hell);
}

int SpecialDropsMrg::load_tower_special_drop(xmlNodePtr cur)
{
	return load_special_drop(cur, stage_mode_tower);
}
*/
void SpecialDropsMrg::init()
{
	special_drop_map.clear();
	initSpecialDrops(stage_mode_easy);
	initSpecialDrops(stage_mode_nor);
	initSpecialDrops(stage_mode_hard);
	initSpecialDrops(stage_mode_hell);
	initSpecialDrops(stage_mode_tower);
}
/**
  * @brief load stage configs from an xml file
  * @return 0 on success, -1 on error
  */
int load_monsters(xmlNodePtr cur, uint32_t type)
{
	// since we need to reload monsters.xml at run time, so we need to init 'all_stages'
	clear_monsters(type);

	// load monsters from xml file
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Monster"))) {
			uint32_t id;
			uint32_t long_range_atk = 0;
			get_xml_prop(id, cur, "ID");

			if (id >= monster_max_num) {
				ERROR_LOG("invalid monster id: %u", id);
				return -1;
			}

			MonstersMap::iterator it = all_monsters[type - 1]->find(id);
			if (it == all_monsters[type - 1]->end()) {
				monster_t mon_tmp;
				memset(&mon_tmp, 0x0, sizeof(mon_tmp));
				all_monsters[type - 1]->insert(MonstersMap::value_type(id, mon_tmp));
			}
			it = all_monsters[type - 1]->find(id);
			monster_t* mon = &(it->second);
			if (mon->id) {
				ERROR_LOG("duplicate monster id: %u", id);
				return -1;
			}
			mon->id = id;
			uint32_t attr_type = 0;
			get_xml_prop_raw_str(mon->name, cur, "Name");
			get_xml_prop_def(mon->type, cur, "Type", 0);
			get_xml_prop_def(attr_type, cur, "AttrType", 0);
			get_xml_prop_def(mon->flag, cur, "Flag", 0);
			get_xml_prop(mon->lv, cur, "Lv");
			get_xml_prop_def(long_range_atk, cur, "LRA", 0);
			get_xml_prop_def(mon->hp, cur, "Hp", 0);
			get_xml_prop_def(mon->cur_hp, cur, "Cur_Hp", 0);
			get_xml_prop_def(mon->mp, cur, "Mp", 0);
			get_xml_prop_def(mon->atk, cur, "Atk", 0);
			get_xml_prop_def(mon->atk_duration, cur, "AtkDuration", 0);
			get_xml_prop_def(mon->def_rate, cur, "DefRate", 0);
			get_xml_prop_def(mon->def_threshold_value, cur, "DefThreshold", 0);
			get_xml_prop_def(mon->atk_threshold_value, cur, "AtkThreshold", 0);
			get_xml_prop_def(mon->team, cur, "Team", monster_team);
			get_xml_prop_def(mon->dodge_rate, cur, "DodgeRate", 0);
			get_xml_prop_def(mon->hit_rate, cur, "HitRate", 0);
			get_xml_prop_def(mon->exp, cur, "YieldingExp", 0);
			get_xml_prop_def(mon->sp, cur, "SkillPoint", 0);
			get_xml_prop_def(mon->spd, cur, "Speed", 0);
			get_xml_prop_def(mon->visual_field, cur, "VisualField", 0);
			get_xml_prop_def(mon->len, cur, "Length", 0);
			get_xml_prop_def(mon->width, cur, "Width", 0);
			get_xml_prop_def(mon->height, cur, "Height", 0);
			
			get_xml_prop_def(mon->stage_diff, cur, "StageDiff", 0);
			get_xml_prop_def(mon->activity_stage, cur, "ActivityStage", 0);
			get_xml_prop_def(mon->undead, cur, "UnDead", 0);
			get_xml_prop_def(mon->aura_id, cur, "Aura_Id", 0);
			get_xml_prop_def(mon->no_world_drop, cur, "NoWorldDrop", 0);
			get_xml_prop_def(mon->rigidity_factor, cur, "RigidityFactor", 100);
			get_xml_prop_def(mon->dead_call_summon, cur, "DeadCallSummon", 0);
			get_xml_prop_def(mon->suicide_call_summon, cur, "SuicideCallSummon", 0);
			get_xml_prop_def(mon->makemoney, cur, "MakeMoney", 1);

			load_monster_skills(cur, mon);

			mon->long_range_atk = long_range_atk;
			mon->visual_field_sqrt = mon->visual_field;
			mon->visual_field *= mon->visual_field;
			mon->attr_type = attr_type;

			if (mon->attr_type && type != stage_mode_tower) {
			//new monster attr calc
				calc_mon_attr(mon, 0, mon->lv, type, mon->attr_type);
				
			}
			if ( load_item_drop(cur, &(mon->drops)) == -1) {
				return -1;
			}
		}
		cur = cur->next;
	}

	TRACE_LOG("============= Start Dumping Monsters =============");
	for (MonstersMap::iterator it = all_monsters[type - 1]->begin(); it != all_monsters[type - 1]->end(); ++it) {
		monster_t* mon = &(it->second);
		if (mon->id) {
			TRACE_LOG("monster[%u]: id=%u atk=%u lv=%u hp=%u mp=%u exp=%u stdiff=%u", type, mon->id, mon->atk, mon->lv, mon->hp, mon->mp, mon->exp, mon->stage_diff);
			for ( uint32_t i = 0; i < mon->drops.material_drop_num; i++ ) {
				other_drop_t* tmp = &(mon->drops.material_drop[i]);
				TRACE_LOG("monster material_drop: id=%u itemid=[%u %u %u]", mon->id, tmp->item_id, 
						tmp->drop_odds_lower, tmp->drop_odds_upper);
			}
			for ( uint32_t i = 0; i < mon->drops.lucky_drop_num; i++ ) {
				other_drop_t* tmp = &(mon->drops.lucky_drop[i]);
				TRACE_LOG("monster lucky_drop: id=%u itemid=[%u %u %u]", mon->id, tmp->item_id, 
						tmp->drop_odds_lower, tmp->drop_odds_upper);
			}
			for ( uint32_t i = 0; i < mon->drops.clothes_drop_num; i++ ) {
				clothes_drop_t* tmp = &(mon->drops.clothes_drop[i]);
				TRACE_LOG("monster clothes_drop: id=%u itemid=[%u %u %u]", mon->id, tmp->drop_lv, 
						tmp->drop_odds_lower, tmp->drop_odds_upper);
			}
			for ( uint32_t i = 0; i < mon->drops.items_drop_num; i++ ) {
				items_drop_t* tmp = &(mon->drops.items_drop[i]);
				TRACE_LOG("monster items_drop: id=%u itemid=[%u %u %u]", mon->id, tmp->drop_lv, 
						tmp->drop_odds_lower, tmp->drop_odds_upper);
			}
			for ( uint32_t i = 0; i < mon->drops.buffs_drop_num; i++ ) {
				buff_drop_t* tmp = &(mon->drops.buff_drop[i]);
				TRACE_LOG("monster buffs_drop: id=%u itemid=[%u %u %u]", mon->id, tmp->buff_id, 
						tmp->drop_odds_lower, tmp->drop_odds_upper);
			}
		}
	}
	TRACE_LOG("============= End Dumping Monsters =============\n");

	return 0;
}

int load_easy_monsters(xmlNodePtr cur)
{
	return load_monsters(cur, stage_mode_easy);
}

int load_nor_monsters(xmlNodePtr cur)
{
	return load_monsters(cur, stage_mode_nor);
}

int load_hard_monsters(xmlNodePtr cur)
{
	return load_monsters(cur, stage_mode_hard);
}
int load_hell_monsters(xmlNodePtr cur)
{
	return load_monsters(cur, stage_mode_hell);
}
int load_tower_monsters(xmlNodePtr cur)
{
	return load_monsters(cur, stage_mode_tower);
}

int load_epic_monsters(xmlNodePtr cur)
{
	return load_monsters(cur, stage_mode_epic);
}

int load_legend_monsters(xmlNodePtr cur)
{
	return load_monsters(cur, stage_mode_legend);
}


/**
  * @brief reload monsters configs from an xml file
  * @return 0 on success, -1 on error
  */
int reload_monsters()
{
	try {
		//load_xmlconf("./conf/monsters.xml", load_nor_monsters);
	} catch (...) {
		INFO_LOG("reload monsters catched exception");
		return -1;
	}
	INFO_LOG("reload monsters succ");
	return 0;
}



/**
  * @brief load item drop configs from an xml file
  * @return 0 on success, -1 on error
  */
int load_item_drop(xmlNodePtr cur, item_drop_t* p_drops)
{
	cur = cur->xmlChildrenNode;
	p_drops->lucky_drop_num = 0;
	p_drops->material_drop_num = 0;
	p_drops->unique_items_drop_num = 0;
	p_drops->card_drop_num = 0;

	while (cur) {
	  if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("ItemDrop"))) {
	  	xmlNodePtr cur_chd= cur->xmlChildrenNode;
	  	while (cur_chd) {
	  		if (!xmlStrcmp(cur_chd->name, reinterpret_cast<const xmlChar*>("MechanismDrop"))){
				get_xml_prop_def(p_drops->mechanism_drop_id, cur_chd, "ID", 0);
				p_drops->p_data = mechanism_drop_mgr::getInstance()->get_mechanism_drop_by_id(p_drops->mechanism_drop_id); 		
			}else if (!xmlStrcmp(cur_chd->name, reinterpret_cast<const xmlChar*>("MaterialDrop"))) {
	  			double odds = 0.0;
	  			uint32_t add_odds = 0;
	  			uint32_t from_odds = 0;
				uint32_t max_odds = 0;
	  			
				if (p_drops->material_drop_num == max_drop_num) {
	  				ERROR_LOG("material num more than max_drop_num[%u %u]",p_drops->material_drop_num, max_drop_num);
	  				return -1;
	  			}
    
	  			if (p_drops->material_drop_num > 0) {
	  				from_odds = p_drops->material_drop[p_drops->material_drop_num - 1].drop_odds_upper + 1;
	  			}
    
	  			get_xml_prop(p_drops->material_drop[p_drops->material_drop_num].item_id, cur_chd, "ItemID");
				if (!check_item_exits(p_drops->material_drop[p_drops->material_drop_num].item_id)) {
					return -1;
				}
	  			get_xml_prop(odds, cur_chd, "DropOdds");
	  			add_odds = static_cast<uint32_t>(odds * drop_odds_multiple);

				get_xml_prop_def(max_odds, cur_chd, "MAX", 100);
				/*if ( add_odds == 0 ) {
	  				ERROR_LOG("add_odds == 0,itemid=[%u]", p_drops->material_drop[p_drops->material_drop_num].item_id);
	  				return -1;
				}*/
	  			p_drops->material_drop[p_drops->material_drop_num].drop_odds_lower = from_odds;
	  			//p_drops->material_drop[p_drops->material_drop_num].drop_odds_upper = from_odds + add_odds - 1;
	  			p_drops->material_drop[p_drops->material_drop_num].drop_odds_upper = !(from_odds+add_odds) ? 0 : from_odds + add_odds - 1;

				p_drops->material_drop[p_drops->material_drop_num].max_odds = max_odds;
	  			p_drops->material_drop_num++;
				//TRACE_LOG("material_num[%u]",p_drops->material_drop_num);
	  		} else if (!xmlStrcmp(cur_chd->name, reinterpret_cast<const xmlChar*>("LuckyDrop"))) {
	  			double odds = 0.0;
	  			uint32_t add_odds = 0;
	  			//uint32_t from_odds = 0;
				uint32_t max_odds = 0;
	  			
				if (p_drops->lucky_drop_num == max_drop_num) {
	  				ERROR_LOG("lucky num more than max_drop_num[%u %u]",p_drops->lucky_drop_num, max_drop_num);
	  				return -1;
	  			}
    
	  			if (p_drops->lucky_drop_num > 0) {
	  			//	from_odds = p_drops->lucky_drop[p_drops->lucky_drop_num - 1].drop_odds_upper + 1;
	  			}
    
	  			get_xml_prop(p_drops->lucky_drop[p_drops->lucky_drop_num].item_id, cur_chd, "ItemID");
				if (!check_item_exits(p_drops->lucky_drop[p_drops->lucky_drop_num].item_id)) {
					return -1;
				}
	  			get_xml_prop(odds, cur_chd, "DropOdds");
	  			add_odds = static_cast<uint32_t>(odds * drop_odds_multiple);

				get_xml_prop_def(max_odds, cur_chd, "MAX", 100);

	  			p_drops->lucky_drop[p_drops->lucky_drop_num].drop_odds_lower = 0;
	  			p_drops->lucky_drop[p_drops->lucky_drop_num].drop_odds_upper = add_odds;
				p_drops->lucky_drop[p_drops->lucky_drop_num].max_odds = max_odds;
	  			p_drops->lucky_drop_num++;
	  		} else if (!xmlStrcmp(cur_chd->name, reinterpret_cast<const xmlChar*>("ClothesDrop"))) {
	  			double odds = 0.0;
	  			uint32_t add_odds = 0;
	  			uint32_t from_odds = 0;
	  			
				if (p_drops->clothes_drop_num == max_drop_num) {
	  				ERROR_LOG("clothes num more than max_drop_num[%u %u]",p_drops->clothes_drop_num, max_drop_num);
	  				return -1;
	  			}
    
	  			if (p_drops->clothes_drop_num > 0) {
	  				//from_odds = p_drops->clothes_drop[p_drops->clothes_drop_num - 1].drop_odds_upper + 1;
	  			}
    
	  			get_xml_prop(p_drops->clothes_drop[p_drops->clothes_drop_num].drop_lv, cur_chd, "DropLv");
				if (p_drops->clothes_drop[p_drops->clothes_drop_num].drop_lv > max_drop_lv) {
	  				ERROR_LOG("than max drop lv, drop_lv=[%u]", p_drops->clothes_drop[p_drops->clothes_drop_num].drop_lv);
	  				return -1;
				}
	  			get_xml_prop_def(p_drops->clothes_drop[p_drops->clothes_drop_num].drop_id, cur_chd, "DropID", 1);
	  			get_xml_prop(odds, cur_chd, "DropOdds");
	  			add_odds = static_cast<uint32_t>(odds * drop_odds_multiple);
				//TODO:
				/*if ( add_odds == 0 ) {
	  				ERROR_LOG("add_odds == 0,drop_lv=[%u]", p_drops->clothes_drop[p_drops->clothes_drop_num].drop_lv);
	  				return -1;
				}*/
	  			p_drops->clothes_drop[p_drops->clothes_drop_num].drop_odds_lower = from_odds;
	  			p_drops->clothes_drop[p_drops->clothes_drop_num].drop_odds_upper = from_odds + add_odds - 1;
	  			p_drops->clothes_drop[p_drops->clothes_drop_num].drop_odds_upper = !(from_odds+add_odds) ? 0 : from_odds + add_odds - 1;
				p_drops->clothes_drop_num++;
	  		} else if (!xmlStrcmp(cur_chd->name, reinterpret_cast<const xmlChar*>("ItemsDrop"))) {
	  			double odds = 0.0;
	  			uint32_t add_odds = 0;
	  			uint32_t from_odds = 0;
				uint32_t max_odds = 0;
	  			
				if (p_drops->items_drop_num == max_drop_num) {
	  				ERROR_LOG("items num more than max_drop_num[%u %u]",p_drops->items_drop_num, max_drop_num);
	  				return -1;
	  			}
    
	  			if (p_drops->items_drop_num > 0) {
	  				from_odds = p_drops->items_drop[p_drops->items_drop_num - 1].drop_odds_upper + 1;
	  			}
    
	  			get_xml_prop(p_drops->items_drop[p_drops->items_drop_num].drop_lv, cur_chd, "DropLv");
				if (p_drops->items_drop[p_drops->items_drop_num].drop_lv > max_drop_lv) {
	  				ERROR_LOG("than max drop lv, drop_lv=[%u]", p_drops->items_drop[p_drops->items_drop_num].drop_lv);
	  				return -1;
				}
	  			get_xml_prop(odds, cur_chd, "DropOdds");
	  			add_odds = static_cast<uint32_t>(odds * drop_odds_multiple);
				get_xml_prop_def(max_odds, cur_chd, "MAX", 100);
				//TODO:
	  			p_drops->items_drop[p_drops->items_drop_num].drop_odds_lower = from_odds;
	  			//p_drops->items_drop[p_drops->items_drop_num].drop_odds_upper = from_odds + add_odds - 1;
	  			p_drops->items_drop[p_drops->items_drop_num].drop_odds_upper = !(from_odds+add_odds) ? 0 : from_odds + add_odds - 1;
				p_drops->items_drop[p_drops->items_drop_num].max_odds = max_odds;
	  			p_drops->items_drop_num++;
	  		} else if (!xmlStrcmp(cur_chd->name, reinterpret_cast<const xmlChar*>("UniqueDrop"))) {
	  			double odds = 0.0;
	  			uint32_t add_odds = 0;
	  			//uint32_t from_odds = 0;
				uint32_t max_odds = 0;
	  			
				if (p_drops->unique_items_drop_num == max_drop_num) {
	  				ERROR_LOG("items num more than max_drop_num[%u %u]",p_drops->unique_items_drop_num, max_drop_num);
	  				return -1;
	  			}
    
	  			if (p_drops->unique_items_drop_num > 0) {
	  			//	from_odds = p_drops->unique_items_drop[p_drops->unique_items_drop_num - 1].drop_odds_upper + 1;
	  			}

				get_xml_prop(p_drops->unique_items_drop[p_drops->unique_items_drop_num].item_id, cur_chd, "ItemID");
				if (!check_item_exits(p_drops->unique_items_drop[p_drops->unique_items_drop_num].item_id)) {
					return -1;
				}
				get_xml_prop_def(p_drops->unique_items_drop[p_drops->unique_items_drop_num].role_type, cur_chd, "RoleType", 0);
				TRACE_LOG("load unique item: %u %u %u", p_drops->unique_items_drop[p_drops->unique_items_drop_num].item_id, 
					p_drops->unique_items_drop[p_drops->unique_items_drop_num].role_type,
					p_drops->unique_items_drop[p_drops->unique_items_drop_num].bit_pos);
				
				p_drops->unique_items_drop[p_drops->unique_items_drop_num].bit_pos = 
					get_unique_item_bitpos(p_drops->unique_items_drop[p_drops->unique_items_drop_num].item_id);
				if (! (p_drops->unique_items_drop[p_drops->unique_items_drop_num].bit_pos)) {
					ERROR_LOG("bit_pos can not be zero[%u]", 
						p_drops->unique_items_drop[p_drops->unique_items_drop_num].item_id);
	  				return -1;
				}
    
	  			get_xml_prop(odds, cur_chd, "DropOdds");

				TRACE_LOG("load unique item: %u %u %u", p_drops->unique_items_drop[p_drops->unique_items_drop_num].item_id, 
					p_drops->unique_items_drop[p_drops->unique_items_drop_num].role_type,
					p_drops->unique_items_drop[p_drops->unique_items_drop_num].bit_pos);
	  			add_odds = static_cast<uint32_t>(odds * drop_odds_multiple);

				get_xml_prop_def(max_odds, cur_chd, "MAX", 100);

	  			p_drops->unique_items_drop[p_drops->unique_items_drop_num].drop_odds_lower = 0;
	  			p_drops->unique_items_drop[p_drops->unique_items_drop_num].drop_odds_upper = add_odds;//!(from_odds+add_odds) ? 0 : from_odds + add_odds - 1;
				p_drops->unique_items_drop[p_drops->unique_items_drop_num].max_odds = max_odds;
	  			p_drops->unique_items_drop_num++;
	  		} else if (!xmlStrcmp(cur_chd->name, reinterpret_cast<const xmlChar*>("BuffDrop"))) {
	  			double odds = 0.0;
	  			uint32_t add_odds = 0;
	  			uint32_t from_odds = 0;
				uint32_t max_odds = 0;
	  			
				if (p_drops->buffs_drop_num == max_drop_num) {
	  				ERROR_LOG("buffs num more than max_drop_num[%u %u]",p_drops->buffs_drop_num, max_drop_num);
	  				return -1;
	  			}
    
	  			if (p_drops->buffs_drop_num > 0) {
	  				from_odds = p_drops->buff_drop[p_drops->buffs_drop_num - 1].drop_odds_upper + 1;
	  			}
    
	  			get_xml_prop(p_drops->buff_drop[p_drops->buffs_drop_num].buff_id, cur_chd, "BuffID");
	  			get_xml_prop(odds, cur_chd, "DropOdds");
	  			add_odds = static_cast<uint32_t>(odds * drop_odds_multiple);

				get_xml_prop_def(max_odds, cur_chd, "MAX", 100);

	  			p_drops->buff_drop[p_drops->buffs_drop_num].drop_odds_lower = from_odds;
	  			p_drops->buff_drop[p_drops->buffs_drop_num].drop_odds_upper = !(from_odds+add_odds) ? 0 : from_odds + add_odds - 1;
				p_drops->buff_drop[p_drops->buffs_drop_num].max_odds = max_odds;
	  			p_drops->buffs_drop_num++;
	  		} else if (!xmlStrcmp(cur_chd->name, reinterpret_cast<const xmlChar*>("UniqueDrop"))) {
				
			} else if (!xmlStrcmp(cur_chd->name, reinterpret_cast<const xmlChar*>("CardDrop"))) {
				double odds = 0.0;
	  			uint32_t add_odds = 0;
	  			//uint32_t from_odds = 0;
				uint32_t max_odds= 0;
	  			
				if (p_drops->card_drop_num == max_drop_num) {
	  				ERROR_LOG("card_drop num more than max_drop_num[%u %u]",
							p_drops->card_drop_num,
						   	max_drop_num);
	  				return -1;
	  			}
    
	  			if (p_drops->card_drop_num > 0) {
	  			//	from_odds = p_drops->card_drop[p_drops->card_drop_num - 1].drop_odds_upper + 1;
	  			}
    
	  			get_xml_prop(p_drops->card_drop[p_drops->card_drop_num].item_id, cur_chd, "ItemID");
				if (!check_item_exits(p_drops->card_drop[p_drops->card_drop_num].item_id)) {
					return -1;
				}

	  			get_xml_prop(odds, cur_chd, "DropOdds");

				get_xml_prop_def(max_odds, cur_chd, "MAX", 100);
	  			add_odds = static_cast<uint32_t>(odds * drop_odds_multiple);

	  			p_drops->card_drop[p_drops->card_drop_num].drop_odds_lower = 0;
	  			p_drops->card_drop[p_drops->card_drop_num].drop_odds_upper = add_odds;
				p_drops->card_drop[p_drops->card_drop_num].max_odds = max_odds;
	  			p_drops->card_drop_num++;

			}
	  		cur_chd = cur_chd->next;	
	  	}
	  }
	  cur = cur->next;	
	}
	
	return 0;
}

/**
  * @brief load quality configs from an xml file
  * @return 0 on success, -1 on error
  */
int load_quality(xmlNodePtr cur, uint32_t stage_mode)
{
	// load monsters from xml file
	cur = cur->xmlChildrenNode;
	stage_mode--;

	// since we need to reload monsters.xml at run time, so we need to init 'all_stages'
	memset(&quality_drop_odds[stage_mode], 0, sizeof(quality_drop_odds[stage_mode]));

	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Drop"))) {
			uint32_t idx = 0;
			uint32_t drop_lv;
			uint32_t odds = 0;
			//get_xml_prop(drop_lv, cur, "Lv");
			get_xml_prop(drop_lv, cur, "ID");
			if ( drop_lv > max_drop_lv) {
				ERROR_LOG("drop_lv more than max:[%u %u]", drop_lv, max_drop_lv);
				return -1;
			}
			
			get_xml_prop(odds, cur, "White");
			quality_drop_odds[stage_mode][drop_lv - 1][idx] = odds * drop_odds_multiple;
			TRACE_LOG("White:[%u %u %u %u]",stage_mode, drop_lv, idx, quality_drop_odds[stage_mode][drop_lv - 1][idx]);
			idx++;
			
			get_xml_prop(odds, cur, "Green");
			quality_drop_odds[stage_mode][drop_lv - 1][idx] = odds * drop_odds_multiple + quality_drop_odds[stage_mode][drop_lv - 1][idx - 1];
			TRACE_LOG("Green:[%u %u %u %u]",stage_mode, drop_lv, idx, quality_drop_odds[stage_mode][drop_lv - 1][idx]);
			idx++;
			
			get_xml_prop(odds, cur, "Blue");
			quality_drop_odds[stage_mode][drop_lv - 1][idx] = odds * drop_odds_multiple + quality_drop_odds[stage_mode][drop_lv - 1][idx - 1];
			TRACE_LOG("Blue:[%u %u %u %u]",stage_mode, drop_lv, idx, quality_drop_odds[stage_mode][drop_lv - 1][idx]);
			idx++;
			
			get_xml_prop(odds, cur, "Purple");
			quality_drop_odds[stage_mode][drop_lv - 1][idx] = odds * drop_odds_multiple + quality_drop_odds[stage_mode][drop_lv - 1][idx - 1];
			TRACE_LOG("Purple:[%u %u %u %u]",stage_mode, drop_lv, idx, quality_drop_odds[stage_mode][drop_lv - 1][idx]);
			idx++;
			
			get_xml_prop(odds, cur, "Silvery");
			quality_drop_odds[stage_mode][drop_lv - 1][idx] = odds * drop_odds_multiple + quality_drop_odds[stage_mode][drop_lv - 1][idx - 1];
			TRACE_LOG("Slivery:[%u %u %u %u]",stage_mode, drop_lv, idx, quality_drop_odds[stage_mode][drop_lv - 1][idx]);
			idx++;
			
			get_xml_prop(odds, cur, "Gold");
			quality_drop_odds[stage_mode][drop_lv - 1][idx] = odds * drop_odds_multiple + quality_drop_odds[stage_mode][drop_lv - 1][idx - 1];
			TRACE_LOG("Gold:[%u %u %u %u]",stage_mode, drop_lv, idx, quality_drop_odds[stage_mode][drop_lv - 1][idx]);
		}
	  	cur = cur->next;	
	}
	if (stage_mode != 10)
		return 0;
//	TRACE_LOG("<<<<<<<<<<<<<<<<<<<<<<<<");
	for (uint32_t i = 0; i < 11; i++) {
		for (uint32_t j = 0; j < 100; j++) {
			for (uint32_t n = 0; n < 6; n++) {
				TRACE_LOG("%u %u %u : %u ", i, j, n, quality_drop_odds[i][j][n]);
			}
		}
	}
//	TRACE_LOG(">>>>>>>>>>>>>>>>>>>>>>>>");
	return 0;
}


int load_easy_quality(xmlNodePtr cur)
{
	return load_quality(cur, stage_mode_easy);
}

int load_nor_quality(xmlNodePtr cur)
{
	return load_quality(cur, stage_mode_nor);
}

int load_hard_quality(xmlNodePtr cur)
{
	return load_quality(cur, stage_mode_hard);
}

int load_hell_quality(xmlNodePtr cur)
{
	return load_quality(cur, stage_mode_hell);
}

int load_tower_quality(xmlNodePtr cur)
{
	return load_quality(cur, stage_mode_tower);
}

int load_epic_quality(xmlNodePtr cur)
{
	return load_quality(cur, stage_mode_epic);
}

int load_legend_quality(xmlNodePtr cur)
{
	return load_quality(cur, stage_mode_legend);
}


/**
  * @brief load quality configs from an xml file
  * @return 0 on success, -1 on error
  */
int reload_quality()
{
	try {
		load_xmlconf("./conf/stage_easy/quality.xml", load_easy_quality);
		load_xmlconf("./conf/stage_normal/quality.xml", load_nor_quality);
		load_xmlconf("./conf/stage_hard/quality.xml", load_hard_quality);
		load_xmlconf("./conf/stage_hell/quality.xml", load_hell_quality);
		load_xmlconf("./conf/stage_tower/quality.xml", load_tower_quality);
	} catch (...) {
		INFO_LOG("reload quality catched exception");
		return -1;
	}
	INFO_LOG("reload quality succ");
	return 0;
}

void update_challenge_attr(Player * p, Battle * btl)
{
	const challenge_t * t_c = get_challenge(btl->stage()->id);
	Player * player = btl->get_some_player();

	if (!t_c || (player->pass_cnt > 9 && btl->stage()->id == 954)) {
		return;
	}
	uint32_t pass_cnt = 0;
	if (btl->stage()->id == 954) {
		pass_cnt = player->pass_cnt;
		if (pass_cnt >= max_diff_cnt) {
			pass_cnt = max_diff_cnt - 1;
		}
	} else {
		pass_cnt = btl->difficulty() - 1;
	}

	const challenge_arr_t & attr_add = t_c->challenges[pass_cnt];
	p->hp = (p->hp * (100 + attr_add.hp_add)) / 100;
	p->maxhp = (p->maxhp * (100 + attr_add.hp_add)) / 100;
	if (btl->stage()->id == 954 || btl->difficulty() >= 5) {
		p->lv = player->lv + attr_add.lv_add;
	}

	p->atk = (p->atk * (100 + attr_add.atk_add)) / 100;

	for (SkillMap::iterator it = p->skills_map.begin(); it != p->skills_map.end(); ++it) {
		player_skill_t & skill_info = it->second; 
		skill_info.skill_attr.cut_cd = (skill_info.p_skill_info->cool_down * (attr_add.cool_dec)) / 100;
	}
}

void  ran_ai(int * ptr, int n, int m) 
{
	int k = 0;
	int j = 0;
	for (int i = 0; i < n; i++) {
		k = ptr[i];
		j = rand()%(n - i) + i;
		ptr[i] = ptr[j];
		ptr[j] = k;
	}
}


void update_challenge_ai(Battle * btl)
{
	Player * player = btl->get_some_player();
	const challenge_t * t_challenge = get_challenge(btl->stage()->id);
	if (!t_challenge || (player->pass_cnt > 9 && btl->stage()->id == 954)) {
		return;
	}

	const challenge_arr_t & attr_t = t_challenge->challenges[player->pass_cnt]; 
	int low_ai_cnt = attr_t.low_cnt * btl->mon_num_ / 100;
	int mid_ai_cnt = attr_t.mid_cnt * btl->mon_num_ / 100;
	int high_ai_cnt = attr_t.high_cnt * btl->mon_num_ / 100; 
	int boss_ai_cnt = attr_t.boss_cnt * btl->mon_num_ / 100;

	if (!low_ai_cnt && !mid_ai_cnt && !high_ai_cnt && !boss_ai_cnt) {
		return;
	}	

	int ptr[200] = {0};	
	memset(ptr, 0, sizeof(ptr));
	for (int i = 0; i < low_ai_cnt; ++i) {
		ptr[i] = 1;
	}


	for (int i = 0; i < mid_ai_cnt; i++) {
		ptr[i + low_ai_cnt] = 2;
	}

	for (int i = 0; i < high_ai_cnt; i++) {
		ptr[i + low_ai_cnt + mid_ai_cnt] = 3;
	}

	for (int i = 0; i < boss_ai_cnt; i++) {
		ptr[i + low_ai_cnt + mid_ai_cnt + high_ai_cnt] = 4;
	}

	int cur_idx = 0;

	int cur_monster = 0;

	for (int i = 0; i != stage_max_map_num; ++i) 
	{
		if(btl->maps_[i].id)
		{
			map_t * cur_map = &(btl->maps_[i]);
			uint32_t map_monster_cnt = cur_map->monsters.size();
			ran_ai(ptr + cur_idx, btl->mon_num_ - cur_idx, map_monster_cnt);	
			cur_idx += map_monster_cnt;

			for (PlayerSet::iterator it = cur_map->monsters.begin(); 
					it != cur_map->monsters.end(); ++it) {
				Player * p = *it;
				if (p->mon_info->type != 1) {
					switch (ptr[cur_monster]) {
						case 1:
							p->i_ai->select_ai(LOW_LV_AI);
							break;
						case 2:
							p->i_ai->select_ai(MIDDLE_LV_AI);
							break;
						case 3:
							p->i_ai->select_ai(HIGH_LV_AI);
							break;
						case 4:
							p->i_ai->select_ai(BOSS_AI, p->role_type);
						default:
							p->i_ai->select_ai(HIGH_LV_AI);
							break;
						
					}

					TRACE_LOG("IN STAGE %u Map %u select AI TYPE %u", btl->stage()->id,
							i, ptr[cur_monster]);
				}

				if (cur_monster < btl->mon_num_) {
					cur_monster++;
				}
			}
		}

	}
}

int calc_monster_gold_drop(Player *p, Player * killer)
{
	static double b_value[4] = {1, 1, 1.25, 20};
	static double c_value[11] = {0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 0, 0, 0, 0, 0}; 

	double a1 = 0;
	double a2 = 0;
	double A = 0;
	double B = 0;
	double C = 0;

	uint32_t tmp_idx = 0;
	if (p->mon_info->attr_type <= 4 && p->mon_info->attr_type != 0) {
		tmp_idx = p->mon_info->attr_type - 1;
	}
	B = b_value[tmp_idx];

	tmp_idx = 0;
	if (killer->btl->difficulty() <= 11 && killer->btl->difficulty() != 0) {
		tmp_idx = killer->btl->difficulty() - 1;
	}
	
	C = c_value[tmp_idx];

	const Player * team_member = killer->btl->get_one_other_player(killer); 
	if (team_member) {
		int lv_dis = (int)killer->lv - (int)p->lv;
		if (lv_dis <= 0) {
			a1 = 1.0;
		} else if (lv_dis <= 5) {
			a1 = 0.9;
		} else if (lv_dis <= 15) {
			a1 = 0.8;
		} else if (lv_dis <= 20) {
			a1 = 0.6;
		} else if (lv_dis < 25) {
			a1 = 0.4;
		} else {
			a1 = 0;
		}

		lv_dis = (int)team_member->lv - (int)p->lv;
		if (lv_dis <= 0) {
			a2 = 1.0;
		} else if (lv_dis <= 5) {
			a2 = 0.9;
		} else if (lv_dis <= 10) {
			a2 = 0.8;
		} else if (lv_dis <= 15) {
			a2 = 0.7;
		} else if (lv_dis <= 20) {
			a2 = 0.6;
		} else {
			a2 = 0.5;
		}

		A = a1 * a2;
	} else {
		int lv_dis = (int)killer->lv - (int)p->lv;
		if (lv_dis <= -2) {
			A = 1.5;
		} else if (lv_dis <= -1) {
			A = 1.25;
		} else if (lv_dis == 0) {
			A = 1.0;
		} else if (lv_dis == 1) {
			A = 0.8;
		} else if (lv_dis == 2) {
			A = 0.6;
		} else if (lv_dis == 3) {
			A = 0.4;
		} else if (lv_dis == 4) {
			A = 0.2;
		} else {
			A = 0;
		}
	}

	double Gold;
	if (p->lv * 2 < 20) {
		Gold = 20;
	} else {
		Gold = 2 * p->lv;
	}

	int makemoney = p->mon_info->makemoney;
	double greed_rate = ((double)(killer->greed_rate) + (double)(killer->buf_greed_rate) ) / 100.0; 

	return (int)(Gold * A * B * C * (greed_rate + 1.0)) * makemoney;
}

bool player_teleport(Player* player, int x, int y)
{
	if (!player || !(player->btl)) {
		return false;
	}
	
	KfAstar::Points* pts = NULL;

	
	KfAstar* p_star = player->btl->get_cur_map_path( player->cur_map );
	if(p_star)
	{
		pts = (KfAstar::Points*)p_star->findpath(KfAstar::Point(0, 0), KfAstar::Point(0, 0));
		pts = (KfAstar::Points*)p_star->findpath(KfAstar::Point(player->pos().x(), player->pos().y()), KfAstar::Point(x, y));

		if (pts && pts->size() > 0) {
	//		uint32_t idx = pts->size() - 1;

			Vector3D new_pos((*pts)[0].x, (*pts)[0].y);
			player->adjust_position_ex(new_pos);
			player->adjust_coordination();
			//player->teleport((*pts)[0].x, (*pts)[0].y);
			return true;
		}
	}
	return false;
}

