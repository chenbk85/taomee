#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee/project/utilities.h>
#include <libtaomee++/utils/md5.h>

extern "C" {
#include <async_serv/mcast.h>
}
using namespace taomee;

#include "battle.hpp"
#include "common_op.hpp"
#include "utils.hpp"
#include "message.hpp"
#include "player.hpp"
#include "cachesvr.hpp"
#include "switch.hpp"
#include "login.hpp"
#include "trade.hpp"
#include "cache_switch.hpp"
#include "global_data.hpp"
#include "mcast_proto.hpp"
#include "limit.hpp"
#include "home.hpp"
#include "player_attribute.hpp"
#include "master_prentice.hpp"
#include "chat.hpp"
#include "other_active.hpp"
#include "achievement.hpp"
//-----------------------------------------------------------
// static function declarations
//-----------------------------------------------------------
static int walk(player_t* p, uint32_t xpos, uint32_t ypos, uint32_t flag);
static int walk_keyboard(player_t* p, uint32_t xpos, uint32_t ypos, uint8_t dir, uint8_t state);
static int stand(player_t* p, uint32_t xpos, uint32_t ypos, uint8_t dir);
static int jump(player_t* p, uint32_t xpos, uint32_t ypos);


//static int send_walk_rsp(player_t* p, uint32_t xpos, uint32_t ypos, uint32_t flag);
//static int send_walk_keyboard_rsp(player_t* p, uint32_t xpos, uint32_t ypos, uint8_t dir, uint8_t state);
//static int send_stand_rsp(player_t* p, uint32_t xpos, uint32_t ypos, uint8_t dir);
//static int send_jump_rsp(player_t* p, uint32_t xpos, uint32_t ypos);
//
//--------------------------------------------------------------------
/**
 * @brief for the platform of statistics (0< answer < 6)
 */
inline void do_stat_log_questionnaire(uint32_t uid, uint32_t seq, uint32_t answer)
{
    uint32_t buf[6] = {0,0,0,0,0,0};
    buf[answer] = uid;
    uint32_t cmd = stat_log_questionnaire + seq;
    msglog(statistic_logfile, cmd, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [uid=%u] [seq=%u] [answer=%u]", cmd, uid, seq, answer);
}

//--------------------------------------------------------------------

int stat_log_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t type, times, flag;
	unpack(body, type, i);
	unpack(body, times, i);
	unpack(body, flag, i);

	KDEBUG_LOG(p->id ,"STAT LOG\t[%u %u %u]", type, times, flag);
	
	switch (type) {
		case 1:
			do_stat_log_universal_interface_1(stat_log_scan_times_chapter, 0, 1);
			do_stat_log_universal_interface_1(stat_log_scan_times_chapter1, 0, 1);
			break;
		case 2:
			//do_stat_log_universal_interface_1(stat_log_user_come_back, 0, 1);
			break;
		case 3:
		case 4:
		case 5:
		case 6:
			do_stat_log_universal_interface_1(0x09800001, type - 3, 1);
			break;
		case 7:
			do_stat_log_universal_interface_1(0x09800005, 0, 1);
			break;
		case 8:
			WARN_LOG("adobe_debug : %u %u %u %u", p->id, p->reg_tm, p->role_type, p->role_tm);
			do_stat_log_universal_interface_2(0x09010405, 0, p->id, 1);
			break;
		case 9:
			do_stat_log_universal_interface_1(0x09500202, 0, p->id);
			break;
		case 10:
			do_stat_log_universal_interface_1(0x09526008, 0, p->id);
			break;
        case 11: // seer2 associated operation
			do_stat_log_universal_interface_1(0x09526203, 0, p->id);
            break;
		case 12: // mole kart 
			do_stat_log_universal_interface_1(0x09526204, 0, p->id);
            break;
		case 13: // qq 
			{
			}
            break;


		default:
			break;
	}
	p->waitcmd = 0;
	return 0;
}

/**
  * @brief player walks
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int walk_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t x, y, flag;
	unpack(body, x, i);
	unpack(body, y, i);
	unpack(body, flag, i);

	TRACE_LOG("id[%u]:x[%d],y[%d]",p->id,x,y);
	if(p->trade_grp != 0)
	{
		if (p->trade_grp->trade_type) {
			return send_header_to_player(p, p->waitcmd, cli_err_trade_shop_busy, 1);
		}
		return trsvr_player_walk(p, x, y, flag);
	}
	if(p->home_grp != 0)
	{
		p->waitcmd = 0;
		return homesvr_player_walk(p, x, y, flag);
	}
	return walk(p, x, y, flag);
}

/**
  * @brief player moves with keyboard
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int walk_keyboard_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t x, y;
	uint8_t dir;
	uint8_t state;
	unpack(body, x, i);
	unpack(body, y, i);
	unpack(body, dir, i);
	unpack(body, state, i);
	if (p->trade_grp != 0)
	{
		if (p->trade_grp->trade_type) {
			return send_header_to_player(p, p->waitcmd, cli_err_trade_shop_busy, 1);
		}
		return trsvr_player_walk_keyboard(p, x, y, dir, state);
	}
	if( p->home_grp != 0)
	{
		p->waitcmd = 0;
		return homesvr_walk_keyboard(p, x, y, dir, state);
	}

	TRACE_LOG("id[%u]:x[%d],y[%d],dir=[%d],state=[%d]",p->id,x,y,dir,state);
	return walk_keyboard(p, x, y, dir, state);
}

/**
  * @brief player stands
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int stand_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t x, y;
	uint32_t old_x, old_y;
	uint8_t dir;
	
	unpack(body, old_x, i);
	unpack(body, old_y, i);
	unpack(body, x, i);
	unpack(body, y, i);
	unpack(body, dir, i);
	TRACE_LOG("%u:%u  %u:%u %d", old_x, x, old_y, y, dir);
	
	if( p->battle_grp != 0)
	{
		return btlsvr_player_stop(p, old_x, old_y, x, y, dir);
	}
	else if (p->trade_grp != 0)
	{
		return trsvr_player_stand(p, x, y, dir);
	}
	else if( p->home_grp != 0)
	{
		p->waitcmd = 0;
		return homesvr_player_stop(p, x, y, dir);
	}
	return stand(p, x, y, dir);
}

/**
  * @brief player jump
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int jump_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t x, y;
	uint32_t old_x, old_y;
	unpack(body, old_x, i);
	unpack(body, old_y, i);
	unpack(body, x, i);
	unpack(body, y, i);
	
	if(p->battle_grp != 0)
	{
		return btlsvr_player_jump(p, old_x, old_y, x, y);
	}
	else if (p->trade_grp != 0)
	{
		if (p->trade_grp->trade_type) {
			return send_header_to_player(p, p->waitcmd, cli_err_trade_shop_busy, 1);
		}
		return trsvr_player_jump(p, x, y);
	}
	else if(p->home_grp != 0)
	{
		p->waitcmd = 0;
		return homesvr_player_jump(p, x, y);
	}
	return jump(p, x, y);
}

/**
  * @brief player add friend
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int add_friend_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	userid_t friend_id;
	unpack(body, friend_id, i);

	
	if ((friend_id == p->id) || !is_valid_uid(friend_id)) {
		ERROR_LOG("invalid id\t[%u %u]", p->id, friend_id);
		return -1;							
	}
	
	/*
	send_simple_notification(friend_id, p, cli_proto_add_friend, 0, p->cur_map);
	send_header_to_player(p, cli_proto_add_friend, 0, 1);
	KDEBUG_LOG(p->id, "ADD FRIEND REQ\t[%u %u]", p->id, friend_id);
	return 0;*/
	return db_query_forbiden_friend_flag(p, friend_id);
}

/**
  * @brief reply to add friend request
  * @param p the player who replies the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int reply_add_friend_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t initiator; //adder
	uint32_t accept;

	int i = 0;
	unpack(body, initiator, i);
	unpack(body, accept, i);
    
	if ((initiator == p->id) || !is_valid_uid(initiator)) {
		ERROR_LOG("invalid id\t[addee=%u adder=%u]", p->id, initiator);
		return -1;
	}

	KDEBUG_LOG(p->id, "REPLY ADD FRIEND\t[addee=%u adder=%u accept=%d]", p->id, initiator, accept);

	if (!accept) { //reject the request
		send_header_to_player(p, cli_proto_reply_add_friend, 0, 1);
		send_simple_notification(initiator, p, cli_proto_reply_add_friend, 0, p->cur_map);
	} else {
		player_t* p_friend = get_player(initiator);
		if (p_friend) {
			add_player_friend(p_friend, p->id);
		}
		add_player_friend(p, initiator);
		send_request_to_db(0, p->id, p->role_tm, dbproto_add_friend, &initiator, sizeof(initiator));
		return send_request_to_db(p, initiator, 0, dbproto_add_friend, &(p->id), sizeof(p->id));
	}   
	
	return 0;
}

/**
  * @brief player deletes friend
  * @param p the player who decides to delete friends
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int del_friend_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	userid_t friend_id;
	unpack(body, friend_id, i);

	if (!is_valid_uid(friend_id)) {
		ERROR_LOG("invalid id\t[%u %u]", p->id, friend_id);
		return -1;
	}

	erase_player_friend(p, friend_id);
	/* 有师徒关系立即返回错误码 */
	/*if (p->mentor == friend_id) {
		return send_header_to_player(p, p->waitcmd, cli_err_del_friend_have_mentor, 1);
	}

	if (p->apprentice == friend_id) {
		return send_header_to_player(p, p->waitcmd, cli_err_del_friend_have_apprentice, 1);
	}*/

	KDEBUG_LOG(p->id, "DEL FRIEND REQ\t[%u %u]", p->id, friend_id);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_del_friend, &friend_id, sizeof(friend_id));
}

/**
  * @brief player add friend to blacklist
  * @param p the player who decides to add friend to blacklist
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int add_blacklist_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	userid_t friend_id;
	unpack(body, friend_id, i);

	if ((p->id == friend_id) || !is_valid_uid(friend_id)) {
		ERROR_LOG("invalid id\t[%u %u]", p->id, friend_id);
		return -1;
	}
	erase_player_friend(p, friend_id);
	/* 有师徒关系立即返回错误码 */
	/*if (p->mentor == friend_id) {
		return send_header_to_player(p, p->waitcmd, cli_err_add_blacklist_have_mentor, 1);
	}

	if (p->apprentice == friend_id) {
		return send_header_to_player(p, p->waitcmd, cli_err_add_blacklist_have_apprentice, 1);
	}*/

	KDEBUG_LOG(p->id, "ADD BLACKLIST\t[%u %u]", p->id, friend_id);


	/* delete self from friend's and my own friend list */
	send_request_to_db(0, friend_id, 0, dbproto_del_friend, &(p->id), sizeof(p->id));
	//send_request_to_db(p, p->id,  dbproto_del_friend, &friend_id, sizeof(friend_id));		

	return send_request_to_db(p, p->id, 0, dbproto_add_blacklist, &friend_id, sizeof(friend_id));		
}

/**
  * @brief player get blacklist
  * @param p the player who decides to get blacklist
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_blacklist_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_blacklist, 0, 0);
}

/**
  * @brief check users onoff status
  * @param p the player who send the checking request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int check_users_onoff_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0, idx2 = 0;
	uint32_t count;
	userid_t uid;

	unpack(body, count, idx);
	CHECK_VAL_LE(count, 200);
	CHECK_VAL_EQ(bodylen, 4 + count * sizeof(userid_t));

	pack_h(pkgbuf, count, idx2);

	for (uint32_t i = 0; i < count; ++i) {
		unpack(body, uid, idx);
		pack_h(pkgbuf, uid, idx2);
		//TRACE_LOG("uid=%u",uid);
	}

	chk_users_onoff_across_svr(p, pkgbuf, idx2);

	return 0;
}

int get_encryption_session_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    /*int idx = 0; 
    pack_h(dbpkgbuf, 10000, idx);
    pack_h(dbpkgbuf, p->fdsess->remote_ip, idx);
    return send_request_to_db_no_roletm(p, p->id, dbproto_add_session, dbpkgbuf, idx);
	*/

	int idx = 0;
	uint32_t gf_flag = 10000;
	unpack(body, gf_flag, idx);
	 
	const uint32_t verify_len = 32;
	char verify_code[verify_len];

	//get md5 by send_body
	static utils::MD5 md5;
	const uint32_t verify_buf_len = 1024;
	static char verify_buf[verify_buf_len] = {0};
	
	uint32_t s_len = 0;
	static uint16_t m_channel_id = config_get_intval("channel_id", 0);
	static uint32_t m_security_code = config_get_intval("security_code", 0);
	memset(verify_buf, 0x00, verify_buf_len);
	sprintf(verify_buf, "channelId=%d&securityCode=%u&data=", m_channel_id, m_security_code);
	s_len = strlen(verify_buf);

	idx = 0;
	pack_h(verify_buf + s_len, gf_flag, idx);
	pack_h(verify_buf + s_len, p->fdsess->remote_ip, idx);
	md5.reset();
	md5.update(verify_buf, s_len + idx);
	memcpy(verify_code, md5.toString().c_str(), verify_len);

	KDEBUG_LOG(p->id, "GET SESS\t[gameflg:%u ip:%04x]", gf_flag, p->fdsess->remote_ip);
	//send pack
	idx = 0;
	pack_h(dbpkgbuf, m_channel_id, idx);
	pack(dbpkgbuf, verify_code, verify_len, idx);
	pack_h(dbpkgbuf, gf_flag, idx);
	pack_h(dbpkgbuf, p->fdsess->remote_ip, idx);
	return send_request_to_db_no_roletm(p, p->id, dbproto_add_session, dbpkgbuf, idx);
}

int get_questionnaire_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0, pos = 0;
    uint32_t count = 0, answer_seq = 0, answer = 0;
    char out[2048] = {0};
    
    unpack(body, count, idx);
    CHECK_VAL_EQ(bodylen, 4 + count * 8);

    memset(out, 0, 2048);
    for (uint32_t i = 0; i < count; i++) {
        unpack(body, answer_seq, idx);
        unpack(body, answer, idx);
        pos += sprintf(out + pos, "%u %u ", answer_seq, answer);
        if (answer < 7) {
            do_stat_log_questionnaire(p->id, answer_seq, answer-1);
        }
    }
    WARN_LOG(" answer : [%u %s]", p->id, out);
    return send_header_to_player(p, p->waitcmd, 0, 1);
}

int market_user_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	body[299] = 0x0;
    WARN_LOG("market: %u %s", p->id, body);
    p->waitcmd = 0;
	return 0;
}

/**
 * @brief finger guessing
 */
int  finger_guessing_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t swap_id = 0;
    unpack(body, swap_id, idx);
    if (swap_id < 1090 || swap_id > 1108) {
        return -1;
    }
    uint32_t limit_times = p->act_record[act_record_pos] + (is_vip_player(p) ? 5 : 2);
    uint32_t finger_time = get_swap_action_times(p, 1087);
    if ( finger_time >= limit_times || finger_time >= 17 ) {
        return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
    }
    swap_action_data* p_data = g_swap_action_mrg.get_swap_action(swap_id);
    if (p_data == NULL) {
        return -1;
    }

    uint32_t item_id = p_data->rewards[0].give_id;
    uint32_t item_type = p_data->rewards[0].give_type;

    const GfItem* itm = items->get_item(item_id);
    if ( itm && is_valid_montype(itm->summon_id())  && item_id != 1500362) {
        if (check_summon_type_exist(p, itm->summon_id()) ||
            p->my_packs->is_item_exist(item_id)) {
            return send_header_to_player(p, p->waitcmd, cli_err_same_summon_exist, 1);
        }
    }

    uint32_t rand_num = rand() % 1000;

    uint32_t flag = 0;
    if(is_vip_player(p)) {
        if (rand_num > 400 && rand_num <= 500) {
            flag = 1;
        }
    } else {
        if ( item_type == 3 && item_id == 10) { //侠士值
            return send_header_to_player(p, p->waitcmd, cli_err_limit_vip_act, 1);
        }
        if (rand_num > 250 && rand_num <= 300) {
            flag = 1;
        }
    }

    uint32_t done = get_swap_action_times(p, 1088);
    uint32_t wins = get_swap_action_times(p, 1089);

    if (done == 10) {
        //add message: finger guessing clear zero
        clear_swap_action_times(p, 1088);
        clear_swap_action_times(p, 1089);
        done = 0;
        wins = 0;
    }

    add_swap_action_times(p, 1088);
    done += 1;

    if (flag == 1) {
        add_swap_action_times(p, 1089);
        wins += 1;
    }

    TRACE_LOG("%u, finger_guessing [%u %u %u]", p->id, flag, done, wins);
    idx = sizeof(cli_proto_t); 
    pack(pkgbuf, flag, idx);
    pack(pkgbuf, done, idx);
    pack(pkgbuf, wins, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}
/**
 * @brief get finger guessing data
 */
int get_finger_guessing_data_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    uint32_t done = get_swap_action_times(p, 1088);
    uint32_t wins = get_swap_action_times(p, 1089);

    TRACE_LOG("%u, finger_guessing [%u %u]", p->id, done, wins);
    int idx = sizeof(cli_proto_t); 
    pack(pkgbuf, done, idx);
    pack(pkgbuf, wins, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int get_hunter_self_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	TRACE_LOG("%u, bodylen %u", p->id, bodylen);
	//return sent_get_hunter_top_rsp(p);
	int idx = 0;
	pack_h(dbpkgbuf, p->role_type, idx);
    return send_request_to_db(p, p->id, p->role_tm, dbproto_get_hunter_self_info, dbpkgbuf, idx);
}

int db_get_hunter_self_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    db_hunter_info_t* hunter = reinterpret_cast<db_hunter_info_t*>(body);
    TRACE_LOG("hunter info: uid=[%u] %u %u %u", p->id, hunter->layer, hunter->interval, hunter->order);
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, hunter->order, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    send_to_player(p, pkgbuf, idx, 1);
	if (hunter->order) {
		proc_ranker_range_achieve_logic(p, fumo_ranker_type, 1, hunter->order - 1);
	}
	return 0;
}

int get_hunter_top_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	TRACE_LOG("%u, bodylen %u", p->id, bodylen);
	int idx = 0;
	uint32_t role_type = 0;
	unpack(body, role_type, idx);
	if (role_type >= 1 && role_type <= max_role_type) {
		int idx = 0;
		pack_h(dbpkgbuf, role_type, idx);
    	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_hunter_top_info, dbpkgbuf, idx);
	} else {
		ERROR_LOG("ERROR ROLE _TYPE [%u %u]", p->id, role_type);
		return -1;
	}

}

/**
 * @brief Analyse player on-offline information
 * @return times of on-offline running days
 */
uint32_t get_running_days_online_cmd(player_t* p,uint8_t* body, uint32_t bodylen)
{
    time_t time_point = get_now_tv()->tv_sec;
    struct tm *p_tmp = localtime( &time_point ); 
    p_tmp->tm_year  = 0;
    p_tmp->tm_mon   = 0;
    p_tmp->tm_mday  = 0;

    time_point = mktime(p_tmp) - (4 * 3600 * 12);

    int idx = 0;
    pack_h(dbpkgbuf, static_cast<uint32_t>(time_point), idx);
    return send_request_to_db(p, p->id, p->role_tm, dbproto_get_hunter_top_info, dbpkgbuf, idx);
}


//---------------------------------------------------------------------------------
// DB callback function
//---------------------------------------------------------------------------------
int db_get_running_days_online_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    int idx = 0;
    uint32_t cnt = 0;
    unpack_h(body, cnt, idx);
    CHECK_VAL_EQ(bodylen, cnt * sizeof(on_off_line_t) + 4);

    time_t time_point = get_now_tv()->tv_sec;
    struct tm *p_tmp = localtime( &time_point ); 
    p_tmp->tm_year  = 0;
    p_tmp->tm_mon   = 0;
    p_tmp->tm_mday  = 0;

    time_point = mktime(p_tmp) - (4 * 3600 * 24);


    on_off_line_t * on_off = reinterpret_cast<on_off_line_t*>((uint8_t*)body + 4);

    for (uint32_t i = 0; i < cnt; i++) {
        if ( on_off[i].on_time >= time_point && on_off[i].on_time < time_point + 86400) {
            p->on_off_line[0]->push_back(on_off[i]);
        } else if (on_off[i].on_time >= time_point + 86400 && on_off[i].on_time < time_point + 86400 * 2 ) {
        } else if (on_off[i].on_time >= time_point + 86400 * 2 && on_off[i].on_time < time_point + 86400 * 3 ) {
        } else if (on_off[i].on_time >= time_point + 86400 * 3 && on_off[i].on_time < time_point + 86400 * 4 ) {
        } else if (on_off[i].on_time >= time_point + 86400 * 4 && on_off[i].on_time < time_point + 86400 * 5 ) {
        }
        TRACE_LOG("uid=[%u] on off line info [%lu %lu]", id, on_off[i].on_time, on_off[i].off_time);
    }
    return 0;
}

int db_get_hunter_top_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{

    CHECK_DBERR(p, ret);

    db_hunter_top_header_t* hunter_header = reinterpret_cast<db_hunter_top_header_t*>(body);
    TRACE_LOG("hunter header: uid=[%u] %u %u %u %u", p->id, hunter_header->layer, hunter_header->interval, hunter_header->order, hunter_header->cnt);

    CHECK_VAL_EQ(bodylen, sizeof(db_hunter_top_header_t) + hunter_header->cnt * sizeof(db_hunter_top_info));

    db_hunter_top_info * hunter = NULL;
    int idx = sizeof(cli_proto_t);
	if (hunter_header->layer) {
    	pack(pkgbuf, hunter_header->layer, idx);
	    pack(pkgbuf, hunter_header->interval, idx);
		pack(pkgbuf, hunter_header->order + 1, idx);
	} else {
		pack(pkgbuf, p->fumo_tower_top, idx);
	    pack(pkgbuf, p->fumo_tower_used_tm, idx);
		pack(pkgbuf, 0, idx);
	}
    
    pack(pkgbuf, hunter_header->cnt, idx);

    hunter = reinterpret_cast<db_hunter_top_info*>(hunter_header->data);
    for (uint32_t i = 0; i < hunter_header->cnt; i++) {
        pack(pkgbuf, hunter[i].userid, idx);
        pack(pkgbuf, hunter[i].roletm, idx);
        pack(pkgbuf, hunter[i].role_type, idx);
        pack(pkgbuf, hunter[i].nick, sizeof(hunter->nick), idx);
        pack(pkgbuf, hunter[i].layer, idx);
        pack(pkgbuf, hunter[i].interval, idx);
        pack(pkgbuf, hunter[i].userlv, idx);
        TRACE_LOG("hunter: %u %u", hunter[i].userid, hunter[i].roletm);
    }
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int db_add_session_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);

	char out[32];
	//no role time
	hex2str((uint8_t*)(body) - 4, 16, out);

	TRACE_LOG("add session[%16s]", (uint8_t*)(body) - 4);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, out, 32, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

//-----------------------------------------------------------
// static function declarations
//-----------------------------------------------------------

/**
  * @brief player walks
  * @param p the player
  * @param xpos walks to x pos of the map
  * @param ypos walks to y pos of the map
  * @return 0 on success, -1 on error
  */
static int walk(player_t* p, uint32_t xpos, uint32_t ypos, uint32_t flag)
{
	//reset_common_action (p);
	p->direction = 0;
	p->xpos      = xpos;
	p->ypos      = ypos;
//	ERROR_LOG("%u WALK POS X=%u Y=%u", p->id, p->xpos, p->ypos);
	// pack walk info and send it to other players
	int i = sizeof(cli_proto_t);

	pack(pkgbuf, p->role_type, i);
	pack(pkgbuf, xpos, i);
	pack(pkgbuf, ypos, i);
	pack(pkgbuf, flag, i);
	init_cli_proto_head(pkgbuf, p, cli_proto_walk, i);

	send_to_map(p, pkgbuf, i, 1);
	return 0;
}


//static int send_walk_rsp(player_t* p, uint32_t xpos, uint32_t ypos, uint32_t flag)
//{
//	int i = sizeof(cli_proto_t);
//
//	pack(pkgbuf, p->role_type, i);
//	pack(pkgbuf, xpos, i);
//	pack(pkgbuf, ypos, i);
//	pack(pkgbuf, flag, i);
//	init_cli_proto_head(pkgbuf, p, cli_proto_walk, i);
//	return send_to_player(p, pkgbuf, i, 1);
//}
/**
  * @brief player moves with keyboard
  * @param p the player
  * @param xpos walks to x pos of the map
  * @param ypos walks to y pos of the map
  * @return 0 on success, -1 on error
  */
static int walk_keyboard(player_t* p, uint32_t xpos, uint32_t ypos, uint8_t dir, uint8_t state)
{
	//reset_common_action (p);
	p->direction = dir;
//	p->xpos      = xpos;
//	p->ypos      = ypos;
//	ERROR_LOG("%u WALK_KEY POS X=%u Y=%u", p->id, p->xpos, p->ypos);

	// pack walk info and send it to other players
	int i = sizeof(cli_proto_t);

	pack(pkgbuf, p->role_type, i);
	pack(pkgbuf, xpos, i);
	pack(pkgbuf, ypos, i);
	pack(pkgbuf, dir, i);
	pack(pkgbuf, state, i);
	init_cli_proto_head(pkgbuf, p, cli_proto_walk_keyboard, i);

	send_to_map(p, pkgbuf, i, 1);
	return 0;
}

//static int send_walk_keyboard_rsp(player_t* p, uint32_t xpos, uint32_t ypos, uint8_t dir, uint8_t state)
//{
//	// pack walk info and send it to other players
//	int i = sizeof(cli_proto_t);
//
//	pack(pkgbuf, p->role_type, i);
//	pack(pkgbuf, xpos, i);
//	pack(pkgbuf, ypos, i);
//	pack(pkgbuf, dir, i);
//	pack(pkgbuf, state, i);
//	init_cli_proto_head(pkgbuf, p, cli_proto_walk_keyboard, i);
//
//	return send_to_player(p, pkgbuf, i, 1);
//}

/**
  * @brief player stands
  * @param p the player
  * @param xpos walks to x pos of the map
  * @param ypos walks to y pos of the map
  */
static int stand(player_t* p, uint32_t xpos, uint32_t ypos, uint8_t dir)
{
	p->xpos = xpos;
	p->ypos = ypos;
	p->direction = dir;

//	ERROR_LOG("%u STAND POS X=%u Y=%u", p->id, p->xpos, p->ypos);

	int len = pack_stand_pkg(pkgbuf, p, p->id, xpos, ypos, dir);
	send_to_map(p, pkgbuf, len, 1);
	return 0;
}

//static int send_stand_rsp(player_t* p, uint32_t xpos, uint32_t ypos, uint8_t dir)
//{
//	int len = pack_stand_pkg(pkgbuf, p, p->id, xpos, ypos, dir);
//	return send_to_player(p, pkgbuf, len, 1);
//}


/**
  * @brief player jumps
  * @param p the player
  * @param xpos walks to x pos of the map
  * @param ypos walks to y pos of the map
  * @return 0 on success, -1 on error
  */
static int jump(player_t* p, uint32_t xpos, uint32_t ypos)
{
	//reset_common_action (p);
	//p->direction = 0;
	p->xpos      = xpos;
	p->ypos      = ypos;

//	ERROR_LOG("%u JUMP POS %u %u", p->id,  p->xpos, p->ypos);

	// pack walk info and send it to other players
	int i = sizeof(cli_proto_t);

	pack(pkgbuf, xpos, i);
	pack(pkgbuf, ypos, i);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, i);

	send_to_map(p, pkgbuf, i, 1);
	return 0;
}

//static int send_jump_rsp(player_t* p, uint32_t xpos, uint32_t ypos)
//{
//	// pack walk info and send it to other players
//	int i = sizeof(cli_proto_t);
//
//	pack(pkgbuf, xpos, i);
//	pack(pkgbuf, ypos, i);
//	init_cli_proto_head(pkgbuf, p, p->waitcmd, i);
//
//	return send_to_player(p, pkgbuf, i, 1);
//}

//---------------------------------------------------------------------------
// callback: process on db return and send callback packages to client
//---------------------------------------------------------------------------
/**
  * @brief callback for handling adding friends returned from dbproxy
  * @param p the requester
  * @param id id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */
int db_add_friend_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	//userid_t friend_id = *(reinterpret_cast<userid_t*>(body)); 
	if (ret) {
		player_t* recver = get_player(id);
		if (recver) {
			send_header_to_player(recver, cli_proto_add_friend, cli_err_base_dberr + ret, 0);
		}
		send_header_to_player(p, p->waitcmd, 0, 1);
		return 0;
	}
	send_header_to_player(p, p->waitcmd, 0, 1);
	// notify the requester
	send_simple_notification(id, p, cli_proto_reply_add_friend, 1, p->cur_map);

	return 0;
}

/**
  * @brief callback for handling deleting friends returned from dbproxy
  * @param p the requester
  * @param id id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */
int db_del_friend_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	send_header_to_player(p, cli_proto_del_friend, 0, 1);

	return 0;
}

/**
  * @brief callback for handling adding blacklist returned from dbproxy
  * @param p the requester
  * @param id id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */
int db_add_blacklist_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	userid_t uid = *(reinterpret_cast<userid_t*>(body)); 
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, uid, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);

	send_to_player(p, pkgbuf, idx, 1);

	return 0;
}

/**
  * @brief callback for handling deleting blacklist returned from dbproxy
  * @param p the requester
  * @param id id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */
int db_del_blacklist_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	send_header_to_player(p, p->waitcmd, 0, 1);

	return 0;
}

/**
  * @brief callback for handling getting blacklist returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */
int db_get_blacklist_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	uid_list_t* blacklist = reinterpret_cast<uid_list_t*>(body);

	CHECK_VAL_LE(blacklist->cnt, 100);
	CHECK_VAL_EQ(bodylen, sizeof(uid_list_t) + blacklist->cnt * sizeof(userid_t));

	//send request package to cache server
	if (blacklist->cnt > 0) {
		int idx = 0;
		pack_h(pkgbuf, blacklist->cnt, idx);
		for (uint32_t i = 0; i < blacklist->cnt; i++) {
			pack_h(pkgbuf, blacklist->id[i], idx);
		}
		return 0;
//		return send_request_to_cachesvr(p, p->id, cachesvr_get_timestamp, pkgbuf, idx);
 	} else {
		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, blacklist->cnt, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
 	}

	return 0;
}

void friend_login_op(const void *body, uint32_t bodylen)
{
	if (bodylen >= sizeof(friend_login_t))
	{
		const friend_login_t * pkg = reinterpret_cast<const friend_login_t*>(body);
		uint32_t login = pkg->userid;
		//TRACE_LOG("mcast user's friend login[%u %s]", login, pkg->usernick);
		if (bodylen == (sizeof(friend_login_t) + (pkg->friend_cnt) * sizeof(userid_t)))
		{
			for(uint32_t i = 0; i < pkg->friend_cnt; ++i)
			{
				player_t * p = get_player(pkg->friends[i]);
				if (p)
				{
					TRACE_LOG("get msg usr friend login[%u %u %s]", login, p->id, pkg->usernick); 
					int idx = sizeof(cli_proto_t);
					pack(pkgbuf, login, idx);
					pack(pkgbuf, pkg->usernick, 16, idx);
					init_cli_proto_head(pkgbuf, p, cli_proto_friend_login, idx);
					send_to_player(p, pkgbuf, idx, 0);
				}

			}
		}
	}

}	

void update_limit_data(const void *body, uint32_t bodylen)
{
struct limit_data_mcast_header {
	uint32_t type;//0 reset; 1 update
	uint32_t cnt;
};

struct limit_data_mcast_item {
	uint32_t type;
	uint32_t item_id;
	uint32_t item_cnt;
};
	const limit_data_mcast_header* head = reinterpret_cast<const limit_data_mcast_header*>(body);
	const limit_data_mcast_item* item = reinterpret_cast<const limit_data_mcast_item*>((reinterpret_cast<const limit_data_mcast_header*>(body) + 1));

	if (head->type == 0) {
	//reset all data
		g_limit_data_mrg.init();
		for (uint32_t i = 0; i < head->cnt; i++) {
			global_limit_data_t data;
			data.type = item->type;
			data.item_id = item->item_id;
			data.left_cnt = item->item_cnt;
			TRACE_LOG("%u %u %u %u", item->type, head->cnt, item->item_id, item->item_cnt);
			g_limit_data_mrg.add_limit_data(data);
			item++;
		}
	} else {
		for (uint32_t i = 0; i < head->cnt; i++) {
			TRACE_LOG("%u %u %u",head->cnt, item->item_id, item->item_cnt);
			g_limit_data_mrg.update_item_cnt(item->item_id, item->item_cnt);
			item++;
		}
	}
}	
/**
 * @brief Interface: notify player mcast information (loop mode)
 */
int notify_official_mcast_info(uint32_t userid, uint32_t mcast_cmd, char* info)
{
    int idx = sizeof(cli_proto_t);

    TRACE_LOG("world notice info [%s ]", info);
    pack(pkgbuf, static_cast<uint32_t>(0), idx);
    pack(pkgbuf, static_cast<uint32_t>(2), idx);
    pack(pkgbuf, static_cast<uint32_t>(1), idx);
    pack(pkgbuf, info, max_official_mcast_size, idx);	
    init_cli_proto_head(pkgbuf, 0, cli_proto_mcast_official_notice, idx);
    send_to_all(pkgbuf, idx);
    return 0;
}


/**
 * @brief Interface: notify player mcast information
 */
int notify_player_mcast_info(player_t* p, uint32_t mcast_cmd, player_mcast_t* info)
{
    int idx = sizeof(cli_proto_t);
	if (info->type == 1) {
		idx += pack_chat_rsp_pkg(pkgbuf + idx, p, 0, talk_type_trade, sizeof(info->info), info->info, false);
	} else {
		idx += pack_chat_rsp_pkg(pkgbuf + idx, p, 0, talk_type_system, sizeof(info->info), info->info, false);
	}
    //pack(pkgbuf, info, sizeof(player_mcast_t), idx);
    KDEBUG_LOG(p->id, "BROADCAST\t cmd=[%u] len=[%u]", mcast_cmd, idx);
    init_cli_proto_head(pkgbuf, 0, cli_proto_chat, idx);

	if (info->type == 1) {
		send_trade_mcast_info(pkgbuf, idx);
	} else {
	    send_to_all(pkgbuf, idx);
	}

    idx = sizeof(mcast_pkg_t);

	if (info->type == 1) {
		idx += pack_chat_rsp_pkg(pkgbuf + idx, p, 0, talk_type_trade, sizeof(info->info), info->info, false);
	} else {
		idx += pack_chat_rsp_pkg(pkgbuf + idx, p, 0, talk_type_system, sizeof(info->info), info->info, false);
	}
	
//	pack(pkgbuf, 0, idx);
//    pack(pkgbuf, info, sizeof(player_mcast_t), idx);
    init_mcast_pkg_head(pkgbuf, mcast_cmd, 0);
    send_mcast_pkg(pkgbuf, idx);
    return 0;
}

void send_trade_mcast_info(uint8_t* buf, uint32_t len)
{
    init_cli_proto_head(buf, 0, cli_proto_chat, len);
    list_head_t * l;
    list_for_each(l, &trade_players.player_list) {
        player_t * player = list_entry(l, player_t, tradehook);
        send_to_player(player, buf, len,  0);
    }
}

int mcast_trade( const void * body, uint32_t bodylen)
{
    CHECK_VAL_EQ(bodylen, sizeof(talk_msg_out_head_t) + max_trade_mcast_size);
	if (bodylen > max_trade_mcast_size + sizeof(talk_msg_out_head_t)) {
		ERROR_LOG("MCAST TRADE ERR %u > %u", bodylen,(uint32_t)(max_trade_mcast_size + sizeof(talk_msg_out_head_t)));
		return 0;
	}
    const talk_msg_out_head_t *rsp = reinterpret_cast<const talk_msg_out_head_t*>((uint8_t*)body);
    TRACE_LOG("got world notice info [%d|%s|%d]", taomee::bswap(rsp->type), rsp->from_nick, taomee::bswap(rsp->from_id));

	uint32_t type = taomee::bswap(rsp->type);
    int idx = sizeof(cli_proto_t);
//    pack(pkgbuf, rsp->type, idx);
//    pack(pkgbuf, rsp->nick, max_nick_size, idx);
//    pack(pkgbuf, rsp->front_id, idx);
//    pack(pkgbuf, rsp->info, max_trade_mcast_size, idx);	
	pack(pkgbuf, body, bodylen, idx);
    if (type == talk_type_system) {
        init_cli_proto_head(pkgbuf, 0, cli_proto_chat, idx);
        send_to_all(pkgbuf, idx);
    } else if (type == talk_type_trade) {
        send_trade_mcast_info(pkgbuf, idx);
    }
    return 0;
}
/**
 * @brief world public notice (player and official)
 */
int world_public_notice(const void* body, uint32_t bodylen)
{
    CHECK_VAL_EQ(bodylen, sizeof(official_mcast_t));

    const official_mcast_t *rsp = reinterpret_cast<const official_mcast_t*>((uint8_t*)body);
    TRACE_LOG("got world notice info [%u %u %u|%s ]", rsp->index, rsp->type, rsp->front_id, rsp->info);
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->index, idx);
    pack(pkgbuf, rsp->type, idx);
    pack(pkgbuf, rsp->front_id, idx);
    pack(pkgbuf, rsp->info, max_official_mcast_size, idx);	
    init_cli_proto_head(pkgbuf, 0, cli_proto_mcast_official_notice, idx);
    send_to_all(pkgbuf, idx);
    return 0;
}

bool if_version_kaixin()
{
	return (g_version_number == 1);
}

bool is_btl_time_limited(player_t* p)
{
	if (p->adult_flg) {
		return false;
	}
	if (p->oltoday >= battle_time_limit) {
		return true;
	}
	return false;
}

int load_char_content(xmlNodePtr cur)
{
	// load unique items from xml file
	cur = cur->xmlChildrenNode;
	uint32_t i = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("content"))) {
			uint32_t id;
			get_xml_prop(id, cur, "id");
			get_xml_prop_raw_str_def(char_content[i].content, cur, "char", "");
			get_xml_prop_raw_str_def(char_content[i].name, cur, "name", "");

			i++;
		}
		cur = cur->next;
	}
	return 0;
}

//------------------------------------------------------------------------
// littie function
//------------------------------------------------------------------------
/**
 * @brief hatch summon daily
 */
uint32_t get_daily_hatch_summon_itemid(uint32_t * cnt, int * day)
{
    //struct tm p_tm = {0}; // 2011-08-26
    //p_tm.tm_year = 2011 - 1900;
    //p_tm.tm_mon  = 8; // [0-11]
    //p_tm.tm_mday = 16;
    //p_tm.tm_hour = 0;
    //p_tm.tm_min  = 0;
    //p_tm.tm_sec  = 0;

    //uint32_t time_point = mktime(&p_tm);
    //sscanf("111-07-26 00:00:00", "%d-%d-%d %d:%d:%d", &(p_tm->tm_year), &(p_tm->tm_mon));
    active_data* pactive = get_active_data_mgr()->get_data_by_active_id(5);

    if (pactive == NULL) {
        return 0;
    }
    uint32_t time_point = pactive->from_;

    //uint32_t last_time = 28;
    static uint32_t item_array[28] = 
        {1500565, 1500568, 1300002, 1300004, 1300005, 1300102, 1500568,
         1500224, 1500225, 1500226, 1300005, 1300102, 1300002, 1500565,
         1500568, 1300002, 1500224, 1500225, 1500226, 1300004, 1500568,
         1500224, 1500225, 1500226, 1300005, 1300102, 1500225, 1500565};

    uint32_t cur_time = get_now_tv()->tv_sec;
    if (time_point + (86400 * 28) < cur_time || time_point > cur_time) {
        return 0;
    }

    uint32_t pos = (cur_time - time_point) / 86400;

    *cnt = 1;
    *day = pos;

    return item_array[pos];
}

bool can_get_summon_item(player_t* p)
{
    int times = get_swap_action_times(p, 1140);
    int limit_times = is_vip_player(p) ? 10 : 15;

    if (times >= limit_times) {
        return true;
    }
    return false;
}

int daily_hatch_summon_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    time_t cur_time = get_now_tv()->tv_sec;
    struct tm *p_tm = localtime( &cur_time );

    if (p->my_packs->get_item_cnt(1410019) != 0 || check_summon_type_exist(p, 1182)) {
        return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
    }

    uint32_t cnt = 0;
    int day = 0;
    uint32_t itemid = get_daily_hatch_summon_itemid(&cnt, &day);
    if (itemid == 0) {
        // game over
        return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
    }
    if (p_tm->tm_hour < 12 || (p_tm->tm_hour > 14 && p_tm->tm_hour < 18) || p_tm->tm_hour > 20) {
        // time limit
        return send_header_to_player(p, p->waitcmd, cli_err_swap_action_not_in_time, 1);
    }
    
    int times = get_swap_action_times(p, 1140);
    if (times > day || times > 15 || !is_swap_action_in_new_day(p, 1140)) {
        // daily limit cli_err_day_limit_time_act
        return send_header_to_player(p, p->waitcmd, cli_err_day_limit_time_act, 1);
    }

    if (p->my_packs->del_item(p, itemid, 1, channel_string_other) < 0) {
        // not found item
        return send_header_to_player(p, p->waitcmd, 103122, 1);
    }
    db_use_item_ex(0, p->id, p->role_tm, itemid, 1, false);

    add_swap_action_times(p, 1140);

    TRACE_LOG("%u, daily hatch summon [%u %u]", p->id, itemid, cnt);
    int idx = sizeof(cli_proto_t); 
    pack(pkgbuf, itemid, idx);
    pack(pkgbuf, static_cast<uint32_t>(times + 1), idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int get_daily_hatch_summon_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    uint32_t cnt = 0;
    int day = 0;
    uint32_t itemid = get_daily_hatch_summon_itemid(&cnt, &day);

    TRACE_LOG("%u, daily hatch summon [%u %u]", p->id, itemid, cnt);
    cnt = get_swap_action_times(p, 1140);
    TRACE_LOG("%u, daily hatch summon times [%u]", p->id, cnt);
    int idx = sizeof(cli_proto_t); 
    pack(pkgbuf, itemid, idx);
    pack(pkgbuf, cnt, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}


int player_city_team(uint32_t uid, uint32_t role_time)
{
	static int team[11] = { 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1};

	time_t now_time = get_now_tv()->tv_sec;

	struct tm * now_tm = localtime(&now_time);

	uint32_t t_value = (now_tm->tm_mday + role_time) % 11; 

	return team[t_value] ? 1 : 2;
}

int get_city_win_team()
{
	static int win[7] = { 1, 0, 1, 0, 1, 1, 0};

	time_t now_time = get_now_tv()->tv_sec;

	struct tm * now_tm = localtime(&now_time);

	uint32_t t_value = (now_tm->tm_mday + 11) % 7;

	uint32_t city_reward_time_id = 7;

	if (get_now_active_data_by_active_id(city_reward_time_id)) {
		return win[t_value] ? 1 : 2;
	}

	return 0;
}

/**
 * @bref 进阶
 */
int db_set_power_user_info(player_t* p)
{
    uint32_t total_sp = 0;
	PlayerSkillMap::iterator it = p->player_skill_map->begin();
	for (; it != p->player_skill_map->end(); ++it) {
		total_sp += it->second.lv;
	}
    total_sp += 5;
	KDEBUG_LOG(p->id, "UPGRADE RESET SKILLS\t%u leftsp:%u", p->id, total_sp);
	int idx = 0;
	pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx);
	pack_h(dbpkgbuf, total_sp, idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(1500387), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(1410026), idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_set_power_user_info, dbpkgbuf, idx);
}

int up_power_user_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    if (p->lv < 45) {
        return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
    }
    if (p->power_user) {
        return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
    }
    bool base_condition = false;
    std::set<uint32_t>::iterator it = p->finished_tasks_set->end();
    for (; it != p->finished_tasks_set->begin(); --it){
        if (*it == 1729) {
            base_condition = true;
        }
    }
    if (!base_condition) {
        return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
    }

    return db_set_power_user_info(p);
}

void broadcast_power_user_event(player_t* p)
{
    char title_name[max_title_len] = {0};
    if (!get_title_name(1004 + p->role_type, other_title, title_name)) {
        return ;
    }
    char words[max_trade_mcast_size] = {0};
    snprintf(words, max_trade_mcast_size, "%s:%s.", char_content[15].content, title_name);
    
    TRACE_LOG("BROADCAST: %s", words);

    player_mcast_t info = {0};
    memset(&info, 0x00, sizeof(player_mcast_t));
    info.type = 2; //:player_market 2:player_world
    info.front_id = 2;
    memcpy(&(info.nick), char_content[12].content, sizeof(info.nick));
    memcpy(&(info.info), words, max_trade_mcast_size);

    notify_player_mcast_info(p, mcast_trade_message, &info);
}

/**
  * @brief callback for handling set_power_user_info returned from dbproxy
  * @param p the requester
  * @param id id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */
int db_set_power_user_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
    db_power_user_info_t *rsp = reinterpret_cast<db_power_user_info_t *>(body);

    p->power_user = rsp->power_user;
    p->skill_bind_num = 0;
    p->player_skill_map->clear();
    p->skill_point = rsp->skill_point;

    p->my_packs->add_item(p, rsp->item_id_1, 1, channel_string_other);
    p->my_packs->add_item(p, rsp->item_id_2, 1, channel_string_other);

    do_special_title_logic(p, 1004 + p->role_type);

    p->maxhp    += get_base_hp(p) * 0.3;
    p->atk      += get_base_hp(p) * 0.3;

    p->hp   =   p->maxhp;

    broadcast_power_user_event(p);

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->power_user, idx);
    pack(pkgbuf, rsp->skill_point, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    send_to_player(p, pkgbuf, idx, 1);

    //send broadcast 
    idx = sizeof(cli_proto_t);
    pack(pkgbuf, p->id, idx);
    pack(pkgbuf, rsp->power_user, idx);
    init_cli_proto_head(pkgbuf, p, cli_proto_synchro_power_user, idx);
    send_to_map(p, pkgbuf, idx, 1);

    db_prentice_graduate(p);
    return 0;
}


//------------------------------------------------------
// 功夫城烟花之夜
//------------------------------------------------------
enum {
    fw_type_scene   = 1,
    fw_type_self_1  = 2,
    fw_type_self_2  = 3,
    fw_type_self_3  = 4,
};

int notify_player_play_fireworks(player_t* p)
{
    uint32_t odd = rand() % 100;
    do_stat_log_universal_interface_1(0x09526202, 0, p->id);
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, p->id, idx);
    pack(pkgbuf, p->role_tm, idx);
    if (odd < 95) {
        uint32_t add_val = rand() % 3;
        pack(pkgbuf, static_cast<uint32_t>(fw_type_self_1 + add_val), idx);
        TRACE_LOG("trace fireworks -----> [%u]", fw_type_self_1 + add_val);
        init_cli_proto_head(pkgbuf, p, cli_proto_notify_client_play_cartoon, idx);
        send_to_map(p, pkgbuf, idx, 1);
    } else {
        pack(pkgbuf, static_cast<uint32_t>(fw_type_scene), idx);
        TRACE_LOG("trace fireworks -----> [%u]", fw_type_scene);
        init_cli_proto_head(pkgbuf, p, cli_proto_notify_client_play_cartoon, idx);
        send_to_player(p, pkgbuf, idx, 1);
    }
    return 0;
}

int notify_rebuild_gf_fireworks(uint32_t type)
{
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, type, idx);
    TRACE_LOG("cws rebuilt fireworks");
    init_cli_proto_head(pkgbuf, 0, cli_proto_client_rebuild_gf_fireworks, idx);
    send_to_all(pkgbuf, idx);
    return 0;
}


void do_fireworks_active_logic()
{
    char words[max_official_mcast_size] = {0};
    snprintf(words, max_official_mcast_size - 1, "%s", char_content[28].content);
    notify_official_mcast_info(0, mcast_world_notice, words);
}

void process_active_broadcast()
{
    time_t cur_time = get_now_tv()->tv_sec;
    static uint32_t last_update_tm = 0;
    if (last_update_tm == 0) {
        last_update_tm = cur_time;
    }

    if (last_update_tm == cur_time || cur_time < last_update_tm + 60) {
        return;
    } else {
        last_update_tm = cur_time;
    }

    //ERROR_LOG("trace ---------> time [%u]", cur_time);

    //struct tm _tm;
    //localtime_r(&cur_time, &_tm);

    do_active_breadcast_logic();

    do_active_event_logic();
	do_active_escort_reward_logic();
}

void send_shengdan_broad_message()
{	
	char words[max_trade_mcast_size] = {0};
	snprintf(words, max_trade_mcast_size, "%s", char_content[29].content);
   	notify_official_mcast_info(0, mcast_world_notice, words);
}

void send_50_lamp_win_message(player_t* p)
{	
	char words[max_trade_mcast_size] = {0};
	snprintf(words, max_trade_mcast_size, "%s%s%s", char_content[46].content, p->nick, char_content[47].content);
   	notify_official_mcast_info(0, mcast_world_notice, words);
}


bool is_in_right_hour(uint32_t begin_hour, uint32_t end_hour)
{
	if (begin_hour >= end_hour || end_hour > 24) {
		return false;
	}
    time_t cur_time = get_now_tv()->tv_sec;
    struct tm _tm;
    localtime_r(&cur_time, &_tm);

    if (_tm.tm_hour >= (int)begin_hour && _tm.tm_hour < (int)end_hour) {
		return true;
	}
	if (_tm.tm_wday == 5 && _tm.tm_hour < 12) {
		return true;
	}
	if (_tm.tm_wday == 4 && _tm.tm_hour > 12) {
		return true;
	}
	return false;	
}

bool is_date(uint32_t mon_day)
{
    time_t cur_time = get_now_tv()->tv_sec;

    struct tm _tm;
    localtime_r(&cur_time, &_tm);

    return ((int)mon_day == _tm.tm_mday);
}

const char* get_date_str()
{
    time_t cur_time = get_now_tv()->tv_sec;

    struct tm _tm;
    localtime_r(&cur_time, &_tm);

    static char date_str[128] = "";

	sprintf(date_str, "%d-%02d-%02d %02d:%02d:%02d", _tm.tm_year + 1900,
					_tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
	return date_str;
}

int list_simple_role_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t list_id = 0;
	unpack(body, list_id, idx);
	if (is_valid_uid(list_id)) {
		int idx = 0;
		pack_h(dbpkgbuf, list_id, idx);
		return send_request_to_db(p, list_id, 0, dbproto_list_simple_role_info, dbpkgbuf, idx);
	}
	return send_header_to_player(p, p->waitcmd, 12334, 1);
}

struct role_simple_info_rsp_t {
	uint32_t userid;
	uint32_t role_regtime;
	uint32_t role_type;
	uint32_t level;
	uint8_t  nick_name[max_nick_size];
}__attribute__((packed));

int db_list_simple_role_info_callback(player_t *p, userid_t id, void * body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int idx = 0;
	uint32_t role_cnt = 0;
	unpack_h(body, role_cnt, idx);
	CHECK_VAL_EQ(bodylen, sizeof(role_simple_info_rsp_t) * role_cnt + sizeof(role_cnt));
	if (p->waitcmd == cli_proto_list_simple_role_info) {
		idx = sizeof(cli_proto_t);
		pack(pkgbuf, role_cnt, idx);
		for (uint32_t i = 0; i < role_cnt; i++) {
			role_simple_info_rsp_t * rsp = reinterpret_cast<role_simple_info_rsp_t*>((char*)body + 4 + sizeof(role_simple_info_rsp_t) * i);
			pack(pkgbuf, rsp->userid, idx);
			pack(pkgbuf, rsp->role_regtime, idx);
			pack(pkgbuf, rsp->role_type, idx);
			pack(pkgbuf, rsp->level, idx);
			pack(pkgbuf, rsp->nick_name, max_nick_size, idx);
		}
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	} else if (p->waitcmd == cli_proto_bind_magic_number) {
		uint32_t max_role_lv = 0;
		for (uint32_t i = 0; i < role_cnt; i++) {
			role_simple_info_rsp_t * rsp = reinterpret_cast<role_simple_info_rsp_t*>((char*)body + 4 + sizeof(role_simple_info_rsp_t) * i);
			if (max_role_lv < rsp->level) {
				max_role_lv = rsp->level;
			}
		}

		if (max_role_lv > 20) {
			send_header_to_player(p, p->waitcmd, cli_err_role_lv_more_then_20, 1);
		} else {
			idx = 0;	
			char magic_no[10] = {0};
			unpack(p->session, magic_no, 10, idx); 
			idx = 0;
			pack(dbpkgbuf, magic_no, 10, idx);
			send_request_to_cachesvr(p, p->id, p->role_tm, cache_bind_magic_number, dbpkgbuf, idx);
		}
	}

	return 0;
}
	

//------------------------------------------------------
//  每日运势 ()
//------------------------------------------------------
uint16_t get_box_id(uint32_t type)
{
    static uint32_t limit[3][4] = {{0,},{10, 20, 30, 40},{20, 30, 45, 5}};
    uint32_t odds = rand() % 100;
    uint32_t ret = 0;
    for (uint32_t i = 0; i < 4; i++) {
        ret += limit[type][i];
        if (odds < ret) {
            return (1422 + i);
        }
    }
    return 1425;
}

int rand_box_data(uint32_t type, void *buf)
{
    int idx = 0;
    pack(buf, static_cast<uint16_t>(1425), idx);
    for (uint32_t i = 1; i < 6; i++) {
        if (i == 3 && is_in_active_time_section(26) == 0) {
            pack(buf, static_cast<uint16_t>(1422), idx);
        } else {
            pack(buf, get_box_id(type), idx);
        }
    }
    pack(buf, static_cast<uint16_t>(1422), idx);
    return idx;
}

int player_zhanbo_fate_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t type = 0;
    unpack(body, type, idx);

    if (type == 1) {
        if (get_swap_action_times(p, 1397) > 2) {
            return send_header_to_player(p, p->waitcmd, cli_err_day_limit_time_act, 1);	
        }
        add_swap_action_times(p, 1397);

        do_stat_log_universal_interface_2(0x9020006, 0, p->id, 1);
    } else if (type == 2) {
        if (p->my_packs->get_item_cnt(gold_coin_item_id) < 200) {
            return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);	
        }
        db_del_item_elem_t item_elem = {gold_coin_item_id, 200};
        player_del_items(p, &item_elem, 1, false);
        do_stat_log_universal_interface_2(0x9020007, 0, p->id, 1);
    } else {
        return -1;
    }

    uint32_t odd_num_1 = rand() % 100 + 1;
    uint32_t odd_num_2 = rand() % 100 + 1;
    uint32_t odd_num_3 = rand() % 100 + 1;

    TRACE_LOG("fate ---> %u [%u |%u %u %u]", p->id, type, odd_num_1, odd_num_2, odd_num_3);
    //temp save ----------------
    idx = 0;
    pack(p->other_info_2[active_daily_zhanbo - 1], static_cast<uint32_t>(p->lv), idx);
    pack(p->other_info_2[active_daily_zhanbo - 1], type, idx);
    pack(p->other_info_2[active_daily_zhanbo - 1], odd_num_1, idx);
    pack(p->other_info_2[active_daily_zhanbo - 1], odd_num_2, idx);
    pack(p->other_info_2[active_daily_zhanbo - 1], odd_num_3, idx);

    idx += rand_box_data(type, p->other_info_2[active_daily_zhanbo - 1] + idx);

    save_player_active_info_2(p, active_daily_zhanbo, p->other_info_2[active_daily_zhanbo - 1]);

    idx = sizeof(cli_proto_t); 
    pack(pkgbuf, p->other_info_2[active_daily_zhanbo - 1], ACTIVE_BUF_LEN, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int player_select_fate_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    //uint8_t fate[ACTIVE_BUF_LEN + 1] = {0};
    //unpack(body, nick, ACTIVE_BUF_LEN, idx);
    if (get_swap_action_times(p, 1404)) {
        return send_header_to_player(p, p->waitcmd, cli_err_day_limit_time_act, 1);	
    }

    if (p->battle_grp) {
        return send_header_to_player(p, p->waitcmd, cli_err_unable_in_battle, 1);
    }

    idx = 0;
    uint32_t  lv = 0, type = 0, para_1 = 0, para_2 = 0, para_3 = 0;
    unpack(p->other_info_2[active_daily_zhanbo - 1], lv, idx);
    unpack(p->other_info_2[active_daily_zhanbo - 1], type, idx);
    unpack(p->other_info_2[active_daily_zhanbo - 1], para_1, idx);
    unpack(p->other_info_2[active_daily_zhanbo - 1], para_2, idx);
    unpack(p->other_info_2[active_daily_zhanbo - 1], para_3, idx);

    TRACE_LOG("fate ----> [%u][%u %u|%u %u %u]", p->id, lv, type, para_1, para_2, para_3);

    uint32_t money = lv * (500 + (2000 - 500) * para_1 * 0.01);
    uint32_t p_exp = lv * (100 + (500 - 100) * para_2 * 0.01);
    uint32_t m_exp = lv * (100 + (500 - 100) * para_3 * 0.01);
    if (type == 2) {
        money = lv * (2000 + (3000 - 2000) * para_1 * 0.01);
        p_exp = lv * (500 + (1000 - 500) * para_2 * 0.01);
        m_exp = lv * (500 + (1000 - 500) * para_3 * 0.01);
    }
	KDEBUG_LOG(p->id ,"FATE \t[ %u | %u %u %u]", type, money, p_exp, m_exp);

    if (p->lv >= player_max_level) {
        money += p_exp;
        p_exp = 0;
    }

    save_player_active_info_2(p, active_daily_zhanbo, p->other_info_2[active_daily_zhanbo - 1]);

    idx = 0;
    pack_h(dbpkgbuf, money, idx);
    pack_h(dbpkgbuf, p_exp, idx);
    pack_h(dbpkgbuf, m_exp, idx);
    return send_request_to_db(p, p->id, p->role_tm, dbproto_set_player_fate, dbpkgbuf, idx);
}

int db_set_player_fate_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
    db_set_player_fate_t *fate = reinterpret_cast<db_set_player_fate_t*>(body);

    p->coins            += fate->money;
    p->exp              += fate->p_exp;
    p->allocator_exp    += fate->m_exp;

    do_stat_log_universal_interface_1(0x9020008, 0, p->id);
    add_swap_action_times(p, 1404);

    calc_player_level(p);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, fate->money, idx);
	pack(pkgbuf, fate->p_exp, idx);
	pack(pkgbuf, fate->m_exp, idx);
	pack(pkgbuf, p->other_info_2[active_daily_zhanbo - 1], 40, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

bool check_login_box(player_t* p, uint32_t index, uint32_t lag)
{
    uint32_t daily_swap = get_swap_action_times(p, 1422) + get_swap_action_times(p, 1423)
        + get_swap_action_times(p, 1424) + get_swap_action_times(p, 1425);
    if (daily_swap >= 7) {
        return false;
    }

    int idx = 0;
    char *p_box = (char *)(p->other_info_2[active_daily_zhanbo - 1] + (sizeof(uint32_t) * 5));
    uint16_t box_id[7] = {0};
    for (uint32_t i = 0; i < 7; i++) {
        unpack(p_box, box_id[i], idx);
        TRACE_LOG("box ID----->[%u]", box_id[i]);
    }
    uint8_t flag_bit = 0;
    unpack(p_box, flag_bit, idx);
    if (test_bit_on(flag_bit, lag + 1)) {
        return false;
    }
    flag_bit = set_bit_on(flag_bit, lag + 1);

    int ifx = 0;
    pack(p_box + idx - sizeof(uint8_t), flag_bit, ifx);
    save_player_active_info_2(p, active_daily_zhanbo, p->other_info_2[active_daily_zhanbo - 1]);

    static uint32_t box_time[7] = {0, 30, 60, 120, 180, 240, 300};
    uint32_t ol_today = p->oltoday + ( get_now_tv()->tv_sec - p->login_tm);
    if (box_id[lag] == index && ol_today >= (box_time[lag] * 60)) {
        return true;
    }
    return false;
}

int notify_dirty_words(player_t* p, char* msg, uint32_t msg_len)
{
	
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, msg_len, idx);
	pack(pkgbuf, msg, msg_len, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_notify_dirty_words, idx);
	return send_to_player(p, pkgbuf, idx, 1);

}

int player_bet_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
    if (is_in_active_time_section(31)) {
        return send_header_to_player(p, p->waitcmd, cli_err_swap_action_not_in_time, 1);
    }
    
    if (get_swap_action_times(p, 1426)) {
        return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
    }
    static uint16_t odds[10] = {50, 4, 2, 40, 1, 2, 1, 0, 0, 0};
    static uint32_t rewards[10][3] = {{3, 11, 3}, 
                                    {3, 11, 7}, 
                                    {3, 11, 14}, 
                                    {3, 1, 100000}, 
                                    {2, 1500564, 1},
                                    {2, 1500567, 1},
                                    {2, 1500579, 1},
                                    {3, 11, 30},
                                    {3, 11, 90},
                                    {3, 11, 365}};
    
    uint32_t limit = rand() % 100;
    uint16_t tmp = 0;
    uint32_t i = 0;
    for (; i < 10; i++) {
        tmp += odds[i];
        if (tmp > limit) {
            break;
        }
    }

    TRACE_LOG("bet ---> %u [%u |%u %u %u]", p->id, i, rewards[i][0], rewards[i][1], rewards[i][2]);

    //temp save ----------------
    int idx = 0;
    pack(p->other_info_2[active_player_bet - 1], rewards[i][0], idx);
    pack(p->other_info_2[active_player_bet - 1], rewards[i][1], idx);
    pack(p->other_info_2[active_player_bet - 1], rewards[i][2], idx);

    idx = sizeof(cli_proto_t);
    pack(pkgbuf, rewards[i][0], idx);
    pack(pkgbuf, rewards[i][1], idx);
    pack(pkgbuf, rewards[i][2], idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int player_fatch_bet_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
    if (get_swap_action_times(p, 1426)) {
        return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
    }

    int idx = 0;
    uint32_t type = 0, itemid = 0, cnt = 0;
    unpack(p->other_info_2[active_player_bet - 1], type, idx);
    unpack(p->other_info_2[active_player_bet - 1], itemid, idx);
    unpack(p->other_info_2[active_player_bet - 1], cnt, idx);

    TRACE_LOG("bet ---> %u [%u %u %u]", p->id, type, itemid, cnt);

    memset(p->other_info_2[active_player_bet - 1], 0x00, ACTIVE_BUF_LEN);
    if (type == 3) {
        if (itemid > 11) {
            return send_header_to_player(p, p->waitcmd, cli_err_system_busy, 1);
        }
    } else if (type == 1 || type == 2) {
        const GfItem* itm = items->get_item(itemid);
        if ( !itm ) {
            return send_header_to_player(p, p->waitcmd, cli_err_system_busy, 1);
        }
    } else {
        return send_header_to_player(p, p->waitcmd, cli_err_system_busy, 1);
    }

    add_swap_action_times(p, 1426);

    add_item_to_player(p, type, itemid, cnt, 0, channel_string_active);

    idx = sizeof(cli_proto_t);;
    pack(pkgbuf, type, idx);
    pack(pkgbuf, itemid, idx);
    pack(pkgbuf, cnt, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);	
}

int generate_magic_number_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t generate_cnt = 0;
	unpack(body, generate_cnt, idx);
	if (p->coins < generate_cnt * 500000 || p->lv < 70) {
		ERROR_LOG("PLayer [%u | %u] try generate invite number not enough coins %u or level %u to low", p->id,
				p->role_tm, p->coins, p->lv);
		return -1;
	}

	if (generate_cnt == 1 || generate_cnt == 10) {
		idx = 0;
		pack_h(dbpkgbuf, generate_cnt, idx);
		return send_request_to_cachesvr(p, p->id, p->role_tm, cache_generate_magic_number, dbpkgbuf, idx);

	}
	ERROR_LOG("WRONG %u Generate_cnt for magic number!", p->id);
	return -1;
}

int bind_magic_number_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	if (get_swap_action_times(p, 1497)) {
		return send_header_to_player(p, p->waitcmd, cli_err_already_bind_magic_number, 1);
	}

	int idx = 0;
	char magic_no[10] = {0};
	unpack(body, magic_no, 10, idx);

	idx = 0;
	pack(p->session, magic_no, 10, idx);

	idx = 0;
	pack_h(dbpkgbuf, p->id, idx);
	return send_request_to_db(p, p->id, 0, dbproto_list_simple_role_info, dbpkgbuf, idx);
}

int list_magic_invite_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t invite_num = get_player_stat_info(p, 8);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, invite_num, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

struct magic_number_rsp_t {
	uint8_t magic_number[10];
}__attribute__((packed));


int cache_generate_magic_number_callback(player_t *p, cachesvr_proto_t * data)
{
	if (data->ret) {
		return send_header_to_player(p, p->waitcmd, data->ret, 1);
	}
	int idx = 0;
	uint32_t generate_cnt  = 0;
	unpack_h(data->body, generate_cnt, idx);
	CHECK_VAL_EQ(data->len - sizeof(cachesvr_proto_t), 4 + sizeof(magic_number_rsp_t) * generate_cnt);
	p->coins -= (500000 * generate_cnt); 	
	db_set_role_base_info(p);

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, generate_cnt, idx);
	for (uint32_t i = 0; i < generate_cnt; ++i) {
		magic_number_rsp_t * rsp = reinterpret_cast<magic_number_rsp_t*>(data->body + 4  + i * sizeof(magic_number_rsp_t));
		pack(pkgbuf, rsp->magic_number, 10, idx);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	do_stat_log_universal_interface_2(0x09060005, 0, p->id, generate_cnt);
	return 0;

}

int cache_bind_magic_number_callback(player_t * p, cachesvr_proto_t * data)
{
	if (data->ret) {
		return send_header_to_player(p, p->waitcmd, data->ret, 1);
	}

	//add a double exp buff in the future two weeks
	do_swap_vip_qualify(p, p->id, 7);
	db_add_buff(p, 1400, 24 * 3600 * 12, 1);
	add_swap_action_times(p, 1497);
	send_header_to_player(p, p->waitcmd, data->ret, 1);
	do_stat_log_universal_interface_2(0x09060006, 0, p->id, 1);
	return 0;
}

int get_invitee_player(player_t * p)
{

	if (get_swap_action_times(p, 1498)) {
		return 0;
	}
	return send_request_to_cachesvr(p, p->id, p->role_tm, cache_get_magic_number_creator, NULL, 0);
}

int cache_get_magic_number_creator_callback(player_t * p, cachesvr_proto_t * data)
{
	int idx  = 0;
	uint32_t creator_id = 0;
	uint32_t creator_tm = 0;
	unpack_h(data->body, creator_id, idx);
	unpack_h(data->body, creator_tm, idx);
	//check player has set invitee reward before
	if (p->lv >= 40) {
		add_swap_action_times(p, 1498);
		int idx = 0;
		uint32_t stat_invite_id = 8;
		pack_h(dbpkgbuf, stat_invite_id, idx);
		pack_h(dbpkgbuf, 1, idx);
		send_request_to_db(NULL, creator_id, creator_tm, dbproto_add_player_stat_info, dbpkgbuf, idx);

		int ifx = 0;	
		uint32_t max_bag_grid_count = 500;
		uint32_t item_type = 2;
		pack_h(dbpkgbuf, item_type, ifx);
		pack_h(dbpkgbuf, 1500607, ifx);
		pack_h(dbpkgbuf, 1, ifx);
		pack_h(dbpkgbuf, max_bag_grid_count, ifx);
		send_request_to_db(0, creator_id, creator_tm, dbproto_add_item, dbpkgbuf, ifx);
		KDEBUG_LOG(p->id, "PLayer Invitee Over, Reward! Invitee %u %u", creator_id, creator_tm);
	}
	return 0;
}


