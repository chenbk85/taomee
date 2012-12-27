#include <libtaomee++/inet/pdumanip.hpp>

using namespace taomee;

extern "C" {
#include <arpa/inet.h>

#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/dataformatter/bin_str.h>

#include <libtaomee/project/utilities.h>
}


#include "utils.hpp"

#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "map.hpp"
#include "player.hpp"
#include "switch.hpp"
#include "login.hpp"
#include "home.hpp"
#include "task.hpp"
#include "skill.hpp"
#include "fwd_decl.hpp"
#include "battle_switch.hpp"
#include "restriction.hpp"
#include "global_data.hpp"
#include "item.hpp"
#include "fight_team.hpp"
#include "common_op.hpp"
#include "other_active.hpp"
#include "chat.hpp"
#include "cachesvr.hpp"


//-----------------------------------------------------------
// static function declarations
//-----------------------------------------------------------

static bool verify_session(const player_t * p,const login_session_t * sess);


//---------------------------------------------------------------------

/**
  * @brief handle login protocol. Don't support guest login
  * @return 0 on success, -1 on error
  */
int login_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
	if (is_valid_uid(p->id)) {	
		uint32_t role_tm = 0;
		unpack(body + sizeof(login_session_t), role_tm, idx);
		uint32_t version_data = 0;
		unpack(body + sizeof(login_session_t), version_data, idx);
#ifdef RELEASE_VER
		static uint32_t local_version = config_get_intval("online_version", 0);
		if (version_data != local_version) {
			ERROR_LOG("The version of client[%u] and server[%u] is not match",version_data,local_version);
			return -1;
		}
#endif
		uint32_t g_version = 0;
		unpack(body + sizeof(login_session_t), g_version, idx);
		uint32_t is_adult = 0;
		unpack(body + sizeof(login_session_t), is_adult, idx);
		player_t* player = add_player(p);
		player->waitcmd = cli_proto_login;
		player->role_tm = role_tm;
		//player->role_tm = ntohl(*(reinterpret_cast<uint32_t*>(body + sizeof(login_session_t))));
        
		memcpy(player->session, body, sizeof(login_session_t));
		player->sesslen = sizeof(login_session_t);

#ifdef VERSION_KAIXIN
		if (g_version == version_taomee || g_version == version_xiaoba) {
			ERROR_LOG("login kaixin server with taomee client");
			return -1;
		}
		if (is_adult) {
			player->adult_flg = 1;
		} else {
			player->adult_flg = 0;
		}
		//player->adult_flg = 1; 
		TRACE_LOG("1 %u %u", player->id, player->adult_flg);
#else
		if ( g_version != version_taomee &&
			 g_version != version_xiaoba && 
			 g_version != vsersion_taiwan) {
			ERROR_LOG("login taomee server with kaixin client");
			return -1;
		}
		player->adult_flg = 0;
		TRACE_LOG("2 %u %u", player->id, player->adult_flg);
#endif
		if (db_check_session(player, 6, body, sizeof(login_session_t), 1) != 0) {
			//verify session
			if (verify_login_session(player->id, body) == -1) {
				ERROR_LOG("send to dbproxy err, and invalid local session: uid=%u", player->id);
				send_header_to_player(player, player->waitcmd, cli_err_invalid_session, 1);
				return -1;
			}
			KDEBUG_LOG(p->id, "LOGIN\t[uid=%u role_tm=%u fd=%d]", player->id, player->role_tm, player->fd);		

			//get player's info from db
			return db_get_player(player, player->id);	
		} else {	
			return 0;
		}
	}
	ERROR_LOG("userid not valid : %u", p->id);
	return -1;
}
/**  
 * @brief check session
 * @param p   player_t*
 * @param gameid		项目类型:
 * @param session		计算得到的session值
 * @param session_len     session长度
 * @param del_flag		验证后是否要删除session(0:不是,1:是)
 * @return 0 on success, -1 on error
**/
int db_check_session(player_t* p, uint32_t gameid, uint8_t* session, 
	uint32_t session_len, uint32_t del_flag)
{
	int idx = 0;
	pack_h(dbpkgbuf, gameid, idx);
	pack(dbpkgbuf, session, session_len, idx);
	pack_h(dbpkgbuf, del_flag, idx);

	return send_request_to_db_no_roletm(p, p->id, dbproto_check_session, dbpkgbuf, idx);
}

int db_check_session_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	if ((ret && ret !=  dberr_session_check && verify_login_session(p->id, p->session) == -1) ||
		(ret == dberr_session_check)) {
		
		ERROR_LOG("local:invalid session: uid=%u ret=%u", p->id, ret);
		send_header_to_player(p, p->waitcmd, cli_err_invalid_session, 1);
		return -1;
	}
	TRACE_LOG("%u adult %u", p->id, p->adult_flg);
	//get player's info from db
	KDEBUG_LOG(p->id, "LOGIN\t[uid=%u role_tm=%u fd=%d]", p->id, p->role_tm, p->fd);		
	return db_get_player(p, p->id);	
	
/*
	//get player's info from db
	KDEBUG_LOG(p->id, "LOGIN\t[uid=%u role_tm=%u fd=%d]", p->id, p->role_tm, p->fd);		
	return db_get_player(p, p->id);
	
	if (!ret) {
		return db_get_player(p, p->id);	
	} else if (ret == dberr_session_check) {
		ERROR_LOG("dbproxy:invalid session: uid=%u", p->id);
		send_header_to_player(p, p->waitcmd, cli_err_invalid_session, 1);
		return -1;
	} else {
		if (verify_login_session(p->id, p->session) == -1) {
			ERROR_LOG("local:invalid session: uid=%u", p->id);
			send_header_to_player(p, p->waitcmd, cli_err_invalid_session, 1);
			return -1;
		}
		//get player's info from db
		KDEBUG_LOG(p->id, "LOGIN\t[uid=%u role_tm=%u fd=%d]", p->id, p->role_tm, p->fd);		
		return db_get_player(p, p->id);	
	}
	return 0;
*/
}

//---------------------------------------------------------------------

//-----------------------------------------------------------
// static function definations
//-----------------------------------------------------------


//-----------------------------------------------------------
// inline function definations
//-----------------------------------------------------------
/** 
 * @brief verify if the DESed session from the user is correct
 * @param p the user that is currently logging in
 * @param const login_session_t*, UnDESed session
 * @return true or false
 */
inline bool verify_session(const player_t* p, const login_session_t* sess)
{
	time_t diff = get_now_tv()->tv_sec - sess->tm1;

	// user id
	if ((sess->uid != p->id) || (sess->tm1 != sess->tm2) || (diff > 1800) || (diff < -1800)) {
		ERROR_LOG("invalid session: uid=%u %u tm=%u %u diff=%ld",
					p->id, sess->uid, sess->tm1, sess->tm2, diff);
		return false;
	}

	return true;
}

uint16_t get_award_cnt(player_t* p)
{
	uint16_t ret_sum = 0;
	for (uint32_t res_id = 436; res_id < max_restr_count; res_id++) {
		if (g_all_restr[res_id - 1].type == 13) {
			if (p->daily_restriction_count_list[res_id - 1]) {
				ret_sum ++;
			}
		} else {
			break;
		}
		
	}
	return ret_sum;
}

/**
 * @brief send login response to the given player
 * @param player
 * @return 0 on success, -1 on error
 */
int send_login_rsp(player_t* p)
{
	db_get_user_public_info(p);
	int idx = sizeof(cli_proto_t);
	//clear old show state
	if (p->player_show_state) {
		p->player_show_state = 0;
	}

//	if (is_buff_actived_on_player(p, 1312)) {
//		p->player_show_state = 1;
//		DEBUG_LOG("PLAYER %u HAS LIE SHOW STATE  %u", p->id, 1);
//	}
//  get_reward_time_when_login(p);

	if (p->olcount == 0) 
	{
		if (p->role_type == 4) 
		{
			enter_map(p, 31, 627, 719);	
		} 
		else 
		{
			enter_map(p, 5, 0, 0);					
		}
	} 
	else
	{
		if (p->last_mapid >= 43 && p->last_mapid <= 52) {
			p->last_mapid = 1;
		}
		if (p->last_mapid >= 1004 && p->last_mapid <= 1013) {
			p->last_mapid = 41;			
		}

		if ( enter_map(p, p->last_mapid, p->xpos, p->ypos) < 0 ) 
		{
            enter_map(p, 1, 0, 0);
        }

	}

	uint32_t sys_tm = get_now_tv()->tv_sec;
	uint32_t lv = p->lv;
	TRACE_LOG("%u", sys_tm);
	pack(pkgbuf, sys_tm, idx);
	pack(pkgbuf, p->role_tm, idx);
	pack(pkgbuf, p->role_type, idx);
	pack(pkgbuf, p->power_user, idx);
	pack(pkgbuf, p->nick, max_nick_size, idx);
	pack(pkgbuf, lv, idx);
    pack(pkgbuf, get_player_speed(p), idx);
    /*
	switch(p->role_type)
	{
		case 1:
			pack(pkgbuf, 145, idx);
		break;
		
		case 2:
			pack(pkgbuf, 150, idx);
		break;

		case 3:
			pack(pkgbuf, 140, idx);
		break;

		case 4:
			pack(pkgbuf, 140, idx);
		break;

		default:
			pack(pkgbuf, 140, idx);
		break;
	}*/

//	pack(pkgbuf, p->start_tm, idx); //for offline exp start time
	pack(pkgbuf, p->skill_point, idx);
	pack(pkgbuf, p->coins, idx);
	pack(pkgbuf, p->honor, idx);
	pack(pkgbuf, p->exploit, idx);
	pack(pkgbuf, p->win_times, idx);
	pack(pkgbuf, p->lose_times, idx);
	pack(pkgbuf, p->max_conti_win_times, idx);
	pack(pkgbuf, p->fumo_points_total, idx);
	pack(pkgbuf, p->fumo_tower_top, idx);	
	pack(pkgbuf, get_mapid_low32(p->cur_map->id), idx); //map_id
	pack(pkgbuf, p->xpos, idx);//xpos
	pack(pkgbuf, p->ypos, idx);//ypos
	pack(pkgbuf, p->oltoday, idx);
	if (p->adult_flg) 
	{
		pack(pkgbuf, 24 * 2 * 60 * 60, idx); /* battle time limit */
	} 
	else 
	{
		pack(pkgbuf, battle_time_limit, idx); /* battle time limit */
	}
	pack(pkgbuf, p->dexp_time, idx); /* double experience time */
	pack(pkgbuf, p->day_flag, idx); /* double experience done flag */
	pack(pkgbuf, p->max_times_chapter, idx);
   	pack(pkgbuf, p->player_show_state, idx);
	pack(pkgbuf, p->achieve_point, idx);	
	pack(pkgbuf, p->vitality_point, idx);
	pack(pkgbuf, p->m_home_data->home_action_point, idx);
	pack(pkgbuf, p->using_achieve_title, idx);	
	pack(pkgbuf, p->forbiden_add_friend_flag, idx);
	pack(pkgbuf, p->amb_status, idx);
	pack(pkgbuf, p->team_info.team_id, idx);
	uint16_t award_cnt = get_award_cnt(p);
	pack(pkgbuf, award_cnt, idx);
	pack(pkgbuf, p->itembind.buf, sizeof(p->itembind.buf), idx);
	

	idx += pack_player_task_info(p, pkgbuf + idx);
	idx += pack_player_skill_bind(p, pkgbuf + idx);
	idx += pack_player_fumo_info(p, pkgbuf + idx);
#ifdef DEV_SUMMON
	idx += pack_player_summon_full_info(p, pkgbuf + idx);
#endif
	idx += pack_player_secondary_pro_info(p, pkgbuf + idx);
	
	init_cli_proto_head(pkgbuf, p, cli_proto_login, idx);
	send_to_player(p, pkgbuf, idx, 1);
	set_bit_on(p->user_progress, user_progress_login);
	notify_user_login(p, 1);
	notify_user_on_off(p, 1);
	report_user_onoff_to_chat_svr(p, 1);
	report_user_basic_info_to_cachesvr(p);
	handle_player_normal_logic(p, NULL);
	return 0;
}

/**
 * @brief pack the player's fumo info
 * @param p the player
 * @param pkgbuf
 * @return the idx
 */
int pack_player_fumo_info(player_t* p, void* buf)
{
	int idx = 0;

	pack(buf, static_cast<uint32_t>(p->fumo_stage_map->size()), idx);
	TRACE_LOG("%lu ", p->fumo_stage_map->size());
	std::map<uint32_t, fumo_stage_info_t>::iterator it = p->fumo_stage_map->begin();
	for ( ; it != p->fumo_stage_map->end(); ++it) {
		uint16_t stage_id = it->second.stage_id;
		pack(buf, stage_id, idx);
		pack(buf, it->second.grade, sizeof(it->second.grade), idx);
		TRACE_LOG("--stage_id[%u %u %u %u %u %u %u]", stage_id, it->second.grade[0], it->second.grade[1], 
			it->second.grade[2], it->second.grade[3], it->second.grade[4], it->second.grade[5]);
	}

	return idx;
}

int pack_player_secondary_pro_info(player_t* p, void* buf)
{
	int idx = 0;
	pack(buf, static_cast<uint32_t>(p->m_pro_data_maps->size()), idx);
	std::map<uint32_t, secondary_profession_data>::iterator pItr = p->m_pro_data_maps->begin();
	for(; pItr != p->m_pro_data_maps->end(); ++pItr)
	{
		pack(buf, (int)pItr->second.m_type, idx);
		pack(buf,  pItr->second.m_exp,  idx);
	}
	return idx;
}
/*
int pack_player_mail_head_list(player_t* p, void* buf)
{
	int idx = 0;
	uint32_t mail_list_count = p->m_read_mails->size() + p->m_unread_mails->size();
	pack(buf,  mail_list_count, idx);

	std::map<uint32_t, mail_data>::iterator pItr = p->m_read_mails->begin();

	for(; pItr != p->m_read_mails->end(); ++pItr )
	{
		pack(buf,  pItr->second.mail_id, idx);
		pack(buf,  pItr->second.mail_time, idx);
		pack(buf,  pItr->second.mail_state, idx);
		pack(buf,  pItr->second.mail_templet, idx);
		pack(buf,  pItr->second.mail_type, idx);
		pack(buf,  pItr->second.sender_id, idx);
		pack(buf,  pItr->second.sender_role_tm, idx);
		pack(buf,  pItr->second.mail_title,  sizeof(pItr->second.mail_title), idx);
	}

	pItr = p->m_unread_mails->begin();
	for(; pItr != p->m_unread_mails->end(); ++pItr)
	{
		pack(buf,  pItr->second.mail_id, idx);
		pack(buf,  pItr->second.mail_time, idx);
		pack(buf,  pItr->second.mail_state, idx);
		pack(buf,  pItr->second.mail_templet, idx);
		pack(buf,  pItr->second.mail_type, idx);
		pack(buf,  pItr->second.sender_id, idx);
		pack(buf,  pItr->second.sender_role_tm, idx);
		pack(buf,  pItr->second.mail_title,  sizeof(pItr->second.mail_title), idx);
	}

	return idx;
}*/
 
/**
  * @brief set up a timer for kick all the players offline at 12 o'clock
  * @param owner
  * @param data 0: set up a timer; 1: set to kick all the players offline after 30 secs;
  *                     2: kick all the players offline
  * @return 0
  */
#if 0
int kick_all_users_offline(void* owner, void* data)
{
	struct tm tm_tmp = *get_now_tm();

	if (!data) {		
		tm_tmp.tm_hour = 23;
		tm_tmp.tm_min  = 45;
		tm_tmp.tm_sec  = 0;
		int32_t exptm   = mktime(&tm_tmp);
		if (get_now_tv()->tv_sec > exptm) {
			exptm += 24 * 60 * 60;
			set_battle_time_limit(1);
		} else {
			set_battle_time_limit(0);
		}

		KDEBUG_LOG(p->id, "KICK OFFLINE TIME\t[exptm=%lu]", exptm);
		ADD_TIMER_EVENT(&g_events, kick_all_users_offline, (void*)1, exptm);
	} else if (data == reinterpret_cast<void*>(1)) {
		// reset kick timer,make it run after 24hours
		int32_t exptm = get_now_tv()->tv_sec + 24 * 60 * 60;
		KDEBUG_LOG(p->id, "KICK OFFLINE TIME\t[exptm=%lu]", exptm);
		ADD_TIMER_EVENT(&g_events, kick_all_users_offline, (void*)1, exptm);
		
		// send a pkg to inform the players that server is going to close
		int idx = sizeof(cli_proto_t);
		init_cli_proto_head_full(pkgbuf, 0, 0, cli_proto_login, idx, cli_err_closed_time);
		send_to_all(pkgbuf, idx);
		
		// kick all players offline after 15 mins, 15 sec before 24:00
		exptm = get_now_tv()->tv_sec + 845;
		ADD_TIMER_EVENT(&g_events, kick_all_users_offline, (void*)2, exptm);
	} else if (data == reinterpret_cast<void*>(2)) {
		KDEBUG_LOG(p->id, "KICK ALL USERS OFFLINE\t[usr_cnt=%u]", player_num);
		traverse_players_remove(del_player);
	}

	return 0;
}
#else
int kick_all_users_offline(void* owner, void* data)
{
	uint32_t day_tv = 24 * 60 * 60;
	struct tm tm_tmp = *get_now_tm();
	tm_tmp.tm_hour = 23;
	tm_tmp.tm_min  = 45;
	tm_tmp.tm_sec  = 0;

	uint32_t notify_tm	= mktime(&tm_tmp);

	tm_tmp.tm_hour = 23;
	tm_tmp.tm_min  = 59;
	tm_tmp.tm_sec  = 30;	
	uint32_t kick_tm	= mktime(&tm_tmp);

	if (!data) {		
		// add notify timer
		if (get_now_tv()->tv_sec > notify_tm && get_now_tv()->tv_sec < kick_tm) {

			set_battle_time_limit(0);
			//kick user directly at 23:59:30
			ADD_TIMER_EVENT_EX(&g_events, kick_all_users_offline_idx, (void*)2, kick_tm);
			KDEBUG_LOG(0, "ADD KICK TIMER:%ld ", kick_tm - get_now_tv()->tv_sec);

			// notify at 23:45:00 tomorrow
			uint32_t tmp_tm = notify_tm + day_tv;
			ADD_TIMER_EVENT_EX(&g_events, kick_all_users_offline_idx, (void*)1, tmp_tm);
			KDEBUG_LOG(0, "ADD NOTIFY TIMER:%ld ", tmp_tm - get_now_tv()->tv_sec);
		} else if (get_now_tv()->tv_sec <= notify_tm ) {
			set_battle_time_limit(0);
			// notify at 23:45:00
			ADD_TIMER_EVENT_EX(&g_events, kick_all_users_offline_idx, (void*)1, notify_tm);
			KDEBUG_LOG(0, "ADD NOTIFY TIMER:%ld ", notify_tm - get_now_tv()->tv_sec);
		} else {
			set_battle_time_limit(1);
			// notify at 23:45:00 tomorrow
			uint32_t tmp_tm = notify_tm + day_tv;
			ADD_TIMER_EVENT_EX(&g_events, kick_all_users_offline_idx, (void*)1, tmp_tm);
			KDEBUG_LOG(0, "ADD NOTIFY TIMER:%ld ", tmp_tm - get_now_tv()->tv_sec);
		}
	} else if (data == reinterpret_cast<void*>(1)) {
		// reset kick timer,make it run after 24hours
		uint32_t tm_notify_tm = notify_tm + day_tv;
		ADD_TIMER_EVENT_EX(&g_events, kick_all_users_offline_idx, (void*)1, tm_notify_tm);
		KDEBUG_LOG(0, "ADD NOTIFY TIMER:%ld ", tm_notify_tm - get_now_tv()->tv_sec);

#ifndef VERSION_KAIXIN
		// send a pkg to inform the players that server is going to close
		int idx = sizeof(cli_proto_t);
		init_cli_proto_head_full(pkgbuf, 0, 0, cli_proto_login, idx, cli_err_closed_time);
		send_to_all(pkgbuf, idx);
#endif		
		// kick all players offline after 15 mins, 15 sec before 24:00
		ADD_TIMER_EVENT_EX(&g_events, kick_all_users_offline_idx, (void*)2, kick_tm);
		KDEBUG_LOG(0, "ADD KICK TIMER:%ld ", kick_tm - get_now_tv()->tv_sec);
	} else if (data == reinterpret_cast<void*>(2)) {
		g_fanfan_mrg.daily_reset();
		set_battle_time_limit(1);
		KDEBUG_LOG(0, "KICK ALL USERS OFFLINE\t[usr_cnt=%u]", player_num);
		g_swap_action_cnt_1 = 0;
		g_swap_action_cnt_2 = 0;
		g_swap_action_cnt_3 = 0;
		g_swap_action_cnt_4 = 0;
		traverse_players_remove(del_player);
	}
	return 0;
}

#endif


