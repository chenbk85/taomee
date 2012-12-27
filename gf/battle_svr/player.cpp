#include <cstdio>
#include <map>
#include <math.h>
#include <libtaomee++/random/random.hpp>
#include <libtaomee++/inet/pdumanip.hpp>
#include <kf/pet_attr.hpp>
#include <kf/fight_team.hpp>

using namespace std;
using namespace taomee;
extern "C" {
#include <glib.h>

#include <libtaomee/log.h>
#include <libtaomee/timer.h>
}
#include "fwd_decl.hpp"
#include "title_attribute.hpp"
#include "item.hpp"
#include "skill.hpp"
#include "battle.hpp"
#include "monster.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "ai/MonsterAI.hpp"
#include "player_status.hpp"
#include "base_effect.hpp"
#include "buff.hpp"
#include "aura.hpp"
#include "roll.hpp"
#include "map_summon_object.hpp"
#include "skill_imme_effect.hpp"
#include "player_attribute.hpp"
#include "pet_attribute.hpp"
//#include "app_log.hpp"
#include "battle_logic.hpp"

/*! hold all the players */
static PlayerMap all_players;


/*! monster list */
MonList g_monlist;

MonlistMrg g_monlist_mrg;
struct timeval MonlistMrg::mon_update_tv_arr[max_monlist_arr_cnt];

std::vector<uint32_t> watch_list;

void init_watch_list()
{
	watch_list.clear();
	watch_list.push_back(150051250);
	
}

bool is_player_in_watch_list(uint32_t uid)
{
	uint32_t size = watch_list.size();
	for (uint32_t i = 0; i < size; i++) {
		if (watch_list[i] == uid) {
			return true;
		}
	}
	return false;
}

int is_enemy(const Player * atker, const Player * target)
{
	static int player_enemy_info[6][6] = {
		{0, 1, 1, 1, 1, 0},
		{1, 0, 1, 1, 1, 0},
		{1, 1, 0, 0, 1, 0},
		{0, 1, 1, 1, 1, 0},
		{1, 0, 1, 1, 1, 0},
		{1, 1, 0, 0, 1, 0}
	};
  	return player_enemy_info[atker->team - 1][target->team - 1]; 
}


/*! an intrusive list to link all player who have summon monster together */
//PlayerList g_summon_list;

inline bool check_map_summon(Player * player, uint32_t mode)
{
	for (std::list<map_summon_object*>::reverse_iterator it = player->btl->map_summon_list.rbegin();
			it != player->btl->map_summon_list.rend(); ++it ) {
		if (player->check_distance(*it, 30) && (*it)->model_ == mode ) {
			return false;
		}
	}

	return true;

}

void set_monster_dead()
{
}

//-----------------------------------------------------------
// Public Methods
Player::Player(userid_t uid, fdsession_t* fdsession)
{
	fdsess        = fdsession;
	if (fdsess) {
		fd        = fdsess->fd;
		id        = uid;
	} else {
		id        = Object::id();
		role_tm   = get_now_tv()->tv_sec;
	}
	memset(once_bit, 0x00, sizeof(once_bit));
	cur_map       = 0;
	btl_status    = 0;
	btl           = 0;
	watch_btl     = 0;
	show_state    = 0;
	//clothes_num   = 0;
	memset(&clothes_info, 0x00, sizeof(clothes_info));

	my_summon     = NULL;
	my_numen	  = 0;
	vip           = 0;
	vip_level     = 0;
	max_bag_grid_count = 0;
	coins         = 0;
	strength      = 0;
	agility_       = 0;
	body_quality_  = 0;
	stamina       = 0;
	hp            = 0;
	maxhp         = 0;
	mp            = 0;
	continue_win  = 0;
	btl_again	  = 0;
	honor 	 	  = 0;
	exploit   	  = 0;
	max_conti_win_times  = 0;
	time_out_lv   = 0;

	maxmp         = 0;
	atk           = 0;
	memset(weapon_atk, 0, sizeof(weapon_atk));
	crit_rate     = 0;
	def_rate      = 0;
	def_value     = 0;
	def_threshold_value = 0;
	atk_threshold_value = 0;
	hit_rate      = 0;
	dodge_rate    = 0;
	dir           = dir_right;
	lag           = 0;
	avg_lag       = 0;
	memset(&test_start_tv, 0, sizeof(test_start_tv));
	test_seq      = 0;
	time_ev       = 0;
	pvp_end_ev	  = 0;
	speed         = 0;	
	//buf trim
	critical_max_damage_buf_trim = 0;
	sustain_damage_value_change_buf_trim = 0;
	body_quality_trim = 0;
	agility_buf_trim = 0;
	atk_buf_trim 	= 0;
	def_buf_trim 	= 0;
	def_value_buf_trim = 0;
	hit_buf_trim	= 0;
	crit_buf_trim	= 0;
	dodge_buf_trim	= 0;
	hp_max_buf_trim	= 0;
	hp_max_pvp_trim = 0;
	mp_max_buf_trim	= 0;
	exp_factor_trim = 0;
	summon_mon_exp_factor_trim = 0;
	player_only_exp_factor_trim = 0;
	skill_cd_time_buf_trim = 1.0;
	skill_mana_buf_trim = 1.0;
	atk_damage_change_buf_trim = 1.0;
	sustain_damage_change_buf_trim = 1.0;
	speed_change_buf_trim = 1.0;

	rigidity_factor = 100;

	dead_call_summon = 0;
	suicide_call_summon = 0;	

	waitcmd       = 0;
	load_tm 	  = 0;
	be_hit_count = 0;
	//about summon monster
	my_summon	= 0;
    common_flag_ = 0;
	memset(&summon_info, 0x00, sizeof(summon_info));
	memset(&mon_update_tv, 0, sizeof(mon_update_tv));

	my_numen = 0;
	memset(&numen_info, 0x00, sizeof(numen_info));
	//about monster info
	mon_info       = 0;
	super_armor	  = false;
	
	i_ai = new AIInterface(this, NULL_AI);

	need_sav_      = false;
	damage				= 0;
	fumo_points_end 	= 0;
	fumo_points_start 	= 0;
	fumo_tower_top		= 0;
	fumo_tower_used_tm  = 0;
    cur_tower_layer     = 0;

	//about box
	can_open_box = false;
	normal_box_opened = 0;
	vip_box_opened = 0;

	memset(&suit_add_attr, 0x0, sizeof(suit_add_attr));
	
	if (is_valid_uid(uid)) {
		pkg_queue  = g_queue_new();
		all_players[uid] = this;
	}

	//about battle skill
	memset(&stuck_end_tm, 0x00, sizeof(stuck_end_tm));
	memset(&flying_end_tm, 0x00, sizeof(flying_end_tm));
	memset(&detective_info, 0x00, sizeof(detective_info));
	invincible  = false;
	invincible_time = 0;
	undead_flag = false;

    revive_count = 0;
    dead_start_tm = 0;

	hit_fly_flag = true;
	damage_ration = 0;
	pass_cnt = 0;

	team_lv = 0;
//	memset(other_active_info, 0, sizeof(other_active_info));
	lucky_cnt = 0;

	p_cur_skill = 0;
	
	cur_item_picking = 0;

	cur_picking_summon = 0;
	cur_skip_packet_count = 0;
	//team btl
	team_job = team_member_type_nor;
	team_btl_flg = 0;
	
	mf_rate = 0;
	greed_rate = 0;
	buf_greed_rate = 0;
	buf_mf_rate = 0;
	tmp_buff_flag = 0;

	use_skill_cnt = 0;
	be_attacked_cnt = 0;

	init_cheat_check_data();

	fly_flag = 0;
	memset(unique_item_bit, 0x00, sizeof(unique_item_bit));
	memset(&boost_info, 0x00, sizeof(boost_info));
	memset(&skip_step_info, 0x00, sizeof(skip_step_info));
	
	boost_info.judge_flag = 1;
	init_player_status(this);


	if (g_special_double_time_mrg.is_in_double_time()) {
		in_specal_double_time = true;
	} else {
		in_specal_double_time = false;
	}

	if (g_special_double_time_mrg.is_in_double_time_2()) {
		in_specal_double_time_2 = true;
	} else {
		in_specal_double_time_2 = false;
	}

	if (g_special_double_time_mrg.is_in_double_time_witch_can_still_use_double_tm_item()) {
		in_specal_double_time_witch_can_still_use_double_tm_item = true;
	} else {
		in_specal_double_time_witch_can_still_use_double_tm_item = false;
	}

	TRACE_LOG("alloc player %u %p: fdsess=%p fd=%d", id, this, fdsess, fd);
}

Player::~Player()
{
	//save player info
	if (is_valid_uid(id)) 
	{
		if (need_sav_) 
		{
			db_set_player_basic_attr(this);
			uint32_t buf[2] = {fumo_points_end - fumo_points_start, 0};
			do_stat_item_log_4(stat_log_fumo_gain, lv, 0, buf, sizeof(buf));
			if (coins > pre_coins) 
			{
				do_stat_item_log_2(stat_log_produce_coins, 0, 0, coins - pre_coins);
			}
		}
		db_set_player_task_buf(this);

		if (my_summon && my_summon->need_sav_) 
		{
			db_set_summon_basic_attr(my_summon, this);
		}
		if( my_summon )
		{
			delete my_summon;
			my_summon = NULL;
		}
		
	}

	if (my_numen) {
		delete my_numen;
		my_numen = 0;
	}
	cur_skip_packet_count = 0;


	leave_map(true);

    if (is_valid_uid(id)) 
	{
		//summon monster
		/*if (my_summon) {
			my_summon->set_dead();
			my_summon->del_objlist();
			my_summon = 0;
		}*/
        
		if (skill_point > pre_skill_point) {
            do_stat_item_log_2(stat_log_got_skillpoints, 0, role_type, skill_point - pre_skill_point);
        }

        cached_pkg_t* pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(pkg_queue));
		while (pkg) {
			g_slice_free1(pkg->len, pkg);
			pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(pkg_queue));
		}
		g_queue_free(pkg_queue);
		all_players.erase(id);

		int32_t lag_type = avg_lag / 50 ;
		lag_type = lag_type > 8 ? 8 : lag_type;
		do_stat_item_log_2(stat_log_lag_info, 0, lag_type, 1);
		
	}

	/*if (!is_summon_mon(role_type)) {
		del_objlist();
	}*/
	del_objlist();

	delete i_ai;
	attack_times = 0;
	fly_flag = 0;
	final_player_status(this);
	TRACE_LOG("dealloc player %u", id);
}

void
Player::del_objlist()
{
	for (list<Object*>::iterator it = objlist_.begin(); it != objlist_.end(); ++it) {
		(*it)->set_dead();
	}
	objlist_.clear();
}

void
Player::move(const player_move_t* mv)
{
	////////////////////这里记录连续20次移动的起始和结束时间/////////////////////////////
	
	const timeval* tv = get_now_tv();
	////////////////////////////////////////////////////////////////////////////////////	
	
	if (!in_battle()) {
		send_header_to_player(this, waitcmd, cli_err_battle_not_started, 1);
		return;
	}

	if (!can_action()) {
		send_header_to_player(this, waitcmd, cli_err_cannot_action, 1);
		return;
	}
	
	
	TRACE_LOG("MOVE %u %u", id, mv->dir);
	if( timecmp(*tv, stuck_end_tm) < 0 ){
		TRACE_LOG("MOVE check stuck_time userid = %u", id);
	}
	
	TRACE_LOG("MOVE userid= (%u), last_pos = (%u %u), now_pos = (%u, %u), old_pos = (%u, %u) new_pos = (%u, %u)",
			id, old_pos().x(), old_pos().y(),  pos().x(), pos().y(), mv->old_x, mv->old_y,  mv->x, mv->y
			);


	int skip_packet_flag = 0;

	if( (pos().x() != (int)mv->old_x || pos().y() != (int)mv->old_y) && (btl->is_battle_pvp() || btl->is_battle_tmp_team_pvp())) {
		if( old_pos().x() != 0)
		{
			if(get_fly_flag() == 1)
			{
				skip_packet_flag = 1;
				set_fly_flag(0);
				TRACE_LOG("ERROR MOVE SKIP THIS PACKET");
			}
			else
			{
				adjust_position(Vector3D(mv->x, mv->y, mv->z), mv->dir);
			}
		}
		else
		{
			adjust_position(Vector3D(mv->x, mv->y, mv->z), mv->dir);
		}
	}
	else
	{
		adjust_position(Vector3D(mv->x, mv->y, mv->z),  mv->dir);
	}
		

	uint16_t* plag;
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, id, idx);
	pack_h(pkgbuf, pos().x(), idx);
	pack_h(pkgbuf, pos().y(), idx);
	pack_h(pkgbuf, pos().z(), idx);
	pack_h(pkgbuf, mv->dir, idx);
	pack_h(pkgbuf, mv->mv_type, idx);

	plag = reinterpret_cast<uint16_t*>(pkgbuf + idx);
	idx += sizeof(*plag);


	init_btl_proto_head(pkgbuf, waitcmd, idx);

	if(skip_packet_flag == 0)
	{
		for (PlayerSet::const_iterator it = cur_map->players.begin(); it != cur_map->players.end(); ++it) {
			Player* player = *it;
			*plag = this->lag + player->lag;
			send_to_player(player, pkgbuf, idx, 0);
		}
	}else {
		send_to_player(this, pkgbuf, idx, 0);
	}
	

	waitcmd = 0;

	proc_move_extra_logic();//处理特殊行走逻辑
	
	TRACE_LOG("uid=%u %u %u %u %u", id, pos().x(), pos().y(), pos().z(), dir);
}

void
Player::jump(const player_jump_t* mv)
{
	if (!in_battle()) {
		send_header_to_player(this, waitcmd, cli_err_battle_not_started, 1);
		return;
	}

	if (!can_action()) {
		send_header_to_player(this, waitcmd, cli_err_cannot_action, 1);
		return;
	}
	
	
	TRACE_LOG("JUMP userid= (%u), last_pos = (%u %u), now_pos = (%u, %u),  end_pos = (%u, %u)",
			            id, old_pos().x(), old_pos().y(),  pos().x(), pos().y(), mv->x, mv->y
						            );


	adjust_position(Vector3D(mv->x, mv->y));

	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, id, idx);
	pack_h(pkgbuf, pos().x(), idx);
	pack_h(pkgbuf, pos().y(), idx);

	init_btl_proto_head(pkgbuf, waitcmd, idx);
	cur_map->send_to_map(pkgbuf, idx, this);

	TRACE_LOG("uid=%u %u %u %u %u", id, pos().x(), pos().y(), pos().z(), dir);
}

void
Player::stop(const player_stop_t* req)
{
	if (!in_battle()) {
		send_header_to_player(this, waitcmd, cli_err_battle_not_started, 1);
		return;
	}

		
	TRACE_LOG("STOP userid= (%u), last_pos = (%u %u), now_pos = (%u, %u), old_pos =(%u, %u), new_pos = (%u, %u)",
			            id, old_pos().x(), old_pos().y(),  pos().x(), pos().y(), req->old_x, req->old_y, req->x, req->y);


	
	int skip_packet_flag = 0;
	
	if( (pos().x() != (int)req->old_x || pos().y() != (int)req->old_y ) && old_pos().x() != 0 && (btl->is_battle_pvp() || btl->is_battle_tmp_team_pvp()))
	{
		if( get_fly_flag() == 1)
		{
			TRACE_LOG("ERROR STOP SKIP THIS PACKET");
			skip_packet_flag = 1;
			set_fly_flag(0);
		}
		else
		{
			adjust_position(Vector3D(req->x, req->y), req->dir);
		}
	}
	else
	{
		adjust_position(Vector3D(req->x, req->y), req->dir);
	}


	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, id, idx);
	pack_h(pkgbuf, pos().x(), idx);
	pack_h(pkgbuf, pos().y(), idx);
	pack_h(pkgbuf, req->dir, idx);
	init_btl_proto_head(pkgbuf, waitcmd, idx);
	if(skip_packet_flag == 0)
	{
		cur_map->send_to_map(pkgbuf, idx, this);
	}
	else
	{
		send_to_player(this, pkgbuf, idx, 1);
	}
}

void
Player::summon_stand(const Player * summon)
{
    uint8_t buf[1024] = {0};
    int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack_h(buf, summon->pos().x(), idx);
	pack_h(buf, summon->pos().y(), idx);
	pack_h(buf, summon->dir, idx);
    init_cli_proto_head_full(buf + sizeof(btl_proto_t), summon->id, cli_proto_stand, 0, idx - sizeof(btl_proto_t));
    init_btl_proto_head(buf, btl_transmit_only, idx);
    summon->cur_map->send_to_map(buf, idx);
}

void
Player::enter_map(uint32_t mid)
{
	if (!in_battle()) {
		WARN_LOG("you're not ready to enter map: uid=%u", this->id);
		send_header_to_player(this, waitcmd, cli_err_battle_not_started, 1);
		return;
	}
	uint32_t original_mid = mid;
	mid = get_stage_map_id(mid);

	const tomaps_t* to = cur_map->get_tomap(mid);
	if (to == 0) {
		WARN_LOG("map not exist[%u %u]", id, mid);
		send_header_to_player(this, waitcmd, cli_err_battle_map_not_exist, 1);
		return;
	}

	if (cur_map->get_enemy_cnt(this)) {
		WARN_LOG("monster exist[%u %u]", id, mid);
		send_header_to_player(this, waitcmd, cli_err_battle_monster_exist, 1);
		return;
	}

	//删除当前地图的召唤物
	
	btl->del_map_summon_by_owner_id(id);

	set_pos(Vector3D(to->init_x, to->init_y));

	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, pos().x(), idx);
	pack_h(pkgbuf, pos().y(), idx);
	pack_h(pkgbuf, dir, idx);
	init_btl_proto_head(pkgbuf, waitcmd, idx);
	send_to_player(this, pkgbuf, idx, 1);

	map_t* m = btl->get_map(mid);
	if (m) {
		m->send_players_info(this);
		m->send_players_status_info(this, false);
		m->send_all_item_drop(this);
		m->send_player_info(this);
		notify_map_summon_object(this);
		if (btl->is_team_btl()) {
			btl->notify_team_member_enter_map(this, original_mid);
		}
		do_enter_map(m);
        //if (my_summon) {
        //    my_summon->set_pos(pos());
        //    m->send_player_info(my_summon);
        //}
		if (!(m->visited_flg)) {
			m->activate_monsters();
			m->visited_flg = true;
		}
	} else {
		WARN_LOG("there is no this map [%u] uid=%u", mid, id);
	}

	// TODO: for team battle, more info should be sent to the other team members

	return;
}

void
Player::leave_map(bool leave_stage)
{
	if (cur_map) {
		if (is_valid_uid(id)) {
			TRACE_LOG("S M4 %u", (uint32_t)cur_map->players.size());
			cur_map->players.erase(this);
			// leave map rsp
			int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
			pack(pkgbuf, id, idx);
			init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), id, cli_proto_leave_map, 0, idx - sizeof(btl_proto_t));
			init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
			cur_map->send_to_map(pkgbuf, idx);
			//DEBUG_LOG("L M\t[uid=%u map=%u]", id, cur_map->id);
			KDEBUG_LOG(id, "L M\t[uid=%u map=%u]", id, cur_map->id);
			//monser
			for (PlayerSet::iterator it = cur_map->monsters.begin(); it != cur_map->monsters.end(); ++it) {
				Player* mon = *it;
				mon->on_player_leave_map(this);
			}
			//summon
			if (my_summon) {
				int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
				pack(pkgbuf, my_summon->id, idx);
				init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), my_summon->id, cli_proto_leave_map, 0, idx - sizeof(btl_proto_t));
				init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
				cur_map->send_to_map(pkgbuf, idx);
				
				my_summon->cur_map = 0;
				my_summon->summon_info.active_flag = false;
			}

			if (my_numen) {
				int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
				pack(pkgbuf, my_numen->id, idx);
				init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), my_numen->id, cli_proto_leave_map, 0, idx - sizeof(btl_proto_t));
				init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
				cur_map->send_to_map(pkgbuf, idx);
			}
			if (leave_stage && btl) {
				btl->notify_team_member_enter_map(this, 0);
			}
		} else {
			if (role_type < 19001) {
				for (PlayerSet::iterator it = cur_map->players.begin(); it != cur_map->players.end(); ++it) {
					Player* p = *it;
					if (p->my_summon && p->my_summon->i_ai->target == this) {
						p->my_summon->i_ai->target = 0;
						p->my_summon->i_ai->change_state(WAIT_STATE);
						TRACE_LOG("summon change target!");
					}
				}
				cur_map->monsters.erase(this);
			}/* else {
				cur_map->barriers.erase(this);
			}*/
		}
		
		cur_map = 0;
	}
}

bool Player::is_monster_in_cur_map(uint32_t mon_id)
{
    if (cur_map) {
        return cur_map->is_monster_exist(mon_id);
    }
    return false;
}

void
Player::monster_enter_map(map_t* m)
{
	if (m) {
		do_enter_map(m);
		//TODO:noti to map
		int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
			
		idx += pack_rt_birth_mon_basic_info(pkgbuf + idx);
		init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_realtm_birth_mon_info, idx - sizeof(btl_proto_t));
		init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
		m->send_to_map(pkgbuf, idx, 0);
	} else {
		WARN_LOG("there is no this map mon_uid=%u", id);
	}
	
	return;
}

void
Player::attack(const player_attack_t* req)
{
	//check in battle
	if (!in_battle()) 
	{
		WARN_LOG("you're not ready to attack: uid=%u", id);
		send_header_to_player(this, waitcmd, cli_err_battle_not_started, 1);
		return;
	}
	//check the skill is exist
	TRACE_LOG("ATK %u %u", id, req->skill_id); 
	player_skill_t* p_skill = get_skill(req->skill_id);
	if(p_skill == NULL)
	{
		send_header_to_player(this, waitcmd, cli_err_cool_time, 1);
		return;
	}

//	teleport(pos().x() - 400, pos().y());

	player_skill_t* player_skill = can_use_skill(req->skill_id, true);
	if ( !player_skill ) {
		send_header_to_player(this, waitcmd, cli_err_cool_time, 1);
		//////然后还得通知客户端这个技能释放失败，让客户端吧动画停止CD重置////
		notify_launch_skill_fail(req->skill_id, p_skill->lv);	
		TRACE_LOG("notify_launch_skill_fail userid = %u, role_regtime = %u, skill_id = %u", id, role_tm, req->skill_id);
		return;
	}

	if (!player_skill->skill_info->is_normal_skill()) {
		this->use_skill_cnt ++;
	}


	adjust_position(Vector3D(req->x, req->y, req->z));
	if (req->skill_id == 200604) {
		rabbit_twinkling_move();
	} else if (req->skill_id == 200607) {
        if (btl->is_battle_pve() && !this->cur_map->is_all_monster_dead() && !btl->is_battle_ended()) {
            this->call_monster(11521, pos().x(), pos().y(), this->team, 0, 0);
        }

        if ((btl->is_battle_pvp() || btl->is_battle_tmp_team_pvp()) && !btl->is_battle_ended()) {
            this->call_monster(11521, pos().x(), pos().y(), this->team, 0, 0);
        }
    }
	

	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, id, idx);
	pack(pkgbuf, role_type, idx);
	pack(pkgbuf, req->skill_id, idx);
	pack(pkgbuf, player_skill->lv, idx);
	pack(pkgbuf, pos().x(), idx);
	pack(pkgbuf, pos().y(), idx);
	pack(pkgbuf, pos().z(), idx);
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), id, cli_proto_player_attack, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	cur_map->send_to_map(pkgbuf, idx, this);

    if (req->skill_id == 100607) { 
        set_all_skill_cool_time(0);
    }
	launch_skill(player_skill);
}

void 
Player::do_item_buf(const GfItem* itm)
{
	BufSkillMap::iterator it = buf_skill_map.find(itm->buf_type());
	if (it == buf_skill_map.end()) {
		buf_skill_t buf_skill = {0};
		buf_skill.buf_duration = itm->buf_duration();
		buf_skill.buf_type = itm->buf_type();
		buf_skill.per_trim = itm->per_trim();
		buf_skill.mutex_type = itm->mutex_type();
		buf_skill.start_tm = get_now_tv()->tv_sec;
		
		add_player_buff(this, buf_skill.buf_type, 0);
		if (itm->buf_ex_id1()) {
			buf_skill_map.insert(BufSkillMap::value_type(buf_skill.buf_type, buf_skill));
		}

		TRACE_LOG("INSERT APP BUF: %u %u %u", buf_skill.buf_type, buf_skill.buf_duration, buf_skill.per_trim);
	} else {
		TRACE_LOG("BEFORE UPDATE: %u %u %u", it->second.buf_type, it->second.buf_duration, it->second.per_trim);
		it->second.buf_duration = itm->buf_duration();
		it->second.buf_type = itm->buf_type();
		it->second.per_trim = itm->per_trim();
		it->second.start_tm = get_now_tv()->tv_sec;
		
		add_player_buff(this, it->second.buf_type, 0, it->second.buf_duration);
		TRACE_LOG("AFTER UPDATE: %u %u %u", it->second.buf_type, it->second.buf_duration, it->second.per_trim);
	}
	
}

bool Player::is_have_bless_buff(uint32_t buff_id)
{
    std::list<buff*>::iterator it = m_bufflist.begin();

    for ( ; it != m_bufflist.end(); ++it) {
        if ( (*it)->get_buff_id() == buff_id) {
            return true;
        }
    }
    return false;
    //return buf_skill_map.find(buff_id) != buf_skill_map.end();
}

void
Player::set_app_buf_start(uint32_t tm_now)
{
	uint32_t tmp_tm = 0;
	if (btl->battle_mode() != btl_mode_lv_matching) {
		for (BufSkillMap::iterator it = buf_skill_map.begin(); it != buf_skill_map.end(); ++it) {
			add_player_buff(this, it->second.buf_type, 0, it->second.buf_duration);
			TRACE_LOG("SET BUF RUN: %u %u %u",it->second.buf_type, tmp_tm, it->second.per_trim);
		}

    } else {
        if (btl->pvp_lv() == pvp_monster_game || btl->pvp_lv() == pvp_monster_practice) {
            if (this->team == 1) {
                add_player_buff(this, 1213, 0, tmp_tm);
                TRACE_LOG("SET BUF RUN: %u 1213", this->id);
            } else {
                add_player_buff(this, 1214, 0, tmp_tm);
                TRACE_LOG("SET BUF RUN: %u 1214", this->id);
            }
        }
    }
}

void
Player::init_player_passive_skill(bool noti_flg)
{
	for ( SkillMap::iterator it = skills_map.begin(); it != skills_map.end(); ++it ) {
		//get skill by skillid
		player_skill_t* tmp_skill = &(it->second);
		if (is_valid_uid(id) && !(tmp_skill->flag)) {
			continue;
		}
		skill_t* p_skill = g_skills->get_skill(tmp_skill->skill_id);
		if (p_skill->type == passive_skill) {
			skill_lv_info_t* lv_info = &(p_skill->lv_info[tmp_skill->lv]);
			for (uint32_t n = 0; n < lv_info->effects_num; n++) {
				skill_effects_t* p_skill_effect = lv_info->effects + n;
				if (p_skill_effect->aura_id) {
					add_player_aura(this, p_skill_effect->aura_id, id, noti_flg);
				}
			}
		}
		TRACE_LOG("player passive skill[%u %u]", id, it->second.skill_id);
	}

	if (numen_info.numen_id) {
		for (uint32_t i = 0; i < max_numen_skill_num; i++) {

			numen_skill_t* tmp_skill = &(numen_info.skill_arr[i]);
			if (!(tmp_skill->skill_id)) {
				continue;
			}
			skill_t* p_skill = g_skills->get_skill(tmp_skill->skill_id);
			if (p_skill->type == passive_skill) {
				skill_lv_info_t* lv_info = &(p_skill->lv_info[tmp_skill->skill_lv]);
				for (uint32_t n = 0; n < lv_info->effects_num; n++) {
					skill_effects_t* p_skill_effect = lv_info->effects + n;
					if (p_skill_effect->aura_id) {
						add_player_aura(this, p_skill_effect->aura_id, id, noti_flg);
					}
				}
			}
			TRACE_LOG("numen passive skill[%u %u]", id, tmp_skill->skill_id);
		}
	}
	if (!is_summon_mon(role_type) && my_summon) {
		my_summon->init_player_passive_skill(noti_flg);
	}
	hp	= max_hp();
	mp	= max_mp();
//	summon_info.fight_value = get_max_fight_value();

	if (noti_flg) {
		noti_hpmp_to_btl();
	}
}


void
Player::set_revive()
{
	if (is_dead()) 
	{
		TRACE_LOG("revi %u", id);
		//attr change
		set_dead(false);
		hp = max_hp();
		mp = max_mp();
		noti_hpmp_to_btl();
		//summon monster activate
		if (my_summon && my_summon->if_can_fight()) {
			my_summon->summon_info.active_flag = true;
		}

		if (btl->stage()->id == 993) {
			if (is_valid_uid(id)) {
				btl->common_flag1_ = 100;
				btl->battle_extra_logic_msg(this);
			}
			if (is_player_buff_exist(this, 1315)) {
				del_player_buff(this, 1315);
			}
		}


		if(!is_summon_mon(role_type))
		{
			invincible_time++;
			timeval tv = *get_now_tv();
			tv.tv_sec += 3;
			ev_mgr.add_event(*this, &Player::invincible_chg, tv);

        	dead_start_tm = 0;
        	revive_count += 1;

        	if (btl->stage()->id == 939)
			{
            	int x_pos = 104;
            	int y_pos = 309;
            	if (team == 2) x_pos = 1421;
            	set_pos(Vector3D(x_pos, y_pos));
            	adjust_coordination();
        	}
			do_stat_item_log_2(stat_log_role_relive, 0, role_type, 1);
		}
	}
}

void
Player::repair_all_attire()
{
	for (uint32_t i = 0; i < clothes_info.clothes_cnt; i++) {
		player_clothes_info_t* p_clothes = &(clothes_info.clothes[i]);
		p_clothes->duration = p_clothes->init_duration;
		TRACE_LOG("%u %u %u", p_clothes->clothes_id, p_clothes->duration, p_clothes->init_duration);
	}
	//calc_player_def_atk(this);
	atk       = calc_player_atk_attr(this);
	def_value = calc_player_def_value_attr(this);
	def_rate  = calc_player_def_rate_attr(this);
	atk       += atk_buf_trim;
    def_rate  += def_buf_trim;	 
}

void
Player::set_all_clothes_0_duration()
{
	uint32_t duration_punish_percent = get_duration_punish_percent(btl->stage()->id);
	for (uint32_t i = 0; i < clothes_info.clothes_cnt; ++i) {
		player_clothes_info_t* p_clothes = clothes_info.clothes + i;
		p_clothes->duration *= (100 - duration_punish_percent);
		p_clothes->duration /= 100;
		duration_state_chg_noti(this, p_clothes);
	}
}

void
Player::punish_exp_when_dead()
{
	uint32_t punish_exp_percent = get_exp_punish_percent(btl->stage()->id);
	uint32_t lv_init_exp = calc_exp(lv);
	uint32_t punish_exp = 
		exp > lv_init_exp ?	(exp - lv_init_exp) * punish_exp_percent / 100 : 0;
	exp -= punish_exp;
	noti_chg_attr_to_player();
}
void
Player::use_item(uint32_t item_id)
{
	const GfItem* itm = items->get_item(item_id);

	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, item_id, idx);
	init_btl_proto_head(pkgbuf, waitcmd, idx);
	TRACE_LOG("use item:uid=%u itmid=%u %u %u %u %u",id, item_id, itm->category(), 
				itm->add_hp(), itm->add_mp(), itm->duration);
	send_to_player(this, pkgbuf, idx, 1);

	//monitor
    report_add_to_monitor(this, 31, itm->id(), 1, 0);
	
	switch (item_id) {
		case relive_player_item_id:
		case relive_player_item_id_1:
			//Phoenix Down, player revive
			if (btl && btl->is_battle_tower()) {
				if (btl->is_battle_tower()) {
					do_stat_item_log(stat_log_fumo_relive, cur_tower_layer, 0, 1);
				}
			}
			return set_revive();
			break;
		case repair_clothes_item_id:
			return repair_all_attire();
			break;

		case  clear_random_one_debuff_item_id:
			del_random_player_buff(this, 2);
			del_random_player_buff(this, 3);
			break;
		case clear_all_monster_item_id: //药师后花园爆破核桃
			proc_use_item_extra_logic(item_id);
			return;
		case super_walnut_item_id: //药师后花园冲击核桃
			proc_use_item_extra_logic(item_id);
			return;
		case 1600005:
			{
				if(my_summon)
				{
					my_summon->add_pet_fight_value(30);
				}
			}	
			return;

		case 1600006:
			{
				if(my_summon)
				{
					my_summon->add_pet_fight_value(100);
				}
			}
			return;

		default:
			//return repair_all_attire();
			break;
	}

	//other notic excepet Phoenix Down, noti to map
	if (itm->buf_type() == 0) 
	{
		if (!(itm->buff_target()))
		{
			hp_mp_chg_buf_event(itm->add_hp(), itm->add_mp(), itm->interval, itm->duration);
			if (itm->add_per_hp() || itm->add_per_mp()) 
			{
				add_per_hp_mp_immediately(itm->add_per_hp(), itm->add_per_mp());
			}
		}
		else if( itm->buff_target() == 1 && my_summon)
		{
			my_summon->hp_mp_chg_buf_event(itm->add_hp(), itm->add_mp(), itm->interval, itm->duration);
			if (itm->add_per_hp() || itm->add_per_mp())
			{
				my_summon->add_per_hp_mp_immediately(itm->add_per_hp(), itm->add_per_mp());
			}
		}
	} 
	else 
	{
		if (!(itm->buff_target())) 
		{
			do_item_buf(itm);	
		} 
		else if (itm->buff_target() == 1 && my_summon) 
		{
			my_summon->do_item_buf(itm);
		}
	}
}

void
Player::pick_item(uint32_t unique_id, uint32_t max_bag_grid_count)
{
	TRACE_LOG("player pick item[%u %u]", id, unique_id);
	uint8_t flag = 1;
	
	//get the item from map
	ItemDropMap::iterator it = cur_map->item_drops.find(unique_id);
	if ( it == cur_map->item_drops.end() ) {
		//WARN_LOG("player cant find item drop[%u %u]", id, unique_id);
		//send_header_to_player(this, waitcmd, cli_err_no_item, 1);
		flag = 0;
		//return;
	}

	if (!cur_picking_summon) {
	//test if can pick
		if ( flag && !(it->second.collision(this)) ) {
		//WARN_LOG("player cant pick item drop,collision[%u %u %u],[%u %u %u]", id, pos().x(), pos().y(),
		//		unique_id, it->second.pos().x(), it->second.pos().y());
		//send_header_to_player(this, waitcmd, cli_err_cannot_pick_item, 1);
			flag = 0;
		//return;
		}
	}

	if ( flag && it->second.item_state() == can_pick ) {
		cur_item_picking = unique_id;
		it->second.set_state(cannot_pick);
	} else if ( flag && it->second.item_state() != can_pick) {
		//WARN_LOG("player cant pick item drop[%u %u]", id, unique_id);
		//send_header_to_player(this, waitcmd, cli_err_cannot_pick_item, 1);
		flag = 0;
		//return;
	}

	//if cannot pick
	if (!flag) {
		int idx = sizeof(btl_proto_t);
	//	pack_h(pkgbuf, cur_picking_summon, idx);
		pack_h(pkgbuf, flag, idx);
		pack_h(pkgbuf, id, idx);
		pack_h(pkgbuf, unique_id, idx);
		pack_h(pkgbuf, static_cast<uint32_t>(0), idx);
		pack_h(pkgbuf, static_cast<uint32_t>(0), idx);
		pack_h(pkgbuf, cur_picking_summon, idx);
		init_btl_proto_head(pkgbuf, waitcmd, idx);

		send_to_player(this, pkgbuf, idx, 1);
		return;
	}

	uint32_t itmid = it->second.item_id();
    //send to monitor
    uint32_t stageid = 0;
    if (btl) {
        stageid = btl->stage()->id;
    }
    report_add_to_monitor(this, 21, stageid, itmid, 1);

    //send to db
	const GfItem* itm = items->get_item(itmid);
	int idx = 0;
	
	pack_h(dbpkgbuf, itmid, idx);
	pack_h(dbpkgbuf, itm->db_category(), idx);
	pack_h(dbpkgbuf, itm->max(), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(itm->duration * clothes_duration_ratio), idx);
	pack_h(dbpkgbuf, max_bag_grid_count, idx);
	uint32_t pos = get_unique_item_bitpos(itmid);
	TRACE_LOG("get_unique_item_bitpos%u:%u", pos, itmid);
	if (pos && !test_bit_on(unique_item_bit, pos)) {
		uint8_t	bit_tmp[sizeof(unique_item_bit)]= "";
		memcpy(bit_tmp, unique_item_bit, sizeof(unique_item_bit));
		taomee::set_bit_on(bit_tmp, pos);
		pack_h(dbpkgbuf, static_cast<uint32_t>(sizeof(bit_tmp)), idx);
		pack(dbpkgbuf, bit_tmp, sizeof(bit_tmp), idx);
	} else {
		pack_h(dbpkgbuf, 0, idx);
	}

	//DEBUG_LOG("PICK ITEM[%u %u %u]", id, itmid, unique_id);
	KDEBUG_LOG(id, "PICK ITEM[%u %u %u]", id, itmid, unique_id);
	send_request_to_db(this, id, role_tm, dbproto_player_pick_item, dbpkgbuf, idx);
}

void
Player::do_enter_map(map_t* m, bool pre_in)
{
	leave_map();
	if (is_valid_uid(id)) {
		m->players.insert(this);
		//summon monster
		if (my_summon) {		
			my_summon->cur_map = m;
            my_summon->summon_info.active_flag = true;
			my_summon->set_pos(pos());
			m->send_player_info(my_summon, id);

			//my_summon->set_pos(pos());
            //summon_stand(my_summon);
		}
		//numen 
		if (my_numen) {
			my_numen->cur_map = m;
            my_numen->set_pos(pos());
            m->send_player_info(my_numen, id);
		}
	} else {
		if (pre_in) {
			if (role_type >= 30001 && role_type < 40000 ) { 
				m->pre_barriers.insert(this);
			} else if (role_type >= 19001 && role_type < 30000 ) {
				m->barriers.insert(this);
//			} else if (m->id == 1004803 ) {
//				m->pre_monsters.insert(this);
			} else {
                //ERROR_LOG("TEST monster [%u %u]", role_type, team);
				m->monsters.insert(this);
			}	
		} else {
			m->monsters.insert(this);
		}	
	}

	cur_map = m;

	//DEBUG_LOG("E M\t[uid=%u map=%u]", id, cur_map->id);
	KDEBUG_LOG(id, "E M\t[uid=%u map=%u]", id, cur_map->id);
}

void
Player::speak(const void* buf, uint32_t buflen) const
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	taomee::pack(pkgbuf, nick, sizeof(nick), idx);
	taomee::pack(pkgbuf, 0, idx);
	taomee::pack(pkgbuf, buflen, idx);
	taomee::pack(pkgbuf, buf, buflen, idx);
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), id, cli_proto_talk, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	
	cur_map->send_to_map(pkgbuf, idx);
}

uint32_t Player::get_weapon_avg_value()
{
	return (weapon_atk[0] + weapon_atk[1])/2;
}

int Player::calc_atkthreshold_damage(Player* target, int damage)
{
	uint32_t real_damage = damage;
	float x = get_weapon_avg_value();
	float y = target->atk_threshold_value;
	float min = 0.0;
	float max = 0.0;

	if( is_valid_uid(id) && !is_valid_uid(target->id) )
	{
		if( x >= 0.0 && x <= 28.0 ){
			real_damage = damage;
		}
	
		if(  x < sqrt((y - 28)/18)*(11 + 3 * y) ){
			real_damage = damage * 15 / 100;
			return real_damage;
		}

		max =  sqrt(  (y -28)/18 ) * (26 + 3*y);
		min =  sqrt(  (y -28)/18 ) * (11 + 3*y);
		if(  x >= min && x < max){
			real_damage = damage *  (20 + (x - min) * 30 /(max -min) ) / 100; 
			return real_damage;
		}

		min = max;
		max = min + sqrt((y-28)/18)* 3* y/2;
		if(x >= min && x < max){
			real_damage = damage *  (50 + (x - min) * 30 /(max -min) ) / 100;
			return real_damage;
		}

		min = max;
		max =  sqrt(  (y -28)/18 ) * (26 + 3*y)  + sqrt((y-28)/18)* 3* y;
		if( x >= min && x < max){
			real_damage = damage *  (80+ (x - min) * 30 /(max -min) ) / 100;
			return real_damage;
		}

		min = max;
		max = sqrt(  (y -28)/18 ) * (26 + 3*y)  + sqrt((y-28)/18)* 8.25* y;
		if( x >= min && x < max){
			real_damage = damage *  (100+ (x - min) * 30 /(max -min) ) / 100;
			return real_damage;
		}

		if( x >= max){
			real_damage = damage * 2;
			return real_damage;
		}

	}
	return real_damage;
}

int Player::calc_defthreshold_damage(Player* target, int damage)
{
	uint32_t real_damage = 0;
	//monster or summon monster attack user
	if( !is_valid_uid(id) && is_valid_uid(target->id)){
		uint32_t tmp_def_value = target->def_value + target->def_value_buf_trim;
		if( def_threshold_value == 0){
			real_damage = damage * ( 1 - sqrt( tmp_def_value / (1.2 * target->lv) )*0.09 );
		}else if(  tmp_def_value < def_threshold_value){
			real_damage = damage;
		}else if( tmp_def_value >= def_threshold_value && tmp_def_value < def_threshold_value + 60){
			real_damage = damage*0.9 - ( tmp_def_value - def_threshold_value )*damage/500;
		}else if(  tmp_def_value >= def_threshold_value + 60 && tmp_def_value < def_threshold_value + 120){
			real_damage = damage*0.7 - ( tmp_def_value - def_threshold_value - 50 )*damage/500;
		}else if( tmp_def_value  >= def_threshold_value + 120 && tmp_def_value < def_threshold_value + 180){
			real_damage = damage*0.5 - ( tmp_def_value - def_threshold_value - 100 )*damage/500;
		}else if( tmp_def_value  >= def_threshold_value + 180 && tmp_def_value < def_threshold_value + 264){
			int value = tmp_def_value - def_threshold_value - 150;
			real_damage = damage*0.35 * ( 1 -  value* sqrt(value)/1796 );
		}else{
			real_damage = damage* 0.2;	
		}
		float new_def_rate = target->def_rate - target->def_buf_trim;
		if (new_def_rate < 0.2) {
			new_def_rate = 0.2;
		}
		return real_damage * new_def_rate;
	//player or monster attack summon monster
	} else if (is_summon_mon(target->role_type) ) {
		float new_def_rate = target->def_rate - target->def_buf_trim;
		if (new_def_rate < 0.2) {
			return damage * 0.2;
		}
		return damage * new_def_rate;
	}	

	float new_def_rate = target->def_rate > target->def_buf_trim ? target->def_rate - target->def_buf_trim : 0.01;
	return damage * new_def_rate;
}


int  Player::calc_damage(Player* target, const skill_simple_info* p_skill_base, const skill_t* p_skill, const skill_effects_t* p_skill_info, int& crit_atk)
{
	int dmgs = 0;
	int base_dmgs = 0;
	crit_atk = 0;

	if (p_skill_info) {
        uint32_t attr_dmg = get_skill_attr_dmg(p_skill->id);
        //uint32_t extra_dmg = get_skill_extra_dmg(p_skill->id);
		//base_damage
		if (p_skill_base->type == normal_skill) {
			base_dmgs = p_skill_info->base_damage[0] * (atk + atk_buf_trim) / 100
				+ p_skill_info->base_damage[1] * ranged_random(weapon_atk[0], weapon_atk[1]) / 100;
			TRACE_LOG(" tmp damg----------- [%u][%d]", target->id, base_dmgs);
		} 
		else {
			base_dmgs = (p_skill_info->base_damage[0] + attr_dmg) * (atk + atk_buf_trim) / 100
				+ (p_skill_info->base_damage[1] + attr_dmg) * (ranged_random(weapon_atk[0], weapon_atk[1])) / 100 
				+ suit_add_attr.skill_atk;
			TRACE_LOG(" tmp damg----------- [%u][%d]", target->id, base_dmgs);
		}
		//elem_damage
		for (uint32_t i = 0; i < 5; i++) 
		{
			base_dmgs += p_skill_info->elem_damage[i];
		}
		TRACE_LOG(" tmp damg----------- [%u][%d]", target->id, base_dmgs);
		//addition damage
		base_dmgs += p_skill_info->addition_damage; 	
		TRACE_LOG(" tmp damg----------- [%u][%d]", target->id, base_dmgs);
        base_dmgs += get_skill_extra_dmg(p_skill->id);
		TRACE_LOG(" tmp damg----------- [%u][%d]", target->id, base_dmgs);
	}
	if( p_skill && p_skill_base->depend_damage_active_flag)
	{
		base_dmgs += p_skill->depend_info.numberic_damage;
		base_dmgs +=  base_dmgs* (p_skill->depend_info.percent_damage/ 100.0);
	}
    TRACE_LOG(" trace demage-----------skill> [%u][%d]", target->id, base_dmgs);
	dmgs = calc_defthreshold_damage(target, base_dmgs);	
    TRACE_LOG(" trace demage-----------def> [%u][%d]", target->id, dmgs);
	//if player lv bigger than monster
	if( is_valid_uid(target->id) ){
		uint32_t lv_diff = abs(lv - target->lv);
		uint32_t lv_radio = lv_diff >= 10 ? 50 : lv_diff * 5;
		if( lv > target->lv){
			dmgs = dmgs + dmgs * lv_radio / 100;	
		}
		if( lv < target->lv){
			dmgs = dmgs - dmgs * lv_radio / 100;
		}
	}
    TRACE_LOG(" trace demage-----------lv> [%u][%d]", target->id, dmgs);
	//if cirit
	if(is_crit()){
		crit_atk = 1;
		dmgs = (dmgs * (1.20 + ((float)critical_max_damage_buf_trim + 100.0) / 100.0));
	}
    TRACE_LOG(" trace demage-----------crit> [%u][%d] %u", target->id, dmgs, crit_atk);
	//random dmgs between some range
	uint32_t range = 0;
	if (!is_valid_uid(id)) {
		range = dmgs * 5 / 100;
	} 
	else {
		range = dmgs * 5 / 100;
	}

	if (is_valid_uid(id) && p_skill_base->type == normal_skill) {
		dmgs = dmgs + ranged_random(0, range);			
	} 
	else {
		dmgs = dmgs - range + ranged_random(0, range * 2);				
	}

    TRACE_LOG(" trace demage-----------range> [%u][%d]", target->id, dmgs);
	//per damage
	if (p_skill_info->per_damage > 0) {
		dmgs = target->hp * p_skill_info->per_damage / 100;					
		if(p_skill->id == 4120094 && dmgs <= 100){
			dmgs = 200;
		}
	}
	//damage ration
	if (target->damage_ration != 0) {
		dmgs = dmgs * target->damage_ration / 100;				
	}

	dmgs = calc_atkthreshold_damage(target, dmgs);
    TRACE_LOG(" trace demage-----------atk> [%u][%d]", target->id, dmgs);
	//damage change by buff
	dmgs = dmgs * atk_damage_change_buf_trim;	
    TRACE_LOG(" trace demage-----------buf> [%u][%d]", target->id, dmgs);
	//if the target has some effect , it will cause damage change too
	dmgs = dmgs * target->sustain_damage_change_buf_trim;		
    TRACE_LOG(" trace demage-----------buf_trim> [%u][%d]", target->id, dmgs);

	if (target->sustain_damage_value_change_buf_trim >= dmgs) {
		dmgs = 1;
	} else {
		dmgs -= target->sustain_damage_value_change_buf_trim;
	}
	return dmgs;
}

int Player::calc_hit_score(Player* target, int32_t damages, int32_t crit_atk)
{
	if( target->is_dead())
	{
		if ( !is_valid_uid(id) && is_valid_uid(target->id)) //怪物把人杀死
		{
			target->score.deaded_cnt++;
			//loss all clothes duration
			player_clothes_duration_chg(target, all_clothes);
			target->need_sav_ = true;
		}
		else if (!is_valid_uid( target->id))  //怪物被杀死
		{
			Player* pp = this;
			if (is_summon_mon(role_type))//如果是召唤兽杀死 
			{
				pp = summon_info.owner;															
			}
			if( damages > (int32_t)target->mon_info->hp /3 )
			{
				pp->score.sec_kill_cnt++;
			}
			if( target->role_type < 19001)
			{
				pp->score.kill_mon_cnt++;
				if( pp->my_summon && pp->my_summon->if_can_fight() && pp->my_summon->summon_info.add_anger(3))
				{
					pp->my_summon->noti_summon_anger_to_player();
				}
			}
		}
	}

	if( is_valid_uid(target->id))//玩家被攻击
	{
		target->score.on_hit_cnt++;
		if( target->my_summon && target->my_summon->if_can_fight() && target->my_summon->summon_info.add_anger(5))
		{
			target->my_summon->noti_summon_anger_to_player();
		}
		uint32_t duration_type = !crit_atk ? armor_normal : armor_crit;
		player_clothes_duration_chg(target, duration_type);
		target->need_sav_ = true;
	}
	else//怪物被攻击
	{
		Player* pp = this;
		if( is_summon_mon(role_type)){
			pp = summon_info.owner;
		}
		target->i_ai->set_hatred( target->i_ai->hatred() + 1);
		uint32_t tm = get_now_tv()->tv_sec;
		if( tm - pp->score.last_hit_tm <= 2)
		{
			pp->score.cur_hit_num ++;
			if( pp->score.cur_hit_num % 15 == 0)
			{
				pp->score.combo_hit_cnt++;
			}
		}
		else
		{
			pp->score.cur_hit_num = 0;
		}
		player_clothes_duration_chg(pp, weapon);
		pp->need_sav_ = true;
		pp->score.last_hit_tm = tm;
	}

	return 0;
}

int  Player::after_on_hit(Player* target)
{
	if( target->is_dead())//被杀死
	{
        if (target->mon_info && target->mon_info->type == 1) {
            boss_killer_flg = 1;
        }
		target->on_dead(this);
		if(!is_valid_uid(target->id) && !is_summon_mon(target->role_type)){
			target->leave_map();
		}	
	}

	if (is_summon_mon(target->role_type) && target->btl->pvp_lv() == pvp_dragon_ship) {
		add_player_buff(target, 1342, 0, 1, false);
	}
	return 0;
}

class AttackImpact {
 public:
	AttackImpact() {}
	~AttackImpact() {}

	void init()
	{
		hit_flag_ = Hit_Normal;
		fly_flag_ = Body_Normal; 
	}

	void CalcImpactInfo(SkillEffects * effect);
	void pack_impact(void * buf, int & idx);
	enum Impack_Hit_Type {
		Hit_Miss   = 0, //miss
		Hit_Normal = 1, //普通命中
		Hit_Crit   = 2, //暴击
	};

	enum Impack_Body_Type {
		Body_Normal = 0, //无特殊效果
		Body_Down  = 1, //倒地
		Body_FLy   = 2, //击飞
		Body_Back  = 3, //击退
	};

	enum Impack_Skill_Type {
		Skill_No_Break = 0,
		Skill_Break = 1,
	};


 private:
	AttackImpact(AttackImpact & other);
	AttackImpact & operator = (AttackImpact & other);
	uint32_t hit_flag_;
	uint32_t fly_flag_;  
	uint32_t damage_;
	uint32_t break_flag_;
	uint32_t time_duration_;
};

bool Player::can_be_hurted()
{
	if (is_dead() 
			|| btl->is_battle_ended()
			|| (is_invincibility())
			|| timecmp(*get_now_tv(), flying_end_tm) < 0) {
		return false;
	}
	return true;
}


bool
Player::on_hit(Player* atker, const skill_simple_info* p_skill_base, const skill_t* p_skill,  const skill_effects_t* p_skill_info, bool last_hit)
{

	struct timeval cur_time   = *get_now_tv();	
	//被攻击(攻击伤害之前判定）触发效果
	player_passive_buff_trigger(this, cur_time, on_attack_effect_trigger_type);	

//	AttackImpact * calc_attack_impact(
//	
	//////////////////////////////////////////////////////////////////////////////////////////////
	//send to btl for this hit info
	if ( p_skill_base->id != 4029999 && !atker->is_hit(this, p_skill) ) {
		int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
		//idx += pack_on_hit_info(pkgbuf + idx, atker, p_skill_base, 0, 0, p_skill_info, false, 0);
		idx += calc_attack_impact(pkgbuf + idx, atker, p_skill_base, 0, 0, p_skill_info, false);

		init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_players_hit, idx - sizeof(btl_proto_t));
		init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
		btl->send_to_btl(pkgbuf, idx, 0, 0);
		TRACE_LOG("on hit!!! %u ", id);
		return true;
	}


	inc_be_hit_count();

	//TODO:compute the damage
	int crit = 0;
	int dmgs = atker->calc_damage(this, p_skill_base, p_skill, p_skill_info, crit);
    TRACE_LOG(" trace demage-----------> [%u][%d]", id, dmgs);

	//受伤触发效果
	if (dmgs) {
		player_passive_buff_trigger(this, cur_time,	on_damage_effect_trigger_type);	
	}

    //////////////////////////////////////////////////////////////////////
    //检查攻击者是否有吸血攻击buff效果
	effect_data* pEffectdata = get_effect_data_in_player(atker, vampire_attach_effect_type);
	if( pEffectdata )	
	{
		int vampire_dmgs = dmgs * pEffectdata->trigger_percent / 100;
		atker->chg_hp(vampire_dmgs);
		atker->noti_hpmp_to_btl();
	}
    //////////////////////////////////////////////////////////////////////////
	//有X%的概率对敌人造成x倍伤害 (类似暴击之类的)
	pEffectdata = get_effect_data_in_player(atker, crit_attach_effect_type);
	if( pEffectdata )	
	{
		if ((uint32_t)ranged_random(0, 100) < pEffectdata->trigger_rate) {
			dmgs += dmgs * pEffectdata->trigger_percent / 100;
			crit = 1;
		}
	}
    TRACE_LOG(" trace demage-----------> [%u][%d]", id, dmgs);

    int rebound_dmgs = dmgs; //保存反弹的伤害的基础值

    //////////////////////////////////////////////////////////////////////////
	//对玩家技能有着X%的抗性
	pEffectdata = get_effect_data_in_player(this, damage_reduction_effect_type);
	if( pEffectdata )	
	{
		if ((uint32_t)ranged_random(0, 100) < pEffectdata->trigger_rate) {
			dmgs -= dmgs * pEffectdata->trigger_percent / 100;
		}
	}
    TRACE_LOG(" trace demage-----------> [%u][%d]", id, dmgs);

	// compute hp after being hit
	TRACE_LOG("on hit!!! %u hp%u %u", dmgs, hp, maxhp);
	chg_hp(-dmgs);
	atker->damage += dmgs;	
	// notify players who are hit
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	//idx += pack_on_hit_info(pkgbuf + idx, atker, p_skill_base, dmgs, crit, p_skill_info, last_hit, break_skill_flag);
	idx += calc_attack_impact(pkgbuf + idx, atker, p_skill_base, dmgs, crit, p_skill_info, last_hit);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_players_hit, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	btl->send_to_btl(pkgbuf, idx, 0, 0);
	TRACE_LOG("on hit!!! %u|%u hp%u %u", id, role_type, hp, maxhp);

	//calc score for this hit
	atker->calc_hit_score(this, dmgs,  crit);	
	atker->after_on_hit(this);

	if(!is_dead())	
	{
		if(p_skill_info->buff_id != 0 && 
			(p_skill_info->buff_target == 0 || p_skill_info->buff_target == enemys))
		{	
			uint32_t atker_id = atker->id;
			if (is_summon_mon(atker->role_type) && atker->summon_info.owner){
				atker_id = atker->summon_info.owner->id;
			}
			add_player_buff(this, p_skill_info->buff_id, atker_id);
		}
		if (p_skill_info->imme_buff != 0)
		{
			process_skill_effect_logic(atker, p_skill_info->imme_buff, this);
		}
	}

    /////////////////////////////////////////////////////////////////////////////////////////
    //check the player who has attacked has effect of rebounding damage (反弹伤害)
	pEffectdata = get_effect_data_in_player(this, rebound_attack_effect_type);
	if( pEffectdata && !(atker->is_dead()) )	
	{
		Player* real_atker = atker;
        if ((uint32_t)ranged_random(0, 100) < pEffectdata->trigger_rate) {
            if( real_atker->p_cur_skill)
            {
                real_atker->p_cur_skill->set_dead();
                real_atker->on_skill_end(real_atker->p_cur_skill);
            }

            rebound_dmgs = rebound_dmgs * pEffectdata->trigger_percent / 100;

            real_atker->chg_hp(-rebound_dmgs);
            real_atker->notify_damage_by_buff(id, real_atker->hp, rebound_dmgs);	
            calc_hit_score(real_atker, rebound_dmgs, crit);
            after_on_hit(real_atker);
        }
	}
	return true;
}

bool
Player::on_buf_hit(Player* atker, const skill_simple_info* p_skill_base, const buff_effect_t* p_buf_info, uint32_t effect_target)
{
	if (invincible_time || timecmp(*get_now_tv(), flying_end_tm) < 0 || 
			is_effect_exist_in_player(this, invincibility_type)) {
		return false;
	}
	
	return true;
}

bool
Player::on_buf_hit(Player* atker, uint32_t buff_id, uint32_t buff_lv)
{
	//TODO:if can get buff effect?
	if (invincible_time || timecmp(*get_now_tv(), flying_end_tm) < 0 ||
			is_effect_exist_in_player(this, invincibility_type)) {
		return false;
	}
	
	//get buff effect info
	const buff_effect_t* buf_effect = g_skills->get_buff_effect(buff_id, buff_lv);
	if (buf_effect == 0) {
		return false;
	}
	
	return true;
}

void
Player::noti_buf_event_to_map(uint16_t buf_id, uint8_t buf_type, uint8_t flag)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, id, idx);
	pack(pkgbuf, buf_id, idx);
	pack(pkgbuf, buf_type, idx);
	pack(pkgbuf, flag, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_players_buf_event_noti, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	btl->send_to_btl(pkgbuf, idx, 0, 0);
	TRACE_LOG("buff change [%u %u %u %u]", id, buf_id, buf_type, flag);
}

void Player::noti_aura_event_to_map(uint16_t aura_type, uint8_t flag, uint8_t up_flag)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, id, idx);
	pack(pkgbuf, aura_type, idx);
	pack(pkgbuf, flag, idx);
	pack(pkgbuf, up_flag, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_players_aura_event_noti, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	btl->send_to_btl(pkgbuf, idx, 0, 0);
}


void
Player::hp_mp_chg_buf_event(int hp_chg, int mp_chg, uint32_t interval, uint32_t times)
{
	player_hpmp_chg_info_t info;
	info.hp_chg = hp_chg;
	info.mp_chg = mp_chg;
	hpmp_chg(info);
	timeval tv = *get_now_tv();
	tv.tv_sec += interval;
	ev_mgr.add_event(*this, &Player::hpmp_chg, info, tv, interval * 1000, times - 2);
}

void
Player::add_per_hp_mp_immediately(int per_hp_chg, int per_mp_chg)
{
	player_hpmp_chg_info_t info;
	info.hp_chg = max_hp() * per_hp_chg / 100;
	info.mp_chg = max_mp() * per_mp_chg / 100;
	hpmp_chg(info);
}

void
Player::noti_summon_anger_to_player()
{
	if (summon_info.owner) {
		int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
		pack(pkgbuf, id, idx);
		pack(pkgbuf, summon_info.anger_value, idx);
		init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_summon_anger_noti, idx - sizeof(btl_proto_t));
		init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
		send_to_player(summon_info.owner, pkgbuf, idx, 0);
	}
}

void
Player::set_pos_after_onhit(const Player* atker, const skill_effects_t* p_skill_info, bool last_hit, uint32_t hit_flag)
{
	int offset_x = 0;
	if (last_hit && p_skill_info->offset_x > 0) {
		offset_x = p_skill_info->offset_x;
	} else {
		if (hit_flag == 1) {
			offset_x = 10;
		} else if (hit_flag == 2) {
			offset_x = 80 + rand() % 30;
		} else if (hit_flag == 3) {
			offset_x = 200 + rand() % 50;
		} else  {
			offset_x = p_skill_info->offset_x;
		}
	}

		


	
	//get a new pos
	Vector3D offset_pos(offset_x,0,0);
	Vector3D new_pos;
	if ( p_skill_info->dir == 1 ) {//front of player
		new_pos = pos() + (atker->dir==dir_right ? offset_pos : offset_pos.get_reverse());
	} else if ( p_skill_info->dir == 2 ){//back of player
		new_pos = pos() - (atker->dir==dir_right ? offset_pos : offset_pos.get_reverse());
	} else {
		new_pos = pos() + (atker->pos().x() < pos().x() ? offset_pos : offset_pos.get_reverse());
	}

	//test the new pos
	KfAstar::Point pt_start(pos().x(), pos().y());
	uint16_t x = new_pos.x() < 0 ? 0 : new_pos.x();
	KfAstar::Point pt_end(x, new_pos.y());
	
	//const KfAstar::Points* pt = cur_map->path->find_linear_path(pt_start, pt_end);
	KfAstar::Points* pt = NULL;
	KfAstar* p_star = btl->get_cur_map_path( cur_map );
	if(p_star)
	{
		pt = (KfAstar::Points*)p_star->find_linear_path(pt_start, pt_end);
		new_pos.set_x((*pt)[0].x);
		set_pos(new_pos);
	}
	detective_info.be_hit_tm = get_now_tv()->tv_sec;
	detective_info.hit_offset_x = offset_x;
}

void Player::set_skill_addition_attr(uint32_t skill_id, addition_attr_t* pattr)
{
    SkillMap::iterator it = skills_map.find(skill_id);
    if ( it == skills_map.end() || it->second.flag == 0) {
        ERROR_LOG("Player no skill [%u]", skill_id);
        return;
    }
    it->second.skill_attr.cut_cd    += pattr->cut_cd;
    it->second.skill_attr.cut_mp    += pattr->cut_mp;
    it->second.skill_attr.attr_dmg  += pattr->attr_dmg;
	//ERROR_LOG("ADD ATTR_DMG %u  %u %u", skill_id, it->second.skill_attr.attr_dmg,  pattr->attr_dmg);
    it->second.skill_attr.extra_dmg += pattr->extra_dmg;
}

void Player::reset_skill_addition_attr()
{
   	SkillMap::iterator it = skills_map.begin();
   	for (; it != skills_map.end(); ++it) {
		it->second.skill_attr.cut_cd = 0;
		it->second.skill_attr.cut_mp = 0;
		it->second.skill_attr.attr_dmg = 0;
		//ERROR_LOG("ADD ATTR_DMG %u  %u %u", skill_id, it->second.skill_attr.attr_dmg,  pattr->attr_dmg);
		it->second.skill_attr.extra_dmg = 0;
   	}
}

uint32_t Player::get_skill_attr_dmg(uint32_t skill_id)
{
    SkillMap::iterator it = skills_map.find(skill_id);
    if ( it == skills_map.end() )return 0;
    return it->second.skill_attr.attr_dmg;
}

uint32_t Player::get_skill_extra_dmg(uint32_t skill_id)
{
    SkillMap::iterator it = skills_map.find(skill_id);
    if ( it == skills_map.end() )return 0;
    return it->second.skill_attr.extra_dmg;
}

void 
Player::set_skill_cool_time(uint32_t skill_id, int cool_time)
{
	struct timeval tv = *get_now_tv();

	timeadd(tv, cool_time / 1000.0);
	
	SkillMap::iterator it = skills_map.find(skill_id);
	assert( it != skills_map.end());
	it->second.last_tv = tv;
}

void 
Player::set_all_skill_cool_time(int cool_time)
{
	struct timeval tv = *get_now_tv();

	timeadd(tv, cool_time / 1000.0);
	
	SkillMap::iterator it = skills_map.begin();
    for (; it != skills_map.end(); ++it) {
        //if (it->second.skill_id == 100605) continue;
        it->second.last_tv = tv;
    }
}

uint32_t  Player::get_skill_cool_time( uint32_t skill_id)
{
	player_skill_t* p_skill = NULL;
	SkillMap::iterator it = skills_map.find(skill_id);
	if ( it == skills_map.end() )return 0;
	p_skill = &(it->second);

	return (p_skill->p_skill_info->cool_down - p_skill->skill_attr.cut_cd) * skill_cd_time_buf_trim;
}


double  Player::get_skill_mp_consumed( uint32_t skill_id)
{
	if (!is_valid_uid(id) && !is_summon_mon(role_type)) {
		return 0;
	}

	player_skill_t* p_skill = NULL;
	SkillMap::iterator it = skills_map.find(skill_id);
	if ( it == skills_map.end() )return 0;
	p_skill = &(it->second);

	float base_mp = 100;
	if (is_valid_uid(id)) {
		base_mp = calc_player_base_mp_ex(this);
	}

    float cut_mp = p_skill->skill_attr.cut_mp;

    float base_mp_per = p_skill->skill_info->base_mp_consume;
	float mp_consumed = p_skill->skill_info->mp_consume;

    //NOTI_LOG("addition cutmp [%u][%f %f %f]", skill_id, base_mp, cut_mp, mp_consumed);
	return (mp_consumed + base_mp * base_mp_per / 100.0) * 
			((100.0 - cut_mp) / 100.0);
}

void 
Player::on_skill_end(Object* p_skill)
{
	ObjList::iterator it = std::find(objlist_.begin(), objlist_.end(), p_skill);
	//assert( it != objlist_.end() );
	if (it == objlist_.end()) {
		WARN_LOG("on skill end error[%u]", id);
		return;
	}
	TRACE_LOG("player destroy skill %u",p_cur_skill ? p_cur_skill->get_skill_id() : (*it)->id());
	//if skill still on effect
	if ( p_cur_skill == p_skill ) {
		invincible = false;
		p_cur_skill = 0;TRACE_LOG("skillupdate in start_stage3 %u", id);
	}

	objlist_.erase(it);
	return;
}

void
Player::skill_effect_noti(uint32_t skill_id, uint32_t x, uint32_t y)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, skill_id, idx);
	pack(pkgbuf, x, idx);
	pack(pkgbuf, y, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_skill_effect_noti, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	cur_map->send_to_map(pkgbuf, idx);
	TRACE_LOG("[%u %u %u]", skill_id, x, y);

}

player_skill_t*
Player::select_specifed_skill(uint32_t skill_id)
{
	player_skill_t* selected_skill = 0;

	for ( SkillMap::iterator it = skills_map.begin(); it != skills_map.end(); ++it ) {
		//get skill by skillid
		player_skill_t* tmp_skill = &(it->second);
		skill_t* p_skill = g_skills->get_skill(tmp_skill->skill_id);
		if (p_skill->id == skill_id) {
			selected_skill = tmp_skill;
			break;
		}
	}
	return selected_skill;
}


player_skill_t*
Player::select_skill(const Player* preyer, uint32_t skillid)
{
	//select assigm skill
	if (skillid != 0) {
		return select_assign_skill(preyer, skillid);
	}

	SkillEffects p_skill_effects(this, 0, 0, 0, 0);
	player_skill_t* selected_skill = 0;
	uint32_t max_damage = 0;
	
	for ( SkillMap::iterator it = skills_map.begin(); it != skills_map.end(); ++it ) {
		//get skill by skillid
		player_skill_t* tmp_skill = &(it->second);

		skill_t* p_skill = g_skills->get_skill(tmp_skill->skill_id);

		//select attack skills
		if ( p_skill->id == 4029999 || (!skillid && !(p_skill->is_atk_skill() || p_skill->is_mult_continue_atk_skill())) 
				|| !can_use_skill(tmp_skill->skill_id, false) ) {
			continue;
		}

		//only ai control
		if (p_skill->ai_control_flag) {
			continue;
		}
		if (p_skill->type == passive_skill) {
			continue;
		}

		if (is_summon_mon(role_type) && 
			(tmp_skill->skill_id == 4020027)) {
			continue;
		}

		//calc the damage of every effect
		skill_lv_info_t* lv_info = &(p_skill->lv_info[tmp_skill->lv]);
		uint32_t all_damage = 0;
		for ( uint32_t j = 0; j < lv_info->effects_num; j ++ ) {
			p_skill_effects.init(&(lv_info->effects[j]), p_skill);
			all_damage += p_skill_effects.get_damage(preyer);
		}

		//select effect which create max damage
		if ( all_damage > max_damage ) {
			selected_skill = tmp_skill;
			max_damage = all_damage;
		}
	}

//	if (selected_skill) {
//		ERROR_LOG("SELECT SKILL %u", selected_skill->skill_id);
//	}

	return selected_skill;
}

player_skill_t*
Player::select_assign_skill(const Player* preyer, uint32_t skillid)
{
	for ( SkillMap::iterator it = skills_map.begin(); it != skills_map.end(); ++it ) {
		//get skill by skillid
		player_skill_t* tmp_skill = &(it->second);

		//designated skill id
		if (skillid == tmp_skill->skill_id) {
			if (!can_use_skill(skillid, false) ) {
				return 0;
			}
			//face to preyer
			if (preyer) {
				if ( (pos().x() > preyer->pos().x() && dir == dir_right) || 
					 (pos().x() < preyer->pos().x() && dir == dir_left) ) {
					//turn back
					dir = dir == dir_right ? dir_left : dir_right;
					//monster_stand();
					int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
					pack(pkgbuf, pos().x(), idx);
					pack(pkgbuf, pos().y(), idx);
					pack(pkgbuf, dir, idx);
					init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), id, cli_proto_stand, 0, idx - sizeof(btl_proto_t));
					init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
					cur_map->send_to_map(pkgbuf, idx);
				}
			}

			return tmp_skill;
		}
	}

	return 0;
}

player_skill_t*
Player::select_super_armor_skill()
{
	player_skill_t* selected_skill = 0;

	for ( SkillMap::iterator it = skills_map.begin(); it != skills_map.end(); ++it ) {
		//get skill by skillid
		player_skill_t* tmp_skill = &(it->second);
		skill_t* p_skill = g_skills->get_skill(tmp_skill->skill_id);
		if (p_skill->id == 4029999) {
			selected_skill = tmp_skill;
			break;
		}
	}
	return selected_skill;
}

player_skill_t*
Player::select_buff_skill(const Player* preyer, uint32_t buff_skill)
{
	player_skill_t* selected_skill = 0;

	if (buff_skill) {
		for ( SkillMap::iterator it = skills_map.begin(); it != skills_map.end(); ++it ) {
			//get skill by skillid
			if ((it->second).skill_id == buff_skill) {
				skill_t * p_skill = g_skills->get_skill((it->second).skill_id);
				if (p_skill->is_buff_skill() && can_use_skill(p_skill->id, false)) {
					return &(it->second);
				}	
			}	
		}
		return NULL;
	}

	
	for ( SkillMap::iterator it = skills_map.begin(); it != skills_map.end(); ++it ) {
		//get skill by skillid
		player_skill_t* tmp_skill = &(it->second);
		skill_t* p_skill = g_skills->get_skill(tmp_skill->skill_id);

		//only ai control
		if (p_skill->ai_control_flag) {
			continue;
		}

		//tmp deal
		if (tmp_skill->skill_id == 4020013 || tmp_skill->skill_id == 4020020) {
			continue;
		}

		if (p_skill->type == passive_skill) {
			continue;
		}

		if ( !(p_skill->is_atk_skill()) && can_use_skill(tmp_skill->skill_id, false) ) {
			selected_skill = tmp_skill;
			break;
		} 
	}

	TRACE_LOG("buf skill select[%u %p]",id, selected_skill);
	return selected_skill;
}


attack_region_t 
Player::get_attack_region(uint32_t skill_id)
{
	SkillEffects p_skill_effects(this, 0, 0, 0, 0);

	//bool usable = false;
	attack_region_t rgn;

	//TRACE_LOG("skill_num[%u]",skills_num);
	SkillMap::iterator it;
	for ( it = skills_map.begin(); it != skills_map.end(); ++it ) {
		//get skill by skillid
		player_skill_t* tmp_skill = &(it->second);
		skill_t* p_skill = g_skills->get_skill(tmp_skill->skill_id);
		
		if (skill_id != 0 && tmp_skill->skill_id != skill_id) {
			continue;
		}

		if ( p_skill->id == 4029999 || !(p_skill->is_atk_skill()) 
				|| !can_use_skill(tmp_skill->skill_id, false) ) {
			//give a rgn of buf_skill ,tmp resolvent
			if (skill_id != 0 && p_skill->is_buff_skill()) {
				rgn.x = 300;
				rgn.y = 300;
				return rgn;
			}
				
			continue;
		}

		skill_lv_info_t* lv_info = &(p_skill->lv_info[tmp_skill->lv]);
		//calc the region of first effect
		if ( lv_info->effects_num > 0 ) {
			TRACE_LOG("SKILL UPDATE");
			p_skill_effects.init(&(lv_info->effects[0]), p_skill);
			attack_region_t tmp = p_skill_effects.get_atk_region();
			
			if (rgn < tmp) {
				rgn = tmp;
			}
		}
	}
	
	return rgn;
}

void Player::call_map_summon(uint32_t summon_id, uint32_t x, uint32_t y, bool notify, bool owner_flg)
{
    TRACE_LOG("dead [%u] call summon [%u]", role_type, summon_id);
	map_summon_object* m = create_map_summon_object(summon_id);
	if(m == NULL)return;
	m->set_pos( Vector3D(x, y) , cur_map);

	uint32_t owner_tmp = 0;
	if (owner_flg) {
		owner_tmp = id;
	} else {
		owner_tmp = 0;
	}
	m->set_owner_id(owner_tmp, team, btl);
	btl->add_map_summon(m, notify);
}


void
Player::call_monster(uint32_t mon_id, uint32_t x, uint32_t y, int team, int add_buff_id, int add_aura_id)
{
	if (cur_map && btl) {
		call_monster_to_map(cur_map, btl, mon_id, x, y, team, add_buff_id, add_aura_id);
	}
}

void
Player::call_monsters(uint32_t mon_id, uint32_t mon_cnt)
{
	if (cur_map && btl) {
		//find monster point
		//const KfAstar::Points* points = cur_map->path->find_surrounding_points2(KfAstar::Point(pos().x(), pos().y()));
		KfAstar::Points* points = NULL;
		KfAstar* p_star = btl->get_cur_map_path(cur_map );
		if(p_star)
		{
			points = (KfAstar::Points*)p_star->find_surrounding_points2(KfAstar::Point(pos().x(), pos().y()));
			for (uint32_t i = 0; i < mon_cnt; i++) 
			{
				int idx = i % (points->size());
				call_monster_to_map(cur_map, btl, mon_id, (*points)[idx].x, (*points)[idx].y);
			}
		}
	}
}

void
Player::update(int milli_sec)
{
	player_timer(this, *get_now_tv());
   	if( is_dead()){
		return;
	}	
	if (this->btl && !btl->is_battle_ended()) {
		i_ai->UpdateAI(milli_sec);
	}
}
/*
void
Player::unpack_buf_skill(uint32_t num, const uint8_t* body)
{
	buf_skill_t buf_skill = {0};
	buf_skill_map.clear();
	int idx = 0;
	for (uint32_t i = 0; i < num; ++i) {
		memset(&buf_skill, 0, sizeof(buf_skill));
		buf_skill.start_tm = get_now_tv()->tv_sec;
		unpack_h(body, buf_skill.buf_type, idx);
		unpack_h(body, buf_skill.buf_duration, idx);
		unpack_h(body, buf_skill.per_trim, idx);
		unpack_h(body, buf_skill.db_buff_flg, idx);
		unpack_h(body, buf_skill.mutex_type, idx);
		buf_skill_map.insert(BufSkillMap::value_type(buf_skill.buf_type, buf_skill));
		TRACE_LOG("buf skill[%u %u %u ]", buf_skill.buf_type, buf_skill.buf_duration, buf_skill.per_trim);
	}
}
*/

bool Player::is_skill_exist(uint32_t skill_id)
{
	SkillMap::iterator it = skills_map.find(skill_id);
	return it != skills_map.end();
}

void Player::calc_depend_skill_damage()
{
	for ( SkillMap::iterator it = skills_map.begin(); it != skills_map.end(); ++it ) 
	{
		player_skill_t* tmp_skill = &(it->second);
		skill_t* p_skill = g_skills->get_skill(tmp_skill->skill_id);
		if(p_skill == NULL)continue;
		if(!p_skill->depend_info.skill_id)continue;
		if( is_skill_exist(p_skill->id)){
			tmp_skill->depend_damage_active_flag = 1;	
		}
	}
}

void
Player::unpack_all_skill(uint32_t skills_num, uint32_t buff_num, uint32_t home_btl_pets_num, const uint8_t* body)
{
	buf_skill_t buf_skill = {0};
	buf_skill_map.clear();
	int idx = 0;
	for (uint32_t i = 0; i < buff_num; ++i) {
		memset(&buf_skill, 0, sizeof(buf_skill));
		buf_skill.start_tm = get_now_tv()->tv_sec;
		unpack_h(body, buf_skill.buf_type, idx);
		unpack_h(body, buf_skill.start_tm, idx);
		unpack_h(body, buf_skill.buf_duration, idx);
		unpack_h(body, buf_skill.per_trim, idx);
		unpack_h(body, buf_skill.db_buff_flg, idx);
		unpack_h(body, buf_skill.mutex_type, idx);
		buf_skill_map.insert(BufSkillMap::value_type(buf_skill.buf_type, buf_skill));
		TRACE_LOG("buf skill[%u %u %u ]", buf_skill.buf_type, buf_skill.buf_duration, buf_skill.per_trim);
	}
    //init skills
    player_skills_info_t skills_unit = {0};
    for (uint32_t i = 0; i < skills_num; i ++) {
        memset(&skills_unit, 0, sizeof(skills_unit));
        unpack_h(body, skills_unit.skill_id, idx);
        unpack_h(body, skills_unit.skill_lv, idx);
        unpack_h(body, skills_unit.bind_flag, idx);
        player_skill_t skill(skills_unit.skill_id, skills_unit.skill_lv, skills_unit.bind_flag);
        skills_map.insert(SkillMap::value_type(skills_unit.skill_id, skill));
        TRACE_LOG("skill [%u %u %u]",skills_unit.skill_id, skills_unit.skill_lv, skills_unit.bind_flag);
    }
    //init player normal attack
	for ( uint32_t i = 0; i < normal_attack_num; i++ ) {
		uint32_t skillid = role_type * 100000 + 501 + i;
		player_skill_t skill(skillid, 1, 1);
		skills_map.insert(SkillMap::value_type(skillid, skill));
	}
	//init home_btl_pet
	clear_home_btl_pets();
	for(uint32_t i =0; i< home_btl_pets_num; i++)
	{
		uint32_t pet_id = 0;
		unpack_h(body, pet_id, idx);
		add_home_btl_pets(pet_id);
	}
}

void
Player::init_tmp_team_attr(const tmp_team_pvp_attr_t* p_attr)
{
	tmp_team_id = p_attr->tmp_team_id;
	tmp_team_leader = p_attr->tmp_team_leader;
}
 
void
Player::init_player_attr(const player_attr_t* attr)
{
	//init normal attr
	role_type  	 = attr->role_type;
	power_user   = attr->power_user;
	mon_info     = get_monster(role_type);
	speed = mon_info->spd;
	role_tm    	 = attr->role_tm;
	memcpy(nick, attr->nick, sizeof(nick));
	parentid	 = attr->parentid;
	memcpy(once_bit, attr->once_bit, sizeof(once_bit));
	TRACE_LOG("once bit :%x %x %x %x ", once_bit[0], once_bit[1]
			, once_bit[2], once_bit[10]);
	achieve_title= attr->achieve_title;
	lv           = attr->lv;
	exp          = attr->exp;
	allocator_exp = attr->allocator_exp;
    dexp_tm      = attr->dexp_tm;
    if (dexp_tm > 0) {
        need_sav_ = true;
    }
	half_exp     = attr->half_exp;
	coins        = attr->coins;
	pre_coins = coins;
	skill_point  = attr->skill_point;
    pre_skill_point = attr->skill_point;
	hp           = attr->hp;
	mp           = attr->mp;
	attire_gs    = attr->attire_gs;
	continue_win = attr->continue_win;
	honor 		 = attr->honor;
	max_conti_win_times = attr->max_conti_win_times;
	btl_again	 = attr->btl_again;
	fumo_tower_top		= attr->fumo_tower_top;
	fumo_tower_used_tm = attr->fumo_tower_used_tm;
	show_state	 = attr->show_state;
    taotai_do_times = attr->taotai_do_times;
    taotai_win_times = attr->taotai_win_times;
	team_lv = attr->team_lv;
    memcpy(&add_quality, &(attr->add_quality), sizeof(quality_t));

	pass_cnt = attr->pass_cnt;
	if (pass_cnt >= max_diff_cnt) {
		pass_cnt = max_diff_cnt - 1;
	}

//	memcpy(other_active_info, attr->other_actived_info, sizeof(other_active_info));

	memcpy(unique_item_bit, attr->unique_item_bit, sizeof(unique_item_bit));
    TRACE_LOG("unique_item_bit : [%x]", unique_item_bit[8]);
	unpack_all_skill(attr->skills_num, attr->buf_skill_num, attr->home_btl_pet_num, attr->buf);
	calc_depend_skill_damage();

	vip = attr->vip;
	vip_level = attr->vip_level;
	max_bag_grid_count = attr->max_bag_grid_count;

	//init clothes
	player_clothes_init(this, attr);

	//init task items
	for ( uint32_t i = 0; i < attr->task_items_num; i++ ) {
		if (attr->task_items[i].monster_id) {
			task_item_map.insert(TaskItemMap::value_type(attr->task_items[i].monster_id, attr->task_items[i]));
		} else {
			stage_task_list.push_back(attr->task_items[i]);
		}
	}

	fumo_points_start = attr->fumo_points_today;
	fumo_points_end = fumo_points_start;


	//TODO:init summon info
	if (attr->summon_info.mon_tm != 0) {
//		ERROR_LOG("PVP SUMMON %u ---------------", id);
		summon_info.attr_type = attr->summon_info.attr_type;
		summon_info.attr_per = attr->summon_info.attr_per;
		summon_info.mon_type = attr->summon_info.mon_type;
		summon_info.mon_tm = attr->summon_info.mon_tm;
		summon_info.mon_lv = attr->summon_info.mon_lv;
		summon_info.mon_exp = attr->summon_info.mon_exp;
		summon_info.fight_value = attr->summon_info.mon_fight_value;
		summon_info.renascence_time = attr->summon_info.renascence_time;
		//skills
		summon_info.skills_cnt = attr->summon_info.skills_cnt;
		memcpy(summon_info.skills, attr->summon_info.skills, sizeof(summon_info.skills));
		memcpy(summon_info.nick, attr->summon_info.nick, max_nick_size);
	}

	if (attr->numen_info.numen_id) {
		numen_info.numen_id = attr->numen_info.numen_id;
        memcpy(numen_info.numen_nick, attr->numen_info.name, max_nick_size);
		memcpy(numen_info.skill_arr,  attr->numen_info.skill_arr, sizeof(numen_info.skill_arr));
	}
	
	
	calc_player_attr();


	if (this->atk >= 5000) {
		ERROR_LOG("PLayer %u Role_Type %u ATTACK %u TOO HIGH", this->id, this->role_type, this->atk); 
	}


	if (g_special_double_time_mrg.is_in_double_time()) {
		in_specal_double_time = true;
	} else {
		in_specal_double_time = false;
	}

	if (g_special_double_time_mrg.is_in_double_time_2()) {
		in_specal_double_time_2 = true;
	} else {
		in_specal_double_time_2 = false;
	}

	if (g_special_double_time_mrg.is_in_double_time_witch_can_still_use_double_tm_item()) {
		in_specal_double_time_witch_can_still_use_double_tm_item = true;
	} else {
		in_specal_double_time_witch_can_still_use_double_tm_item = false;
	}
}
/**
 * @brief calc title attribute
 */
void calc_player_title_attribute(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return ;
    }

    static float hit_para = (25)/ pow(p->lv, 1.32) / 100; 

    p->agility_      += pattr->agility;
    p->strength     += pattr->strength;
    p->body_quality_ += pattr->body;
    p->stamina      += pattr->stamina;

    p->hit_rate     += pattr->hit * hit_para;
    p->def_value    += pattr->def;

    p->atk          += pattr->atk;
    p->maxhp        += pattr->hp;
    p->maxmp        += pattr->mp;
}

#define OLD_CALC 0
/**
 * @brief calc and init a player's battle attr ex
 */
void Player::calc_player_attr_ex(bool reset_cur_status)
{
	if (is_summon_mon(role_type)){
		return calc_summon_attr(reset_cur_status);
	}	
	calc_player_additional_basic_attribute(this);
#if OLD_CALC
	//be carefull you should call "calc_player_attr_by_suit " function first 
	//because the below function depends on this function
	calc_player_attr_by_suit(this);

	//////////user attribute factor para/////////////
	mon_info = get_monster(role_type);

	/* Four base attribute */
	strength = 		calc_player_strength_attr(this);
	agility_  = 		calc_player_agility_attr(this);
	body_quality_ = 	calc_player_body_quality_attr(this);
	stamina = 		calc_player_stamina_attr(this);	

    /* no base attribute affect*/
	hit_rate   = calc_player_hit_attr(this);        /**/
	def_value = calc_player_def_value_attr(this);   /**/

    /* Note: follow the base attribute */
    //calc_player_title_attribute(this);

	calc_player_recover_hp_attr(this);          /* body_quality */
	calc_player_recover_mp_attr(this);          /* stamina */
	def_rate =  calc_player_def_rate_attr(this);    /* def value */
	crit_rate = calc_player_crit_attr(this);        /* agility      */

    maxhp   = calc_player_hp_attr(this);        /* body_quality */
 	maxmp   = calc_player_mp_attr(this);        /* stamina      */
	atk     = calc_player_atk_attr(this);       /* strength     */ 
    dodge_rate = calc_player_dogde_attr(this);  /* agility      */
#else
    calc_player_attr_ex2(this);
	add_player_team_attr(reset_cur_status);

	calc_player_additional_attribute(this);
#endif
	if (reset_cur_status) {
		hp = maxhp;
		mp = maxmp;	
	}
	
	set_region(mon_info->len, mon_info->width, mon_info->height);
}

/**
 * @brief calc and init a player's battle attr
 */
void Player::calc_player_attr(bool reset_cur_status)
{
	calc_player_attr_ex(reset_cur_status);
    TRACE_LOG("plsyer base attr: agility=%u strength=%u body=%u stamina=%u", 
        agility(), strength, body_quality(), stamina);
	TRACE_LOG("player attr: "
				"uid=%u lv=%u exp=%u hp=%u/%u mp=%u/%u atk=%u crit=%u agi=%u def=%f hit=%f dodge=%f addhp=%u addmp=%u",
				id, lv, exp, hp, max_hp(), mp, max_mp(), atk, crit_rate, agility(), def_rate, hit_rate, dodge_rate, 
				auto_add_hpmp.hp_chg, auto_add_hpmp.mp_chg);
}

void calc_summon_attr_ex(attr_data * attr, uint32_t attr_type, uint32_t attr_per, uint32_t lv)
{
	calc_summon_attr(attr, attr_type, attr_per, lv);

}


/**
 * @brief calc and init a summon's battle attr
 */
void Player::calc_summon_attr(bool reset_cur_status)
{
	attr_data * attr = new attr_data;

   	calc_summon_attr_ex(attr,
					  summon_info.attr_type,
		  			  summon_info.attr_per,
					  summon_info.mon_lv);

	agility_ =  attr->agility;
	strength = attr->strength;
	stamina =  attr->stamina; 
	body_quality_ = attr->body_quality;
	accurate_value = attr->accurate_value;

	if (reset_cur_status) hp = attr->hp;
	maxhp = attr->hp;
	mp = 0;
	maxmp = 0;
	atk = attr->attack;
	
	crit_value = attr->crit_value;  
	hit_value =  attr->hit_value;
	def_value =  attr->def_value;
	dodge_value = attr->dodge_value;
	
	crit_rate = calc_pet_crit_rate_attr(this);
	def_rate = calc_pet_def_rate_attr(this);
	hit_rate = calc_pet_hit_rate_attr(this);
	dodge_rate = 1 - calc_pet_dodge_rate_attr(this);

    TRACE_LOG("summon arr [%u] [%u] [%u %f %f %f]", 
        maxhp, atk, crit_rate, def_rate, hit_rate, dodge_rate);
	delete attr;

}

int
Player::pack_rt_birth_mon_basic_info(void* buf)
{
	int idx = 0;

    if (summon_info.owner) {
        pack(buf, summon_info.owner->id, idx);
    } else if (numen_info.owner) {
        pack(buf, numen_info.owner->id, idx);
		TRACE_LOG("%u %u %u", id, role_type, numen_info.owner->id);
    } else {
        pack(buf, static_cast<uint32_t>(0), idx);
    }
	pack(buf, id, idx);
	pack(buf, role_tm, idx);
	pack(buf, role_type, idx);
    if (is_valid_uid(id)) {
        pack(buf, power_user, idx);
    } else {
        pack(buf, static_cast<uint32_t>(0), idx);
    }
	pack(buf, show_state, idx);
	pack(buf, vip, idx);
	pack(buf, vip ? vip_level : 0, idx);
	pack(buf, team, idx);
	pack(buf, nick, max_nick_size, idx);
	pack(buf, static_cast<uint32_t>(lv), idx);
	pack(buf, max_hp(), idx);
	pack(buf, hp, idx);
    if (is_summon_mon(role_type) ){
        pack(buf, static_cast<uint32_t>(get_max_fight_value()), idx);
        pack(buf, static_cast<uint32_t>(summon_info.fight_value), idx);
    } else {
        pack(buf, max_mp(), idx);
        pack(buf, mp, idx);
    }
    pack(buf, static_cast<uint32_t>(exp), idx);
	pack(buf, honor, idx);
	pack(buf, pos().x(), idx);
	pack(buf, pos().y(), idx);
	pack(buf, dir, idx);	
	pack(buf, get_speed(), idx);
	//pack clothes
	pack(buf, static_cast<uint32_t>(clothes_info.clothes_cnt), idx);
	TRACE_LOG("cloth cnt:[%u %u]",id, clothes_info.clothes_cnt);
	for (uint32_t i = 0; i != clothes_info.clothes_cnt; ++i) {
		pack(buf, clothes_info.clothes[i].clothes_id, idx);
		pack(buf, clothes_info.clothes[i].gettime, idx);
		pack(buf, clothes_info.clothes[i].timelag, idx);
		pack(buf, static_cast<uint32_t>(clothes_info.clothes[i].lv), idx);
		//TRACE_LOG("cloth info:[%u %u]",id, clothes_info.clothes[i].clothes_id);
	}

	//pack player's summon
	uint16_t summon_cnt = 0;
    /*
	if (my_summon) {
		summon_cnt = 1;
		pack(buf, summon_cnt, idx);
		pack(buf, my_summon->summon_info.mon_tm, idx);
		pack(buf, my_summon->id, idx);
		pack(buf, my_summon->role_type, idx);
		pack(buf, my_summon->nick, max_nick_size, idx);
		pack(buf, my_summon->lv, idx);
		pack(buf, my_summon->summon_info.fight_value, idx);
		pack(buf, my_summon->exp, idx);
		pack(buf, my_summon->summon_info.anger_value, idx);
	} else {
		pack(buf, summon_cnt, idx);
	}
*/
    pack(buf, summon_cnt, idx);
	return idx;
}

int Player::pack_status_info(uint8_t* buf)
{
	int idx = 0;
	uint32_t count = m_bufflist.size();
	pack(buf, id, idx);
	pack(buf, count, idx);
	std::list<buff*>::iterator pItr = m_bufflist.begin();
	for(; pItr != m_bufflist.end(); ++pItr)
	{
		buff* pBuff = *pItr;
		uint16_t buff_id = pBuff->get_buff_id();
		uint8_t  buff_icon = pBuff->get_buff_icon();
		pack(buf, buff_id, idx);
		pack(buf, buff_icon, idx);
	}
	
	count = m_auralist.size();
	pack(buf, count, idx);
	std::list<aura*>::iterator pItr2 = m_auralist.begin();
	for(; pItr2 != m_auralist.end(); ++pItr2)
	{
		aura* pAura = *pItr2;
		uint16_t aura_id = pAura->get_aura_id();
		uint8_t  aura_icon = pAura->get_aura_icon();
		pack(buf, aura_id, idx);
		pack(buf, aura_icon, idx);
	}
	return idx;
}

int
Player::pack_player_attr_chg(uint8_t* buf)
{
	int i = 0;
	pack(buf, lv, i);
	pack(buf, exp, i);
	pack(buf, skill_point, i);
	pack(buf, coins, i);
	TRACE_LOG("%u %u", id, coins);
	return i;	
}

int
Player::pack_basic_info(void* buf)
{
	int idx = 0;

    if (summon_info.owner) {
        pack(buf, summon_info.owner->id, idx);
    } else if (numen_info.owner) {
        pack(buf, numen_info.owner->id, idx);
		TRACE_LOG("%u %u %u", id, role_type, numen_info.owner->id);
    } else {
    	pack(buf, static_cast<uint32_t>(0), idx);
    }
	pack(buf, id, idx);
	pack(buf, role_tm, idx);
	pack(buf, role_type, idx);
    if (is_valid_uid(id)) {
        pack(buf, power_user, idx);
    } else {
        pack(buf, static_cast<uint32_t>(0), idx);
    }
	pack(buf, show_state, idx);
	pack(buf, vip, idx);
	pack(buf, vip ? vip_level : 0, idx);
	pack(buf, team, idx);
	pack(buf, nick, max_nick_size, idx);
	pack(buf, static_cast<uint32_t>(lv), idx);
	pack(buf, max_hp(), idx);
	pack(buf, hp, idx);
    if (is_summon_mon(role_type)) {
        pack(buf, static_cast<uint32_t>(get_max_fight_value()), idx);
        pack(buf, static_cast<uint32_t>(summon_info.fight_value), idx);
    } else {
        pack(buf, max_mp(), idx);
        pack(buf, mp, idx);
    }
	pack(buf, static_cast<uint32_t>(exp), idx);
	if(is_summon_mon(role_type))
	{
		pack(buf, static_cast<uint32_t>(summon_info.anger_value), idx);
	}
	else
	{
		pack(buf, honor, idx);
	}
	pack(buf, pos().x(), idx);
	pack(buf, pos().y(), idx);
	pack(buf, dir, idx);
	pack(buf, get_speed(), idx);
	TRACE_LOG("viphpmp:%u %u %u %u %u %u %u", vip, vip_level, hp, max_hp(), mp, max_mp(), exp);
	//pack clothes
	pack(buf, static_cast<uint32_t>(clothes_info.clothes_cnt), idx);
	TRACE_LOG("cloth cnt:[%u %u]",id, clothes_info.clothes_cnt);
	for (uint32_t i = 0; i != clothes_info.clothes_cnt; ++i) {
		pack(buf, clothes_info.clothes[i].clothes_id, idx);
		pack(buf, clothes_info.clothes[i].gettime, idx);
		pack(buf, clothes_info.clothes[i].timelag, idx);
		pack(buf, static_cast<uint32_t>(clothes_info.clothes[i].lv), idx);
		TRACE_LOG("cloth info:[%u] [%u %u %u %u]",id, clothes_info.clothes[i].clothes_id, 
            clothes_info.clothes[i].gettime,clothes_info.clothes[i].timelag,clothes_info.clothes[i].lv);
	}
/*
	//pack player's summon
	uint16_t summon_cnt = 0;
	if (my_summon) {
		summon_cnt = 1;
		pack(buf, summon_cnt, idx);
		pack(buf, my_summon->summon_info.mon_tm, idx);
		pack(buf, my_summon->id, idx);
		pack(buf, my_summon->role_type, idx);
		pack(buf, my_summon->nick, max_nick_size, idx);
		pack(buf, my_summon->lv, idx);
		pack(buf, my_summon->summon_info.fight_value, idx);
		pack(buf, my_summon->exp, idx);
		pack(buf, my_summon->summon_info.anger_value, idx);
	} else {
		pack(buf, summon_cnt, idx);
	}
*/
    pack(buf, static_cast<uint16_t>(0), idx);
	return idx;
}

int
Player::pack_h_basic_info(void* buf)
{
	int idx = 0;

	if (summon_info.owner) {
		pack_h(buf, summon_info.owner->id, idx);
	} else if (numen_info.owner) {
        pack(buf, numen_info.owner->id, idx);
		TRACE_LOG("%u %u %u", id, role_type, numen_info.owner->id);
    } else {
		pack_h(buf, static_cast<uint32_t>(0), idx);
	}

	pack_h(buf, id, idx);
	pack_h(buf, role_tm, idx);
	pack_h(buf, role_type, idx);
    if (is_valid_uid(id)) {
        pack_h(buf, power_user, idx);
    } else {
        pack_h(buf, static_cast<uint32_t>(0), idx);
    }
	pack_h(buf, show_state, idx);
	pack_h(buf, vip, idx);
	pack_h(buf, vip ? vip_level : 0, idx);
	pack_h(buf, team, idx);
	pack(buf, nick, max_nick_size, idx);
	pack_h(buf, static_cast<uint32_t>(lv), idx);
	pack_h(buf, max_hp(), idx);
	pack_h(buf, hp, idx);
    if (is_summon_mon(role_type)) {
        pack(buf, static_cast<uint32_t>(get_max_fight_value()), idx);
        pack(buf, static_cast<uint32_t>(summon_info.fight_value), idx);
    } else {
        pack_h(buf, max_mp(), idx);
        pack_h(buf, mp, idx);
    }
	pack_h(buf, static_cast<uint32_t>(exp), idx);
	pack_h(buf, honor, idx);
	pack_h(buf, pos().x(), idx);
	pack_h(buf, pos().y(), idx);
	pack_h(buf, dir, idx);
	pack_h(buf, get_speed(), idx);
	//pack clothes
	pack_h(buf, static_cast<uint32_t>(clothes_info.clothes_cnt), idx);
	TRACE_LOG("cloth cnt:[%u %u]",id, clothes_info.clothes_cnt);
	for (uint32_t i = 0; i != clothes_info.clothes_cnt; ++i) {
		pack_h(buf, clothes_info.clothes[i].clothes_id, idx);
		pack_h(buf, clothes_info.clothes[i].gettime, idx);
		pack_h(buf, clothes_info.clothes[i].timelag, idx);
		pack_h(buf, static_cast<uint32_t>(clothes_info.clothes[i].lv), idx);
		//TRACE_LOG("cloth info:[%u %u]",id, clothes_info.clothes[i].clothes_id);
	}

	//pack player's summon
	/*
	uint16_t summon_cnt = 0;
	if (my_summon) {
		summon_cnt = 1;
		pack_h(buf, summon_cnt, idx);
		pack_h(buf, my_summon->summon_info.mon_tm, idx);
		pack_h(buf, my_summon->id, idx);
		pack_h(buf, my_summon->role_type, idx);
		pack(buf, my_summon->nick, max_nick_size, idx);
		pack_h(buf, my_summon->lv, idx);
		pack_h(buf, my_summon->summon_info.fight_value, idx);
		pack_h(buf, my_summon->exp, idx);
		pack_h(buf, my_summon->summon_info.anger_value, idx);
	} else {
		pack_h(buf, summon_cnt, idx);
	}*/
	pack_h(buf, 0, idx);
	return idx;
}

void
Player::noti_chg_attr_to_player()
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	idx += pack_player_attr_chg(pkgbuf + idx);

	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), id, cli_proto_player_attr_chg, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_player(this, pkgbuf, idx, 0);
}

void
Player::restart_init(const restart_player_attr_t* p_attr)
{
	//about player attr
	set_dead(false);
	hp	= max_hp();
	mp	= max_mp();
	dir	= dir_right;
    uint32_t btl_interval = 0;
    if (score.start_tm != 0) {
        btl_interval = btl->is_battle_tower() ? 0 : get_now_tv()->tv_sec - score.start_tm;
    }
	score.init();
	
	//about battle
	cur_map		= 0;
	if(my_summon)
	{
		my_summon->cur_map = NULL;
		my_summon->summon_info.fight_value = 100;
		my_summon->summon_info.anger_value = 0;
	}
	btl_status	= 0;
	delete btl;
	btl         = 0;


	//about battle skill
	memset(&stuck_end_tm, 0x00, sizeof(stuck_end_tm));
	memset(&flying_end_tm, 0x00, sizeof(flying_end_tm));
	invincible  = false;
	p_cur_skill = 0;TRACE_LOG("skillupdate in start_stage4 ");

	del_objlist();

	cur_item_picking = 0;

	//buff skills
	for (BufSkillMap::iterator it = buf_skill_map.begin(); it != buf_skill_map.end(); ++it) {
		if (!(it->second.db_buff_flg) ) {
			TRACE_LOG("!(it->second.db_buff_flg) %u %u", it->second.buf_type,it->second.db_buff_flg);
			continue;
		}
		uint32_t usedtime = 0;

		if (! (is_in_not_use_double_tm_time(this))) {
			usedtime = get_now_tv()->tv_sec - it->second.start_tm;
		}
		if (it->second.buf_type != 1400) {
			uint32_t lefttime = it->second.buf_duration > usedtime ? 
				it->second.buf_duration - usedtime : 0;
			it->second.buf_duration = lefttime;
			it->second.start_tm = get_now_tv()->tv_sec;
		}
	}

	
	//restart player attr
	if (!is_in_not_use_double_tm_time(this)) {
	    if (btl_interval < dexp_tm) {
	        dexp_tm = dexp_tm - btl_interval;
	    } else {
	        dexp_tm = 0;
	    }
	}
	if (g_special_double_time_mrg.is_in_double_time()) {
		in_specal_double_time = true;
	} else {
		in_specal_double_time = false;
	}

	if (g_special_double_time_mrg.is_in_double_time_2()) {
		in_specal_double_time_2 = true;
	} else {
		in_specal_double_time_2 = false;
	}
	
	if (g_special_double_time_mrg.is_in_double_time_witch_can_still_use_double_tm_item()) {
		in_specal_double_time_witch_can_still_use_double_tm_item = true;
	} else {
		in_specal_double_time_witch_can_still_use_double_tm_item = false;
	}

	if(my_summon)
	{
		if (g_special_double_time_mrg.is_in_double_time()) {
			my_summon->in_specal_double_time = true;
		} else {
			my_summon->in_specal_double_time = false;
		}

		if (g_special_double_time_mrg.is_in_double_time_2()) {
			my_summon->in_specal_double_time_2 = true;
		} else {
			my_summon->in_specal_double_time_2 = false;
		}
	}
	half_exp = p_attr->half_exp;
	
	//init task info
	task_item_map.clear();
	stage_task_list.clear();
	TRACE_LOG("task item num[%u %u], double exp time=[%u] interval=[%u]",
        id, p_attr->task_items_num, dexp_tm, btl_interval);
	for ( uint32_t i = 0; i < p_attr->task_items_num; i++ ) {
		TRACE_LOG("task item[%u %u %u %u]",p_attr->task_items[i].monster_id, 
				p_attr->task_items[i].item_id, p_attr->task_items[i].drop_odds, p_attr->task_items[i].rest_cnt);
		if (p_attr->task_items[i].monster_id) {
			task_item_map.insert(TaskItemMap::value_type(p_attr->task_items[i].monster_id, p_attr->task_items[i]));
		} else {
			stage_task_list.push_back(p_attr->task_items[i]);
		}
	}

	//about box
	can_open_box = false;
	normal_box_opened = false;
	vip_box_opened = false;
	cur_tower_layer = 0;
	//init_player_passive_skill(false);
	//remove timer
	ev_mgr.remove_events(*this);
}

uint32_t Player::get_summon_resuscitate_time()
{
	if(!is_valid_uid(id))return 0;
	if(!my_summon)return 0;
	if( is_vip_player())
	{
		return summon_info.renascence_time / 2;
	}
	return summon_info.renascence_time;
}

int Player::revive_pet()
{
	if(!is_valid_uid(id))return 0;
	if(!my_summon)return 0;
	my_summon->set_revive();
	my_summon->summon_info.active_flag  =true;
	my_summon->init_player_passive_skill();
	return 0;
}


void
Player::summon_init()
{
	if (is_valid_uid(id)) {
		return;
	}

	calc_summon_attr();

	//about battle skill
	memset(&stuck_end_tm, 0x00, sizeof(stuck_end_tm));
	memset(&flying_end_tm, 0x00, sizeof(flying_end_tm));
	p_cur_skill = 0;TRACE_LOG("skillupdate in start_stage5");

	del_objlist();

	btl = summon_info.owner->btl;
	cur_map = summon_info.owner->cur_map;
	
	//remove timer
	ev_mgr.remove_events(*this);
	set_revive();
	TRACE_LOG("summon init![%u %u]", summon_info.owner->id, id);
}

int
Player::pack_player_attr(void* buf)
{
	int idx = 0;
	pack(buf, role_type, idx);
	
	/*! player's level */
	pack(buf, static_cast<uint32_t>(lv), idx);

	/*! strength */
	pack(buf, static_cast<uint32_t>(strength), idx);
	
	/*! agility */
	pack(buf, static_cast<uint32_t>(agility()), idx);
	
	/*! body quality */
	pack(buf, static_cast<uint32_t>(body_quality()), idx);
	
	/*! stamina */
	pack(buf, static_cast<uint32_t>(stamina), idx);
	
	/*! player's current hp */
	//pack(buf, static_cast<uint32_t>(hp), idx);
	
	/*! player's current max_hp */
	pack(buf, static_cast<uint32_t>(max_hp()), idx);
	
	/*! player's clothes addhp */
	pack(buf, static_cast<uint32_t>(clothes_addhp), idx);
	
	/*! player's current mp */
	//pack(buf, static_cast<uint32_t>(mp), idx);
	
	/*! player's max mp */
	pack(buf, static_cast<uint32_t>(max_mp()), idx);
	
	/*! player's clothes addmp */
	pack(buf, static_cast<uint32_t>(clothes_addmp), idx);
	
	/*! player's pure attack power */
	pack(buf, static_cast<uint32_t>(atk), idx);
	
	/*! weapon's attack power */
	pack(buf, static_cast<uint32_t>(weapon_atk[0]), idx);

	/*! weapon's attack power */
	pack(buf, static_cast<uint32_t>(weapon_atk[1]), idx);
	
	/*! critical attack rate */
	pack(buf, static_cast<uint32_t>(crit_rate), idx);
	
	/*! defence rate */
	pack(buf, static_cast<uint32_t>(def_rate * 1000), idx);
	
	/*! hit rate */
	pack(buf, static_cast<uint32_t>(hit_rate * 1000), idx);
	
	/*! dodge rate */
	pack(buf, static_cast<uint32_t>(dodge_rate * 1000), idx);

	return idx;
}

void Player::gain_coin(uint32_t gold)
{
	this->coins += gold;
  	need_sav_ = true;
	noti_chg_attr_to_player();
}

void Player::reduce_exploit(uint32_t reduce_num)
{
	if (exploit > reduce_num) {
		exploit -= reduce_num;
	} else {
		exploit = 0;
	}
}

void Player::add_exploit(uint32_t add_num)
{
	exploit += add_num;
}


//-----------------------------------------------------------
// Private Methods
void
Player::add_exp(int gain_exp)
{
	/*if ( 1 == half_exp) {
		gain_exp = gain_exp / 2;
		TRACE_LOG("deduct experience\t[uid=%u] [gain_exp=%d]", id, gain_exp);
	}*/
	bool is_summon = is_summon_mon(role_type);
	if (lv >= max_exp_lv && !is_summon) {
		return;
	}

	float dexp_per = 1;
    if ( (is_valid_uid(id) && dexp_tm > 0) || 
		 (is_summon && summon_info.owner && summon_info.owner->dexp_tm > 0)) {
		dexp_per = 2;
    }

	float total_per = 1;

	if (exp_factor_trim) {
		float tmp_trim = (exp_factor_trim == 0 ? 1 : exp_factor_trim) - 1.0;
		total_per = dexp_per +  tmp_trim;
	} else {
		if (is_summon_mon(role_type)) {
			Player* owner = summon_info.owner;
			if (owner) {
				float tmp_trim = (owner->summon_mon_exp_factor_trim < 1 ? 1 : owner->summon_mon_exp_factor_trim) - 1.0;
				total_per = dexp_per + tmp_trim;
			}
		} else {
			float tmp_trim = (player_only_exp_factor_trim < 1 ? 1 : player_only_exp_factor_trim) - 1.0;
			total_per = dexp_per + tmp_trim;
		}
	}

	float time_limit_per = 1.0;
	
	if (is_summon_mon(role_type)) {
		Player* owner = summon_info.owner;
		if (owner) {
			if (owner->half_exp) {
				time_limit_per = 0.5;
			}
		}
	} else {
		if (half_exp) {
			time_limit_per = 0.5;
		}
	}

	if (in_specal_double_time_witch_can_still_use_double_tm_item) {
		total_per *= 2;
	} else if (is_summon_mon(role_type) && summon_info.owner && summon_info.owner->in_specal_double_time_witch_can_still_use_double_tm_item) {
		total_per *= 2;
	}

	if (in_specal_double_time_2) {
		total_per = 4 * time_limit_per;
	} else {
		if (in_specal_double_time) {
			total_per = 3 * time_limit_per;
		} else if (is_summon_mon(role_type) && summon_info.owner && summon_info.owner->in_specal_double_time) {
			total_per = 3 * time_limit_per;
		} else {
			total_per *= time_limit_per;
		}
	}
	
	//protect
	total_per = total_per > 4 ? 4 : total_per;
	total_per = total_per == 0 ? 1 : total_per;
	gain_exp = int((float)gain_exp * total_per);
	gain_exp = gain_exp < 1 ? 1 : gain_exp;

	if (is_summon && if_can_get_exp())
	{
		Player * owner = summon_info.owner;
		if(owner && owner->is_vip_player())
		{
			owner->allocator_exp += (int)(0.3 * gain_exp);
		}
	}

	KDEBUG_LOG(id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=mon_kill]", 
						3, 2, gain_exp);
	
	if ( if_can_get_exp() && calc_lv_add_exp(gain_exp, exp, lv, get_limit_lv(), role_type, id, is_summon) ) {
		on_lv_up();
	}
}

void Player::on_lv_up()
{
	//save to db
	if (is_valid_uid(id)) {
		skill_point++;
		calc_player_attr();
		db_set_player_base_info(this);
        do_stat_log_role_exp(id, role_type, exp);
	}
	else if (is_summon_mon(role_type)) {
		summon_info.mon_lv ++;
		calc_player_attr();
		db_set_summon_basic_attr(this, summon_info.owner);
	}

	hp = max_hp();
	mp = max_mp();

	//noti to map
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, id, idx);
	pack_h(pkgbuf, static_cast<uint32_t>(lv), idx);
	pack_h(pkgbuf, static_cast<uint32_t>(hp), idx);

	if (is_summon_mon(role_type)) {
		pack_h(pkgbuf, static_cast<uint32_t>(get_max_fight_value()), idx);
	} else {
		pack_h(pkgbuf, static_cast<uint32_t>(mp), idx);
	}
	init_btl_proto_head(pkgbuf, btl_noti_player_lvup, idx);
	cur_map->send_to_map(pkgbuf, idx);

	// ambassador task, set achieve num of player's child num
	if (lv == 5 && parentid) {
		db_add_amb_achieve_num(this, parentid);
		db_set_user_flag(this);
		do_stat_log_achieve_child_num(this);
	}
}

void 
Player::summon_attr_chg()
{
	if (!is_summon_mon(role_type)) {
		return;
	}

	uint16_t min_value = min_fight_value;
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, id, idx);
	pack(pkgbuf, lv, idx);
	pack(pkgbuf, exp, idx);
	pack(pkgbuf, summon_info.fight_value, idx);
	pack(pkgbuf, min_value, idx);

	Player* p = summon_info.owner;
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), p->id, cli_proto_summon_attr_chg, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_player(p, pkgbuf, idx, 0);

	need_sav_ = true;
}


void Player::send_batlle_section()
{
	if(btl && cur_map)
	{
		int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
		pack(pkgbuf, static_cast<uint32_t>(btl->stage()->id), idx);
		pack(pkgbuf, cur_map->id, idx);
		pack(pkgbuf, btl->battle_section_data, idx);
		pack(pkgbuf, 0, idx);//flag2
		pack(pkgbuf, 0, idx);//flag3
		init_cli_proto_head(pkgbuf + sizeof(btl_proto_t),  cli_proto_btl_section_change,  idx - sizeof(btl_proto_t));
		init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
		send_to_player(this, pkgbuf, idx, 0);
	}
}

void Player::call_summon_after_dead()
{
	if( dead_call_summon)
	{
		call_map_summon(dead_call_summon, pos().x(), pos().y(), false);
	}
}

void Player::call_summon_after_suicide()
{
	if( suicide_call_summon )
	{
		call_map_summon(suicide_call_summon, pos().x(), pos().y(), false);
	}	
}

void Player::suicide()
{
	int damage = max_hp();
	
	chg_hp( 0 - damage);
	//notify_damage_by_buff(id, 0, 0);
	on_dead(NULL, false);
	notify_damage_by_buff(id, 0, is_valid_uid(id) ? damage : 0);
}

bool need_notify_dead(Player * killer, Player * dead_monster)
{
	if (killer 
		   && !is_valid_uid(dead_monster->id)
		   && !is_summon_mon(dead_monster->role_type)
		   && (is_valid_uid(killer->id) || is_summon_mon(killer->role_type))) {

		return true;
	}

	return false;

}
void notify_monster_dead_to_online(Player * killer, uint32_t mon_type)
{
	if (is_summon_mon(killer->role_type)) {
		killer = killer->summon_info.owner;
	}

	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, mon_type, idx);
	pack_h(pkgbuf, killer->btl->difficulty(), idx);
	init_btl_proto_head(pkgbuf, btl_kill_monster, idx);
	send_to_player(killer, pkgbuf, idx, 0);
}
	

void
Player::on_dead(Player* killer, bool can_get_exp)
{
	if(is_summon_mon(role_type))
	{
		Player* owner = summon_info.owner;
		if(owner)
		{
			uint32_t revive_delay_time = owner->get_summon_resuscitate_time();
			TRACE_LOG("revive %u %u %u", id, owner->id, revive_delay_time);
			revive_delay_time = revive_delay_time == 0 ? 1 : revive_delay_time;
			timeval tv = *get_now_tv();
			tv.tv_sec += revive_delay_time;
			ev_mgr.add_event(*owner, &Player::revive_pet, tv);
		}	
	}
	
	
	del_infect_aura_when_dead(this);
	btl->del_map_summon_by_owner_id(id);
	if( can_get_exp)
	{
		call_summon_after_dead();
	}
	else
	{
		call_summon_after_suicide();
	}
	// ---special player dead will cause buff ----//
	
	switch(role_type)
	{
		case 11302://虫卵
		{
			if( i_ai->common_flag_  == 1)//白色
			{
				call_monster(11303, pos().x(), pos().y());
			}
			else if( i_ai->common_flag_  == 2)//绿色
			{
				call_monster(11304, pos().x(), pos().y(), 1);
			}
			else if( i_ai->common_flag_  == 3)//红色
			{
				call_monster(11303, pos().x(), pos().y());
			}
			else if( i_ai->common_flag_  == 4)//红色结束
			{
				call_monster(11303, pos().x(), pos().y());
				call_monster(11303, pos().x(), pos().y());
			}			
		}
		break;
		
		case 19124://干燥石碑
		{
			if(btl){
				btl->add_buff_to_all_players(8, 0);
			}
		}
		break;

		case 19125://热力石碑
		{
			if(btl){
				btl->add_buff_to_all_players(9, 0);
			}
		}

		case 19126://治愈珊瑚
		{
			if( killer)
			{
				add_player_aura(killer, 1, killer->id);
			}
		}
		break;

		case 11314://好人兔(打地鼠关卡)
		{
			if( btl->btl_logic_data.data && killer)
			{
				uint32_t* para = (uint32_t*)btl->btl_logic_data.data;
				para[0] += 1;
			}
		}
		break;

		case 11315://僵尸(打地鼠关卡)
		{
			if( btl->btl_logic_data.data && killer)
			{
				uint32_t* para = (uint32_t*)btl->btl_logic_data.data;
				if( para[0] >= 3){
					para[0] -= 3;
				}else{
					para[0] = 0;
				}
			}		
		}
		break;

		case 13098:
		case 13099:
		case 13100:
		case 13101:
			call_monster(39009, 316, 426);
			call_monster(39009, 466, 426);
			call_monster(39009, 616, 426);
			call_monster(39009, 766, 426);
		break;

		case 13102:
		case 13103:
		case 13104:
		case 13105:
			call_monster(39010, 316, 426);
			call_monster(39010, 466, 426);
			call_monster(39010, 616, 426);
			call_monster(39010, 766, 426);
			break;
		case 19164:
			{
				Player * boss = cur_map->get_boss();
				if (boss && killer) { //被玩家打死
					boss->atk = (int) (boss->atk * 1.2) ;
				} else if (boss && !boss->is_dead()) {
					boss->hp = (int)(boss->max_hp() * 0.7); 	
					boss->call_map_summon(51, this->pos().x(), this->pos().y(), false);
					boss->noti_hpmp_to_btl();
				}
			}
			break;
		case 19165:
			call_monster(13142, 638, 462);
			call_monster(13143, 403, 463);
			break;
		case 13140:
			cur_map->set_monster_dead(13141);
			break;
		case 13156: // nine head dradon
			{
				if ((uint32_t)ranged_random(0, 100) < 25) {
					btl->drop_item_to_team(cur_map, 1410030, pos().x() + 500, pos().y());
				}
				if ((uint32_t)ranged_random(0, 100) < 1) {
					btl->drop_item_to_team(cur_map, 1500579, pos().x() + 500, pos().y());
				}
				if ((uint32_t)ranged_random(0, 100) < 40) {
					btl->drop_item_to_team(cur_map, 1500580, pos().x() + 500, pos().y());
				}
				if ((uint32_t)ranged_random(0, 100) < 50) {
					btl->drop_item_to_team(cur_map, 1500902, pos().x() + 500, pos().y());
				}
				if ((uint32_t)ranged_random(0, 100) < 1) {
					btl->drop_item_to_team(cur_map, 1500564, pos().x() + 500, pos().y());
				}
			}
			break;
		case 11517: // little dradon
			{
				Player* boss = cur_map->get_one_monster(13156);
				if (boss) {
					player_hpmp_chg_info_t info = { 0 };
					info.hp_chg -= ((boss->max_hp() + 6000)/ 4);
					boss->hpmp_chg(info);
					if (boss->is_dead()) {
						boss->on_dead(killer);
						//btl->on_player_dead(boss);
					}
				}
				call_monster(11517, pos().x(), pos().y());
			}
			break;
		default:
			break;
	}	
	
	//*怪物死亡触发机关状态变更*/
	if (role_type >= 19136 && role_type <= 19139) { //海火封印
		if (btl->stage()->id == 48) { 
			uint32_t id = 1004803;
			id = get_stage_map_id(id);
			map_t * mid_map = btl->get_map(id);
			if (mid_map) {
				uint32_t opened = 1;
				uint32_t mechan = mid_map->get_mechanism_id_by_type(37501 + role_type - 19136);
				if (mechan) {
					mid_map->mechansim_status_changed(mechan, opened);
					process_battle_extra_logic(btl); //机关触发逻辑处理
				}
			}
		}
	}

	if (role_type >= 13091 && role_type <= 13094 && killer) {
		Player * p = 0;
		if (is_summon_mon(killer->role_type)) {
			p = killer->summon_info.owner;
		} else {
			p = killer;
		}
        //ERROR_LOG("monitor stage [%u]", btl->stage()->id);
		p->add_contest_exp(btl->stage()->id, role_type, false); 
	}

	if (role_type == btl->common_flag3_ && btl->stage()->id == 964) {
		if (killer && is_valid_uid(killer->id)) {
			btl->common_flag1_ = killer->id; 
		}
	}

	
	/////////////////////////////////////////////


	//monster dead, drop item
	if (!is_valid_uid(id) && !is_summon_mon(role_type)) {
		if (!killer) {
			killer = cur_map->get_one_player();
		}
		//伏魔塔 龙珠掉落 任务掉落
		if (btl->is_battle_tower()) {
            if (killer && (this->role_type == 13050 || this->role_type == 13034)) {
                btl->monster_private_drop(this, killer);
            }
            if (killer && this->role_type == 13120) {
                monster_drop_item(this, killer);
            }
		}

		if( killer && !(btl->is_battle_tower())) {
			Player* p = killer;
			if ( is_summon_mon(killer->role_type) && killer->summon_info.owner) {
				p = killer->summon_info.owner;
			}
			monster_drop_item(this, p);
			btl->monster_private_drop(this, p);
			//15%概率掉落3色琥珀，活动结束后该代码注释掉
			//monster_drop_amber_to_player(p,  pos().x(), pos().y());
		}
	} else {
		//summon monster stop
		if (my_summon) {
			my_summon->summon_info.active_flag = false;
			my_summon->i_ai->change_state(WAIT_STATE);
		}
        
		//monitor
        if (killer && !is_valid_uid(killer->id)) {
            //killed by boss
            if ( is_boss_type(killer->role_type) ) {
                do_stat_item_log(stat_log_killed_boss, btl->stage()->id, role_type, 1);
            }
			if (btl->is_battle_tower()) {
				do_stat_item_log(stat_log_fumo_dead,  cur_tower_layer, 0, 1);
			}
        }

		if (!btl->watchers_.empty()) {
		//	this->noti_hpmp_to_btl();
		}
    } 

	if( killer &&  !is_summon_mon(this->role_type))
	{
		uint32_t stg_id = btl->stage()->id;
        //运营关卡927不能获得经验
		if ( stg_id != 927 && stg_id != 929  && stg_id != 931 && 
			 stg_id != 933 && stg_id != 939 && stg_id != 718) { 
			btl->on_player_killed(killer, this, can_get_exp);	
		}

		if (stg_id != 718) {
			btl->drop_coins_on_monster_dead(this, killer);
		}
	}
	btl->on_player_dead(this);
	if (need_notify_dead(killer, this)) {
		//send_monster_be_killed_in
		notify_monster_dead_to_online(killer, this->role_type);
	}	
}

bool
Player::is_in_no_exp_stage()
{
	switch (btl->stage()->id){
		case 910:
		case 929:
		case 914:
		case 927: 
		case 921:
			return true;
			break;
		default:
			return false;
			break;
	}
	return false;
}

void Player::get_exp_from_victim(Player* victim)
{
	uint32_t std_exp = 50;
	uint32_t mon_type = victim->mon_info->type;
	uint32_t type_para = 1;
	if (mon_type == 0) 
	{
		mon_type = 1;
	}
	if (mon_type == 1) 
	{
		type_para = 16;
	} 
	else if (mon_type == 4 || mon_type == 5) 
	{
		type_para = 2;
	}
	if (victim->lv == 0) 
	{
		victim->lv = 1;
	}
	uint32_t victim_exp = (std_exp + ((victim->lv - 1) * 3)) * (btl->difficulty() + 3) * type_para;
	if (victim->role_type >= 19001 && victim->role_type < 30000) 
	{
		victim_exp = 0;
	}
	victim_exp = victim_exp > 1000000 ? 1000 : victim_exp;
	
	uint32_t start_exp = exp;
	uint32_t base_mon_exp = victim_exp;//victim->exp;//
	uint32_t per_exp = base_mon_exp;
	if (btl->is_team_btl() && btl->get_players_num() > 1) 
	{
		for (uint32_t i = 1; i < btl->get_players_num(); i++) 
		{
			base_mon_exp *= 2.2;
		}
		if (is_summon_mon(role_type)) 
		{
			Player* owner = summon_info.owner;
			if (owner) 
			{
				per_exp = base_mon_exp / btl->get_players_num();
			}
		} 
		else 
		{
			per_exp = base_mon_exp / btl->get_players_num();
		}
	}
	if (!(btl->is_battle_tower()) && !is_in_no_exp_stage()) 
	{
		int lv_diff = victim->lv - lv;
		// add exp and lv up
		if (lv_diff >= 5) 
		{
			if (is_valid_uid(id) && btl->is_team_btl() && btl->get_players_num() > 1) 
			{
				uint16_t mon_type = victim->mon_info->type;
				uint32_t max_per_exp = 0;
				uint32_t base_exp = victim->lv <= 10 ? (72 + 8 * lv) : (112 + 4 * lv);
				if (mon_type == 1) {
					max_per_exp = base_exp * 8 * btl->difficulty() / 2;
				} else if (mon_type == 2) {
					max_per_exp = base_exp * 2 * btl->difficulty() / 2;
				} else {
					max_per_exp = base_exp * btl->difficulty() / 2;
				}
				per_exp = max_per_exp;
			} 
			else 
			{
				per_exp = per_exp * 1.5;
			}
			add_exp(per_exp);
		
		}else if (lv_diff < 5 && lv_diff >= 0) {
			add_exp(per_exp * (1 + 0.1 * lv_diff));
		} else if (lv_diff <= -1 && lv_diff >= -2) {
			add_exp(per_exp * (0.9 + 0.1 * lv_diff));
		} else if (lv_diff <= -3 && lv_diff >= -4) {
			add_exp(per_exp * (0.8 + 0.1 * lv_diff));
		} else if (lv_diff == -5) {
			add_exp(per_exp * 0.25);
		} else if (lv_diff == -6) {
			add_exp(per_exp * 0.2);
		} else if (lv_diff == -7) {
			add_exp(per_exp * 0.1);
		} else if (lv_diff == -8) {
			add_exp(per_exp * 0.05);
		} else if (lv_diff == -9) {
			add_exp(per_exp * 0.02);
		} else {
			add_exp(1);
			//return;
		}
	}
	
	//summon get exp
	if (is_summon_mon(role_type)) {
		summon_attr_chg();
		return;
	}
	
	// add skill point
	//int tmp_rate = 0;
	//int lv_p_diff_m = lv - victim->lv;
	/*
	if (lv_p_diff_m > -2 && lv_p_diff_m < 8) {
		tmp_rate = 80 - lv_p_diff_m * 10;
	} else if(lv_p_diff_m <= -2) {
		tmp_rate = 100;
	} else if(lv_p_diff_m >= 8) {
		tmp_rate = 0;
	}*/

	//not get skill points any more
	/*
	static int skill_award_info[9] = {100, 100, 90, 70, 50, 40, 30, 10, 0};
	if (lv_p_diff_m <= 1) {
		tmp_rate = 100;
	} else if (lv_p_diff_m > 8) {
		tmp_rate = 0;
	} else {
		tmp_rate = skill_award_info[lv_p_diff_m];
	}
	
	if (rand() % 100 < tmp_rate) {
		skill_point += victim->skill_point;
		TRACE_LOG("rate=%d	add skill:%u ", tmp_rate, victim->skill_point);
	} else {
		TRACE_LOG("not rate=%d	add skill:%u ", tmp_rate, victim->skill_point);
	}*/
	need_sav_ = true;
	
	//stage_score
	score.get_exp += exp - start_exp;
	
	// notify the player the gaining of exp and sp and money
	noti_chg_attr_to_player();
	
	TRACE_LOG("battle info:[id=%u lv=%u exp=%u sp=%u]",id, lv, exp, skill_point);

}

void
Player::on_player_killed(Player* victim,  bool can_get_exp)
{
	if (!is_valid_uid(victim->id) && (is_valid_uid(id) || is_summon_mon(role_type))) {
        //killed monster
        if (victim->role_type > 11000 && victim->role_type < 19000) {
            uint32_t mid = victim->role_type - 11000;
            do_stat_item_log(stat_log_killed_monster, mid, ((role_type-1) * 3)+1 , 1);
        }
		KDEBUG_LOG(id, "MON KILL\t[mon=%u]", victim->role_type);
		if( can_get_exp)get_exp_from_victim(victim);
    }
}

inline void
Player::on_player_leave_map(Player* p)
{
	if (i_ai->target == p) {
		i_ai->change_state(WAIT_STATE);
	}
}

void
Player::add_fumo_points(uint32_t& get_points)
{
	if (fumo_points_end >= max_fumo_points_daily_got) {
		get_points = 0xffffffff;
	} else {
		uint32_t total_points = fumo_points_end + get_points;
		if (total_points > max_fumo_points_daily_got) {
			get_points = max_fumo_points_daily_got - fumo_points_end;
			fumo_points_end = max_fumo_points_daily_got;
		} else {
			fumo_points_end = total_points;
		}
	}

}

void 
Player::calc_stage_score(bool is_btl_over)
{
	uint32_t total_score = 0;
	uint32_t del_score = 0;
	float time_limit_per = 1.0;
	if (half_exp) {
		time_limit_per = 0.5;
	} 
	
	score.end_tm = get_now_tv()->tv_sec;
	
	uint32_t pass_tm = score.end_tm - score.start_tm;
	uint32_t all_tm = score.map_num * 120;
	del_score = (all_tm >= pass_tm) ? 0 : ((pass_tm - all_tm) / 30 + 1) * 200;
	uint32_t pass_tm_score = (del_score >= 2000) ? 0 : 2000 - del_score;

	del_score = (score.on_hit_cnt <= 10) ? 0 : ( (score.on_hit_cnt - 10) / 10 + 1) * 200;
	uint32_t on_hit_score = (del_score >= 2000) ? 0 : 2000 - del_score;

	del_score = (score.all_mon_cnt - score.kill_mon_cnt) * 150;
	uint32_t kill_mon_score = (del_score >= 2000) ? 0 : 2000 - del_score;

	del_score = score.deaded_cnt * 250;
	uint32_t deaded_score = (del_score >= 2000) ? 0 : 2000 - del_score;

	del_score = (score.all_drop_cnt - score.pick_drop_cnt) * 150;
	uint32_t pick_drop_score = (del_score >= 2000) ? 0 : 2000 - del_score;

	uint32_t pass_score = 0;
	uint32_t combo_hit_score = score.combo_hit_cnt * 50;
	combo_hit_score = combo_hit_score > 500 ? 500 : combo_hit_score;
	uint32_t back_hit_score = 0;
	pass_score = combo_hit_score + back_hit_score;

	uint32_t operator_score = 0;
	uint32_t sec_kill_score = score.sec_kill_cnt * 100;
	sec_kill_score = sec_kill_score > 700 ? 700 : sec_kill_score;
	uint32_t break_skill_score = score.break_skill_cnt * 50;
	break_skill_score = break_skill_score > 800 ? 800 : break_skill_score;
	operator_score = sec_kill_score + break_skill_score;

	total_score = pass_tm_score + on_hit_score + kill_mon_score + deaded_score + pick_drop_score +
		pass_score + operator_score;
	TRACE_LOG("score[%u %u %u %u %u %u %u %u]",pass_tm_score, on_hit_score, kill_mon_score, deaded_score, pick_drop_score,
		   pass_score, break_skill_score, total_score);

	uint32_t stage_grade = 0;
	uint32_t exp_rate = 0;
	uint32_t money_rate = 0;
	if (total_score >= 10200) {
		stage_grade = 1;
		exp_rate = 15;
		money_rate = 400;
	} else if (total_score < 10200 && total_score >= 9200) {
		stage_grade = 2;
		exp_rate = 10;
		money_rate = 300;
	} else if (total_score < 9200 && total_score >= 8000) {
		stage_grade = 3;
		exp_rate = 8;
		money_rate = 250;
	} else if (total_score < 8000 && total_score >= 6000) {
		stage_grade = 4;
		exp_rate = 6;
		money_rate = 200;
	} else if (total_score < 6000 && total_score >= 5000) {
		stage_grade = 5;
		exp_rate = 5;
		money_rate = 150;
	} else if (total_score < 5000) {
		stage_grade = 6;
		exp_rate = 2;
		money_rate = 100;
	}
	score.stage_grade = stage_grade;
	uint32_t fumo_get_point = 0;


	score.all_mon_cnt = (score.all_mon_cnt == 0 ? 1: score.all_mon_cnt);

	uint32_t pass_percent = 100 * score.kill_mon_cnt / score.all_mon_cnt;
	uint32_t pass_exp = score.get_exp / 10;
	uint32_t evalute_exp = exp_rate * score.get_exp / 1000;
	
	if (!(btl->is_battle_tower())) {
		TRACE_LOG("%u %u %u", btl->get_winner(), id, fumo_points_end);
		TRACE_LOG("%u %u %u %u %u ", fumo_points_end, fumo_get_point, btl->difficulty(), lv, btl->stage()->max_lv);

		if (fumo_points_end >= max_fumo_points_daily_got) {
			fumo_get_point = 0xffffffff;
		} else {
			uint32_t base_lv = (btl->stage()->lv[0] + 3);
			
			fumo_get_point = get_fumo_points(btl->difficulty(), 
				stage_grade, lv > base_lv ? lv - base_lv : 0);
			if (btl->stage()->id >= 900 && fumo_get_point > 1) {
				fumo_get_point = 1;
			}
		}
		pass_percent = 100 * score.kill_mon_cnt / score.all_mon_cnt;
		pass_exp = score.get_exp / 10;
		evalute_exp = exp_rate * score.get_exp / 1000;
	} else {
		fumo_get_point = get_tower_fumo_point(cur_tower_layer, lv, stage_grade);
		if (fumo_tower_top < cur_tower_layer) {
			fumo_tower_top = cur_tower_layer;
			fumo_tower_used_tm = pass_tm;
		}
		if (fumo_tower_top == cur_tower_layer && (!fumo_tower_used_tm || fumo_tower_used_tm > pass_tm)) {
			fumo_tower_used_tm = pass_tm;
		}
	}

	fumo_get_point = (float)fumo_get_point * time_limit_per;
	add_fumo_points(fumo_get_point);

	//uint32_t money_reward = money_rate * ( rand() % 90 + 21 ) / 100;
	uint32_t money_reward = money_rate * ( 90 + rand() % 20 ) / 100;

	//uint32_t vip_reward_rate = 10;
	//uint32_t vip_reward = ( pass_exp + evalute_exp ) * vip_reward_rate / 100;
	uint32_t vip_rate = 0;
	if (is_vip_player()) {
		vip_rate = vip_level * 5 + 5;
	}
	uint32_t vip_reward_rate = vip_rate;
	uint32_t vip_reward = ( pass_exp + evalute_exp ) * vip_rate / 100;
	TRACE_LOG("vip exp[%u %u %u]:[%u %u]",id, vip, vip_level, vip_reward_rate, vip_reward);
	
	if (btl && btl->difficulty() == btl_easy) {
		uint32_t discount = 80;
		pass_exp = pass_exp * discount / 100;
		evalute_exp = evalute_exp * discount / 100;
		vip_reward = vip_reward * discount / 100;

		money_reward = money_reward * 60 / 100;
	}
	money_reward = (float)money_reward * time_limit_per;
	uint32_t team_add_exp_per = 0;
	uint32_t team_add_exp = 0;
	if (btl->is_team_btl() && btl->players_.size() > 1) {
		team_add_exp = pass_exp * 0.2 * (btl->players_.size() -1);
		team_add_exp_per = pass_percent;
	}
	uint32_t summon_exp = pass_exp + evalute_exp + vip_reward + team_add_exp;
	uint32_t total_exp = lv >= max_exp_lv ? 0 : summon_exp;

	uint32_t dexp_per = 1;
    if (dexp_tm > 0) {
		dexp_per = 2;
    }
	float tmp_trim = (exp_factor_trim == 0 ? 1 : exp_factor_trim);
	float total_per = (float)dexp_per * tmp_trim * time_limit_per;
	
	total_exp = ((float)total_exp * (total_per > 4 ? 2 : total_per));
	//add to player
	KDEBUG_LOG(id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=stage_end]", 
					3, 2, total_exp);
	if ( calc_lv_add_exp(total_exp, exp, lv, get_limit_lv(), role_type, id) ) {
		on_lv_up();
	}
	//add to summon monster
	if ( my_summon && my_summon->if_can_get_exp() && 
			calc_lv_add_exp(summon_exp, my_summon->exp, my_summon->lv, my_summon->get_limit_lv(), 0, 0, true) ) {
		my_summon->on_lv_up();
	}

	coins += money_reward;
	need_sav_ = true;
	TRACE_LOG("player attr:[%u], exp[%u %u], coins[%u %u] %u", id, exp , total_exp, coins, money_reward, fumo_get_point);

    if (!is_btl_over && btl->is_battle_tower()) {
        return;
    }
	if (is_btl_over) {
		btl->send_btl_over_pkg(this);
	}
	can_open_box = true;
	
	// notify the player the stage score
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, btl->difficulty(), idx);
	pack(pkgbuf, pass_tm, idx);
	pack(pkgbuf, static_cast<uint32_t>(score.on_hit_cnt), idx);
	pack(pkgbuf, pass_score, idx);
	pack(pkgbuf, operator_score, idx);

	pack(pkgbuf, pass_percent, idx);
	pack(pkgbuf, pass_exp, idx);
	pack(pkgbuf, exp_rate, idx);
	pack(pkgbuf, evalute_exp, idx);
	
	pack(pkgbuf, team_add_exp_per, idx);
	pack(pkgbuf, team_add_exp, idx);
	pack(pkgbuf, vip_reward_rate, idx);
	pack(pkgbuf, vip_reward, idx);

	pack(pkgbuf, total_exp, idx);
	pack(pkgbuf, money_reward, idx);
	pack(pkgbuf, stage_grade, idx);
	pack(pkgbuf, fumo_get_point, idx);
	
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), id, cli_proto_stage_score, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_player(this, pkgbuf, idx, 0);

	//stat log
	if (btl->is_team_btl()) {
		uint32_t tmp_buf[2] = {0};
		tmp_buf[0] = 1;
		tmp_buf[1] = id;
		do_stat_item_log_4(stat_log_team_btl_count, btl->difficulty() - 1, 0, tmp_buf, sizeof(tmp_buf));	
		tmp_buf[0] = pass_tm;
		tmp_buf[1] = id;
		do_stat_item_log_4(stat_log_team_btl_time, btl->difficulty() - 1, 0, tmp_buf, sizeof(tmp_buf));	
		tmp_buf[0] = score.on_hit_cnt ;
		tmp_buf[1] = id;
		do_stat_item_log_4(stat_log_team_btl_injured, btl->difficulty() - 1, 0, tmp_buf, sizeof(tmp_buf));	
	}
	if (btl->stage()->id > 900) {
		do_stat_item_log_5(stat_log_specal_stage, btl->stage()->id - 900, 0, 1);
	}
}

void 
Player::calc_pvp_score(uint32_t result_type)
{

	uint32_t win_flag = 5;
	uint32_t gain_exp = 0;
	uint32_t gain_honor = 0;
	uint32_t calc_lv = lv;

	float time_limit_per = 1.0;
	if (half_exp) {
		time_limit_per = 0.5;
	} 


	if ( (btl->pvp_lv() == pvp_lv_1 && lv >= 40) 
        || (btl->pvp_type() != 0 && btl->pvp_lv() != pvp_monster_) ) {
	// when user level > 40 invite_pk/footprint, then no rewards
		if (result_type) {
			continue_win ++;
			//calc win flag
			int hp_rate = (hp * 10) / (max_hp());
			if (hp_rate < 1) {
				win_flag = 4;
			} else if (hp_rate >= 1 && hp_rate < 5) {
				win_flag = 3;
			} else if (hp_rate >= 5 && hp_rate < 8) {
				win_flag = 2;
			} else if (hp_rate >= 8) {
				win_flag = 1;
			}
		} else {
			win_flag = 5;
			continue_win = 0;
		}
	} else {
		if (result_type) {
			gain_exp = 100 + (10000000 * calc_lv + 9 * calc_lv * calc_lv * calc_lv) / 5000000;
			exploit = 4;
			continue_win ++;
			//calc honor
			if (honor >= 261030 && lv >= 80) {
				gain_honor += 100;
			} else if (honor >= 115925 && lv >= 60) {
				gain_honor += 100;
			} else if (honor >= 37944 && lv >= 40) {
				gain_honor += 90;
			} else if (honor >= 6255 && lv >= 20) {
				gain_honor += 85;
			} else if (lv >= 10) {
				gain_honor += 75;
			}
			//calc win flag
			int hp_rate = (hp * 10) / max_hp();
			if (hp_rate < 1) {
				win_flag = 4;
			} else if (hp_rate >= 1 && hp_rate < 5) {
				win_flag = 3;
			} else if (hp_rate >= 5 && hp_rate < 8) {
				win_flag = 2;
			} else if (hp_rate >= 8) {
				win_flag = 1;
			}
		} else {
			gain_honor += 5;
			win_flag = 5;
			//gain_exp = 50 + (4000000 + 9 * calc_lv * calc_lv) / 10000000;
			gain_exp = 50 + (4000000 * calc_lv + 9 * calc_lv * calc_lv * calc_lv) / 10000000;
			exploit = 2;
			continue_win = 0;
		}
		
		if (continue_win >= 10 && continue_win < 20) {
			exploit += 1;
		} else if (continue_win >= 20 && continue_win < 30) {
			exploit += 2;
		} else if (continue_win >= 30) {
			exploit += 3;
		} 
	
		//add to player
		//when lv >= max_exp_lv  then got 0 exp
		uint32_t tmp_buf[1] = {0};
		tmp_buf[0] = gain_honor;
		do_stat_item_log_4(stat_log_pvp_honor, role_type - 1, 0, tmp_buf, sizeof(tmp_buf));
	}

	// time limit cut
	exploit = (float)exploit * time_limit_per;
	
	gain_honor = (float)gain_honor * time_limit_per;

	if (btl->is_battle_tmp_team_pvp()) {
		if (result_type) {
		//win
			gain_exp = 3;
		} else {
		//lose 
			gain_exp = 1;
		}
	} else {
		gain_exp = (float)gain_exp * time_limit_per;
		if (lv >= max_exp_lv) {
			gain_exp = 0;
		}
		add_exp(gain_exp);
	}
	
	honor += gain_honor;

	if (btl->pvp_lv() >= pvp_red_blue_1 && btl->pvp_lv() <= pvp_red_blue_3) {
		exploit *= 2;	
	}

	
	if (btl->pvp_lv() == pvp_lv_1) {
		uint32_t tmp_buf[1] = {0};
		tmp_buf[0] = gain_honor;
		do_stat_item_log_4(stat_log_pvp_lv1_honor, 0, 0, tmp_buf, sizeof(tmp_buf));
	} else {
		uint32_t tmp_buf[1] = {0};
		tmp_buf[0] = gain_honor;
		do_stat_item_log_4(stat_log_pvp_lv2_honor, 0, 0, tmp_buf, sizeof(tmp_buf));
	}
	need_sav_ = true;

	if (continue_win > max_conti_win_times) {
		max_conti_win_times = continue_win;
	}

	if (result_type) {
		if (btl->pvp_lv() == pvp_eve_of_16_fight_1) {
			exploit = 600;
		} else if (btl->pvp_lv() == pvp_eve_of_16_fight_2) {
			exploit = 900;
		} else if (btl->pvp_lv() == pvp_eve_of_16_fight_3) {
			exploit = 1200;
		} else if (btl->pvp_lv() == pvp_eve_of_16_fight_4) {
			exploit = 1500;
		} 
	} else {
		if (btl->pvp_lv() == pvp_eve_of_16_fight_1) {
			exploit = 0;
		} else if (btl->pvp_lv() == pvp_eve_of_16_fight_2) {
			exploit = 0;
		} else if (btl->pvp_lv() == pvp_eve_of_16_fight_3) {
			exploit = 0;
		} else if (btl->pvp_lv() == pvp_eve_of_16_fight_4) {
			exploit = 0;
		} 
	}

	

	TRACE_LOG("[%u win_flag%u exploit%u honor%u exp%u continue_win%u]", id, win_flag , 
			exploit, gain_honor, gain_exp, continue_win);
	uint32_t tmp_buf[1];
	tmp_buf[0] = exploit;
	do_stat_item_log_4(stat_log_pvp_exploit, 0, 0, tmp_buf, sizeof(tmp_buf));

	// notify the player the stage score
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	pack(pkgbuf, win_flag, idx);
	pack(pkgbuf, exploit, idx);
	pack(pkgbuf, gain_honor, idx);
	pack(pkgbuf, gain_exp, idx);
	pack(pkgbuf, continue_win, idx);
	TRACE_LOG("[win_flag%u exploit%u honor%u exp%u continue_win%u]", win_flag , exploit, gain_honor, gain_exp, continue_win);
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), id, cli_proto_pvp_score, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_player(this, pkgbuf, idx, 0);

	db_set_player_pvp_info(this, result_type, !result_type);
}

player_skill_t*  Player::get_skill(uint32_t skill_id)
{
	SkillMap::iterator it = skills_map.find(skill_id);
	if ( it == skills_map.end() )return NULL;
	return &(it->second);
}

int Player::notify_launch_skill_fail(uint32_t skill_id, uint32_t skill_lv)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, skill_id, idx);
	pack(pkgbuf, skill_lv, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_notify_launche_skill_fail, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	return send_to_player(this, pkgbuf, idx, 0);
}

int Player::notify_check_cheat()
{	
//	GF_LOG_WRITE(LOG_INFO, "cheat check ok", "userid = %u, role_regtime = %u", id, role_tm);	
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t),  cli_proto_check_cheat, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	return send_to_player(this, pkgbuf, idx, 0);
}

int Player::notify_damage_by_buff(uint32_t atk_id, int32_t cur_hp, int32_t damage, uint32_t action)
{
	uint32_t monsterid = 0;
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, atk_id, idx);
	pack(pkgbuf, 0, idx);
	pack(pkgbuf, static_cast<uint8_t>(0), idx);
	pack(pkgbuf, static_cast<uint8_t>(0), idx);
	pack(pkgbuf, static_cast<uint8_t>(0), idx);
		
	if (!is_valid_uid(id)) {
		monsterid = role_type;
	}
	pack(pkgbuf, id, idx);
	pack(pkgbuf, monsterid, idx);
	pack(pkgbuf, damage, idx);
	pack(pkgbuf, cur_hp, idx);
	pack(pkgbuf, 1,  idx);
	pack(pkgbuf, static_cast<uint8_t>(0), idx); //是否暴击
	pack(pkgbuf, static_cast<uint8_t>( action ), idx); //击倒类型 0: 无动作，1：后退， 2：倒地 3：击飞
	if( action != 0){
		pack(pkgbuf, 2000, idx); //飞行时间
	}else {
		pack(pkgbuf, 0, idx); 
	}
	
	pack(pkgbuf, static_cast<uint32_t>(pos().x()), idx);
	pack(pkgbuf, static_cast<uint32_t>(pos().y()), idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_players_hit, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	btl->send_to_btl(pkgbuf, idx, 0, 0);
	return 0;
}

int Player::notify_fall_down(int seconds)
{
	uint32_t monsterid = 0;
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, 0, idx);
	pack(pkgbuf, 0, idx);
	pack(pkgbuf, static_cast<uint8_t>(0), idx);
	pack(pkgbuf, static_cast<uint8_t>(0), idx);
	pack(pkgbuf, static_cast<uint8_t>(0), idx);

	if (!is_valid_uid(id)) {
		monsterid = role_type;
	}
	pack(pkgbuf, id, idx);
	pack(pkgbuf, monsterid, idx);
	pack(pkgbuf, 1, idx);
	pack(pkgbuf, hp - 1, idx);
	pack(pkgbuf, 0,  idx);
	pack(pkgbuf, static_cast<uint8_t>(0), idx);
	pack(pkgbuf, static_cast<uint8_t>(2), idx);
	pack(pkgbuf, seconds*1000, idx);
	pack(pkgbuf, static_cast<uint32_t>(pos().x()), idx);
	pack(pkgbuf, static_cast<uint32_t>(pos().y()), idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_players_hit, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	
	btl->send_to_btl(pkgbuf, idx, 0, 0);
	return 0;
}


int 
Player::notify_be_kicked()
{
	TRACE_LOG("kick %u", id);

	int idx = 0;
	idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, id, idx);
	init_btl_proto_head(pkgbuf, btl_nodi_user_be_kicked, idx);

	return send_to_player(this, pkgbuf, idx, 1);
}


int 
Player::pack_on_hit_info_ex(uint8_t* buf, const Player* atker, const skill_simple_info* p_skill, uint32_t dmg, uint32_t crits,  uint32_t prey_duration, uint8_t fall_down, uint32_t fly_duration, bool last_hit, uint8_t break_flag)
{
	uint32_t monsterid = 0;
	if(!is_valid_uid(id)){
		monsterid = role_type;
	}
	int idx = 0;
	pack(buf, atker->id, idx);
    pack(buf, p_skill->id, idx);
    pack(buf, static_cast<uint8_t>(p_skill->lv), idx);
    pack(buf, static_cast<uint8_t>(atker->score.cur_hit_num), idx);
    pack(buf, break_flag, idx);
    pack(buf, id, idx);

	ERROR_LOG("BREAK_FALG %u ID=%u Type=%u FALL_ %u %u %u ", break_flag, id, role_type, fall_down, prey_duration, 
			fly_duration);
    pack(buf, monsterid, idx);
    pack(buf, dmg, idx);
    pack(buf, hp, idx);
    pack(buf, prey_duration, idx);
    pack(buf, static_cast<uint8_t>(crits), idx);
    pack(buf, fall_down, idx);
    pack(buf, fly_duration, idx);
    pack(buf, static_cast<uint32_t>(pos().x()), idx);
    pack(buf, static_cast<uint32_t>(pos().y()), idx);
	return idx;
}

struct on_hit_info_t {
  uint32_t atker_id;
  uint32_t skill_id;
  uint32_t skill_lv;
  uint32_t continue_hit_num;
  uint32_t break_flag;
  uint32_t id;
  uint32_t monster_id;
  uint32_t damage;
  uint32_t hp;
  uint32_t prey_duration;
  uint8_t  crits;
  uint32_t fall_down;
  uint32_t fly_duration;
  uint32_t new_pos_x;
  uint32_t new_pos_y;
};	

int Player::pack_on_hit_info(void * buf, on_hit_info_t * info)
{
	if(!is_valid_uid(id)){
		info->monster_id = role_type;
	}
	int idx = 0;
	pack(buf, info->atker_id, idx);
    pack(buf, info->skill_id, idx);
    pack(buf, info->skill_lv, idx);
    pack(buf, info->continue_hit_num, idx);
    pack(buf, info->break_flag, idx);
    pack(buf, info->id, idx);
    pack(buf, info->monster_id, idx);
    pack(buf, info->damage, idx);
    pack(buf, info->hp, idx);
    pack(buf, info->prey_duration, idx);
    pack(buf, info->crits, idx);
    pack(buf, info->fall_down, idx);
    pack(buf, info->fly_duration, idx);
    pack(buf, info->new_pos_x, idx);
    pack(buf, info->new_pos_y, idx);
	return idx;
}	

	

int 
Player::calc_attack_impact(uint8_t* buf, Player* atker, const skill_simple_info* p_skill, uint32_t dmg, uint32_t crits, const skill_effects_t* p_skill_info,  bool last_hit)
{
	uint32_t hit_flag = 0;//0:only reduce HP; 1:stuck; 2:fall down; 3:Knockback; 4只击退不倒地
	uint32_t prey_duration = 0;
	uint32_t fly_duration = 0;
	uint8_t break_flag = 0;
	uint8_t fall_down = 0; // 1：受伤 2：击倒，3:击飞， 4：击退

    if (is_valid_uid(atker->id) && (!is_valid_uid(id))
        && ( atker->btl->stage()->id == 931 || atker->btl->stage()->id == 939 || atker->btl->stage()->id == 705) ) {
        ERROR_LOG("  [%u %u] [%u %u]", atker->id, atker->role_type, id, role_type);
        return pack_on_hit_info_ex(buf, atker, p_skill, dmg, crits, prey_duration, fall_down, fly_duration, last_hit, break_flag);
    }
	//没有技能打断的打击都无击倒或击飞
	if( p_skill->id == 4030049 || p_skill->id == 4120094)//这2个技能伤血，不击倒，不击飞
	{

	}
	else if (hit_fly_flag == false)
	{

	}
	else if( is_valid_uid(atker->id) && p_skill->type == normal_skill)//如果是人攻击怪物使用普通攻击
	{
		if(p_skill_info->rand_fall == 0)//如果没有配置随机击打效果，则按照默认的配置效果
		{
			fall_down = p_skill_info->fall_down;
			hit_flag = fall_down;
			prey_duration = p_skill_info->prey_duration;
			fly_duration = p_skill_info->fly_duration;
		}
		else//否则按照随机值
		{
			int rand1 = rand() % 100;
			if(rand1 >= 30)
			{
				hit_flag = 1;
				fall_down = 1;
				int rand2 = rand() % 100;
				if(rand2 < 7)
				{
					hit_flag = 3;
					fall_down = 3;
					prey_duration = 1000;
					fly_duration = 700;
				}
				else if(rand2 < 14)
				{
					hit_flag = 2;
					fall_down = 2;
					prey_duration = 500;
					fly_duration = 300;	
				}else{
					prey_duration = p_skill_info->prey_duration;
				}
			}
		}
	}
	else if(!is_valid_uid(atker->id) && p_skill->type == normal_skill)
	{
		hit_flag = 1;
		fall_down = 1;
		if(p_skill_info->rand_fall == 1)
		{
			int rand1 = rand() % 100;
			if( rand1 < 10)
			{
				hit_flag = 3;
				fall_down = 3;
				prey_duration = 1000;
				fly_duration = 700;
			}
			else
			{
				prey_duration = p_skill_info->prey_duration;
			}
		}
	}
	else//非普通攻击技能
	{
		hit_flag = 1;
		if( !mon_info || mon_info->spd > 0)
		{
			if (p_skill_info->fall_down && p_skill_info->fall_down != 4) 
			{
				fall_down = last_hit ? p_skill_info->fall_down : 1;
			} else if (p_skill_info->fall_down == 4) {
				fall_down = p_skill_info->fall_down;
				hit_flag = 4;
			} else {
				fall_down = 1;
			}
		}
		prey_duration = p_skill_info->prey_duration;
		fly_duration = p_skill_info->fly_duration;
	}

	uint32_t hit_type = 1;
	if (fall_down > 1 ) {
		hit_type = 2;
	}
//	if (buff_has_stuck_effect(p_skill_info->buff_id)) {
//		hit_type = 3;
//	}
	if (p_cur_skill) { 
		if (p_cur_skill->judge_can_be_break(hit_type)) {
			p_cur_skill->set_dead();
			on_skill_end(p_cur_skill);
			if (!is_valid_uid(id)) {
				atker->score.break_skill_cnt++;
			}
			break_flag = 1;
		} else { 
		//	break_flag = 0;
			fall_down = 5;
			hit_flag = 0;
		}
	}
    TRACE_LOG("break... [%u|%u] %u %u", atker->id, id, hit_type, break_flag);

	//调整被攻击之后的位置
	if (hit_flag != 0 && (!mon_info || mon_info->spd > 0) && dmg != 0 
			&&  !is_effect_exist_in_player(this, fortitude_effect_type)){
		set_pos_after_onhit(atker, p_skill_info, last_hit, hit_flag);
	}
	prey_duration = prey_duration  * get_rigidity_factor();
	if (is_valid_uid(this->id)) {
    	ERROR_LOG("PREY >>>>>>>>>>>>>>>> %u  %u Skill %u ", prey_duration, p_skill_info->fly_duration,  p_skill->id);
	}
	if (hit_flag != 0) {
		stuck_end_tm = *get_now_tv();
		timeadd(stuck_end_tm, prey_duration / 1000.0);				    
	}
	
	if (hit_flag != 0 && (fall_down > 1 && fall_down != 4) && last_hit ) {
		flying_end_tm = *get_now_tv();
		timeadd(flying_end_tm, prey_duration / 1000.0 + 0.5);
	}

	if (hit_flag != 0 && !is_valid_uid(id)) {
		if (!i_ai->is_in_state(DEFENSE_STATE)) 
		{
			i_ai->change_state(STUCK_STATE);
		}
		if( fall_down > 1)
		{
			super_armor = true;
		}
	}

	//存在不可击飞buff或光环效果
	if(is_effect_exist_in_player(this, fortitude_effect_type))
	{
		fall_down = 0;
	}
	
	if(fall_down == 3)
	{
		set_fly_flag(1);	
	}
//	ERROR_LOG("%u FLY %u time %u %p", id, fall_down, fly_duration, p_cur_skill);
	return pack_on_hit_info_ex(buf, atker, p_skill, dmg, crits, prey_duration, fall_down, fly_duration, last_hit, break_flag);

}



int
Player::pack_on_hit_info(uint8_t* buf, const Player* atker, const skill_simple_info* p_skill, uint32_t dmg, uint32_t crits, const skill_effects_t* p_skill_info, bool last_hit, uint8_t break_flag)
{
	uint32_t hit_flag = 0;//0:only reduce HP; 1:stuck; 2:fall down; 3:Knockback
	uint32_t prey_duration = 0;
	uint32_t fly_duration = 0;
	uint8_t fall_down = 0;
	
    if (is_valid_uid(atker->id) && atker->btl->stage()->id == 931) {
        goto stage_931;
    }
	if( p_skill->id == 4030049 || p_skill->id == 4120094)
	{

	}else if (hit_fly_flag == false) {
		//do nothing
	} else if (is_valid_uid(atker->id) && p_skill->type == normal_skill) {
		//normal skill deal : player hit monster
		int rand1 = rand() % 100;
		if (rand1 >= 30) {
			hit_flag = 1;//back
			fall_down = 1;
			if (p_skill_info->rand_fall == 1) {
				int rand2 = rand() % 100;
				if (rand2 < 20) {
					hit_flag = 3;//fly
					fall_down = 3;
					prey_duration = 1000;
					fly_duration = 700;
				} else if (rand2 < 40) {
					hit_flag = 2;//fall
					fall_down = 2;
					prey_duration = 500;
					fly_duration = 300;
				} else {
					prey_duration = p_skill_info->prey_duration;
				}
			}
		} else {
			break_flag = 0;
		}
	} else if (!is_valid_uid(atker->id) && p_skill->type == normal_skill) {
		//normal skill deal : monster hit player
		hit_flag = 1;//back
		fall_down = 1;
		if (p_skill_info->rand_fall == 1) {
			int rand1 = rand() % 100;
			if (rand1 < 10) {
				hit_flag = 3;//fly
				fall_down = 3;
				prey_duration = 1000;
				fly_duration = 700;
			} else {
				prey_duration = p_skill_info->prey_duration;
			}
		}
	} else {
		//other skill execpet nomal skill
		hit_flag = 1;
		if (!mon_info || mon_info->spd > 0) {
			//monster or player which can move
			if (p_skill_info->fall_down) {
				fall_down = last_hit ? p_skill_info->fall_down : 1;
			} else {
				fall_down = 1;
			}
		}
		prey_duration = p_skill_info->prey_duration;
		fly_duration = p_skill_info->fly_duration;
	}

	//compute atkee offset
	if (hit_flag != 0 && (!mon_info || mon_info->spd > 0)) {
		set_pos_after_onhit(atker, p_skill_info, last_hit, hit_flag);
	}
	//compute the on hit time
	
	
	prey_duration = prey_duration  * get_rigidity_factor();


	if (hit_flag != 0) {
		stuck_end_tm = *get_now_tv();
		timeadd(stuck_end_tm, prey_duration / 1000.0);
	}
	
	if (hit_flag != 0 && fall_down > 1 && last_hit) {
		flying_end_tm = *get_now_tv();
		timeadd(flying_end_tm, prey_duration / 1000.0 + 0.5);
	}
	
	if (hit_flag != 0 && !is_valid_uid(id)) {
		if (!i_ai->is_in_state(DEFENSE_STATE)) {
			i_ai->change_state(STUCK_STATE);
		} 

		if (fall_down > 1) {
			super_armor = true;
		}
	}

stage_931:
	uint32_t monsterid = 0;
	if (!is_valid_uid(id)) {
		monsterid = role_type;
	}

	//pack 
	int idx = 0;
	pack(buf, atker->id, idx);
	pack(buf, p_skill->id, idx);
	pack(buf, static_cast<uint8_t>(p_skill->lv), idx);
	pack(buf, static_cast<uint8_t>(atker->score.cur_hit_num), idx);
	pack(buf, break_flag, idx);
	pack(buf, id, idx);
	pack(buf, monsterid, idx);
	pack(buf, dmg, idx);
	pack(buf, hp, idx);
	pack(buf, prey_duration, idx);
	pack(buf, static_cast<uint8_t>(crits), idx);
	
	pack(buf, fall_down, idx);
	pack(buf, fly_duration, idx);

	pack(buf, static_cast<uint32_t>(pos().x()), idx);
	pack(buf, static_cast<uint32_t>(pos().y()), idx);

	TRACE_LOG("hit[%u %u %u %u %u %u]",id, dmg, fall_down, fly_duration, pos().x(), pos().y());
	TRACE_LOG("pack on hit[%u %u %u %u %d %u]\n",id, role_type, atker->id, dmg, hp, is_dead());
	//NOTI_LOG("onhit[%u]:[%u %u]", id, pos().x(), pos().y());
	return idx;
}

bool
Player::can_action()
{
	return true;
	/*
	if ( p_cur_skill != 0 ) {
		WARN_LOG("on use a skill,cant action: uid=%u",id);
		return false;
	}

	//check if on hit
	if ( timecmp(*get_now_tv(), stuck_end_tm) < 0 ) {
		WARN_LOG("on hit,cant action: uid=%u",id);
		return false;
	}
	*/
	//return true;
}

bool Player::is_invincibility()
{
	return (invincible || invincible_time || is_effect_exist_in_player(this, invincibility_type) );
}


player_skill_t*
Player::can_use_skill(uint32_t skill_id, bool mp_use_flag)
{
	const timeval* tv = get_now_tv();
	
	if ( p_cur_skill != 0 ) 
	{
		return 0;
	}

	//check if on hit
	if ( timecmp(*tv, stuck_end_tm) < 0 ) 
	{
		return 0;
	}

	//test skill cool time
	player_skill_t* p_skill;

	SkillMap::iterator it = skills_map.find(skill_id);
	if ( it == skills_map.end() ) 
	{
		WARN_LOG("you have no this skill: uid=%u skillid=%u", id, skill_id);
		return 0;
	}
	p_skill = &(it->second);

	if ( timecmp(*tv, p_skill->last_tv) < 0) 
	{
		return 0;
	}

    //沉默buff存在不能释放技能
    if (is_player_buff_exist(this, 1330) && is_skill_can_be_silent(skill_id)) {
        return 0;
    }

	int real_need_mp  = get_skill_mp_consumed(skill_id);
	//有X%的几率施放技能时不消耗灵力
	effect_data* pEffectdata = get_effect_data_in_player(this, protect_wakan_effect_type);
	if( pEffectdata )	
	{
		if ((uint32_t)ranged_random(0, 100) < pEffectdata->trigger_rate) {
			real_need_mp = 0;
		}
	}
	
    int last_mp = mp;
    if (is_summon_mon(role_type)) {
        last_mp = summon_info.fight_value;
    }
	if(  (hp <= static_cast<int>(p_skill->p_skill_info->hp_consumed)) || last_mp < real_need_mp) 
	{
		WARN_LOG("dont have enough hp or mp[%u %u, %u %u], you cant use this skill: uid=%u skillid=%u", hp, mp, 
				p_skill->p_skill_info->hp_consumed, real_need_mp, id, skill_id);
		return 0;
	} 
	else if ( mp_use_flag ) 
	{	
		TRACE_LOG("player hp mp before use skill[%u %u %u]",id, hp, mp);
		hp -= p_skill->p_skill_info->hp_consumed;
        if (is_summon_mon(role_type)) {
            summon_info.fight_value -= real_need_mp;
        } else {
            mp -= real_need_mp;
        }
		
		TRACE_LOG("player hp mp after use skill[%u %u %u]",id, hp, mp);

        //if (is_summon_mon(role_type))
        //    NOTI_LOG(" AI summon [%u|%u] use skill [%u %u]",id, role_type, hp, mp);
		//notify it's hpmp to players int battle when mp change
		if (real_need_mp) {
			noti_hpmp_to_btl();
		}
	}
	return p_skill;
}

void
Player::noti_hpmp_to_btl()
{
	//noti to all player in map
	if (!btl) {
		return;
	}
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, hp, idx);
	pack(pkgbuf, max_hp(), idx);
	
	if(is_summon_mon(role_type))
	{
		uint32_t max_fight_value = get_max_fight_value();
		uint32_t cur_fight_value = summon_info.fight_value;
		pack(pkgbuf, cur_fight_value , idx);
		pack(pkgbuf, max_fight_value, idx);
		TRACE_LOG("summon %u %u %u %u %u", id, role_type, hp, max_fight_value, cur_fight_value);
	}
	else
	{
		pack(pkgbuf, mp, idx);
		pack(pkgbuf, max_mp(), idx);
		TRACE_LOG("%u %u %u %u", id, hp, max_mp(), mp);
	}
	
	
	
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), id, cli_proto_player_hpmp_chg, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	btl->send_to_btl(pkgbuf, idx, 0, 0);
}

int
Player::test_lag()
{
	test_start_tv = *get_now_tv();
	++test_seq;
	if (test_seq == 0) {
		++test_seq;
	}

	// send a package to test lag
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, test_seq, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_battle_test_lag, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	send_to_player(this, pkgbuf, idx, 0);

	return 0;
}

int
Player::judge_boost_plugin(uint8_t move_type)
{
	//first
	if (boost_info.judge_cnt == 0) {
		boost_info.pos_x = pos().x();
		boost_info.pos_y = pos().y();
		boost_info.judge_cnt = 1;
		boost_info.last_judge_tm = *get_now_tv();
		return 0;
	}

	//get speed
	int x_speed = 0;
	int y_speed = 0;
	
	const monster_t* mon = get_monster(role_type, btl->difficulty());
	const timeval* tv = get_now_tv();

	float run_ratio = move_type == 2 ? 1.0 : 1.6;
	int spd_max = mon->spd * run_ratio * 1.2;
	int time_elapsed = timediff2(*tv, boost_info.last_judge_tm);

	if (time_elapsed == 0) {
		x_speed = spd_max + 1;
	} else {
		x_speed = abs(pos().x() - boost_info.pos_x) * 1000 / time_elapsed;
		y_speed = abs(pos().y() - boost_info.pos_y) * 1000 / time_elapsed;
	}

	//judge speed
	if (x_speed > spd_max || y_speed > spd_max) {
		boost_info.detect_cnt++;
	}
	boost_info.judge_cnt++;
	boost_info.last_judge_tm = *get_now_tv();
	boost_info.pos_x = pos().x();
	boost_info.pos_y = pos().y();
	TRACE_LOG("[%u %u %u %u %u %u %u]",id, time_elapsed, pos().x(), pos().y(), x_speed, y_speed, spd_max);

	//judge if use boost plugin
	if (boost_info.judge_cnt > 30) {
		if (boost_info.detect_cnt > 20) {
			//battle over
			WARN_LOG("FIND PLUGIN\t[%u %u]",id, boost_info.judge_cnt);
			do_stat_log_boost_num(1);
			//btl->on_player_use_plugin(this);
			//return 0;
		}

		//init next judge
		memset(&boost_info, 0x00, sizeof(boost_info));
		timeval tv = *get_now_tv();
		tv.tv_sec += 120;
		ev_mgr.add_event(*this, &Player::judge_boost_flag, tv);
	}

	return 0;
}

int
Player::judge_skip_step_move(const player_move_t* mv)
{
	const monster_t* mon = get_monster(role_type, btl->difficulty());
	float run_ratio = mv->mv_type == 2 ? 1.0 : 1.6;
	int distance_max = mon->spd * run_ratio * 16 / 100 * 1.3;
	
	int x_dis = abs(pos().x() - static_cast<int>(mv->x));
	int y_dis = abs(pos().y() - static_cast<int>(mv->y));
	int dis = sqrt(x_dis * x_dis + y_dis * y_dis); 

	TRACE_LOG("[%u %u %u %u %u %u %u]",id, pos().x(), pos().y(), mv->x, mv->y, distance_max, dis);

	skip_step_info.judge_cnt++;
	if (dis > distance_max) {
		skip_step_info.detect_cnt++;
	}
	
	if (skip_step_info.judge_cnt > 20) {
		if (skip_step_info.detect_cnt > 10) {
			do_stat_log_boost_num(2);
			WARN_LOG("SKIP STEP\t[%u %u > %u cnt=%u]",id, dis, distance_max, skip_step_info.detect_cnt);
		}
		memset(&skip_step_info, 0, sizeof(skip_step_info));
		timeval tv = *get_now_tv();
		tv.tv_sec += 120;
		ev_mgr.add_event(*this, &Player::judge_skip_step_flag, tv);
	}
	return 0;
}

int
Player::judge_boost_flag()
{
	boost_info.judge_flag = 1;

	return -1;
}

int
Player::judge_skip_step_flag()
{
	skip_step_info.judge_flag = 1;

	return -1;
}

int
Player::auto_regen_hpmp()
{
	if (((hp < max_hp()) || (mp < max_mp())) && !is_dead()) {
		chg_hp(auto_add_hpmp.hp_chg);
		chg_mp(auto_add_hpmp.mp_chg);
		//noti to all player in map
		noti_hpmp_to_btl();
		TRACE_LOG("hpmp auto add:uid=%u hpmp=%u %u add_hpmp=%u %u",
					id, hp, mp, auto_add_hpmp.hp_chg, auto_add_hpmp.mp_chg);
	}

	return 0;
}

int
Player::hpmp_chg(player_hpmp_chg_info_t& info)
{
	if (!is_dead() && ((info.hp_chg && hp <= max_hp()) || (info.mp_chg && mp <= max_mp()))) {
		TRACE_LOG("before use item:uid=%u hp_mp=%u %u %u %u", id, hp, max_hp(), mp, max_mp());
		// calc the player's hp & mp
		
		chg_hp(info.hp_chg); // !! note: this player will be set dead if hp is changed down to 0
		chg_mp(info.mp_chg);

		TRACE_LOG("after use item:uid=%u hp_mp=%u %u %u %u", id, hp, max_hp(), mp, max_mp());

		//noti to all player in map
		noti_hpmp_to_btl();

		if (is_valid_uid(id) && btl->stage_->id == 985 && info.hp_chg > 0 ) {
			Player * yaoshi = this->cur_map->get_one_monster(13131);
			if (yaoshi) {
				int d_hp = info.hp_chg * (-2);
				yaoshi->chg_hp(d_hp);
				yaoshi->noti_hpmp_to_btl();
			}
		}

		if (is_dead()) {
			return -1;
		}

		if ((info.hp_chg && hp < max_hp()) || (info.mp_chg && mp < max_mp())) {
			return 0;
		}		
	}

	// return -1 and event manager will delete this timed event
	return -1;
}

int
Player::property_chg(uint32_t& buf_type)
{
	return -1;
}

int
Player::invincible_chg()
{
	invincible_time = invincible_time <= 0 ? 0 : invincible_time - 1;
	TRACE_LOG("invincible finish[%u %d]", id, invincible_time);

	return -1;
}

int
Player::reduce_jewelry_duration()
{
	TRACE_LOG("jewelry duration loss[%u]", id);
	player_clothes_duration_chg(this, jewelry);	
	need_sav_ = true;

	for (int i = 0; i != clothes_info.jewelry_cnt; ++i) {
		if (clothes_info.clothes[clothes_info.weapon_cnt + clothes_info.armor_cnt + i].duration != 0) {
			return 0;
		}
	}

	// return -1 and event manager will delete this timed event
	return -1;
}


/**
  * @brief	reduce pvp coins safe
  */
void Player::reduce_pvp_coins()
{ 
	if ( btl && btl->pvp_lv() <= pvp_lv_2) {
		uint32_t coins = 200; 
		TRACE_LOG("%u %u", id, coins);
		reduce_coins(coins); 
	}
}
	


#ifdef DEV_SUMMON
/*
int Player::auto_reduce_fight_value()
{
	Player* summon = my_summon;
	if (!summon || summon->summon_info.owner != this) {
		TRACE_LOG("reduce error[%u %u]", id, summon->summon_info.owner->id);
		return -1;
	}

	if (summon->summon_info.fight_value <= min_fight_value) {
		TRACE_LOG("fight value end[%u %u %u]", id, summon->id, summon->summon_info.fight_value);
		summon->summon_info.active_flag = false;
		return -1;
	} else {
		summon->summon_info.fight_value -= 1;
	}
	TRACE_LOG("fight value[%u %u %u]", id, summon->id, summon->summon_info.fight_value);

	summon->summon_attr_chg();

	return 0;
}
*/
#endif

int
Player::pvp_battle_start()
{
	if (btl) {
		btl->set_battle_start(this);
	}
	notify_pvp_room_start(this, this->btl->id(), this->btl->pvp_lv());

	return 0;
}

/**
  * @brief set player to be the team leader
  */
void 
Player::set_player_leader(Player *p)
{ 
	//DEBUG_LOG("SET LEADER: %u %u", btl->id(), id);
	KDEBUG_LOG(id, "SET LEADER: %u %u", btl->id(), id);
	team_job = team_member_type_leader; 
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, id, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_notify_team_leader_change, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	btl->send_to_btl(pkgbuf, idx, p, 0);
}

int
Player::end_pvp_wait(uint32_t& type)
{
	TRACE_LOG("send_btl_over_pkg:%u", this->id);
	btl->set_end_reason(end_btl_pvp_timeout);
	btl->set_winner(0);
	btl->send_btl_over_pkg(this);
	this->pvp_end_ev = 0;	
	return 0;
}

int 
Player::proc_lv_matching(uint32_t& type)
{
	DEBUG_LOG("WAITING LIST SIZE\t[%lu]", btl->s_player_list_.size());
	if (type == 1/* && !btl->is_battle_started()*/) {
		for (Battle::PlayerList::iterator it = Battle::s_player_list_.begin();
				it != Battle::s_player_list_.end(); ++it) {

			if ((*it) != this && 
					(*it)->btl   &&
					abs(static_cast<int>((*it)->lv) - static_cast<int>(this->lv)) <= 6 ) {

				if (btl->pvp_lv() != (*it)->btl->pvp_lv()) {
					continue;
				}
				TRACE_LOG("matched pvp");
				//"this" is first player 
				Player* second_p = *it;
				Battle* sec_btl 	= second_p->btl;
				btl->send_lv_match_rsp_in_timer(second_p, btl_mode_lv_matching, this);
				
				//del other player's btl
				second_p->leave_map();
				delete sec_btl;
				second_p->team = player_team_2;
				btl->add_player(second_p);
				btl->start_battle();
				btl->remove_wait_player(this);
				btl->remove_wait_player(second_p);
				ev_mgr.remove_event(this->time_ev);
				this->time_ev = 0;
				ev_mgr.remove_event(second_p->time_ev);
				second_p->time_ev = 0;
				
				return 0;
			}			
		}
		if (!btl->is_battle_started()) {
			this->time_out_lv = 1;
			type = 2;
		}
	} else if (type == 2 /*&& !btl->is_battle_started()*/) {
		TRACE_LOG("send_btl_over_pkg:%u", this->id);
		btl->set_end_reason(end_btl_pvp_timeout);
		btl->set_winner(0);
		btl->send_btl_over_pkg(this);
		btl->remove_wait_player(this);
		this->time_ev = 0;
	}

	return 0;
}

uint16_t Player::get_limit_lv()
{
	uint16_t max_level = 100;
	if (is_summon_mon(role_type)) {
		max_level = summon_info.owner->lv;
		uint32_t type = role_type % 10;
		uint32_t mon_type = role_type / 10;
		if (mon_type == 101) {
			if (type == 2) {
				max_level = 25;
			}
			if (type == 3) {
				max_level = 35;
			}
			if (type == 4) {
				max_level = 45;
			}
		} else {
			if (type == 2) {
				max_level = 20;
			}
			if (type == 3) {
				max_level = 30;
			}
			if (type == 4) {
				max_level = 40;
			}
		}
		if (max_level > summon_info.owner->lv) {
			max_level = summon_info.owner->lv;
		}
	} else {
		max_level = max_exp_lv;
	}
	return max_level;
}

inline bool
Player::if_can_get_exp()
{
	if (is_summon_mon(role_type)) {
		uint32_t type = role_type % 10;
		uint32_t mon_type = role_type / 10;
		/*if (mon_type == 100 || mon_type == 102 || mon_type == 103 || 
			mon_type == 104 || mon_type == 105) {
			if ( (lv == 20 && type == 2) ||
		  		 (lv == 30 && type == 3) ||
		  		 (lv == 40 && type == 4)) {
				return false;
			}
		} else if (mon_type == 101) {
			if ( (lv == 25 && type == 2) ||
		  		 (lv == 35 && type == 3) ||
		  		 (lv == 45 && type == 4) ) {
				return false;
			}
		}*/
		if (mon_type == 101) {
			if ( (lv == 25 && type == 2) ||
		  		 (lv == 35 && type == 3) ||
		  		 (lv == 45 && type == 4) ) {
				return false;
            }
		//}else if (mon_type == 108 && lv == 40) {
		//can not get exp temporarily
		//	return false;
		//}else if (mon_type == 109 && lv == 1) {
		//can not get exp temporarily
		//	return false;
		} else {
			if ( (lv == 20 && type == 2) ||
		  		 (lv == 30 && type == 3) ||
		  		 (lv == 40 && type == 4)) {
				return false;
			}
		}

		if (summon_info.owner->lv <= lv) {
			return false;
		}
	}
	return true;
}
//--------------------------------------------------------------------

/**
  * @brief allocate and add a player to this server
  * @return pointer to the newly added player
  */
Player* add_player(userid_t uid, fdsession_t* fdsess)
{
	return new Player(uid, fdsess);
}

/**
  * @brief delete player
  * @param p player to be deleted
  */
void del_player(Player* p)
{
	//DEBUG_LOG("D U\t[uid=%u fd=%d]", p->id, p->fd);
	KDEBUG_LOG(p->id, "D U\t[uid=%u fd=%d]", p->id, p->fd);

	Battle *watch_btl = p->watch_btl;
	if (watch_btl) { 
		watch_btl->del_watcher(p, end_btl_normal);
		p->btl = 0;
		delete p;
		return;
	}

	Battle* btl = p->btl;
	if (btl) {
		btl->del_player(p, end_btl_offline);
		delete p;
		if (btl->is_battle_deletable()) {
			delete btl;
		}
	} else {
		delete p;
	}
}

/**
  * @brief set need_sav_ false 
  * @param p player 
  */
void set_unneed_save(Player* p)
{
	if (is_valid_uid(p->id)) {
		p->need_sav_ = false;
		if (p->my_summon) {
			p->my_summon->need_sav_ = false;
		}
	}

}
/**
  * @brief clear players' info whose online server is down
  * @param fd online fd
  */
void clear_players(int fd)
{
	DEBUG_LOG("B4\t[player_num=%lld]", static_cast<long long>(all_players.size()));

	/*for (PlayerMap::iterator it = all_players.begin(); it != all_players.end(); ++it) {
		Player* p = it->second;
		if ((p->fd == fd) || (fd == -1)) {
			del_player(p);
		}
	}*/

	PlayerMap::iterator it = all_players.begin();
	while (it != all_players.end()) {
		PlayerMap::iterator t_it = it++;
		Player* p = t_it->second;
		if ((p->fd == fd) || (fd == -1)) {
			set_unneed_save(p);
			del_player(p);
		}
	}

	DEBUG_LOG("AF\t[player_num=%lld]", static_cast<long long>(all_players.size()));
}

/**
  * @brief get player by user id
  * @param uid user id
  * @return pointer to the player if found, 0 otherwise
  */
Player* get_player(userid_t uid)
{
	PlayerMap::iterator it = all_players.find(uid);
	if (it != all_players.end()) {
		return it->second;
	}

	return 0;
}

//--------------------------------------------------------------------

int pack_player_clothes_duration(Player* p, void* buf)
{
	uint32_t cnt = 0;
	int idx = 4;
	int start_idx = 0;

	for (uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++) {
		player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
		if (clothes->duration_chg_flag) {
			pack_h(buf, clothes->unique_id, idx);
			pack_h(buf, static_cast<uint32_t>(clothes->duration), idx);
			TRACE_LOG("pack clothes[%u %u %u]",p->id, clothes->clothes_id, clothes->duration);
			cnt++;
		}
	}
	
	pack_h(buf, cnt, start_idx);

	return idx;
}

int db_set_unique_item_bit(Player* p)
{
	int idx = 0;
	pack(dbpkgbuf, p->unique_item_bit, sizeof(p->unique_item_bit), idx);
	
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_unique_item_bit, dbpkgbuf, idx);
}


int db_set_player_base_info(Player* p)
{
	int idx = 0;
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->lv), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->exp), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->coins), idx);
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_player_basic_attr2, dbpkgbuf, idx);
}

int db_set_player_basic_attr(Player* p)
{
	int idx = 0;
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->lv), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->exp), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->allocator_exp), idx);
    
	//double_time
	uint32_t btl_interval = 0;
	if (p->score.start_tm != 0) {
		btl_interval = get_now_tv()->tv_sec - p->score.start_tm;
	}
 
 	if (!(p->btl) || p->btl->is_battle_pvp() || p->btl->is_battle_tmp_team_pvp() || p->btl->is_battle_tower()) {
        btl_interval = 0;
    }
	if (is_in_not_use_double_tm_time(p)) {
		pack_h(dbpkgbuf, static_cast<uint32_t>(p->dexp_tm), idx);
	} else {
	    if (btl_interval < p->dexp_tm) {
	        pack_h(dbpkgbuf, static_cast<uint32_t>(p->dexp_tm - btl_interval), idx);
	    } else {
	        pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);
	    }
	}

	uint32_t tmp_coins = (p->coins > p->pre_coins) ? (p->coins - p->pre_coins) : 0;
	pack_h(dbpkgbuf, tmp_coins, idx);
	//pack_h(dbpkgbuf, static_cast<uint32_t>(p->hp), idx);
	//pack_h(dbpkgbuf, static_cast<uint32_t>(p->mp), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->skill_point), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->fumo_points_end - p->fumo_points_start), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->fumo_tower_top), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->fumo_tower_used_tm), idx);

	TRACE_LOG("set_player_basic_attr exp %u double exp time=[%u], interval time=[%u]",p->exp, p->dexp_tm, btl_interval);

	//record stage
	/*uint32_t stage_id = 0;
	if (p->score.end_tm) {
		stage_id = p->btl->stage_id();
		if (stage_id > max_stage_num) {
			WARN_LOG("stage id more than max[%u %u %u]",p->id, stage_id, max_stage_num);
			stage_id = 0;
		}
	}
	pack_h(dbpkgbuf, stage_id, idx);*/

	//save info of clothes duration
	idx += pack_player_clothes_duration(p, dbpkgbuf + idx);
	
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_player_basic_attr, dbpkgbuf, idx);
}

int db_set_summon_basic_attr(Player* summon, Player* owner)
{
	int idx = 0;
	pack_h(dbpkgbuf, static_cast<uint32_t>(summon->summon_info.mon_tm), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(summon->lv), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(summon->exp), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(summon->summon_info.fight_value), idx);
	TRACE_LOG("db save summon:[%u %u %u %u]", owner->id, summon->summon_info.mon_tm, summon->lv, summon->exp);
	
	return send_request_to_db(0, owner->id, owner->role_tm, dbproto_set_summon_basic_attr, dbpkgbuf, idx);
}

int db_set_player_pvp_info(Player* p, uint32_t win_flg, uint32_t failed_flg)
{
	int idx = 0;
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->honor), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->exploit), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(win_flg), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(failed_flg), idx);		

	pack_h(dbpkgbuf, static_cast<uint32_t>(p->max_conti_win_times), idx);

	TRACE_LOG("set_player_pvp_info %u %u %u %u", p->honor, p->exploit, win_flg, p->max_conti_win_times);

	
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_pvp_info, dbpkgbuf, idx);
}

int db_set_player_task_buf(Player* p)
{
	return 0;
}

int db_set_player_kill_boss(Player* p, uint32_t monsterid)
{
	int idx = 0;
	pack_h(dbpkgbuf, monsterid, idx);
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_player_kill_boss, dbpkgbuf, idx);
}

int db_add_amb_achieve_num(Player* p, uint32_t parentid)
{
	return send_request_to_db(0, parentid, 0, dbproto_add_amb_achieve_num, 0, 0);	
}

int db_set_user_flag(Player* p)
{
	
	set_bit_on(p->once_bit, flag_pos_achieve_lv);
	TRACE_LOG("once bit :set %u %x %x %x %x", flag_pos_achieve_lv, p->once_bit[0]
		, p->once_bit[1], p->once_bit[2], p->once_bit[10]);
	int idx = 0;
	pack(dbpkgbuf, p->once_bit, sizeof(p->once_bit), idx);
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_user_flag, dbpkgbuf, idx);
}

/**
  * @brief callback for using a item
  * @param p the player who initiated the request to dbproxy
  * @param id the requester id
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_player_pick_item_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	uint8_t pick_flag = 1;
	uint32_t owner_id = 0;

	ItemDropMap::iterator it = p->cur_map->item_drops.find(p->cur_item_picking);

	uint32_t rsp_item_id = 0;
	uint32_t rsp_unique_id = 0;
	if ( ret == 1017 ) {
		//db timeout
		return send_header_to_player(p, p->waitcmd, cli_err_base_dberr + ret, 1); 
	} else if ( ret ){
		//set the state of the item
		it->second.set_state(can_pick);
		//return send_header_to_player(p, p->waitcmd, cli_err_base_dberr + ret, 1); 
		pick_flag = 0;

		p->cur_item_picking = ret;
	} 

	if (p->cur_picking_summon && p->my_summon) {
		p->my_summon->i_ai->item_flag = 0;
	}
	
	if (pick_flag) {
		//delete the item from map
		owner_id = it->second.get_owner_id();

		p->cur_map->item_drops.erase(p->cur_item_picking);
	
		//stage_score
		p->score.pick_drop_cnt++;
		//get item id
		player_pick_item_rsp_t* rsp = reinterpret_cast<player_pick_item_rsp_t*>(body);
		rsp_item_id = rsp->item_id;
		rsp_unique_id = rsp->unique_id;
	}

	//uint32_t unique_id = *(reinterpret_cast<uint32_t*>(body));
	TRACE_LOG("clothes unique id[%u %u %u]",p->id, rsp_item_id, rsp_unique_id);

	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, pick_flag, idx);
	pack_h(pkgbuf, p->id, idx);
	pack_h(pkgbuf, p->cur_item_picking, idx);
	pack_h(pkgbuf, rsp_item_id, idx);
	pack_h(pkgbuf, rsp_unique_id, idx);
	pack_h(pkgbuf, p->cur_picking_summon, idx);

	init_btl_proto_head(pkgbuf, p->waitcmd, idx);

	if (pick_flag) {
		if (!owner_id) {
			p->cur_map->send_to_map(pkgbuf, idx, p);
		} else {
			send_to_player(p, pkgbuf, idx, 1);
		}
		
		uint32_t pos = get_unique_item_bitpos(rsp_item_id);
		TRACE_LOG("get_unique_item_bitpos%u:%u %u", pos, rsp_item_id, rsp_unique_id);
		if (pos) {
			if (!test_bit_on(p->unique_item_bit, pos)) {	
				taomee::set_bit_on(p->unique_item_bit, pos);
				//db_set_unique_item_bit(p);
				DEBUG_LOG("set bit on pos:%u %u get unique item:%u", p->id, pos, rsp_item_id);
			}
		}
		do_stat_log_got_amber(rsp_item_id, p->role_type);
        if (rsp_item_id == 1500362)
            do_stat_item_log_5(0x09526001, 0, 1, 0);
	} else {
		send_to_player(p, pkgbuf, idx, 1);
	}

	p->cur_item_picking = 0;
	p->cur_picking_summon = 0;
	
	return 0;
}

uint16_t get_fumo_points(uint32_t difficulty,uint32_t	stage_grade, uint16_t lv)
{
	static uint16_t fumo_points_map[6][6][5] = 
		{
			{
				{11, 9, 6, 4, 4},
				{7, 5, 3, 2, 2},
				{4, 3, 2, 1, 1},
				{2, 2, 1, 1, 1},
				{2, 2, 1, 1, 1},
				{0, 0, 0, 0, 0}
			},
			{
				{17, 13, 9, 5, 4},
				{10, 8, 5, 3, 3},
				{7, 5, 3, 2, 2},
				{3, 3, 2, 1, 1},
				{3, 3, 2, 1, 1},
				{0, 0, 0, 0, 0}
			},
			{
				{25, 20, 13, 7, 5},
				{15, 11, 8, 4, 4},
				{10, 8, 5, 3, 3},
				{5, 4, 3, 1, 1},
				{5, 4, 3, 1, 1},
				{0, 0, 0, 0, 0}
			},
			{
				{38, 29, 20, 10, 5},
				{23, 17, 11, 6, 4},
				{15, 11, 8, 4, 3},
				{8, 6, 4, 2, 2},
				{8, 6, 4, 2, 2},
				{0, 0, 0, 0, 0}
			},
			{
				{38, 38, 38, 38, 38},
				{23, 23, 23, 23, 23},
				{15, 15, 15, 15, 15},
				{8, 8, 8, 8, 8},
				{8, 8, 8, 8, 8},
				{0, 0, 0, 0, 0}
			},
			{
				{57, 57, 57, 57, 57},
				{33, 33, 33, 33, 33},
				{22, 22, 22, 22, 22},
				{12, 12, 12, 12, 12},
				{12, 12, 12, 12, 12},
				{12, 12, 12, 12, 12}
			}

		};
	if (lv > 4) {
		lv = 4;
	}
	if (lv < 5 && stage_grade < max_stage_grade + 1
		&& difficulty < 7) {
		TRACE_LOG("%u %u %u %u ", difficulty, stage_grade, lv, fumo_points_map[difficulty - 1][stage_grade - 1][lv]);
		return fumo_points_map[difficulty - 1][stage_grade - 1][lv];
	}
	return 0;
}

uint16_t get_tower_fumo_point(uint32_t layer, uint32_t player_lv, uint32_t stage_grade)
{
	static uint16_t tower_fumo_point_map[10][6] = {
		{5, 4, 3, 2, 1, 1}, 
		{6, 5, 4, 2, 1, 1},
		{7, 6, 4, 3, 1, 1},
		{9, 7, 5, 3, 2, 1},
		{10, 8, 6, 4, 2, 1},
		{12, 10, 7, 5, 2, 1},
		{15, 12, 9, 6, 3, 1},
		{18, 14, 11, 7, 4, 1},
		{21, 17, 13, 9, 4, 1},
		{26, 21, 15, 10, 5, 1}
	};
	if (layer > 10 || stage_grade > 6 || player_lv < 10) {
		return 0;
	}
	uint16_t ret_points = tower_fumo_point_map[layer - 1][stage_grade - 1];
	int calc_lv = player_lv;
	int calc_layer = layer;
	if ((calc_lv - calc_layer * 5 - 10) > 0) {
		float para = pow(0.92, (calc_lv - calc_layer * 5 - 10));
		para = ret_points * para;
		ret_points = para < 1 ? 1 : para;
	}
	return ret_points;
}
void player_map_routing(struct timeval cur_time)
{
	PlayerMap::iterator pItr = all_players.begin();
	while (pItr != all_players.end())
	{
		//TRACE_LOG("player_map_routing");
		Player* p = pItr->second;
		if(p != NULL)
		{
			player_timer(p, cur_time);
		}
		++pItr;
	}
}

void player_timer(Player* p, struct timeval cur_time)
{
	if(!p->is_dead())
	{
		player_bufflist_routing(p, cur_time);
	}	
	if(!p->is_dead())
	{
		player_auralist_routing(p, cur_time);
	}
	if(p->is_dead())
	{
		del_buff_when_condition(p, buff_dead_delete_reason);
	}
	const timeval* tv = get_now_tv();

	if(is_valid_uid(p->id))
	{
		if( p->mon_update_tv.tv_sec == 0 && p->mon_update_tv.tv_usec == 0)
		{
			p->mon_update_tv = *tv;	
		}
		
		if( !p->is_dead() && 
			 p->my_summon && 
			 !p->my_summon->is_dead() && 
			 p->my_summon->summon_info.active_flag && 
			 p->my_summon->cur_map)
		{
			int milli_sec = timediff2(*tv, p->mon_update_tv); 
			p->my_summon->update(milli_sec);   		
		}
		p->mon_update_tv = *tv;
	}

//	p->cur_map->clear_dead_barriers();

}

uint32_t Player::get_speed()
{
	return speed * speed_change_buf_trim;	
}


void Player::fall_down(int seconds)
{
	notify_fall_down( seconds );
}

void Player::monster_speaks(uint32_t word_idx)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, word_idx, idx);
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), id, cli_proto_monster_speak, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	cur_map->send_to_map(pkgbuf, idx);	
}


void Player::add_all_monsters_buff(uint32_t buff_id)
{
	map_t* pMap = cur_map;
	PlayerSet::iterator pItr = pMap->monsters.begin();	
	for( ; pItr != pMap->monsters.end(); ++pItr)
	{
		Player* p = *pItr;
		add_player_buff(p, buff_id, id);
	}	
}



void notify_delete_player_obj(Player* p)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_del_map_summon_obj, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	p->cur_map->send_to_map(pkgbuf, idx, 0);
}


void Player::set_all_skills_ready()
{
	for ( SkillMap::iterator it = skills_map.begin(); it != skills_map.end(); ++it )
	{
		player_skill_t* tmp_skill = &(it->second);
		set_skill_cool_time(tmp_skill->skill_id,  0);
	}
}

void Player::set_all_skills_cd()
{
	for ( SkillMap::iterator it = skills_map.begin(); it != skills_map.end(); ++it ) 
	{
		player_skill_t* tmp_skill = &(it->second);
		set_skill_cool_time(tmp_skill->skill_id, tmp_skill->p_skill_info->cool_down);
	}		
}

void Player::proc_use_item_extra_logic(uint32_t item_id) 
{
	if (btl->stage()->id != 925 ) {
		return;
	}

	if (item_id == super_walnut_item_id) { 
		for (PlayerSet::iterator it = cur_map->monsters.begin(); 
				it != cur_map->monsters.end(); ++it) {
			if ((*it)->role_type == 13084) {
				add_player_buff((*it), 510, this->id, 11);
			}
		}
	} else if (item_id == clear_all_monster_item_id) {
		call_map_summon(9, 550, 330, false);//地图中央高爆核桃爆炸 
	}
}

void Player::monster_stand_by_faint()
{

	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, this->pos().x(), idx);
	pack(pkgbuf, this->pos().y(), idx);
	pack(pkgbuf, this->dir, idx);

	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), this->id, cli_proto_stand, 0, 
		idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	cur_map->send_to_map(pkgbuf, idx);
}

void Player::adjust_coordination()
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, this->id, idx);
	pack(pkgbuf, static_cast<uint32_t>(this->pos().x()), idx);
	pack(pkgbuf, static_cast<uint32_t>(this->pos().y()), idx);
	pack(pkgbuf, static_cast<uint32_t>(this->dir), idx);

	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), this->id, cli_adjust_coordinate, 0, 
		idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	cur_map->send_to_map(pkgbuf, idx);
}

void Player::proc_move_extra_logic()
{
	if (btl->stage()->id == 926) {
		if (is_valid_uid(id)) {
			if (is_player_buff_exist(this, 31) && check_map_summon(this, 9911)) { 
				call_map_summon(14, this->pos().x(), this->pos().y(), false);

			} else if (is_player_buff_exist(this, 32) && check_map_summon(this, 9910)) { //冰buff
				call_map_summon(15, this->pos().x(), this->pos().y(), false);
			}
		}
	} 
}

void Player::add_contest_exp(uint32_t stageid, uint32_t contest_id, bool all_flag) 
{
    int32_t pos = get_unique_item_bitpos(stageid - 935 + 3001);
    if (pos) {
        TRACE_LOG("unique_item_bit before: [%x]", this->unique_item_bit[8]);
        if (!test_bit_on(this->unique_item_bit, pos)) {	
            taomee::set_bit_on(this->unique_item_bit, pos);
            TRACE_LOG("unique_item_bit after : [%x]", this->unique_item_bit[8]);
            db_set_unique_item_bit(this);
            int add_exp = 0;
            if (all_flag) {
                add_exp = this->lv * 3000 + 10000;
            } else {
                add_exp = this->lv * 2000 + 10000;
            }
            if ( calc_lv_add_exp(add_exp, this->exp, this->lv, this->get_limit_lv(), 
                    this->role_type, this->id) ) {
                this->on_lv_up();
            }
            db_set_player_base_info(this);
        }
    }
}


void Player::add_pet_fight_value(uint32_t add_value)
{
	if(is_summon_mon(role_type))    
	{
		summon_info.fight_value += add_value;
		summon_info.fight_value = summon_info.fight_value > get_max_fight_value() ? get_max_fight_value() : summon_info.fight_value;
		//summon_attr_chg();
		noti_hpmp_to_btl();
	}
}

void Player::talk(uint8_t* msg, uint32_t msg_len, userid_t recvid)
{
	int idx = sizeof(btl_proto_t);
	pack(pkgbuf, msg, msg_len, idx);
	init_btl_proto_head_full(pkgbuf, btl_transmit_only, idx, 0);
	btl->send_to_btl(pkgbuf, idx, 0, 0);
} 

void Player::change_show(uint32_t state)
{
	this->show_state = state;
	uint8_t buf[1024] = {0};
    int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(buf, this->show_state, idx);
    init_cli_proto_head_full(buf + sizeof(btl_proto_t), this->id, cli_proto_chg_app, 0, idx - sizeof(btl_proto_t));
    init_btl_proto_head(buf, btl_transmit_only, idx);
    this->cur_map->send_to_map(buf, idx);
}


void Player::rabbit_twinkling_move()
{
	if (!btl) {
		return ;
	}
	std::list<map_summon_object*>::iterator pItr = btl->map_summon_list.begin();
	for(; pItr != btl->map_summon_list.end(); ++pItr)
	{
		map_summon_object* obj = *pItr;
		if(obj && obj->owner_id_ == this->id){
			if (player_teleport(this, obj->pos().x(), obj->pos().y())) {
				btl->del_map_summon(obj);
				return;
			}
		}
	}
	return ;
}

void Player::teleport(uint32_t x, uint32_t y)
{
	Vector3D new_pos(x, y);
	adjust_position_ex(new_pos);
	send_player_move_noti(new_pos, 4);
}

void Player::send_player_move_noti(const Vector3D& new_pos, uint8_t move_type)
{
	uint16_t* plag;
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, new_pos.x(), idx);
	pack(pkgbuf, new_pos.y(), idx);
	pack(pkgbuf, new_pos.z(), idx);
	/*pack(pkgbuf, player->i_ai->target_pos2.x(), idx);
	pack(pkgbuf, player->i_ai->target_pos2.y(), idx);
	pack(pkgbuf, player->i_ai->target_pos2.z(), idx);*/
	pack(pkgbuf, this->dir, idx);
	pack(pkgbuf, move_type, idx);

	plag = reinterpret_cast<uint16_t*>(pkgbuf + idx);
	idx += sizeof(*plag);

	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), this->id, cli_proto_npc_move, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	
	

	for (PlayerSet::const_iterator it = this->cur_map->players.begin(); it != this->cur_map->players.end(); ++it) {
		Player* p = *it;
		*plag = bswap(static_cast<uint16_t>(this->lag + p->lag));
		TRACE_LOG("SET_POS: uid=%u , [%u %u]", p->id, new_pos.x(), new_pos.y());
		send_to_player(p, pkgbuf, idx, 0);
	}
}

void  Player::add_player_team_attr(bool reset_cur_status)
{
	fight_team_attr_add_t attr_add_info;
	::calc_player_team_attr_add_by_team_lv(&attr_add_info, this->team_lv);

	this->atk += attr_add_info.atk;
	this->def_value += attr_add_info.def_value;
	this->maxmp += attr_add_info.mp;
	this->maxhp += attr_add_info.hp;

	if (reset_cur_status) {
		this->mp = this->max_mp();
		this->hp = this->max_hp();
	}
}

