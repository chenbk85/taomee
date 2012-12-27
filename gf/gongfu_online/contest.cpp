/*
 * =====================================================================================
 *
 *       Filename:  contest.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/28/2011 12:01:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include <libtaomee/project/utilities.h>

#include "dbproxy.hpp"
#include "contest.hpp"
#include "player.hpp"
#include "cli_proto.hpp"
#include "item.hpp"
#include "kill_boss.hpp"
#include "utils.hpp"
//#include "achievement_logic.hpp"
#include "battle_switch.hpp"
#include "common_op.hpp"
#include "active_config.hpp"
#include "fight_team.hpp"
#include "stage_config.hpp"
#include "global_data.hpp"
#include "achievement.hpp"

pvp_watch_data_t * create_watch_info(player_t * p)
{
//	ERROR_LOG("Player %u CREATE WATCH INFO", p->id);
	return new pvp_watch_data_t();
}

void destory_watch_info(player_t * p, pvp_watch_data_t * info)
{
	if (info) {
//		ERROR_LOG("PLayer %u Destory WATCH INFO", p->id);
		delete info;
	}
}



int join_contest_team_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	//if (!is_player_achievement_data_exist(p, 114)) {
	if ( !p->pTitle->is_get_title(100)) {
		return send_header_to_player(p, p->waitcmd, cli_err_you_are_not_hero, 1);
	}
//	ERROR_LOG("Join %u", p->id);
	uint32_t team_id = 0;
	int idx = 0;
	taomee::unpack(body, team_id, idx);
	if (team_id >= 1 && team_id <= 5) {
		int ifx = 0;
		taomee::pack_h(dbpkgbuf, team_id, ifx);
		taomee::pack(dbpkgbuf, p->nick, 16, ifx);
		return send_request_to_db(p, p->id, p->role_tm, 
				dbproto_join_contest_team, dbpkgbuf, ifx);
	} else {
		return -1;
	}
}


/** 
 * @brief  拉取武斗队伍信息
 * 
 */
int get_contest_team_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
//	ERROR_LOG("GET ALL TEAM %u", p->id);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_contest_team_info, 0, 0);
}

/** 
 * @brief 拉取队伍捐赠信息（包括自己的信息） 
 * 
 */
int get_contest_donate_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
//	ERROR_LOG("GET TEAM DONATE! %u", p->id);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_contest_donate, 0, 0);
}

int get_self_contest_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_self_contest_info, 0, 0);

}

/** 
 * @brief 捐赠复活草 
 * 
 */
int contest_donate_plant_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
//	ERROR_LOG("DONATE PLANT ! %u", p->id);
	int idx = 0;
	uint32_t plant_cnt;
	taomee::unpack(body, plant_cnt, idx);
	//check vaild num
	uint32_t type_a = 1302001;
	uint32_t type_b = 1302000;

    if (p->contest_leader_flag == 1) {
        return send_header_to_player(p, p->waitcmd, cli_err_contest_donate_over, 1);
    }

	uint32_t cnt_1 = p->my_packs->get_item_cnt(type_a);
	uint32_t cnt_2 = p->my_packs->get_item_cnt(type_b);
	if (cnt_1 + cnt_2 < plant_cnt) {
		return send_header_to_player(p, p->waitcmd, cli_err_not_enough_plant, 1);
	}

	uint32_t plant_A = 0;
	uint32_t plant_B = 0;
	uint32_t typecnt = 0;
	idx = 0;
	typecnt = 2;

	if (plant_cnt > cnt_1) {
		plant_A = cnt_1;
		plant_B = plant_cnt - cnt_1;
	} else {
		plant_A = plant_cnt;
		plant_B = 0;
	}
	taomee::pack_h(dbpkgbuf, typecnt, idx);
	taomee::pack_h(dbpkgbuf, type_a, idx);
	taomee::pack_h(dbpkgbuf, plant_A, idx);
	taomee::pack_h(dbpkgbuf, type_b, idx);
	taomee::pack_h(dbpkgbuf, plant_B, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_donate_item, dbpkgbuf, idx);

}

bool is_player_contest_stage_win(player_t * p, uint32_t stage)
{
    return taomee::test_bit_on(p->contest_win_stage, stage); 
}

bool is_valid_time_member(player_t * p)
{
    if (get_team_join_tm(p) < get_stage_end_time(941, 0)) {
        return true;
    }
    return false;
}

bool check_contest_game_limit(player_t* p, uint32_t swap_id)
{
    bool ret = false;
    switch (swap_id) {
        case 1069://资格赛礼包
            if (is_player_finish_stage(p, 935, 1) || is_player_finish_stage(p, 936, 1)
                || is_player_finish_stage(p, 937, 1) || is_player_finish_stage(p, 938, 1)) {
                ret = true;
            }
            break;
        case 1070://学院新秀礼包
            //if (p->pTitle->is_get_title(100)) {
            if (is_player_finish_stage(p, 935, 1) && is_player_finish_stage(p, 936, 1)
                && is_player_finish_stage(p, 937, 1) && is_player_finish_stage(p, 938, 1)) {
                ret = true;
            }
            break;
        case 1071://淘汰赛礼包
            if (p->taotai_info->win_times >= 2) {
                ret = true;
            }
            break;
        case 1072://功夫奇才礼包
            if (p->taotai_info->win_times >= 6) { 
                ret = true;
            }
            break;
        case 1073://学院鼓励礼包
            if (p->taotai_info->win_times >= 12) { 
                ret = true;
            }
            break;
        case 1074://排名争霸赛礼包
            if (p->final_info->win_times >= 1) {
                ret = true;
            }
            break;
        case 1075://初级白大侠是
            if (p->contest_final_rank > 0 && p->contest_final_segment == 1) {
                ret = true;
            }
            break;
        case 1076://高级白大侠是
            if (p->contest_final_rank > 0 && p->contest_final_segment == 2) {
                ret = true;
            }
            break;
        case 1077://高手荣誉包
            if (p->contest_final_rank > 0 && p->contest_final_rank <= 20) {
                ret = true;
            }
            break;
        case 1078://初级天下第一
            if (p->contest_final_rank == 1 && p->contest_final_segment == 1) {
                ret = true;
            }
            break;
        case 1079://高级天下第一
            if (p->contest_final_rank == 1 && p->contest_final_segment == 2) {
                ret = true;
            }
            break;
        case 1080://队长礼包 ---> 优秀团长：本队积分获得侠士团第一名的团
            //if (p->pTitle->is_get_title(105)) {
            if (is_player_team_captain(p) && get_team_active_rank(p) == 1) {
                ret = true;
            }
            break;
        case 1081://队员荣誉礼包 ---> 优秀团员：本队积分获得侠士团前10名所有团员
            //if (is_player_contest_stage_win(p, 1) || is_player_contest_stage_win(p, 2)) {
            if (get_team_active_rank(p) < 11 && is_valid_time_member(p)) {
                ret = true;
            }
            break;
        case 1082://优秀队长礼包 ---> 优秀团队：本队积分获得侠士团前100名 的所有团员
            // if (is_player_contest_stage_win(p, 3) || is_player_contest_stage_win(p, 4)) {
            if (get_team_active_rank(p) < 101 && is_valid_time_member(p)) {
                ret = true;
            }
            break;
       
        default:
            break;
    }
    return ret;
}

int form_contest_game_errcode(uint32_t swap_id)
{
    int ret = 1;
    switch (swap_id) {
        case 1069://资格赛礼包
            ret = 1;
            break;
        case 1070://学院新秀礼包
            break;
        case 1071://淘汰赛礼包
            break;
        case 1072://功夫奇才礼包
            break;
        case 1073://学院鼓励礼包
        case 1074://排名争霸赛礼包
            break;
        case 1075://初级白大侠是
            break;
        case 1076://高级白大侠是
            break;
        case 1077://高手荣誉包
            break;
        case 1078://初级天下第一
            break;
        case 1079://高级天下第一
            break;
        case 1081://队长礼包
            break;
        case 1080://队员荣誉礼包
            break;
        case 1082://优秀队长礼包
            break;
        default:
            break;
    }
    return ret;
}

/**
 * @brief Team member fatch reward
 */
int team_member_reward_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t reward_stage = 0;
    unpack(body, reward_stage, idx); 

    if (reward_stage < 1 || reward_stage > 2) {
        return -1;
    }
    
    if (get_swap_action_times(p, 1018 + reward_stage) >= 1) {
        return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
    }
    ////----------1:taotai 2:advance
    if ( p->contest_win_stage == 0 || !is_player_contest_stage_win(p,reward_stage) ) {
        return send_header_to_player(p, p->waitcmd, cli_err_not_victory_team, 1);
    }

    uint32_t item_cnt = p->lv == max_exp_lv ? 3 : 2;
    uint32_t max_bag  = get_player_total_item_bag_grid_count(p);
    if (item_cnt + p->my_packs->all_items_cnt() > max_bag) {
        return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1);
    }

    uint32_t reward_exp = (p->lv * 1000) + (50000 * reward_stage);

    idx = 0;
    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(reward_stage), idx);
    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(4), idx);
    //功夫豆
    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(3), idx);
    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx);
    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(10000 * (1 + reward_stage)), idx);
    //经验
    if (p->lv == max_exp_lv) {
        taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(2), idx);
        taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(1300316), idx);
        taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx);
    } else {
        taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(3), idx);
        taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(2), idx);
        taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(reward_exp), idx);
    }
    //功夫券
    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(2), idx);
    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(1700061), idx);
    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(300), idx);
    //复活草
    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(2), idx);
    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(1302001), idx);
    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx);

    return send_request_to_db(p, p->id, p->role_tm, dbproto_team_member_reward, dbpkgbuf, idx);
}

int db_join_contest_team_callback(player_t * p, userid_t uid, void * body, uint32_t bodylen,
		uint32_t ret)
{
	CHECK_DBERR(p, ret);
	join_contest_team_rsp_t  * pkg = reinterpret_cast<join_contest_team_rsp_t*>(body);
	int idx = sizeof(cli_proto_t);
	taomee::pack(pkgbuf, pkg->team_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx); 
	return send_to_player(p, pkgbuf, idx, 1);
}

int db_get_contest_team_info_callback(player_t * p, userid_t uid, void * body,uint32_t bodylen,
		uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t team_cnt = 0;
	int ifx = 0;
	taomee::unpack_h(body, team_cnt, ifx);
	CHECK_VAL_EQ(bodylen - 4, team_cnt * sizeof(contest_team_rsp_t));
	static uint8_t tmpbuf[8192];
	int idx = sizeof(cli_proto_t);
	taomee::pack(tmpbuf, team_cnt, idx);
//	ERROR_LOG("get team_cnt =%u", team_cnt);
	for (uint32_t i = 0; i < team_cnt; i++) {
		contest_team_rsp_t * team = reinterpret_cast<contest_team_rsp_t*>((char*)(body) + 4
			   	 + i *sizeof(contest_team_rsp_t));
		taomee::pack(tmpbuf, team->team_id, idx);
	//	ERROR_LOG("team id = %u", team->team_id);
		taomee::pack(tmpbuf, team->leader_id, idx);
		taomee::pack(tmpbuf, team->leader_tm, idx);
		taomee::pack(tmpbuf, team->leader_name, 16, idx); 
		taomee::pack(tmpbuf, team->team_member, idx);
	//	ERROR_LOG("team member = %u", team->team_member);
		taomee::pack(tmpbuf, team->team_plant, idx);
	//	ERROR_LOG("team plant =%u", team->team_plant);
        if ( is_valid_uid(team->leader_tm) ) {
            p->contest_leader_flag = 1;
        }
		if (team->team_id == p->team_id && p->waitcmd == cli_proto_player_gain_item) {
			if (get_unique_item_bitpos(13098)) {
				if (!test_bit_on(p->unique_item_bit, 68)) {
					uint32_t type = 2;
					uint32_t add_num = (team->team_plant / team->team_member);
					if (add_num == 0) {
						add_num ++;
					}
					int ifx = 0;
					uint32_t item_id = 1302001;
					uint32_t max_bag_grid_count = get_player_total_item_bag_grid_count(p); 
					if (!p->my_packs->check_enough_bag_item_grid(p, item_id, add_num)) {
						return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1); 
					}
					pack_h(dbpkgbuf, type, ifx);
					pack_h(dbpkgbuf, item_id, ifx);
					pack_h(dbpkgbuf, add_num, ifx);
					pack_h(dbpkgbuf, max_bag_grid_count, ifx);
					return send_request_to_db(p, p->id, p->role_tm,
						   	dbproto_add_item, dbpkgbuf, ifx);
				} else {
						return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
				}
			} 
		}

		//ERROR_LOG("TEAM ID=%u LEADER_ID=%u LEADER_TM=%u", team->team_id, team->leader_id, team->leader_tm);
		if (team->team_id == p->team_id && team->leader_id == p->id && 
				 team->leader_tm == p->role_tm) {
//			if (!is_player_achievement_data_exist(p, 112)) {
//				do_achieve_type_logic(p, 112);
//                //add_title_interface(p, enum_achieve_title);
//			}
            do_special_title_logic(p, 105);
		}
	}
	init_cli_proto_head(tmpbuf, p, p->waitcmd, idx);
	return send_to_player(p, tmpbuf, idx, 1);
}

int db_get_contest_donate_callback(player_t * p, userid_t uid, void * body, uint32_t bodylen,
		uint32_t ret)
{
	CHECK_DBERR(p, ret);
	contest_donate_rsp_t * pkg = reinterpret_cast<contest_donate_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, pkg->top_cnt * sizeof(top_donate_t) + sizeof(contest_donate_rsp_t));

	int idx = sizeof(cli_proto_t);
	taomee::pack(pkgbuf, pkg->self_team_id, idx);
	taomee::pack(pkgbuf, pkg->self_plant, idx);
	taomee::pack(pkgbuf, pkg->top_cnt, idx);

	for (uint32_t i = 0; i < pkg->top_cnt; i++) {
		top_donate_t * rsp = &pkg->top[i];
		taomee::pack(pkgbuf, rsp->max_id, idx);
		taomee::pack(pkgbuf, rsp->max_tm, idx);
		taomee::pack(pkgbuf, rsp->max_name, 16, idx);
		taomee::pack(pkgbuf, rsp->max_plant, idx);
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

struct donate_item_t {
	uint32_t item_type;
	uint32_t item_cnt;
}__attribute__((packed));

struct  donate_item_rsp_t {
	uint32_t itemtype_cnt;
	donate_item_t donate[];
}__attribute__((packed));

int db_contest_donate_plant_callback(player_t * p, userid_t uid, void * body, uint32_t bodylen,
		uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t type_a = 0;
	uint32_t type_b = 0;
	uint32_t type_c = 0;
	uint32_t type_d = 0;
	int ifx = 0;
	taomee::unpack(p->session, type_a, ifx);
	taomee::unpack(p->session, type_b, ifx);
	taomee::unpack(p->session, type_c, ifx);
	taomee::unpack(p->session, type_d, ifx);
	
	int idx =sizeof(cli_proto_t);
	taomee::pack(pkgbuf, type_a, idx);
	taomee::pack(pkgbuf, type_b, idx);
	taomee::pack(pkgbuf, type_c, idx);
	taomee::pack(pkgbuf, type_d, idx);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int db_donate_item_callback(player_t *p, userid_t uid, void *body, uint32_t bodylen,
		uint32_t ret)
{
	CHECK_DBERR(p, ret);
	donate_item_rsp_t * pkg = reinterpret_cast<donate_item_rsp_t *>(body);
	CHECK_VAL_EQ(bodylen - 4, pkg->itemtype_cnt * sizeof(donate_item_t));

	uint32_t donate_cnt = 0;
	if (p->waitcmd == cli_proto_contest_donate_plant) {
		memset(p->session, 0, 30);
		p->sesslen = 0;
	}

	for (uint32_t i = 0; i < pkg->itemtype_cnt; i++){
		donate_item_t * item = &(pkg->donate[i]); 
		if ((item->item_type == 1302000 || item->item_type == 1302001)) {
			donate_cnt += item->item_cnt;
			pack(p->session, item->item_type, p->sesslen);
			pack(p->session, item->item_cnt, p->sesslen);
		}
		p->my_packs->del_item(p, item->item_type, item->item_cnt, channel_string_other);
	}

	if (p->waitcmd == cli_proto_contest_donate_plant) {
		int idx = 0;
		taomee::pack_h(dbpkgbuf, donate_cnt, idx);
		return send_request_to_db(p, p->id, p->role_tm, 
			dbproto_contest_donate_plant, dbpkgbuf, idx);
	}
	return 0;

}

struct self_contest_t {
	uint32_t team_id;
	uint32_t plant_cnt;
    uint32_t win_stage;
}__attribute__((packed));

/** 
 * @brief 比武大会拉自己信息 
 * 
 */
int db_get_self_contest_info_callback(player_t *p, userid_t uid, void *body, uint32_t bodylen,
		uint32_t ret)
{
	CHECK_DBERR(p, ret);
	self_contest_t * pkg = reinterpret_cast<self_contest_t*>(body);

	int idx = sizeof(cli_proto_t);
	taomee::pack(pkgbuf, pkg->team_id, idx);
	p->team_id = pkg->team_id;
    p->contest_win_stage = pkg->win_stage;
	int j = 0;
	int ifx = idx + 4;
	for (int i = 935; i <= 938; i++) {
		kill_boss_data * kill = get_player_kill_boss_data(p, i, 1); 
		if (kill) {
			j++;
			taomee::pack(pkgbuf, kill->boss_id_, ifx);
			taomee::pack(pkgbuf, kill->kill_time_, ifx);	
		}
	}
	taomee::pack(pkgbuf, j, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, ifx);
	send_to_player(p, pkgbuf, ifx, 1);

    //send_request_to_db(p, p->id, p->role_tm, dbproto_get_taotai_game_info, 0, 0);
    idx = 0;
    pack_h(dbpkgbuf, static_cast<uint32_t>(2), idx); //Three section of contest
    send_request_to_db(p, p->id, p->role_tm, dbproto_get_pvp_game_info, dbpkgbuf, idx);

    return 0;
}

/**
 * @brief  Team member fatch reward callback
 */
int db_team_member_reward_callback(player_t *p, userid_t uid, void *body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    uint32_t win_stage = 0, item_cnt = 0; ////1:taotai 2:contest
    int idx = 0;
    taomee::unpack_h(body, win_stage, idx);
    taomee::unpack_h(body, item_cnt, idx);
    TRACE_LOG("team member reward [%u] [%u]",uid, item_cnt);
    CHECK_VAL_EQ(bodylen, item_cnt * sizeof(pack_item_info_t) + 8); 

    idx = sizeof(cli_proto_t);
    taomee::pack(pkgbuf, item_cnt, idx);
    pack_item_info_t* item_unit = reinterpret_cast<pack_item_info_t*>((uint8_t*)body + 8);
    for (uint32_t i = 0; i < item_cnt; i++) {
        taomee::pack(pkgbuf, item_unit[i].item_id, idx);
        taomee::pack(pkgbuf, item_unit[i].item_cnt, idx);
        TRACE_LOG("team member reward [%u %u]", item_unit[i].item_id, item_unit[i].item_cnt);
    }
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    send_to_player(p, pkgbuf, idx, 1); 

    add_swap_action_times(p, 1018 + win_stage);
    return 0;
}

void add_player_do_times(player_t* p, uint32_t pvp_type)
{
    if (pvp_type == 0) {
        p->taotai_info->do_times += 1;
    } else if (pvp_type == 1) {
        p->advance_info->do_times += 1;
    } else if (pvp_type == 2) {
        p->final_info->do_times += 1;
    }
}
void add_player_win_times(player_t* p, uint32_t pvp_type)
{
    if (pvp_type == 0) {
        p->taotai_info->win_times += 1;
    } else if (pvp_type == 1) {
        p->advance_info->win_times += 1;
    } else if (pvp_type == 2) {
        p->final_info->win_times += 1;
    }
}

uint32_t get_taotai_can_join_times(player_t* p)
{
    uint32_t total_times = 3 + (is_vip_player(p) ? 2:0);
    if (!is_player_finish_stage(p, 935, 1) && !is_player_finish_stage(p, 936, 1)
        && !is_player_finish_stage(p, 937, 1) && !is_player_finish_stage(p, 938, 1)) {
        total_times = 0;
    }
    return total_times;
}

bool is_contest_first_stage_over()
{
    uint32_t cur_time = get_now_tv()->tv_sec;

    uint32_t beg_tm = get_utc_second_ex("2011/10/14 12:59:00");
    uint32_t end_tm = get_utc_second_ex("2011/10/16 15:01:00");

    if (cur_time > beg_tm && cur_time < end_tm) {
        return false;
    }
    return true;
}

bool is_contest_second_stage_over()
{
    uint32_t cur_time = get_now_tv()->tv_sec;

    uint32_t beg_tm = get_utc_second_ex("2011/10/14 12:59:00");
    uint32_t end_tm = get_utc_second_ex("2011/10/16 15:01:00");

    if (cur_time > beg_tm && cur_time < end_tm) {
        return false;
    }
    return true;
}

bool is_contest_final_stage_over()
{
    uint32_t cur_time = get_now_tv()->tv_sec;

    uint32_t beg_tm = get_utc_second_ex("2011/10/22 12:59:00");
    uint32_t end_tm = get_utc_second_ex("2011/10/23 15:01:00");

    if (cur_time > beg_tm && cur_time < end_tm) {
        return false;
    }
    return true;
}

uint32_t get_advance_can_join_times(player_t *p) {
	uint32_t total_times = 0;

	if (p->taotai_info->win_times >= 1) {
		total_times = 8;
		if (is_vip_player(p)) {
			total_times += 4;
		}

		if (is_player_achievement_data_exist(p, 108)) {
			total_times += 1;
		}
	}

	return total_times;
}

uint32_t get_final_can_join_times(player_t *p) 
{
    return p->taotai_info->win_times >= 2 ? 1:0;
    //return p->advance_info->win_times > 1 ? 1:0;
}

bool is_hundred_hunter(player_t *p, uint32_t id)
{
    switch (id) {
        case 1005:
            return p->contest_final_rank <= 10 && p->contest_final_rank > 0;
            break;
        case 1006:
            return p->contest_final_rank <= 20 && p->contest_final_rank > 10;
            break;
        case 1007:
            return p->contest_final_rank <= 50 && p->contest_final_rank > 20;
            break;
        case 1008:
            return p->contest_final_rank <= 100 && p->contest_final_rank > 50;
            break;
        default:
            return false;
            break;
    }
}

/**
 * @brief 比武大会
 */
int get_taotai_game_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = sizeof(cli_proto_t);
    /*
    uint32_t total_times = 5 + (is_vip_player(p) ? 1:0) + (p->pTitle->is_get_title(16) ? 1:0);
    if (!is_player_finish_stage(p, 935, 1) && !is_player_finish_stage(p, 936, 1)
        && !is_player_finish_stage(p, 937, 1) && !is_player_finish_stage(p, 938, 1)) {
        total_times = 0;
    }*/

    uint32_t total_times = get_taotai_can_join_times(p);
    uint32_t day_do_times = get_swap_action_times(p, 1068);
    TRACE_LOG("Player [%u] taotai game information[%u %u %u]",p->id, total_times, 
        day_do_times, p->taotai_info->win_times);
    pack(pkgbuf, total_times, idx);
    pack(pkgbuf, day_do_times, idx);
    pack(pkgbuf, p->taotai_info->win_times, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
    //return send_request_to_db(p, p->id, p->role_tm, dbproto_get_taotai_game_info, 0, 0);
}

int get_advance_game_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	int idx = sizeof(cli_proto_t);
	uint32_t total_times = get_advance_can_join_times(p);
    TRACE_LOG("Player [%u] advance game information[%u %u %u]",p->id, total_times, 
        p->advance_info->do_times, p->advance_info->win_times);
    pack(pkgbuf, total_times, idx);
    pack(pkgbuf, p->advance_info->do_times, idx);
    pack(pkgbuf, p->advance_info->win_times, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int get_final_game_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	int idx = sizeof(cli_proto_t);
	uint32_t total_times = get_final_can_join_times(p);
    TRACE_LOG("Player [%u] final game information[%u %u %u]",p->id, total_times, 
        p->final_info->do_times, p->final_info->win_times);
    pack(pkgbuf, total_times, idx);
    pack(pkgbuf, p->final_info->do_times, idx);
    pack(pkgbuf, p->final_info->win_times, idx);

    uint32_t add_score = p->game_flower / 5;
    uint32_t plus_score = p->final_info->win_times * 3 + add_score;
    uint32_t duce_score = p->final_info->do_times - p->final_info->win_times;
    int32_t total_score = plus_score - duce_score;

    TRACE_LOG("Player [%u] final game score[%u %u]", p->id, add_score, total_score);
    pack(pkgbuf, total_score, idx);
    pack(pkgbuf, add_score, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int get_final_game_rank_list_cmd(player_t *p, uint8_t *body, uint32_t bodylen)
{
    uint32_t rank = 0;
	int idx = 0;
	taomee::unpack(body, rank, idx);
    if (rank < 1 || rank > 2) {
        return -1;
    }

    if (g_pvp_game_cache.rank[rank - 1].timestamp != 0 && get_now_tv()->tv_sec < g_pvp_game_cache.rank[rank - 1].timestamp + 300) {
        return send_final_game_rank_list_to_player(p, rank);
    }

    idx = 0;
    taomee::pack_h(dbpkgbuf, rank, idx);
    return send_request_to_db(p, p->id, p->role_tm, dbproto_get_final_game_rank_list, dbpkgbuf, idx);
}

int player_gain_item_cmd(player_t *p, uint8_t *body, uint32_t bodylen)
{
	if (!p->team_id) {
		return send_header_to_player(p, p->waitcmd, cli_err_join_no_team, 1);
	}
	if (get_unique_item_bitpos(13098)) {
		if (!test_bit_on(p->unique_item_bit, 68)) {
			return send_request_to_db(p, p->id, p->role_tm, dbproto_get_contest_team_info, 0, 0);
		} else {
			return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
		}
	} else {
		p->waitcmd = 0;
		return 0;
	}
}

int send_final_game_rank_list_to_player(player_t* p, uint32_t rank)
{
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, g_pvp_game_cache.rank[rank - 1].cnt, idx);
    for (uint32_t i = 0; i < g_pvp_game_cache.rank[rank - 1].cnt; i++) {
        final_game_score_t * score = &(g_pvp_game_cache.rank[rank - 1].score[i]);
        TRACE_LOG("PLAYER [%u %u %u] [%u]", score->userid, score->roletm, score->lv, score->score);

        pack(pkgbuf, score->userid, idx);
        pack(pkgbuf, score->nick, sizeof(score->nick), idx);
        pack(pkgbuf, score->lv, idx);
        pack(pkgbuf, score->score, idx);
    }
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int db_get_final_game_rank_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    int u_idx = 0;
    uint32_t rank = 0, final_num = 0;
    unpack_h(body, rank, u_idx);
    unpack_h(body, final_num, u_idx);
    CHECK_VAL_EQ(bodylen, final_num * (sizeof(final_game_score_t)) + 8);

    g_pvp_game_cache.rank[rank - 1].timestamp   = get_now_tv()->tv_sec;
    g_pvp_game_cache.rank[rank - 1].cnt         = final_num;

    final_game_score_t * score = reinterpret_cast<final_game_score_t*>((uint8_t*)body + 8);
    for (uint32_t i = 0; i < final_num; i++) {
        TRACE_LOG("PLAYER [%u %u %u] [%u]", score[i].userid, score[i].roletm, score[i].lv, score[i].score);

        g_pvp_game_cache.rank[rank - 1].score[i] = score[i];
    }

    return send_final_game_rank_list_to_player(p, rank);
}

int db_get_taotai_game_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    pvp_game_data_t *rsp = reinterpret_cast<pvp_game_data_t *>(body);

    //KDEBUG_LOG(p->id,"GET PVP MONSTAR GAME\t[%u %u]", rsp->do_times, rsp->win_times);
    p->taotai_info->do_times = rsp->do_times;
    p->taotai_info->win_times = rsp->win_times;

	//send_request_to_db(p, p->id, p->role_tm, dbproto_get_advance_game_info, 0, 0);

    return 0;
}


int db_get_advance_game_info_callback(player_t * p, userid_t id, void * body, uint32_t bodylen, 
		uint32_t ret) {
	CHECK_DBERR(p, ret);
	//_LOG("GET ADVANCE GAME CALLBACK %u", p->id);
	pvp_game_data_t * rsp = reinterpret_cast<pvp_game_data_t *>(body);
	p->advance_info->do_times = rsp->do_times;
	p->advance_info->win_times = rsp->win_times;
	return 0;
}

void set_player_final_game_info(player_t* p)
{
    for (int i = 1; i >= 0; i--) {
        for (uint32_t j = 0; j < g_pvp_game_cache.rank[i].cnt; j++) {
            final_game_score_t * score = &(g_pvp_game_cache.rank[i].score[j]);
            if (p->id == score->userid && p->role_tm == score->roletm) {
                p->contest_final_rank       =   j + 1;
                p->contest_final_segment    =   i + 1;
                return;
            }
        }
    }

	if (p->contest_final_rank >= 1 && p->contest_final_segment == 1 && get_now_active_data_by_active_id(35)) {
		proc_ranker_range_achieve_logic(p, ranker_contest_final, 1, p->contest_final_rank - 1);
	}
}

int save_pvp_game_data_to_player(player_t * p, uint32_t pvp_type, uint32_t done, uint32_t win, uint32_t flower, uint32_t rank, uint32_t segment)
{
    if (pvp_type == 0) {
        p->taotai_info->do_times = done;
        p->taotai_info->win_times = win;
    } else if (pvp_type == 1) {
        p->advance_info->do_times = done;
        p->advance_info->win_times = win;
    } else if (pvp_type == 2) {
        p->final_info->do_times = done;
        p->final_info->win_times = win;
        p->game_flower = flower;
        p->contest_final_rank = rank;
        p->contest_final_segment = segment;
    } else {
    }
    return 0;
}

int db_get_pvp_game_info_callback(player_t * p, userid_t id, void * body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);

    uint32_t contest_num = 0;
    int idx = 0;
    unpack_h(body, contest_num, idx);
    TRACE_LOG("Player [%u] contest number [%u]", id, contest_num);
    CHECK_VAL_EQ(bodylen, (contest_num + 1) * (sizeof(pvp_game_data_t) + 16) + 4);

    uint32_t pvp_type = 0, dotimes = 0, wintimes = 0, flower = 0, rank = 0, segment = 0;
    for (uint32_t i = 0; i <= contest_num; i++) {
        unpack_h(body, pvp_type, idx);
        unpack_h(body, dotimes, idx);
        unpack_h(body, wintimes, idx);
        unpack_h(body, flower, idx);
        unpack_h(body, rank, idx);
        unpack_h(body, segment, idx);
        KDEBUG_LOG( p->id,"PVP GAME DATA\t[%u] [%u %u] [%u]", pvp_type, dotimes, wintimes, flower);
        save_pvp_game_data_to_player(p, pvp_type, dotimes, wintimes, flower, rank, segment);
    }
    set_player_final_game_info(p);
    return 0;
}

int save_taotai_game_data(player_t* p, bool win_flag)
{
    TRACE_LOG("save_pvp_monster_game_data:%u", p->id);
    int idx = 0;
    pack_h(dbpkgbuf, p->id, idx);
    pack_h(dbpkgbuf, p->role_tm, idx);
    //pack_h(dbpkgbuf, p->role_type, idx);
    //pack(dbpkgbuf, p->nick, sizeof(p->nick), idx);
    //pack_h(dbpkgbuf, p->team_number, idx);
    p->taotai_info->do_times += 1;
    if ( win_flag ) {
        pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx);
        p->taotai_info->win_times += 1;
    } else {
        pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);
    }
    KDEBUG_LOG(p->id,"FINISH PVP MONSTAR GAME\t[%u %u]", p->taotai_info->do_times, p->taotai_info->win_times);
    //send_request_to_db(0, p->id, p->role_tm, dbproto_save_taotai_game_data, dbpkgbuf, idx);
    return 0;
}

int save_advance_game_data(player_t* p, bool win_flag)
{
   // ERROR_LOG("save_pvp_advance_game_data:%u", p->id);
    int idx = 0;
    pack_h(dbpkgbuf, p->id, idx);
    pack_h(dbpkgbuf, p->role_tm, idx);
    if ( win_flag ) {
        pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx);
        p->advance_info->win_times += 1;
    } else {
		p->advance_info->do_times += 1;
        pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);
    }
    KDEBUG_LOG(p->id,"FINISH PVP ADVANCE GAME\t[%u %u]", p->advance_info->do_times, p->advance_info->win_times);
    //send_request_to_db(0, p->id, p->role_tm, dbproto_save_advance_game_data, dbpkgbuf, idx);
    return 0;
}

int pack_player_pvp_game_times(player_t* p, uint32_t pvp_type, uint8_t * buf)
{
    int idx = 0;
    if (pvp_type == 0) {
        pack_h(buf, p->taotai_info->do_times, idx);
        pack_h(buf, p->taotai_info->win_times, idx);
    } else if (pvp_type == 1) {
        pack_h(buf, p->advance_info->do_times, idx);
        pack_h(buf, p->advance_info->win_times, idx);
    } else if (pvp_type == 2) {
        pack_h(buf, p->final_info->do_times, idx);
        pack_h(buf, p->final_info->win_times, idx);
    } else {
        pack_h(buf, static_cast<uint32_t>(0), idx);
        pack_h(buf, static_cast<uint32_t>(0), idx);
    }
    return idx;
}

int save_pvp_game_data(player_t* p, uint32_t pvp_type, bool win_flag)
{
    int idx = 0;
    uint8_t tmpbuf[1024] = {0};
    pack_h(tmpbuf, p->id, idx);
    pack_h(tmpbuf, p->role_tm, idx);
    pack(tmpbuf, p->nick, sizeof(p->nick), idx);
    pack_h(tmpbuf, static_cast<uint32_t>(p->lv), idx);
    pack_h(tmpbuf, pvp_type, idx);
    if ( win_flag ) {
        //pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx);
        add_player_win_times(p, pvp_type);
    } else {
        //pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);
        add_player_do_times(p, pvp_type);
    }

    idx += pack_player_pvp_game_times(p, pvp_type, tmpbuf + idx);
	pack_h(tmpbuf, p->game_flower, idx);

    KDEBUG_LOG(p->id,"FINISH PVP GAME\t[%u] [%u] [%u %u]",
        pvp_type, win_flag, p->taotai_info->do_times, p->taotai_info->win_times);
    send_request_to_db(0, p->id, p->role_tm, dbproto_save_pvp_game_data, tmpbuf, idx);

    //记录一天进行的次数
    if (!win_flag) {
        add_swap_action_times(p, 1068);
    }

    return 0;
}

int save_pvp_game_flower(player_t* p, uint32_t pvp_type, uint32_t flower)
{
    int idx = 0;
    pack_h(dbpkgbuf, p->id, idx);
    pack_h(dbpkgbuf, p->role_tm, idx);
    pack_h(dbpkgbuf, pvp_type, idx);
    pack_h(dbpkgbuf, flower, idx);

    p->game_flower += flower;

    KDEBUG_LOG(p->id,"FINISH PVP [%u] GAME\t[%u]",pvp_type, flower);
    return send_request_to_db(0, p->id, p->role_tm, dbproto_save_pvp_game_flower, dbpkgbuf, idx);
}
/*
int db_save_taotai_game_data_callback(player_t *p, userid_t uid, void *body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    pvp_game_data_t *rsp = reinterpret_cast<pvp_game_data_t *>(body);

    p->taotai_info.do_times = rsp->do_times;
    p->taotai_info.win_times = rsp->win_times;

    int idx = sizeof(cli_proto_t);

}
*/

int get_contest_pvp_list_cmd(player_t * p, uint8_t * body, uint32_t len)
{
	get_pvp_room_info(p, pvp_contest_final);
	return 0;
}

int db_contest_yazhu(player_t * p)
{
	uint32_t flower_fumo = 1500363;
	uint32_t flower_shop = 1700082;
	uint32_t cnt_1 = p->my_packs->get_item_cnt(flower_fumo);
	uint32_t cnt_2 = p->my_packs->get_item_cnt(flower_shop);
	if (!cnt_1 && !cnt_2) {
	//	memset(p->watch_info, 0, sizeof(p->watch_info));
		if (p->watch_info) {
			pvp_watch_data_t * info = p->watch_info;
			destory_watch_info(p, info);
			p->watch_info = 0;
		}
		return send_header_to_player(p, p->waitcmd, cli_err_not_enough_flower, 1);
	}
	uint32_t item_A = 0;
	uint32_t item_B = 0;
	int idx = 0;
	int typecnt = 2;
	if (cnt_1 > 0) {
		item_A = 1;
		item_B = 0;

		p->my_packs->del_item(p, flower_fumo, 1, channel_string_other);
	} else  {
		item_A = 0; 
		item_B = 1;
		p->my_packs->del_item(p, flower_shop, 1, channel_string_other);
	}


	taomee::pack_h(dbpkgbuf, typecnt, idx);
	taomee::pack_h(dbpkgbuf, flower_fumo, idx);
	taomee::pack_h(dbpkgbuf, item_A, idx);
	taomee::pack_h(dbpkgbuf, flower_shop, idx);
	taomee::pack_h(dbpkgbuf, item_B, idx);
	return send_request_to_db(0, p->id, p->role_tm, dbproto_donate_item, dbpkgbuf, idx);
}


int contest_pvp_yazhu_cmd(player_t * p, uint8_t * body, uint32_t len)
{
	int idx = 0;

	uint32_t battle_id;
	uint32_t room_id;
	uint32_t watch_id;


	unpack(body, battle_id, idx);
	unpack(body, room_id, idx);
	unpack(body, watch_id, idx);

	uint32_t flower_fumo = 1500363;
	uint32_t flower_shop = 1700082;
	uint32_t cnt_1 = p->my_packs->get_item_cnt(flower_fumo);
	uint32_t cnt_2 = p->my_packs->get_item_cnt(flower_shop);
	if (!cnt_1 && !cnt_2) {
	//	memset(p->watch_info, 0, sizeof(p->watch_info));
		return send_header_to_player(p, p->waitcmd, cli_err_not_enough_flower, 1);
	}

	if ((battle_id < 0) || (room_id < 0) || !(is_valid_uid(watch_id))) {
		return -1;
	}

	if (p->battle_grp) {
		return -1;
	} else {
		p->watch_info = create_watch_info(p);
		p->watch_info->battle_id = battle_id;
		p->watch_info->room_id = room_id;
		p->watch_info->watch_id = watch_id;
		btlsvr_player_contest_yazhu(p);
	}
	return 0;
}

int contest_cancel_yazhu_cmd(player_t *p, uint8_t * body, uint32_t len)
{
	if (p->battle_grp && p->watch_info) {
		btlsvr_contest_cancel_yazhu(p, 0);
	} else {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}
	return 0;
}

int get_city_battle_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t city_team_flag = 1173;

	int idx = sizeof(cli_proto_t);

	uint32_t team = player_city_team(p->id, p->role_tm);
	if (!get_swap_action_times(p, city_team_flag)) {
		team = 0;
	}

	time_t now = get_now_tv()->tv_sec;

	struct tm * now_time = localtime(&now);
	uint32_t city_btl_end  = 7;

	uint32_t city_btl_going = 8 ;
	uint32_t status = 0;
	if (get_now_active_data_by_active_id(city_btl_going)) {
		if (now_time->tm_min < 40) {
			status = 1;
		} else {
			status = 2;
		}
	}else if (get_now_active_data_by_active_id(city_btl_end)) {
		status = 3;
	}

	uint32_t win = rand()% + 5;
	if (status == 3) {
		win = get_city_win_team();
	}

	pack(pkgbuf, team, idx);
	pack(pkgbuf, status, idx);
	pack(pkgbuf, win, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int auto_join_contest_cmd(player_t * player, uint8_t * body, uint32_t bodylen)
{
	if (g_server_config[4].value ) {

		if (player->contest_session)  {
			return send_header_to_player(player, player->waitcmd, cli_err_all_ready_in_group, 1);
		}

		if ( player->exploit < 1000) {
			ERROR_LOG("%u JOIN Contest Group %lu already or not enough exploit %u",
					player->id, player->contest_session, player->exploit);
			return send_header_to_player(player, player->waitcmd, cli_err_no_enough_exploit, 1);
		}

//		if (!get_now_active_data_by_active_id(14)) {
//			return send_header_to_player(player, player->waitcmd, cli_err_not_in_contest_time, 1);
//		}
//
		do_stat_log_universal_interface_2(0x09526206, 0, player->id, 1);
		return auto_join_contest_group(player);
	}
	return send_header_to_player(player, player->waitcmd, cli_err_system_repair, 1);
}


int list_contest_group_info_cmd(player_t * player, uint8_t * body, uint32_t bodylen)
{
	if (player->contest_session) {
		return list_contest_group_info(player);;
	}

	return send_header_to_player(player, player->waitcmd, cli_err_no_group_now, 1);
}

int leave_contest_group_cmd(player_t * player, uint8_t * body, uint32_t bodylen)
{
	if (player->contest_session) {
		do_stat_log_universal_interface_2(0x09526207, 0, player->id, 1);
		player->contest_session = 0;
		return leave_contest_group(player);
	}
	return send_header_to_player(player, player->waitcmd, cli_err_no_group_now, 1);
}

int contest_guess_champion_cmd(player_t * player, uint8_t * body, uint32_t bodylen)
{
	if (player->contest_session) {
		if (player->exploit < 200) {
			return send_header_to_player(player, player->waitcmd, cli_err_no_enough_exploit, 1);
		}
		int idx = 0;
		uint32_t guess_id = 0;
		uint32_t guess_tm = 0;
		unpack(body, guess_id, idx);
		unpack(body, guess_tm, idx);
		return player_guess_contest_champion(player, guess_id, guess_tm);	
	}
	return send_header_to_player(player, player->waitcmd, cli_err_no_group_now, 1);
}

int list_all_contest_player_cmd(player_t * player, uint8_t * body, uint32_t bodylen)
{
	if (player->contest_session) {
		return list_all_contest_player(player);
	}
	return send_header_to_player(player, player->waitcmd, cli_err_no_group_now, 1);
}

int list_all_passed_contest_cmd(player_t * player, uint8_t * body, uint32_t bodylen)
{
	if (player->contest_session) {
		return list_all_passed_contest(player);
	}
//	ERROR_LOG("PLAYER %u is not in a contest group!", player->id);
	return send_header_to_player(player, player->waitcmd, cli_err_no_group_now, 1);
}


int	re_join_contest_cmd(player_t * player, uint8_t * body, uint32_t bodylen) 
{
	int idx = sizeof(cli_proto_t);
	uint32_t sess_flag = 0;
	if (player->contest_session) {
		sess_flag = 1;
	}
	pack(pkgbuf, sess_flag, idx);
	init_cli_proto_head(pkgbuf, player, player->waitcmd, idx);
	return send_to_player(player, pkgbuf, idx, 1);
}
