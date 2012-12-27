/**
 *============================================================
 *  @file      cli_proto.hpp
 *  @brief    client protocol definations and package sending functions
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_CLI_PROTO_HPP_
#define KFBTL_CLI_PROTO_HPP_

#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/project/types.h>
#include <libtaomee/project/stat_agent/msglog.h>
}

#include "player.hpp"

/**
  * @brief command id for client protocol
  */
enum cli_cmd_t {
	cli_proto_leave_map 			= 2002,
	cli_proto_chg_app               = 2004,
	cli_proto_talk					= 2102,
	cli_proto_stand					= 2104,
    cli_adjust_coordinate           = 2106,
	
	cli_proto_sync_process			= 2408,
	cli_proto_sync_process_noti		= 2409,

	cli_proto_player_move			= 2451,
	cli_proto_npc_move				= 2452,
	cli_proto_player_attack 		= 2454,
	cli_proto_player_enter_map		= 2455,
	cli_proto_player_pick_item		= 2457,
	cli_proto_summon_attack 		= 2458,
	
	cli_proto_monster_speak 		= 2460,
	cli_proto_summon_pick_item      = 2461,
	cli_proto_summon_follow         = 2471,

	// for battle notifications
//	cli_proto_btl_invitation		= 2501,
//	cli_proto_battle_inv_rsp		= 2502,

	cli_proto_battle_ready		= 2503,
	cli_proto_battle_start		= 2504,
	cli_proto_battle_test_lag	= 2506,
	cli_proto_players_hit		= 2507,
	cli_proto_players_info		= 2508,
	cli_proto_player_attr_chg	= 2509,
	cli_proto_player_hpmp_chg	= 2510,
	cli_proto_monster_item_drop	= 2511,
	cli_proto_stage_score		= 2512,
	cli_proto_duration_state_chg= 2513,
	cli_proto_players_buf_hit	= 2516,
	cli_proto_pvp_score			= 2517,
	cli_proto_battle_count_down	= 2518,
	cli_proto_summon_attr_chg	= 2519,
	cli_proto_players_buf_event_noti = 2521,
	cli_proto_realtm_birth_mon_info	 = 2522,
	cli_proto_skill_effect_noti	 = 2524,
	cli_proto_summon_anger_noti	 = 2525,
	cli_proto_del_map_summon_obj = 2526,
	cli_proto_players_status_info = 2527,
	cli_proto_players_aura_event_noti = 2528,

	cli_proto_contest_skill   = 2550,
	cli_proto_contest_hpmp_change = 2551,
	cli_proto_contest_change_status = 2553,

	cli_proto_notify_player_enter_state_in_room = 2721,
	cli_proto_notify_player_leave_state_in_room = 2723,
	cli_proto_notify_player_state_in_room 	  	= 2722,
	cli_proto_notify_team_leader_change	 	  	= 2724,
	cli_proto_notify_team_member_enter_map		= 2725,
	cli_proto_notify_launche_skill_fail         = 2726,
	cli_proto_notify_speed_change               = 2727,
	/*! for roll item in battle */
	cli_proto_roll_item            = 2729,
	cli_proto_roll_item_list       = 2730,
	
	/*! check cheat */
	cli_proto_check_cheat          = 2735,

	/*! battle section change */
	cli_proto_btl_section_change   = 2740,
	//for test
	cli_proto_get_player_attr   = 9001,
	cli_proto_get_mon_attr  	= 9002,

	cli_proto_probe				= 30000,
	cli_get_version				= 30001,
};

/**
  * @brief command id for online-battle_server communication
  */
enum btl_cmd_t {
	/*! starting command id */
	btl_cmd_start			= 10001,

	btl_fight_mode_start	= 10001,

	/*! 按等级自动匹配对战 */
	btl_lv_matching_battle	= 10001,
	/*! pve */
	btl_pve_battle			= 10002,

	btl_create_battle_team	= 10003,
	btl_join_battle_team	= 10004,
	btl_hot_join_battle_team = 10005,

	btl_pvp_mathing_battle  = 10006,
	btl_pvp_invite_battle   = 10007,
	btl_pve_invite_battle   = 10008,
	btl_pve_mating_battle   = 10009,
	
	btl_contest_watch_yazhu      = 10041,


	btl_initiate_battle		= 10021,
	btl_rsp_battle_invit	= 10022,
	btl_player_btl_restart	= 10023,
	btl_tmp_team_pvp		= 10024,
	btl_contest_watch_in    = 10041,

	btl_fight_mode_end		= 10050,

	btl_player_ready		= 10051,
	btl_player_move			= 10052,
	btl_player_stop			= 10053,
	btl_player_rsp_lag		= 10054,
	btl_player_jump 		= 10055,
	btl_player_pick_item	= 10056,
	btl_player_attack		= 10057,
	btl_player_enter_map	= 10058,
	btl_player_use_item		= 10059,

	btl_escape_battle		= 10061,
	btl_cancel_battle		= 10062,
	btl_noti_load_percent	= 10063,
	btl_noti_player_lvup	= 10064,
	btl_player_open_treasure_box	= 10065,
	btl_summon_unique_skill	= 10066,

	btl_player_clothes_duration	= 10071,
	btl_sync_process						= 10072,

	btl_player_roll_item    	= 10084,
	btl_player_get_roll_item 	= 10085,	

	btl_contest_watch_out    = 10086,
	
	btl_ready_battle_team						= 10091,
	btl_start_battle_team						= 10092,
	btl_leave_battle_team						= 10093,
	btl_cancel_ready_battle_team      = 10095,	
	btl_set_btl_team_hot_join   	  = 10096,
	btl_kick_user_from_room			  = 10097,
	btl_nodi_user_be_kicked			  = 10098,

	btl_contest_watch_over      = 10112,

	btl_summon_ai_change        = 10113,
	btl_player_talk             = 10114,
	btl_player_detect_player_need_kick	 = 10115,


	btl_battle_over			= 10201,
	btl_noti_online_all_player_joined_ready_to_start			= 10202,
	

	btl_transmit_only		= 12001,

	//for test

	btl_get_player_attr  	= 12990,
	btl_get_mon_attr		= 12991,


	//for mechanism
	btl_change_mechanism   = 12992,
	btl_change_mechanism_end = 12993,

	//for extra item drop
	btl_extra_item_drop_proc = 12994,

	//for first kill boss
	btl_first_kill_boss   = 12995,
	btl_auto_del_item          = 12996,
	btl_open_box_times         = 12997,

	btl_kill_monster      = 12998,

    btl_change_game_value = 12999,
	/*! ending command id */
	btl_cmd_end,

	/*! max number of commands supported */
	btl_cmd_max				= 13000
};

/**
  * @brief severe battle server error
  */
enum btl_err_t {
	btl_err_player_not_found	= 1001,
	btl_err_no_battle_grp		= 1002,

	btl_err_max
};

/**
  * @brief errno for client protocol
  */
enum cli_err_t {
	// errnos from 10001 to 50000 are reserved for common errors
	cli_err_system_error	= 10002,
	cli_err_system_busy		= 10003,
	cli_err_multi_login		= 10004,
	cli_err_user_offline	= 10006,

	// error no for battle
	cli_err_btl_same_id				= 11001,
	cli_err_btl_not_exist			= 11002,
	cli_err_cannot_cancel_battle	= 11003,
	cli_err_battle_ended			= 11004,
	cli_err_battle_not_started		= 11005,
	cli_err_battle_system_err		= 11006,
	cli_err_battle_no_such_stage	= 11007,
	cli_err_battle_lv_not_match		= 11008,
	cli_err_battle_map_not_exist	= 11009,
	cli_err_battle_monster_exist	= 11010,
	cli_err_matich_overtime 		= 11011,
	cli_err_battle_cannot_open_box	= 11012,
	cli_err_battle_room_not_exist	= 11013,
	cli_err_battle_room_full		= 11014,
	cli_err_battle_no_room			= 11015,
	cli_err_battle_pvp_room_not_exist	= 11017,

	cli_err_player_not_all_ready	= 11020,

	cli_err_cool_time					= 12001,
	cli_err_cannot_action				= 12002,
	cli_err_no_item						= 12003,
	cli_err_cannot_pick_item			= 12004,
	cli_err_no_enough_anger_value		= 12005,
	cli_err_roll_item_fail              = 12006,
	cli_err_can_not_move                = 12007,
	/*! we return errno from dbproxy by plusing 100000 to dberr (100000 + dberr) */
	cli_err_base_dberr		= 100000,
};

/**
  * @brief some constants
  */
enum {
	/*! max acceptable length in bytes for each client package */
	btlproto_max_len	= 8 * 1024
};

#pragma pack(1)

/**
  * @brief battle protocol type
  */
struct btl_proto_t {
	/*! package length */
	uint32_t	len;
	/*! battle group id */
	uint32_t	seq;
	/*! command id */
	uint16_t	cmd;
	/*! errno */
	uint32_t	ret;
	/*! user id */
	userid_t	id;
	/*! package body */
	uint8_t		body[];
};

/**
  * @brief client protocol type
  */
struct cli_proto_t {
	/*! package length */
	uint32_t	len;
	/*! protocol command id */
	uint16_t	cmd;
	/*! user id */
	userid_t	id;
	/*! used as an auto-increment sequence number for checking GameCheater */
	uint32_t	seqno;
	/*! error number: 0 for OK */
	uint32_t	ret;
	/*! body of the package */
	uint8_t		body[];
};

/**
  * @brief cache a package
  */
struct cached_pkg_t {
	uint16_t	len;
	uint8_t		pkg[];
};

#pragma pack()

/*! for packing protocol data and send to client. 2M */
extern uint8_t pkgbuf[1 << 21];

/**
  * @brief send a package to player p
  * @param p player to send a package to
  * @param pkg package to send
  * @param len length of the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_to_player(Player* p, void* pkgbuf, uint32_t len, int completed);
/**
  * @brief send a package header to player p
  * @param p player to send a package to
  * @param cmd command id of the package
  * @param err errno to be set into the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_header_to_player(Player* p, uint32_t cmd, uint32_t err, int completed);
/**
  * @brief send a package header to online server
  * @param uid user who will receive the package
  * @param fdsess
  * @param cmd
  * @param seqno
  * @param err errno to be set into the package
  * @return 0 on success, -1 on error
  */
int send_header_to_online(userid_t uid, fdsession_t* fdsess, uint32_t cmd, uint32_t seqno, uint32_t err);

//--------------------------------------------------------------------

/**
  * @brief init handlers to handle protocol packages from client
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_cli_proto_handles();
/**
  * @brief dispatches protocol packages from client
  * @param data package from client
  * @param fdsess fd session
  * @param first_tm true if we are processing a package from a client,
  *              false if from the pending-processing queue
  * @return value that was returned by the corresponding protocol handler
  */
int dispatch(void* data, fdsession_t* fdsess, bool first_tm = true);
/**
  * @brief scan the 'awaiting_playerlist' and process cached packages of those players whose waitcmd is 0
  */
void proc_cached_pkgs();

//-----------------------------------------------------------
// inline function definations
//-----------------------------------------------------------
/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  * @param ret return code
  */
inline void init_btl_proto_head_full(void* header, uint32_t cmd, uint32_t len, uint32_t ret)
{
	btl_proto_t* p = reinterpret_cast<btl_proto_t*>(header);

	p->len = len;
	p->cmd = cmd;
	p->ret = ret;
}

/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  */
inline void init_btl_proto_head(void* header, uint32_t cmd, uint32_t len)
{
	init_btl_proto_head_full(header, cmd, len, 0);
}

/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  */
inline void init_cli_proto_head_full(void* header, userid_t id, uint16_t cmd, uint32_t ret, uint32_t len)
{
	cli_proto_t* p = reinterpret_cast<cli_proto_t*>(header);

	p->id  = taomee::bswap(id);
	p->len = taomee::bswap(len);
	p->cmd = taomee::bswap(cmd);
	p->ret = taomee::bswap(ret);
}

/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  */
inline void init_cli_proto_head(void* header, uint16_t cmd, uint32_t len)
{
	init_cli_proto_head_full(header, 0, cmd, 0, len);
}

//--------------------
/**
 * @brief as first interface of statistics
 */
inline void do_stat_item_log(uint32_t cmdid, uint32_t id, uint32_t type, uint32_t num)
{
    uint32_t buf[1] = { num };
    uint32_t itype = ((type - 1) << 12);
    msglog(statistic_logfile, cmdid + itype + id, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log cmid:%x id:%u type:[%u %x] num:%u", cmdid, id, type, itype, num);
}

/**
 * @brief as second interface of statistics
 */
inline void do_stat_item_log_2(uint32_t cmdid, uint32_t id, uint32_t type, uint32_t num)
{
    uint32_t buf[1] = { num };
    msglog(statistic_logfile, cmdid + type + id, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log cmid:%x id:%u type:%u num:%u", cmdid, id, type, num);
}
/**
 * @brief as third interface of statistics
 * NOTE: for stage and room
 */
inline void do_stat_item_log_3(uint32_t cmdid, uint32_t id1, uint32_t id2, 
    uint32_t type, uint32_t* buf, uint32_t len)
{
    uint32_t itype = ((type - 1) << 12);
    msglog(statistic_logfile, cmdid + itype + id1 +id2, get_now_tv()->tv_sec, buf, len);
    TRACE_LOG("stat log cmid:%x id:[%u, %u] type:[%u %x] ", cmdid, id1,id2,type,itype);
}
/**
 * @brief as fourth interface of statistics
 * NOTE: for self define message
 */
inline void do_stat_item_log_4(uint32_t cmdid, uint32_t id, uint32_t type, uint32_t buf[], int len)
{
    msglog(statistic_logfile, cmdid + type + id , get_now_tv()->tv_sec, buf, len);
}

/**
 * @brief as fourth interface of statistics
 * NOTE: for self define message
 */
inline void do_stat_item_log_5(uint32_t cmdid, uint32_t id, uint32_t data1, uint32_t data2)
{
	uint32_t buf[2] = { data1, data2 };
    msglog(statistic_logfile, cmdid + id , get_now_tv()->tv_sec, buf, sizeof(buf));
}
/**
 * @brief as fourth interface of statistics
 * NOTE: for self define message
 */
inline void do_stat_item_finish_931_stage(uint32_t cmdid, uint32_t role_type, uint32_t finish_tm)
{
	uint32_t buf[3] = { 0, 1, finish_tm };
    msglog(statistic_logfile,  cmdid + role_type, get_now_tv()->tv_sec, buf, sizeof(buf));
}

/**
 * @brief as second interface of statistics
 */
inline void do_stat_enter_leave_battle(uint32_t stage_id, uint32_t diff, uint32_t enter_flg, uint32_t leave_flg, uint32_t tm)
{
	if (diff > 10) {
		diff = 1;
	}

	uint32_t base_msgid = 0x09800000;
	uint32_t msgid = base_msgid + 0x00001000 * diff + stage_id;
    uint32_t buf[3] = { enter_flg, leave_flg, tm };
	
    msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, buf, sizeof(buf));
}

/**
 * @brief as second interface of statistics
 */
inline void do_stat_team_enter_leave_battle(uint32_t stage_id, uint32_t diff, uint32_t enter_flg, uint32_t leave_flg, uint32_t tm)
{
	if (diff > 10) {
		diff = 1;
	}

	uint32_t base_msgid = 0x09806000;
	uint32_t msgid = base_msgid + 0x00001000 * diff + stage_id;
    uint32_t buf[3] = { enter_flg, leave_flg, tm };
	
    msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, buf, sizeof(buf));
}


#endif // KF_CLI_PROTO_HPP_

