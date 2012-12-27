#include <cstdio>
#include <list>

using namespace std;

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <libtaomee++/random/random.hpp>

using namespace taomee;

extern "C" {
#include <glib.h>
#include <assert.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
}

#include "monster.hpp"
#include "player.hpp"
#include "skill.hpp"

#include "battle_impl.hpp"
#include "battle_manager.hpp"
#include "battle_switch.hpp"
#include "player_status.hpp"
#include "cache_switch.hpp"
#include "battle_logic.hpp"
#include "monster.hpp"
#include "indoor_test.hpp"

/*! level matching battle player set */
list<Player*> Battle::s_player_list_;
/*! for generating a unique battle ID */
uint32_t Battle::s_base_id_ = 0;

uint32_t pvp_players_statistics = 0;
uint32_t pve_players_statistics = 0;
uint32_t pve_team_players_statistics = 0;


//////////////////////////////internal function define////////////////////////////////////////////
static void start_pve_battle(Battle* btl, bool restart_flag);
static void start_pvp_battle(Battle* btl, bool restart_flag);
static void on_pve_player_dead(Battle* btl, Player* p);
static void on_pvp_player_dead(Battle* btl, Player* p);

///////////////////////////////////////////////////////////////////////////////////////////////////
void on_pve_player_dead(Battle* btl, Player* p)
{
	if(!is_valid_uid(p->id) && !is_summon_mon(p->role_type))
	{
		if (p->role_type >= 30002 && p->role_type <= 30004) 
		{
			p->cur_map->del_mechanism_from_map(p->id);
		}
		if(p->role_type == 11356)
		{
			if(p->pos().y() == 206)
			{
				KfAstar::Point pt1(200,140);
				KfAstar::Point pt2(400,260);
				btl->change_map_data(p, pt1, pt2, 0);
			}

			if(p->pos().y() == 319)
			{
				KfAstar::Point pt1(200,260);
				KfAstar::Point pt2(400,380);
				btl->change_map_data(p, pt1, pt2, 0);
			}

			if(p->pos().y() == 433)
			{
				KfAstar::Point pt1(200,380);
				KfAstar::Point pt2(400,520);
				btl->change_map_data(p, pt1, pt2, 0);			
			}
		}
		if (p->role_type < 19001) 
		{
			--btl->mon_num_;
			if (p->btl->is_battle_tower()) 
			{
				Player* player = btl->get_some_player();
				if (p->cur_map->monsters.size() == 1 && player->cur_tower_layer == (max_fumo_top_floor - 1)) 
				{
					player->cur_tower_layer++;
					btl->on_btl_over(player);
					if (btl->sent_finish_tower_info2db(player) != 0) 
					{
						ERROR_LOG("synchro uid=[%u] tower err.", player->id);
					}
					do_stat_item_log_5(stat_log_fumo_pass_tm,  player->cur_tower_layer, 1, get_now_tv()->tv_sec - player->score.start_tm);
				} 
				else if (p->cur_map->monsters.size() == 1) 
				{
					player->cur_tower_layer++;
					player->calc_stage_score(false);
					if (btl->sent_finish_tower_info2db(player) != 0) 
					{
						ERROR_LOG("synchro uid=[%u] tower err.", player->id);
					}
					do_stat_item_log_5(stat_log_fumo_pass_tm, player->cur_tower_layer, 1, get_now_tv()->tv_sec - player->score.start_tm);
				}
			}
			else
			{
				if(p->mon_info->type == 1)
				{
					btl->on_boss_dead();
					btl->set_all_players_relive();
					btl->notify_first_boss_kill(btl->stage()->id, btl->difficulty(), p->role_type);
					btl->on_btl_over(p);
				}
			}
			if(p->role_type == 11362 && btl->stage_->id == 943)
			{
				btl->on_btl_over(p);
			}
			if(p->role_type == 11357 && btl->stage_->id == 943)
			{
				p->call_monster(11358, p->pos().x(), p->pos().y());
				p->call_monster(11358, p->pos().x(), p->pos().y());
			}
			if(p->role_type == 11358 && btl->stage_->id == 943)
			{
				p->call_monster(11359, p->pos().x(), p->pos().y());
				p->call_monster(11359, p->pos().x(), p->pos().y());
			}
			if(p->role_type == 11359 && btl->stage_->id == 943)
			{
				p->call_monster(11361, p->pos().x(), p->pos().y());
				p->call_monster(11361, p->pos().x(), p->pos().y());
			}

            if (p->role_type == 11451 && btl->stage_->id == 963) {
                btl->on_btl_over(p);
            }
			if (p->role_type == 11512 || p->role_type == 11515 || p->role_type == 11516) 
			{
				notify_client_btl_monsters_dead(p); 
				Player* player = btl->get_one_player();
				if (player) {
					if (!(player->cur_map->get_one_monster(13153))) {
						player->call_monster(13153, p->pos().x(), p->pos().y());
		
						Player* boss = player->cur_map->get_one_monster(13153);
						if (boss) {
							uint32_t tmp = rand() % 3;
							if ( tmp == 0) {
								add_player_buff(boss, 215, 0, 6000, true); //speed up, hp down
							} else if (tmp == 1) {
								add_player_buff(boss, 214, 0, 6000, true); //hp up, speed down
							} else {
								add_player_buff(boss, 213, 0, 6000, true); //def up  hp up 
								add_player_buff(player, 519, 0, 6000, false); //player crit up
							}
						}
					}
				}
			}

			if (p->role_type == 13142)
			{
				if (p->cur_map) {
					Player* boss = p->cur_map->get_one_monster(13143);
					if (boss && boss->is_dead()) {
						btl->on_btl_over(p);
					}
				}
			}
			if (p->role_type == 13143)
			{
				if (p->cur_map) {
					Player* boss = p->cur_map->get_one_monster(13142);
					if (boss && boss->is_dead()) {
						btl->on_btl_over(p);
					}
				}
			}

		}
		else if( p->role_type == 19141 && btl->stage_->id == 928)
		{
			btl->on_btl_over(p);
		}
		else if( p->role_type == 19142 && btl->stage_->id == 929)
		{
			btl->on_btl_over(p);
		} 
		else if (p->role_type == 19143 && btl->stage_->id == 932) 
		{
			btl->on_btl_over(p);
		} 
		else if (p->role_type == 19144 && btl->stage_->id == 933) 
		{
			btl->on_btl_over(p);
		} 
		else if (p->role_type == 19145) 
		{
			btl->on_btl_over(p);
		}
		
		return ;	
	}
	else if (is_valid_uid(p->id) && !is_summon_mon(p->role_type)) 
	{
		static int stage_dead_over[15] = { 921, 928, 927, 930, 933, 943, 0, 0, 
			                               0, 0, 0, 0, 0, 0, 0 };
		for (int i = 0; i < 15; i++) {
			if (btl->stage_->id == stage_dead_over[i]) {
				btl->on_btl_over(p, true);
				break;
			}
		}
	}
	do_stat_item_log_2(stat_log_role_dead, 0, p->role_type, 1);
}

void end_pvp_on_player_dead(Battle* btl, Player* p, uint32_t win_team)
{
    
    btl->set_battle_status(Battle::battle_ended);
    btl->end_reason_ = end_btl_normal;
    btl->set_all_monsters_dead();
    for (PlayerVec::iterator it = btl->players_.begin(); it != btl->players_.end(); ++it) 
    {
        if ((!(*it)->is_dead() && win_team == 0) ||
			(win_team && win_team == (*it)->team)) 
        {
            btl->winner_ = (*it)->id;
            btl->send_btl_over_pkg(*it);
            (*it)->calc_pvp_score(1);
            if (btl->stage_->id == 931)
                do_stat_item_finish_931_stage(0x09540000, (*it)->role_type, get_now_tv()->tv_sec - (*it)->score.start_tm);
        }
        else
        {
            btl->send_btl_over_pkg(*it, false);
            (*it)->calc_pvp_score(0);
        }
    }

	if (btl->pvp_lv() == pvp_contest_last || btl->pvp_lv() == pvp_ghost_game || btl->pvp_lv() == pvp_16_contest ||
		   	btl->pvp_lv() == pvp_dragon_ship) {
		btl->final_battle_logic();
	}


}

void on_pvp_player_dead(Battle* btl, Player* p)
{
	if(p->mon_info && 
	   p->mon_info->type == 1 &&
	   (btl->pvp_lv() == pvp_monster_game || btl->pvp_lv() == pvp_monster_practice)
	   )
	{
		uint32_t winner_team = p->mon_info->team == 1 ? 2:1;
		btl->over_pvp_939_stage(winner_team);
	}
	else if( p->mon_info && p->mon_info->type == 1 && btl->pvp_lv() == pvp_monster_)
	{
		btl->on_boss_dead();
		btl->set_battle_status(Battle::battle_ended);
		btl->end_reason_ = end_btl_normal;
		btl->set_all_monsters_dead();
		for (PlayerVec::iterator it = btl->players_.begin(); it != btl->players_.end(); ++it) 
		{
			if ((*it)->boss_killer_flg == 1) 
			{
				btl->winner_ = (*it)->id;
				btl->send_btl_over_pkg(*it);
				(*it)->calc_pvp_score(1);
				if (btl->stage_->id == 931)do_stat_item_finish_931_stage(0x09540000, (*it)->role_type, get_now_tv()->tv_sec - (*it)->score.start_tm);
			} 
			else 
			{
				btl->send_btl_over_pkg(*it, false);
				(*it)->calc_pvp_score(0);
			}
		}
	} else {
        if (btl->pvp_lv() == pvp_monster_game || btl->pvp_lv() == pvp_monster_practice)return;
		//万圣杯玩家死亡	
		if (btl->pvp_lv() == pvp_ghost_game && is_valid_uid(p->id)) {
			const Player *player = btl->get_one_other_player(p);
			if (player && !player->is_dead() && p->revive_count < 3 ) {
				call_monster_to_map(p->cur_map, btl,
									11492,
									player->pos().x(), player->pos().y(), 
									monster_team);
			} else {
				end_pvp_on_player_dead(btl, p);
				return;
			}

		}

        if (btl->pvp_lv() == pvp_contest_last || btl->pvp_lv() == pvp_ghost_game) {
            Player * dead_p = btl->get_dead_player();
            if (dead_p && dead_p->revive_count < 3) {
                dead_p->dead_start_tm = get_now_tv()->tv_sec;
                return;
            }
        }

		if (is_valid_uid(p->id) && btl->get_pvp_win_team()) {
        	end_pvp_on_player_dead(btl, p, btl->get_pvp_win_team());
		}
    }

}




void start_pve_battle(Battle* btl, bool restart_flag)
{
	btl->set_battle_status(Battle::battle_started);
	for (PlayerVec::iterator it = btl->players_.begin(); it != btl->players_.end(); ++it) 
	{
		Player* p = *it;
		btl->notify_player_speed_change(p);
#ifdef DEV_SUMMON
		if (p->summon_info.mon_tm != 0 && p->my_summon == 0) 
		{
			p->my_summon = create_summon(p);
		} 
		else if (p->my_summon) 
		{
			p->my_summon->summon_init();
            p->my_summon->set_pos(p->pos());
            p->summon_stand(p->my_summon);
		}	
#endif
		if (p->numen_info.numen_id && p->my_numen == 0) {
			p->my_numen = create_numen(p);
		}
	}

	for (int i = 0; i != stage_max_map_num; ++i) 
	{
		if(btl->maps_[i].id)
		{
			if (btl->is_battle_tower()) 
			{
				Player* player_t = btl->get_some_player();
				uint32_t lv_coef = i * 5 + player_t->lv;
				uint32_t lv_fact = (i + 3) * 5;
				if (i < 3) 
				{
					btl->maps_[i].mon_lv = lv_coef > lv_fact ? lv_fact : lv_coef;
				} 
				else 
				{
					btl->maps_[i].mon_lv = lv_fact;
				}
			}
			btl->mon_num_ += create_monsters(&(btl->maps_[i]), btl);

	//		btl->create_random_monsters(&btl->maps_[i]);
		}
	}

	//for changle stage monster AI
	update_challenge_ai(btl);

	//special btl create monster from home btl pets
	if(btl->stage_->id == 952)
	{
		Player* p = btl->get_some_player();
		for(uint32_t i=0; i< p->home_btl_pets.size(); i++)
		{
			uint32_t pet_id = p->home_btl_pets[i];
			const monster_t* mon = get_monster(pet_id, btl->difficulty());
			if(mon == NULL)continue;
			Player* p_monster = new Player;
			p_monster->set_pos(Vector3D(400, 400));
			set_monster_attr(p_monster, mon, btl, p->lv);
			p_monster->do_enter_map( &btl->maps_[0]);
			btl->mon_num_++;
		}
	}

	if(btl->is_team_btl())
	{
		notify_team_room_start(0, btl->id());
	}

	if(restart_flag == false)
	{
		btl->send_btl_ready_noti();
	}
}

void start_pvp_battle(Battle* btl, bool restart_flag)
{
	btl->set_battle_status(Battle::battle_started);
	for (PlayerVec::iterator it = btl->players_.begin(); it != btl->players_.end(); ++it)
	{
		Player* p = *it;
		btl->notify_player_speed_change(p);
		p->remove_pvp_end_ev();
#ifdef DEV_SUMMON
		if (p->summon_info.mon_tm != 0 && p->my_summon == 0) 
		{
			p->my_summon = create_summon(p);
		} 
		else if (p->my_summon) 
		{
			p->my_summon->summon_init();
            p->my_summon->set_pos(p->pos());
            p->summon_stand(p->my_summon);
		}

		if (p->numen_info.numen_id && p->my_numen == 0) {
			p->my_numen = create_numen(p);
		}

#endif
	}

	for (int i = 0; i != stage_max_map_num; ++i)
	{
		if(btl->maps_[i].id)
		{
			if (btl->is_battle_tower())
			{
				Player* player_t = btl->get_some_player();
				uint32_t lv_coef = i * 5 + player_t->lv;
				uint32_t lv_fact = (i + 3) * 5;
				if (i < 3)
				{
					btl->maps_[i].mon_lv = lv_coef > lv_fact ? lv_fact : lv_coef;
				}
				else
				{
					btl->maps_[i].mon_lv = lv_fact;
				}
			}
			btl->mon_num_ += create_monsters(&(btl->maps_[i]), btl);
		}
	}




	if(btl->is_team_btl())
	{
		btl->set_all_player_skills_cd();
	}
	if(restart_flag == false)
	{
		btl->send_btl_ready_noti();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------
// Public
//
Battle::Battle(Player* p, battle_mode_t btl_mode, const stage_t* stg, uint32_t difficulty, uint8_t in_team_btl)
{
	btl_mode_    = btl_mode;
	btl_difficulty_ = difficulty;
	in_team_btl_flg = in_team_btl;
	players_num_limit = 2;
	status_ 	 = 0;
	watch_status_ = 1;
	end_reason_  = 0;
	winner_      = 0;
	winner_team = 0;
	ready_num_	 = 0;
	mon_num_     = 0;
	hot_join_flag_ = battle_permit_hot_join;
	btl_team_ready_num_ = 0;
	id_			 = BattleManager::get_instance()->produce_battle_id();
	assert(id_);
	
	stage_       = stg;

	// create map of this battle
	map_num_ = stg->map_num;
	for (int i = 0; i != stage_max_map_num; ++i) {
		if (stg->maps[i].id) {
			maps_[i] = stg->maps[i];
			dynamic_paths[i] = new KfAstar( *maps_[i].path); 
		} else {
			maps_[i].id = 0;
			dynamic_paths[i] = NULL;
		}
	}
	
	init_map_ = &(maps_[stg->init_map_id]);
	p->team = player_team_1;
	add_player(p);
	p->set_player_leader(p);

	init_roll_data();
	init_map_summon();
	BattleManager::get_instance()->add_battle(this);

	_creater_lv = p->lv;

	//battle statistic
	if (btl_mode_ == btl_mode_lv_matching) {
		pvp_players_statistics += 2;
	} else {
		do_stat_item_log_2(stat_log_enter_stage, 0, 0, 1);
#ifdef VERSION_KAIXIN
		do_stat_item_log_2(stat_log_enter_stage_kaixin, stg->id, 0, 1);
#endif
		if (in_team_btl_flg) {
			pve_team_players_statistics += 2;
		} else {
			pve_players_statistics += 1;
		}
	}
	
	local_flg = true;
	battle_section_data = 0;
	start_tm = *get_now_tv();
	btl_section_routing_tm = start_tm;
	p_proc = NULL;
	p_init_proc = NULL;
	time_frequency = 0;

	common_flag1_ = 0;
	common_flag2_ = 0;
	common_flag3_ = 0;
	common_flag4_ = 0;
	common_flag5_ = 0;
	common_flag6_ = 0;

	btl_start_tm = 0;
	TRACE_LOG("alloc battle: %u %u", id_, in_team_btl_flg);
	//calc player's luck
	
	//for luck 
	const challenge_t *ch = get_challenge(stg->id);
	if (ch && (p->pass_cnt <= max_diff_cnt - 1)) {
		if (stg->id != 954) {
			p->pass_cnt = difficulty - 1;
		}
		const challenge_arr_t * dirr = &(ch->challenges[p->pass_cnt]);
		if (dirr) {
			p->lucky_cnt = dirr->drop_add;
		}
	//	ERROR_LOG("PLayer %u Luck is %u", p->id, p->lucky_cnt);
	}
	
}

Battle::~Battle()
{	
	//battle statistic
	if (btl_mode_ == btl_mode_lv_matching) {
		pvp_players_statistics = pvp_players_statistics < 2 ? 0 : pvp_players_statistics - 2;
	} else {
		if (in_team_btl_flg) {
			pve_team_players_statistics = pve_team_players_statistics < 2 ? 0 : pve_team_players_statistics - 2;
		} else {
			pve_players_statistics = pve_players_statistics < 1 ? 0 : pve_players_statistics - 1;
		}
	}
	
	for(int i = 0; i != stage_max_map_num; ++i)
	{
		if(maps_[i].id && dynamic_paths[i] != NULL)  
		{	
			delete dynamic_paths[i];
			dynamic_paths[i] = NULL;
		}
	}

	if (!(watchers_.empty())) {
		for (PlayerVec::iterator it = watchers_.begin(); it != watchers_.end(); ++it) {
			Player * p = *it;
			delete p;
		}
		watchers_.clear();
	}
	//clear stage info
	raninfo.clear();



	BattleManager::get_instance()->release_battle_id(id_);
	if (is_team_btl() && !local_flg) {
		//BattleManager::get_instance()->del_battle(this);
		if  (is_battle_pvp() || is_battle_tmp_team_pvp()) {
			uint32_t lv = _creater_lv; 
			ERROR_LOG("WINNER %u", winner_);
			notify_destroy_pvp_room(0, id_, lv, this->pvp_lv()) ;
		} else {
			notify_destroy_room(0, id_, stage_->id,  btl_difficulty_);
		}
	}
	
	BattleManager::get_instance()->del_battle(this);
	final_roll_data();
	final_map_summon();
	
	KDEBUG_LOG(winner_, "BTL OVR\t[win=%u rsn=%u]", winner_, end_reason_);
	TRACE_LOG("dealloc battle: %u %u", id_, is_team_btl());

}

void Battle::register_init_logic_function(uint32_t stage_id, battle_init_logic_proc proc)
{
	if(stage()->id == stage_id)
	{
		p_init_proc = proc;
	}
}

void Battle::unregister_init_logic_function( )
{
	p_init_proc = NULL;
}

void Battle::register_final_logic_function(uint32_t stage_id, battle_final_logic_proc proc)
{
	if(stage()->id == stage_id)
	{
		p_final_proc = proc;
	}
}

void Battle::unregister_final_logic_function( )
{
	p_final_proc = NULL;
}

void Battle::register_logic_function(uint32_t stage_id,  
									 battle_logic_proc proc,  
									 uint32_t time_val, 
									 battle_logic_data_alloc alloc,
									 battle_logic_data_release release
									 )
{
	if( stage()->id == stage_id){
		p_proc = proc;
		time_frequency = time_val;	
		init_battle_logic_data(alloc, release);
	}	
}

void Battle::unregister_logic_function()
{
	p_proc = NULL;
	time_frequency = 0;
	final_battle_logic_data();
}

void Battle::init_battle_logic_data( battle_logic_data_alloc alloc_func, battle_logic_data_release release_func)
{
	if(alloc_func == NULL || release_func == NULL)return;
	btl_logic_data.p_alloc = alloc_func;
	btl_logic_data.p_release = release_func;
	btl_logic_data.data = btl_logic_data.p_alloc();
}

void Battle::final_battle_logic_data()
{
	if(btl_logic_data.p_release == NULL || btl_logic_data.p_alloc == NULL)return ;
	btl_logic_data.p_release( btl_logic_data.data);
	btl_logic_data.p_alloc = NULL;
	btl_logic_data.p_release = NULL;
	btl_logic_data.data = NULL;
}


void Battle::init_battle_logic()
{
	//runing logic function register
	//第3个参数单位毫秒
	register_logic_function(45, process_battle_45_logic, 10*1000);
	register_logic_function(925, process_battle_925_logic, 2*1000);
	register_logic_function(926, process_battle_926_logic, 20*1000);
	register_logic_function(927, process_battle_927_logic, 1*1000);
	register_logic_function(928, process_battle_928_logic, 5*1000);
	register_logic_function(929, process_battle_929_logic, 1000, battle_929_data_alloc, battle_929_data_release);
	register_logic_function(930, process_battle_930_logic, 1*1000);

	register_logic_function(932, process_battle_932_logic, 1* 300);

	register_logic_function(933, process_battle_933_logic, 1 * 1000);
	register_logic_function(934, process_battle_934_logic, 1 * 500);
	register_logic_function(939, process_battle_939_logic, 30 * 1000);
	register_logic_function(941, process_battle_941_final_game_logic, 1 * 1000);
	register_logic_function(942, process_battle_942_logic, 1*1000);
	
	register_logic_function(952, process_battle_952_logic, 500);
	register_logic_function(953, process_battle_953_logic, 1*200);

	register_logic_function(964, process_battle_964_logic, 1 * 200);

	register_logic_function(977, process_battle_977_logic, 400); 
	register_logic_function(978, process_battle_978_logic, 1*200); 

	register_logic_function(979, process_battle_979_logic, 1000); 

	register_logic_function(984, process_battle_984_logic, 1000);

	register_logic_function(985, process_battle_985_logic, 500);

	register_logic_function(986, process_battle_986_logic, 500);

	register_logic_function(988, process_battle_988_logic, 500);

	register_logic_function(989, process_battle_989_logic, 500);

	register_logic_function(990, process_battle_990_logic, 500);

	register_logic_function(991, process_battle_991_logic, 200);

	register_logic_function(993, process_battle_993_logic, 500);

	register_logic_function(701, process_battle_701_logic, 500);
	
	register_logic_function(702, process_battle_702_logic, 1000);

	register_logic_function(703, process_battle_703_logic, 1000);

	register_logic_function(995, process_battle_995_logic, 500);

	register_logic_function(996, process_battle_996_logic, 500);

	register_logic_function(997, process_battle_997_logic, 500);

	register_logic_function(705, process_battle_705_logic, 500);
	register_logic_function(719, process_battle_719_logic, 200);
	register_logic_function(721, process_battle_721_logic, 500);

	register_logic_function(718, process_battle_718_logic, 500);

	register_logic_function(734, process_battle_734_logic, 100);

	//init logic funtion register
	register_init_logic_function(925, process_battle_925_init_logic);


	//final logic function register
}

void Battle::final_battle_logic()
{
	unregister_logic_function();
	unregister_init_logic_function();
	unregister_final_logic_function();
}

void Battle::add_buff_to_all_players(uint32_t buff_id, uint32_t creater_id)
{
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
		Player* p = *it;
		if(p == NULL )continue;
		add_player_buff(p, buff_id, creater_id);
	}
}

void Battle::del_buff_from_all_players(uint32_t buff_id)
{
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
		Player* p = *it;
		if(p == NULL )continue;
		del_player_buff(p, buff_id);
	}
}

void Battle::set_all_player_skills_ready()
{
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it)
	{
		Player* p = *it;
		if(p == NULL)continue;
		p->set_all_skills_ready();
	}
}

void Battle::set_all_player_skills_cd()
{
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it)
	{
		Player* p = *it;
		if(p == NULL)continue;
		p->set_all_skills_cd();
	}
}
/*
void Battle::set_all_map_summmon_dead()
{
}
*/
uint32_t Battle::get_lower_player_lv()
{
    PlayerVec::iterator it = players_.begin();
    uint32_t lower_lv = (*it)->lv;
    while (it != players_.end()) {
        if (lower_lv > (*it)->lv)
            lower_lv = (*it)->lv;
        ++it;
    }
    return lower_lv;
}

/**
 * @brief Here,only for pvp monster_
 */
uint32_t Battle::get_left_player_lv()
{
    PlayerVec::iterator it = players_.begin();
    uint32_t left_lv = 10;
    for (; it != players_.end(); ++it) {
        if (abs((*it)->pos().x() - 900) > abs((*it)->pos().x() - 300)) {
            left_lv = (*it)->lv;
        }
    }
    return left_lv;
}
/**
 * @brief Here,only for pvp monster_
 */
uint32_t Battle::get_right_player_lv()
{
    PlayerVec::iterator it = players_.begin();
    uint32_t right_lv = 10;
    for (; it != players_.end(); ++it) {
        if (abs((*it)->pos().x() - 900) <= abs((*it)->pos().x() - 300)) {
            right_lv = (*it)->lv;
        }
    }
    return right_lv;
}

void
Battle::start_battle(bool restart_flag)
{
	if(is_battle_pvp())
	{
		start_pvp_battle(this, restart_flag);
	}
	if(is_battle_pve())
	{
		start_pve_battle(this, restart_flag);
	}
	if (is_battle_tmp_team_pvp()) {
		start_pvp_battle(this, false);
	}
	//calc_player_battle_team buff
//	add_player_team_buff();
}

void
Battle::set_player_ready(Player* p)
{
	if (!is_battle_started()) {
		WARN_LOG("battle not started: uid=%u", p->id);
		send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
		return;
	}

	//monitor load time
	p->load_tm = get_now_tv()->tv_sec - p->load_tm;
	uint32_t tmp_tm = p->load_tm / 30;
	if (tmp_tm) {
		tmp_tm = tmp_tm > 14 ? 14 : tmp_tm;
		do_stat_item_log_2(stat_log_load_lag - 1, 0, tmp_tm, 1);
	}

	if (!test_bit_on(p->btl_status, player_ready_for_battle)) {
		++ready_num_;
	}

	uint32_t all_ready = 0;
	if (ready_num_ == players_.size()) {
		all_ready = 1;
	}

	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, p->id, idx);
	pack_h(pkgbuf, all_ready, idx);
	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	TRACE_LOG("SEND TO ONlINE %u %u", p->id, all_ready);

	if (test_bit_on(p->btl_status, player_ready_for_battle)) {
		send_to_player(p, pkgbuf, idx, 1);
		return;
	}

	p->cur_map->send_to_map(pkgbuf, idx, p);

	// test a player's latency at an interval of 60 secs
	ev_mgr.add_event(*p, &Player::test_lag, *get_now_tv(), 30000, -1);

	p->set_battle_status(player_ready_for_battle);

	KDEBUG_LOG(p->id, "RDY BTL\t[uid=%u %u %lu]", p->id, ready_num_, players_.size());

	if (ready_num_ == players_.size()) 
	{
		if (p->btl->is_battle_pvp() || p->btl->is_battle_tmp_team_pvp()) {
			battle_count_down(p);
		} else {
			set_battle_start(p);
		}
	}
}

void
Battle::set_team_btl_player_ready(Player* p, bool is_ready)
{	
	if (is_ready) {
		if (test_bit_on(p->btl_status, player_team_btl_wait_ready)) {
			return;
		}
		p->set_battle_status(player_team_btl_wait_ready);
		++btl_team_ready_num_;

		KDEBUG_LOG(p->id, "TEAM BTL S RDY\t[uid=%u %u %lu]", p->id, btl_team_ready_num_, players_.size());
		nodi_player_state_to_room(room_player_ready, p);
	} else {
		if (!test_bit_on(p->btl_status, player_team_btl_wait_ready)) {
			return;
		}
		p->set_battle_status_off(player_team_btl_wait_ready);
		--btl_team_ready_num_;

		KDEBUG_LOG(p->id, "TEAM BTL C RDY\t[uid=%u %u %lu]", p->id, btl_team_ready_num_, players_.size());
		nodi_player_state_to_room(room_player_cancel_ready, p);
	}
}

void
Battle::battle_count_down(Player* p)
{
	// notify the players that the battle has been started
	uint32_t count_down_tm = 3;
	if (this->is_battle_pvp() || this->is_battle_tmp_team_pvp()) {
		count_down_tm = 8;
	}

	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_battle_count_down, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	p->cur_map->send_to_map(pkgbuf, idx);
	
	timeval tv = *get_now_tv();
	tv.tv_sec += count_down_tm;
	ev_mgr.add_event(*p, &Player::pvp_battle_start, tv);
	
	p->cur_map->send_players_info(0);
	KDEBUG_LOG(p->id, "BTL COUNT DOWN\t[uid=%u tm=%u]", p->id, count_down_tm);

	//for watchers;
	notify_pvp_room_start(0, this->id(), this->pvp_lv());
	this->watch_status_ = 2;

	for (PlayerVec::iterator it = players_.begin(); it != players_.end();
			++it) {
		(*it)->noti_hpmp_to_btl();
	}

	if (!watchers_.empty()) {
		int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
		uint32_t svr_id = get_server_id();
		uint32_t room_id = this->id();
		uint32_t room_status = 2;
		pack(pkgbuf, svr_id, idx);
		pack(pkgbuf, room_id, idx);
		pack(pkgbuf, room_status, idx);
		init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_contest_change_status, idx - sizeof(btl_proto_t));
		init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
		this->send_to_watcher(pkgbuf, idx);
	}
}

void
Battle::set_battle_start(Player *p)
{
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) 
	{
		Player* soldier = *it;
		soldier->set_battle_status(player_in_battle);

		if (is_battle_pve()) 
		{
			//auto regenerate hpmp
			ev_mgr.add_event(*soldier, &Player::auto_regen_hpmp, *get_now_tv(), 10000, -1);
			// reduce jewelry's duration
			if (soldier->clothes_info.jewelry_cnt > 0) {
				ev_mgr.add_event(*soldier, &Player::reduce_jewelry_duration, *get_now_tv(), 60000, -1);
			}
#ifdef DEV_SUMMON
			//summon monster
			if (soldier->my_summon) {
				soldier->my_summon->summon_info.active_flag = true; 
				//ev_mgr.add_event(*soldier, &Player::auto_reduce_fight_value, *get_now_tv(), 60000, -1);
			}
#endif
			soldier->set_app_buf_start(get_now_tv()->tv_sec);
			//pve 3s 无敌
			add_player_buff(soldier, 1341, 0, 3, false);
		} 
		else 
		{
			//pvp时，屏蔽各种人物身上保存的buff效果
            //soldier->set_app_buf_start(get_now_tv()->tv_sec);
			uint32_t tmp_buf[2] = {0};
			if (pvp_lv() < pvp_monster_ && pvp_lv() > pvp_lv_0) {
				tmp_buf[pvp_lv()- 1] = 1;
				do_stat_item_log_4(stat_log_pvp_join, 0, 0, tmp_buf, sizeof(tmp_buf));
			}
			if (soldier->my_summon) {
				soldier->my_summon->summon_info.active_flag = true; 
			}

			if (pvp_lv() == pvp_dragon_ship) {
				add_player_buff(soldier, 28, 0, 1000, false);
			}
		
		}
		//初始化各种被动技能
		soldier->init_player_passive_skill();
		
		
		//stage_score
		soldier->score.start_tm = get_now_tv()->tv_sec;
		start_tm = *get_now_tv();
		btl_section_routing_tm = start_tm;

		if (stage()->id == 927 || stage()->id == 929 ) {
			soldier->score.all_mon_cnt = 100;
		} else if (stage()->id == 932 || stage()->id == 933 || stage()->id == 934 || 
			stage()->id == 953) {
			soldier->score.all_mon_cnt = 1;
		} else {	
			soldier->score.all_mon_cnt = mon_num_;
		}
		soldier->score.map_num = stage_->map_num;
	}
	TRACE_LOG("call_monsters_timer create %u", id());
	//if (ev_mgr.add_event
	if (this->pvp_lv() == pvp_ghost_game) {
		ev_mgr.add_event(*this, &Battle::call_monsters_timer, *get_now_tv(), 10000, -1);
	} else {
		ev_mgr.add_event(*this, &Battle::call_monsters_timer, *get_now_tv(), 45000, -1);
	}


	// notify the players that the battle has been started
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_battle_start, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	p->cur_map->send_to_map(pkgbuf, idx);
	init_map_->activate_monsters();
	
	//noti to player all info
	if (is_battle_pve()) {
		p->cur_map->send_players_info(0);
		p->cur_map->send_players_status_info(p, false);
	}
	btl_start_tm = get_now_tv()->tv_sec;
	init_battle_logic();

	do_after_start_btl();
	KDEBUG_LOG(p->id, "BTL STARTED\t[uid=%u]", p->id);

}

void
Battle::do_after_start_btl()
{
	if (is_battle_pvp()) {
		if (this->pvp_lv() == pvp_eve_of_16_fight_1) {
			//reduce_players_exploit(500);
		} else if (this->pvp_lv() == pvp_eve_of_16_fight_2) {
			//reduce_players_exploit(1000);
		} else if (this->pvp_lv() == pvp_eve_of_16_fight_3) {
			//reduce_players_exploit(1500);
		} else if (this->pvp_lv() == pvp_eve_of_16_fight_4) {
			//reduce_players_exploit(2000);
		}
	}
}

void Battle::reduce_players_exploit(uint32_t reduce_num)
{
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
		Player * player = *it;
		player->reduce_exploit(reduce_num);
	}
}


void
Battle::add_tmp_buf(Player* p)
{
	if (players_.size() > 1) {
		uint32_t buff_id1 = 0;
		uint32_t buff_id2 = 0;
		PlayerVec::iterator it1 = players_.begin();
		uint32_t show_state1 = (*it1)->show_state;
		++it1;
		uint32_t show_state2 = (*it1)->show_state;

		TRACE_LOG("add_tmp_buf: %u %u %u", p->id, show_state1, show_state2);
		if (show_state1 && show_state2) {
			if (show_state1 == show_state2) {
				if (show_state1 == small_moon_chip) {
					buff_id1 = 602;
				} else {
					buff_id1 = 601;
				}
			} else {
				buff_id1 = 601;
				buff_id2 = 602;
			}
			TRACE_LOG("Player add buff[%u %u %u]", p->id, show_state1, (uint32_t)time(NULL));
			if (buff_id1) {
				add_player_buff(p, buff_id1, 0);
			}
			if (buff_id2) {
				add_player_buff(p, buff_id2, 0);
			}

		}
	}
}

int
Battle::start_team_battle(Player* p)
{
	p->set_battle_status(player_team_btl_wait_ready);
	++btl_team_ready_num_;

	DEBUG_LOG("START TEAM BTL\t[uid=%u %u %lu]", p->id, btl_team_ready_num_, players_.size());

	start_battle();
	return 0;
}


void
Battle::send_btl_over_pkg(Player* p, bool win_flag)
{
	TRACE_LOG("send_btl_over_pkg:%u", p->id);
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, (uint32_t)(btl_mode_), idx);
	pack_h(pkgbuf, end_reason_, idx);
	if( win_flag ){
		pack_h(pkgbuf, p->id, idx);
	}else {
		pack_h(pkgbuf, static_cast<uint32_t>(0), idx);
	}

	uint32_t pass_time = get_now_tv()->tv_sec - btl_start_tm;

	pack_h(pkgbuf, static_cast<uint32_t>(stage_->id), idx);
	pack_h(pkgbuf, static_cast<uint32_t>(stage_->boss_id), idx);
	pack_h(pkgbuf, static_cast<uint32_t>(difficulty()), idx);
	pack_h(pkgbuf, static_cast<uint8_t>(p->get_stage_grade()), idx);
	pack_h(pkgbuf, static_cast<uint32_t>(p->damage), idx);
	pack_h(pkgbuf, pass_time, idx);
	pack_h(pkgbuf, p->use_skill_cnt, idx);
	pack_h(pkgbuf, p->be_attacked_cnt, idx);
	init_btl_proto_head(pkgbuf, btl_battle_over, idx);
	send_to_player(p, pkgbuf, idx, 0);

	if (this->watch_status_) {
		this->watch_status_ = 0;
		if (!this->watchers_.empty()) {
			int idx = sizeof(btl_proto_t);
			pack_h(pkgbuf, winner_, idx);
			init_btl_proto_head(pkgbuf, btl_contest_watch_over, idx);
			this->send_to_watcher(pkgbuf, idx);
		}
	}

	if (winner_ == p->id && is_battle_pvp() && pvp_lv() == pvp_16_contest) {
		notify_pvp_over_2_switch(id_, p->id, pvp_lv(), common_flag1_); 
	}

}

int Battle::save_pvp_taotai_game_data(Player* p, bool win_flag)
{
    int idx = 0;
    pack_h(dbpkgbuf, p->id, idx);
    pack_h(dbpkgbuf, p->role_tm, idx);
    //pack_h(dbpkgbuf, p->role_type, idx);
    //pack(dbpkgbuf, p->nick, sizeof(p->nick), idx);
    //pack_h(dbpkgbuf, p->team_number, idx);
    //p->taotai_do_times += 1;
    if ( win_flag ) {
        pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx);
        //p->taotai_win_times += 1;
    } else {
        pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);
    }
	
    KDEBUG_LOG(p->id,"SAVE PVP MONSTAR GAME\t[%u %u] %u", p->taotai_do_times, p->taotai_win_times, win_flag);
    //send_request_to_db(0, p->id, p->role_tm, dbproto_save_pvp_game_info, dbpkgbuf, idx);
    return 0;
}

void
Battle::on_boss_dead()
{
}


void
Battle::set_all_monsters_dead()
{
	// init monsters
	TRACE_LOG("%u %u", id_, map_num_);
	for (int i = 0; i != stage_max_map_num; ++i) {
		map_t *m = &(maps_[i]);
		if (m->id) {
			m->set_all_monsters_dead();
		}
	}
}

void Battle::notify_client_player_killed_monster(uint32_t uid, uint32_t mon_type)
{
    for (int i = 0; i != stage_max_map_num; ++i) {
		map_t *m = &(maps_[i]);
		if (m->id) {
			//m->set_special_monster_dead(mon_type);
            PlayerSet::iterator it = m->monsters.begin();
            for (; it != m->monsters.end(); ++it) {
                Player* monster = *it;
                if ( !(monster->is_dead()) && (monster->role_type == mon_type) ) {
                    //monster->suicide();
                    monster->set_dead();
                    monster->notify_damage_by_buff(uid, 0, monster->max_hp() + 1);
                }
            } //end for
		} //end if
	}
}

void Battle::set_all_players_relive()
{
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) 
	{
		Player* p = *it;
		if(p == NULL)continue;
		if( !p->is_dead())continue;
		p->set_revive();
	}
}

void
Battle::on_btl_over(Player* p, bool flag)
{
	DEBUG_LOG("B E\t[%u %u]", stage_->id, id_);
	set_battle_status(battle_ended);
	end_reason_ = end_btl_normal;
	winner_ = players_[0]->id;
	set_all_monsters_dead();
	//p->cur_map->set_all_monsters_dead();
	mon_num_ = 0;

	//uint32_t winner = 0;

	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
		//stage_score
		(*it)->score.end_tm = get_now_tv()->tv_sec;

		if (p) {
			TRACE_LOG("boss dead[%u %u]",(*it)->id, p->role_type);
		
			if(  (p->role_type == 13054) ||  (is_valid_uid(p->id) && stage_->id == 921)  ){
				//功夫宝典被打死战斗结束
				end_reason_ = end_stage_lose;
				send_btl_over_pkg(*it, false);   
				continue;
			}
			if(  (p->role_type == 13084) ||  (is_valid_uid(p->id) && stage_->id == 925)  ){
				//大炮被打掉后战斗结束
				end_reason_ = end_stage_lose;
			    send_btl_over_pkg(*it, false);
				continue;	
			}
            if(  (p->role_type == 11451) ||  (is_valid_uid(p->id) && stage_->id == 963)  ){
				//大炮被打掉后战斗结束
				end_reason_ = end_stage_lose;
			    send_btl_over_pkg(*it, false);
				continue;	
			}

		}

		if (flag) { //时间用尽引起的战斗失败
			end_reason_ = end_stage_lose;
			send_btl_over_pkg(*it, false);
			continue;
		}
		

		if (is_team_btl()) {
			do_stat_team_enter_leave_battle(stage()->id, difficulty(), 0, 1, get_now_tv()->tv_sec - start_tm.tv_sec );
		} else {
			do_stat_enter_leave_battle(stage()->id, difficulty(), 0, 1, get_now_tv()->tv_sec - start_tm.tv_sec );
		}
		(*it)->calc_stage_score();

	
		//finished stage
		if (stage_) {
			
            if (stage_->id > 900) {
                do_stat_item_log_2(stat_log_spe_stage_userlv_finish, (*it)->lv, (stage_->id - 900) << 8, 1);
                if (stage_->id == 918) {
                    do_stat_item_log(0x09503103, stage_->id, (*it)->role_type, 1);
                } else if (stage_->id == 921) {
                    uint32_t stat_buf[3] = {0, 1, 0};
                    do_stat_item_log_4(0x09503015, 0, 0, stat_buf, sizeof(stat_buf));
                }
            } else {
                //do_stat_item_log_4(stat_log_stage_finish, stage_->id, (*it)->role_type, buf, sizeof(buf));
                do_stat_item_log(stat_log_stage_finish, stage_->id, (*it)->role_type, 1);
            }
			uint32_t id1 = 0;
			uint32_t buf[13] = { 0 };
			buf[0] = 1;
			buf[difficulty()] = 1;
			buf[4 + difficulty()] = (*it)->score.end_tm - (*it)->score.start_tm;
			if ((*it)->score.on_hit_cnt) {
				buf[8 + difficulty()] = 1;
			}

			if (stage_->id > 3 && stage_->id < 256) {
				id1 = ((stage_->id - 3) << 8);
			} else if (stage_->id == 917) {
                id1 = 0x40000;
            } else if (stage_->id == 2) { //bishuidongxue
                id1 = 0xff00;
            } else if (stage_->id == 909) {
            	id1 = 0x38a00;
            } else if (stage_->id == 917) {
            	id1 = 0x40000;
            }
			do_stat_item_log_3(stat_log_stage_user_lv, id1,(*it)->lv, 1, buf, sizeof(buf));
		}
	}

	//unregister_logic_function();	
	final_battle_logic();
}

void
Battle::over_pvp_939_stage(uint32_t winner_team)
{
    on_boss_dead();
    set_battle_status(battle_ended);
    end_reason_ = end_btl_normal;
    set_all_monsters_dead();

    for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
        TRACE_LOG("trace uid=[%u] [%u %u]",(*it)->id, (*it)->team, winner_team);

        if ((*it)->team == winner_team) {
            winner_ = (*it)->id;
            send_btl_over_pkg(*it);
            (*it)->calc_pvp_score(1);
            //if (pvp_lv() == pvp_monster_game) {
              //  (*it)->taotai_do_times += 1;
              //  (*it)->taotai_win_times += 1;
              //  save_pvp_taotai_game_data((*it), true);
            //}
        } else {
            send_btl_over_pkg(*it, false);
            (*it)->calc_pvp_score(0);
            //if (pvp_lv() == pvp_monster_game) {
            //    (*it)->taotai_do_times += 1;
            //    save_pvp_taotai_game_data((*it), false);
            //}
        }
    }

    final_battle_logic();
}

int Battle::sent_finish_tower_info2db(Player* p)
{
    int idx = 0;
    pack_h(dbpkgbuf, p->id, idx);
    pack_h(dbpkgbuf, p->role_tm, idx);
    pack_h(dbpkgbuf, p->role_type, idx);
    pack(dbpkgbuf, p->nick, sizeof(p->nick), idx);
    pack_h(dbpkgbuf, p->cur_tower_layer, idx);
    uint32_t interval = get_now_tv()->tv_sec - p->score.start_tm;
    pack_h(dbpkgbuf, interval, idx);
    pack_h(dbpkgbuf, static_cast<uint32_t>(p->lv), idx);
	
	DEBUG_LOG("FINISH TOWER FLOOR\t[%u %u %u %u]", p->id, p->role_tm, p->cur_tower_layer, interval);
    return send_request_to_db(0, p->id, p->role_tm, dbproto_synchro_tower_info, dbpkgbuf, idx);
}
void
Battle::on_player_dead(Player* p)
{
	if(is_battle_pve())
	{
		on_pve_player_dead(this, p);
	}
	if(is_battle_pvp() || is_battle_tmp_team_pvp())
	{
		on_pvp_player_dead(this, p);
	}
}

void
Battle::on_player_use_plugin(Player* p)
{
	end_reason_ = end_boost_plugin;
	send_btl_over_pkg(p);
}

void
Battle::noti_online_all_player_joined_ready_to_start()
{
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, uint32_t(stage()->id), idx);
	pack_h(pkgbuf, btl_mode_, idx);
	init_btl_proto_head(pkgbuf, btl_noti_online_all_player_joined_ready_to_start, idx);
	this->send_to_btl(pkgbuf, idx);
}

//---------------------------------------------------------------------
// Private
//
void
Battle::send_btl_ready_noti()
{
	// notify the players that the battle has been started
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, init_map_->id, idx);
	TRACE_LOG("stage init map[%u]", init_map_->id);
	pack(pkgbuf, static_cast<uint32_t>(players_.size()), idx);
	TRACE_LOG("stage player num[%u %u]", (uint32_t)players_.size(), (uint32_t)(players_.size() + stage_->mon_set.size()));
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
		Player* p = *it;
		p->load_tm = get_now_tv()->tv_sec;
		pack(pkgbuf, p->role_type, idx);
		pack(pkgbuf, p->id, idx);
		pack(pkgbuf, p->nick, max_nick_size, idx);
		pack(pkgbuf, p->team, idx);


		//pack skill_res need info
		int ifx = idx + 4;
		uint32_t skill_res_cnt = 0;
		for ( SkillMap::iterator it = p->skills_map.begin(); it != p->skills_map.end(); ++it ) {
			player_skill_t & skill = it->second;
			if (skill.flag) {
				pack(pkgbuf, skill.skill_id, ifx);
				pack(pkgbuf, skill.lv, ifx);
				skill_res_cnt++;
				TRACE_LOG("ALL SKILL %u %u %u", skill.skill_id, skill.lv, skill.flag); 
			}
		}
		pack(pkgbuf, skill_res_cnt, idx);
		idx = ifx;

		//pack clothes info
		pack(pkgbuf, static_cast<uint32_t>(p->clothes_info.clothes_cnt), idx);
		TRACE_LOG("player clothes cnt[%u %u]", p->id, p->clothes_info.clothes_cnt);
		for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i) {
			pack(pkgbuf, p->clothes_info.clothes[i].clothes_id, idx);
			TRACE_LOG("player clothes:[%u %u]",p->id, p->clothes_info.clothes[i].clothes_id);
		}	
#ifdef DEV_SUMMON
		//pack summon info
		uint32_t summon_cnt = p->my_summon ? 1 : 0;
		pack(pkgbuf, summon_cnt, idx);
		if (summon_cnt > 0) {
			pack(pkgbuf, p->my_summon->role_type, idx);
			pack(pkgbuf, static_cast<uint32_t>(p->my_summon->skills_map.size()), idx);
			for ( SkillMap::iterator it = p->my_summon->skills_map.begin(); it != p->my_summon->skills_map.end(); ++it ) {
				pack(pkgbuf, it->second.skill_id, idx);
				pack(pkgbuf, it->second.lv, idx);
				TRACE_LOG("monster skill[%u %u]", p->my_summon->role_type, it->second.skill_id);
			}
		}
#endif
        if (pvp_type() != 0) {
            uint32_t stat_id = ((pvp_type() - 1) * 2) << 8;
            do_stat_item_log_2(stat_log_invite_pvp_lv, stat_id, p->lv, 1);
            do_stat_item_log_2(stat_log_invite_pvp_role, stat_id, p->role_type, 1);
            do_stat_item_log_5(stat_log_invite_pvp_times, stat_id, p->id, 1);
        }
	}
    if (pvp_type() != 0) {
        uint32_t stat_id = ((pvp_type() - 1) * 2) << 8;
        do_stat_item_log_2(stat_log_invite_pvp_times, 0, stat_id, 1);
    }

	pack(pkgbuf, (int)stage_->map_num, idx);
	for (uint16_t i = 0; i < stage_max_map_num; i++) {
		const map_t * m = &(stage_->maps[i]);
		if (m->id) {
			pack(pkgbuf, m->id, idx);
			uint32_t monster_cnt = m->mons->size() + m->rt_birth_mons->size(); 
			pack(pkgbuf, monster_cnt, idx);
			for (std::vector<map_monster_t>::iterator it = m->mons->begin(); it != m->mons->end();
					++it) {
				const monster_t * moninfo = get_monster(it->id, this->btl_difficulty_);
				pack(pkgbuf, moninfo->id, idx);
				pack(pkgbuf, moninfo->skill_num, idx);
				for (int i = 0; i != moninfo->skill_num; ++i) {
					pack(pkgbuf, moninfo->skills[i].skill_id, idx);
					pack(pkgbuf, moninfo->skills[i].lv, idx);
					TRACE_LOG("mon skill[%u %u]", moninfo->id, moninfo->skills[i].skill_id);
				}
			}

			for (std::vector<map_monster_t>::iterator it = m->rt_birth_mons->begin(); it != m->rt_birth_mons->end();
					++it) {
				const monster_t * moninfo = get_monster(it->id, this->btl_difficulty_);
				pack(pkgbuf, moninfo->id, idx);
				pack(pkgbuf, moninfo->skill_num, idx);
				for (int i = 0; i != moninfo->skill_num; ++i) {
					pack(pkgbuf, moninfo->skills[i].skill_id, idx);
					pack(pkgbuf, moninfo->skills[i].lv, idx);
					TRACE_LOG("mon skill[%u %u]", moninfo->id, moninfo->skills[i].skill_id);
				}
			}
		}
	}

	//pack random monster info in stage

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_battle_ready, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	init_map_->send_to_map(pkgbuf, idx);

	noti_online_all_player_joined_ready_to_start();
}

//----------------------------------------------------------
// static methods
//
void
Battle::set_pvp_matched(Player* p, uint8_t type, Player* first_p)
{
	p->team = player_team_2;
    if (first_p->btl->pvp_lv() == pvp_monster_) {
        p->team = player_team_1;
    }
	add_player(p, 2);
	uint32_t all_ready = 0;
	if (players_.size() == 2) {
		all_ready = 1;
	}
	send_lv_match_rsp(p, btl_mode_lv_matching, type, first_p, all_ready);
	start_battle();
}

void
Battle::set_tmp_team_pvp_team_side(Player* p)
{
	PlayerVec::iterator it = players_.begin();
	for (; it != players_.end(); ++it) {
		Player* p_tmp = (*it);
		if (p_tmp->tmp_team_id == p->tmp_team_id) {
			p->team = p_tmp->team;
			return;
		}
	}
}

int
Battle::pack_tmp_team_match_rsp(uint8_t* buf, uint32_t battle_mode, uint32_t _id)
{
	int idx = sizeof(btl_proto_t);
	pack_h(buf, battle_mode, idx); //battle_mode
	pack_h(buf, _id, idx); 
	return idx;
}


void
Battle::send_tmp_team_match_rsp(Player* p, uint32_t battle_mode)
{
	int idx = 0;
    uint32_t roomid = p->btl->global_room_id();

	idx += pack_tmp_team_match_rsp(pkgbuf, battle_mode, roomid);
	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);
}


void
Battle::send_lv_match_rsp(Player* p, uint32_t battle_mode, uint8_t type, Player* first_player, uint32_t all_ready)
{
	int idx = 0;
	uint32_t flag = 0;
    uint32_t roomid = p->btl->global_room_id();

	if (first_player) {
		idx += pack_lv_match_rsp(pkgbuf, battle_mode, first_player ? first_player->id : 0, type, roomid, all_ready);
		if (!first_player->continue_win && !(p->btl_again)) {
			if (p->btl->pvp_type() == pvp_type_radom) {
				first_player->reduce_pvp_coins();
			}
			first_player->need_sav_ = true;
		}
		init_btl_proto_head(pkgbuf, p->waitcmd, idx);
		send_to_player(first_player, pkgbuf, idx, 0);

		if (!p->continue_win && !(p->btl_again)) {
			flag = 1;
		}
	}
	
	idx = 0;
	idx += pack_lv_match_rsp(pkgbuf, battle_mode, first_player ? first_player->id : 0, type, roomid, all_ready);
	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	if (flag) {
		if (p->btl->pvp_type() == pvp_type_radom) {
			p->reduce_pvp_coins();
		}
		p->need_sav_ = true;
	}
	send_to_player(p, pkgbuf, idx, 1);
	return;
}

void
Battle::send_lv_match_rsp_in_timer(Player* p, uint32_t battle_mode, Player* first_player)
{
	int idx = 0;
    uint32_t roomid = first_player->btl->global_room_id();
	//idx += pack_lv_match_rsp(pkgbuf, battle_mode, first_player->id, first_player->btl->id());
	idx += pack_lv_match_rsp(pkgbuf, battle_mode, first_player->id, 0, roomid);
	init_btl_proto_head(pkgbuf, btl_lv_matching_battle, idx);
	send_to_player(first_player, pkgbuf, idx, 0);
	if (!first_player->continue_win && !(first_player->btl_again)) {
		first_player->reduce_pvp_coins();
		first_player->need_sav_ = true;
	}
	idx = 0;
	//idx += pack_lv_match_rsp(pkgbuf, battle_mode, p->id, first_player->btl->id());
    idx += pack_lv_match_rsp(pkgbuf, battle_mode, first_player->id, 0, roomid);
	init_btl_proto_head(pkgbuf, btl_lv_matching_battle, idx);
	send_to_player(p, pkgbuf, idx, 0);
	if (!p->continue_win && !(first_player->btl_again)) {
		p->reduce_pvp_coins();
		p->need_sav_ = true;
	}

	return;
}

int
Battle::pack_lv_match_rsp(uint8_t* buf, uint32_t battle_mode, userid_t first_id, uint8_t type, uint32_t _id, uint32_t all_ready)
{
	int idx = sizeof(btl_proto_t);
	pack_h(buf, battle_mode, idx); //battle_mode
	pack_h(buf, first_id, idx); // id of the first player
	pack_h(buf, type, idx); 
	pack_h(buf, _id, idx); 
	pack_h(buf, all_ready, idx);
	return idx;
}

int
Battle::call_monsters_timer()
{
	PlayerVec::iterator it = this->players_.begin();
	map_t* p_cur_map = (*it)->cur_map;
	uint32_t mon_cnt = p_cur_map->rt_birth_mons->size();
	TRACE_LOG("call_monsters_timer in %u %u", id(), mon_cnt);
	if (!mon_cnt) {
		TRACE_LOG("mon_cnt %u", mon_cnt);
		return 0;
	}
	map_t::MapMonVec::iterator itset = p_cur_map->rt_birth_mons->begin();
	for ( ; itset != p_cur_map->rt_birth_mons->end(); ++itset) {
		if (std::find_if(p_cur_map->barriers.begin(), p_cur_map->barriers.end(), finder_t((*itset).id))
			 != p_cur_map->barriers.end()) {
			 TRACE_LOG("have mon %u", (*itset).id);
			 return 0;
		}
	}
	int rand_num = rand() % mon_cnt;
	
	map_monster_t* p_mon = &((*(p_cur_map->rt_birth_mons))[rand_num]);
	TRACE_LOG("in call mons %u %u %u", mon_cnt, rand_num, p_mon->id);
	call_monster_to_map(p_cur_map, this, p_mon->id, p_mon->x, p_mon->y, barrier_team);
	return 0;
}

void
Battle::nodi_player_enter_state_to_room(Player* p)
{
	TRACE_LOG("ID:%u P:%u", id_, p->id);
	//PlayerVec::iterator it = players_.begin();
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	idx += p->pack_basic_info(pkgbuf + idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_notify_player_enter_state_in_room, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_btl(pkgbuf, idx, p, 0);
}

void
Battle::nodi_player_leave_state_to_room(Player* p)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, p->id, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_notify_player_leave_state_in_room, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_btl(pkgbuf, idx, p, 0);
}

void
Battle::notify_team_member_enter_map(Player* p, uint32_t map_id)
{
	TRACE_LOG("p:%u m:%u", p->id, map_id);
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, map_id, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_notify_team_member_enter_map, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_btl(pkgbuf, idx, p, 0);
}

void
Battle::nodi_player_state_to_room(player_action_in_room type, Player* p)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, uint32_t(type), idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_notify_player_state_in_room, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_btl(pkgbuf, idx, p, 0);
}

void
Battle::enter_room(Player* p)
{
	DEBUG_LOG("E R\t[%u %u]", id_, p->id);
	int idx = sizeof(btl_proto_t);

	idx += pack_join_btl_rsp(pkgbuf + idx, p);

	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	// nodify to all the player in rool
	nodi_player_enter_state_to_room(p);

	if (is_team_btl() && !local_flg) {
	// nodify to battle switch
		notify_join_room(p, id_);
	}

	p->set_player_in_team_btl();
	// set player ready
	//set_team_btl_player_ready(p);	
}

void
Battle::leave_room(Player* p)
{
	DEBUG_LOG("L R :%u %u", id_, p->id);
	if (test_bit_on(p->btl_status, player_team_btl_wait_ready)) {
		--btl_team_ready_num_;
	}
	
	if (p->is_team_leader()) {
		PlayerVec::iterator it = players_.begin();
		for (; it != players_.end(); ++it) {
			if ((*it)->id != p->id) {
				(*it)->set_player_leader(p);
				break;
			}
		}	
	}
	// nodify to all the player in rool
	if (!(p->is_team_leader())) {
		nodi_player_leave_state_to_room(p);
	}
	// nodify to battle switch
	notify_leave_room(p, id_);
}

int
Battle::pack_join_none_btl_rsp(uint8_t* buf)
{
	int idx = 0;
	pack_h(buf, btl_mode_pve, idx); //battle_mode
	pack_h(buf, 0, idx);
	pack_h(buf, 0, idx);
	pack_h(buf, 0, idx);
	pack_h(buf, 0, idx);
	return idx;
}


int
Battle::pack_join_btl_rsp(uint8_t* buf, Player* p)
{
	int idx = 0;
	pack_h(buf, btl_mode_pve, idx); //battle_mode
	pack_h(buf, global_room_id(), idx);
	pack_h(buf, static_cast<uint32_t>(stage()->id), idx);
	pack_h(buf, static_cast<uint32_t>(difficulty()), idx);
	int idxtmp = idx;
	idx += 4;
	
	uint32_t cnt = 0;
	PlayerVec::iterator it = players_.begin();
	for (; it != players_.end(); ++it) {
		if (p->id != (*it)->id) {
			cnt ++;
			idx += (*it)->pack_h_basic_info(buf + idx);
			TRACE_LOG("%u %u %s %u %u", (*it)->id, (*it)->clothes_info.clothes_cnt, (*it)->nick, *(uint32_t*)(buf+20), idx);
			
		}
	}
	
	pack_h(buf, cnt, idxtmp);
	return idx;
}

void
Battle::monster_private_drop(Player* p, Player* killer)
{
	PlayerSet::iterator it = p->cur_map->players.begin();
	for (; it != p->cur_map->players.end(); ++it) {
		monster_drop_item_to_player(p, (*it));
	}
}


bool        Battle::init_roll_data()
{
	m_base_roll_id = 0;
	m_roll_datas.clear();
	m_prepare_roll_datas.clear();
	return true;
}

bool        Battle::final_roll_data()
{
	m_base_roll_id = 0;
	std::vector<battle_roll_data*>::iterator pItr = m_roll_datas.begin();
	for(; pItr != m_roll_datas.end(); ++pItr)
	{
		battle_roll_data* data = *pItr;
		destroy_battle_roll_data(data);
	}
	m_roll_datas.clear();


	std::list<battle_roll_data*>::iterator pItr2 = m_prepare_roll_datas.begin();
	for(; pItr2 != m_prepare_roll_datas.end(); ++pItr2)
	{
		battle_roll_data* data2 = *pItr2;
		destroy_battle_roll_data(data2);
	}
	m_prepare_roll_datas.clear();
	return true;
}

battle_roll_data* Battle::get_battle_roll_data_by_roll_id(uint32_t roll_id)
{
	for(uint32_t i = 0; i< m_roll_datas.size(); i++)
	{
		battle_roll_data* pData = m_roll_datas[i];
		for(uint32_t j =0; j < pData->roll_datas.size(); j++)
		{
			if( pData->roll_datas[j].roll_id == roll_id)
			{
				return pData;
			}
		}
	}
	return NULL;
}

uint32_t    Battle::get_next_roll_id()
{
	m_base_roll_id++;
	return m_base_roll_id;
}


bool       Battle::del_player_from_roll(uint32_t player_id)
{
	std::vector<battle_roll_data*>::iterator pItr = m_roll_datas.begin();
	for(;  pItr != m_roll_datas.end(); ++pItr)
	{
		(*pItr)->del_player_id(player_id);
	}

	std::list<battle_roll_data*>::iterator pItr2 = m_prepare_roll_datas.begin();
	for(; pItr2 != m_prepare_roll_datas.end(); ++pItr2)
	{
		(*pItr2)->del_player_id(player_id);
	}

	return true;
}


bool        Battle::add_roll_data(map_t* map, battle_roll_data* data)
{
	m_roll_datas.push_back(data);
	
	timeval tv = *get_now_tv();
	tv.tv_sec += ROLL_EXPIRE_TIME;
	ev_mgr.add_event(*data, &battle_roll_data::timer_roll_result, tv, 0, 0);
	return notify_team_map_roll_item(map, data);
}

bool       Battle::add_roll_data(battle_roll_data* data)
{
	m_roll_datas.push_back(data);

	timeval tv = *get_now_tv();
	tv.tv_sec += ROLL_EXPIRE_TIME;
	ev_mgr.add_event(*data, &battle_roll_data::timer_roll_result, tv, 0, 0);
	return notify_team_roll_item(data);	
}

bool       Battle::del_roll_data( battle_roll_data* data)
{
	std::vector<battle_roll_data*>::iterator pItr = std::find( m_roll_datas.begin(),  m_roll_datas.end(), data);
	if( pItr == m_roll_datas.end()){
		return false;
	}
	m_roll_datas.erase(pItr);
	destroy_battle_roll_data(data);
	return true;
}

bool        Battle::notify_team_roll_point(uint32_t roll_id, uint32_t player_id, int roll_point, uint32_t except_player_id)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, player_id, idx);
	pack(pkgbuf, roll_id, idx);
	pack(pkgbuf, roll_point, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_roll_item, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it)
	{
		Player* p = *it;
		if(p->id == except_player_id)continue;
		send_to_player(p, pkgbuf, idx, 0);	
	}	
	return true;
}


bool    Battle::notify_team_roll_item(battle_roll_data* data)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf,  data->btl->id(), idx);
	uint32_t count = data->roll_datas.size();
	pack(pkgbuf,  count, idx);

	for(uint32_t i=0; i< data->roll_datas.size(); i++)
	{
		pack(pkgbuf, data->roll_datas[i].roll_id, idx);
		pack(pkgbuf, data->roll_datas[i].item_id, idx);
		pack(pkgbuf, data->roll_datas[i].item_count, idx);
	}	
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_roll_item_list, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	/*	
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it)
	{
		Player* p = *it;
		send_to_player(p, pkgbuf, idx, 0);
	}
	*/
	for(uint32_t i=0; i< MAX_ROLL_PLAYER_COUNT; i++)
	{
		if( data->player_ids[i] == 0)continue;
		Player* p = get_player_in_btl( data->player_ids[i]);
		if( p == NULL)continue;
		send_to_player(p, pkgbuf, idx, 0);
	}	
	return true;
}


void Battle::notify_player_speed_change(Player* p)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, p->get_speed(), idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t),  cli_proto_notify_speed_change, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it)
	{
		Player* p = *it;
		send_to_player(p, pkgbuf, idx, 0);
	}
	TRACE_LOG("TEST	%u	%u", p->id, p->get_speed());
}

void Battle::notify_open_box_times(Player* p, uint32_t times)
{
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, times, idx);
	init_btl_proto_head(pkgbuf, btl_open_box_times, idx);
	send_to_player(p, pkgbuf, idx, 0);
}

void Battle::notify_auto_del_item(Player* p, mechanism_del_item_t* p_item_arr, uint32_t item_count)
{
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, item_count, idx);
	for (uint32_t i = 0; i < item_count; i++) {
		pack_h(pkgbuf, p_item_arr[i].itemid, idx);
		pack_h(pkgbuf, p_item_arr[i].itemcnt, idx);
	}
	
	init_btl_proto_head(pkgbuf, btl_auto_del_item, idx);
	send_to_player(p, pkgbuf, idx, 0);
}

bool  Battle::notify_team_get_item(uint32_t roll_id, uint32_t player_id, uint32_t item_id, uint32_t unique, int sucess)
{
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, roll_id, idx);
    pack_h(pkgbuf, player_id, idx);
	pack_h(pkgbuf, item_id, idx);
	pack_h(pkgbuf, unique, idx);
	pack_h(pkgbuf, sucess, idx);
    init_btl_proto_head(pkgbuf, btl_player_get_roll_item, idx);

	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it)
	{
		Player* p = *it;
		send_to_player(p, pkgbuf, idx, 0);					    
	}
    return true;	
}

Player* Battle::get_player_in_btl(uint32_t player_id)
{
	PlayerVec::iterator it = players_.begin();
	for (; it != players_.end(); ++it)
	{
		Player* p = *it;
		if( p->id == player_id ){
			return p;
		}
	}
	return NULL;
}

Player* Battle::get_an_live_enemy(Player* p)
{
	PlayerVec::iterator it = players_.begin();
	for (; it != players_.end(); ++it)
	{
		Player* p1 = *it;
		if( p->check_distance(p1, 2000) &&
			!(p1->is_dead()) && 
			p->team != p1->team && p1->team <= monster_team){
			return p1;
		}
	}

	if (p->cur_map) {
		for (PlayerSet::const_iterator it = p->cur_map->monsters.begin(); it != p->cur_map->monsters.end(); ) {
			Player* p1 = *it;
			++it;
			if( p->check_distance(p1, 2000) &&
				!(p1->is_dead()) && 
				p->team != p1->team && p1->team <= monster_team){
				return p1;
			}
		}
	}
	return NULL;
}

/*
void Battle::process_battle_45_logic(struct timeval  next_tm)
{
    if( battle_section_data == 0){
		uint32_t buff_id = ranged_random(22, 24);
		for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it)
		{
			Player* p = *it;
			if( p->cur_map->id != 1004507) continue;
			if( is_player_buff_exist(p, 22) )continue;
			if( is_player_buff_exist(p, 23) )continue;
			if( is_player_buff_exist(p, 24) )continue;
			add_player_buff(p, buff_id, 0);
		}
	}
}
*/

void Battle::battle_section_routing(struct timeval  next_tm)
{
	if (  taomee::timediff2(next_tm,  btl_section_routing_tm) >= (int32_t)time_frequency )
	{
		if( p_proc )
		{
			p_proc(this, next_tm);
		}
		btl_section_routing_tm = next_tm;
	}	
}

bool   Battle::notify_team_map_roll_point(map_t* map, uint32_t roll_id, uint32_t player_id, int roll_point, uint32_t except_player_id )
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, player_id, idx);
	pack(pkgbuf, roll_id, idx);
	pack(pkgbuf, roll_point, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_roll_item, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	map->send_to_map_except_player(pkgbuf, idx, except_player_id);
	return true;
}

bool   Battle::notify_team_map_roll_item(map_t* map, battle_roll_data* data)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf,  data->btl->id(), idx);
	uint32_t count = data->roll_datas.size();
	pack(pkgbuf,  count, idx);

	for(uint32_t i=0; i< data->roll_datas.size(); i++)
	{
		pack(pkgbuf, data->roll_datas[i].roll_id, idx);
		pack(pkgbuf, data->roll_datas[i].item_id, idx);
		pack(pkgbuf, data->roll_datas[i].item_count, idx);
	}
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_roll_item_list, idx - sizeof(btl_proto_t));	
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	map->send_to_map(pkgbuf, idx);
	return true;
}

bool   Battle::notify_team_map_get_item(map_t* map, uint32_t roll_id, uint32_t player_id, uint32_t item_id, uint32_t unique, int sucess)
{
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, roll_id, idx);
	pack_h(pkgbuf, player_id, idx);
	pack_h(pkgbuf, item_id, idx);
	pack_h(pkgbuf, unique, idx);
	pack_h(pkgbuf, sucess, idx);
	init_btl_proto_head(pkgbuf, btl_player_get_roll_item, idx);
	
	map->send_to_map(pkgbuf, idx);
	return true;
}



bool Battle::drop_item_to_team(map_t* map, uint32_t item_id, uint32_t x, uint32_t y)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	//const KfAstar::Points* points = map->path->find_surrounding_points(KfAstar::Point(x, y));
	const KfAstar::Points* points = NULL;
	KfAstar* p_star = get_cur_map_path( map );
	if(p_star)
	{
		points = p_star->find_surrounding_points(KfAstar::Point(x, y));
	}


	pack(pkgbuf, 1, idx);	
	idx += pack_item_drop(map, item_id, pkgbuf + idx, points);
	
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_monster_item_drop, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	map->send_to_map(pkgbuf, idx);
	return true;
}

///---------------------------------------------------------------------------------///

bool Battle::init_map_summon()
{
	map_summon_list.clear();
	return true;
}

bool Battle::final_map_summon()
{
	std::list<map_summon_object*>::iterator pItr = map_summon_list.begin();
	for(; pItr != map_summon_list.end(); ++pItr)
	{
		(*pItr)->final();
		delete *pItr;
	}
	map_summon_list.clear();
	return true;
}

bool Battle::add_map_summon(map_summon_object* p, bool notify)
{
	map_summon_list.push_back(p);
	if(notify)notify_add_map_summon_object(p);
	notify_strigger_map_summon_object_by_call(p);
	return true;
}

bool Battle::del_map_summon(map_summon_object* p)
{	
	std::list<map_summon_object*>::iterator pItr = std::find( map_summon_list.begin(),  map_summon_list.end(), p);
	if(pItr != map_summon_list.end()){
		notify_del_map_summon_object(p);
		p->final();
		delete p;
		pItr = map_summon_list.erase(pItr);
	}
	return true;	
}

bool Battle::del_map_all_summons_by_role_type(uint32_t role_type)
{
	std::list<map_summon_object*>::iterator pItr = map_summon_list.begin();
	while( pItr != map_summon_list.end())
	{
		map_summon_object* p = *pItr;
		if(p && p->model_ == role_type){
			notify_del_map_summon_object(p);
			p->final();
			delete p;
			p = NULL;
			pItr = map_summon_list.erase(pItr);
		}else{
			++pItr;
		}
	}
	return true;	
}

uint32_t Battle::get_map_summon_count(uint32_t owner_id)
{
	int count = 0;
	std::list<map_summon_object*>::iterator pItr = map_summon_list.begin();
	for(; pItr != map_summon_list.end(); ++pItr)
	{
		map_summon_object* p = *pItr;
		if(p && p->owner_id_ == owner_id){
			count++;
		}
	}
	return count;
}

bool Battle::del_map_summon_by_owner_id(uint32_t owner_id)
{
	std::list<map_summon_object*>::iterator pItr = map_summon_list.begin();
	while(pItr != map_summon_list.end())
	{
		map_summon_object* p = *pItr;
		if(p == NULL){
			++pItr;
			continue;
		}
		if( p->owner_id_ == owner_id){
			notify_del_map_summon_object(p);
			p->final();
			delete p;
			pItr = map_summon_list.erase(pItr);
		}else{
			++pItr;
		}
	}
	return false;
}

void Battle::map_summon_routing(struct timeval  next_tm)
{
	std::list<map_summon_object*>::iterator pItr = map_summon_list.begin();	
	uint32_t break_flag = 0;
	while( pItr != map_summon_list.end())
	{	
		std::list<Player*> temp_list;
		for( PlayerSet::iterator it = (*pItr)->cur_map_->players.begin();  it != (*pItr)->cur_map_->players.end(); ++it)
		{
			temp_list.push_back(*it);
		}
		for( PlayerSet::iterator it = (*pItr)->cur_map_->monsters.begin(); it != (*pItr)->cur_map_->monsters.end(); ++it)
		{
			temp_list.push_back(*it);
		}
		if( (*pItr)->action_type_ ==  action_dead_type )
		{
			if( (*pItr)->check_delete(next_tm))
			{			
				for( std::list<Player*>::iterator it = temp_list.begin();  it != temp_list.end(); ++it)
				{
					Player* p = *it;
					if( !(*pItr)->check_action_radius(p) )continue;
					if( !(*pItr)->check_target_type(p) )continue;
					notify_strigger_map_summon_object( *pItr);
					(*pItr)->process_buff(p);
					(*pItr)->process(p, next_tm);	
				}
				notify_del_map_summon_object(*pItr);
				pItr = map_summon_list.erase(pItr);
			}
			else
			{
				++pItr;
			}
			continue;
		}
		if( !(*pItr)->check_delay_time(next_tm) )
		{
			++pItr;
			continue;
		}
		break_flag = 0;
		for( std::list<Player*>::iterator it = temp_list.begin();  it != temp_list.end(); ++it)
		{
			if( (*pItr)->check_delete(next_tm)){
				notify_del_map_summon_object(*pItr);
				(*pItr)->final();
				delete (*pItr);
				pItr = map_summon_list.erase(pItr);
				break_flag = 1;
				break;
			}
			Player* p = *it;
			if( !(*pItr)->check_action_radius(p) )continue;
			if( !(*pItr)->check_target_type(p) )continue;
			notify_strigger_map_summon_object( *pItr);
			(*pItr)->process_buff(p);
			(*pItr)->process(p, next_tm);
		}
		if(break_flag == 0 )
		{		
			if( (*pItr)->action_type_ == action_times_type )
			{
				(*pItr)->action_times_ --;
			}	
			++pItr;
		}
	}
}

uint32_t Battle::get_dead_player_count()
{
	uint32_t ndead = 0;
	for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) 
	{
		if ((*it)->is_dead()) 
		{
			++ndead;
		}
	}
	return ndead;
}

void Battle::notify_first_boss_kill(uint32_t stage_id, uint32_t diffcult, uint32_t boss_id)
{
	int idx = sizeof(btl_proto_t);
	uint32_t kill_time = time(NULL);
	uint32_t id = diffcult * 10000 + stage_id;
	pack_h(pkgbuf, id, idx);
	pack_h(pkgbuf, boss_id, idx);
	pack_h(pkgbuf, kill_time, idx);
	init_btl_proto_head(pkgbuf, btl_first_kill_boss, idx);
	send_to_btl(pkgbuf, idx, NULL, 0);
}


void battle_statistics()
{
	static struct timeval s_update_tv   = *get_now_tv();
	
	const timeval* tv = get_now_tv();
	int time_elapsed = timediff2(*tv, s_update_tv);
	// update players
	if(time_elapsed > 60 * 1000){
		//statistic
        do_stat_item_log_2(0x09020901, 0, 0, pvp_players_statistics);
        do_stat_item_log_2(0x09020902, 0, 0, pve_players_statistics);
        do_stat_item_log_2(0x09020903, 0, 0, pve_team_players_statistics);

		s_update_tv = *tv;
		DEBUG_LOG("BATTLE STATISTIC\t[battle_id=%u [%u %u %u]]", get_server_id(), pvp_players_statistics, 
				pve_players_statistics, pve_team_players_statistics);
	}

}


/** 
 * @brief 处理关卡机关逻辑
 * @param mechanism
 * @param p
 */
void Battle::proc_battle_mechanism_opened(Player * mechanism, Player *p) 
{
	if (stage()->id == 702) {
		if(mechanism->role_type == 30017)
		{
			notify_delete_player_obj(mechanism);
			Player* boss = p->cur_map->get_boss();
			if (boss && mechanism && p->btl) {

				int x = ranged_random(2500, 4000);
				x = 0 - x;
				boss->chg_hp(x);
				boss->noti_hpmp_to_btl();
				if (boss->is_dead()) {
					p->btl->on_btl_over(p);
				} else {
					int rand_num = rand() % 100;
					if (rand_num < 100) {
						p->call_monster(11514, mechanism->pos().x(), mechanism->pos().y());
					}
				}
			}
		}
	}
}


/** 
 * @brief 处理关卡机关逻辑
 * @param mechanism
 * @param p
 */
void Battle::proc_battle_mechanism_logic(Player * mechanism, Player *p) 
{
	mechanism->mp = 2; //机关状态设置为更改完毕

	if (mechanism->role_type == 30001 
			|| mechanism->role_type == 30012) { //交互性宝箱掉落
		monster_drop_item(mechanism, p);
	} else if (mechanism->role_type == 30002) { // 火焰装置
		p->call_map_summon(10, mechanism->pos().x(), 234, false);
		p->call_map_summon(10, mechanism->pos().x(), 359, false);
		p->call_map_summon(10, mechanism->pos().x(), 501, false);
		p->cur_map->del_mechanism_from_map(mechanism->id);
	} else if (mechanism->role_type == 30003) { //沼泽装置
		p->call_map_summon(11, mechanism->pos().x(), 234, false);
		p->call_map_summon(11, mechanism->pos().x(), 359, false);
		p->call_map_summon(11, mechanism->pos().x(), 501, false);
		p->cur_map->del_mechanism_from_map(mechanism->id);
	} else if (mechanism->role_type == 30004) { //破坏装置
		p->call_map_summon(12, mechanism->pos().x(), 234, false);
		p->call_map_summon(12, mechanism->pos().x(), 359, false);
		p->call_map_summon(12, mechanism->pos().x(), 501, false);
		p->cur_map->del_mechanism_from_map(mechanism->id);
	} else if (mechanism->role_type == 30005) { //元素炼狱火buff机关
		if (is_player_buff_exist(p, 32)) {
			del_player_buff(p, 32);
		}	
		add_player_buff(p, 31, mechanism->id, 10);
	} else if (mechanism->role_type == 30006) { //元素炼狱冰buff机关
		if (is_player_buff_exist(p, 31)) {
			del_player_buff(p, 31);
		}
		add_player_buff(p, 32, mechanism->id, 10);
	} else if (mechanism->role_type == 35000) { //金人堂，镜子1
		common_flag1_ = get_now_tv()->tv_sec;
	} else if (mechanism->role_type == 34999) {//金人堂，镜子2
		common_flag2_ = get_now_tv()->tv_sec;
	} else if (mechanism->role_type == 30015) {//真假宝箱
		p->call_map_summon(33, mechanism->pos().x(), mechanism->pos().y(), true, false);
	} else if (mechanism->role_type == 34997) { //火堆
		p->btl->common_flag1_ = 100;	
		p->btl->battle_extra_logic_msg(p);
//		p->cur_map->mechansim_status_changed(mechanism->id, 0, 0);
	}
	mechanism_drop_to_all_players(mechanism);

	mechanism_del_item_t item_arr[10];
	uint32_t cnt = 0;
	//扣除物品
	//uint32_t item_id = 0;
	switch(mechanism->role_type)
	{
		case 39001:
			item_arr[0].itemid = 1500367;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;

		case 39002:
			item_arr[0].itemid = 1500372;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;

		case 39003:
			item_arr[0].itemid = 1500373;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;

		case 39004:
			item_arr[0].itemid = 1500374;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;

		case 39005:
			item_arr[0].itemid = 1500375;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;

		case 39006:
			item_arr[0].itemid = 1500376;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;

		case 39007:
			item_arr[0].itemid = 1500377;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;

		case 39008:
			item_arr[0].itemid = 1500378;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;
		case 39009:
			item_arr[0].itemid = 1500379;
			item_arr[0].itemcnt = 1;
			cnt++;
			item_arr[1].itemid = 1500381;
			item_arr[1].itemcnt = 1;
			cnt++;
			break;
		case 39010:
			item_arr[0].itemid = 1500380;
			item_arr[0].itemcnt = 1;
			cnt++;
			item_arr[1].itemid = 1500381;
			item_arr[1].itemcnt = 1;
			cnt++;
			break;
		case 39011:
			item_arr[0].itemid = 1500391;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;
		case 39012:
			item_arr[0].itemid = 1500393;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;
		case 39013:
			item_arr[0].itemid = 1500402;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;

		case 39014:
			item_arr[0].itemid = 1500403;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;

		case 39015:
			item_arr[0].itemid = 1500404;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;

		case 39016:
			item_arr[0].itemid = 1500405;
			item_arr[0].itemcnt = 1;
			cnt++;
			break;

		default:
			//item_id = 0;
			break;
	}
	if(cnt && p)
	{
		notify_open_box_times(p, 1);	
		notify_auto_del_item(p, item_arr, cnt);
	}

	if(mechanism->role_type == 30012)
	{
		notify_delete_player_obj(mechanism);
	}
}

/** 
 * @brief send the battle extra logic info (ex:flag1, flag2_, flag3_) to client 
 * 
 * @param p
 */
void Battle::battle_extra_logic_msg(Player * p) 
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, static_cast<uint32_t>(stage()->id), idx);
	pack(pkgbuf, p->cur_map->id, idx);
	pack(pkgbuf, common_flag1_, idx);
	pack(pkgbuf, common_flag2_, idx);//flag2
	pack(pkgbuf, common_flag3_, idx);//flag3
	pack(pkgbuf, battle_section_data, idx);
	TRACE_LOG("SEND TO CLIENT %u %u %u %u", common_flag1_, common_flag2_, common_flag3_, battle_section_data);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t),  cli_proto_btl_section_change,  idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	p->cur_map->send_to_map(pkgbuf, idx);
}

void Battle::drop_coins_on_monster_dead(Player * player, Player * killer)
{
	if (this->battle_mode() == 	btl_mode_lv_matching) {
		return;
	}

	int gold = 0;

	if (!is_valid_uid(player->id) && !is_summon_mon(player->role_type)) {
		if (is_summon_mon(killer->role_type)) {
			gold = calc_monster_gold_drop(player, killer->summon_info.owner);
		} else if (is_valid_uid(killer->id)) {
			gold = calc_monster_gold_drop(player, killer);
		}
	}

	if (gold) {
		for (PlayerVec::iterator it = players_.begin(); it != players_.end(); ++it) {
			Player * player = *it;
			player->gain_coin(gold);
		}
	}
}

uint32_t
Battle::get_friend_cnt(uint32_t team)
{
    uint32_t friend_cnt = 0;
    PlayerVec::iterator cur = players_.begin();
    PlayerVec::iterator end = players_.end();
    for (; cur != end; ++cur) {
        Player* p = (*cur);
        if (p->team == team) {
            friend_cnt++;
        }
    }
    return friend_cnt;
}


void Battle::create_entity()
{

}

/** 
 * @brief 关卡额外逻辑处理 
 * 
 */
void process_battle_extra_logic(Battle * btl)
{
	if (btl->stage()->id == 48) {
		btl->battle_section_data++;

		if (btl->battle_section_data == 4) {
			uint32_t map_id = 1004803; 
			map_id = get_stage_map_id(map_id);
			map_t * m_map = btl->get_map(map_id);	
			if (m_map) {
//				m_map->activate_pre_monsters();//中央地图出怪
				call_monster_to_map(m_map, btl, 13080, 651, 719);
				call_monster_to_map(m_map, btl, 11290, 328, 808);
				call_monster_to_map(m_map, btl, 11290, 670, 900);
				call_monster_to_map(m_map, btl, 11291, 346, 498);
				call_monster_to_map(m_map, btl, 11291, 321, 909);
				call_monster_to_map(m_map, btl, 11291, 1052, 665);
			}	
		}
	}
}



