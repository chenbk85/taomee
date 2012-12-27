#include <cstdio>

#include <libtaomee++/inet/pdumanip.hpp>
#include <kf/player_attr.hpp>
#include <libtaomee++/utils/md5.h>
 
using namespace taomee;

extern "C" {
#include <glib.h>

#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
#include <async_serv/mcast.h>
}
#include "common_op.hpp"
#include "cli_proto.hpp"
#include "item.hpp"
#include "switch.hpp"
#include "battle.hpp"
#include "cachesvr.hpp"
#include "task.hpp"
#include "message.hpp"
#include "player.hpp"
#include "login.hpp"
#include "warehouse.hpp"
#include "apothecary.hpp"
#include "restriction.hpp"
#include "battle_switch.hpp"
#include "mcast_proto.hpp"
#include "trade.hpp"
#include "skill.hpp"
#include "achievement.hpp"
#include "global_data.hpp"
//#include "achievement_logic.hpp"
#include "home.hpp"
#include "card.hpp"
#include "fight_team.hpp"
#include "stat_log.hpp"
#include "player_attribute.hpp"
#include "task_logic.hpp"
#include "chat.hpp"
#include "other_active.hpp"
#include "temporary_team.hpp"
#include "rank_top.hpp"
/*! hashes all players within this server by fd */
//static GHashTable*	all_players;

/*! hashes all players within this server by user id */
//static list_head_t	uid_buckets[c_bucket_size];
/*! number of players within this server */
//uint32_t			player_num;

/*! maximum battle time of one day for one week */
//static uint32_t time_limit_arr[7];
/*! maximum double experience time of on day */
//static uint32_t time_double_arr[7];

//-----------------------------------------------------------
// static function declarations
//-----------------------------------------------------------
static void unpack_basic_player_info(player_t* p, const void* info);
static void unpack_clothes_info(player_t* p, void* clothes_info, uint16_t clothes_cnt);
//static void unpack_skill_bind_info(player_t* p, void* skill_info, uint16_t skill_cnt);
static void unpack_skill_info(player_t* p, void* skill_info, uint16_t skill_cnt);
static int unpack_done_task_info(player_t* p, void* done_task_info, uint32_t cnt);
static int unpack_going_task_info(player_t* p, void* going_task_info, uint32_t cnt);
static int unpack_cancel_task_info(player_t* p, void* cancel_task_info, uint32_t cnt);
static int unpack_packs_clothes_info(player_t* p, void* pack_clothes_info, uint32_t cnt);
static int unpack_packs_item_info(player_t* p, void* pack_item_info, uint32_t cnt);
static int unpack_fumo_stage_info(player_t* p, void* fumo_stage_info, uint32_t cnt);
static int unpack_db_buff_info(player_t* p, void* db_buff_info, uint32_t cnt);


static int send_offline_msg(player_t* p, uint8_t* msg, uint32_t size);
static int report_user_onoff_to_db(const player_t* p, userid_t id, uint32_t loginflag);

//-----------------------------------------------------------
// inline function definations
//-----------------------------------------------------------
/**
  * @brief allocate memory for a player object and initialize it
  * @param uid user id
  * @param fdsess fd session
  * @return pointer to the player if found, 0 otherwise
  */
inline player_t* alloc_player(userid_t uid, fdsession_t* fdsess)
{
	player_t* p = reinterpret_cast<player_t*>(g_slice_alloc0(c_player_size));

	p->fd         = fdsess->fd;
	p->fdsess     = fdsess;
	p->id         = uid;
	p->login_tm   = get_now_tv()->tv_sec;
    uint32_t randnum = 1;//rand() % 100 + 1;//
	p->seqno      = randnum;
	p->real_seqno = randnum;
	p->pkg_queue  = g_queue_new();
	p->forbiden_add_friend_flag = 0;
	p->pay_passwd_check_flg = false;
	p->pay_passwd_err_cnt = 0;
	p->pay_passwd_err_tm = 0;
    p->safe_trade_room_id = 0;
	//player's tasks
	p->finished_tasks_set = new std::set<uint32_t>;
	p->canceled_tasks_set = new std::set<uint32_t>;
	p->going_tasks_map = new std::map<uint32_t, task_going_t>;




	p->fumo_stage_map = new std::map<uint32_t, fumo_stage_info_t>;
	p->buf_skill_map = new BufSkillMap;
	p->player_skill_map = new PlayerSkillMap;
	p->home_btl_pets    = new std::vector<uint32_t>;

    p->reward_role_vec = new std::vector<role_info_t>;

    p->p_master     = new mp_master_t;
    memset(p->p_master, 0x00, sizeof(p->p_master));
    p->p_prentice   = new std::vector<mp_prentice_t>;
	p->master_prentice_tm = 0;

//	p->cards_info = new std::map<uint32_t, player_card_t>;
	p->cardset_info = new std::map<uint32_t, player_card_set_t*>;

    p->group_datas = new group_t;
    p->add_quality = new quality_t;

	p->taotai_info = new pvp_game_data_t;
	p->advance_info = new pvp_game_data_t;
	p->m_swap_action_data_maps = new PlayerSwapActionDataMap;

	p->final_info = new pvp_game_data_t;
	p->p_banner_info = new red_blue_info_t;

	p->watch_info = 0;
    p->game_flower = 0;
    p->contest_win_stage = 0;
    p->contest_final_rank = 0;
    p->contest_final_segment = 0;
#ifdef DEV_SUMMON
	//player's summons
	p->summon_mon_num = 0;
	p->fight_summon = 0;
#endif
    p->m_numens = new std::vector<numen_obj_t>;

    p->friends_vec = new std::vector<uint32_t>;

	p->temporary_team_id = 0;
	p->m_temporary_team_map = new std::map<uint32_t, temporary_team_member_t>;
	p->m_temporary_team_map_bak = new std::map<uint32_t, temporary_team_member_t>;
//	p->player_task_steps = new TaskStepManager();
	//player's packs
	p->my_packs = new player_packs(p);
	p->tmp_session = new char[max_tmp_session_len];
	p->battle_grp = 0;
	p->trade_grp = 0;
	p->home_grp = 0;
	p->last_mcast_tm = 0;
	p->contest_session = 0;
	//p->tradehook_flag = false;
	
	p->team_id = 0;
	p->attire_gs = 0;

	p->offline_data = 0;

	if (!p->puzzle_data) {
		p->puzzle_data = new puzzle_info_t;
	}

	p->p_range_info = new player_range_t();

//	p->wuseng_reward_flag = 0;

	memset(&(p->team_info), 0, sizeof(p->team_info));
	p->team_top10_tv = 0;
	
	INIT_LIST_HEAD(&p->wait_cmd_player_hook);
	INIT_LIST_HEAD(&p->timer_list);

	list_add_tail(&p->playerhook, &uid_buckets[uid % c_bucket_size]);
	g_hash_table_insert(all_players, &(p->fd), p);
	++player_num;
	report_user_onoff_to_db(p, p->id, 60);
	init_player_warehouse(p);
	init_player_vip(p);
	init_player_secondary_profession(p);
	add_player_timer(p);
	init_player_mail(p);
	init_player_buy_item_limit_data(p);	
	init_player_kill_boss_data(p);
	init_player_achievement_data(p);
	init_player_use_item_data(p);
    init_player_ring_task(p);
	init_player_home_data(p);

    p->nick_change_tm = 0;
	p->pTitle = new CTitle();
	p->player_show_state = 0;
	p->open_box_times =0;
    p->using_achieve_title = 0;
	p->m_medal_count = 0;

    p->trigger_timer = 0;
    p->trigger_event = 0;
    p->trigger_times = 0;
    p->trigger_block = 0;
    p->trigger_victory = 0;

	p->seqno = 155; //155 = 0 - ( 0 / 7) + 50 % 21 + 1001 % 13  (0为初始seqno, 50为登陆包长度,
					//1001为登陆命令号
	memset(p->client_buf, 0, sizeof(p->client_buf));
	memset(p->other_info, 0, sizeof(p->other_info));
	TRACE_LOG("alloc player %u", p->id);

	return p;
}

/**
  * @brief allocates a player
  * @param fd socket fd
  * @return pointer to the player if found, 0 otherwise
  */
inline void dealloc_player(player_t* p)
{
	TRACE_LOG("dealloc player %u", p->id);
    if (p->safe_trade_room_id) {
		do_cancel_safe_trade(p);
	}

	cached_pkg_t* pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(p->pkg_queue));
	while (pkg) {
		g_slice_free1(pkg->len, pkg);
		pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(p->pkg_queue));
	}
	list_del_init(&(p->wait_cmd_player_hook));
	g_queue_free(p->pkg_queue);

	//player's task
	delete p->finished_tasks_set;
	delete p->canceled_tasks_set;
	delete p->going_tasks_map;
	delete p->fumo_stage_map;
	delete p->buf_skill_map;
	delete p->player_skill_map;
	delete p->m_swap_action_data_maps;
	delete p->home_btl_pets;
	//player's packs
	delete p->my_packs;
    //player title
    delete p->pTitle;
	if (p->tmp_session) {
		delete [] p->tmp_session;
		p->tmp_session = 0;
	}
	delete p->friends_vec;

    delete p->group_datas;
    delete p->add_quality;
	delete p->taotai_info;
	delete p->advance_info;
	delete p->final_info;
//	delete p->player_task_steps;
    delete p->p_master;
    delete p->p_prentice;
    delete p->m_numens;

    delete p->reward_role_vec;

	delete p->p_banner_info;

	delete p->p_range_info;


	report_user_onoff_to_chat_svr(p, 0);
	//delete p->cards_info;
	if (!p->cardset_info->empty()) {
		for (PCardSetMapIterator it = p->cardset_info->begin();
				it != p->cardset_info->end(); ++it) {
			player_card_set_t * set = (it->second);
			delete set;
		}

		p->cardset_info->clear();
	}
	delete p->cardset_info;

	for (uint32_t i = 0; i < p->summon_mon_num; i++) {
		summon_mon_t * p_mon = &(p->summons[i]);
		if (p_mon && p_mon->pet_attr) {
			delete p_mon->pet_attr;
		}
	}

	if (p->offline_data) {
		delete p->offline_data;
		p->offline_data = NULL;
	}

	if (p->puzzle_data) {
		delete p->puzzle_data;
		p->puzzle_data = NULL;
	}

	if (p->temporary_team_id) {
		player_leave_temporary_team(p);
	}
	delete p->m_temporary_team_map;
	delete p->m_temporary_team_map_bak;

	//player's warehouse
	final_player_warehouse(p);
	final_player_vip(p);
	final_player_secondary_profession(p);
	final_player_mail(p);
	final_player_buy_item_limit_data(p);
	final_player_kill_boss_data(p);
	final_player_achievement_data(p);
	final_player_use_item_data(p);
	final_player_ring_task(p);
	final_player_home_data(p);
	p->player_show_state = 0;
	p->open_box_times = 0;
    p->using_achieve_title = 0;
	p->forbiden_add_friend_flag = 0;
//	p->contest_exploit = 0;
	REMOVE_TIMERS(p);
	list_del(&p->playerhook);
	report_user_onoff_to_db(p, p->id, 61);
	p->m_medal_count = 0;
	--player_num;

	g_hash_table_remove(all_players, &(p->fd));

}

/**
  * @brief free memory allocated for a player_t object, invoked automatically by g_hash_table_remove
  * @param p pointer to the player_t object
  */
inline void free_player(void* p)
{
	//TRACE_LOG("free player %u", (reinterpret_cast<player_t*>(p))->id);
	g_slice_free1(c_player_size, p);
}

/**
 * @brief as universal interface for platform of statstics
 */
void do_stat_log_universal_interface_1(uint32_t cmd, uint32_t id, uint32_t cnt)
{
    uint32_t buf[1] = {0};
    buf[0] = cnt;
    msglog(statistic_logfile, cmd + id, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [id=%d] [cnt=%d]", cmd, id, cnt);
}

/**
 * @brief as universal interface for platform of statstics
 */
void do_stat_log_universal_interface_2(uint32_t cmd, uint32_t id, uint32_t cnt1, uint32_t cnt2)
{
    uint32_t buf[2] = {0};
    buf[0] = cnt1;
	buf[1] = cnt2;
    msglog(statistic_logfile, cmd + id, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [id=%d] [cnt1=%d] [cnt2=%d]", cmd, id, cnt1, cnt2);
}


/**
 * @brief as universal interface for platform of statstics
 */
void do_stat_log_universal_interface(uint32_t cmd, uint32_t id)
{
    msglog(statistic_logfile, cmd + id, get_now_tv()->tv_sec, 0, 0);
    TRACE_LOG("stat log: [cmd=%x] [id=%d]", cmd, id);
}

/**
 * @brief as universal interface for platform of statstics
 */
void do_stat_log_item_universal_interface(uint32_t id, int channel_id, uint32_t cnt)
{
    uint32_t buf[item_cost_enum] = {0};
    int index = channel_id ? (channel_id - 1) : 0;
    buf[index] = cnt;
    uint32_t stat_cmd = 0x09700000 + ((id / 100000 - 13) * 2 << 16) + id - (id / 100000 * 100000);
    msglog(statistic_logfile, stat_cmd, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [channel_id=%d] [cnt=%u]", stat_cmd, channel_id, cnt);
}

/**
 * @brief add here for platform of statstics
 */
void do_stat_log_achieve_child_num(player_t* p)
{
    uint32_t buf[1];
	buf[0] = p->id;
	msglog(statistic_logfile, stat_log_achieve_child_num, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [buf=%u]",
        stat_log_get_amb_reward, buf[0]);
}

/*
void send_keepalive_pkg(void* key, void* player, void* userdata)
{
	player_t* p = reinterpret_cast<player_t*>(player);
	if ( (get_now_tv()->tv_sec - p->last_act_tm) > 29 ) {	
		init_cli_proto_head_full(pkgbuf, 0, 0, cli_proto_keepalive_noti, sizeof(cli_proto_t), 0);
		send_to_player(p, pkgbuf, sizeof(cli_proto_t), 0);

		//char msg[] = "Master wugui! Tailang will be back";
		//sys_notify_to_player(p, 1, msg, strlen(msg));
	}
}
*/
void send_keepalive_pkg(player_t* p)
{
	uint32_t tmnow = get_now_tv()->tv_sec;

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, tmnow, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_keepalive_noti, idx);
	send_to_player(p, pkgbuf, idx, 0);
}


bool is_achieve_amb_lv(player_t* p)
{
	return test_bit_on(p->once_bit, flag_pos_achieve_lv);
}

void set_once_bit_on(player_t* p, uint32_t pos)
{
	TRACE_LOG("set bit:%u %u %x %x %x", p->id, pos, p->once_bit[0], p->once_bit[1], p->once_bit[2]);

	set_bit_on(p->once_bit, pos);

	TRACE_LOG("set bit:%u %u %x %x %x", p->id, pos, p->once_bit[0], p->once_bit[1], p->once_bit[2]);
}

void set_player_event_trigger(player_t* p, uint32_t times, uint32_t block, uint32_t victory)
{
    p->trigger_times = times;
    p->trigger_block = block;
    p->trigger_victory = victory;
}


void reduce_exploit(player_t* p, uint32_t reduce_exploit)
{
	if (p->exploit >= reduce_exploit) {
		p->exploit -= reduce_exploit;
	} else {
		p->exploit = 0;
	}

	db_set_role_info(p, "pvp_fight", p->exploit);
}


//--------------------------------------------------------------------

/**
  * @brief allocate and add a player to this server
  * @param player
  * @return pointer to the newly added player
  */
player_t* add_player(player_t* player)
{
	player_t* p = alloc_player(player->id, player->fdsess);
	p->seqno = player->seqno;
	return p;
}

/**
  * @brief clear players' info whose online server is down
  * @param fd online fd
  */
void clear_players_battle(void* key, void* player, void* userdata)
{
	struct clear_players_battle_in_t {
		int btl_fd;
	};
	clear_players_battle_in_t* p_btlinfo = reinterpret_cast<clear_players_battle_in_t*>(userdata);
	player_t* p = reinterpret_cast<player_t*>(player);
	if (p->battle_grp && battle_fds[p->battle_grp->fd_idx] == p_btlinfo->btl_fd) {
		KDEBUG_LOG(p->id, "BTL FD CLOSED\t[%u free battle grp %u %u]", p->id, p->battle_grp->btl_type, p->battle_grp->fd_idx);
		p->waitcmd = 0;
		if (p->watch_info) {
			DEBUG_LOG("CLEAR WATCH INFO %u", p->id);
			destory_watch_info(p, p->watch_info);
			p->watch_info = 0;
		}
	}


}

void clear_players_home(void* key, void* player, void* userdata)
{
	struct clear_players_home_in_t{
		int home_fd;
	};
	clear_players_home_in_t* p_homeinfo = reinterpret_cast<clear_players_home_in_t*>(userdata);
	player_t* p = reinterpret_cast<player_t*>(player);
	if(p->home_grp && home_fds[p->home_grp->fd_idx] == p_homeinfo->home_fd){
		p->waitcmd = 0;
	}
}

void do_while_home_svr_crashed(int home_fd)
{
	g_hash_table_foreach(all_players, clear_players_home, &home_fd);
}
/**
  * @brief do when battle server crashed
  * @param fd online fd
  */
void do_while_battle_svr_crashed(int btl_fd)
{
	g_hash_table_foreach(all_players, clear_players_battle, &btl_fd);
}

int pack_login_report_info(player_t* p, uint32_t login_tm, uint32_t logout_tm, uint8_t* buf)
{
	int idx = 0;
	uint32_t gf_flag = 6;
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
	pack_h(verify_buf + s_len, login_tm, idx);
	pack_h(verify_buf + s_len, logout_tm, idx);
	md5.reset();
	md5.update(verify_buf, s_len + idx);
	memcpy(verify_code, md5.toString().c_str(), verify_len);

	KDEBUG_LOG(p->id, "GET SESS\t[gameflg:%u ip:%04x]", gf_flag, p->fdsess->remote_ip);
	//send pack
	idx = 0;
	pack_h(buf, m_channel_id, idx);
	pack(buf, verify_code, verify_len, idx);
	pack_h(buf, gf_flag, idx);
	pack_h(buf, login_tm, idx);
	pack_h(buf, logout_tm, idx);
	
	return idx;
}


/**
  * @brief delete player
  * @param p player to be deleted
  */
void del_player(player_t* p)
{
	KDEBUG_LOG(p->id, "DEL USER\t[uid=%u flg=0x%x fd=%d]", p->id, p->flag, p->fd);
	if (test_bit_on(p->user_progress, user_progress_login)) {
		notify_user_login(p, 0);
		notify_user_on_off(p, 0);
		send_team_member_onoff(p, 0, p->team_info.team_id, p->id, p->role_tm);
	}


	if (p->watch_info && p->battle_grp) {
		btlsvr_contest_cancel_yazhu(p, 1);
		if (p->battle_grp) {
			free_battle_grp(p->battle_grp, p);
			p->battle_grp = 0;
		}
		pvp_watch_data_t * watch = p->watch_info;
		if (watch) {
			destory_watch_info(p, watch);
			p->watch_info = 0;
		}
	}

	home_grp_t* hrp = p->home_grp;
	if(hrp){
		if( hrp->home_owner_id == p->id)
		{
			homesvr_home_owner_off_line(p);
		}
		else
		{
			homesvr_leave_home(p, hrp->home_owner_id, hrp->role_regtime);
		}
		free_home_grp(p->home_grp, p);
	}	

	battle_grp_t* grp = p->battle_grp;
	if (grp) { 
		btlsvr_cancel_battle(p);
		free_battle_grp(p->battle_grp, p);
	}

	trade_grp_t * trp = p->trade_grp;
	if (trp) {
		trsvr_player_leave_market(p);
		//remove player from the trade hook;
		list_del(&p->tradehook);
		KDEBUG_LOG(p->id, "ROMVE PLAYER [%u] FROM TRADE PLAYER LIST", p->id);
		free_trade_grp(p->trade_grp);
		p->trade_grp = 0;
	}

	//leave_map(p);

	if (!is_guest_id(p->id)) {
		int32_t now = get_now_tv()->tv_sec;
		int32_t tmdiff = now - p->login_tm;
		if (tmdiff > 0) {
            db_logout_info_t logout_info;
            if (p->cur_map ) {
				//40号是小屋地图不存数据库
                logout_info.map_id = get_mapid_low32(p->cur_map->id) == 40 ? 1 : get_mapid_low32(p->cur_map->id);	
            } else {
                logout_info.map_id = 1;
            }
            logout_info.xpos = p->xpos;
            logout_info.ypos = p->ypos;
            logout_info.oltm = tmdiff;
            TRACE_LOG("DEL USER\t[uid=%u mapid=%u xpos=%u ypos=%u]", 
                p->id, logout_info.map_id, logout_info.xpos,logout_info.ypos);
			send_request_to_db(0, p->id, p->role_tm, dbproto_logout, &logout_info, sizeof(logout_info));

			/* 登陆时长报告 */
			//static db_update_login_info_t db_update_login_info;
			//db_update_login_info.type = 6; // 1:mole,2:seer......6:gongfupai
			//db_update_login_info.login_time = p->login_tm;
			//db_update_login_info.logout_time = now;
			
			int idx = 0;
			idx += pack_login_report_info(p, p->login_tm, static_cast<uint32_t>(now), pkgbuf + idx);
			send_request_to_db_no_roletm(0, p->id, dbproto_login_report_info, pkgbuf, idx);


			
			//send_request_to_db(0, p->id , 6, dbproto_login_report_info, &db_update_login_info, sizeof(db_update_login_info));
		} else if (tmdiff < 0) {
			ERROR_LOG("Server stamp error, now=%ld, login time=%u, id=%u",
						get_now_tv()->tv_sec, p->login_tm, p->id);
		}
	}
	
    leave_map(p);

	dealloc_player(p);
}

/**
  * @brief get player by user id
  * @param uid user id
  * @return pointer to the player if found, 0 otherwise
  */
player_t* get_player(userid_t uid)
{
	player_t* p;

	list_for_each_entry (p, &uid_buckets[uid % c_bucket_size], playerhook) {
		if (p->id == uid) {
			return p;
		}
	}

	return 0;
}

/**
  * @brief get player by socket fd
  * @param fd socket fd
  * @return pointer to the player if found, 0 otherwise
  */
player_t* get_player_by_fd(int fd)
{
	return reinterpret_cast<player_t*>(g_hash_table_lookup(all_players, &fd));
}

/**
  * @brief a timer function to keep all players alive
  * @return 0
  */

int keep_players_alive(void* owner, void* data)
{
//	traverse_players(send_keepalive_pkg, 0);
	player_t* player = reinterpret_cast<player_t*>(owner);
	send_keepalive_pkg(player);
	ADD_TIMER_EVENT_EX(player, keep_players_alive_idx, 0, get_now_tv()->tv_sec + 30);
	return 0;
}

/**
  * @brief init the no time limit day
  * @param day_limit string config of no time limit day
  * @return 0
  */
int init_time_limit_day(char* day_limit, int type)
{
	const char* delim = ",";
	char* day = strtok(day_limit, delim);
    uint32_t *arr;
    if (type == 0) {
        arr = time_limit_arr;
    } else {
        arr = time_double_arr;
    }
	int cnt = 0;
	while (day) {
		if (cnt > 7) {
			return -1;
		}
		arr[cnt++] = atoi(day);
		day = strtok(0, delim);
	}
	KDEBUG_LOG(0, "time limit %u %u %u %u %u %u %u", time_limit_arr[0], time_limit_arr[1], time_limit_arr[2],	time_limit_arr[3], time_limit_arr[4], time_limit_arr[5],time_limit_arr[6]);
	KDEBUG_LOG(0, "time double %u %u %u %u %u %u %u", time_double_arr[0], time_double_arr[1], time_double_arr[2],	time_double_arr[3], time_double_arr[4], time_double_arr[5],time_double_arr[6]);

	if (cnt != 7) {
		return -1;
	}

	return 0;
}

/**
  * @brief init the no time double day
  * @param day_double string config of no time double day
  * @return 0
  */
int init_time_double_day(char* day_double)
{
	const char* delim = ",";
	char* day = strtok(day_double, delim);

	int cnt = 0;
	while (day) {
		if (cnt > 7) {
			return -1;
		}
		time_double_arr[cnt++] = atoi(day);
		day = strtok(0, delim);
	}
	KDEBUG_LOG(0, "time double %u %u %u %u %u %u %u",time_double_arr[0], time_double_arr[1], 
        time_double_arr[2],time_double_arr[3], time_double_arr[4], time_double_arr[5], time_double_arr[6]);

	if (cnt != 7) {
		return -1;
	}

	return 0;
}

/**
  * @brief set day time limit for one day
  * @param flag 0 for initilization, 1 for pre-set for the next day in advance
  */
void set_battle_time_limit(uint32_t flag)
{
	if (!flag) {
		battle_time_limit = time_limit_arr[get_now_tm()->tm_wday];
	} else {
		battle_time_limit = time_limit_arr[(get_now_tm()->tm_wday + 1) % 7];
	}
}

int init_income_per_arr()
{
	char* income_config_arr[7];
	income_config_arr[0] = config_get_strval("sun_income_per");
	income_config_arr[1] = config_get_strval("mon_income_per");
	income_config_arr[2] = config_get_strval("tues_income_per");
	income_config_arr[3] = config_get_strval("wed_income_per");
	income_config_arr[4] = config_get_strval("thurs_income_per");
	income_config_arr[5] = config_get_strval("fri_income_per");
	income_config_arr[6] = config_get_strval("sat_income_per");

	for (uint32_t week = 0; week != 7; week++) {
		const char* delim = ",";
		char* day = strtok(income_config_arr[week], delim);

		int cnt = 0;
		memset (income_per_arr[week], 100, sizeof(income_per_arr[week]));
		while (day) {
			if (cnt > 23) {
				return -1;
			}
			income_per_arr[week][cnt++] = atoi(day);
			day = strtok(0, delim);
		}
		KDEBUG_LOG(0, "income per arr %u %u %u %u %u %u %u",income_per_arr[week][0], income_per_arr[week][1], income_per_arr[week][2], income_per_arr[week][3], income_per_arr[week][4], income_per_arr[week][5], income_per_arr[week][6]);
	}
	return 0;

}


/**
  * @brief tmp , just for operating activity
  */
bool is_market_activity(player_t* p) 
{
	uint32_t limit_tm = time_limit_arr[get_now_tm()->tm_wday];
	if (limit_tm == 18000) {
		return true;
	}
	return false;
}

/**
  * @brief send online time notification to timer owner
  * @return 0
  */
int send_online_tm_notification(void* owner, void* data)
{
	player_t* p = reinterpret_cast<player_t*>(owner);

	if (p->adult_flg) {
		return 0;
	}
	
	int32_t now_sec = get_now_tv()->tv_sec;

	/* send to client */
	int idx = sizeof(cli_proto_t);
	if (p->oltoday < battle_time_limit) {
		ADD_TIMER_EVENT_EX(p, send_online_tm_notification_idx, 0, now_sec + report_oltoday_int);
		pack(pkgbuf, p->oltoday + (now_sec - p->login_tm), idx);
		TRACE_LOG("send to online1 [%u][%u] Oltoday:%u", p->oltoday, now_sec - p->login_tm, p->oltoday + (now_sec - p->login_tm));
	} else {
		pack(pkgbuf, battle_time_limit, idx);
		TRACE_LOG("send to online2 [%u][%u] Oltoday:%u", p->oltoday, now_sec - p->login_tm, battle_time_limit);
	}

	init_cli_proto_head(pkgbuf, p, cli_proto_noti_online_tm, idx);
	send_to_player(p, pkgbuf, idx, 0);
	
	return 0;
}

/**
  * @brief a timer function to kick user offline if max online time expires
  * @return 0
  */
int limit_players_online_time(void* owner, void* data)
{
	player_t* player = reinterpret_cast<player_t*>(owner);

	TRACE_LOG("uid=%u, oltoday=%u", player->id, player->oltoday);

	if (data == (void*)1) {
		//error:player->oltoday = exp_half_time_limit;
		//error:player->half_exp = 1;
	} else {
		player->oltoday = battle_time_limit;
		send_online_tm_notification(owner, 0);
	}
	return 0;
}
/**
  * @brief send level up to player
  * @return 0
  */
void send_lv_up_mail(player_t* p, uint32_t mail_templet_id)
{
	char title[MAX_MAIL_TITLE_LEN + 1] = {0};
	char content[MAX_MAIL_CONTENT_LEN + 1] = {0 };
	
	db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_id);
}

/**
  * @brief send level top limit to player
  * @return 0
  */
void send_lv_limit_mail(player_t* p)
{
	char title[MAX_MAIL_TITLE_LEN + 1] = {0};
	char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
	db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_level_up_limit);
}
//--------------------------------------------------------------------

//-----------------------------------------------------------
// static function declarations
//-----------------------------------------------------------

/**
  * @brief initialize players
  */
void init_players()
{
	all_players = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_player);
	
	for (int i = 0; i != c_bucket_size; ++i) {
		INIT_LIST_HEAD(&uid_buckets[i]);
	}
}

/**
  * @brief finalize players
  */
void fini_players()
{
	g_hash_table_destroy(all_players);
}

/**
  * @brief traverse all the players and call 'action' on each player
  * @note you should not remove any player in 'action'
  */
void traverse_players(void (*action)(void* key, void* player, void* userdata), void* data)
{
	g_hash_table_foreach(all_players, action, data);
}

/**
  * @brief traverse all the players and call 'action' on each player
  * @note you can remove any player safely in 'action'
  */
void traverse_players_remove(void (*action)(player_t* player))
{
	//g_hash_table_foreach_remove(all_players, action, data);
	list_head_t* cur;
	list_head_t* next;
	for (int i = 0 ; i != c_bucket_size; ++i) {
		list_for_each_safe (cur, next, &(uid_buckets[i])) {
			player_t* player = list_entry(cur, player_t, playerhook);
			action(player);												
		}					
	}
}

uint32_t get_random_player_list_by_lv(player_t* p, player_t** p_list)
{
	list_head_t* cur;
	list_head_t* next;
	uint32_t ret_cnt = 0;
	for (int i = 0 ; i != c_bucket_size; ++i) {
		list_for_each_safe (cur, next, &(uid_buckets[i])) {
			player_t* player = list_entry(cur, player_t, playerhook);
			if ((p->lv < 5 && player->lv < p->lv + 5) || 
				(p->lv >= 5 && player->lv > (p->lv - 5) && player->lv < (p->lv + 5))) {
				if (p->id != player->id) {
					p_list[ret_cnt] = player;
					ret_cnt++;
				}
			}
			if (ret_cnt > 4) {
				break;
			}
		}
		if (ret_cnt > 4) {
			break;
		}
	}
	return ret_cnt;
}

/**
 * @brief pack p's clothes into buf
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_player_clothes(const player_t* p, uint32_t waitcmd, void* buf)
{
	int i = 0;
	pack(buf, p->clothes_num, i);
	TRACE_LOG("clothes_num:[%u] cmd:[%u]",p->clothes_num, waitcmd);
	for ( uint32_t cnt = 0; cnt < p->clothes_num; cnt++ ) {
		pack(buf, p->clothes[cnt].clothes_id, i);
        //if (waitcmd == cli_proto_get_usr_simple_info) {
            TRACE_LOG("clothes_time: [%u %u]",p->clothes[cnt].gettime, p->clothes[cnt].timelag);
            pack(buf, p->clothes[cnt].gettime, i);
            pack(buf, p->clothes[cnt].timelag, i);
            pack(buf, static_cast<int32_t>(p->clothes[cnt].lv), i);
        //}
	}
	return i;
}



/**
 * @brief pack p's clothes into buf
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_player_clothes_full(const player_t* p, void* buf)
{
	int i = 0;
	pack(buf, p->clothes_num, i);
	for ( uint32_t cnt = 0; cnt < p->clothes_num; cnt++ ) {
		pack(buf, p->clothes[cnt].clothes_id, i);
		pack(buf, p->clothes[cnt].unique_id, i);
		//pack(buf, p->clothes[cnt].get_time, i);
	}
	return i;
}

/**
 * @brief pack p's battle attr
 * @param p
 * @param buf
 * @return number of bytes packed into buf
 */
int pack_player_attr(const player_t* p, void* buf)
{
	int idx = 0;

	/*battle info*/
	uint32_t lv = p->lv;
	pack(buf, lv, idx);
	pack(buf, p->exp, idx);
	pack(buf, p->addhp, idx);
	pack(buf, p->addmp, idx);

	
	pack(buf, p->honor, idx);
	pack(buf, p->exploit, idx);
	pack(buf, p->win_times, idx);
	pack(buf, p->lose_times, idx);
	pack(buf, p->max_conti_win_times, idx);
	pack(buf, p->using_achieve_title, idx);
    pack(buf, p->other_info_2[active_player_attribute - 1], ACTIVE_BUF_LEN, idx);

	TRACE_LOG("[%u %u %u %u %u %u %u %u]", p->id, p->hp, p->mp, p->honor, p->exploit, p->win_times, p->lose_times, p->max_conti_win_times);
	return idx;
}

struct attire_gs_t {
	int agility;
	int strength;
	int body_quality;
	int stamina;

	int atk;
	int hit;
	int crit;
	int def;
	int dodge;

	int add_hp;
	int add_mp;

	void init()
	{
		agility = 0;
		strength = 0;
		body_quality = 0;
		stamina = 0;
		atk = 0;
		hit = 0;
		crit = 0;
		def = 0;
		dodge = 0;
		add_hp = 0;
		add_mp = 0;
	}

};

/**
 * @brief calc player's Attribute by attire strengthen level
 */
void calc_player_strengthen_attire(player_t* p, attire_gs_t * info)
{
	static float hit_para = (25)/ pow(p->lv, 1.32) / 100;

    int atk = 0, hp = 0,  agility = 0, strength = 0, hit = 0, def = 0, body = 0, dodge = 0;
    for (uint32_t i = 0; i < p->clothes_num; i++) {
        const player_clothes_info_t* clothes = &(p->clothes[i]);
        const GfItem* itm = items->get_item(clothes->clothes_id);
        if ( items->is_atk_clothes(itm->equip_part()) && clothes->lv >= 4 ) {
            atk      += calc_weapon_add_atk(itm->use_lv(), clothes->lv, itm->quality_lv);
            agility  += calc_weapon_add_agility(itm->use_lv(), clothes->lv, itm->quality_lv);
            strength += calc_weapon_add_strength(itm->use_lv(), clothes->lv, itm->quality_lv);
            hit      += calc_weapon_add_hit(itm->use_lv(), clothes->lv, itm->quality_lv);

            TRACE_LOG("strengthen:[%u %u %u]", agility, strength, hit);
        }
        if ( items->is_def_clothes(itm->equip_part()) && clothes->lv >= 1 ) {
            def   += calc_clothes_add_defense(clothes->lv);
            body  += calc_clothes_add_body(itm->use_lv(), clothes->lv, itm->quality_lv);
        	hp    += calc_clothes_add_hp(itm->use_lv(), clothes->lv, itm->quality_lv);
            dodge += calc_clothes_add_dodge(itm->use_lv(), clothes->lv, itm->quality_lv);
            TRACE_LOG("strengthen:[%u %u %u]", def, body, dodge);
        }
    }

	info->atk += atk;
	info->hit += hit;
	info->strength += strength;
	info->agility += agility;
	info->def += def;
	info->body_quality += body;
	info->dodge += dodge;
	info->add_hp += hp;


    p->agility      += agility;
    p->strength     += strength;
    p->hit_rate     += hit * hit_para;
    p->def_rate     += def;
    p->body_quality += body;
}

/**
 * @brief calc title attribute
 */
void calc_player_title_attribute(player_t* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->using_achieve_title);
    if (pattr == NULL) {
        return ;
    }

    p->agility      += pattr->agility;
    p->strength     += pattr->strength;
    p->body_quality += pattr->body;
    p->stamina      += pattr->stamina;
    //p->def_value    += pattr->def;

    p->atk          += pattr->atk;
    p->maxhp        += pattr->hp;
    p->maxmp        += pattr->mp;
}

void calc_attire_gs(player_t *p, attire_gs_t * gs)
{
	double c = 1.7095;
	double c_1 = 0.5850;
	double x = pow(gs->add_hp, c);
	x += pow(gs->add_mp, c);
	x += pow(gs->agility, c);
	x += pow(gs->atk, c);
	x += pow(gs->def * 5, c);
	x += pow(gs->crit, c);
	x += pow(gs->dodge, c);
	x += pow(gs->hit, c);
	x += pow(gs->stamina, c);
	x += pow(gs->strength, c);
	x += pow(gs->body_quality, c);
	x += pow(double(p->maxhp) / 20, c);
	x += pow(double(p->maxmp) / 30, c);

	p->attire_gs = (int)pow(x, c_1);
}


/**
 * @brief calc and init a player's battle attr
 * @param p
 */
void calc_player_attr(player_t* p)
{
	p->strength     = calc_strength[p->role_type](p->lv);
	p->agility      = calc_agility[p->role_type](p->lv);
	p->body_quality = calc_body_quality[p->role_type](p->lv);
	p->stamina      = calc_stamina[p->role_type](p->lv);
	p->maxhp        = 0;
	p->maxmp      	= 0;
	p->atk		  	= 0;
	p->crit_rate  	= 0;
	p->def_rate   	= 0;
	p->hit_rate  	= 0;
	p->dodge_rate 	= 0;

	struct attire_gs_t  info;
	info.init();


	struct weared_suit_t {
		uint32_t suit_id;
		uint32_t suit_cnt;
	};
	static weared_suit_t weared_suit_arr[max_suit_step]; 
	memset(weared_suit_arr, 0x0, sizeof(weared_suit_arr));
	uint32_t suit_id = 0;


	for (uint32_t i = 0; i != p->clothes_num; ++i) {
		const player_clothes_info_t* clothes = &(p->clothes[i]);
		const GfItem* itm = items->get_item(clothes->clothes_id);
		if( !itm ) {
			WARN_LOG("clothes id error[uid=%u clothesid=%u]",p->id,clothes->clothes_id);
			continue;
		}
		p->strength     += itm->strength;
		p->agility      += itm->agility;
		p->body_quality += itm->body_quality;
		p->stamina      += itm->stamina;

		//suit
		suit_id = itm->suit_id();
		if (suit_id) {
			for (uint32_t j = 0; j < max_suit_step; ++j) {
				if (!(weared_suit_arr[j].suit_id)) {
					weared_suit_arr[j].suit_id = suit_id;
					weared_suit_arr[j].suit_cnt = 1;
					break;
				}
				if (weared_suit_arr[j].suit_id == suit_id) {
					weared_suit_arr[j].suit_cnt ++;
					break;
				}
			}
		}
	}

	// suit attr
	suit_step_t suit_add_attr = {0};
	for (uint32_t j = 0; j < max_suit_step; ++j) {
		uint32_t suit_id = weared_suit_arr[j].suit_id;
		uint32_t suit_cnt = weared_suit_arr[j].suit_cnt;
		
		if (!suit_id) {
			break;
		}
		for (uint32_t n = suit_cnt; n > 0; --n) {
			if (suit_arr[suit_id].suit_step[n].attire_num) {
				suit_step_t* p_step = &(suit_arr[suit_id].suit_step[n]);
				suit_add_attr.hp 	  	+= p_step->hp;
				suit_add_attr.mp	  	+= p_step->mp;
				suit_add_attr.atk		+= p_step->atk;
				suit_add_attr.crit  	+= p_step->crit / 10;
				suit_add_attr.hit		+= p_step->hit / 2000.0;
				suit_add_attr.dodge 	+= p_step->dodge / 1000.0;	
				suit_add_attr.def		+= p_step->def;

				suit_add_attr.strength 		+= p_step->strength;
				suit_add_attr.agility		+= p_step->agility;
				suit_add_attr.body_quality 	+= p_step->body_quality;
				suit_add_attr.stamina		+= p_step->stamina;
				suit_add_attr.skill_atk		+= p_step->skill_atk;

				suit_add_attr.add_hp   += p_step->add_hp;
				suit_add_attr.add_mp += p_step->add_mp;


				info.strength += p_step->strength;
				info.agility += p_step->agility;
				info.body_quality += p_step->body_quality;
				info.stamina += p_step->stamina;

				info.hit += p_step->hit;
				info.crit += p_step->crit;
				info.atk += p_step->atk;
				info.def += p_step->def;
				info.dodge += p_step->dodge;

				info.add_hp += p_step->add_hp;
				info.add_mp += p_step->add_mp;

				TRACE_LOG("%u %u %u %u %u %u %u %u %u %u", p_step->attire_num, p_step->add_hp, p_step->add_mp, p_step->atk, p_step->crit, 
					p_step->hit, p_step->dodge, p_step->def, suit_add_attr.hp, suit_add_attr.mp);
			}
		}
	}	

    calc_player_strengthen_attire(p, &info);
    calc_player_title_attribute(p);

	p->maxhp	  += suit_add_attr.hp;
	p->maxmp	  += suit_add_attr.mp;
	p->atk		  += suit_add_attr.atk;
	p->crit_rate  += suit_add_attr.crit / 10;
	p->hit_rate   += suit_add_attr.hit / 2000.0;
	p->dodge_rate += suit_add_attr.dodge / 1000.0;	
	
	p->strength 	+= suit_add_attr.strength;
	p->agility		+= suit_add_attr.agility;
	p->body_quality += suit_add_attr.body_quality;
	p->stamina		+= suit_add_attr.stamina;


	p->maxhp      += calc_hp[p->role_type](p->body_quality);
	p->maxmp      += calc_mp[p->role_type](p->stamina);
	p->atk		  += calc_atk(p->strength);
	p->def_rate   += calc_def_rate(p->body_quality);
	p->hit_rate   += hit_rates[p->role_type];
	p->dodge_rate += calc_dodge_rate(p->agility);
	
	uint32_t itms_def = 0;
	for (uint32_t i = 0; i != p->clothes_num; ++i) {
		const player_clothes_info_t* clothes = &(p->clothes[i]);
		const GfItem* itm = items->get_item(clothes->clothes_id);
		if( !itm ) {
			WARN_LOG("clothes id error[uid=%u clothesid=%u]",p->id,clothes->clothes_id);
			continue;
		}
		p->maxhp 	  += itm->hp();
		p->maxmp	  += itm->mp();
		p->atk		  += itm->atk[0];
		p->crit_rate  += itm->crit() / 10;
		//p->def_rate   += itm->def / static_cast<double>(p->lv + 50) * 0.09;
		p->hit_rate   += itm->hit() / 2000.0;
		p->dodge_rate += itm->dodge() / 1000.0;

		info.atk += itm->atk[0];
		info.crit += itm->crit();
		info.hit += itm->hit();
		info.dodge += itm->dodge();
		info.def += itm->def;
		info.add_hp += itm->add_hp();
		info.add_mp += itm->add_mp();

		info.strength += itm->strength;
		info.agility += itm->agility;
		info.body_quality += itm->body_quality;
		info.stamina += itm->stamina;

		uint8_t duration_state = get_cloth_duration_state(clothes->duration, itm->duration * clothes_duration_ratio);
		itms_def += itm->def - itm->def * duration_state / 100;

	}

	
	p->def_rate += (itms_def  + suit_add_attr.def) / static_cast<double>(p->lv + 50) * 0.09;

	// TODO:
	p->hp = p->maxhp;
	p->mp = p->maxmp;

	calc_attire_gs(p, &info);


	TRACE_LOG("player attr: "
				"uid=%u lv=%u exp=%u \n \
				stre=%u agi=%u quality=%u stam=%u \n\
				hp=%u/%u mp=%u/%u atk=%u def=%f hit=%f dodge=%f crit=%u gs=%u",
				p->id, p->lv, p->exp, 
				p->strength, p->agility, p->body_quality, p->stamina,	
				p->hp, p->maxhp, p->mp, p->maxmp, p->atk, p->def_rate, 
				p->hit_rate, p->dodge_rate, p->crit_rate, p->attire_gs);
}

/**
 * @brief pack the player's fumo info to btl
 * @param p the player
 * @param pkgbuf
 * @return the idx
 */
int pack_fumo_to_btl(player_t* p, void* buf)
{
	int idx = 0;

	//total fumo points
	pack_h(buf, p->fumo_points_today, idx);
	TRACE_LOG("%u ", p->fumo_points_today);
/*
	std::map<uint32_t, fumo_stage_info_t>::iterator it;
	for ( it = p->fumo_stage_map->begin(); it != p->fumo_stage_map->end(); ++it) {
		uint16_t stage_id = it->second.stage_id;
		pack(buf, stage_id, idx);
		pack(buf, it->second.grade, sizeof(it->second.grade), idx);
		TRACE_LOG("--stage_id[%u]", stage_id);
	}
*/
	return idx;
}

int add_daily_swap_buff(player_t* p)
{
    std::vector<uint32_t> swap_vec;
    swap_vec.push_back(1439);
    swap_vec.push_back(1457);
    
    for (uint32_t i = 0; i < swap_vec.size(); i++) {
        //if (get_swap_action_times(p, swap_vec[i]) == 0 || g_swap_action_mrg.check_limit(p, swap_vec[i])) {
        if (get_swap_action_times(p, swap_vec[i]) == 0) {
            continue;
        }
        uint32_t buff_type = g_swap_action_mrg.get_buff_id( swap_vec[i] );
        
        add_buff_to_player(p, buff_type, 0, 1200, get_now_tv()->tv_sec, 0);
    }
    return 0;
}

/**
 * @brief pack the player's buf skill info to btl
 * @param p the player
 * @param pkgbuf
 * @return the idx
 */
int pack_buf_skill_to_btl(player_t* p, void* buf, uint32_t& buff_cnt)
{
	int idx = 0;

	//total fumo points
	//uint32_t cnt = p->buf_skill_map->size();
	//pack_h(buf, cnt, idx);
	TRACE_LOG("buf skill size:%lu ", p->buf_skill_map->size());
    add_daily_swap_buff(p);

	BufSkillMap::iterator it;
	for ( it = p->buf_skill_map->begin(); it != p->buf_skill_map->end(); ++it) {
		if (it->second.buf_type < max_nor_buff_type) {
			buff_cnt ++;
			pack_h(buf, it->second.buf_type, idx);
			if (it->second.buf_type == 1400) {
				pack_h(buf, it->second.start_tm, idx);
			} else {
				pack_h(buf, 0, idx);
			}
			pack_h(buf, it->second.buf_duration, idx);
			pack_h(buf, it->second.per_trim, idx);
			pack_h(buf, it->second.db_buff_flg, idx);
			pack_h(buf, it->second.mutex_type, idx);
			TRACE_LOG("buf skill[%u %u %u %u %u]", 
				it->second.buf_type, it->second.buf_duration, it->second.per_trim, it->second.mutex_type, it->second.db_buff_flg);
		}
	}

	return idx;
}

bool is_buff_actived_on_player(player_t * p, int buff_type)
{
	BufSkillMap::iterator it = p->buf_skill_map->find(buff_type);
	if (it != p->buf_skill_map->end() && it->second.buf_duration) {
//		DEBUG_LOG("BUF %u id %u %u", it->second.buf_type, it->second.buf_duration, it->second.start_tm);
		return true;
	}
	return false;
}

uint32_t get_player_speed(player_t * p)
{
    uint32_t speed = 140;
    switch (p->role_type) {
        case 1:
            speed += 5;
            break;
        case 2:
            speed += 10;
            break;
    }

	for ( uint32_t i = 0; i < p->clothes_num; i++ ) {
        const player_clothes_info_t* clothes = &(p->clothes[i]);
        const GfItem* itm = items->get_item(clothes->clothes_id);

        if (items->is_mount_clothes(itm->equip_part())) {
            speed = speed + (speed * itm->speed_ratio);
            break;
        }
    }	
    TRACE_LOG(" player speed [%u] [%u]", p->id, speed);
    return speed;
}

void del_buff_on_player(player_t * p, uint32_t buf_type)
{
	if (p->buf_skill_map->erase(buf_type)) {
		db_del_buff(p, buf_type);
	}
}

void db_del_buff(player_t * p, uint32_t buf_type)
{
	int idx = 0;
	pack_h(dbpkgbuf, static_cast<uint32_t>(buf_type), idx);
	send_request_to_db(0, p->id, p->role_tm, dbproto_del_buff_on_player, dbpkgbuf, idx);
}

/**
 * @brief pack the player's attr which need broadcast to map
 * @param buf
 * @return the idx
 */
int pack_player_lvup_noti(uint8_t* buf, uint32_t lv, uint32_t hp, uint32_t mp)
{
	int i = 0;
	pack(buf, lv, i);
	pack(buf, hp, i);
	pack(buf, mp, i);
	return i;
}

/**
 * @brief pack the player's attr which need broadcast to map
 * @param buf
 * @return the idx
 */
int pack_player_honor_up_noti(uint8_t* buf, uint32_t honor)
{
	int i = 0;
	pack(buf, honor, i);
	return i;	
}

/**
 * @brief pack the player's bass attr 
 * @param buf
 * @return the idx
 */
int pack_player_base_attr(uint8_t* buf, player_t* p, uint32_t coins)
{
	int i = 0;
	pack(buf, p->lv, i);
	pack(buf, p->exp, i);
	pack(buf, p->skill_point, i);
	pack(buf, coins, i);

	return i;	
}


//------------------------------------------------------------------
// Cmds
//------------------------------------------------------------------

/**
  * @brief player changes nickname
  * @param p the player who tends to change nickname
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int change_usr_nickname_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	body[bodylen - 1] = '\0';

	CHECK_DIRTYWORD(p, body);
	CHECK_INVALID_WORD(p, body);

    if (p->nick_change_tm != 0 && p->nick_change_tm + (86400 * 20) > get_now_tv()->tv_sec) {
        return send_header_to_player(p, p->waitcmd, cli_err_nick_change_in_timelimit, 1); 
    }
	memcpy(p->nick, body, max_nick_size);

	uint32_t need_item = 0;
#ifdef VERSION_KAIXIN
	need_item = 1700067;
#endif
	if (need_item && !(p->my_packs->is_item_exist(need_item))) {
		KDEBUG_LOG(p->id, "CHANGE NICK ERR \t[needed %u]", need_item);
		p->waitcmd = 0;
		return 0;
	}
	int idx = 0;
	pack(pkgbuf, p->nick, bodylen, idx);
	pack_h(pkgbuf, need_item, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_change_usr_nickname, pkgbuf, idx);

	//KDEBUG_LOG(p->id, "CHANG USER NICKNAME\t[%u %u %s]", p->id, p->role_tm, body);
	//return send_request_to_db(p, p->id, p->role_tm, dbproto_change_usr_nickname, body, bodylen);	
}





int forbiden_add_friend_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t flag = 0;
	unpack(body, flag, idx);
	//modify memory
	p->set_add_friend_flag(flag);	
	//db opt//
	db_forbiden_add_friend(p, flag);
	//notify client//
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, flag, idx);
	init_cli_proto_head(pkgbuf, p,  cli_proto_forbiden_add_friend, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int db_forbiden_add_friend(player_t* p, uint32_t flag)
{
	int idx = 0;
	pack_h(pkgbuf, flag, idx);
	return send_request_to_db(NULL, p->id, p->role_tm, dbproto_forbiden_add_friend_flag , pkgbuf, idx);
}


int db_query_forbiden_friend_flag(player_t* p, uint32_t mimi_id)
{
	return send_request_to_db(p,  mimi_id,  0,  dbproto_query_forbiden_friend_flag, 0, 0);
}

int db_query_forbiden_friend_flag_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	query_forbiden_friend_flag_rsp_t *rsp = reinterpret_cast<query_forbiden_friend_flag_rsp_t*>(body);
	if( rsp->err_code != 0){//没有找到该玩家
		send_header_to_player(p, cli_proto_add_friend, cli_err_add_friend_invalid_id, 1);	
		return 0;
	}

	if(rsp->flag == 1)//该玩家已经不允许被加好友
	{
		send_header_to_player(p, cli_proto_add_friend, cli_err_add_friend_fobiden, 1);
		return 0;
	}
	else
	{
		send_simple_notification(rsp->userid, p, cli_proto_add_friend, 0, p->cur_map);
		send_header_to_player(p, cli_proto_add_friend, 0, 1);
		KDEBUG_LOG(p->id, "ADD FRIEND REQ\t[%u %u]", p->id, rsp->userid);
	}
	return 0;
}




/**
 * @brief player get simple information
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_usr_simple_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	userid_t id;
	uint32_t role_time;
	unpack(body, id, idx);
	unpack(body, role_time, idx);
	TRACE_LOG("uid[%u] get info about [%u, %u]",p->id, id, role_time);

	if ( !is_valid_uid(id) ) {
		ERROR_LOG("invalid chkee to get simple info: [uid=%u, chkee=%u]",p->id, id);
		return -1;
	}
	player_t* chkee = get_player(id);
	if ( chkee && chkee->role_tm == role_time) {
		uint32_t vip_lv = is_vip_player(chkee) ? chkee->vip_level : 0;
		idx = sizeof(cli_proto_t);
		pack(pkgbuf, chkee->id, idx);
		pack(pkgbuf, chkee->role_tm, idx);
		pack(pkgbuf, chkee->role_type, idx);
	    //added by cws 20120509
		pack(pkgbuf, chkee->hp, idx);
		pack(pkgbuf, chkee->mp, idx);
        TRACE_LOG("mp[%u], hp[%u]",chkee->mp,chkee->hp);

        pack(pkgbuf, chkee->power_user, idx);
        pack(pkgbuf, chkee->using_achieve_title, idx);
		pack(pkgbuf, static_cast<uint32_t>(chkee->lv), idx);
		pack(pkgbuf, chkee->nick, sizeof(chkee->nick), idx);
		//pack(pkgbuf, chkee->color, idx);
		pack(pkgbuf, chkee->vip, idx);
		pack(pkgbuf, vip_lv, idx);
		pack(pkgbuf, (uint32_t)0, idx);
		pack(pkgbuf, (uint32_t)0, idx);
		pack(pkgbuf, (uint32_t)0, idx);
		idx += pack_player_clothes(chkee, p->waitcmd, pkgbuf + idx);

		init_cli_proto_head(pkgbuf, p, cli_proto_get_usr_simple_info, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}

	idx = 0;
	pack_h(pkgbuf, id, idx);
	pack_h(pkgbuf, role_time, idx);
/////
   // pack_h(pkgbuf, (uint32_t)0, idx);
	//	pack_h(pkgbuf, (uint32_t)0, idx);
	//	pack_h(pkgbuf, (uint32_t)0, idx);
		return send_request_to_db(p, id, role_time, dbproto_get_usr_simple_info, pkgbuf, idx);
}

void print_player_attribute(uint8_t *attr)
{
    uint16_t strength = 0, agility = 0, body = 0, stamina = 0;
    uint32_t hp = 0, mp = 0;
    uint16_t atk = 0, crit = 0;
    uint32_t def = 0, hit = 0, dodge = 0;

    int idx = 0;
    unpack(attr, strength, idx);
    unpack(attr, agility, idx);
    unpack(attr, body, idx);
    unpack(attr, stamina, idx);
    unpack(attr, hp, idx);
    unpack(attr, mp, idx);
    unpack(attr, atk, idx);
    unpack(attr, crit, idx);
    unpack(attr, def, idx);
    unpack(attr, hit, idx);
    unpack(attr, dodge, idx);

    TRACE_LOG("player attribute --> [%u %u %u %u] [%u %u] [%u %u %u %u %u]", 
        strength, agility, body, stamina, hp, mp, atk, crit, def, hit, dodge);
   //   TRACE_LOG("CWSTEST----------> [%u %u]",hp,mp); 
}

/**
 * @brief player get detail information
 * @param p the request player 
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int get_usr_detail_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	userid_t id;
	uint32_t role_time;
	unpack(body, id, idx);
	unpack(body, role_time, idx);
	TRACE_LOG("uid[%u] get info about [%u, %u]",p->id, id, role_time);

	if ( !is_valid_uid(id) ) {
		ERROR_LOG("invalid chkee to get detail info: [uid=%u, chkee=%u]",p->id, id);
		return -1;
	}

	player_t* chkee = get_player(id);
	if ( chkee  && chkee->role_tm == role_time) {

        calc_and_save_player_attribute(chkee, 1);
		idx = sizeof(cli_proto_t);
		
		pack(pkgbuf, chkee->id, idx);
		idx += pack_player_attr(chkee, pkgbuf + idx);
        print_player_attribute(chkee->other_info_2[active_player_attribute - 1]);

		init_cli_proto_head(pkgbuf, p, cli_proto_get_usr_detail_info, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}

	idx = 0;
	pack_h(pkgbuf, id, idx);
	pack_h(pkgbuf, role_time, idx);
	return send_request_to_db(p, id, role_time, dbproto_get_usr_detail_info, pkgbuf, idx);
}


/**
  * @brief player get max times chapter
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0
  */
int get_max_times_chapter_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    return send_request_to_db(p, p->id, p->role_tm, dbproto_get_max_times_chapter, 0, 0);
}

/**
  * @brief player set max times chapter
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0
  */
int set_max_times_chapter_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    int idx2 = 0;
    uint32_t chapter = 0;
    CHECK_VAL_EQ(bodylen, sizeof(uint32_t));

    unpack(body, chapter, idx);
    TRACE_LOG("times chapter: uid=[%u], chapter=[%u]", p->id, chapter);
    pack_h(dbpkgbuf, static_cast<uint32_t>(chapter), idx2);

    do_stat_log_universal_interface_1(stat_log_scan_times_chapter, 0, 1);
	send_header_to_player(p, p->waitcmd, 0, 1);

    return send_request_to_db(0, p->id, p->role_tm, dbproto_set_max_timse_chapter, dbpkgbuf, idx2);
}


/**
  * @brief player set it's client buf
  * @param p the player who tends to change nickname
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int set_client_buf_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	KDEBUG_LOG(p->id, "SET CLIENT BUF\t[%u %u %u %u %u %u %u %u %u]", p->id, p->role_tm, body[4], body[5], body[6], body[7], body[8], body[9], body[10]);
	
	send_request_to_db(0, p->id, p->role_tm, dbproto_set_client_buf, body + 4, bodylen - 4);
	memcpy(p->client_buf, body + 4, sizeof(p->client_buf));	
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, (uint8_t*)body, bodylen, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief player get it's client buf
  * @param p the player who tends to change nickname
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_client_buf_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_client_buf, 0, 0);
}


/**
 * @brief callback for get client buf
 */
int db_get_client_buf_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
    TRACE_LOG("uid=[%u] len=[%u]", p->id, bodylen);
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, (uint8_t*)body, bodylen, idx);
	memcpy(p->client_buf, (uint8_t*)body, sizeof(p->client_buf));
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}


//-----------------------------------------------------
/**
  * @brief get player's info from db
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see get_player_callback
  */
int db_get_player(player_t* p, userid_t uid)
{
	return send_request_to_db(p, uid, p->role_tm, dbproto_get_player, 0, 0);
}

/**
  * @brief set role info:exp,lv,coins...to db
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see set_role_base_info_callback
  */
int db_set_role_base_info(player_t* p)
{
	int idx = 0;
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->lv), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->exp), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(p->coins), idx);
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_player_basic_attr, dbpkgbuf, idx);
}

/**
  * @brief add amb chieve num to db
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see add_amb_achieve_num_callback
  */
int db_add_amb_achieve_num(uint32_t parentid)
{
	return send_request_to_db(0, parentid, 0, dbproto_add_amb_achieve_num, 0, 0);	
}

/**
  * @brief get fumo info from db
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see set_role_base_info_callback
  */
int db_get_fumo_info(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_fumo_info, 0, 0);
}

/**
  * @brief set stage info to db
  * @param p the requester
  * @param uid id of the requestee
  * @return 0 on success, -1 on error
  * @see set_role_base_info_callback
  */
int db_set_stage_info(player_t* p, 	uint16_t stageid, uint32_t bossid, uint32_t difficulty, 
	uint8_t stage_grade, uint32_t damage)
{
	int idx = 0;
	pack_h(dbpkgbuf, static_cast<uint32_t>(stageid), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(bossid), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(difficulty), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(stage_grade), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(damage), idx);
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_player_stage_list, dbpkgbuf, idx);
}

int db_take_off_attire(player_t* p, uint32_t unique_id)
{
	int idx = 0;
	pack_h(dbpkgbuf, unique_id, idx);
	return send_request_to_db(0, p->id, p->role_tm, dbproto_take_off_clothes, dbpkgbuf, idx);
}

/**
 * @brief get the date of double experience from database 
 */
int db_get_double_exp_data(player_t* p)
{
    return send_request_to_db(p, p->id, p->role_tm, dbproto_get_double_exp_data, 0, 0);
}

void add_buff_to_player(player_t*p, uint32_t buff_type, uint32_t mutex_type, uint32_t duration, uint32_t start_tm, uint32_t db_flg)
{
	BufSkillMap::iterator it = p->buf_skill_map->find(buff_type);
	if (it == p->buf_skill_map->end()) {
		buf_skill_t buf_skill = {0};
		buf_skill.buf_duration = duration;
		buf_skill.buf_type = buff_type;
		buf_skill.db_buff_flg = db_flg;
		buf_skill.mutex_type = mutex_type;
		buf_skill.start_tm = start_tm;
		p->buf_skill_map->insert(BufSkillMap::value_type(buf_skill.buf_type, buf_skill));
		TRACE_LOG("INSERT %u %u", buf_skill.buf_type ,buf_skill.db_buff_flg);
	} else {
		it->second.buf_duration = duration;
		it->second.start_tm = start_tm;
		TRACE_LOG("UPDATE");
	}
}

int db_add_buff(player_t* p, uint32_t buf_type, uint32_t duration, uint32_t mutex_type, bool ret_flg)
{
	int idx = 0;
    uint32_t buff_start_tm = get_now_tv()->tv_sec;
    const buff_skill_t* buff = get_buff(buf_type);
    if (buff) {
        duration = buff->duration;
    }
	pack_h(dbpkgbuf, mutex_type, idx);
	pack_h(dbpkgbuf, buf_type, idx);
    //	pack_h(dbpkgbuf, 1001, idx);
	pack_h(dbpkgbuf, duration, idx);
	pack_h(dbpkgbuf, buff_start_tm, idx);

	KDEBUG_LOG(p->id, "SET EX DB TM\t[uid=%u buff=%u duration=%u]",p->id, buf_type, duration);
	if (ret_flg) {
		return send_request_to_db(p, p->id, p->role_tm, dbproto_add_buff, dbpkgbuf, idx);
	} else {
		add_buff_to_player(p, buf_type, mutex_type, duration, buff_start_tm);
		return send_request_to_db(0, p->id, p->role_tm, dbproto_add_buff, dbpkgbuf, idx);
	}
}

/**
 * @brief set the date of double experience from database 
 */
int db_set_double_exp_data(player_t* p, uint32_t dexp_time)
{
    int idx = 0;
    if (dexp_time == 0) {
        uint32_t index = time_double_arr[get_now_tm()->tm_wday];
        if (p->lv > 19) {
            dexp_time = 2 * index;
        } else if (p->lv > 9 && p->lv < 20) {
            dexp_time = index;
        }
     /*
        if ((ptm->tm_wday > 0 && ptm->tm_wday < 5) && p->lv > 19) {
            dexp_time = 60 * 60;
        } else if ((ptm->tm_wday > 0 && ptm->tm_wday < 5) && (p->lv > 9 && p->lv < 20)) {
            dexp_time = 30 * 60;
        } else if ((ptm->tm_wday == 0 || ptm->tm_wday > 4) && p->lv > 19) {
            dexp_time = 2 * 60 * 60;
        } else if ((ptm->tm_wday == 0 || ptm->tm_wday > 4) && (p->lv > 9 && p->lv < 20)) {
            dexp_time = 60 * 60;
        }
        */
    }
    pack_h(dbpkgbuf, static_cast<uint32_t>(dexp_time), idx);
    pack_h(dbpkgbuf, static_cast<uint32_t>(dexp_time), idx);

	KDEBUG_LOG(p->id, "get double exp time %u %u", p->id, dexp_time);
    return send_request_to_db(p, p->id, p->role_tm, dbproto_set_double_exp_data, dbpkgbuf, idx);
}

void set_player_limit_time(player_t* p)
{
	if (p->adult_flg) {
		return;
	}
	TRACE_LOG("add send_online_tm_notification [timer %u %u %u] [map %u %u %u]", p->id, p->oltoday,
        battle_time_limit, p->last_mapid, p->xpos, p->ypos);
	//not need limit time
	if (p->id == 218480601 || p->id == 213331159 || p->id == 216747148 || p->id == 209741886 ||
		p->id == 217336709 || p->id == 217613599 || p->id == 216061001 ) {
		p->oltoday = 0;
	}

	if (p->oltoday < battle_time_limit) {
		ADD_TIMER_EVENT_EX(p, limit_players_online_time_idx, 0, get_now_tv()->tv_sec + (battle_time_limit - p->oltoday));
		if ((p->oltoday + report_oltoday_int) < battle_time_limit) {
			ADD_TIMER_EVENT_EX(p, send_online_tm_notification_idx, 0, get_now_tv()->tv_sec + report_oltoday_int);
		}
		if (exp_half_time_limit < battle_time_limit) {
			if (p->oltoday >= exp_half_time_limit) {
				//p->half_exp = 1;
			} else {
				ADD_TIMER_EVENT_EX(p, limit_players_online_time_idx, (void*)1, get_now_tv()->tv_sec + (exp_half_time_limit - p->oltoday));
			}
			TRACE_LOG("user today online time Oltoday:%u", p->oltoday);
		}
	}

}


/**
  * @brief callback for handling player's info returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  * @see db_get_player
  */
int db_get_player_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p,ret);

	if (p->id != id) {
		return 0;
	}

	get_player_rsp_t* rsp = reinterpret_cast<get_player_rsp_t*>(body);
	TRACE_LOG("id=[%u],bodylen=[%u],msglen=[%u],clothes_cnt=[%u],skill_bind_cnt=[%u],db_buff_cnt=[%u]",
			p->id,bodylen,rsp->offline_msg_size, rsp->clothes_cnt, rsp->skill_cnt, rsp->db_buff_cnt);
	
	CHECK_VAL_EQ(bodylen, sizeof(get_player_rsp_t) + rsp->offline_msg_size + 
		rsp->clothes_cnt * sizeof(get_player_wear_clothes_t) + 
		rsp->skill_cnt * sizeof(get_player_skill_bind_t) +
		rsp->done_tasks_num * sizeof(get_player_done_task_t) + 
		rsp->going_tasks_num * sizeof(get_player_going_task_t) + 
		rsp->cancelled_daily_tasks_num * sizeof(get_player_cancel_daily_task_t) +
		rsp->packs_clothes_cnt * sizeof(get_player_pack_clothes_t) +
		rsp->packs_item_cnt * sizeof(get_player_pack_item_t) +
		rsp->fumo_stage_cnt * sizeof(get_player_fumo_stage_t) +
		rsp->db_buff_cnt * sizeof(get_player_db_buff_t));
	
	CHECK_VAL_GE(rsp->offline_msg_size, 8);

	

	
	uint8_t* offline_msg_info = rsp->buf;
	uint8_t* clothes_info 	= rsp->buf + rsp->offline_msg_size;
	uint8_t* skill_info 	= clothes_info 			+ rsp->clothes_cnt * sizeof(get_player_wear_clothes_t);
	uint8_t* done_task_info = skill_info 			+ rsp->skill_cnt * sizeof(get_player_skill_bind_t);
	uint8_t* going_task_info = done_task_info 		+ rsp->done_tasks_num * sizeof(get_player_done_task_t);
	uint8_t* cancelled_info = going_task_info 		+ rsp->going_tasks_num * sizeof(get_player_going_task_t);
	uint8_t* packs_clothes_info  = cancelled_info 	+ rsp->cancelled_daily_tasks_num * sizeof(get_player_cancel_daily_task_t);
	uint8_t* packs_item_info = packs_clothes_info 	+ rsp->packs_clothes_cnt * sizeof(get_player_pack_clothes_t);
	uint8_t* fumo_stage_info = packs_item_info 		+ rsp->packs_item_cnt * sizeof(get_player_pack_item_t);
	uint8_t* db_buff_info = fumo_stage_info 		+ rsp->fumo_stage_cnt * sizeof(get_player_fumo_stage_t);

	unpack_basic_player_info(p, body);	

    init_player_random_event(p); /* follow basic player info*/
	set_player_limit_time(p);

	//init and calc player vip info
	init_player_vip_info(p, rsp);

	unpack_clothes_info(p, clothes_info, rsp->clothes_cnt);
	unpack_skill_info(p, skill_info, rsp->skill_cnt);
	//unpack_skill_bind_info(p, skill_info, rsp->skill_cnt);
	if (unpack_done_task_info(p, done_task_info, rsp->done_tasks_num) != 0 ||
		unpack_going_task_info(p, going_task_info, rsp->going_tasks_num) != 0 ||
		unpack_cancel_task_info(p, cancelled_info, rsp->cancelled_daily_tasks_num) != 0 ||
		unpack_packs_clothes_info(p, packs_clothes_info, rsp->packs_clothes_cnt) != 0 ||
		unpack_packs_item_info(p, packs_item_info, rsp->packs_item_cnt) != 0 ||
		unpack_fumo_stage_info(p, fumo_stage_info, rsp->fumo_stage_cnt) != 0 ||
		unpack_db_buff_info(p, db_buff_info, rsp->db_buff_cnt) != 0) {
		return -1;
	}

    //get_god_guard_mgr()->calc_group_quality_and_power(p, p->lv, p->add_quality);
	//calc_player_attr_ex(p);
	//send offline message to player
	send_offline_msg(p, offline_msg_info, rsp->offline_msg_size);
	TRACE_LOG("%u adult %u", p->id, p->adult_flg);
	do_stat_log_universal_interface_2(stat_log_new_lv_up, p->role_type, p->id, p->lv);
	return db_query_restriction_list(p);
}

/**
 * @brief callback for handling getting simple info returned from dbproxy
 * @param p the requester
 * @param uid id of the requestee
 * @param body body of the returning package
 * @param length of body
 * @param ret errno returned from dbproxy
 * @return 0 on success, -1 on error
 */
int db_get_usr_simple_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	if (ret == 1106) {
		int idx = 0;
		pack_h(dbpkgbuf, id, idx);
		send_request_to_db(0, p->id, p->role_tm, dbproto_del_friend_whatever, dbpkgbuf, idx);
		idx = sizeof(cli_proto_t);
		pack(pkgbuf, 0, idx);
		init_cli_proto_head(pkgbuf, p, cli_proto_get_usr_simple_info, idx);
	
        TRACE_LOG("cwstest no ok");
        return send_to_player(p, pkgbuf, idx, 1);

    }
	
	CHECK_DBERR(p,ret);
	get_user_simple_info_rsp_t* info = reinterpret_cast<get_user_simple_info_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(get_user_simple_info_rsp_t) + info->clothes_cnt * sizeof(clothes_info));

	uint32_t vip_lv = 0;
	if (info->vip !=0 && info->vip != 2) {
        uint32_t cur_time = time(NULL);
        if ( cur_time > info->vip_end_tm ) {
            cur_time = info->vip_begin_tm;
        }
		vip_lv = calc_player_vip_level(get_vip_config_data_mgr(), info->vip_begin_tm, cur_time, info->m_base_value, info->vip);
	}
    //added by cws20120509 (nonsence code)
    int idx1=0;
    uint16_t  strength=0, agility=0, body1=0,stamina=0;
    uint32_t hp=0,mp=0;
    uint16_t atk=0, crit=0;
    uint32_t def=0,hit=0,dodge=0; 
    unpack(info->datas,strength,idx1);
    unpack(info->datas,agility,idx1);
    unpack(info->datas,body1,idx1);
    unpack(info->datas,stamina,idx1);
    unpack(info->datas,hp,idx1);
    unpack(info->datas,mp,idx1);
    unpack(info->datas,atk,idx1);
    unpack(info->datas,crit,idx1);
    unpack(info->datas,def,idx1);
    unpack(info->datas,hit,idx1);
    unpack(info->datas,dodge,idx1);
    TRACE_LOG("cwstest player attribute --> [%u %u %u %u] [%u %u] [%u %u %u %u %u]", 
        strength, agility, body1, stamina, hp, mp, atk, crit, def, hit, dodge);
 
        
    TRACE_LOG("simple info[%u %u %u %u %u %u]", p->id, info->vip, info->vip_begin_tm, info->vip_end_tm, info->m_base_value, vip_lv);
//
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, info->id, idx);
	pack(pkgbuf, info->role_tm, idx);
	pack(pkgbuf, info->role_type, idx);
	//added by cws 20120509
    pack(pkgbuf, hp, idx);
	pack(pkgbuf, mp, idx);
    
    pack(pkgbuf, info->power_user, idx);
    pack(pkgbuf, (uint32_t)0, idx);
	pack(pkgbuf, info->lv, idx);
	pack(pkgbuf, info->nick, sizeof(info->nick), idx);
	pack(pkgbuf, info->vip, idx);
	pack(pkgbuf, vip_lv, idx);
	pack(pkgbuf, (uint32_t)0, idx);
	pack(pkgbuf, (uint32_t)0, idx);
	pack(pkgbuf, (uint32_t)0, idx);
	pack(pkgbuf, info->clothes_cnt, idx);
	TRACE_LOG("[id=%u, clothes_cnt=%u]",info->id,info->clothes_cnt);
	for ( uint32_t i = 0; i < info->clothes_cnt; i++) {
		pack(pkgbuf, info->clothes[i].clothesid, idx);
		pack(pkgbuf, info->clothes[i].gettime, idx);
		pack(pkgbuf, info->clothes[i].endtime, idx);
		pack(pkgbuf, info->clothes[i].attirelv, idx);
		TRACE_LOG("[id=%u, clothes_id=%u]",info->id,info->clothes[i].clothesid);
	}
	TRACE_LOG("[id=%u,role_tm=[%u],nick=[%s]",info->id, info->role_tm, info->nick);
	
	init_cli_proto_head(pkgbuf, p, cli_proto_get_usr_simple_info, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
 * @brief callback for handling getting detail info returned from dbproxy
 * @param p the requester
 * @param uid id of the requestee
 * @param body body of the returning package
 * @param length of body
 * @param ret errno returned from dbproxy
 * @return 0 on success, -1 on error
 */
int db_get_usr_detail_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	if (ret == 1106) {
		int idx = 0;
		pack_h(dbpkgbuf, id, idx);
		send_request_to_db(0, p->id, p->role_tm, dbproto_del_friend_whatever, dbpkgbuf, idx);
		idx = sizeof(cli_proto_t);
		pack(pkgbuf, 0, idx);
		init_cli_proto_head(pkgbuf, p, cli_proto_get_usr_detail_info, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}
	CHECK_DBERR(p,ret);

	get_user_detail_info_rsp_t* info = reinterpret_cast<get_user_detail_info_rsp_t*>(body);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, info->id, idx);
	pack(pkgbuf, info->lv, idx);
	pack(pkgbuf, info->exp, idx);
	pack(pkgbuf, 0, idx);
	pack(pkgbuf, 0, idx);

	
	pack(pkgbuf, info->honor, idx);
	pack(pkgbuf, info->exploit, idx);
	pack(pkgbuf, info->win_times, idx);
	pack(pkgbuf, info->lose_times, idx);
	pack(pkgbuf, info->max_conti_win_times, idx);
	pack(pkgbuf, info->using_achieve_title, idx);
	pack(pkgbuf, info->datas, ACTIVE_BUF_LEN, idx);

    //print_player_attribute(info->datas);	
	TRACE_LOG("[id=%u,[%u %u]",info->id, info->lv, info->exp);
	
	init_cli_proto_head(pkgbuf, p, cli_proto_get_usr_detail_info, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief callback for handling nickname change action returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_change_usr_nickname_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p,ret);

	//send_request_to_cachesvr(0, id, cachesvr_update_timestamp, 0, 0);
	uint32_t used_item = *(reinterpret_cast<uint32_t*>(body));
	if (used_item) {
		p->my_packs->del_item(p, used_item, 1, channel_string_other);
		send_use_item_rsp(p, used_item, 0);
	}
    p->nick_change_tm = get_now_tv()->tv_sec;
	
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, p->role_tm, idx);
	pack(pkgbuf, p->nick, max_nick_size, idx);
	TRACE_LOG("[id=%u, role_tm=%u, nick=%s]",p->id, p->role_tm, p->nick);

	init_cli_proto_head(pkgbuf, p, cli_proto_change_usr_nickname, idx);
	if (p->trade_grp) {
		send_to_player(p, pkgbuf, idx, 1);
	} else {
		send_to_map(p, pkgbuf, idx, 1);
	}

	return 0;
}

 /**
  * @brief callback for get fumo info
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_fumo_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	get_fumo_info_rsp_t* info = reinterpret_cast<get_fumo_info_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(get_fumo_info_rsp_t) + info->count * sizeof(fumo_stage_t));

	p->fumo_stage_map->clear();
	fumo_stage_t* p_stage = info->fumo_stage;
	for (uint32_t i = 0; i < info->count; ++i) {
		p->fumo_stage_map->insert(std::map<uint32_t, fumo_stage_info_t>::value_type
			(p_stage->stage_id, fumo_stage_info_t(p_stage->stage_id, p_stage->grade)));
		TRACE_LOG("%u %u %u %u %u", p_stage->stage_id, p_stage->grade[0], p_stage->grade[1], p_stage->grade[2]
			, p_stage->grade[3]);
		p_stage ++;
	}

//	p->m_medal_count = calc_player_medal_count(p);
	return 0;
}
 /**
  * @brief callback for get double exp time info
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_double_exp_data_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);

    get_double_exp_data *data = reinterpret_cast<get_double_exp_data*>(body);
    CHECK_VAL_EQ(bodylen, sizeof(get_double_exp_data));

    TRACE_LOG("double exp time set: uid=[%u] day_flag=[%d] time=[%u]", p->id, data->day_flag, data->double_exp_time);
    if (data->day_flag != 0) {
        //return cli_err_limit_time_double_exp;
        return send_header_to_player(p, p->waitcmd, cli_err_limit_time_double_exp, 1);
    }

    return db_set_double_exp_data(p, 0);
}
 /**
  * @brief callback for set double exp time info
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_set_double_exp_data_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);

    get_double_exp_data *data = reinterpret_cast<get_double_exp_data*>(body);
    CHECK_VAL_EQ(bodylen, sizeof(get_double_exp_data));

	p->day_flag += data->double_exp_time;
    p->dexp_time += data->double_exp_time;
    TRACE_LOG("double exp time set: uid=[%u] time=[%u]", p->id, p->dexp_time);
	restriction_t* p_restr = 0;
	if (p->waitcmd == cli_proto_double_exp_action) {
		p_restr = &(g_all_restr[double_exp_daily_action - 1]);
	} else if (p->waitcmd == cli_proto_double_exp_action_use_item) {
		p_restr = &(g_all_restr[double_exp_daily_use_item - 1]);
	} else {
		return -1;
	}
//	p->sesslen = 0;
//	pack(p->session, data, sizeof(*data), p->sesslen);
	return db_add_action_count(p, p->id, p->role_tm, p_restr->id, p_restr->restr_flag, p_restr->toplimit, p_restr->userflg);
	 
}


int db_add_buff_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	add_buf_rsp_t *data = reinterpret_cast<add_buf_rsp_t*>(body);

	add_buff_to_player(p, data->type, data->mutex_type, data->duration, data->start_tm);

	KDEBUG_LOG(p->id, "ADD BUFF\t%u %u %u", p->id, data->type, data->duration);
	if (p->waitcmd == cli_proto_get_buff_skill) {
		return send_get_buff_skill_rsp(p, data);
	}
	uint32_t item_id = *(reinterpret_cast<uint32_t*>(p->session));
	return send_use_item_rsp(p, item_id, 1);
}

/**
 * @brief callback for get max times chapter
 */
int db_get_max_times_chapter_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);

    uint32_t *chapter = reinterpret_cast<uint32_t*>(body);
    TRACE_LOG("times chapter: uid=[%u] chapter=[%u]", p->id, *chapter);
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, *chapter, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}
//------------------------------------------------------------------
// static function
//------------------------------------------------------------------
static void unpack_basic_player_info(player_t* p, const void* info)
{
	int i = 0;
	uint32_t tmp_lv = 0;
	unpack_h(info, p->role_tm, i);
	unpack_h(info, p->role_type, i);
	unpack_h(info, p->power_user, i);
	unpack_h(info, p->flag, i);
	unpack_h(info, p->reg_tm, i);
	unpack(info, p->nick, sizeof(p->nick), i);
	unpack_h(info, p->nick_change_tm, i);
	unpack_h(info, p->vip, i);
	unpack_h(info, p->vip_months_cnt, i);
	
	unpack_h(info, p->vip_begin_time, i);
	unpack_h(info, p->vip_end_time, i);
	unpack_h(info, p->vip_base_point, i);
	unpack_h(info, p->vip_sword_value, i);
	//unpack_h(info, p->extern_item_bag_grid_count, i);
	unpack_h(info, p->extern_warehouse_grid_count, i);
	
	unpack_h(info, p->skill_point, i);
    unpack_h(info, p->last_mapid, i);
    unpack_h(info, p->xpos, i);
    unpack_h(info, p->ypos, i);
	unpack_h(info, tmp_lv, i);
	unpack_h(info, p->exp, i);
	unpack_h(info, p->allocator_exp, i);
	unpack_h(info, p->hp, i);
	unpack_h(info, p->mp, i);
	unpack_h(info, p->coins, i);
	
	unpack_h(info, p->honor, i);
	unpack_h(info, p->exploit, i);
	unpack_h(info, p->win_times, i);
	unpack_h(info, p->lose_times, i);
	unpack_h(info, p->max_conti_win_times, i);

	//fumo points
	unpack_h(info, p->fumo_points_today, i);
	unpack_h(info, p->fumo_points_total, i);
	unpack_h(info, p->fumo_tower_top, i);
	unpack_h(info, p->fumo_tower_used_tm, i);
	unpack_h(info, p->olcount, i);
	unpack_h(info, p->oltoday, i);
	unpack_h(info, p->ollast, i);
	unpack_h(info, p->oltime, i);

	unpack(info, p->unique_item_bit, sizeof(p->unique_item_bit), i);
	unpack_h(info, p->itembind, i);
	unpack_h(info, p->amb_status, i);
	unpack_h(info, p->parentid, i);
	unpack_h(info, p->child_cnt, i);
	unpack_h(info, p->achieve_cnt, i);
	unpack(info, p->once_bit, sizeof(p->once_bit), i);
	unpack(info, p->act_record, sizeof(p->act_record), i);
	unpack_h(info, p->dexp_time, i);
	p->dexp_time = 0;
	unpack_h(info, p->day_flag, i);
	unpack_h(info, p->max_times_chapter, i);
	unpack_h(info, p->player_show_state, i);
	unpack_h(info, p->open_box_times, i);
	unpack_h(info, p->strengthen_cnt, i);
	unpack_h(info, p->achieve_point, i);
	unpack_h(info, p->last_update_ap_tm, i);
	unpack_h(info, p->using_achieve_title, i);
	unpack_h(info, p->forbiden_add_friend_flag, i);
	uint32_t home_active_point;
	uint32_t home_last_tm;
	unpack_h(info, home_active_point, i);
	unpack_h(info, home_last_tm, i);
	unpack_h(info, p->vitality_point, i);
	unpack_h(info, p->team_info.team_id, i);
	unpack(info, p->group_datas, sizeof(group_t), i);
    i += (80 - sizeof(group_t));

	//p->m_home_data->set_action_point(home_active_point, home_last_tm);

	TRACE_LOG("[%u %u %u %u %x] double exp time=[%u] flag=[%u] chapter=[%u] oltoday=[%u]",
        p->amb_status, p->parentid, p->child_cnt, p->achieve_cnt, p->once_bit[0], 
        p->dexp_time, p->day_flag, p->max_times_chapter, p->oltoday);

	p->lv = tmp_lv;

	if (get_date(get_now_tv()->tv_sec) != get_date(p->ollast )){		
		p->oltoday = 0;		
		TRACE_LOG("date diff: %u != %u", get_date(get_now_tv()->tv_sec), get_date(p->ollast ));
	}
	p->oltoday_db = p->oltoday;

	TRACE_LOG("[%u, %u ,%u, %u, %s, %u, %u, %u, %u, %u, %u, %u, \
		%x %x %x %u %u %u %u %u %u %u %u %u %u %u %x %x %u %u]",
				p->role_tm,p->role_type,p->flag,p->reg_tm,p->nick,p->vip,p->skill_point,p->coins,
				p->olcount,p->oltoday,p->ollast,p->oltime,p->unique_item_bit[0],
				p->unique_item_bit[1],p->unique_item_bit[2],p->honor,p->exploit,p->win_times,
				p->lose_times,p->max_conti_win_times, p->fumo_points_today, p->fumo_points_total,
				p->amb_status, p->parentid, p->child_cnt, p->achieve_cnt, p->once_bit[0], p->once_bit[10], p->fumo_tower_top, p->fumo_tower_used_tm);
}

static void unpack_clothes_info(player_t* p, void* clothes_info, uint16_t clothes_cnt)
{
	if ( clothes_cnt <= max_clothes_on ) {
		p->clothes_num = clothes_cnt;
	} else {
		ERROR_LOG("too many clothes on: uid=%u, cnt=%d",p->id,clothes_cnt);
		p->clothes_num = max_clothes_on;
	}

	uint32_t conflict_flag = 0x00000000;
    player_clothes_info_t clothes = {0};
	int idx = 0;
	for ( uint32_t i = 0; i < p->clothes_num; ) {
        memset(&clothes, 0x00, sizeof(player_clothes_info_t));
		uint32_t duration = 0, attirelv = 0;
        unpack_h(clothes_info, clothes.clothes_id, idx);
		unpack_h(clothes_info, clothes.unique_id, idx);
		unpack_h(clothes_info, duration, idx);
		unpack_h(clothes_info, attirelv, idx);
		unpack_h(clothes_info, clothes.gettime, idx);
		unpack_h(clothes_info, clothes.timelag, idx);

        const GfItem* itm = items->get_item(clothes.clothes_id);
        uint32_t tmp_pos = itm->equip_part();
        uint32_t tmp_flag = (0x01 << tmp_pos) & 0xffffffff;
        if (conflict_flag & tmp_flag) {
            ERROR_LOG("%u WEAR CLOTEHS WITH SAME EQUIP PART %u", p->id,  itm->equip_part());
            conflict_flag |= tmp_flag;
            p->clothes_num--;
            db_take_off_attire(p, clothes.unique_id);
            continue;
        }
        conflict_flag |= tmp_flag;

        p->clothes[i].clothes_id    = clothes.clothes_id;
        p->clothes[i].unique_id     = clothes.unique_id;
        p->clothes[i].lv            = attirelv;
        p->clothes[i].duration      = duration;
        p->clothes[i].gettime       = clothes.gettime;
        p->clothes[i].timelag       = clothes.timelag;

        TRACE_LOG("clothesid=[%u], unique_id=[%u], duration=[%u],gettime=[%u],timelag=[%u],lv=[%u]",
            p->clothes[i].clothes_id, p->clothes[i].unique_id, p->clothes[i].duration,
            p->clothes[i].gettime, p->clothes[i].timelag,p->clothes[i].lv);
        i++;
    }

    TRACE_LOG("clothes_cnt=[%d]",p->clothes_num);
}

/*
static void unpack_skill_bind_info(player_t* p, void* skill_info, uint16_t skill_cnt)
{
	if( skill_cnt <= max_skills_bind ) {
		p->skill_bind_num = skill_cnt;
	} else {
		ERROR_LOG("too many skill bind:uid=%u cnt=%u", p->id, skill_cnt);
		p->skill_bind_num = max_skills_bind;
	}
	
	player_skill_bind_t* skill = reinterpret_cast<player_skill_bind_t*>(skill_info);
	for ( uint32_t i = 0; i < p->skill_bind_num; i++ ) {
		p->skills_bind[i].skill_id = skill[i].skill_id;
		p->skills_bind[i].lv = skill[i].lv;
		p->skills_bind[i].key = skill[i].key;
		TRACE_LOG("uid=[%u] skillid=[%u], lv=[%u], key=[%u]",p->id, p->skills_bind[i].skill_id, 
				p->skills_bind[i].lv, p->skills_bind[i].key);
	}
}
*/
static void unpack_skill_info(player_t* p, void* skill_info, uint16_t skill_cnt)
{
	p->skill_bind_num = 0;
	player_skill_bind_t* skill = reinterpret_cast<player_skill_bind_t*>(skill_info);
	for (uint32_t i = 0; i < skill_cnt; i++) {
		(*p->player_skill_map)[(skill + i)->skill_id] = *(skill + i);
		if ((skill+i)->key) {
			if (p->skill_bind_num > max_skills_bind) {
				continue;
			}
			p->skills_bind[p->skill_bind_num].skill_id = skill[i].skill_id;
			p->skills_bind[p->skill_bind_num].lv = skill[i].lv;
			p->skills_bind[p->skill_bind_num].key = skill[i].key;
			TRACE_LOG("uid=[%u] skillid=[%u], lv=[%u], key=[%u]",p->id, 
					p->skills_bind[p->skill_bind_num].skill_id, 
					p->skills_bind[p->skill_bind_num].lv, 
					p->skills_bind[p->skill_bind_num].key);
			p->skill_bind_num ++;
		}
	}

}

#ifdef NEW_LOGIN_PACKAGE
static int unpack_done_task_info(player_t* p, void* done_task_info, uint32_t cnt)
{
	p->finished_tasks_set->clear();
	get_player_done_task_t* p_task = reinterpret_cast<get_player_done_task_t*>(done_task_info);
	for ( uint32_t i = 0; i < cnt; i++) {
		uint32_t taskid = p_task[i].task_id;
		TRACE_LOG("taskid=[%u]",taskid);
		p->finished_tasks_set->insert(taskid);
	}
    //del_clear_done_task(p);
	return 0;
}

static int unpack_going_task_info(player_t* p, void* going_task_info, uint32_t cnt)
{
	if ( cnt > (max_primary_tasks + max_minor_tasks + max_daily_tasks) ) {
		ERROR_LOG("the number of going tasks error!\t[uid=%u, role_tm=%u, tasks_num=%u]",
								p->id, p->role_tm, cnt);
		return -1;
	}
	
	p->going_tasks_map->clear();
	p->minior_tasks_num = 0;
	get_player_going_task_t* p_task = reinterpret_cast<get_player_going_task_t*>(going_task_info);
	for ( uint32_t i = 0; i < cnt; i++) {
			task_going_t task_info;
			memcpy(&task_info, p_task + i, sizeof(task_going_t));
			p->going_tasks_map->insert(std::map<uint32_t, task_going_t>::value_type(task_info.id, task_info));
			if ( get_task_type(task_info.id) == minor_task || get_task_type(task_info.id) == active_task) {
				p->minior_tasks_num++;
			}
			TRACE_LOG("going_task_info.id=[%u]", task_info.id);
	}
    //del_clear_going_task(p);
	return 0;
}
static int unpack_cancel_task_info(player_t* p, void* cancel_task_info, uint32_t cnt)
{
	p->canceled_tasks_set->clear();
	get_player_cancel_daily_task_t* p_task = reinterpret_cast<get_player_cancel_daily_task_t*>(cancel_task_info);
	for ( uint32_t i = 0; i < cnt; i++) {
		uint32_t taskid = p_task[i].task_id;
		TRACE_LOG("taskid=[%u]",taskid);
		p->canceled_tasks_set->insert(taskid);
	}
	return 0;
}
static int unpack_packs_clothes_info(player_t* p, void* pack_clothes_info, uint32_t cnt)
{
	if (cnt > get_player_total_item_bag_grid_count(p)) {
		WARN_LOG("pack too large:[%u %u %u]", p->id, cnt, get_player_total_item_bag_grid_count(p));
	}
	get_player_pack_clothes_t* p_pack = reinterpret_cast<get_player_pack_clothes_t*>(pack_clothes_info);
	for (uint32_t i = 0; i < cnt; i++) {
		p->my_packs->add_clothes(p, p_pack[i].clothes_id, 
			p_pack[i].unique_id, p_pack[i].duration, 0, p_pack[i].gettime, p_pack[i].timelag,p_pack[i].attirelv);
	}
	TRACE_LOG("pack clothes cnt[%u %u]",p->id, p->my_packs->all_clothes_cnt());
	return 0;
}
static int unpack_packs_item_info(player_t* p, void* pack_item_info, uint32_t cnt)
{
	if (cnt > get_player_total_item_bag_grid_count(p)) {
		WARN_LOG("pack too large:[%u %u %u]", p->id, cnt, get_player_total_item_bag_grid_count(p));
	}
	get_player_pack_item_t* p_pack = reinterpret_cast<get_player_pack_item_t*>(pack_item_info);
	for (uint32_t i = 0; i < cnt; i++) 
	{
		p->my_packs->add_item(p, p_pack[i].item_id, p_pack[i].cnt, 0, false);
	}
	TRACE_LOG("pack items cnt[%u %u]",p->id, p->my_packs->all_items_cnt());
	return 0;
}
static int unpack_fumo_stage_info(player_t* p, void* fumo_stage_info, uint32_t cnt)
{
	p->fumo_stage_map->clear();
	get_player_fumo_stage_t* p_stage = reinterpret_cast<get_player_fumo_stage_t*>(fumo_stage_info);
	for (uint32_t i = 0; i < cnt; ++i) {
		p->fumo_stage_map->insert(std::map<uint32_t, fumo_stage_info_t>::value_type
			(p_stage[i].stage_id, fumo_stage_info_t(p_stage[i].stage_id, p_stage[i].grade)));
		TRACE_LOG("%u %u %u %u %u", p_stage[i].stage_id, p_stage[i].grade[0], p_stage[i].grade[1], p_stage[i].grade[2]
			, p_stage[i].grade[3]);
	}
//	p->m_medal_count = calc_player_medal_count(p);

	return 0;
}

static int unpack_db_buff_info(player_t* p, void* db_buff_info, uint32_t cnt)
{
	p->buf_skill_map->clear();
	get_player_db_buff_t* p_buff = reinterpret_cast<get_player_db_buff_t*>(db_buff_info);
	for (uint32_t i = 0; i < cnt; ++i) {
		buf_skill_t buf_tmp = { 0 };
		buf_tmp.buf_duration = p_buff->duration;
		buf_tmp.buf_type = p_buff->buff_type;
		buf_tmp.mutex_type = p_buff->mutex_type;
		buf_tmp.start_tm = p_buff->start_tm;
		buf_tmp.db_buff_flg = 1;
		
		p->buf_skill_map->insert(BufSkillMap::value_type(buf_tmp.buf_type, buf_tmp));
		TRACE_LOG("%u %u %u", buf_tmp.buf_type, buf_tmp.buf_duration, buf_tmp.db_buff_flg);

		p_buff++;
	}
	return 0;
}

#endif
static int send_offline_msg(player_t* p, uint8_t* msg, uint32_t size)
{
	int idx = 0;
	uint32_t total_len = 0;
	uint32_t msg_cnt = 0;
	uint32_t msg_len = 0;
	uint32_t pkglen = 0;

	unpack_h(msg, total_len, idx);
	unpack_h(msg, msg_cnt, idx);
	KDEBUG_LOG(p->id, "id=[%u], total_len=[%u], msg_cnt=[%u]",p->id, total_len, msg_cnt);
	if (total_len == size) {
		for (uint32_t i = 0; i < msg_cnt; i++) {
			if ( (total_len - idx) < (4 + sizeof(cli_proto_t))){
				ERROR_LOG("invalid offline msg: uid=%u cnt=%u remainlen=%u", p->id, msg_cnt, total_len -idx);
				break;
			}
			unpack_h(msg, msg_len, idx);
			TRACE_LOG("id=[%u], msg_len=[%u]",p->id, msg_len);
			pkglen = msg_len - sizeof(msg_len);
			if ( ((idx + pkglen) <= total_len) && (pkglen > sizeof(cli_proto_t))) {
				send_to_player(p, msg+idx, pkglen, 0);
				idx += pkglen;
			} else {
				ERROR_LOG("invalid offline msg: uid=%u total=%u cnt=%u pkglen=%u", p->id, total_len, msg_cnt, pkglen);
				break;
			}
		}
	} else {
		ERROR_LOG("offline msg len dismatch: uid=%u len=%u %u", p->id, total_len, size);
	}
	return 0;
}


/**
  * @brief send a UDP request to db server
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
static int report_user_onoff_to_db(const player_t* p, userid_t id, uint32_t loginflag)
{
	int idx = 0;

	pack_h(pkgbuf, loginflag, idx);
	pack_h(pkgbuf, static_cast<uint32_t>(get_server_id()), idx);
	pack_h(pkgbuf, static_cast<uint32_t>(get_now_tv()->tv_sec), idx);

	send_udp_onoff_to_db(p, p->id, dbproto_report_user_onoff, pkgbuf, idx);

	return 0;
}

int db_set_player_show_state(player_t* p, uint32_t player_state)
{
	int idx = 0; 
	pack_h(dbpkgbuf,  player_state, idx);
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_player_show_state, dbpkgbuf, idx);
}

int db_set_player_open_box_times(player_t* p, uint32_t times)
{
	int idx = 0;
	pack_h(dbpkgbuf, times, idx);
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_open_box_times, dbpkgbuf, idx);
}


int set_player_show_state_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t show_state = 0;
	unpack(body, show_state, idx);


	p->player_show_state = show_state;
	db_set_player_show_state(p, show_state);

	//db_add_donate(p, show_state);
	
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->id, idx);
	pack(pkgbuf, show_state, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	
	send_to_map_except_self(p, pkgbuf, idx);	
	return send_to_player(p, pkgbuf, idx, 1);
}

int db_get_friend_list(player_t *p)
{
	uint32_t friend_type = FRIENDS;
	int idx = 0;
	pack_h(dbpkgbuf, friend_type, idx); 
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_friend_list, dbpkgbuf, idx);

}

int db_get_friend_list_callback(player_t *p, uint32_t uid,  void *body, uint32_t bodylen, uint32_t ret)
{
	
	CHECK_DBERR(p, ret);
	get_friend_rsp_t * pkg = reinterpret_cast<get_friend_rsp_t *>(body);
	CHECK_VAL_EQ(bodylen, sizeof(get_friend_rsp_t)+ (pkg->friend_cnt)*sizeof(friend_t));
	uint32_t friend_cnt = pkg->friend_cnt;	
	uint8_t tmbuf[8192];
	p->friends_vec->clear();

	if (p->waitcmd == cli_proto_login) {
		for(uint32_t i = 0; i <friend_cnt; ++i) {
			p->friends_vec->push_back(pkg->friends[i].userid);
		}
		return send_login_rsp(p);
	}

	int idx = sizeof(mcast_pkg_t);
	pack_h(pkgbuf, p->id, idx);
	pack(pkgbuf, p->nick, max_nick_size, idx);
	pack_h(pkgbuf, friend_cnt, idx);
	init_mcast_pkg_head(pkgbuf, mcast_friend_login, 0);
	for(uint32_t i = 0; i <friend_cnt; ++i)
	{
		p->friends_vec->push_back(pkg->friends[i].userid);
		pack_h(pkgbuf, pkg->friends[i].userid, idx); 
		player_t * f = get_player(pkg->friends[i].userid);
		if (f)
		{
			int fdx = sizeof(cli_proto_t);
			pack(tmbuf, p->id, fdx);
			pack(tmbuf, p->nick, max_nick_size, fdx);
			init_cli_proto_head(tmbuf, f, cli_proto_friend_login, fdx);
			send_to_player(f, tmbuf, fdx, 0);
		}
	}
	send_mcast_pkg(pkgbuf, idx);
	return 0;
}

int db_get_user_public_info(player_t *p)
{
	return send_request_to_db_no_roletm(p, p->id, dbproto_get_user_public_info, 0, 0);
}

int send_user_public_info(player_t* p, get_user_public_info_rsp_t* p_rsp)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, (uint32_t)(p->adult_flg), idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_user_public_info, idx);
	return send_to_player(p, pkgbuf, idx, 0);
}

int db_get_user_public_info_callback(player_t *p, uint32_t uid,  void *body, uint32_t bodylen, uint32_t ret)
{
	
	if (ret || bodylen < sizeof(get_user_public_info_rsp_t)) {
		ERROR_LOG("%u get_user_public_info error: %u %u", p->id, ret, bodylen);
		return 0;
	}
	get_user_public_info_rsp_t* pkg = reinterpret_cast<get_user_public_info_rsp_t*>(body);
	if (strlen(pkg->personal_id)) {
		p->adult_flg = 1;
	}
	KDEBUG_LOG(p->id, "get_user_public_info: %u %s", p->adult_flg, pkg->personal_id);
	return send_user_public_info(p, pkg);
}


int get_player_hero_top_cmd(player_t *p, uint8_t *body = 0, uint32_t bodylen =0)
{
	if (p->lv >= min_hero_top_lv)
	{
		return db_get_player_hero_top(p);
	}
	else {
		uint32_t sort = default_hero_sort;
		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, sort, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}
}

int db_get_player_hero_top(player_t *p)
{
	int idx = 0;
	pack_h(dbpkgbuf, p->exp, idx); 
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_player_hero_top, dbpkgbuf, idx);
}

int db_get_player_hero_top_callback(player_t *p, uint32_t uid, void *body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	hero_top_rsp_t * pkg = reinterpret_cast<hero_top_rsp_t *> (body);
	uint32_t sort = pkg->sort; 
	if (pkg->sort >= min_hero_top_sort)
	{
		sort = default_hero_sort;
	}
	sort++;
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, sort, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int db_set_player_hero_top_info(player_t *p)
{
	int idx = 0;
	pack_h(dbpkgbuf, (uint32_t)p->lv, idx);
	pack_h(dbpkgbuf, p->exp, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_set_player_hero_top, dbpkgbuf, idx);
}

int db_set_player_hero_top_callback(player_t *p, uint32_t uid, void *body, uint32_t bodylen, uint32_t ret)
{
	return 0;
}

int get_donate_count_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return db_get_donate_count(p);	
}

int db_get_donate_count(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_donate_count, 0, 0);
}

int db_get_donate_count_callback(player_t *p, uint32_t uid, void *body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	donate_rsp_t *rsp = reinterpret_cast<donate_rsp_t*>(body);
	int idx = sizeof(cli_proto_t);
	pack( pkgbuf, rsp->red, idx);
	pack( pkgbuf, rsp->black, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int db_add_donate(player_t* p, int add_value,  bool callback )
{
	int idx = 0;
	pack_h(dbpkgbuf, add_value, idx);
	if(callback){
		return send_request_to_db(p,  p->id,  p->role_tm, dbproto_add_donate, dbpkgbuf, idx);
	}
	return send_request_to_db(NULL,  p->id,  p->role_tm, dbproto_add_donate, dbpkgbuf, idx);
}


int db_set_offline_info(player_t * p, uint32_t player_lv, uint32_t p_s_tm, uint32_t s_s_tm, uint32_t coins, uint32_t fumo, 
	uint32_t player_add_exp = 0, uint32_t mon_tm = 0, uint32_t mon_add_exp = 0, uint32_t add_mon_lv = 0, 
	uint32_t reduce_coins = 0, uint32_t reduce_fumo = 0, uint32_t summon_end_flag = 0)
{
	int idx = 0;
	pack_h(dbpkgbuf, player_lv, idx);
	pack_h(dbpkgbuf, p_s_tm, idx);
	pack_h(dbpkgbuf, s_s_tm, idx);
	pack_h(dbpkgbuf, coins, idx);
	pack_h(dbpkgbuf, fumo, idx);
	pack_h(dbpkgbuf, player_add_exp, idx);
	pack_h(dbpkgbuf, mon_tm, idx);
	pack_h(dbpkgbuf, mon_add_exp, idx);
	pack_h(dbpkgbuf, add_mon_lv, idx);
	pack_h(dbpkgbuf, reduce_coins, idx);
	pack_h(dbpkgbuf, reduce_fumo, idx);
	pack_h(dbpkgbuf, summon_end_flag, idx);
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_offline_info, dbpkgbuf, idx);
}

int db_set_role_info(player_t* p, const char* column_str, uint32_t value)
{
	char column_buf[32] = "";
	int idx = 0;
	strcpy(column_buf, column_str);
	pack(dbpkgbuf, column_buf, sizeof(column_buf), idx);
	pack_h(dbpkgbuf, value, idx);
	return send_request_to_db(0, p->id, p->role_tm, dbproto_set_role_info, dbpkgbuf, idx);
}

/** 
 * @brief  get the total info for offline 
 * 
 **/
int insert_offline_fumo_coins_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
    uint32_t activeid = 0;
	uint32_t coins = 0;
	uint32_t fumo = 0;
	int idx = 0;
	unpack(body, activeid, idx);
	unpack(body, coins, idx);
	unpack(body, fumo, idx);
	uint32_t offline_swap_id = 1169;
	if (!get_swap_action_times(p, offline_swap_id)) {
		ERROR_LOG("CAN OPEN OFFLINE MODE %u", p->id);
        return send_header_to_player(p, p->waitcmd, cli_err_not_in_offline_mode, 1);
	}

	if (!(p->offline_data)) {
		return -1;
	}

    if (activeid) {
        coins   = 1000000;
        fumo    = 1000;

        uint32_t offline_active_limit_swap_id = 1254;
        if (get_swap_action_times(p, offline_active_limit_swap_id)) {
            return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
        }

        uint32_t ret_err = check_player_can_join_active(p, 10);
        if (ret_err) {
            return send_header_to_player(p, p->waitcmd, ret_err, 1);
        }

        if (p->offline_data->off_coins + coins > 10000000 || p->offline_data->off_fumo + fumo > 1000000) {
            return send_header_to_player(p, p->waitcmd, cli_err_team_coins_limit, 1);
        }
        db_set_offline_info(p, p->lv,
                            p->offline_data->player_start_tm,
                            p->offline_data->summon_start_tm,
                            p->offline_data->off_coins + coins,
                            p->offline_data->off_fumo + fumo
                            );
        add_swap_action_times(p, offline_active_limit_swap_id);
    } else {
        if (p->coins < coins || p->fumo_points_total < fumo || coins > 10000000 ||
            fumo >= 1000000) {
            ERROR_LOG("DON't HAS ANY FUMO OR COINS IN OFFLINE %u", p->id);
            return -1;
        }

        do_stat_log_coin_add_reduce(p, (uint32_t)0, coins);
        p->coins -= coins;
        p->fumo_points_total -= fumo;
        do_stat_log_fumo_add_reduce((uint32_t)0, fumo);

        db_set_offline_info(p, p->lv,
                            p->offline_data->player_start_tm,
                            p->offline_data->summon_start_tm,
                            p->offline_data->off_coins + coins,
                            p->offline_data->off_fumo + fumo,
                            0,
                            0,
                            0,
                            0,
                            coins,
                            fumo);
    }

	p->offline_data->off_coins += coins;
	p->offline_data->off_fumo += fumo;
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, activeid, idx);
	pack(pkgbuf, coins, idx);
	pack(pkgbuf, fumo, idx);
	init_cli_proto_head(pkgbuf, p,  p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int get_offline_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_offline_info, 0, 0);
}

int exchange_coins_exploit_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	uint32_t type = 0;
	uint32_t in_value = 0;
	uint32_t other_value = 0;
	int idx = 0;
	unpack(body, type, idx);
	unpack(body, in_value, idx);

	if (type == 1) {
	//coins -> exploit
		if (p->coins < in_value) {
			return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
		}
		if (in_value < 100) {
			in_value = 0;
		} else {
			in_value = in_value - in_value % 100;
		}
		other_value = in_value / 100;
		
		if (p->exploit > 4000000000) {
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
	} else if (type == 2) {
	//exploit -> coins
		if (p->exploit < in_value) {
			return send_header_to_player(p, p->waitcmd, cli_err_no_enough_exploit, 1);
		}
		other_value = in_value * 90;
		if (p->coins > 4000000000) {
			return send_header_to_player(p, p->waitcmd, cli_err_coins_too_much, 1);
		}
	} else {
		ERROR_LOG("%u exchange_coins_exploit type err %u", p->id, type);
		return -1;
	}
	idx = 0;
	pack_h(dbpkgbuf, type, idx);
	if (type == 1) {
		pack_h(dbpkgbuf, in_value, idx);
		pack_h(dbpkgbuf, other_value, idx);
	} else {
		pack_h(dbpkgbuf, other_value, idx);
		pack_h(dbpkgbuf, in_value, idx);
	}
	
	return send_request_to_db(p, p->id, p->role_tm, dbproto_exchange_coins_exploit, dbpkgbuf, idx);
}

int db_exchange_coins_exploit_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	exchange_coins_exploit_t *data = reinterpret_cast<exchange_coins_exploit_t*>(body);
	do_stat_log_exploit_add_reduce(p, data->exploit, channel_string_other);
	do_stat_log_coin_add_reduce(p, data->coins, channel_string_other);
	p->coins = data->coins;
	p->exploit = data->exploit;
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->coins, idx);
	pack(pkgbuf, p->exploit, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int start_offline_mode_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t type = 0;
	uint32_t offline_summon_mode = 1;
	uint32_t mon_tm = 0;
	int idx = 0;
	unpack(body, type, idx);
	uint32_t now = get_now_tv()->tv_sec;
	uint32_t offline_swap_id = 1169;
	
	if (!get_swap_action_times(p, offline_swap_id)) {
		ERROR_LOG("CAN't ONPEN FUMO NO SWAP ID %u", p->id);
		return -1;
	} 

	if (!(p->offline_data)) {
		ERROR_LOG("NO COINS FOR OFFLINE MODE %u", p->id);
		return -1;
	}


	if (type == offline_summon_mode) {
		unpack(body, mon_tm, idx);
		summon_mon_t * p_mon = get_player_summon(p, mon_tm);
		if (!p_mon || !p->offline_data->off_fumo 
				|| p->offline_data->summon_start_tm || p_mon->lv == max_exp_lv ) {
			ERROR_LOG("START OFFLINE SUMMON ERROR %u %u", __LINE__, p->id);
			return -1;
		}
		if (!p_mon || get_player_offline_summon(p) || p_mon->call_flag != summon_in_home) {
			ERROR_LOG(" %u Don't has this summon or has a offline summon ready!", p->id);
			return -1;
		}	
		p_mon->call_flag = summon_in_offline;

		p->offline_data->summon_start_tm = now;
		db_set_offline_info(p, p->lv,
   						    p->offline_data->player_start_tm,
					     	p->offline_data->summon_start_tm,
	                  		p->offline_data->off_coins,
					        p->offline_data->off_fumo,
							0,
							mon_tm);

	} else {
		if (!p->offline_data->off_coins || p->offline_data->player_start_tm || p->lv == max_exp_lv) {
			return -1;
		}
		p->offline_data->player_start_tm = now;

		db_set_offline_info(p, p->lv,
   						    p->offline_data->player_start_tm,
					     	p->offline_data->summon_start_tm,
	                  		p->offline_data->off_coins,
					        p->offline_data->off_fumo);
	}

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, type, idx);
	pack(pkgbuf, now, idx);
	init_cli_proto_head(pkgbuf, p,  p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int  calc_off_exp(player_t* p, uint32_t lv, uint32_t coins, uint32_t time)
{
	uint32_t next_exp = calc_exp(max_exp_lv);
	uint32_t cur_exp = p->exp;//calc_exp(lv);
	uint32_t max_gain_exp = next_exp > cur_exp ? next_exp - cur_exp : 0;
	uint32_t us_time = time / 60;
	uint32_t us_time_bak = us_time;

	if (coins < us_time * lv) {
		us_time = coins / lv;
		us_time_bak = us_time;
	} 
	if (max_gain_exp < us_time * lv) {
		us_time = max_gain_exp / (60 * lv);
	}

	if (max_gain_exp && max_gain_exp < us_time_bak * lv && us_time_bak) {
		return max_gain_exp;
	}
	return us_time * lv;
}

int calc_off_coins(player_t* p, uint32_t lv, uint32_t coins, uint32_t time)
{
	uint32_t next_exp = calc_exp(max_exp_lv);
	uint32_t cur_exp = p->exp;//calc_exp(lv);
	uint32_t max_gain_exp = next_exp > cur_exp ? next_exp - cur_exp : 0;
	uint32_t us_time = time / 60;
	uint32_t us_time_bak = us_time;
	
	if (coins < us_time * lv) {
		us_time = coins / lv;
		us_time_bak = us_time;
	}
	if (max_gain_exp < us_time * lv) {
		us_time = max_gain_exp / (60 * lv);
	}

	if (max_gain_exp && max_gain_exp < us_time_bak * lv && us_time_bak) {
		return max_gain_exp;
	}

	return us_time * lv;
}

int calc_off_summon_exp(player_t* p, summon_mon_t* p_mon, uint32_t mon_type, uint32_t lv, uint32_t fumo, uint32_t time)
{
	summon_info_t * std_mon = get_summon(mon_type);
	uint32_t type = p_mon->mon_type % 10;
	uint32_t max_lv = 0;
	if (type == 5) {
		max_lv = p->lv;
	} else {
		max_lv = std_mon->next_lv > p->lv ? p->lv : std_mon->next_lv ;
	}
	uint32_t next_exp = calc_exp(max_lv, true);
	uint32_t cur_exp = p_mon->exp;//calc_exp(lv, true);
	uint32_t max_gain_exp = next_exp > cur_exp ? next_exp - cur_exp : 0;

	uint32_t us_time = time / 3600;
	uint32_t us_time_bak = us_time;
	uint32_t per_fumo = (lv + 9) / 10;
	

	if (per_fumo * us_time < fumo) {
		if (max_gain_exp < lv * us_time * 210) {
			us_time = max_gain_exp / (210 * lv);
		}
	} else {
		us_time = fumo / per_fumo;
		us_time_bak = us_time;
		if (max_gain_exp < lv * us_time * 210) {
			us_time = max_gain_exp / (210 * lv);
		}
	}

	if (max_gain_exp && (max_gain_exp < lv * us_time_bak * 210) && us_time_bak) {
		return max_gain_exp;
	} 
	return us_time * lv * 210;

}

int calc_off_summon_fumo(player_t* p, summon_mon_t* p_mon, uint32_t mon_type, uint32_t lv, uint32_t fumo, uint32_t time)
{
	summon_info_t * std_mon = get_summon(mon_type);
	uint32_t type = p_mon->mon_type % 10;
	uint32_t max_lv = 0;
	if (type == 5) {
		max_lv = p->lv;
	} else {
		max_lv = std_mon->next_lv > p->lv ? p->lv : std_mon->next_lv ;
	}
	uint32_t next_exp = calc_exp(max_lv, true);
	uint32_t cur_exp = p_mon->exp;
	uint32_t max_gain_exp = next_exp > cur_exp ? next_exp - cur_exp : 0;
	
	uint32_t us_time = time / 3600;
	uint32_t us_time_bak = us_time;
	uint32_t per_fumo = (lv + 9) / 10;

	if (per_fumo * us_time < fumo) {
		if (max_gain_exp < lv * us_time * 210) {
			us_time = max_gain_exp / (210 * lv);
		}

	} else {
		us_time = fumo / per_fumo;
		us_time_bak = us_time;
		if (max_gain_exp < lv * us_time * 210) {
			us_time = max_gain_exp / (210 * lv);
		}
	}
	
	if (max_gain_exp && (max_gain_exp < lv * us_time_bak * 210) && us_time_bak && !us_time) {
		return per_fumo;
	} 

	return us_time * per_fumo;
}

int send_end_player_offline_rsp(player_t* p)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, (uint32_t)(p->lv), idx);
	pack(pkgbuf, p->exp, idx);
	pack(pkgbuf, p->offline_data->off_coins, idx);
	init_cli_proto_head(pkgbuf, p,  p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);

}

int end_player_offline_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{

	if (!(p->offline_data) || !p->offline_data->player_start_tm || !p->offline_data->off_coins) {
		ERROR_LOG("NOT IN OFFLINE MODE USER %u", p->id);
		return -1;
	}
	uint32_t time = get_now_tv()->tv_sec - p->offline_data->player_start_tm;

	if (p->offline_data->player_start_tm && p->offline_data->off_coins) {
        do_stat_log_universal_interface_2(0x09819004, 0, p->id, time);
		
		uint32_t add_player_exp = calc_off_exp(p, p->lv,
			   								   p->offline_data->off_coins,	
			  						           time);

		uint32_t reduce_player_coins = calc_off_coins(p, p->lv,
													  p->offline_data->off_coins,
													  time);
		KDEBUG_LOG(p->id, "END PLAYER OFFLINE\t[%u %u %u %u %u %u]", p->exp, p->coins, 
			p->offline_data->off_coins, p->offline_data->off_fumo, add_player_exp, reduce_player_coins);
		p->offline_data->off_coins -= reduce_player_coins;
		p->offline_data->player_start_tm = 0;
		
		calc_lv_add_exp(add_player_exp, p->exp, p->lv, max_exp_lv, p->role_type, p->id, false);  

		db_set_offline_info( p, p->lv,
			   				p->offline_data->player_start_tm, 
							p->offline_data->summon_start_tm, 
							p->offline_data->off_coins,
							p->offline_data->off_fumo,
						   	add_player_exp,
						   	0);
		return send_end_player_offline_rsp(p);
	} 
	return -1;
}


int send_end_summon_offline_rsp(player_t* p, summon_mon_t * p_mon)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->offline_data->off_fumo, idx);
	pack(pkgbuf, 1, idx);
	pack(pkgbuf, p_mon->mon_tm, idx);
	pack(pkgbuf, (uint32_t)p_mon->lv, idx);
	pack(pkgbuf, p_mon->exp, idx);
	init_cli_proto_head(pkgbuf, p,  p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);

}

int end_summon_offline_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	

	if (!(p->offline_data)) {
		return -1;
	}

	summon_mon_t * p_mon = get_player_offline_summon(p);
	if (!p_mon || !p->offline_data->summon_start_tm ) {
		ERROR_LOG("NO OFFLINE MODE SUMMON %u", p->id);
		return -1;
	}

	uint32_t time = get_now_tv()->tv_sec - p->offline_data->summon_start_tm;
	time = (time + 300) / (60 * 60);
	time *= 60 * 60;
	p_mon->call_flag = summon_in_home;
	
	uint32_t add_mon_exp = calc_off_summon_exp(p, p_mon, p_mon->mon_type,
											   p_mon->lv,
											   p->offline_data->off_fumo,
											   time);

	uint32_t reduce_off_fumo = calc_off_summon_fumo(p, p_mon, p_mon->mon_type,
													p_mon->lv,
													p->offline_data->off_fumo,
													time);

	p->offline_data->off_fumo -= reduce_off_fumo;
	p->offline_data->summon_start_tm = 0;

	summon_info_t * std_mon = get_summon(p_mon->mon_type);
	uint32_t type = p_mon->mon_type % 10;
	uint32_t max_lv = 0;
	if (type == 5) {
		max_lv = p->lv;
	} else {
		max_lv = std_mon->next_lv > p->lv ? p->lv : std_mon->next_lv ;
	}
	uint32_t pre_lv = p_mon->lv;
	calc_lv_add_exp(add_mon_exp, p_mon->exp, p_mon->lv, max_lv, p->role_type, p->id, true);  
	if (pre_lv != p_mon->lv) {
		attr_data * data = new attr_data;
	   	calc_summon_attr(data, std_mon->attr_type, p_mon->attr_per, p_mon->lv);
		set_summon_attr(p_mon, data);
	}

	uint32_t summon_end_flag = 1;

	
//	db_set_fight_summon(p, p_mon->mon_tm, summon_in_home);
	db_set_offline_info(p, p->lv,
		   			    p->offline_data->player_start_tm,
					   	p->offline_data->summon_start_tm,
						p->offline_data->off_coins,
						p->offline_data->off_fumo,
						0,
						p_mon->mon_tm,
						add_mon_exp,
						p_mon->lv > pre_lv ? p_mon->lv - pre_lv : 0,
					   	0,
					   	reduce_off_fumo,
						summon_end_flag);
	return send_end_summon_offline_rsp(p, p_mon);
}

int db_set_offline_info_without_back(player_t * p, uint32_t p_s_tm, uint32_t s_s_tm, uint32_t coins,uint32_t fumo) 
{
	int idx = 0;
	pack_h(dbpkgbuf, p_s_tm, idx);
	pack_h(dbpkgbuf, s_s_tm, idx);
	pack_h(dbpkgbuf, coins, idx);
	pack_h(dbpkgbuf, fumo, idx);
	return send_request_to_db(NULL, p->id, p->role_tm, dbproto_set_offline_info, dbpkgbuf, idx);
}


int db_get_offline_info(player_t * p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_offline_info, 0, 0); 
}

int db_get_offline_info_callback(player_t * p, uint32_t uid, void * body, uint32_t bodylen, uint32_t ret) 
{
	CHECK_DBERR(p, ret);
	db_offline_rsp_t * rsp = reinterpret_cast<db_offline_rsp_t*>(body);
	if (!p->offline_data) {
		p->offline_data = new offline_exp_data_t;
	}

	p->offline_data->player_start_tm = rsp->player_start_tm;
	p->offline_data->summon_start_tm = rsp->summon_start_tm;
	p->offline_data->off_coins = rsp->offline_coins;
	p->offline_data->off_fumo = rsp->offline_fumo;

	summon_mon_t * p_mon = get_player_offline_summon(p);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->offline_coins, idx);
	pack(pkgbuf, rsp->offline_fumo, idx);
	pack(pkgbuf, rsp->player_start_tm, idx);
	pack(pkgbuf, p_mon ? p_mon->mon_type : 0, idx);
	pack(pkgbuf, p_mon ? p_mon->mon_tm : 0, idx);
	pack(pkgbuf, rsp->summon_start_tm, idx);
	init_cli_proto_head(pkgbuf, p,  p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);

}


/**
  * @brief player get player other info
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0
  */
int player_get_other_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = sizeof(cli_proto_t);

	int idx_cnt = idx;
	idx += 4;
	uint32_t count = 0;
	for (uint32_t i = 0; i < max_rank_top_type; i++) {
		if (p->other_info[i]) {
			count++;
			pack(pkgbuf, i, idx);
			pack(pkgbuf, p->other_info[i], idx);
		}
	}

	pack(pkgbuf, count, idx_cnt);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int db_set_player_other_info(player_t *p, uint32_t type, uint32_t value)
{
	int idx = 0;
	pack_h(dbpkgbuf, type, idx);
	pack_h(dbpkgbuf, value, idx);
	return send_request_to_db(NULL, p->id, p->role_tm, dbproto_set_other_info, dbpkgbuf, idx);	
}

uint32_t set_player_other_info(player_t *p, uint32_t type, uint32_t value)
{
	if (type <= max_rank_top_type) {
		p->other_info[type] = value;
		db_set_player_other_info(p, type, value);
		return p->other_info[type];
	}
	return 0;
}

uint32_t get_player_other_info_value(player_t *p, uint32_t type)
{
	if (type <= max_rank_top_type) {
		return p->other_info[type];
	}
	return 0;
}

int db_get_other_info(player_t *p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_other_info, 0, 0);
}

int db_get_other_info_callback(player_t *p, uint32_t uid,  void *body, uint32_t bodylen, uint32_t ret)
{
	
	CHECK_DBERR(p, ret);
	get_other_info_rsp_t * pkg = reinterpret_cast<get_other_info_rsp_t *>(body);
	CHECK_VAL_EQ(bodylen, sizeof(get_other_info_rsp_t)+ (pkg->cnt) * sizeof(get_other_info_rsp_item_t));

	for(uint32_t i = 0; i < pkg->cnt; ++i)
	{
		if (pkg->item[i].type > max_rank_top_type) {
			WARN_LOG("other info > max_rank_top_type %u %u %u", p->id, pkg->item[i].type, pkg->item[i].value);
			continue;
		}
		if (p->other_info[pkg->item[i].type]) {
			WARN_LOG("same other info %u %u %u", p->id, pkg->item[i].type, pkg->item[i].value);
			continue;
		}
		p->other_info[pkg->item[i].type] = pkg->item[i].value;
		TRACE_LOG("%u %u", pkg->item[i].type, p->other_info[pkg->item[i].type]);
	}
	if (p->waitcmd == cli_proto_login) {
		return db_get_friend_list(p);
	}
	return 0;
}

/**
  * @brief player get player other info
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0
  */
int get_player_community_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t uid = 0;
	uint32_t role_tm = 0;
	unpack(body, uid, idx);
	unpack(body, role_tm, idx);

	return send_request_to_db(p, uid, role_tm, dbproto_get_player_community, 0, 0);
}

int db_get_player_community_callback(player_t *p, uint32_t uid,  void *body, uint32_t bodylen, uint32_t ret)
{
	
	CHECK_DBERR(p, ret);
	get_player_community_info_rsp_t * pkg = reinterpret_cast<get_player_community_info_rsp_t *>(body);

	int idx = sizeof(cli_proto_t);

	pack(pkgbuf, pkg->team_id, idx);	
	pack(pkgbuf, pkg->friend_cnt, idx);	
	pack(pkgbuf, pkg->summon_cnt, idx);	
	pack(pkgbuf, pkg->card_cnt, idx);	
	pack(pkgbuf, pkg->achieve_cnt, idx);	
	pack(pkgbuf, pkg->fumo_points, idx);	

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}
void erase_player_friend(player_t* p, uint32_t friend_id)
{
	std::vector<uint32_t>::iterator begin = p->friends_vec->begin();
	std::vector<uint32_t>::iterator end = p->friends_vec->end();
	for (;begin != end; ++begin) {
		if (*begin == friend_id) {
			p->friends_vec->erase(begin);
		}
	}
}
bool is_player_have_friend(player_t* p, uint32_t friend_id)
{
	std::vector<uint32_t>::iterator begin = p->friends_vec->begin();
	std::vector<uint32_t>::iterator end = p->friends_vec->end();
	for (;begin != end; ++begin) {
		if (*begin == friend_id) {
			return true;
		}
	}
	return false;
}

void add_player_friend(player_t* p, uint32_t friend_id)
{
	if (is_player_have_friend(p, friend_id)) {
		return;
	}
	p->friends_vec->push_back(friend_id);
}

int list_other_active_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t active_id = 0;
	unpack(body, active_id, idx);
	uint32_t info = get_player_other_info_value(p, active_id);
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, active_id, idx);
	pack(pkgbuf, info, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int  handle_player_normal_logic(void * owner, void * data)
{
	player_t * p = reinterpret_cast<player_t*>(owner);
//	ERROR_LOG("HANDLE PLAYER NORMAL LOGIC EVENT %u", p->id);

	if (p->p_banner_info->cur_banner) {
		proc_player_red_blue_logic(p);
	}

	ADD_TIMER_EVENT(p,  handle_player_normal_logic, NULL,  get_now_tv()->tv_sec + 1);
	return 0;
}


void send_player_synthesis_attribute_to_db(player_t *p)
{
    int idx = 0;
    uint8_t *pattribute = p->other_info_2[active_player_attribute - 1];
    memset(pattribute, 0x00, 41);
    pack(pattribute, p->strength, idx);
    pack(pattribute, p->agility, idx);
    pack(pattribute, p->body_quality, idx);
    pack(pattribute, p->stamina, idx);
    pack(pattribute, p->hp, idx);
    pack(pattribute, p->mp, idx);
    pack(pattribute, p->atk, idx);
    pack(pattribute, p->crit_rate, idx);
    pack(pattribute, static_cast<uint32_t>(p->def_rate), idx);
    pack(pattribute, static_cast<uint32_t>(p->hit_rate), idx);
    pack(pattribute, static_cast<uint32_t>(p->dodge_rate), idx);

    TRACE_LOG("player attribute -->[%u %u %u %u] [%u %u] [%u %u %f %f %f]", 
        p->strength, p->agility, p->body_quality, p->stamina, p->hp, p->mp,
        p->atk, p->crit_rate, p->def_rate, p->hit_rate, p->dodge_rate);

    if (idx > 40) {
        ERROR_LOG("Data is to long [%u]", idx);
    }
    save_player_active_info_2(p, active_player_attribute, pattribute);
}

uint32_t  calc_and_save_player_attribute(player_t *p, uint32_t guard)
{
    uint32_t _power = 0;
    //calc player synthesis attribute 
    if (guard) {
        _power = get_god_guard_mgr()->calc_group_quality_and_power(p, p->lv, p->add_quality);
    }
    calc_player_attr_ex(p);
    send_player_synthesis_attribute_to_db(p);

    return _power;
}

bool is_player_has_stage_fumo_info(player_t *p, int stage_id, int diff)
{

	std::map<uint32_t, fumo_stage_info_t>::iterator it = p->fumo_stage_map->find(stage_id);
	if (it != p->fumo_stage_map->end()) {
		fumo_stage_info_t * info = &(it->second);
		if (info->grade[diff - 1]) {
			return true;
		}
	}
	return false;
}

void add_player_other_info(uint32_t uid, uint32_t role_tm, uint32_t type, uint32_t add_val)
{
}

