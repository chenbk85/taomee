#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee/project/utilities.h>
#include <libtaomee++/utils/md5.h>
#include <kf/utils.hpp>
#include <kf/fight_team.hpp>
#include <libtaomee++/time/time.hpp>
extern "C" {
#include <async_serv/mcast.h>
}
using namespace taomee;

#include "utils.hpp"
#include "message.hpp"
#include "player.hpp"
#include "switch.hpp"
#include "login.hpp"
#include "global_data.hpp"
#include "mcast_proto.hpp"

#include "fight_team.hpp"
#include "common_op.hpp"
#include "active_config.hpp"
#include "stat_log.hpp"
#include "chat.hpp"
#include "cachesvr.hpp"
#include "rank_top.hpp"

//-----------------------------------------------------------
// static function declarations
//-----------------------------------------------------------
//--------------------------------------------------------------------

int get_team_tax(player_t * p, uint32_t contribute_flag);

int del_team_member(player_t* p, uint32_t uid, uint32_t role_tm)
{
	bool find_flg = false;
	if (p->team_info.team_id) {
		for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
			if (p->team_info.team_member[i].uid == uid && 
				p->team_info.team_member[i].role_tm == role_tm) {
				find_flg = true;
			}
			if (find_flg) {
				if (i != (p->team_info.team_member_cnt - 1)) {
					p->team_info.team_member[i] = p->team_info.team_member[i + 1];
					//p->team_info.team_member[i].uid = p->team_info.team_member[i + 1].uid;
					//p->team_info.team_member[i].role_tm = p->team_info.team_member[i + 1].role_tm;
				}
			}
		}
	}
	if (find_flg) {
		p->team_info.team_member_cnt--;

		KDEBUG_LOG(p->id, "DEL MEMBER\t[%u %u %u %u]", p->team_info.team_id, uid, role_tm, p->team_info.team_member_cnt);
	} else {
		KDEBUG_LOG(p->id, "DEL MEMBER ERR\t[%u %u %u %u]", p->team_info.team_id, uid, role_tm, p->team_info.team_member_cnt);
	}
	return 0;
}

bool is_have_team_member(player_t* p, uint32_t uid, uint32_t role_tm)
{
	for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
		if (p->team_info.team_member[i].uid == uid && 
			p->team_info.team_member[i].role_tm == role_tm) {
			return true;
		}
	}
	return false;
}


int add_team_member(player_t* p, uint32_t uid, uint32_t role_tm)
{
	if (is_have_team_member(p, uid, role_tm)) {
		return 0;
	}
	if (p->team_info.team_id) {
		if (p->team_info.team_member_cnt < max_team_member_cnt && p->team_info.team_member_cnt ) {
			uint32_t i = p->team_info.team_member_cnt;
			p->team_info.team_member[i].uid = uid;
			p->team_info.team_member[i].role_tm = role_tm;
			p->team_info.team_member[i].level = 0;
			p->team_info.team_member_cnt ++;
			KDEBUG_LOG(p->id, "ADD MEMBER\t[%u %u %u]", p->team_info.team_id, uid, role_tm);
			return 0;
		}
	} 
	KDEBUG_LOG(p->id, "ADD MEMBER ERR\t[%u %u %u %u]", p->team_info.team_id, uid, role_tm, p->team_info.team_member_cnt);
	return -1;
}



bool is_player_team_captain(player_t* p)
{
	if (p->team_info.team_id && p->team_info.captain_uid == p->id && 
		p->team_info.captain_role_tm == p->role_tm) {
		return true;
	}
	return false;
}

bool is_player_have_team(player_t* p)
{
	if (p->team_info.team_id) {
		return true;
	}
	return false;
}

bool is_player_have_access_to_del(player_t* p, uint32_t del_uid, uint32_t del_role_tm)
{
	if (!(is_player_have_team(p))) {
		return false;
	}
	if (p->id == del_uid && p->role_tm == del_role_tm && is_player_team_captain(p)) {
	//can not delete captain it's self
		return false;
	}
	if ((p->id == del_uid && p->role_tm == del_role_tm) ||
		is_player_team_captain(p)) {
		return true;
	}
	return false;
}

uint32_t get_team_join_tm(player_t* p) 
{
	if (p->team_info.team_id) {
		for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
			if (p->team_info.team_member[i].uid == p->id && 
				p->team_info.team_member[i].role_tm == p->role_tm) {
				return p->team_info.team_member[i].join_tm;
			}
		}
	}
	return 0;
}

uint32_t get_team_active_rank(player_t* p)
{
    if (!is_player_have_team(p)) return 999;
    for (uint32_t i = 0; i < g_team_active_chache.cnt; i++) {
        if (p->team_info.team_id == g_team_active_chache.elem[i].id) {
            return g_team_active_chache.elem[i].active_index;
        }
    }
    return 999;
}

bool judge_if_can_get_fight_team_reward(player_t* p, uint32_t swap_id)
{
	uint32_t score_index = p->team_info.score_index;
	if (!(p->team_info.team_id) || score_index == 0) {
		return false;
	}
	uint32_t join_tm = get_team_join_tm(p);

	
	swap_action_data* p_res = g_swap_action_mrg.get_swap_action(1187);
	if (!p_res) {
		return false;
	}
	if (join_tm > p_res->start_end_time[0]) {
		return false;
	}
	bool is_captain = (p->team_info.captain_uid == p->id &&  p->team_info.captain_role_tm == p->role_tm);

	if (swap_id == 1187) {
		return true;
	}

	if (swap_id == 1188) {
		if (score_index <= 100) {
			return true;
		}
	} else if (swap_id == 1189) {
		if (score_index <= 10) {
			return true;
		}
 	} else if (swap_id == 1190) {
		if (score_index <= 100 && is_captain) {
			return true;
		}
	} else if (swap_id == 1191) {
		if (score_index <= 10 && is_captain) {
			return true;
		}
	}
	return false;
}


int db_chat_across_team_member_changed(player_t* p, uint8_t* body)
{
	chat_across_team_member_changed_rsp_t info = { 0 };
	int idx = 0;
	unpack(body, info.type, idx);
	unpack(body, info.team_id, idx);
	unpack(body, info.del_uid, idx);
	unpack(body, info.del_role_tm, idx);

	if (info.type == 0) {
	//delete team member
		if (p->id == info.del_uid && p->team_info.team_id == info.team_id
			&& p->role_tm == info.del_role_tm) {
			KDEBUG_LOG(p->id, "%u reset", p->team_info.team_id);
			p->team_info.reset();
			report_user_base_info_to_chat_svr(p, talk_type_fight_team, p->team_info.team_id);
		} else {
		//delete member from team
			del_team_member(p, info.del_uid, info.del_role_tm);
		}
	} else {
	//add member to team
		add_team_member(p, info.del_uid, info.del_role_tm);
	}
	return 0;
}


int pack_del_team_member_pkg(uint8_t* buf, uint32_t cli_cmd, uint32_t team_id, uint32_t del_uid, uint32_t del_role_tm, player_t *p) 
{
	int idx = sizeof(cli_proto_t);
	pack(buf, team_id, idx);
	pack(buf, del_uid, idx);
	pack(buf, del_role_tm, idx);
	init_cli_proto_head(buf, p, cli_cmd, idx);
	return idx;
}
/*
void send_del_team_member(player_t* p, uint32_t team_id, uint32_t del_uid, uint32_t del_role_tm)
{
	player_t* to = get_player(del_uid);
	int off_len = sizeof(userid_t);
	
	(*(uint32_t*)pkgbuf) = del_uid;
	int pkglen = pack_del_team_member_pkg(pkgbuf + off_len, 
		cli_proto_send_del_team_member, team_id, del_uid, del_role_tm, p);

	KDEBUG_LOG(p->id, "SEND DEL TEAM MEMBER[%u %u]", p->id, del_uid);
	
	if (to) {
		send_to_player(to, pkgbuf + off_len, pkglen, 0);
	} else {
		chat_across_svr_2(p, pkgbuf,  pkglen + off_len);
	}
}
*/
int pack_team_member_changed_pkg(uint8_t* buf, uint32_t cli_cmd, uint32_t team_id, uint32_t del_uid, uint32_t del_role_tm, uint32_t type, player_t *p) 
{
	int idx = sizeof(cli_proto_t);
	pack(buf, type, idx);
	pack(buf, team_id, idx);
	pack(buf, del_uid, idx);
	pack(buf, del_role_tm, idx);
	pack(buf, (uint32_t)get_server_id(), idx);
	pack(buf, p->nick, sizeof(p->nick), idx);
	init_cli_proto_head(buf, p, cli_cmd, idx);
	return idx;
}

void send_team_member_changed(player_t* p, uint32_t type, uint32_t team_id, uint32_t uid, uint32_t role_tm)
{
	
	int off_len = sizeof(userid_t);

	for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
		//if (p->id == uid) {
		//	continue;
		//}
		
		player_t* to = get_player(p->team_info.team_member[i].uid);

		KDEBUG_LOG(p->id, "SEND TEAM MEMBER CHANGED to %u [%u %u %u %u %u]", p->team_info.team_member[i].uid
			, i, type, p->id, uid, role_tm);
		(*(uint32_t*)pkgbuf) = p->team_info.team_member[i].uid;
		int pkglen = pack_team_member_changed_pkg(pkgbuf + off_len, 
			cli_proto_send_team_member_changed, team_id, uid, role_tm, type, p);
		if (to) {
			send_to_player(to, pkgbuf + off_len, pkglen, 0);
		} else {
			chat_across_svr_2(p, pkgbuf,  pkglen + off_len);
		}
	}
}


void on_team_member_changed(player_t* p, uint32_t type, uint32_t team_id, uint32_t uid, uint32_t role_tm)
{
	send_team_member_changed(p, type, team_id, uid, role_tm);

	fight_team_info_t tmp_team = p->team_info;
	for (uint32_t i = 0; i < tmp_team.team_member_cnt; i++) {
		
		player_t* to = get_player(tmp_team.team_member[i].uid);
		if (to) {
			if (type) {
				add_team_member(to, uid, role_tm);
			} else {
				del_team_member(to, uid, role_tm);
				if (uid == to->id) {
					KDEBUG_LOG(to->id, "%u reset", to->team_info.team_id);
					to->team_info.reset();//must after send_team_member_changed
					report_user_base_info_to_chat_svr(p, talk_type_fight_team, p->team_info.team_id);
				}
			}
		}
	}
	
}

int pack_team_member_onoff_pkg(uint8_t* buf, uint32_t cli_cmd, uint32_t team_id, uint32_t del_uid, uint32_t del_role_tm, uint32_t type, player_t *p) 
{
	int idx = sizeof(cli_proto_t);
	pack(buf, type, idx);
	pack(buf, team_id, idx);
	pack(buf, del_uid, idx);
	pack(buf, del_role_tm, idx);
	pack(buf, (uint32_t)get_server_id(), idx);
	init_cli_proto_head(buf, p, cli_cmd, idx);
	return idx;
}


void send_team_member_onoff(player_t* p, uint32_t type, uint32_t team_id, uint32_t uid, uint32_t role_tm)
{

	if (p->team_info.login_flg && type) {
		return;
	}
	p->team_info.login_flg = 1;
	
	int off_len = sizeof(userid_t);

	for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
		if (p->id == p->team_info.team_member[i].uid) {
			continue;
		}
//		KDEBUG_LOG(p->id, "SEND TEAM MEMBER ONOFF[%u %u %u %u]", type, p->id, uid, role_tm);
		player_t* to = get_player(p->team_info.team_member[i].uid);
		(*(uint32_t*)pkgbuf) = p->team_info.team_member[i].uid;
		int pkglen = pack_team_member_onoff_pkg(pkgbuf + off_len, 
			cli_proto_send_team_member_onoff, team_id, uid, role_tm, type, p);
		if (to) {
			send_to_player(to, pkgbuf + off_len, pkglen, 0);
		} else {
			chat_across_svr_2(p, pkgbuf,  pkglen + off_len);
		}
	}
}

int pack_team_info_changed_pkg(uint8_t* buf, uint32_t cli_cmd, uint32_t type, uint32_t score, player_t *p) 
{
	int idx = sizeof(cli_proto_t);
	pack(buf, p->nick, sizeof(p->nick), idx);
	pack(buf, type, idx);
	pack(buf, score, idx);
	init_cli_proto_head(buf, p, cli_cmd, idx);
	return idx;
}


void notify_team_info_changed(player_t* p, uint32_t type, uint32_t score)
{
	int off_len = sizeof(userid_t);
	if (p->team_info.team_id == 0) {
		return;
	}
	for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
		if (p->id == p->team_info.team_member[i].uid) {
			continue;
		}
		KDEBUG_LOG(p->id, "SEND TEAM MEMBER CHANGED[%u %u ]", type, p->id);
		player_t* to = get_player(p->team_info.team_member[i].uid);
		(*(uint32_t*)pkgbuf) = p->team_info.team_member[i].uid;
		int pkglen = pack_team_info_changed_pkg(pkgbuf + off_len, 
			cli_proto_notify_team_info, type, score, p);
		if (to) {
			send_to_player(to, pkgbuf + off_len, pkglen, 0);
		} else {
			chat_across_svr_2(p, pkgbuf,  pkglen + off_len);
		}
	}

	int idx = 0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	pack_h(dbpkgbuf, type, idx);
	pack_h(dbpkgbuf, score, idx);
	
	KDEBUG_LOG(p->id, "NOTIFY TEAM INFO CHANGED\t[%u %u %u]", p->team_info.team_id, type, score);
	send_request_to_db(0, p->id, p->role_tm, dbproto_team_info_changed, dbpkgbuf, idx);

}
	
//--------------------------------------------------------------------
/**
  * @brief 
  */
int create_fight_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (is_player_have_team(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_fight_team_err, 1);
	}

	if (!is_vip_player(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_vip_only, 1);
	}

	if (p->coins < 500000) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
	}

	uint32_t item1_cnt = p->my_packs->get_item_cnt(1302000);
	uint32_t item2_cnt = p->my_packs->get_item_cnt(1302001);
	if (item1_cnt + item2_cnt < 25) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
	}
	
	int i = 0;
	char team_name[max_nick_size] = "";
	unpack(body, team_name, sizeof(team_name), i);

	int idx = 0;
	pack(dbpkgbuf, team_name, sizeof(team_name), idx);
	pack(dbpkgbuf, p->nick, sizeof(p->nick), idx);
	
	return send_request_to_db(p, p->id, p->role_tm,	dbproto_create_fight_team, dbpkgbuf, idx);	
}

int reduce_create_team_item(player_t* p, uint32_t team_id) 
{
	uint32_t plant_cnt = 25;
	uint32_t type_a = 1302000;
	uint32_t type_b = 1302001;
	uint32_t reduce_coins = 500000;

	uint32_t cnt_1 = p->my_packs->get_item_cnt(type_a);
	uint32_t cnt_2 = p->my_packs->get_item_cnt(type_b);
	if (cnt_1 + cnt_2 < plant_cnt) {
		plant_cnt = cnt_1 + cnt_2;
	}

	uint32_t plant_A = 0;
	uint32_t plant_B = 0;
	uint32_t typecnt = 0;
	
	typecnt = 3;
	if (plant_cnt > cnt_1) {
		plant_A = cnt_1;
		plant_B = plant_cnt - cnt_1;
	} else {
		plant_A = plant_cnt;
		plant_B = 0;
	}

	p->coins = p->coins >= reduce_coins ? p->coins - reduce_coins : 0;
	do_stat_log_coin_add_reduce(p, (uint32_t)0, reduce_coins);
	p->my_packs->del_item(p, type_a, plant_A, channel_string_other);
	p->my_packs->del_item(p, type_b, plant_B, channel_string_other);
	
	int idx = 0;
	taomee::pack_h(dbpkgbuf, typecnt, idx);
	taomee::pack_h(dbpkgbuf, type_a, idx);
	taomee::pack_h(dbpkgbuf, plant_A, idx);
	taomee::pack_h(dbpkgbuf, type_b, idx);
	taomee::pack_h(dbpkgbuf, plant_B, idx);
	taomee::pack_h(dbpkgbuf, 1, idx);
	taomee::pack_h(dbpkgbuf, reduce_coins, idx);
	send_request_to_db(0, p->id, p->role_tm, dbproto_donate_item, dbpkgbuf, idx);

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, team_id, idx);	
	pack(pkgbuf, p->coins, idx);
	pack(pkgbuf, type_a, idx);
	pack(pkgbuf, plant_A, idx);
	pack(pkgbuf, type_b, idx);
	pack(pkgbuf, plant_B, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int db_set_player_team_flg(player_t* p, uint32_t team_id, uint32_t del_uid, uint32_t del_role_tm)
{
	int idx = 0;
	pack_h(dbpkgbuf, team_id, idx);

	KDEBUG_LOG(p->id, "SET TEAM FLG\t[%u %u %u]", team_id, del_uid, del_role_tm);
	return send_request_to_db(p, del_uid, del_role_tm, dbproto_set_player_team_flg, dbpkgbuf, idx);	
}

int send_team_broad_cast(player_t* p, uint32_t action)
{
	char words[max_trade_mcast_size] = {0};

    int index = 8 + ((action - 1) * 2) - 1;

	int len = snprintf(words, max_trade_mcast_size, "%s%u%s", char_content[index].content, p->team_info.team_id, char_content[index+1].content);
	TRACE_LOG("mcast title msg[%s][%d]",words,len);

	player_mcast_t info = {0};
	memset(&info, 0x00, sizeof(player_mcast_t)); 
	info.type = 2; //:player_market 2:player_world
	info.front_id = 2;
	memcpy(&(info.nick), g_title_condition.pve_note2, sizeof(info.nick));
	memcpy(&(info.info), words, max_trade_mcast_size);

	notify_player_mcast_info(p, mcast_trade_message, &info);
	return 0;
}

int db_set_player_team_flg_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) 
{
	CHECK_DBERR(p, ret);

	set_player_team_flg_rsp_t* rsp = reinterpret_cast<set_player_team_flg_rsp_t*>(body);

	if (p->waitcmd == cli_proto_create_fight_team) {

		reduce_create_team_item(p, rsp->team_id);

		p->team_info.team_id = rsp->team_id;
		p->team_info.team_member_cnt = 1;
		p->team_info.team_member[0].uid = p->id;
		p->team_info.team_member[0].role_tm = p->role_tm;
		p->team_info.team_member[0].level = member_lv_captain; 
		p->team_info.captain_uid = p->id;
		p->team_info.captain_role_tm = p->role_tm;

		report_user_base_info_to_chat_svr(p, talk_type_fight_team, p->team_info.team_id);
		send_team_broad_cast(p, create_team_action);
	}

	if (p->waitcmd == cli_proto_reply_invite_team_member) {
		int idx = 0;
		uint32_t inviter = 0;
		unpack_h(p->tmp_session, inviter, idx);
		idx = sizeof(cli_proto_t);
		pack(pkgbuf, rsp->team_id, idx);	
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	    send_to_player(p, pkgbuf, idx, 1);
		send_reply_invite_team_member(p, rsp->team_id, inviter, 2);
		on_team_member_changed(p, 1, rsp->team_id, p->id, p->role_tm);
		//设置每周加入侠士团次数限制
		add_swap_action_times(p, 1440);
	}

	if (p->waitcmd == cli_proto_del_team_member) {
		int tmp_sess = 0;
		uint32_t team_id;
		uint32_t del_uid;
		uint32_t del_role_tm;
		unpack_h(p->tmp_session, team_id, tmp_sess);
		unpack_h(p->tmp_session, del_uid, tmp_sess);
		unpack_h(p->tmp_session, del_role_tm, tmp_sess);
		
		int idx = sizeof(cli_proto_t);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	    send_to_player(p, pkgbuf, idx, 1);

		on_team_member_changed(p, 0, team_id, del_uid, del_role_tm);
		
	}

	if (p->waitcmd == cli_proto_del_team) {
		p->team_info.reset();
	    send_header_to_player(p, p->waitcmd, 0, 1);
		report_user_base_info_to_chat_svr(p, talk_type_fight_team, p->team_info.team_id);
	}
	
	return 0;
}


int db_create_fight_team_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) 
{
	CHECK_DBERR(p, ret);

	set_player_team_flg_rsp_t* rsp = reinterpret_cast<set_player_team_flg_rsp_t*>(body);

	do_stat_log_2num(0x09819005, 1, 0);
	return db_set_player_team_flg(p, rsp->team_id, p->id, p->role_tm);
}

int pack_rltm_invite_team_member_pkg(uint8_t* buf, uint32_t cli_cmd, player_t* inviter) 
{
	int idx = sizeof(cli_proto_t);
	pack(buf, inviter->team_info.team_id, idx);
	pack(buf, inviter->id, idx);
	pack(buf, inviter->nick, sizeof(inviter->nick), idx);
	pack(buf, inviter->team_info.team_name, sizeof(inviter->team_info.team_name), idx);
	
	init_cli_proto_head(buf, inviter, cli_cmd, idx);
	return idx;
}

bool can_player_invaite_member(player_t * p)
{
	for (uint32_t i =0; i < p->team_info.team_member_cnt; i++) {
		if (p->team_info.team_member[i].uid == p->id &&
				p->team_info.team_member[i].role_tm == p->role_tm) {
			if (p->team_info.team_member[i].level != member_lv_tmp &&
					p->team_info.team_member[i].level <= member_lv_team_leader) {
				return true;
			}	
		}
	}
	return false;
}


/**
  * @brief 
  */
int invite_team_member_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t invitee = 0;
	unpack(body, invitee, idx);

	if (!can_player_invaite_member(p)) {

		WARN_LOG("cannot invite %u -> %u ", p->id, invitee);
		return send_header_to_player(p, p->waitcmd, cli_err_fight_team_err, 1);
	}


	
	player_t* to = get_player(invitee);
	int off_len = sizeof(userid_t);
	
	(*(uint32_t*)pkgbuf) = invitee;
	int pkglen = pack_rltm_invite_team_member_pkg(pkgbuf + off_len, cli_proto_send_invite_team_member, p);

	TRACE_LOG("%u -> %u ", p->id, invitee);
	
	if (to) {
		send_to_player(to, pkgbuf + off_len, pkglen, 0);
	} else {
		chat_across_svr_2(p, pkgbuf,  pkglen + off_len);
	}
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

int pack_rltm_reply_invite_team_member_pkg(uint8_t* buf, uint32_t cli_cmd, uint32_t accept, player_t* p) 
{
	int idx = sizeof(cli_proto_t);
	pack(buf, p->id, idx);
	pack(buf, p->role_tm, idx);
	pack(buf, p->nick, sizeof(p->nick), idx);
	pack(buf, accept, idx);

	init_cli_proto_head(buf, p, cli_cmd, idx);
	return idx;
}

void send_reply_invite_team_member(player_t* p, uint32_t team_id, uint32_t inviter, uint32_t accept)
{
	player_t* to = get_player(inviter);
	int off_len = sizeof(userid_t);
	
	(*(uint32_t*)pkgbuf) = inviter;
	int pkglen = pack_rltm_reply_invite_team_member_pkg(pkgbuf + off_len, 
		cli_proto_send_reply_invite_team_member, accept, p);

	KDEBUG_LOG(p->id, "SEND REPLY TO INVITER[%u %u %u]", p->id, inviter, accept);
	
	if (to) {
		send_to_player(to, pkgbuf + off_len, pkglen, 0);
	} else {
		chat_across_svr_2(p, pkgbuf,  pkglen + off_len);
	}
}

int db_add_team_member(player_t* p, uint32_t team_id, uint32_t inviter)
{
	int idx = 0;
	pack_h(dbpkgbuf, team_id, idx);
	pack_h(dbpkgbuf, inviter, idx);
	KDEBUG_LOG(p->id, "ADD TEAM MEMBER\t[%u %u]", team_id, inviter);
	return send_request_to_db(p, p->id, p->role_tm,	dbproto_add_fight_team_member, dbpkgbuf, idx);	
}


int db_add_fight_team_member_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) 
{
	CHECK_DBERR(p, ret);
	do_stat_log_2num(0x09819006, 1, 0);
	
	add_fight_team_member_rsp_t* rsp = reinterpret_cast<add_fight_team_member_rsp_t*>(body);
	p->team_info.team_id = rsp->team_id;
	p->team_info.team_member_cnt = rsp->member_cnt;
	for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
		p->team_info.team_member[i].uid = rsp->member[i].uid;
		p->team_info.team_member[i].role_tm = rsp->member[i].role_tm;
		p->team_info.team_member[i].join_tm = rsp->member[i].join_tm;
		p->team_info.team_member[i].level = rsp->member[i].level;
		p->team_info.team_member[i].t_coins = rsp->member[i].t_coins;
		p->team_info.team_member[i].t_exp = rsp->member[i].t_exp;
	}
	KDEBUG_LOG(p->id, "ADD MEMBER CB\t[%u %u %u]", p->team_info.team_id, p->team_info.team_member_cnt,
		rsp->inviter);
	report_user_base_info_to_chat_svr(p, talk_type_fight_team, p->team_info.team_id);
	return db_set_player_team_flg(p, rsp->team_id, p->id, p->role_tm);
}


/**
  * @brief 
  */
int reply_invite_team_member_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t team_id, inviter, accept;
	unpack(body, team_id, i);
	unpack(body, inviter, i);
	unpack(body, accept, i);
	if (get_swap_action_times(p, 1440)) {
		//没周只能加入一支侠士团
		return send_header_to_player(p, p->waitcmd, cli_err_join_team_once_week, 1);
	}	



	if (is_player_have_team(p) || p->lv < 10) {
		return send_header_to_player(p, p->waitcmd, cli_err_fight_team_err, 1);
	}
	i = 0;
	pack_h(p->tmp_session, inviter, i);
	
	if (accept) {
		return db_add_team_member(p, team_id, inviter);
		
	}

	send_reply_invite_team_member(p, team_id, inviter, 0);
	
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, 0, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int db_get_team_info(player_t* p, uint32_t team_id)
{
	int idx = 0;
	pack_h(dbpkgbuf, team_id, idx);

	KDEBUG_LOG(p->id, "GET TEAM INFO\t[%u]", team_id);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_team_info, dbpkgbuf, idx);	

}

int get_team_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t team_id;
	unpack(body, team_id, i);

	return db_get_team_info(p, team_id);	
}

int db_get_team_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) 
{
	CHECK_DBERR(p, ret);
	get_team_info_rsp_t* rsp = reinterpret_cast<get_team_info_rsp_t*>(body);

	KDEBUG_LOG(p->id, "TEAM INFO\t[%u %u %u %u %u %s]", rsp->team_id, rsp->captain_uid, rsp->captain_tm, rsp->member_cnt, rsp->team_coin, rsp->team_name);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->team_id, idx);
    uint32_t team_order = get_team_order_in_100(rsp->team_id);
	pack(pkgbuf, team_order, idx);
	pack(pkgbuf, rsp->captain_uid, idx);
	pack(pkgbuf, rsp->captain_tm, idx);
	pack(pkgbuf, rsp->team_coin, idx);
	pack(pkgbuf, rsp->score_index, idx);
	pack(pkgbuf, rsp->team_name, sizeof(rsp->team_name), idx);
	pack(pkgbuf, rsp->team_exp, idx);

	pack(pkgbuf, rsp->team_mcast, sizeof(rsp->team_mcast), idx);
	pack(pkgbuf, rsp->member_cnt, idx);


	if (rsp->team_id == p->team_info.team_id) {
		p->team_info.captain_uid = rsp->captain_uid;
		p->team_info.captain_role_tm = rsp->captain_tm;
		memcpy(p->team_info.team_name, rsp->team_name, sizeof(p->team_info.team_name));
		memcpy(p->team_info.team_mcast, rsp->team_mcast, sizeof(p->team_info.team_mcast));
		memcpy(p->team_info.captain_nick, rsp->captain_nick, max_nick_size);
		p->team_info.coin = rsp->team_coin;
		p->team_info.active_flag = rsp->active_flag;
		p->team_info.active_score = rsp->active_score;
		p->team_info.score_index = rsp->score_index;
		p->team_info.team_member_cnt = rsp->member_cnt;
		p->team_info.team_lv = calc_fight_team_lv_by_exp(rsp->team_exp);
		p->team_info.team_exp = rsp->team_exp;
		p->team_info.last_tax_tm = rsp->last_tax_tm;
	}

	for (uint32_t i = 0; i < rsp->member_cnt; i ++) {

		pack(pkgbuf, rsp->member[i].id, idx);
		pack(pkgbuf, rsp->member[i].tm, idx);
		if (rsp->captain_uid == rsp->member[i].id 
				&& rsp->captain_tm == rsp->member[i].tm) {
			rsp->member[i].level = member_lv_captain;
		}
		pack(pkgbuf, rsp->member[i].level, idx);
		pack(pkgbuf, rsp->member[i].t_coins, idx);
		pack(pkgbuf, rsp->member[i].t_exp, idx);
        TRACE_LOG("team member [%u]--->[%u %u %u]", rsp->member[i].id, rsp->member[i].level, rsp->member[i].t_coins, rsp->member[i].t_exp);
		//		ERROR_LOG("TEAM MEMBER >>>>>> %u %u %u", rsp->member[i].id,
		//				 rsp->member[i].tm, rsp->member[i].level);
		//
		//
		if (p->team_info.team_id == rsp->team_id) {
			p->team_info.team_member[i].uid = rsp->member[i].id;
			p->team_info.team_member[i].role_tm = rsp->member[i].tm;
			p->team_info.team_member[i].join_tm = rsp->member[i].join_tm;
			p->team_info.team_member[i].level = rsp->member[i].level;
			p->team_info.team_member[i].t_coins = rsp->member[i].t_coins;
			p->team_info.team_member[i].t_exp = rsp->member[i].t_exp;
		}
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);
	if (p->team_info.team_id == rsp->team_id) {
		send_team_member_onoff(p, 1, p->team_info.team_id, p->id, p->role_tm);
		get_team_tax(p, 0);
		proc_player_team_achieve_logic(p);
	}
	return 0;
}


int del_team_member_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t del_uid, del_role_tm;
	unpack(body, del_uid, i);
	unpack(body, del_role_tm, i);

	if (!is_player_have_access_to_del(p, del_uid, del_role_tm)) {
		WARN_LOG("%u del_team_member %u %u", p->id, del_uid, del_role_tm);
		return send_header_to_player(p, p->waitcmd, cli_err_fight_team_err, 1);
	}
	int tmp_sess = 0;
	pack_h(p->tmp_session, p->team_info.team_id, tmp_sess);
	pack_h(p->tmp_session, del_uid, tmp_sess);
	pack_h(p->tmp_session, del_role_tm, tmp_sess);
	
	int idx = 0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	pack_h(dbpkgbuf, del_uid, idx);
	pack_h(dbpkgbuf, del_role_tm, idx);
	
	KDEBUG_LOG(p->id, "DEL TEAM MEMBER\t[%u %u %u]", p->team_info.team_id, del_uid, del_role_tm);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_del_team_member, dbpkgbuf, idx);	

}

int db_del_team_member_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) 
{
	CHECK_DBERR(p, ret);
	del_team_member_rsp_t* rsp = reinterpret_cast<del_team_member_rsp_t*>(body);
	do_stat_log_2num(0x09819006, 0, 1);
	KDEBUG_LOG(p->id, "DEL TEAM MEMBER CB\t[%u %u %u]", rsp->team_id, rsp->del_uid, rsp->del_role_tm);

	return db_set_player_team_flg(p, 0, rsp->del_uid, rsp->del_role_tm);
}

int db_del_team_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) 
{
	CHECK_DBERR(p, ret);

	KDEBUG_LOG(p->id, "DEL TEAM CB");
	do_stat_log_2num(0x09819005, 0, 1);
	return db_set_player_team_flg(p, 0, p->id, p->role_tm);
}


int del_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!is_player_team_captain(p) || p->team_info.team_member_cnt > 1) {
		WARN_LOG("%u del_team %u %u", p->id, p->team_info.team_id, p->team_info.team_member_cnt);
		return send_header_to_player(p, p->waitcmd, cli_err_fight_team_err, 1);
	}
	if (p->id != p->team_info.captain_uid || p->role_tm != p->team_info.captain_role_tm) {
		WARN_LOG("%u del_team_member not captain %u %u", p->id, p->team_info.captain_uid, p->team_info.captain_role_tm);
		return send_header_to_player(p, p->waitcmd, cli_err_fight_team_err, 1);
	}
	do_stat_log_universal_interface_1(0x09819007, 0, 1);
	int idx = 0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	
	KDEBUG_LOG(p->id, "DEL TEAM\t[%u]", p->team_info.team_id);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_del_team, dbpkgbuf, idx);	
}

int send_team_top10_rsp(player_t* p)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, g_team_top10_chache.cnt, idx);
	for (uint32_t i = 0; i < g_team_top10_chache.cnt; i++) {
		pack(pkgbuf, g_team_top10_chache.elem[i].teamid, idx);
		pack(pkgbuf, g_team_top10_chache.elem[i].team_name, max_nick_size, idx);
		pack(pkgbuf, g_team_top10_chache.elem[i].captain_uid, idx);
		pack(pkgbuf, g_team_top10_chache.elem[i].captain_role_tm, idx);
		pack(pkgbuf, g_team_top10_chache.elem[i].captain_name, max_nick_size, idx);
		pack(pkgbuf, g_team_top10_chache.elem[i].team_exp, idx);
		pack(pkgbuf, g_team_top10_chache.elem[i].member_cnt, idx);
		pack(pkgbuf, g_team_top10_chache.elem[i].team_score, idx);

		TRACE_LOG("%u %s %u %u %s %u %u", g_team_top10_chache.elem[i].teamid,
			g_team_top10_chache.elem[i].team_name,
			g_team_top10_chache.elem[i].captain_uid,
			g_team_top10_chache.elem[i].captain_role_tm,
			g_team_top10_chache.elem[i].captain_name,
			g_team_top10_chache.elem[i].member_cnt,
			g_team_top10_chache.elem[i].team_score);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);

}

uint32_t get_team_order_in_100(uint32_t team_id)
{
    for (uint32_t i = 0; i < g_team_top10_chache.cnt; i++) {
        if (team_id == g_team_top10_chache.elem[i].teamid) {
            return (i + 1);
        }
    }
    return 0;
} 

int get_team_top10_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (cur_time - p->team_top10_tv < 30) {
		return send_team_top10_rsp(p);
	}
	p->team_top10_tv = cur_time;
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_team_top10, 0, 0);	
}

int db_get_team_top10_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) 
{
	CHECK_DBERR(p, ret);
	get_team_top10_rsp_t* rsp = reinterpret_cast<get_team_top10_rsp_t*>(body);

	KDEBUG_LOG(p->id, "GET TOP10 TEAM CB\t[%u]", rsp->cnt);

	g_team_top10_chache.cnt = rsp->cnt;
	for (uint32_t i = 0; i < rsp->cnt; i++) {
		g_team_top10_chache.elem[i].teamid = rsp->elem[i].teamid;
		memcpy(g_team_top10_chache.elem[i].team_name, rsp->elem[i].team_name, max_nick_size);
		g_team_top10_chache.elem[i].captain_uid = rsp->elem[i].captain_uid;
		g_team_top10_chache.elem[i].captain_role_tm = rsp->elem[i].captain_role_tm;
		memcpy(g_team_top10_chache.elem[i].captain_name, rsp->elem[i].captain_name, max_nick_size);
		g_team_top10_chache.elem[i].team_exp = rsp->elem[i].team_exp;
		g_team_top10_chache.elem[i].member_cnt = rsp->elem[i].member_cnt;
		g_team_top10_chache.elem[i].team_score = rsp->elem[i].team_score;
	}

    return send_team_top10_rsp(p);
}

/**
 * @brief
 */
int contribute_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
	uint32_t coin = 0;
	unpack(body, coin, idx);

	if (!is_player_have_team(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_not_team_member, 1);
	}
    //coin
    if (p->coins < coin || coin > 1000000) {
        return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
    }
    
    if (p->team_info.coin + coin > 100000000) {
        return send_header_to_player(p, p->waitcmd, cli_err_team_coins_limit, 1);
    }
	do_stat_log_coin_add_reduce(p, (uint32_t)0, coin);

    p->coins -= coin;
    set_team_member_coin(p, coin);

    idx = 0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	pack_h(dbpkgbuf, coin, idx);
	
	KDEBUG_LOG(p->id, "CONTRIBUTE TEAM\t[%u %u|%u]", p->team_info.team_id, p->coins, coin);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_contribute_team, dbpkgbuf, idx);	
}

int db_contribute_team_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
    db_contribute_team_rsp_t* rsp = reinterpret_cast<db_contribute_team_rsp_t*>(body);

    //p->coins = rsp->player_coin;
    p->team_info.coin = rsp->team_coin;

    db_set_role_base_info(p);
    
    synchro_team_info_to_member(p, 0);

    int idx = sizeof(cli_proto_t);
	pack(dbpkgbuf, rsp->team_id, idx);
	pack(dbpkgbuf, rsp->team_coin, idx);
	pack(dbpkgbuf, p->coins, idx);

	init_cli_proto_head(dbpkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, dbpkgbuf, idx, 1);
}

int fetch_team_coin_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
	uint32_t coin = 0;
	unpack(body, coin, idx);

	if (!is_player_team_captain(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_not_team_captain, 1);
	}

    //coin
    if (p->team_info.coin < coin || coin > 1000000) {
        return send_header_to_player(p, p->waitcmd, cli_err_team_coin_not_enough, 1);
    }

    p->coins += coin;
	do_stat_log_coin_add_reduce(p, coin, (uint32_t)0);

    idx = 0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	pack_h(dbpkgbuf, coin, idx);
	
	KDEBUG_LOG(p->id, "CONTRIBUTE TEAM\t[%u %u|%u]", p->team_info.team_id, p->team_info.coin, coin);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_fetch_team_coin, dbpkgbuf, idx);	
}

int db_fetch_team_coin_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
    db_fetch_team_coin_rsp_t* rsp = reinterpret_cast<db_fetch_team_coin_rsp_t*>(body);

    //p->coins = rsp->player_coin;
    p->team_info.coin = rsp->team_coin;

    db_set_role_base_info(p);

    synchro_team_info_to_member(p, 0);

    int idx = sizeof(cli_proto_t);
	pack(dbpkgbuf, rsp->team_id, idx);
	pack(dbpkgbuf, rsp->team_coin, idx);
	pack(dbpkgbuf, p->coins, idx);

	init_cli_proto_head(dbpkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, dbpkgbuf, idx, 1);
}

void db_set_team_base_info(player_t* p)
{
    int idx = 0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	pack_h(dbpkgbuf, p->team_info.coin, idx);
	
	KDEBUG_LOG(p->id, "SET TEAM COINS \t[%u %u]", p->team_info.team_id, p->team_info.coin);
	send_request_to_db(0, p->id, p->role_tm, dbproto_set_team_base_info, dbpkgbuf, idx);
}


//=================================================
//楼兰杯侠士团挑战赛
//=================================================
void notify_team_active_score_change(player_t* p, uint32_t active_flag, uint32_t type, uint32_t score)
{
    if (!is_player_have_team(p)) return;
    // active_flag ---> 1:楼兰杯
    //             ---> 2:天下第一比武大会
    if (active_flag == 1) {
        if (p->team_info.active_flag != 1) {
            return;
        }

        if (check_player_can_join_active(p, 2) != 0) {
            return;
        }
        do_stat_log_universal_interface_2(0x09526006, 0, p->team_info.team_id, 1);
    }
	int idx = 0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	pack_h(dbpkgbuf, type, idx); // 1--increase; 2--reduce
	pack_h(dbpkgbuf, active_flag, idx);
	pack_h(dbpkgbuf, score, idx);
	
	KDEBUG_LOG(p->id, "NOTIFY TEAM ACTIVE SCORE CHANGED\t[%u %u %u]", p->team_info.team_id, type, score);
	send_request_to_db(0, p->id, p->role_tm, dbproto_team_active_score_change, dbpkgbuf, idx);
}

void pack_player_team_sync_info(player_t * p, void * buf)
{

}	

int pack_team_info_ex(uint8_t* buf, uint32_t cli_cmd, player_t *p, uint32_t is_active_flag, uint32_t add_exp)
{
    int idx = sizeof(cli_proto_t);
	pack(buf, p->team_info.team_id, idx);
	pack(buf, p->team_info.coin, idx);
    if (is_active_flag) {
        pack(buf, p->team_info.active_flag, idx);
    } else {
        pack(buf, static_cast<uint32_t>(2), idx);
    }
	pack(buf, add_exp, idx);
	pack(buf, p->team_info.last_tax_tm, idx);
	init_cli_proto_head(buf, p, cli_cmd, idx);
	return idx;
}

void synchro_team_info_to_member(player_t* p, uint32_t is_active_flag, uint32_t add_exp, uint32_t self_flag)
{
    int off_len = sizeof(userid_t);

	for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
		if (p->id == p->team_info.team_member[i].uid && self_flag == 0) {
			continue;
		}
		
		player_t* to = get_player(p->team_info.team_member[i].uid);

		KDEBUG_LOG(p->id, "SYNCHRO TEAM MEMBER to %u [%u %u]", 
            p->team_info.team_member[i].uid, p->team_info.active_flag, p->team_info.coin);

		(*(uint32_t*)pkgbuf) = p->team_info.team_member[i].uid;
		int pkglen = pack_team_info_ex(pkgbuf + off_len, cli_proto_synchro_team_info, p, is_active_flag, add_exp);
		if (to) {
            to->team_info.active_flag = p->team_info.active_flag;
            to->team_info.coin        = p->team_info.coin;
			send_to_player(to, pkgbuf + off_len, pkglen, 0);
		} else {
			chat_across_svr_2(p, pkgbuf,  pkglen + off_len);
		}
	}
}

int chat_across_team_info(player_t* p, uint8_t* body)
{
    uint32_t team_id = 0, coin = 0, active_flag = 0, add_exp = 0, last_tax_tm = 0;
	int idx = 0;
	unpack(body, team_id, idx);
	unpack(body, coin, idx);
	unpack(body, active_flag, idx);
	unpack(body, add_exp, idx);
	unpack(body, last_tax_tm, idx);

    TRACE_LOG("synchro_team_info %u [%u %u %u]", p->id, team_id, coin, active_flag);
    if (p->team_id == team_id) {
        p->team_info.coin = coin;
        p->team_info.active_flag = active_flag;
		p->team_info.team_exp += add_exp;
		p->team_info.team_lv = calc_fight_team_lv_by_exp(p->team_info.team_exp);
		p->team_info.last_tax_tm = last_tax_tm;
    }
    return 0;
}

int db_set_active_flag(player_t* p, uint32_t coin, uint32_t active_flag, int is_ret)
{
    if (!is_player_have_team(p)) return 0;
    if (p && p->team_info.active_flag == active_flag) return 0;

    int idx = 0;
    pack_h(dbpkgbuf, p->team_info.team_id, idx);
    pack_h(dbpkgbuf, coin, idx);
    pack_h(dbpkgbuf, active_flag, idx);

    if (is_ret) {
        send_request_to_db(p, p->id, p->role_tm, dbproto_team_active_enter, dbpkgbuf, idx);
    } else {
        send_request_to_db(0, p->id, p->role_tm, dbproto_team_active_enter, dbpkgbuf, idx);
    }
    return 0;
}
/**
 * @brief 报名
 */
int team_active_enter_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    //
    if (p->team_info.active_flag) {
        return send_header_to_player(p, p->waitcmd, cli_err_team_active_have_finish, 1);
    }

    if ( !is_player_team_captain(p) ) {
        return send_header_to_player(p, p->waitcmd, cli_err_not_team_captain, 1);
    }

    //time limit
    uint32_t ret_err = check_player_can_join_active(p, 1);
    if (ret_err) {
        return send_header_to_player(p, p->waitcmd, ret_err, 1);
    }
    
    //coin limit
    uint32_t need_coin = 100000;
    //if (p->team_info.coin < need_coin) {
    //    return send_header_to_player(p, p->waitcmd, cli_err_team_coin_not_enough, 1);
    //}
    return db_set_active_flag(p, need_coin, 1, 1);
}

int db_team_active_enter_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);

    team_active_enter_t* rsp = reinterpret_cast<team_active_enter_t*>(body);

    p->team_info.coin = rsp->need_coin;
    p->team_info.active_flag = 1;

    send_team_broad_cast(p, join_team_action);
    synchro_team_info_to_member(p, 1);

    do_stat_log_universal_interface_1(0x09526005, 0, 1);
    int idx = sizeof(cli_proto_t);
	pack(dbpkgbuf, rsp->team_id, idx);
	pack(dbpkgbuf, p->team_info.coin, idx);

	init_cli_proto_head(dbpkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, dbpkgbuf, idx, 1);
}

int send_team_active_top100_rsp(player_t* p)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, g_team_active_chache.type, idx);
	pack(pkgbuf, g_team_active_chache.cnt, idx);
	for (uint32_t i = 0; i < g_team_active_chache.cnt; i++) {
		pack(pkgbuf, g_team_active_chache.elem[i].id, idx);
		pack(pkgbuf, g_team_active_chache.elem[i].name, max_nick_size, idx);
		pack(pkgbuf, g_team_active_chache.elem[i].captain_id, idx);
		pack(pkgbuf, g_team_active_chache.elem[i].captain_tm, idx);
		pack(pkgbuf, g_team_active_chache.elem[i].captain_nick, max_nick_size, idx);
		pack(pkgbuf, g_team_active_chache.elem[i].team_exp, idx);
		pack(pkgbuf, g_team_active_chache.elem[i].member_cnt, idx);
		pack(pkgbuf, g_team_active_chache.elem[i].active_score, idx);
		pack(pkgbuf, g_team_active_chache.elem[i].team_score, idx);

		TRACE_LOG("%u %s %u %u %s %u %u", g_team_active_chache.elem[i].id,
			g_team_active_chache.elem[i].name,
			g_team_active_chache.elem[i].captain_id,
			g_team_active_chache.elem[i].captain_tm,
			g_team_active_chache.elem[i].captain_nick,
			g_team_active_chache.elem[i].member_cnt,
			g_team_active_chache.elem[i].active_score);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int get_team_active_top100_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
	uint32_t type = 0; // -->1:楼兰杯 2:天下第一比武大会
	unpack(body, type, idx);

    uint32_t cur_time = get_now_tv()->tv_sec;
    if (cur_time < g_team_active_chache.timestamp + 60) {
        send_team_active_top100_rsp(p);
    }

    idx = 0;
    pack_h(dbpkgbuf, type, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_team_active_top100, dbpkgbuf, idx);	
}

int db_get_team_active_top100_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    team_active_top100_cache_t* rsp = reinterpret_cast<team_active_top100_cache_t*>(body);

	KDEBUG_LOG(p->id, "GET TOP100 TEAM CB\t[%u]", rsp->cnt);

	g_team_active_chache.cnt = rsp->cnt;
    g_team_active_chache.timestamp = get_now_tv()->tv_sec;
    
	for (uint32_t i = 0; i < rsp->cnt; i++) {
        g_team_active_chache.elem[i].id = rsp->elem[i].id;
        memcpy(g_team_active_chache.elem[i].name, rsp->elem[i].name, max_nick_size);
        g_team_active_chache.elem[i].captain_id = rsp->elem[i].captain_id;
        g_team_active_chache.elem[i].captain_tm = rsp->elem[i].captain_tm;
        memcpy(g_team_active_chache.elem[i].captain_nick, rsp->elem[i].captain_nick, max_nick_size);
		g_team_active_chache.elem[i].team_score = rsp->elem[i].team_score;
        g_team_active_chache.elem[i].member_cnt = rsp->elem[i].member_cnt;
        g_team_active_chache.elem[i].active_score = rsp->elem[i].active_score;
        g_team_active_chache.elem[i].active_index = i + 1;
    }

    return send_team_active_top100_rsp(p);
}

bool is_player_team_in_top100(player_t* p)
{
    if (!is_player_have_team(p)) {
        return false;
    }

    for (uint32_t i = 0; i < g_team_active_chache.cnt; i++) {
        if (g_team_active_chache.elem[i].id == p->team_info.team_id) {
            return true;
        }
    }
    return false;
}

bool is_active_time_member(player_t* p)
{
    active_data* pdata = get_active_data_mgr()->get_data_by_active_id(1);

    uint32_t join_tm = get_team_join_tm(p);

    if (pdata->to_ > join_tm) {
        return true;
    }
    return false;
}

int db_set_member_right_lv(player_t *p, uint32_t userid, uint32_t role_tm, uint32_t right_lv)
{
	int idx =0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	pack_h(dbpkgbuf, userid, idx);
	pack_h(dbpkgbuf, role_tm, idx);
	pack_h(dbpkgbuf, right_lv, idx);
	return send_request_to_db(NULL, p->id, p->role_tm, dbproto_set_memeber_right_lv, dbpkgbuf, idx);	
}


int update_right_lv(player_t * p, uint32_t userid, uint32_t role_tm, uint32_t right_lv)
{
	static int lv_limit[6] = {24, 1, 2, 5, 24, 24};
	if (p->team_info.team_id && p->team_info.captain_uid == p->id && p->team_info.captain_role_tm == p->role_tm) {
		int lv_cnt = 0;
		int default_error_idx = 111;
		int member_idx = default_error_idx;
		for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
			if (p->team_info.team_member[i].level == right_lv) {
				lv_cnt ++;
			}
			if (p->team_info.team_member[i].uid == userid &&
					p->team_info.team_member[i].role_tm == role_tm) { 
				member_idx = i;
			}
		}

		if (member_idx == default_error_idx) {
			return cli_err_no_such_team_member;
		}
		if ((right_lv != 5) && lv_cnt >= lv_limit[right_lv]) {
			return cli_err_full_right_lv;         
		} else {
			p->team_info.team_member[member_idx].level = right_lv;
			db_set_member_right_lv(p, userid, role_tm, right_lv);   
		}
		return 0;
	}
	return cli_err_not_the_captain;
}


int set_team_member_lv_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t role_tm = 0;
	uint32_t userid = 0;
	uint32_t right_lv = 0;
	unpack(body, userid, idx);
	unpack(body, role_tm, idx);
	unpack(body, right_lv, idx);
	int error_no = update_right_lv(p, userid, role_tm, right_lv);
	if (error_no) {
		return send_header_to_player(p, p->waitcmd, error_no, 1);
	} else {
		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, p->nick, max_nick_size, idx);
		pack(pkgbuf, userid, idx);
		pack(pkgbuf, role_tm, idx);
		pack(pkgbuf, right_lv, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		p->waitcmd = 0;
		transmit_pkg_cross_chat_svr(p, talk_type_fight_team, p->team_info.team_id, pkgbuf, idx);
	}
	return 0;
}

bool can_change_team_name(player_t *p)
{
	if (p->team_info.team_id 
			&& p->team_info.captain_uid == p->id
			&& p->team_info.captain_role_tm == p->role_tm) {
		return true;
	}
	return false;
}


int db_player_gain_team_exp(player_t * p, uint32_t add_exp)
{
	int idx = 0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	pack_h(dbpkgbuf, add_exp, idx);
	return 	send_request_to_db(NULL, p->id, p->role_tm, dbproto_gain_team_exp, dbpkgbuf, idx);
}

void player_gain_team_exp_without_contribute(player_t *p, uint32_t add_exp)
{
	int idx = 0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	pack_h(dbpkgbuf, add_exp, idx);
	send_request_to_db(NULL, 0, 0, dbproto_gain_team_exp, dbpkgbuf, idx);

	p->team_info.team_exp += add_exp;
	synchro_team_info_to_member(p, 0, add_exp, 1);
}

void  player_gain_team_exp(player_t * p, uint32_t add_exp)
{
	p->team_info.team_exp += add_exp;
	db_player_gain_team_exp(p, add_exp);
    set_team_member_exp(p, add_exp);
	synchro_team_info_to_member(p, 0, add_exp);
}


void set_team_member_exp(player_t * p, uint32_t add_exp)
{
    for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
        if (p->id == p->team_info.team_member[i].uid 
            && p->role_tm == p->team_info.team_member[i].role_tm) {
            p->team_info.team_member[i].t_exp += add_exp;
            break;
        }
    }
	proc_player_team_achieve_logic(p);
}

void set_team_member_coin(player_t * p, uint32_t add_coin)
{
    for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
        if (p->id == p->team_info.team_member[i].uid 
            && p->role_tm == p->team_info.team_member[i].role_tm) {
            p->team_info.team_member[i].t_coins += add_coin;
            break;
        }
    }
	proc_player_team_achieve_logic(p);

}

int player_gain_team_exp_by_swap(player_t * p, uint32_t swap_id) 
{
	static uint32_t swap_exp[3] = {10, 20, 40};
	uint32_t add_exp = 0;
	if (swap_id >= 1409 && swap_id <= 1411) {
		add_exp = swap_exp[swap_id - 1409];
        if (is_in_active_time_section(34) == 0) {
            add_exp *= 2;
        }
	} else if (swap_id == 1416) {
		add_exp = 10;
	} else if (swap_id == 1427) {
		add_exp = 50;
	} else if (swap_id == 1511) {
		int rank = get_player_ranker_range(p, 10001);
		if (rank < 10) {
			add_exp = 30000 + 500 * (11 - rank);
		} else if (rank < 50) {
			add_exp = 10000 + 200 * (51 - rank);
		} else {
			add_exp = 2000 + 100 * (101 - rank);
		}
		player_gain_team_exp_without_contribute(p, add_exp);
		return 0;
	}

	if (p->team_info.team_lv < 7) {
		player_gain_team_exp(p, add_exp);
	}
	return 0;
}


int change_team_name_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	if (can_change_team_name(p)) {
		//store the mcast info 
		body[bodylen - 1] = '\0';
		CHECK_DIRTYWORD(p, body);
		CHECK_INVALID_WORD(p, body);
		int idx = 0;
		pack_h(dbpkgbuf, p->team_info.team_id, idx);
		pack(dbpkgbuf, body, max_nick_size, idx);
		send_request_to_db(NULL, p->id, p->role_tm, dbproto_change_team_name, dbpkgbuf, idx);

		idx = sizeof(cli_proto_t);
        pack(pkgbuf, p->nick, max_nick_size, idx);
		pack(pkgbuf, body, max_nick_size, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		p->waitcmd = 0;

		transmit_pkg_cross_chat_svr(p, talk_type_fight_team, p->team_info.team_id, pkgbuf, idx);
	}
	return 0;
}

bool can_change_team_mcast(player_t *p)
{
	for (uint32_t i =0; i < p->team_info.team_member_cnt; i++) {
		if (p->team_info.team_member[i].uid == p->id &&
				p->team_info.team_member[i].role_tm == p->role_tm) {
			if (p->team_info.team_member[i].level != member_lv_tmp &&
					p->team_info.team_member[i].level <= member_lv_vice_captain) {
				return true;
			}	
		}
	}
	return false;
//	if (p->team_info.team_id 
//			&& p->team_info.captain_uid == p->id
//			&& p->team_info.captain_role_tm == p->role_tm) {
//		return true;
//	}
//	return false;
}


int change_team_mcast_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	if (can_change_team_mcast(p)) {
		int max_mcast_len = 240;
		//store the mcast info 
		body[bodylen - 1] = '\0';
		CHECK_DIRTYWORD(p, body);
		CHECK_INVALID_WORD(p, body);
		int idx = 0;
		pack_h(dbpkgbuf, p->team_info.team_id, idx);
		pack(dbpkgbuf, body, max_mcast_len, idx);
		send_request_to_db(NULL, p->id, p->role_tm, dbproto_change_team_mcast, dbpkgbuf, idx);

		idx = sizeof(cli_proto_t);
		pack(pkgbuf, body, max_mcast_len, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		p->waitcmd = 0;
		transmit_pkg_cross_chat_svr(p, talk_type_fight_team, p->team_info.team_id, pkgbuf, idx);
	}
	return 0;
}

int search_team_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t team_id;
	uint32_t captain_id;
	unpack(body, team_id, idx);
    unpack(body, captain_id, idx);	

	idx = 0;
	pack_h(dbpkgbuf, team_id, idx);
	pack_h(dbpkgbuf, captain_id, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_search_team_info, dbpkgbuf, idx);
}


int db_search_team_info_callback(player_t *p, userid_t id, void * body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	team_search_rsp_t * rsp = reinterpret_cast<team_search_rsp_t*>(body);

	int idx = sizeof(cli_proto_t);

	pack(pkgbuf, rsp->cnt, idx);
	for (uint32_t i = 0; i < rsp->cnt; i++) {
		pack(pkgbuf, rsp->teams[i].teamid, idx);
		pack(pkgbuf, rsp->teams[i].team_name, max_nick_size, idx);
		pack(pkgbuf, rsp->teams[i].captain_uid, idx);
		pack(pkgbuf, rsp->teams[i].captain_role_tm, idx);
		pack(pkgbuf, rsp->teams[i].captain_name, max_nick_size, idx);
		pack(pkgbuf, rsp->teams[i].team_exp, idx);
		pack(pkgbuf, rsp->teams[i].member_cnt, idx);
		pack(pkgbuf, rsp->teams[i].team_score, idx);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int check_fight_team_exp_swap(player_t * p, uint32_t swap_id)
{
	static uint32_t time_limit[7][3] = {
		{1, 2, 1},
		{1, 3, 2},
		{1, 3, 3},
		{1, 4, 3},
		{1, 4, 4},
		{1, 5, 4},
		{1, 5, 5}
	};
	if (p->team_info.team_id) {
		if (p->team_info.team_lv > 7 || p->team_info.team_lv == 0) {
			ERROR_LOG("check team exp %u %u %u", p->id, p->team_info.team_id,  p->team_info.team_lv);
		}
		uint32_t tmp_team_lv = p->team_info.team_lv == 0 ? 1 : p->team_info.team_lv;
		tmp_team_lv = tmp_team_lv > 7 ? 7 : tmp_team_lv;
		if (get_swap_action_times(p, swap_id) < time_limit[tmp_team_lv - 1][ swap_id - 1409]) {
			return 0;
		} 
	}
	return -1;
}

int get_tax_coin_by_lv(uint32_t lv)
{
	if (lv > 0 && lv < 8) {
		static int tax_coin[7] = {
			50000, 100000, 200000, 500000, 1000000, 2000000, 5000000 
		};
		return tax_coin[lv - 1];
	}
	return 0;
}

int db_get_team_tax_ex(player_t * p, uint32_t reduce_coin)
{
	int idx = 0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	pack_h(dbpkgbuf, reduce_coin, idx);
	uint32_t now_tv = get_now_tv()->tv_sec;
	pack_h(dbpkgbuf, now_tv, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_team_tax_coin, dbpkgbuf, idx);
}

int get_team_tax(player_t * p, uint32_t contribute_flag)
{
	if (!is_same_day(get_now_tv()->tv_sec, p->team_info.last_tax_tm)) {
		uint32_t tax_coin = get_tax_coin_by_lv(p->team_info.team_lv);
		int interval = day_interval(get_now_tv()->tv_sec, p->team_info.last_tax_tm);
		uint32_t reduce_coin = 0;
		if (p->team_info.coin > tax_coin * interval && !contribute_flag) {
			reduce_coin = tax_coin * interval;
		} else if (p->team_info.coin > tax_coin) {
			reduce_coin = tax_coin;
		}
		db_get_team_tax_ex(p, reduce_coin);
	}
	return 0;
}

struct team_tax_rsp_t {
	uint32_t team_id;
	uint32_t reduce_coin;
	uint32_t update_tm;
}__attribute__((packed));

int db_team_tax_coin_callback(player_t * p,
		userid_t id,
		void * body,
		uint32_t bodylen,
		uint32_t ret)
{
	team_tax_rsp_t * rsp = reinterpret_cast<team_tax_rsp_t*>(body);
	if (rsp->team_id == p->team_info.team_id
			&& rsp->reduce_coin
			&& p->team_info.coin > rsp->reduce_coin ) {
		p->team_info.coin -= rsp->reduce_coin;
		p->team_info.last_tax_tm = rsp->update_tm;
		synchro_team_info_to_member(p, 0);
		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, p->team_info.team_id, idx);
		pack(pkgbuf, p->team_info.coin, idx);
		pack(pkgbuf, static_cast<uint32_t>(2), idx);
		pack(pkgbuf, 0, idx);
		pack(pkgbuf, p->team_info.last_tax_tm, idx);
		init_cli_proto_head(pkgbuf, p, cli_proto_synchro_team_info, idx);
		send_to_player(p, pkgbuf, idx, 0);
	}
	return 0;
}


bool can_get_team_attr_add(player_t * p)
{
	return is_same_day(get_now_tv()->tv_sec, p->team_info.last_tax_tm) 
		&& p->team_info.team_id && p->team_info.team_lv;
}

int player_add_team_exp_by_pvp(player_t * p, uint32_t continue_win) 
{
	uint32_t add_exp = 0;
	if ((continue_win * 2 + 1) <= 15) {
		add_exp = continue_win * 2 + 1;
	} else {
		add_exp = 15;
	}	
	db_player_gain_team_exp(p, add_exp);
	set_team_member_exp(p, add_exp);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->team_info.team_id, idx);
	pack(pkgbuf, p->team_info.coin, idx);
	pack(pkgbuf, static_cast<uint32_t>(2), idx);
	pack(pkgbuf, add_exp, idx);
	pack(pkgbuf, p->team_info.last_tax_tm, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_synchro_team_info, idx);
	send_to_player(p, pkgbuf, idx, 0);
	synchro_team_info_to_member(p, 0, add_exp);

	return 0;
}


void calc_fight_team_attr_add(player_t *p, fight_team_attr_add_t * attr)
{
	if (p->team_info.team_id && is_same_day(get_now_tv()->tv_sec, p->team_info.last_tax_tm)) {
		calc_player_team_attr_add_by_team_lv(attr, p->team_info.team_lv);
	}
}

player_t * get_one_team_member_player(uint32_t team_id)
{

	for (int i = 0; i < c_bucket_size; ++i) {
		player_t * p = NULL;
		list_for_each_entry (p, &uid_buckets[i], playerhook) {
			if (p->team_info.team_id == team_id) {
				return p;
			}
		}
	}
	return NULL;
}


static std::map<uint32_t, TeamContestRoom*> *  AllTeamContestRooms = NULL;

void init_fight_team_contest_info()
{
	if (NULL == AllTeamContestRooms) { 
		AllTeamContestRooms = new std::map<uint32_t, TeamContestRoom*>;
	}

	for (uint32_t i = 1004; i < 1014; i++) {
		TeamContestRoom * room = new TeamContestRoom(i);
		AllTeamContestRooms->insert(std::map<uint32_t, TeamContestRoom*>::value_type(i, room));
		TRACE_LOG("INIT TeamContest Room Map %u", i);
	}
}

TeamContestRoom *  get_contest_room()
{
	for (std::map<uint32_t, TeamContestRoom*>::iterator it = AllTeamContestRooms->begin();
			it != AllTeamContestRooms->end(); ++it) {
		TeamContestRoom * room = it->second;
		if (room->can_join()) {
			return room;
		}
	}
	return NULL;
}

void reduce_team_coin(player_t *p, uint32_t coin)
{
	int idx = 0;
	pack_h(dbpkgbuf, p->team_info.team_id, idx);
	pack_h(dbpkgbuf, coin, idx);
	send_request_to_db(NULL, p->id, p->role_tm, dbproto_fetch_team_coin, dbpkgbuf, idx);	

	p->team_info.coin -= coin;

	synchro_team_info_to_member(p, 0);

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->team_info.team_id, idx);
	pack(pkgbuf, p->team_info.coin, idx);
	pack(pkgbuf, static_cast<uint32_t>(2), idx);
	pack(pkgbuf, 0, idx);
	pack(pkgbuf, p->team_info.last_tax_tm, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_synchro_team_info, idx);
	send_to_player(p, pkgbuf, idx, 0);
}

bool not_in_pvp_room()
{
	uint32_t server_id = get_server_id();
	if (server_id >= 100 && server_id <= 150) {
		return false;
	}
	return true;
}



int captain_team_contest_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
//	//不在PVP服务器
//	if (not_in_pvp_room()) {
//		return send_header_to_player(p, p->waitcmd, cli_err_action_svr_not_pvp, 1);  
//	}

	if (p->team_info.captain_uid == p->id && p->role_tm == p->team_info.captain_role_tm) {
		if (get_swap_action_times(p, 1431)) {
			//参加过，或者已经报过名了	
			return send_header_to_player(p, p->waitcmd, cli_err_team_contest_again, 1);
		}	

//		//不在报名时间
		if (!get_now_active_data_by_active_id(28)) {
			return send_header_to_player(p, p->waitcmd, cli_err_not_in_captain_ready_time, 1);
		}

		//团队资金不够
		if (p->team_info.coin < 1000000) {
        	return send_header_to_player(p, p->waitcmd, cli_err_team_coin_not_enough, 1);
		}

		return btlsw_team_contest_auto_join(p);
	}
	return -1;
}

TeamContestRoom * get_player_team_contest_room(uint32_t team_id)
{
	for (std::map<uint32_t, TeamContestRoom*>::iterator it = AllTeamContestRooms->begin();
			it != AllTeamContestRooms->end(); ++it) {
		TeamContestRoom * room = it->second;
		if (room->IsTeamContestRoom(team_id)) {
			return room;
		}
	}
	return NULL;
}

int get_team_contest_room_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	return  btlsw_team_contest_get_server_id(p);
}

void TeamContestRoom::team_join_room(uint32_t team_id)
{
	if (a_team_id == 0 && team_id != b_team_id) {
		a_team_id = team_id;
	} else if (a_team_id != team_id && b_team_id == 0) {
		b_team_id = team_id;
	}
	//send a mail to all team member
}

void TeamContestRoom::Init()
{
	a_team_id = 0;
	b_team_id = 0;
	win_team = 0;
	reward_cnt = 0;
	last_mcast_tm = 0;
	room_status = Contest_NOT_Ready;

	for (std::list<ContestWapper*>::iterator it = ATeamMembers.begin();
			it != ATeamMembers.end(); ++it) {
		ContestWapper * player = *it;
		delete player;
	}

	for (std::list<ContestWapper*>::iterator it = BTeamMembers.begin();
			it != BTeamMembers.end(); ++it) {
		ContestWapper * player = *it;
		delete player;
	}

	ATeamMembers.clear();
	BTeamMembers.clear();
}

void TeamContestRoom::ContestReady()
{
	room_status = Contest_Ready_Status;
	TRACE_LOG("TEAM Room %u SET READY", map_id);
//	if (a_team_id && b_team_id) {
//	} else if (a_team_id) {//匹配失败
//		this->ContestMatchFail();
//	}
}

void TeamContestRoom::ContestStart()
{
	room_status = Contest_Going_Status;
	DEBUG_LOG("TEAM CONTST START %u ATEAM %u BTeam %u", map_id, a_team_id, b_team_id);


	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, room_status, idx);
	pack(pkgbuf, win_team, idx);
	pack(pkgbuf, reward_cnt, idx);

	for (std::list<ContestWapper*>::iterator it = ATeamMembers.begin();
			it != ATeamMembers.end(); ++it) {
		ContestWapper * wapper = *it;
		init_cli_proto_head(pkgbuf, wapper->player, cli_proto_team_contest_status, idx);
		send_to_player(wapper->player, pkgbuf, idx, 0);
	}

	for (std::list<ContestWapper*>::iterator it = BTeamMembers.begin();
			it != BTeamMembers.end(); ++it) {
		ContestWapper * wapper = *it;
		init_cli_proto_head(pkgbuf, wapper->player, cli_proto_team_contest_status, idx);
		send_to_player(wapper->player, pkgbuf, idx, 0);
	}

	if (ATeamMembers.size() + BTeamMembers.size() >= 5) {
		reward_cnt = 6;	
	 } else {
		reward_cnt = 2;
	 }
}

void TeamContestRoom::ContestOver()
{
	if (ATeamMembers.size() >= BTeamMembers.size()) {
		win_team = a_team_id;
	} else {
		win_team = b_team_id;
	}

	room_status = Contest_Ended_Status;
	if (win_team) {
		RewardTeamExp();
	}


	DEBUG_LOG("TEAM CONTEST OVER %u WIN TEAM %u", map_id, win_team);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, room_status, idx);
	pack(pkgbuf, win_team, idx);
	pack(pkgbuf, reward_cnt, idx);

	for (std::list<ContestWapper*>::iterator it = ATeamMembers.begin();
			it != ATeamMembers.end(); ++it) {
		ContestWapper * wapper = *it;
		init_cli_proto_head(pkgbuf, wapper->player, cli_proto_team_contest_status, idx);
		send_to_player(wapper->player, pkgbuf, idx, 0);
	}

	for (std::list<ContestWapper*>::iterator it = BTeamMembers.begin();
			it != BTeamMembers.end(); ++it) {
		ContestWapper * wapper = *it;
		init_cli_proto_head(pkgbuf, wapper->player, cli_proto_team_contest_status, idx);
		send_to_player(wapper->player, pkgbuf, idx, 0);
	}
}


bool TeamContestRoom::ContestNeedEnded()
{
	if (room_status == Contest_Going_Status) {
		if (ATeamMembers.empty() || BTeamMembers.empty()) {
			return true;
		}
	}
	return false;
}

bool can_trigger_btl(const ContestWapper * p1, const ContestWapper * p2) 
{
	return (::abs(p1->player->xpos - p2->player->xpos) <= 100 
				&& ::abs(p1->player->ypos - p2->player->ypos) <= 100);
}

void TeamContestRoom::ContestTriggerLogic()
{
	for (std::list<ContestWapper*>::iterator it = ATeamMembers.begin();
			it != ATeamMembers.end(); ++it) {
		ContestWapper * first = *it;
		if (first->active_flag  == 0) {
			for (std::list<ContestWapper*>::iterator it = BTeamMembers.begin();
					it != BTeamMembers.end(); ++it) {
				ContestWapper * second = *it;
				if (second->active_flag == 0) {
					if (can_trigger_btl(first, second)) {
						first->active_flag = 1;
						second->active_flag = 1;
						first->enemy_uid = second->player->id;
						second->enemy_uid = first->player->id;
						int idx = sizeof(cli_proto_t);
						pack(pkgbuf, second->player->id, idx);
						pack(pkgbuf, second->player->role_tm, idx);
						init_cli_proto_head(pkgbuf, first->player, cli_proto_contest_trigger_info, idx);
						send_to_player(first->player, pkgbuf, idx, 0);
						KDEBUG_LOG(first->player->id, "PLayer CONTEST TRIGGER %u", second->player->id);
						break;
					}	
				}
			}
		}
	}

}

void TeamContestRoom::ContestMatchFail()
{
	DEBUG_LOG("TEAM CONTEST MATCH FAIL A TEAM %u ROOM %u", a_team_id, map_id);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, room_status, idx);
	pack(pkgbuf, 0, idx);
	player_t * a_team_member = get_one_team_member_player(a_team_id);
	if (a_team_member) {
		for (uint32_t i = 0; i < a_team_member->team_info.team_member_cnt; ++i) {
			player_t * member = get_player(a_team_member->team_info.team_member[i].uid);
			if (member && member->role_tm == a_team_member->team_info.team_member[i].role_tm) {
				init_cli_proto_head(pkgbuf, member, cli_proto_team_contest_status, idx);
				send_to_player(member, pkgbuf, idx, 0);
			}
		}
	}

//	this->a_team_id = 0;
}

void TeamContestRoom::update()
{
	uint32_t contest_before_action_id = 28;
	uint32_t contest_entering_action_id = 29;
	uint32_t contest_going_action_id = 30;
	uint32_t contest_reward_action_id = 32;



	//报名时间段
	if (get_now_active_data_by_active_id(contest_before_action_id)) {
		if (room_status != Contest_NOT_Ready) {
			this->Init();
		}
	//进入时间段
	} else if (get_now_active_data_by_active_id(contest_entering_action_id)) {
		if (room_status < Contest_Ready_Status) {
			this->ContestReady();
		}
		this->RoomMcast();
	//进行时间段, 
	} else if (get_now_active_data_by_active_id(contest_going_action_id)) {
		if (room_status < Contest_Going_Status ) {
			this->ContestStart();
			//有队伍全员输掉了， 提前结束
		} else if (ContestNeedEnded()) {
			DEBUG_LOG("TEAM CONTEST OVER BEFORE END A %u %lu B %u %lu", a_team_id, ATeamMembers.size(), b_team_id, 
					BTeamMembers.size());
			this->ContestOver();
			//碰撞检测
		} else if (room_status == Contest_Going_Status &&
				(!ATeamMembers.empty() || !BTeamMembers.empty())){
			ContestTriggerLogic();
		}
	} else if (get_now_active_data_by_active_id(contest_reward_action_id)) {
		//到了结束时间，未结束还有人
		if (room_status < Contest_Ended_Status && 
				!ATeamMembers.empty() && !BTeamMembers.empty() ) {
			DEBUG_LOG("TEAM CONTEST OVER A %u %lu B %u %lu", a_team_id, ATeamMembers.size(), b_team_id, 
					BTeamMembers.size());
			ContestOver();
		}
	}
}


void TeamContestRoom::PlayerLeave(struct player_t * p)
{

	TRACE_LOG("PLayer %u LLeave TEAM CONTEST ROOM %u", p->id, map_id);

	if (p->team_info.team_id == a_team_id) {
		for (std::list<ContestWapper*>::iterator it = ATeamMembers.begin(); 
				it != ATeamMembers.end(); ++it) {
			ContestWapper * wapper = *it;
			if (wapper->player->id  == p->id && wapper->player->role_tm == p->role_tm) {
				it = ATeamMembers.erase(it);
				KDEBUG_LOG(wapper->player->id, "PLAYER LEAVE TEAM %u CONTEST ROOM %u", wapper->player->id, map_id); 
				delete wapper;
				return;
			}
		}

	} else if (p->team_info.team_id == b_team_id) {
		for (std::list<ContestWapper*>::iterator it = BTeamMembers.begin(); 
				it != BTeamMembers.end(); ++it) {
			ContestWapper * wapper = *it;
			if (wapper->player->id == p->id && wapper->player->role_tm == p->role_tm) {
				it = BTeamMembers.erase(it);
				KDEBUG_LOG(wapper->player->id, "PLAYER LEAVE TEAM %u CONTEST ROOM %u", wapper->player->id, map_id); 
				delete wapper;
				return;
			}
		}
	}

}

bool TeamContestRoom::CanPlayerJoin(struct player_t * p)
{
	return room_status == Contest_Ready_Status 
	   && (p->team_info.team_id == a_team_id || p->team_info.team_id == b_team_id);	
}

void TeamContestRoom::PlayerJoin(struct player_t * p)
{
	ContestWapper * wapper = new ContestWapper(p);
	if (p->team_info.team_id == a_team_id) {
		KDEBUG_LOG(p->id, "PLAYER JOIN IN TEAM CONTEST A TEAM %u", p->team_info.team_id); 
		ATeamMembers.push_back(wapper);
	} else {
		KDEBUG_LOG(p->id, "PLAYER JOIN IN TEAM CONTEST B TEAM %u", p->team_info.team_id); 
		BTeamMembers.push_back(wapper);
	}
}

uint32_t get_left_time()
{
	time_t now = time(NULL);
	struct tm * start_tm = localtime(&now); 
	start_tm->tm_sec = 0;
	start_tm->tm_min = 33;
	start_tm->tm_hour = 13;

	uint32_t start_sec = mktime(start_tm);

	if (start_sec > now) {
		return start_sec - now;
	}
	return 0;
}

void TeamContestRoom::SendToRoom(void * buf, uint32_t cmd, int len)
{
	for (std::list<ContestWapper*>::iterator it  = ATeamMembers.begin();
					it != ATeamMembers.end(); ++it) {
		ContestWapper * wapper = *it;
		init_cli_proto_head(buf, wapper->player, cmd, len);
		send_to_player(wapper->player, buf, len, 0);
	}

	for (std::list<ContestWapper*>::iterator it  = BTeamMembers.begin();
					it != BTeamMembers.end(); ++it) {
		ContestWapper * wapper = *it;
		init_cli_proto_head(buf, wapper->player, cmd, len);
		send_to_player(wapper->player, buf, len, 0);
	}

}

void TeamContestRoom::RoomMcast()
{
	uint32_t contest_entering_action_id = 29;
	if (get_now_active_data_by_active_id(contest_entering_action_id)) {
		if (last_mcast_tm != get_now_tv()->tv_sec) { 
			last_mcast_tm = get_now_tv()->tv_sec;
			next_mcast_tm = get_left_time();

			int idx = sizeof(cli_proto_t);
			uint32_t a_team_cnt = ATeamMembers.size();
			uint32_t b_team_cnt = BTeamMembers.size();
			pack(pkgbuf, next_mcast_tm, idx);
			pack(pkgbuf, a_team_cnt, idx);
			pack(pkgbuf, b_team_cnt, idx);
			//if there no one in the room, ignore the mcast step
			if (a_team_cnt || b_team_cnt) {
				TRACE_LOG("TEAM Send Ready Info Left_time %u TeamA %u TeamB %u", next_mcast_tm, a_team_cnt, 
						b_team_cnt);
				SendToRoom(pkgbuf, cli_proto_team_contest_ready_info, idx);
			}
		}
	}
}
	

uint32_t TeamContestRoom::RewardPlayer(player_t * p)
{
	KDEBUG_LOG(p->id, "REWARD PLAYER CONTEST TEAM %u REWARD CUR VAL %u", p->team_info.team_id, reward_cnt);
	reward_cnt --;
	return 0;
}


void TeamContestRoom::RewardTeamExp()
{
	uint32_t team_a_exp = 0;
	uint32_t team_b_exp = 0;
	if (win_team && win_team == a_team_id) {
		team_a_exp = 500;
		team_b_exp = 200;
	} else {
		team_b_exp = 500;
		team_a_exp = 200;
	}

	int idx = 0;
	pack_h(dbpkgbuf, b_team_id, idx);
	pack_h(dbpkgbuf, team_b_exp, idx);
	send_request_to_db(NULL, 0, 0, dbproto_gain_team_exp, dbpkgbuf, idx);

	idx = 0;
	pack_h(dbpkgbuf, a_team_id, idx);
	pack_h(dbpkgbuf, team_a_exp, idx);
	send_request_to_db(NULL, 0, 0, dbproto_gain_team_exp, dbpkgbuf, idx);

}

bool TeamContestRoom::CanPlayerGetReward(player_t * p)
{
	if (win_team  
			&& win_team == p->team_info.team_id 
			&& reward_cnt > 0 
			&& room_status == Contest_Ended_Status) {

		if (GetContestPlayer(p->id, p->role_tm)) {
			return true;
		}
	}
	return false;
}

void update_contest_room_event()
{

	static timeval last_update_tv = *(get_now_tv());

	const timeval * now = get_now_tv();

	int time_elapsed = timediff2(*now, last_update_tv);

	if (time_elapsed > 200) {
		for (std::map<uint32_t, TeamContestRoom*>::iterator it = AllTeamContestRooms->begin();
				it != AllTeamContestRooms->end(); ++it) {
			TeamContestRoom * room = it->second;
			room->update();
		}
		last_update_tv = *now;
	}
}

ContestWapper * get_player_fight_contest_info(player_t * p)
{
	TeamContestRoom * contest_room = get_player_team_contest_room(p->team_info.team_id);
	if (contest_room) {
	   return contest_room->GetContestPlayer(p->id, p->role_tm);
	}
	return NULL;
}

ContestWapper * TeamContestRoom::GetContestPlayer(uint32_t uid, uint32_t role_tm)
{

	for (std::list<ContestWapper*>::iterator it = ATeamMembers.begin(); 
				it != ATeamMembers.end(); ++it) {
			ContestWapper * wapper = *it;
			if (wapper->player->id  == uid && wapper->player->role_tm == role_tm) {
				return wapper;
			}
		}

	for (std::list<ContestWapper*>::iterator it = BTeamMembers.begin(); 
				it != BTeamMembers.end(); ++it) {
			ContestWapper * wapper = *it;
			if (wapper->player->id  == uid && wapper->player->role_tm == role_tm) {
				return wapper;
			}
		}

	return NULL;
}

void player_enter_team_contest_btl(player_t * p)
{
	ContestWapper * wapper = get_player_fight_contest_info(p);
	if (wapper) {
		wapper->active_flag = 2;
	}
	TRACE_LOG("TEAM player %u enter Btl", p->id);
}

void player_leave_team_contest_btl(player_t * p, uint32_t win_flag)
{
	if (win_flag) {
		ContestWapper * wapper = get_player_fight_contest_info(p);
		KDEBUG_LOG(p->id, "CONTEST  %u TEAM BTL WIN", p->team_info.team_id);
		if (wapper) {
			if (wapper->enemy_uid) {
				int idx = sizeof(cli_proto_t);
				pack(pkgbuf, wapper->player->id, idx);
				pack(pkgbuf, wapper->enemy_uid, idx);
				init_cli_proto_head(pkgbuf, p, cli_proto_btl_over_notify, idx);
				send_to_map(p, pkgbuf, idx, 0);
				TRACE_LOG("NOTI TEAM CONTEST BTL %u %u", wapper->player->id, wapper->enemy_uid);
			}
			add_player_team_active_score(p, 1, 1);
		}

	} else {
		TeamContestRoom * contest_room = get_player_team_contest_room(p->team_info.team_id);
		if (contest_room) {
			KDEBUG_LOG(p->id, "CONTEST BTL LOSE TEAM %u ", p->team_info.team_id);
			contest_room->PlayerLeave(p);
		}
	}
}

bool can_player_enter_team_contest(player_t * p, uint32_t map_id)
{
	std::map<uint32_t, TeamContestRoom*>::iterator it = AllTeamContestRooms->find(map_id);
	if (it != AllTeamContestRooms->end()) {
		TeamContestRoom * room = it->second;
		return room->CanPlayerJoin(p);
	}
	return false;
}


void  player_enter_fight_team_contest_map(player_t * p, uint32_t map_id)
{
	std::map<uint32_t, TeamContestRoom*>::iterator it = AllTeamContestRooms->find(map_id);
	if (it != AllTeamContestRooms->end()) {
		TeamContestRoom * room = it->second;
		room->PlayerJoin(p);
	}
}

void player_leave_contest_room(player_t * p, uint32_t map_id)
{
	std::map<uint32_t, TeamContestRoom*>::iterator it = AllTeamContestRooms->find(map_id);
	if (it != AllTeamContestRooms->end()) {
		TeamContestRoom * room = it->second;
		room->PlayerLeave(p);
	}
}

bool can_player_get_contest_reward(player_t * p)
{
	TeamContestRoom * contest_room = get_player_team_contest_room(p->team_info.team_id);
	if (contest_room) {
		return contest_room->CanPlayerGetReward(p);
	} 
	return false;
}

void  reward_player_team_contest_win(player_t * p) {
	TeamContestRoom * contest_room = get_player_team_contest_room(p->team_info.team_id);
	if (contest_room) {
		contest_room->RewardPlayer(p);
	}
}


void player_cancel_contest_team_btl(player_t * p)
{
	ContestWapper * wapper = get_player_fight_contest_info(p);
	if (wapper) {
		wapper->enemy_uid = 0;
		wapper->active_flag = 0;
		KDEBUG_LOG(p->id, "Player Cancel CONTEST TEAM %u BTL", p->team_info.team_id);
	}
}

void add_player_team_active_score(player_t * p, uint32_t active_id,  uint32_t add_score)
{
	int idx = 0;
	pack_h(pkgbuf, active_id, idx);
	pack_h(pkgbuf, p->team_info.team_id, idx);
	pack_h(pkgbuf, add_score, idx);
	pack_h(pkgbuf, 0, idx);
	pack_h(pkgbuf, p->team_info.captain_uid, idx);
	pack_h(pkgbuf, p->team_info.captain_role_tm, idx);
	pack(pkgbuf, p->team_info.captain_nick, max_nick_size,  idx);
	pack(pkgbuf, p->team_info.team_name, max_nick_size, idx);
	pack_h(pkgbuf, p->team_info.team_member_cnt, idx);
	send_request_to_cachesvr(p, p->id, p->role_tm, cache_save_team_active_info, pkgbuf, idx);
}

int get_player_team_coin_donate(player_t * p)
{
  for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
        if (p->id == p->team_info.team_member[i].uid 
            && p->role_tm == p->team_info.team_member[i].role_tm) {
        	return  p->team_info.team_member[i].t_coins; 
        }
    }
	return 0;
}

int get_player_team_exp_donate(player_t * p)
{
	for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
        if (p->id == p->team_info.team_member[i].uid 
            && p->role_tm == p->team_info.team_member[i].role_tm) {
        	return  p->team_info.team_member[i].t_exp; 
        }
    }
	return 0;
}



