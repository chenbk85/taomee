/**
 *============================================================
 *  @file      detective.cpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */
 
extern "C" {
#include <libtaomee/log.h>
}

#include <libtaomee++/random/random.hpp>
#include <libtaomee++/inet/byteswap.hpp>

#include "detective.hpp"
#include "player.hpp"
#include "battle_impl.hpp"


using namespace taomee;

Detective_mrg g_detect_mrg;

uint32_t g_pve_boost_tm_diff = 25;
uint32_t g_pvp_boost_tm_diff = 2;

int get_boost_tm_diff(Player* p)
{
	if (p->btl) {
		if (p->btl->is_battle_pve()) {
			return g_pve_boost_tm_diff;
		} else {
			return g_pvp_boost_tm_diff;
		}
	}
	return g_pve_boost_tm_diff;
}

int BoostMoveDetective::update(Player * p, player_move_t * mv)
{
	if (p->detective_info.common_flag1 > 5) {
		return 0;
	}
	uint32_t now_tm = get_now_tv()->tv_sec;
	if (mv->client_tm < p->detective_info.last_client_mv_tm) {
		WARN_LOG("Detect %u boost mv kick lastmvtm %u > nowtm %u", p->id, p->detective_info.last_client_mv_tm, mv->client_tm);
		p->detective_info.common_flag1++;
		return -1;
	}
	
	TRACE_LOG("boost[%u %u %u %u %u %u]",p->id, p->pos().x(), p->pos().y(), mv->x, mv->y, mv->client_tm - p->detective_info.last_client_mv_tm);

	uint32_t client_passed = mv->client_tm - p->detective_info.begin_client_mv_tm;
	uint32_t server_passed = now_tm - p->detective_info.begin_server_mv_tm;
	int total_tm_diff = client_passed > server_passed ? client_passed - server_passed : server_passed - client_passed;
	if (total_tm_diff > get_boost_tm_diff(p)) {
		WARN_LOG("Detect %u boost mv kick tm diff %u", p->id, total_tm_diff);
		p->detective_info.common_flag1++;
		return -1;
	}

	if (p->detective_info.mv_cnt_in_one_sec > 7) {
		WARN_LOG("Detect %u boost mv kick onsec cnt %u", p->id, p->detective_info.mv_cnt_in_one_sec);
		return 1;
	}
	return 0;
}

int SkipMoveDetective::update(Player * p, player_move_t * mv)
{
	if (p->detective_info.common_flag2 > 2) {
		return 0;
	}
	const monster_t* mon = get_monster(p->role_type, p->btl->difficulty());
	float run_ratio = mv->mv_type == 2 ? 1.0 : 1.6;
	int distance_max = mon->spd * run_ratio * 16 / 100 * 2;//1.3;
	uint32_t now_tm = get_now_tv()->tv_sec;
		
	int x_dis = abs(p->pos().x() - static_cast<int>(mv->x));
	int y_dis = abs(p->pos().y() - static_cast<int>(mv->y));
	int dis = sqrt(x_dis * x_dis + y_dis * y_dis); 

	if (p->detective_info.be_hit_tm && now_tm - p->detective_info.be_hit_tm < 10) {
	// - - 
		return 0;//distance_max += p->detective_info.hit_offset_x;
	}

	if (dis > distance_max) {
		if (!(p->detective_info.skip_mv_tm_tmp)) {
			p->detective_info.skip_mv_tm_tmp = now_tm;
			p->detective_info.skip_mv_cnt_tmp = 1;
		} else if (now_tm - p->detective_info.skip_mv_tm_tmp < 10) {
			p->detective_info.skip_mv_cnt_tmp++;
		} else {
			p->detective_info.skip_mv_cnt_tmp = 1;
			p->detective_info.skip_mv_tm_tmp = now_tm;
		}
	}
	if (p->detective_info.skip_mv_cnt_tmp > 3 && p->detective_info.skip_mv_tm_tmp) {
		p->detective_info.skip_mv_total_cnt++;

		p->detective_info.skip_mv_cnt_tmp = 0;
		p->detective_info.skip_mv_tm_tmp = 0;
		WARN_LOG("Detect %u skip mv %u", p->id, p->detective_info.skip_mv_total_cnt);
	}

	if (p->detective_info.skip_mv_total_cnt > 2) {
		WARN_LOG("Detect %u ignore skip %u", p->id, p->detective_info.skip_mv_total_cnt);
		p->detective_info.common_flag2++;
		return 0;
	}
	
	return 0;
}


int AtkMoveDetective::update(Player * p, player_move_t * mv)
{
	detective_info_t* p_info = &(p->detective_info);

	if (!(p_info->last_skill_id) || p_info->last_skill_id % 10 == 4) {
		return 0;
	}
	skill_t* p_pre_skill = g_skills->get_skill(p_info->last_skill_id);
	if (p_pre_skill) {
		if (p_pre_skill->type == normal_skill) {
			uint32_t pass_sec = mv->client_tm - p_info->last_client_atk_tm;
			uint32_t pass_msec = 0;

			if (pass_sec) {
				pass_sec --;
				pass_msec = mv->client_msec + 1000 - p_info->last_client_atk_msec;

				pass_msec += (pass_sec * 1000);
			} else {
				if (mv->client_msec < p_info->last_client_atk_msec) {
					pass_msec = 0;
				} else {
					pass_msec = mv->client_msec - p_info->last_client_atk_msec;
				}
			}

			if (pass_msec < 300) {
				WARN_LOG("Detect %u kick move when atk %u %u", p->id, pass_msec, p_info->last_skill_id);
				return -1;
			}
		}
		return 0;
	}
	return -1;	
}

int BoostNormalAtkDetective::update(Player * p, player_attack_t* atk)
{
	uint32_t now_tm = get_now_tv()->tv_sec;
	skill_t* p_skill = g_skills->get_skill(atk->skill_id);
	if (p_skill) {
		if (p->detective_info.atk_cnt_in_one_sec > 3) {
			if (p->detective_info.common_flag3 == 0) {
				WARN_LOG("Detect %u ignore atk %u", p->id, p->detective_info.atk_cnt_in_one_sec);
				p->detective_info.common_flag3++;
			}
			p->detective_info.atk_cnt_in_one_sec = 0;
			return 1;
		}
		
		uint32_t client_passed = atk->client_tm - p->detective_info.begin_client_atk_tm;
		uint32_t server_passed = now_tm - p->detective_info.begin_server_atk_tm;
		int total_tm_diff = client_passed > server_passed ? client_passed - server_passed : server_passed - client_passed;
		if (total_tm_diff > get_boost_tm_diff(p) || total_tm_diff < (0 - get_boost_tm_diff(p))) {
			if (p->detective_info.common_flag3 == 0) {
				WARN_LOG("Detect %u boost atk kick tm diff %u", p->id, total_tm_diff);
				p->detective_info.common_flag3++;
			}
			return -1;
		}
		return 0;
	}
	return -1;
}

int RobotNormalAtkDetective::update(Player * p, player_attack_t* atk)
{
	detective_info_t* p_info = &(p->detective_info);

	if (p_info->last_skill_id) {
		return 0;
	}

	skill_t* p_cur_skill = g_skills->get_skill(atk->skill_id);
	skill_t* p_pre_skill = g_skills->get_skill(p_info->last_skill_id);
	if (p_cur_skill && p_pre_skill) {
		if (p_cur_skill->type == normal_skill && p_pre_skill->type == normal_skill) {
			uint32_t cd_msec = 300;//p_pre_skill->lv_info[1].cool_down;
			uint32_t pass_sec = atk->client_tm - p_info->last_client_atk_tm;
			uint32_t pass_msec = 0;
			pass_sec --;
			pass_msec = atk->client_msec + 1000 - p_info->last_client_atk_msec;

			pass_msec += (pass_sec * 1000);

			if (pass_msec < cd_msec) {
				WARN_LOG("Detect %u kick robot nor atk msec %u", p->id, pass_msec);
				return -1;
			}

			if (atk->skill_id % 10 != 1 && atk->skill_id - 1 != p_info->last_skill_id) {
				WARN_LOG("Detect %u kick robot nor atk pre %u now %u", p->id, p_info->last_skill_id, atk->skill_id);
				return -1;
			}
			
			return 0;
		}
		return 0;
	}
	return -1;

}

Detective_mrg::Detective_mrg()
{
	MoveDetective* p_detect = new BoostMoveDetective();
	move_detect_vec.push_back(p_detect);
	p_detect = new SkipMoveDetective();
	move_detect_vec.push_back(p_detect);
//	p_detect = new AtkMoveDetective();
//	move_detect_vec.push_back(p_detect);

	AtkDetective* p_atk = new BoostNormalAtkDetective();
	atk_detect_vec.push_back(p_atk);
}

Detective_mrg::~Detective_mrg()
{
	std::vector<MoveDetective*>::iterator it = move_detect_vec.begin();
	for (;it != move_detect_vec.end(); ++it) {
		MoveDetective* p_detect = (*it);
		delete p_detect;
	}
	move_detect_vec.clear();
}

int Detective_mrg::move_detect(Player * p,player_move_t * mv)
{
	int ret = 0;
	begin_move(p, mv);
	std::vector<MoveDetective*>::iterator it = move_detect_vec.begin();
	for (;it != move_detect_vec.end(); ++it) {
		MoveDetective* p_detect = (*it);
		int tmp_ret = 0;
		tmp_ret = p_detect->update(p, mv);
		if ( tmp_ret == -1) {
			ret = tmp_ret;
			break;
		}
		if ( tmp_ret == 1) {
			ret = tmp_ret;
		}
	}
	end_move(p, mv);
	return ret;
}

int Detective_mrg::atk_detect(Player * p,player_attack_t* atk)
{
	int ret = 0;
	begin_atk(p, atk);
	std::vector<AtkDetective*>::iterator it = atk_detect_vec.begin();
	for (;it != atk_detect_vec.end(); ++it) {
		AtkDetective* p_detect = (*it);
		int tmp_ret = 0;
		tmp_ret = p_detect->update(p, atk);
		if ( tmp_ret == -1) {
			ret = tmp_ret;
			break;
		}
		if ( tmp_ret == 1) {
			ret = tmp_ret;
		}
	}
	end_atk(p, atk);
	return ret;
}


void Detective_mrg::begin_move(Player * p, player_move_t * mv)
{
	uint32_t now_tm = get_now_tv()->tv_sec;
	if (!(p->detective_info.begin_client_mv_tm)) {
	//first in
		p->detective_info.begin_client_mv_tm = mv->client_tm;
		p->detective_info.begin_server_mv_tm = now_tm;
	}
	
	if (p->detective_info.last_client_mv_tm ==  mv->client_tm) {
		p->detective_info.mv_cnt_in_one_sec++;
	} else {
		p->detective_info.mv_cnt_in_one_sec = 0;
	}
}

void Detective_mrg::end_move(Player * p, player_move_t * mv)
{
	uint32_t now_tm = get_now_tv()->tv_sec;
	p->detective_info.last_client_mv_tm = mv->client_tm;
	p->detective_info.last_server_mv_tm = now_tm;
}

void Detective_mrg::begin_atk(Player * p, player_attack_t* atk)
{
	uint32_t now_tm = get_now_tv()->tv_sec;
	if (p->detective_info.begin_client_atk_tm == 0) {
		p->detective_info.begin_client_atk_tm = atk->client_tm;
		p->detective_info.begin_server_atk_tm = now_tm;
		p->detective_info.atk_cnt_in_one_sec = 1;
	} else {
		if (atk->client_tm == p->detective_info.last_client_atk_tm ) {
			p->detective_info.atk_cnt_in_one_sec++;
		} else {
			p->detective_info.atk_cnt_in_one_sec = 1;
		}
	}

}

void Detective_mrg::end_atk(Player * p, player_attack_t* atk)
{
	p->detective_info.last_client_atk_tm = atk->client_tm;
	p->detective_info.last_client_atk_msec = atk->client_msec;
	p->detective_info.last_skill_id = atk->skill_id;
}


