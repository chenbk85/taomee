/**
 *============================================================
 *  @file      battle.hpp
 *  @brief    battle related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_BATTLE_HPP_
#define KF_BATTLE_HPP_

#include <libtaomee++/bitmanip/bitmanip.hpp>

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/log.h>

#include <libtaomee/project/constants.h>
#include <libtaomee/project/types.h>

#include <async_serv/dll.h>
}

#include "fwd_decl.hpp"
#include "cli_proto.hpp"
#include "player.hpp"
#include "battle_switch.hpp"

#define MAX_ARENA_NUM 3
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
	/*! team pve */
	btl_create_btl_team		= 10003,
	btl_join_btl_team		= 10004,
	btl_hot_join_btl_team	= 10005,



	btl_initiate_battle		= 10021,
	btl_rsp_battle_invit	= 10022,
	btl_player_btl_restart	= 10023,

	btl_player_tmp_team_pvp  = 10024,

	btl_contest_watch_yazhu    = 10041,


	btl_fight_mode_end		= 10050,

	btl_player_ready		= 10051,
	btl_player_move			= 10052,
	btl_player_stop			= 10053,
	btl_player_rsp_lag		= 10054,
	btl_player_jump			= 10055,
	btl_player_pick_item	= 10056,
	btl_player_attack		= 10057,
	btl_player_enter_map	= 10058,
	btl_player_use_item		= 10059,

	btl_escape_battle		= 10061,
	btl_cancel_battle		= 10062,
	btl_noti_load_percent	= 10063,
	btl_noti_player_lvup	= 10064,
	btl_player_open_treasure_box	= 10065,
	btl_summon_unique_skill = 10066,

	btl_player_clothes_duration = 10071,
	btl_sync_process		= 10072,

	btl_player_roll_item     = 10084,
    btl_player_get_roll_item = 10085,	
	btl_contest_cancel_yazhu = 10086,

	btl_ready_btl_team			= 10091,
	btl_start_btl_team			= 10092,
	btl_leave_btl_team			= 10093,
	btl_cancel_ready_btl_team	= 10095,
	btl_set_btl_team_hot_join	= 10096,
	btl_kick_user_from_room		= 10097,
	btl_nodi_user_be_kicked		= 10098,

	btl_contest_watch_over     = 10112,
	btl_summon_ai_change       = 10113,
	btl_player_talk            = 10114,
	btl_player_detect_player_need_kick   = 10115,

	btl_battle_over			= 10201,
	btl_noti_online_all_player_joined_ready_to_start			= 10202,

	btl_transmit_only		= 12001,

	
	//for test
	btl_get_player_attr  	= 12990,
	btl_get_mon_attr		= 12991,
	btl_change_mechanism    = 12992,
	btl_get_mechanism_drop  = 12993,

	btl_extra_item_drop_proc = 12994,


	//for first kill boss
	btl_first_kill_boss   = 12995,
    btl_change_game_value = 12999,
	btl_auto_del_item          = 12996,
	btl_open_box_times         = 12997,

	//for task logic check
	btl_kill_monster = 12998,

	/*! ending command id */
	btl_cmd_end,

	/*! max number of commands supported */
	btl_cmd_max				= 13000
};

/**
  * @brief battle error type
  */
enum btl_err_t {
	btl_err_player_not_found	= 1001,
	btl_err_no_battle_grp		= 1002,

	btl_err_max
};

enum {
	/*! max acceptable length in bytes for each package from/to battle server */
	btlproto_max_len	= 8 * 1024,
	/*! max number of battle servers */
	max_battle_svr_num	= 100,

	/*! indicates that a battle is started */
	battle_started		= 1
};

enum {
	pvp_btl_type_radom	= 0,
	pvp_btl_type_invite = 1, //邀请PVP方发起的战斗
	pvp_btl_type_foot   = 2,
	pvp_btl_type_been_invite = 3,//被邀请方响应的战斗
};
/**
  * @brief battle mode
  */
enum battle_mode_t {
	/*! 2 players with the same level will be matched and start a battle */
	btl_mode_lv_matching	= 1,
	/*! n players vs environment */
	btl_mode_pve			= 2,

	btl_mode_watch          = 3,

	btl_mode_temp_team_pvp  = 4,
};


/**
  * @brief battle difficulty
  */
enum pvp_lv_t {
	pvp_lv_0 = 0,
	pvp_lv_1 = 1,
	pvp_lv_2 = 2,
    pvp_monster_ = 3,
    pvp_monster_game = 4,
    pvp_monster_practice = 5,
	pvp_contest_advance = 6,
	pvp_contest_advance_practive = 7,
    pvp_contest_final = 8,

	pvp_16_contest = 9,

	pvp_red_blue_1 = 10,
	pvp_red_blue_2 = 11,
	pvp_red_blue_3 = 12,
	pvp_team_score = 13,

	pvp_summon_mode = 14,
	pvp_ghost_game  = 15,
    pvp_summon_mode_2 = 16,
    
	pvp_eve_of_16_fight_1 = 17,
	pvp_eve_of_16_fight_2 = 18,
	pvp_eve_of_16_fight_3 = 19,
	pvp_eve_of_16_fight_4 = 20,

	pvp_team_contest  = 21,
	
	pvp_temporary_team  = 22,

	pvp_dragon_ship = 23,
    
	pvp_lv_max,
};

/**
  * @brief reasons that causes a battle to end
  */
enum end_battle_reason_t {
	/*! a normal ending */
	end_btl_normal	= 0,
	/*! some players offline */
	end_btl_offline	= 1,
	/*! some players timeout */
	end_btl_timeout	= 2,
	/*! a draw game */
	end_btl_draw	= 3,
	/*! system error */
	end_btl_syserr	= 4,
	/*! some players escaped */
	end_btl_escape	= 5,
	/*! some players use boost plugin */
	end_boost_plugin = 6,
	/*! player stage lose*/
	end_stage_lose = 7,
};

/*! Footprinting pvp battle */
struct pvp_pos{
    uint8_t left_pos;
    uint8_t right_pos;
    uint32_t room_id;
    uint32_t left_id;
    uint32_t right_id;
};


struct player_buf_skill_rsp_t {
	uint32_t	buf_type;
	uint32_t	buf_duration;
	uint32_t	per_trim;
	uint32_t 	mutex_type;
};

#pragma pack(1)

/**
  * @brief protocol type for battle server
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
  * @brief package from online to battle server
  */
struct btl_team_attr_t {
	uint32_t limit_num;
	uint32_t refuse_hot_join;
//	char	 nick[max_nick_size];
};

struct clothes_base_t {
    uint32_t    id;
    uint32_t    gettime;
    uint32_t    timelag;
    uint32_t    lv;
};

struct join_btl_summon_rsp_item_t {
	uint32_t mon_tm;
	uint32_t id;
	uint32_t role_type;
	char nick[max_nick_size];
	uint16_t lv;
	uint16_t fight_value;
	uint32_t exp;
	uint16_t anger_value;
};
struct join_btl_rsp_item_t {
	uint32_t owner_id;
	uint32_t id;
	uint32_t role_tm;
	uint32_t role_type;
	uint32_t power_user;
	uint32_t show_state;
	uint32_t vip;
	uint32_t vip_lv;
	uint8_t team;
	char nick[max_nick_size];
	uint32_t lv;
	uint32_t maxhp;
	uint32_t hp;
	uint32_t maxmp;
	uint32_t mp;
	uint32_t exp;
	uint32_t honor;
	uint32_t x;
	uint32_t y;
	uint8_t dir;
	uint32_t speed;
	uint32_t clothes_cnt;
    clothes_base_t clothes[];
};

#pragma pack()

enum {
	battle_type_single = 0,
	battle_type_multi  = 1,
};
/**
  * @brief battle group
  */
struct battle_grp_t {
	/*! battle group id */
	uint32_t	id;
	/*! flag: bit 1 - battle started */
	uint8_t		flag;
	/*! battle mode - 1: 1 on 1; 2: n on m */
	uint32_t	battle_mode;
	/*! battle type: 0 sigle battle, 1 multibattle*/
	uint8_t		btl_type;
	/*! stage id */
	uint32_t	stage_id;
	/*! battle svr fd index */
	uint32_t	fd_idx;
    /*! record footprinting */
    uint8_t     foot_pos;
    /*! record pvp battle type */
    uint32_t    pvp_btl_type;
	/*killed monster in this battle*/
	uint32_t    monster_killed_cnt;

	uint32_t difficulty; // 1,2,3,4,5,6
	uint32_t enter_type; //0: free 1:other
    int     end_reason;
};

/**
  * @brief specialstages
  */
struct specialstages_t {
	/*! stage id */
	uint32_t	stage_id;
	/*! task id */
	uint32_t	task_id;
};

struct time_limit_cell_t {
	uint32_t from_time;
	uint32_t to_time;
};

struct week_limit_t {
	time_limit_cell_t limit_cell[10 + 1];
};

struct date_limit_t {
	uint32_t from_date;
	uint32_t to_date;
	time_limit_cell_t limit_cell[10 + 1];
};

/**
  * @brief stage activity limit
  */
struct stage_activity_limit_t {
	/*! stage id */
	uint32_t	stage_id;
	/*! limit type : week or date*/
	uint8_t		type;
	/*! week limit */
	week_limit_t week_limit[7];
	/*! date limit */
	date_limit_t date_limit[10];
};

//
/*! hold number of battle servers */
extern int				battle_svr_cnt;
//------------------------------------------------------------
/**
 * @ brief for pvp footprint
 */
int enter_pvp_footprint(player_t* p, int pos, uint32_t foot_pos);
/**
 * @ brief for pvp footprint
 */
int leave_pvp_footprint(player_t* p, bool come_back);

#if 0
/**
  * @brief initiate a battle
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int initiate_battle_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief respond a battle invitation
 * @return 0 on success, -1 on error
 */
int rsp_battle_invit_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
#endif
/**
 * @brief Broadcast user leave pvp point
 */
int send_leave_pvp_point_rsp(player_t* p);

int send_tmp_team_hot_join_pvp_room_2_btl(player_t* p, uint32_t room_id, uint32_t pvp_lv, uint32_t btl_mode);
/**
 * @brief cancel a battle
 * @return 0 on success, -1 on error
 */
int cancel_battle_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief Attend a random battle, to battle with a randomly chosen player
  */
int lv_matching_btl_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief Attend a random battle, to battle with a randomly chosen player team
  */
int tmp_team_pvp_start_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief Attend a pve battle
  */
int pve_btl_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

//------------team battle--------------
/**
  * @brief create a battle team
  */
int create_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief join a battle team
  */
int join_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief hot join a battle team
  */
int hot_join_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


/**
  * @brief set ready in a battle team
  */
int ready_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief cancel ready in a battle team
  */
int cancel_ready_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief set a battle team hot join flag
  */
int set_btl_team_hot_join_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief the team leader kick user from game room
  */
int kick_user_from_room_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief start a battle team
  */
int start_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief leave a battle team
  */
int leave_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief invite friend to btl team
  */
int invite_friend_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief reply friend to btl team
  */
int reply_friend_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief restart a pve battle
  */
int pve_btl_restart_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief restart a jump battle
  */
int pve_btl_stage_jump_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int sync_process_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief a player informs server that he/she is ready to fight
  * @return 0 on success, -1 on error
  */
int player_ready_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player move in a battle
 * @return 0 on success, -1 on error
 */
int player_move_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief response to a lag test package
 */
int player_rsp_lag_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief player attack in a battle
 */
int player_attack_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief summon attack unique skill
 */
int summon_unique_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player attack in a battle
 */
int player_use_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player pick a item in a battle
 */
int player_pick_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player enters a map in battle
 */
int player_enter_map_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player leaves a battle
 */
int player_leave_btl_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player leaves a battle
 */
//int player_revive_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player open treasure box
 */
int player_open_treasure_box_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/** 
 * @brief 玩家尝试更改关卡机关状态
 */
int player_change_mechanism_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief 玩家请求机关掉落 
 * 
 */
int player_get_mechanism_drop_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

int get_challenge_status_cmd(player_t * p, uint8_t * body, uint32_t bodylen);




//for test
int get_player_attr_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int get_mon_attr_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int roll_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


//for summon ai

int summon_ai_change_cmd(player_t *p, uint8_t * body, uint32_t bodylen);


/**
  * @param p the player who launches the request to battle server
  * @param btlpkgbuf body of the request
  * @param len length of btlpkgbuf
  * @param ptr_bs_fd pointer to battle svr fd
  * @return 0 on success, -1 on error
  */
int  send_to_battle_svr(player_t* p, const void* btlpkgbuf, uint32_t len, int index);
/**
  * @brief init handles to handle protocol packages from battle server
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_btl_proto_handles();
/**
  * @brief uniform handler to process packages from battle server
  * @param data package data
  * @param len package length
  * @param bs_fd battle server fd
  */
void handle_battle_svr_return(btl_proto_t* data, uint32_t len, int bs_fd);

/**
  * @brief callback for create a battle team
  */
int btlsvr_create_btl_team_callback(player_t* p, btl_proto_t* bpkg);

int btlsvr_join_btl_team_callback(player_t* p, btl_proto_t* bpkg);

int btlsvr_ready_btl_team_callback(player_t* p, btl_proto_t* bpkg);

int btlsvr_cancel_ready_btl_team_callback(player_t* p, btl_proto_t* bpkg);

int btlsvr_start_btl_team_callback(player_t* p, btl_proto_t* bpkg);

int btlsvr_leave_btl_team_callback(player_t* p, btl_proto_t* bpkg);

int btlsvr_player_tmp_team_pvp_callback(player_t* p, btl_proto_t* bpkg);


int btlsw_auto_join_room_callback(player_t* p, battle_switch_proto_t* bpkg, uint32_t len);


int btlsw_auto_join_pvp_room_callback(player_t* p, battle_switch_proto_t* bpkg, uint32_t len);

int btlsw_get_contest_list_callback(player_t * p, battle_switch_proto_t * bpkg, uint32_t len);

int btlsw_auto_join_contest_group_callback(player_t *p, 
										   battle_switch_proto_t *pkg,
	   									   uint32_t len);	
							

int btlsw_contest_group_auto_msg_callback(player_t * p, 
										  battle_switch_proto_t * pkg,
										  uint32_t len);

int btlsw_list_contest_group_callback(player_t * p,
										   battle_switch_proto_t * pkg,
										   uint32_t len);

int btlsw_leave_contest_group_callback(player_t * p,
							           battle_switch_proto_t * pkg,
									   uint32_t len);

int btlsw_contest_group_enter_room_callback(player_t * p,
	   							  	        battle_switch_proto_t * data, 
											uint32_t len);


int btlsw_contest_btl_callback(player_t * p,
	   						   battle_switch_proto_t * data, 
							   uint32_t len);

int btlsw_contest_all_over_callback(player_t * p,
							        battle_switch_proto_t * data,
									uint32_t len);

int btlsw_player_win_contest_callback(player_t * p,
									 battle_switch_proto_t * data,
									 uint32_t len);

int btlsw_contest_guess_champion_callback(player_t * p,
										  battle_switch_proto_t * data,
		                                  uint32_t len);



int btlsw_list_all_contest_player_callback(player_t * p, 
										   battle_switch_proto_t * data,
										   uint32_t len);

int btlsw_list_passed_contest_info_callback(player_t * p,
										    battle_switch_proto_t * data, 
											uint32_t len);


int btlsw_team_contest_auto_join_callback(player_t *,
	   									  battle_switch_proto_t * data, 
										  uint32_t len);

int btlsw_team_contest_get_server_id_callback(player_t *,
	   									  battle_switch_proto_t * data, 
										  uint32_t len);


/**
  * @brief callback for create a battle team
  */
int btlsvr_nodi_user_be_kicked_callback(player_t* p, btl_proto_t* bpkg);

/**
  * @brief callback for the team leader kick user from game room
  */
int btlsvr_kick_user_from_room_callback(player_t* p, btl_proto_t* bpkg);

/**
  * @brief callback fuction for setting a battle team hot join flag 
  */
int btlsvr_set_btl_team_hot_join_callback(player_t* p, btl_proto_t* pkg);

int btlsvr_change_mechanism_callback(player_t *p, btl_proto_t * pkg);

int btlsvr_first_kill_boss_callback(player_t *p, btl_proto_t * pkg);



//-----------------------------------------------------------------------

/**
 * @brief load battle servers' config from battle_server.xml file
 * @param cur the pointer to xml root node
 * @return 0 on success, -1 on error
 */
int  load_battle_servers(xmlNodePtr cur);
/**
 * @brief reload battle svr config 
 */
void reload_battle_svr_config();
/**
  * @brief get index of a battle server identified by the given 'fd'
  * @param fd fd of a battle server
  * @return index of a battle server, or -1 if battle server identified by 'fd' is not found
  */
int  get_btlsvr_fd_idx(int fd);

//-----------------------------------------------------------------------

/*! for packing protocol data and send to battle server */
extern uint8_t btlpkgbuf[btlproto_max_len];
/*! hold fds to battle servers */
extern int battle_fds[max_battle_svr_num];

//-----------------------------------------------------------------------
/**
  * @brief determine if the given command id is valid for starting a battle
  * @param cmd command id
  */
inline bool is_valid_fight_cmd(uint32_t cmd)
{
	return ((cmd >= btl_fight_mode_start) && (cmd <= btl_fight_mode_end));
}

/**
  * @brief determine if a battle is started
  * @param grp battle group
  * @return true if battle is started, otherwise false
  */
inline bool is_battle_started(const battle_grp_t* grp)
{
	return taomee::test_bit_on(grp->flag, battle_started);
}

/**
  * @brief Allocate a battle group
  * @return pointer to the newly allocated battle group
  */
battle_grp_t* alloc_battle_grp();

/**
  * @brief Free a battle group. If it's a vs_wild mode battle, grp->players[1] will be free too
  * @param grp battle group to free
  */
void free_battle_grp(battle_grp_t* grp, player_t* p);

/**
  * @brief init header of battle server protocol
  * @param p the player who is going to send a pkg to battle server
  * @param id
  * @param header header to be initialized
  * @param len length of the whole protocol package
  * @param cmd client command id  
  */
inline void init_btl_proto_head(const player_t* p, userid_t id, void* header, int len, uint32_t cmd)
{
	btl_proto_t* pkg = reinterpret_cast<btl_proto_t*>(header);

	pkg->len = len;
	pkg->seq = ((p && p->battle_grp) ? p->battle_grp->id : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
}

/**
  * @brief tell battle server that 'p' will cancel a battle
  * @param reason 0: player offline, 1: player requires to leave a battle
  * @return 0 on success, -1 on error
  */
inline int btlsvr_cancel_battle(player_t* p, uint8_t reason = 0)
{
	if (p->watch_info) {
		p->waitcmd = 0;
		return 0;
	}
	int idx = sizeof(btl_proto_t);
	taomee::pack_h(btlpkgbuf, reason, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_cancel_battle);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

/**
  * @brief tell battle server that 'p' will stop moving
  * @return 0 on success, -1 on error
  */
inline int btlsvr_player_stop(player_t* p, uint32_t old_x, uint32_t old_y,  uint32_t x, uint32_t y, uint8_t dir)
{
	int idx = sizeof(btl_proto_t);
	taomee::pack_h(btlpkgbuf, old_x, idx);
	taomee::pack_h(btlpkgbuf, old_y, idx);
	taomee::pack_h(btlpkgbuf, x, idx);
	taomee::pack_h(btlpkgbuf, y, idx);
	taomee::pack_h(btlpkgbuf, dir, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_stop);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

/**
  * @brief tell battle server that 'p' will jump
  * @return 0 on success, -1 on error
  */
inline int btlsvr_player_jump(player_t* p, uint32_t old_x, uint32_t old_y, uint32_t x, uint32_t y)
{
	int idx = sizeof(btl_proto_t);
	taomee::pack_h(btlpkgbuf, old_x, idx);
	taomee::pack_h(btlpkgbuf, old_y, idx);
	taomee::pack_h(btlpkgbuf, x, idx);
	taomee::pack_h(btlpkgbuf, y, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_jump);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

//------------------------------------------------------------

/**
  * @brief judge if the given 'btl_mode' is valid
  * @param btl_mode battle mode
  * @return true if the given battle mode is true, otherwise false
  */
inline bool is_valid_btl_mode(uint32_t btl_mode)
{
	return ((btl_mode >= btl_mode_lv_matching) && (btl_mode <= btl_mode_pve));
}

/**
  * @brief tell battle server that 'p' will jump
  * @return 0 on success, -1 on error
  */
inline int btlsvr_player_use_item(player_t* p, uint32_t itmid)
{
	int idx = sizeof(btl_proto_t);
	taomee::pack_h(btlpkgbuf, itmid, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_use_item);
	TRACE_LOG("%u ", itmid);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_kill_monster_callback(player_t * p, btl_proto_t * pkg);

int btlsvr_player_contest_yazhu(player_t *p);

int btlsvr_contest_cancel_yazhu(player_t *p, uint32_t cancel_reason);

int btlsvr_player_talk(player_t *p, uint8_t * msg, uint32_t msg_len, uint32_t recv_id);

/**
  * @brief tell battle server that 'p' will jump
  * @return 0 on success, -1 on error
  */
inline int btlsvr_player_pick_item(player_t* p, uint16_t cmd, uint32_t unique_id, uint32_t max_bag_grid_count, uint32_t mon_tm = 0)
{
	int idx = sizeof(btl_proto_t);
	taomee::pack_h(btlpkgbuf, unique_id, idx);
	taomee::pack_h(btlpkgbuf, max_bag_grid_count, idx);
	taomee::pack_h(btlpkgbuf, mon_tm, idx);

	init_btl_proto_head(p, p->id, btlpkgbuf, idx, cmd);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

inline bool is_stage_pvp(uint32_t stage_id)
{
	return  (stage_id == 901) || (stage_id == 920) || (stage_id == 907);
}

inline bool is_stage_tower(uint32_t stageid)
{
	return (stageid > 800 && stageid < 900);
}

inline uint32_t get_default_btl_fd_idx(userid_t uid)
{
	return (uid % battle_svr_cnt);
}

inline uint32_t get_globe_room_id(uint32_t btl_id, uint32_t room_id)
{
	return ((btl_id << 16 & 0xffff0000) | room_id);
}

inline uint32_t get_local_room_id(uint32_t globe_id)
{
	return (globe_id & 0x0000ffff);
}

inline uint32_t get_btl_id(uint32_t globe_id)
{
	return (globe_id >> 16 & 0x0000ffff);
}


inline bool is_fd_idx_valid(int fd_idx)
{
	if (fd_idx == -1 || fd_idx >= battle_svr_cnt) {
		return false;
	}
	return true;
}

inline uint16_t get_pvp_stage_id(uint32_t pvp_lv)
{
	if (pvp_lv == pvp_lv_2) {
		return 907;
	} else if (pvp_lv == pvp_lv_1) {
		return 901;
    } else if (pvp_lv == pvp_monster_ || pvp_lv == pvp_monster_game) {
        return 931;
	} else {
		return 920;
	}
}

inline uint32_t get_contest_pvp_type(uint32_t pvp_btl_type)
{
    if (pvp_btl_type == pvp_monster_game) {
        return 0;
    } else if (pvp_btl_type == pvp_contest_advance) {
        return 1;
    } else if (pvp_btl_type == pvp_contest_final) {
        return 2;
    } else {
        return 3;
    }
}

uint32_t get_btl_fd_idx_by_room_id(uint32_t room_id);

int get_btl_fd_idx_by_server_id(uint16_t id);

int date2timestamp(const char* fromstr, const char* fmt, time_t &totime);

int enter_room(player_t* p, int fd_idx, battle_mode_t mode = btl_mode_pve, uint8_t pos = 0);


//-------------------------------------------------------------------------------

/**
  * @brief if user can in special stage
  * @return true can, false cannot
  */
bool can_user_in_special_stages(player_t* p, uint32_t stage_id, uint32_t diff = 0);

/**
  * @brief load skill configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_special_stages(xmlNodePtr cur);

#endif

