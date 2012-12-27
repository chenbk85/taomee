#include <cstdio>
#include <set>


using namespace std;

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <libtaomee++/time/time.hpp>

using namespace taomee;

extern "C" {
#include <glib.h>
#include <assert.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
}
#include "utils.hpp"
#include "player.hpp"
#include "stage.hpp"

#include "battle.hpp"
#include "battle_impl.hpp"
#include "battle_manager.hpp"
#include "battle_switch.hpp"
#include "player_status.hpp"
//#include "app_log.hpp"
#include "battle_logic.hpp"
#include "detective.hpp"
#include "ai/MonsterAI.hpp"

/**
  * @brief battle invitation response
  */
enum btl_inv_rsp_t {
	btl_inv_refuse				= 0,
	btl_inv_accept				= 1,
};

void save_buff_to_db(Player* p)
{
	int idx_tmp = 0;
	int idx = 4;
	uint32_t count = 0;
	for (BufSkillMap::iterator it = p->buf_skill_map.begin(); it != p->buf_skill_map.end(); ++it) {
		if (!(it->second.db_buff_flg) ) {
			TRACE_LOG("!(it->second.db_buff_flg) %u %u", it->second.buf_type,it->second.db_buff_flg);
			continue;
		}
		uint32_t usedtime = 0;
		if (!is_in_not_use_double_tm_time(p)) {
			if (p->btl->battle_mode()!= btl_mode_lv_matching) {
				if (it->second.start_tm && it->second.buf_type != 1400) {
					usedtime = get_now_tv()->tv_sec - it->second.start_tm;
				} else if (it->second.start_tm == 0) {
					usedtime = get_now_tv()->tv_sec - p->btl->btl_start_tm;
				}
			}
		}

		uint32_t lefttime = it->second.buf_duration > usedtime ? 
								it->second.buf_duration - usedtime : 0;
		if (!lefttime ) {
			TRACE_LOG("!lefttime %u %u %u", it->second.buf_type, usedtime, lefttime);
			continue;
		}
		TRACE_LOG("%u %u %u %u %u ", it->second.buf_type, it->second.per_trim, usedtime, lefttime, it->second.buf_duration);
		pack_h(dbpkgbuf, it->second.buf_type, idx);
		pack_h(dbpkgbuf, lefttime, idx);
		pack_h(dbpkgbuf, it->second.mutex_type, idx);
		pack_h(dbpkgbuf, it->second.start_tm, idx);
		count ++;

	}
	pack_h(dbpkgbuf, count, idx_tmp);		

	TRACE_LOG("dbproto_update_db_buff %u ", count);
	send_request_to_db(0, p->id, p->role_tm, dbproto_update_db_buff, dbpkgbuf, idx);
}

uint32_t get_pvp_lv(uint16_t stage_id)
{
	if (stage_id == 907) {
		return pvp_lv_2;
	} else if (stage_id == 901) {
		return pvp_lv_1;
    } else if (stage_id == 931) {
        return pvp_monster_;
	} else {
		return pvp_lv_0;
	}
}

uint16_t get_pvp_stage_id(uint32_t pvp_lv) {
	switch (pvp_lv) {
		case pvp_lv_2:
		case pvp_summon_mode_2:
			return 907;
		case pvp_monster_:
			return 931;
		case pvp_monster_game:
		case pvp_monster_practice:
			return 939;
		case pvp_contest_last:
			return 941;
		case pvp_16_contest:
			return 964;
		case pvp_red_blue_1:
			return 711;
		case pvp_red_blue_2:
			return 712;
		case pvp_red_blue_3:
			return 713;
		case pvp_team_score:
			return 907;
		case pvp_ghost_game:
			return 979;
		case pvp_team_contest:
			return 720;
		case pvp_temporary_team:
			return 1000;
		case pvp_dragon_ship:
			return 734;
		default:
			break;
	}
	return 920;
}

bool is_lv_matched(uint16_t lv1, uint16_t lv2)
{
	return (::abs(lv1 - lv2) >= 5);
}
//----------------------------------------------------------------


/**
  * @brief cancel a battle
  * @return 0 on success, -1 on error
  */
int cancel_battle_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	uint8_t reason = *body;

	//DEBUG_LOG("CNCL BTL\t[uid=%u reason=%d]", p->id, reason);
	KDEBUG_LOG(p->id, "CNCL BTL\t[uid=%u reason=%d]", p->id, reason);

    uint32_t btl_interval = get_now_tv()->tv_sec - p->score.start_tm;
	if (reason) {
		// sync player's attr with Online
		int idx = sizeof(btl_proto_t);
		pack_h(pkgbuf, p->lv, idx);
		pack_h(pkgbuf, p->exp, idx);
		pack_h(pkgbuf, p->allocator_exp, idx);
        if (p->btl->is_battle_pvp() || p->btl->is_battle_tower() || p->btl->is_battle_tmp_team_pvp()) {
            btl_interval = 0;
        }

		if (is_in_not_use_double_tm_time(p)) {
			pack_h(pkgbuf, p->dexp_tm, idx);
		} else {
	        if ( btl_interval < p->dexp_tm) {
	            pack_h(pkgbuf, p->dexp_tm - btl_interval, idx);
	        } else {
	            pack_h(pkgbuf, static_cast<uint32_t>(0), idx);
	        }
		}
		
		pack(pkgbuf, p->once_bit, sizeof(p->once_bit), idx);
		TRACE_LOG("once bit :%x %x %x %x ", p->once_bit[0], p->once_bit[1]
			, p->once_bit[2], p->once_bit[10]);
		pack_h(pkgbuf, p->coins, idx);
		pack_h(pkgbuf, p->hp, idx);
		pack_h(pkgbuf, p->mp, idx);
		pack_h(pkgbuf, p->honor, idx);
		pack_h(pkgbuf, p->exploit, idx);
		pack_h(pkgbuf, p->skill_point, idx);
		pack_h(pkgbuf, p->fumo_points_end, idx);
		pack_h(pkgbuf, p->fumo_tower_top, idx);
		pack_h(pkgbuf, p->fumo_tower_used_tm, idx);

		uint32_t going_flag = (p->btl->is_battle_started() && !p->btl->is_battle_ended()) ;
		pack_h(pkgbuf, going_flag, idx);

		pack(pkgbuf, p->unique_item_bit, sizeof(p->unique_item_bit), idx);
        TRACE_LOG("unique_item_bit : [%x]", p->unique_item_bit[8]);
		pack_h(pkgbuf, static_cast<uint32_t>(p->avg_lag), idx);

		TRACE_LOG("pvp out %u %u %u %u %u", 
			p->continue_win, p->honor, p->exploit, p->max_conti_win_times, p->fumo_points_end);

		//sync player's clothes info
		for ( uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++) {
			player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
			pack(pkgbuf, clothes, sizeof(player_clothes_base_info_t), idx);
		}
		idx += (max_clothes_on - p->clothes_info.clothes_cnt) * sizeof(player_clothes_base_info_t);

		//sync player's summon monster
		#pragma pack(1)
		struct summon_info_pack_t {
			uint32_t	summon_tm;
			uint16_t	summon_lv;
			uint32_t	summon_exp;
			uint16_t	fight_value;
		} summon_info_pack;
		#pragma pack()
		if (p->my_summon) {
			summon_info_pack.summon_tm = p->my_summon->summon_info.mon_tm;
			summon_info_pack.summon_lv = p->my_summon->lv;
			summon_info_pack.summon_exp = p->my_summon->exp;
			summon_info_pack.fight_value = p->my_summon->summon_info.fight_value;
		} else {
			summon_info_pack.summon_tm = 0;
		}
		pack(pkgbuf, &(summon_info_pack), sizeof(summon_info_pack), idx);
		uint32_t count = 0;
		//save 4 leng pkg buf to pack the count later
		int idx_tmp = idx;
		idx += 4;
		for (BufSkillMap::iterator it = p->buf_skill_map.begin(); it != p->buf_skill_map.end(); ++it) {

			uint32_t usedtime = 0;
			if (!is_in_not_use_double_tm_time(p)) {
				if (p->btl->battle_mode()!= btl_mode_lv_matching) {
					if (it->second.start_tm && it->second.buf_type != 1400) {
						usedtime = get_now_tv()->tv_sec - it->second.start_tm;
					} else if (it->second.start_tm == 0) {
						usedtime = get_now_tv()->tv_sec - p->btl->btl_start_tm;
					}
				}
			}

			uint32_t lefttime = it->second.buf_duration > usedtime ? 
									it->second.buf_duration - usedtime : 0;
			TRACE_LOG("%u %u %u %u %u ", it->second.buf_type, it->second.per_trim, usedtime, lefttime, it->second.buf_duration);
			pack_h(pkgbuf, it->second.buf_type, idx);
			pack_h(pkgbuf, lefttime, idx);
			pack_h(pkgbuf, it->second.per_trim, idx);
			pack_h(pkgbuf, it->second.mutex_type, idx);
			count ++;
		}
		pack_h(pkgbuf, count, idx_tmp);
		init_btl_proto_head(pkgbuf, p->waitcmd, idx);
		send_to_player(p, pkgbuf, idx, 1);
	}
	//save buff to db
	save_buff_to_db(p);

	TRACE_LOG("CNCL BTL\t[uid=%u reason=%d dexp time=%u, interval=%u]", 
        p->id, reason, p->dexp_tm, btl_interval);
	del_player(p);
	return 0;
}

void calc_pvp_attr_change(Player * p)
{
	double S_hp = (p->lv + 4) * 75;
	double M_hp = p->hp;

	double t_h_r = M_hp / S_hp;

	if (t_h_r < 1.0) {
		p->hp = p->hp * 3;
	} else if (t_h_r < 4.0) {
		p->hp = ((double)p->hp * ( 3.5 - 0.5 * t_h_r)); 
	} else {
		p->hp = (p->hp * 3) / 2;
	}

	p->maxhp = p->hp;

}

void calc_pvp_attr_change_2(Player * p)
{
	int pre_hp = p->hp;
	double S_hp = (p->lv + 4) * 75;
	double M_hp = p->hp;

	double t_h_r = M_hp / S_hp;

	if (t_h_r < 1.0) {
		p->hp = p->hp * 7;
	} else if (t_h_r < 4.0) {
		p->hp = ((double)p->hp * ( 8.0 - t_h_r)); 
	} else {
		p->hp = p->hp * 4;
	}
	p->hp_max_pvp_trim = p->hp - pre_hp;
	//p->maxhp = p->hp;
}


struct pvp_matching_info_t {
	uint32_t pvp_lv;
	uint32_t room_session;
}__attribute__((packed));

//bool check_can_in_btl(Player * p)
//{
//	if (p->btl) {
//		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
//	}
//	return true;
//}

int pvp_matching_battle_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
//	check_can_in_btl(p);
//	pvp_matching_info_t * info = reinterpret_cast<pvp_matching_info_t*>(body);
//	const stage_t * s = get_pvp_stage_id(info->pvp_lv);
//	assert(s != NULL);
//	player_attr_t * attr = reinterpret_cast<player_attr_t*>(body + sizeof(pvp_matching_info_t));
//	p->init_player_attr(attr);
//
//
//	Battle * btl = BattleManager::instance()->find_pvp_matching_btl(attr->room_id);
//
//	if (btl) {
//		Battle->player_enter(p);
//	} else {
//		btl = create_pvp_mat
//	}
//
	return 0;
}

int pvp_invite_battle_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	return 0;
}

int pve_matching_battle_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	return 0;
}

int pve_invaite_battle_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	return 0;
}


/**
* @brief join a battle team
*/
int tmp_team_pvp_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	if (p->btl) {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}
	
	tmp_team_pvp_attr_t* p_attr = reinterpret_cast<tmp_team_pvp_attr_t*>(body);
	player_attr_t* attr = reinterpret_cast<player_attr_t*>(p_attr + 1);
    uint32_t pvp_lv = attr->stage;
    attr->stage = get_pvp_stage_id(pvp_lv);

	p->init_tmp_team_attr(p_attr);
	p->init_player_attr(attr);

	calc_pvp_attr_change_2(p);

	const stage_t* s = get_stage(attr->stage, attr->difficulty);
	if (s == 0) {
		WARN_LOG("NO SUCH STAGE\t[%u %u %u]", p->id, attr->stage, attr->difficulty);
		do_stat_log_boost_num(0);
		
		send_header_to_player(p, p->waitcmd, cli_err_battle_no_such_stage, 1);
		del_player(p);
		return 0;
	}
    uint32_t fact_room = p_attr->room_id & 0x0000ffff;
	TRACE_LOG("%u %u", p_attr->room_id, fact_room);
	Battle* p_btl = BattleManager::get_instance()->get_battle(fact_room);
	if (p_btl && !(p_btl->is_battle_tmp_team_pvp())) {
		//DEBUG_LOG("BTL NOT MATCHED\t[%u %u]", p->id, p_attr->room_id);
		KDEBUG_LOG(p->id, "BTL NOT MATCHED\t[%u %u]", p->id, p_attr->room_id);
		send_header_to_player(p, p->waitcmd, cli_err_battle_pvp_room_not_exist, 1);
		del_player(p);
		return 0;
	}

	if (!p_btl || p_btl->is_player_full() || p_btl->is_battle_started()) { 
        if (p_attr->pvp_type != pvp_type_radom && fact_room != 0) {
            ERROR_LOG("invitee no fount room [%x]", p_attr->room_id);
            send_header_to_player(p, p->waitcmd, cli_err_battle_pvp_room_not_exist, 1);
            del_player(p);
            return 0;
        }
		// begin level matching
		//uint32_t pvp_lv = get_pvp_lv(attr->stage);
		if (Battle::create_battle(p, btl_mode_temp_team_pvp, get_stage(attr->stage, stage_mode_easy), 1, 1)) {
			p->btl->set_player_num_limit(p_attr->tmp_team_member_cnt + 1);
			p->set_player_in_team_btl();
			//
			p->btl->set_pvp_lv(pvp_lv);
			p->btl->set_pvp_type(p_attr->pvp_type);
			
            p->btl->global_room_id();
			Battle::send_tmp_team_match_rsp(p, btl_mode_temp_team_pvp);
            if (p_attr->pvp_type == pvp_type_radom) {
                //notify to battle switch
				p->btl->local_flg = false;
                notify_create_new_pvp_room(p, p->btl->id(), pvp_lv, p_attr->match_key);
            }
            TRACE_LOG("created pvp btl %u %u %p %u ", p->id, attr->stage, p, pvp_lv);
		}
		return 0;
	}

	if (p_attr->tmp_team_leader == p->id) {
	//第二队加入，增加战斗人数上限
		p->team = player_team_2;
	} else {
		p_btl->set_tmp_team_pvp_team_side(p);
	}
	
	p_btl->add_player(p, p->team);
	p->set_player_in_team_btl();
	
	if (p_attr->tmp_team_leader == p->id) {
	//第二队加入，增加战斗人数上限
		p->btl->set_player_num_limit(p->btl->get_player_num_limit() + p_attr->tmp_team_member_cnt - 1);
	}
	
    if (p_attr->pvp_type == pvp_type_radom) { //is auto lv match pvp_mode
        // nodify to battle switch
        notify_join_pvp_room(p, p_btl->id(), pvp_lv);
    }
	p_btl->send_tmp_team_match_rsp(p, btl_mode_temp_team_pvp);
	if (p_btl->is_player_full()) {
		p_btl->start_battle();
	}
	return 0;

}


/**
* @brief join a battle team
*/
int lv_matching_battle_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	if (p->btl) {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}
	
	hot_join_team_attr_t* p_attr = reinterpret_cast<hot_join_team_attr_t*>(body);
	player_attr_t* attr = reinterpret_cast<player_attr_t*>(p_attr + 1);
    uint32_t pvp_lv = attr->stage;
    attr->stage = get_pvp_stage_id(pvp_lv);
	p->init_player_attr(attr);

	calc_pvp_attr_change_2(p);

	const stage_t* s = get_stage(attr->stage, attr->difficulty);
	if (s == 0) {
		WARN_LOG("NO SUCH STAGE\t[%u %u %u]", p->id, attr->stage, attr->difficulty);
		do_stat_log_boost_num(0);
		
		send_header_to_player(p, p->waitcmd, cli_err_battle_no_such_stage, 1);
		del_player(p);
		return 0;
	}
    uint32_t fact_room = p_attr->room_id & 0x0000ffff;
	TRACE_LOG("%u %u", p_attr->room_id, fact_room);
	Battle* p_btl = BattleManager::get_instance()->get_battle(fact_room);
	if (p_btl && !(p_btl->is_battle_pvp())) {
		//DEBUG_LOG("BTL NOT MATCHED\t[%u %u]", p->id, p_attr->room_id);
		KDEBUG_LOG(p->id, "BTL NOT MATCHED\t[%u %u]", p->id, p_attr->room_id);
		send_header_to_player(p, p->waitcmd, cli_err_battle_pvp_room_not_exist, 1);
		del_player(p);
		return 0;
	}

	if (!p_btl || p_btl->is_player_full() || p_btl->is_battle_started()) { 
        if (p_attr->btl_type != 0 && fact_room != 0) {
            ERROR_LOG("invitee no fount room [%x]", p_attr->room_id);
            send_header_to_player(p, p->waitcmd, cli_err_battle_pvp_room_not_exist, 1);
            del_player(p);
            return 0;
        }
		// begin level matching
		//uint32_t pvp_lv = get_pvp_lv(attr->stage);
		if (Battle::create_battle(p, btl_mode_lv_matching, get_stage(attr->stage, stage_mode_easy), 1, 1)) {
			p->btl->set_player_num_limit(2);
			p->set_player_in_team_btl();
			//
			p->btl->set_pvp_lv(pvp_lv);
			p->btl->set_pvp_type(p_attr->btl_type);
			
            p->btl->global_room_id();
			Battle::send_lv_match_rsp(p, btl_mode_lv_matching, p_attr->btl_type);
            if (p_attr->btl_type == 0) {
                //notify to battle switch
				p->btl->local_flg = false;
                notify_create_new_pvp_room(p, p->btl->id(), pvp_lv);
            }

//			if (pvp_lv == pvp_16_contest) {
//
//			}

			//todo: other
            if (attr->stage < 901) {
                do_stat_item_log(stat_log_stage_login, attr->stage, p->role_type, 1);
                do_stat_item_log_5(stat_log_stage_login_2, 0, p->id, attr->stage);
                do_stat_item_log_2(stat_log_user_stage_login, attr->lv, attr->stage << 8, 1); 
                //do_stat_item_log_4(stat_log_stage_difficulty_login, attr->lv, attr->stage << 8, 1);
            } else {
                do_stat_item_log_2(stat_log_userlv_spe_stage_login, attr->lv, (attr->stage - 900) << 8, 1);

                uint32_t stat_buf[3] = {1, 0, 0};
                if (attr->stage == 921) {
                    do_stat_item_log_4(0x09503015, 0, 0, stat_buf, sizeof(stat_buf));
                }
                if (attr->stage == 931) {
                    do_stat_item_log_4(0x09540000, 0, attr->role_type, stat_buf, sizeof(stat_buf));
                }

            }

            TRACE_LOG("created pvp btl %u %u %p %u ", p->id, attr->stage, p, pvp_lv);
		}
		return 0;
	}

	p->set_player_in_team_btl();
	Player* first_p = p_btl->get_some_player();
    if (p_attr->btl_type == 0) { //is auto lv match pvp_mode
        // nodify to battle switch
        notify_join_pvp_room(p, p_btl->id(), pvp_lv);
    }
	p_btl->set_pvp_matched(p, p_attr->btl_type, first_p);

    if (attr->stage == 931) {
        uint32_t stat_buf[3] = {1, 0, 0};
        do_stat_item_log_4(0x09540000, 0, attr->role_type, stat_buf, sizeof(stat_buf));
    }
	return 0;

}

/**
  * @brief restart a pve battle
  */
int player_btl_restart_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	restart_player_attr_t* p_attr = reinterpret_cast<restart_player_attr_t*>(body);
	bool restart = true;
	if (p->btl->is_team_btl()) {
		WARN_LOG("%u cannot restart battle:%u", p->id, p->btl->id());
		return 0;
	}
	const stage_t* s = p->btl->stage();
	if (s == 0) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_no_such_stage, 1);
	}
	
	uint32_t btl_diff = p->btl->difficulty();
	KDEBUG_LOG(p->id, "BTL RESTART\t[uid=%u stageid=%u diff=%u] experience deduct [%u]",p->id, s->id, btl_diff, p_attr->half_exp);

	if (p_attr->stage_id) {
		s = get_stage(p_attr->stage_id, p_attr->difficulty);
		if (!s) {
			return send_header_to_player(p, p->waitcmd, cli_err_battle_no_such_stage, 1);
		}
		restart = false;
		btl_diff = p_attr->difficulty;
	}
	
	p->restart_init(p_attr);
    TRACE_LOG("btl reatart uid=[%u], stageid=[%u] double exp time=[%u]", p->id, s->id, p->dexp_tm);

	if (Battle::create_battle(p, btl_mode_pve, s, btl_diff)) {
		int idx = sizeof(btl_proto_t);
		uint32_t battle_mode = btl_mode_pve;
		pack_h(pkgbuf, battle_mode, idx); //battle_mode
		init_btl_proto_head(pkgbuf, p->waitcmd, idx);
		send_to_player(p, pkgbuf, idx, 1);

		p->btl->set_all_player_skills_ready();
		p->btl->start_battle(restart);

        if (s->id < 901) {
            do_stat_item_log(stat_log_stage_login, s->id, p->role_type, 1);
            do_stat_item_log_5(stat_log_stage_login_2, 0, p->id, s->id);
            do_stat_item_log_2(stat_log_user_stage_login, p->lv, s->id << 8, 1);

            uint32_t stat_buf[4] = {0, 0, 0, 0}; 
            stat_buf[btl_diff - 1] = 1;
            do_stat_item_log_4(stat_log_stage_difficulty_login, p->lv, s->id << 8, stat_buf, sizeof(stat_buf));
        } else {
            do_stat_item_log_2(stat_log_userlv_spe_stage_login, p->lv, (s->id - 900) << 8, 1);
        }
        if (p->btl->is_battle_tower()) {
            do_stat_item_log_2(stat_log_fumo_enter_new, 1, 0, 1);
        }

	}

	del_buff_when_condition(p, buff_stage_restart_delete_reason );
	del_player_all_aura(p);
	return 0;
}


int sync_process_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t process;
	unpack_h(body, process, idx);

	idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, process, idx);
	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), p->id, cli_proto_sync_process_noti, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	p->btl->send_to_btl(pkgbuf, idx, p);
	return 0;
}

/**
  * @brief Attend a pve battle
  */
int pve_battle_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	player_attr_t* attr = reinterpret_cast<player_attr_t*>(body);
	CHECK_VAL_EQ(bodylen, (uint32_t)(sizeof(player_attr_t) + attr->buf_skill_num * sizeof(buf_skill_rsp_t) + attr->skills_num * sizeof(player_skills_info_t) + attr->home_btl_pet_num*sizeof(uint32_t)));

	KDEBUG_LOG(p->id, "PVE\t[uid=%u stg=%u diff=%u ]", p->id, attr->stage, attr->difficulty);

	const stage_t* s = get_stage(attr->stage, attr->difficulty);
	if (s == 0) {
		WARN_LOG("NO SUCH STAGE\t[%u %u %u]", p->id, attr->stage, attr->difficulty);
		do_stat_log_boost_num(0);
		return send_header_to_player(p, p->waitcmd, cli_err_battle_no_such_stage, 1);
	}

	// init player's attr
	p->init_player_attr(attr);
    TRACE_LOG("player base attr [%u] [%u/%u %u/%u]", p->id, p->hp, p->max_hp(), p->mp, p->max_mp());
	p->team = player_team_1;

	if ((p->lv < s->lv[0]) || (p->lv > s->lv[1])) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_lv_not_match, 1);
	}

	
	if (Battle::create_battle(p, btl_mode_pve, s, attr->difficulty)) {
		int idx = sizeof(btl_proto_t);
		pack_h(pkgbuf, btl_mode_pve, idx); //battle_mode
		init_btl_proto_head(pkgbuf, p->waitcmd, idx);
		send_to_player(p, pkgbuf, idx, 1);
		
        if (attr->stage < 901) {
            do_stat_item_log(stat_log_stage_login, attr->stage, p->role_type, 1);
            do_stat_item_log_5(stat_log_stage_login_2, 0, p->id, attr->stage);
            do_stat_item_log_2(stat_log_user_stage_login, attr->lv, attr->stage << 8, 1);
			if (attr->difficulty < 5) {
	            uint32_t stat_buf[4] = {0, 0, 0, 0}; 
	            stat_buf[attr->difficulty - 1] = 1;
	            do_stat_item_log_4(stat_log_stage_difficulty_login, attr->lv, attr->stage << 8, stat_buf, sizeof(stat_buf));
			}
        } else {
            do_stat_item_log_2(stat_log_userlv_spe_stage_login, attr->lv, (attr->stage - 900) << 8, 1);
			do_stat_item_log_5(stat_log_specal_stage, attr->stage - 900, 1, 0);
        }
		
		p->btl->start_battle();
		p->pre_skill_point = p->skill_point;
	}


	if (p->btl->is_battle_tower()) {
		do_stat_item_log(stat_log_fumo_enter, 0, 0, 1);
        do_stat_item_log_2(stat_log_fumo_enter_new, 1, 0, 1);
	}
	return 0;
}

/**
  * @brief a player informs server that he/she is ready to fight
  * @return 0 on success, -1 on error
  */
int player_ready_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	p->btl->set_player_ready(p);
	
	if( p->btl->p_init_proc)
	{
		p->btl->p_init_proc(p->btl, (void*)p);
	}

//	GF_LOG_WRITE(LOG_INFO, "battle_ready", "userid:%u role_reg_time:%u stage_id:%u", p->id, p->role_tm, p->btl->stage()->id);
	return 0;	
}

/**
  * @brief move in a battle
  */
int player_move_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	player_move_t* p_mv = reinterpret_cast<player_move_t*>(body);
	int ret = g_detect_mrg.move_detect(p, p_mv);
	if (ret == -1) {
		send_header_to_player(p, btl_player_detect_player_need_kick, 0, 0);
	}
	p->move(p_mv);
	uint32_t tmp_id = p->id;
	if (is_player_in_watch_list(tmp_id)) {
		KDEBUG_LOG(tmp_id, "P MV %u %u %u %u", p_mv->client_tm, p_mv->client_msec, p_mv->x, p_mv->y);
	}
	return 0;
}

/**
 * @brief player jumps in a battle
 * @return 0 on success, -1 on error
 */
int player_jump_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	player_jump_t* p_jp = reinterpret_cast<player_jump_t*>(body);
	p->jump(p_jp);

	uint32_t tmp_id = p->id;
	if (is_player_in_watch_list(tmp_id)) {
		KDEBUG_LOG(tmp_id, "P JP %u %u", p_jp->x, p_jp->y);
	}
	return 0;
}


int player_stop_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	player_stop_t* p_st = reinterpret_cast<player_stop_t*>(body);
	p->stop(reinterpret_cast<player_stop_t*>(body));

	uint32_t tmp_id = p->id;
	if (is_player_in_watch_list(tmp_id)) {
		KDEBUG_LOG(tmp_id, "P ST %u %u", p_st->x, p_st->y);
	}
	return 0;
}

int player_rsp_lag_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t seq = *reinterpret_cast<uint32_t*>(body);

	if (p->test_start_tv.tv_sec && (p->test_seq == seq)) {
		double tmdiff = timediff(*get_now_tv(), p->test_start_tv);
		p->lag     = tmdiff * 1000 / 2;
		p->avg_lag = (p->avg_lag * (p->test_seq - 1) + p->lag) / p->test_seq;
		memset(&(p->test_start_tv), 0, sizeof(p->test_start_tv));
		DEBUG_LOG("LAG\t[uid=%u lag=%u %u seq=%u]",
					p->id, p->lag, p->avg_lag, p->test_seq);
	} else {
		WARN_LOG("invalid req: uid=%u tm=%u seq=%u %u",
					p->id,  (uint32_t)(p->test_start_tv.tv_sec), p->test_seq, seq);
	}

	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, p->lag, idx);
	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	return 0;
}

int player_attack_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	player_attack_t* p_atk = reinterpret_cast<player_attack_t*>(body);
	uint32_t tmp_id = p->id;
	if (is_player_in_watch_list(tmp_id)) {
		KDEBUG_LOG(tmp_id, "P ATK %u %u %u %u %u", p_atk->client_tm, p_atk->client_msec, p_atk->skill_id, p_atk->x, p_atk->y);
	}
	int ret = g_detect_mrg.atk_detect(p, p_atk);
	if (ret == 1) {
		p->waitcmd = 0;
		return 0;
	} else if (ret == -1) {
		send_header_to_player(p, btl_player_detect_player_need_kick, 0, 0);
		p->waitcmd = 0;
		return 0;
	}
	p->attack(p_atk);
	return 0;
}

int summon_unique_skill_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t skill_id;
	unpack_h(body, skill_id, idx);

	if (p->my_summon == NULL)
	{
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_anger_value, 1);
	}

	if (p->my_summon->summon_info.anger_value < 100) 
	{
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_anger_value, 1);
	}
	p->my_summon->summon_info.trigger_skill_id = skill_id;
    p->my_summon->common_flag_ = get_now_tv()->tv_sec;

	idx = sizeof(btl_proto_t);
	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	p->my_summon->summon_info.anger_value = 0;
	p->my_summon->noti_summon_anger_to_player();
	return 0;
}

int player_use_item_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t itmid;
	unpack_h(body, itmid, idx);

	p->use_item(itmid);
	return 0;
}

int player_pick_item_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t unique_id;
	uint32_t max_bag_grid_count;
	uint32_t mon_tm;

	unpack_h(body, unique_id, idx);
	unpack_h(body, max_bag_grid_count, idx);
	unpack_h(body, mon_tm, idx);

	if (mon_tm != 0) {
		TRACE_LOG("MON PICK>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
	}

	p->cur_picking_summon = mon_tm;

	p->pick_item(unique_id, max_bag_grid_count);

	if (unique_id == 1500344) {
		do_stat_item_log_5(stat_log_lingli_neihe, 0, 1, 0);
	}
	return 0;
}

int player_enter_map_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t mid;
	unpack_h(body, mid, idx);

    if (mid >= 1080101 && mid <= 1080110) {
        map_t* m = p->btl->get_map(get_stage_map_id(mid));
        if ( m && !(m->visited_flg) ) {
            WARN_LOG("CHANGE_FUMOTA\t [userid:%u role_reg_time:%u timestamp:%u map_id:%u]", p->id, p->role_tm,  (uint32_t)time(NULL), mid);
            do_stat_item_log_2(stat_log_fumo_enter_new, mid % 10, 0, 1);
        }

    }

	p->enter_map(mid);
	///////进入特定的地图///////////
	if( p->btl && mid == 1004507 ){
		if(p->btl->battle_section_data == 0){
			add_player_buff(p, 26, 0);//如果齿轮没有被修复则+上暗流的BUFF图标
		}
		p->send_batlle_section();
	}	
	else
	{
		del_player_buff(p, 26, 0);//出了该地图则删除暗流图标
	}
	////////////////////////////////
	return 0;
}


int get_player_attr_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	p->calc_player_attr();
	
	idx += p->pack_player_attr(pkgbuf + idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_get_player_attr, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	return 0;
}

int get_mon_attr_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t monid;
	unpack_h(body, monid, idx);

	idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);

	idx += pack_mon_attr(monid, pkgbuf + idx);

	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_get_mon_attr, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	return 0;
}

/**
* @brief create a battle team
*/
int create_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	btl_team_attr_t* p_team_attr = reinterpret_cast<btl_team_attr_t*>(body);
	player_attr_t* attr = reinterpret_cast<player_attr_t*>(p_team_attr + 1);
	CHECK_VAL_EQ(bodylen, (uint32_t)(sizeof(btl_team_attr_t) + sizeof(player_attr_t) + attr->buf_skill_num * sizeof(buf_skill_rsp_t) + attr->skills_num * sizeof(player_skills_info_t)) + attr->home_btl_pet_num*sizeof(uint32_t));

//	KDEBUG_LOG(p->id, "TEAM PVE\t[uid=%u stg=%u diff=%u ]", p->id, attr->stage, attr->difficulty);
	
	const stage_t* s = get_stage(attr->stage, attr->difficulty);
	if (s == 0) {
		WARN_LOG("NO SUCH STAGE\t[%u %u %u]", p->id, attr->stage, attr->difficulty);
		do_stat_log_boost_num(0);
		
		send_header_to_player(p, p->waitcmd, cli_err_battle_no_such_stage, 1);
		del_player(p);
		return 0;
	}

	// init player's attr
	p->init_player_attr(attr);
	p->team = player_team_1;

	if ((p->lv < s->lv[0]) || (p->lv > s->lv[1])) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_lv_not_match, 1);
	}

	// begin level matching
	if (Battle::create_battle(p, btl_mode_pve, s, attr->difficulty, 1)) {
		
		p->btl->set_player_num_limit(p_team_attr->limit_num);
		p->btl->set_hot_join_flag(p_team_attr->refuse_hot_join);
		//add btl to btlmanager
		//BattleManager::get_instance()->add_battle(p->btl);
		p->set_player_in_team_btl();

		//send rsp to client
		int idx = sizeof(btl_proto_t);
		pack_h(pkgbuf, btl_mode_pve, idx); //battle_mode
		pack_h(pkgbuf, p->btl->global_room_id(), idx);
		init_btl_proto_head(pkgbuf, p->waitcmd, idx);
		send_to_player(p, pkgbuf, idx, 1);

		//notify to battle switch
		if (!p->btl->local_flg) {
		notify_create_new_room(p, p->btl->id(), attr->stage, attr->difficulty, p_team_attr->refuse_hot_join ? 1 : 0);
		}
		//todo: other
        if (attr->stage < 901) {
            do_stat_item_log(stat_log_stage_login, attr->stage, p->role_type, 1);
            do_stat_item_log_5(stat_log_stage_login_2, 0, p->id, attr->stage);
            do_stat_item_log_2(stat_log_user_stage_login, attr->lv, attr->stage << 8, 1);
        } else {
            do_stat_item_log_2(stat_log_userlv_spe_stage_login, attr->lv, (attr->stage - 900) << 8, 1);
        }
	}

	return 0;

}

/**
* @brief join a battle team
*/
int join_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	if (p->btl) {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}
	
	join_team_attr_t* p_attr = reinterpret_cast<join_team_attr_t*>(body);
	player_attr_t* attr = reinterpret_cast<player_attr_t*>(p_attr + 1);
	TRACE_LOG("%u %u", p_attr->room_id, p_attr->room_id & 0x0000ffff);
	Battle* p_btl = BattleManager::get_instance()->get_battle(p_attr->room_id & 0x0000ffff);
	if (!p_btl || p_btl->is_battle_started() || 
		(p_attr->team_leader && (*p_btl->players_.begin())->id != p_attr->team_leader) ||
		!(p_btl->is_battle_pve())) {
		send_header_to_player(p, p->waitcmd, cli_err_battle_room_not_exist, 1);
		del_player(p);
		return 0;
	}
	p->init_player_attr(attr);
	p->team = player_team_1;	

	if (p_btl->is_player_full()) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_room_full, 1);
	}
	
	p_btl->add_player(p);

	p_btl->enter_room(p);

//	p->btl->start_team_battle(p);
	return 0;
}


/**
* @brief hot join a battle team
*/
int hot_join_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	if (p->btl) {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}
	
	hot_join_team_attr_t* p_attr = reinterpret_cast<hot_join_team_attr_t*>(body);
	player_attr_t* attr = reinterpret_cast<player_attr_t*>(p_attr + 1);
	p->init_player_attr(attr);
	p->team = player_team_1;

	const stage_t* s = get_stage(attr->stage, attr->difficulty);
	if (s == 0) {
		WARN_LOG("NO SUCH STAGE\t[%u %u %u]", p->id, attr->stage, attr->difficulty);
		do_stat_log_boost_num(0);
		
		send_header_to_player(p, p->waitcmd, cli_err_battle_no_such_stage, 1);
		del_player(p);
		return 0;
	}
	/*
	if ((p->lv < s->lv[0]) || (p->lv > s->lv[1])) {
		TRACE_LOG("%u %u %u %u", p->lv, s->lv[0], s->lv[1], s->id);
		
		send_header_to_player(p, p->waitcmd, cli_err_battle_lv_not_match, 1);
		del_player(p);
		return 0;
	}
	*/
	TRACE_LOG("%u %u", p_attr->room_id, p_attr->room_id & 0x0000ffff);
	Battle* p_btl = BattleManager::get_instance()->get_battle(p_attr->room_id & 0x0000ffff);
	if (!p_btl || p_btl->is_player_full() || p_btl->is_battle_started()) {
		p_btl = BattleManager::get_instance()->get_radom_battle(attr->stage, attr->difficulty);
		if (!p_btl || !(p_btl->is_battle_pve())) {
	
			//DEBUG_LOG("HOT NO R\t[%u %u]", p->id);
			KDEBUG_LOG(p->id, "HOT NO R\t[%u]", p->id);
			int idx = sizeof(btl_proto_t);
			
			idx += Battle::pack_join_none_btl_rsp(pkgbuf + idx);
			
			init_btl_proto_head(pkgbuf, p->waitcmd, idx);
			send_to_player(p, pkgbuf, idx, 1);
			/*	

			send_header_to_player(p, p->waitcmd, cli_err_battle_no_room, 1);*/
			del_player(p);
			return 0;
		}
	}
	
	p_btl->add_player(p);

	p_btl->enter_room(p);
	return 0;
//	p->btl->start_team_battle(p);
}

/**
* @brief set ready in team
*/
int ready_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->btl) || p->btl->is_battle_started()) {
		TRACE_LOG("%u %u", p->id, p->btl->is_battle_started());
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}

	p->btl->set_team_btl_player_ready(p);
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

/**
* @brief set ready in team
*/
int cancel_ready_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->btl) || p->btl->is_battle_started()) {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}

	p->btl->set_team_btl_player_ready(p, false);
	return send_header_to_player(p, p->waitcmd, 0, 1);;
}

/**
  * @brief set a battle team hot join flag
  */
int set_btl_team_hot_join_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->btl) || p->btl->is_battle_started()) {
		TRACE_LOG("%u %u %u", p->btl ? p->btl->id() : 0, p->id, p->btl->is_battle_started());
		return 0;
	}
	int idx = 0;
	uint32_t flag;
	unpack_h(body, flag, idx);
	
	p->btl->set_hot_join_flag(flag ? battle_forbid_hot_join : battle_permit_hot_join);
	notify_room_hot_join(p, p->btl->id(), flag ? battle_forbid_hot_join : battle_permit_hot_join);
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

/**
  * @brief the team leader kick user from game room
  */
int kick_user_from_room_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t kick_id;
	unpack_h(body, kick_id, idx);
	Player* kick_player = get_player(kick_id);
	
	if (!kick_player || !(p->btl) || p->btl->is_battle_started()) {
		TRACE_LOG("cannot kick[%u] btl:%u leader:%u started:%u", kick_player ? kick_player->id : 0, 
				p->btl ? p->btl->id() : 0, p->id, p->btl->is_battle_started());
		return 0;
	}
	kick_player->notify_be_kicked();
	del_player(kick_player);
	
	//send rsp to client
	idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, kick_id, idx);
	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


/**
* @brief start a battle team
*/
int start_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->btl)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
	}
	if (p->btl->is_battle_started()) {	
		return send_header_to_player(p, p->waitcmd, 0, 1);
	}

	//DEBUG_LOG("TEAM START\t[stgid=%u]", p->btl->stage()->id);
	KDEBUG_LOG(p->id, "TEAM START\t[stgid=%u]", p->btl->stage()->id);
	p->btl->start_team_battle(p);
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

/**
* @brief leave a battle team
*/
int leave_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	send_header_to_player(p, p->waitcmd, 0, 1);
	del_player(p);
	return 0;
}

/*
int db_pve_get_player_btl_info_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	memcpy(p->unique_item_bit, body, sizeof(p->unique_item_bit));

	return 0;
}
*/


enum mechanism_status {
	not_open = 0,
	opening = 1,
	opened   = 2
};

/** 
 * @brief  更改机关状态
 * 
 */
int change_mechanism_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	struct change_mechanism_t {
		uint32_t mechanism_id;
		uint32_t status;

	} __attribute__((packed)) ;

	change_mechanism_t * pkg = reinterpret_cast<change_mechanism_t*>(body);
	if (!(p->btl)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
	}

	if (!p->btl->is_battle_started()) {	
		return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
	}

	p->waitcmd = 0;
	//机关宝箱类状态 0：表示未开启， 1：表示已开启, 2:表示状态更改已完毕
	map_t * map = p->cur_map;
	if (map->get_mechanism(pkg->mechanism_id)){
		Player * mechanism = map->get_mechanism(pkg->mechanism_id);
		if (!mechanism || mechanism->mp != not_open) { //表示机关被开启过了
			ERROR_LOG("player %u open mechanism  been opened by other!", p->id);
		} else {
			if (p->btl && (p->btl->stage()->id == 932 || p->btl->stage()->id==934 )) {
				int x = extra_mechanism_logic(p->btl, mechanism);
				if (x == 3) {
					map->mechansim_status_changed(pkg->mechanism_id, 3, p);
				} else if (x == 1) {
					map->mechansim_status_changed(pkg->mechanism_id, 1, p);
				}
				return 0;
			}
			map->mechansim_status_changed(pkg->mechanism_id, pkg->status, p); 
			p->btl->proc_battle_mechanism_opened(mechanism, p);
		}
	} else {
		ERROR_LOG("no such mechanism");
	}
	return 0;
}

/** 
 * @brief  机关状态更改完成逻辑处理
 * 
 */
int change_mechanism_end_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	
	if (!(p->btl)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
	}

	if (!p->btl->is_battle_started()) {	
		return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
	}

	int idx = 0;
	uint32_t mechanism_id = 0;
	unpack_h(body, mechanism_id, idx);

	map_t * map = p->cur_map;
	if (!map) {
		p->waitcmd = 0;
		return 0;
	}

	if (map->get_mechanism(mechanism_id)){
		Player * mechanism = map->get_mechanism(mechanism_id);
		if (!mechanism || mechanism->mp != opening ) { //机关不能掉落
			ERROR_LOG("mechanism can't drop item");
		} else {
			p->btl->proc_battle_mechanism_logic(mechanism, p);
			/*
			mechanism->mp = droped;
			monster_drop_item(mechanism, p);
			*/
		}
	} else {
		ERROR_LOG("no such mechanism");
	}
	p->waitcmd = 0;
	return 0;
}

int contest_watch_out_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t out_reason = 0;
	int idx =0;
	unpack_h(body, out_reason, idx);
	DEBUG_LOG("WATCHER LEAVE %u", p->id);

	if (!out_reason) {
		idx = sizeof(btl_proto_t);
		init_btl_proto_head(pkgbuf, p->waitcmd, idx);
		send_to_player(p, pkgbuf, idx, 1);
	}
	del_player(p);
	return 0;
}


struct yazhu_rsp_t {
	uint32_t battle_id;
	uint32_t room_id;
	uint32_t yazhu_id;
}__attribute__((packed));

int contest_watch_yazhu_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	yazhu_rsp_t * rsp = reinterpret_cast<yazhu_rsp_t*>(body);
	uint32_t fact_room = (rsp->room_id) & 0x0000ffff;
	uint32_t svr_id = get_server_id();

	uint32_t status = 0;
	uint32_t battle_mode = 3;

	Battle* p_btl = BattleManager::get_instance()->get_battle(fact_room);
		if (!p_btl || !(p_btl->is_battle_pvp()) || rsp->battle_id != svr_id || p_btl->is_battle_ended() ) {
		KDEBUG_LOG(p->id, "BTL NOT MATCHED\t[%u %u]", p->id, rsp->room_id);
		int idx = sizeof(btl_proto_t);
		pack_h(pkgbuf, battle_mode, idx);
		pack_h(pkgbuf, status, idx);
		pack_h(pkgbuf, p->id, idx);
		pack_h(pkgbuf, rsp->yazhu_id, idx);
		init_btl_proto_head(pkgbuf, btl_contest_watch_yazhu, idx);
		send_to_player(p, pkgbuf, idx, 1);
		del_player(p);
		return 0;
	}

	p->watch_btl = p_btl;
	p->btl = p_btl;
	p_btl->watchers_.push_back(p);

	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, battle_mode, idx);
	pack_h(pkgbuf, p_btl->watch_status_, idx);
	pack_h(pkgbuf, p->id, idx);
	pack_h(pkgbuf, rsp->yazhu_id, idx);
	init_btl_proto_head(pkgbuf, btl_contest_watch_yazhu, idx);
	p_btl->send_to_watcher(pkgbuf, idx, p);

	p_btl->send_to_btl(pkgbuf, idx); //tell other someone come in 
	if (p_btl->watch_status_ == 2) {
		for (PlayerVec::iterator it = p_btl->players_.begin(); it != p_btl->players_.end();
				++it) {
			(*it)->noti_hpmp_to_btl();
		}
	}
	return 0;
}

enum summon_ai_t {
	ai_default = 0,
	ai_fury  = 1,
	ai_wood  = 2,
	ai_speed = 3,
	ai_skill_1 = 4,
	ai_skill_2 = 5,
};

int summon_ai_change_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	struct change_ai_rsp_t {
		uint32_t ai_type;
	}__attribute__((packed));

	if (p->my_summon && !p->my_summon->is_dead()) {
		Player * summon = p->my_summon;
		change_ai_rsp_t * rsp = reinterpret_cast<change_ai_rsp_t*>(body);

		switch (rsp->ai_type) {
			case ai_default:
				summon->i_ai->select_ai(SUMMON_MON_AI, 0);
				break;
			case ai_fury:
				summon->i_ai->select_ai(SUMMON_MON_AI, 1);
				break;
			case ai_wood:
				summon->i_ai->select_ai(SUMMON_MON_AI, 2);
				break;
			case ai_speed:
				summon->i_ai->select_ai(SUMMON_MON_AI, 3);
				break;
			case ai_skill_1:
				summon->i_ai->select_ai(SUMMON_MON_AI, 4);
				break;
			case ai_skill_2:
				summon->i_ai->select_ai(SUMMON_MON_AI, 5);
				break;
			default:
				summon->i_ai->select_ai(SUMMON_MON_AI, 0);
				break;
		}
		if (p->btl->pvp_lv() == pvp_dragon_ship && p->btl->stage()->id == 734) {
			if (rsp->ai_type <= 2 || rsp->ai_type > 5) {
				summon->i_ai->select_ai(SUMMON_MON_AI, 3);
			}	
		} else {
			if (rsp->ai_type >= 3 && rsp->ai_type <= 5) {
				summon->i_ai->select_ai(SUMMON_MON_AI, 0);
			}
		}

		int idx = sizeof(btl_proto_t);
		pack_h(pkgbuf, rsp->ai_type, idx);
		init_btl_proto_head(pkgbuf, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}

	return send_header_to_player(p, p->waitcmd, 12348, 1);
}

int player_talk_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	int i = 0;
    userid_t id;
    uint32_t len;
    unpack_h(body, id, i);
    unpack_h(body, len, i);

	if (p->btl) {
		       p->talk(body + 8, len, id);
	}
	p->waitcmd = 0;
	return 0;
}
	
