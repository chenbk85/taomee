/**
 *============================================================
 *  @file      dbproxy.hpp
 *  @brief    dbproxy related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_DBPROXY_HPP_
#define KF_DBPROXY_HPP_

extern "C" {
#include <libtaomee/project/types.h>
}

#include "fwd_decl.hpp"
#define NEW_LOGIN_PACKAGE

/**
  * @brief command id for dbproxy
  */
enum db_cmd_t {
	//user info
	dbproto_verify_pay_passwd = 0x008B,
	dbproto_is_set_pay_passwd = 0x008D,


#ifndef NEW_LOGIN_PACKAGE
	dbproto_get_player			=  0x0601,
#else
	dbproto_get_player			=  0x06A1,
#endif
    dbproto_user_store_item     =  0x0617,
    dbproto_query_gcoin_balance =  0x0619,
	dbproto_change_usr_nickname	=  0x0702,
	dbproto_add_offline_msg		=  0x0704,
	dbproto_logout				=  0x0705,
	dbproto_get_usr_simple_info	=  0x0606,
	dbproto_get_usr_detail_info	=  0x0607,
    //----- numen 0x0609 0x060A 0x060E 0x060F
    dbproto_get_numen_list      =  0x0706,
    dbproto_change_numen_status =  0x0708,
    dbproto_make_numen_sex      =  0x0609,
    dbproto_invite_numen        =  0x060A,
    dbproto_change_numen_nick   =  0x060E,
    dbproto_set_player_fate     =  0x060F,

	dbproto_set_user_flag		=  0x070C,
	dbproto_add_amb_achieve_num	=  0x070A,
	dbproto_set_amb_task_status =  0x070B,
	dbproto_set_client_buf	    =  0x070D,
	dbproto_get_client_buf	    =  0x071E,
	
	dbproto_discard_clothes		= 0x070E,

	dbproto_add_friend			= 0x0730,
	dbproto_del_friend			= 0x0731,
	dbproto_add_blacklist		= 0x0732,
	dbproto_del_blacklist		= 0x0733,
	dbproto_forbiden_add_friend_flag = 0x0734,
	dbproto_query_forbiden_friend_flag = 0x0735,
	dbproto_del_friend_whatever	= 0x0736,
	dbproto_get_friend_list     = 0x0635,
	dbproto_get_blacklist		= 0x0634,

	dbproto_get_clothes_list	=  0x0641,
	dbproto_get_item_list		=  0x0642,
	dbproto_get_packs			=  0x0643,
    dbproto_get_strengthen_material_list = 0x0644,
	
	dbproto_get_warehouse_item_list         =  0x0625,
	dbproto_move_item_warehouse2bag         =  0x0626,
	dbproto_move_item_bag2warehouse         =  0x0627,
	
	dbproto_get_warehouse_clothes_item_list         =  0x0628,	
	dbproto_move_clothes_item_warehouse2bag         =  0x0629,
	dbproto_move_clothes_item_bag2warehouse         =  0x062A,	

	//for card
	dbproto_get_card_list           = 0x07AD,
	dbproto_insert_card             = 0x07AE,
	dbproto_erase_card              = 0x07AF,
	
	dbproto_learn_secondary_pro  = 0x0630,
	dbproto_secondary_pro_fuse   = 0x0631,
	dbproto_get_secondary_pro_list = 0x0632,

	dbproto_mail_head_list     = 0x0633,
	dbproto_mail_body          = 0x0636,
	dbproto_delete_mail        = 0x0637,
	dbproto_take_mail_enclosure = 0x0638,
	dbproto_send_mail   =  0x0639,
	dbproto_send_system_mail = 0x063A,
	dbproto_reduce_money = 0x0640,
	dbproto_set_player_show_state = 0x0649,

	dbproto_get_buy_item_limit_list = 0x0650,
	dbproto_set_buy_item_limit_data = 0x0651,
	dbproto_get_ring_task_list   	= 0x0652,
	dbproto_replace_ring_task       = 0x0653,
	dbproto_delete_ring_task        = 0x0654,
	dbproto_get_ring_task_history_list = 0x0655,
	dbproto_replace_ring_task_history =  0x0656,
	dbproto_set_open_box_times        =  0x0657,
	dbproto_del_outmoded_task           =  0x0658,

	dbproto_sell_clothes		=  0x0710,
	dbproto_repair_all_clothes	=  0x0711,
	dbproto_coupons_exchange	=  0x071A,
	dbproto_buy_clothes			=  0x0743,
	dbproto_wear_clothes		=  0x0744,
	dbproto_take_off_clothes    =  0x0645,
	dbproto_decompose_attire	=  0x07D9,
	dbproto_compose_attire		=  0x07D8,


	
	dbproto_buy_item			=  0x0745,
	dbproto_set_itembind		=  0x0746,
	dbproto_add_item            =  0x0759,
	dbproto_use_item			=  0x0747,
	dbproto_add_buff			=  0x0752,
	dbproto_del_buff_on_player  =  0x075E,


	dbproto_get_itembind		=  0x0647,
	dbproto_set_unique_item_bit	=  0x074A,
	dbproto_strengthen_attire	=  0x074B,
	//added by cws 0620
	dbproto_strengthen_without_material	=  0x075C,
	dbproto_sell_item			=  0x0758,
	dbproto_batch_sell_item	    =  0x0605,
	dbproto_discard_item		=  0x0757,
	dbproto_donate_item         =  0x075A,
	
	dbproto_use_skill_book		=  0x07C0,
	dbproto_skill_upgrade		=  0x07C2,
	dbproto_get_skills			=  0x06C3,
	dbproto_skill_bind			=  0x07C4,
	dbproto_learn_new_skill		=  0x07C5,
	dbproto_reset_skill			=  0x0753,

	dbproto_distract_clothes_strength = 0x07B1,

	dbproto_get_sell_log		=  0x07C8,
	dbproto_get_coins			=  0x0602,
	dbproto_player_pick_item    =  0x0748,
	dbproto_player_del_items    =  0x074C,

	dbproto_get_kill_boss_list  =  0x0770,
	dbproto_replace_kill_boss   =  0x0771,

	dbproto_get_achievement_data_list = 0x0772,
	dbproto_replace_achievement_data =  0x0773,
	dbproto_get_achieve_title   =  0x0674,
	dbproto_notify_achieve_title   =  0x0774,
	dbproto_set_player_achieve_title   =  0x0775,

	dbproto_get_player_stat_info     = 0x07BF,
	dbproto_add_player_stat_info     = 0x07BE,

	dbproto_get_use_item_list   = 0x0776,
	dbproto_replace_use_item    = 0x0777,

	dbproto_get_other_active_info   = 0x077A,
	dbproto_set_other_active_info   = 0x077B,
	dbproto_get_other_active_info_list  = 0x077C,
	dbproto_set_role_god_guard_info = 0x077D,
	dbproto_get_single_other_active = 0x077E,

	dbproto_get_home_data       = 0x0780,
	dbproto_set_home_data  = 0x0781,
#ifdef DEV_SUMMON
	dbproto_get_summon_list		= 0x0620,
	dbproto_hatch_summon_mon	= 0x0721,
	dbproto_feed_summon_mon		= 0x0722,
	dbproto_set_summon_nick		= 0x0723,
	dbproto_set_fight_summon	= 0x0724,
	dbproto_evolve_summon		= 0x0726,
	dbproto_re_evolve_summon    = 0x0727,
	dbproto_allocate_exp_to_summon = 0x0728,
	//	dbproto_add_allocator_exp    = 0x0729,
	dbproto_fresh_summon_attr    = 0x073B,
    dbproto_renew_summon_skills    = 0x072C,
	dbproto_use_sum_skills_scroll  = 0x072D,
#endif

	dbproto_get_done_task_list	= 0x06D1,
	dbproto_get_going_task_buf	= 0x06D2,
	dbproto_get_all_task_list	= 0x06D9,
	dbproto_set_task_flag		= 0x07D3,
	dbproto_set_task_buf		= 0x07D4,
	dbproto_swap_item			= 0x07D5,
	dbproto_swap_action			= 0x07D6,
	dbproto_get_swap_action		= 0x06D7,
	dbproto_get_fumo_info		= 0x06D8,
    dbproto_get_double_exp_data = 0x06DA,
    dbproto_set_double_exp_data = 0x07DA,
    dbproto_get_max_times_chapter = 0x06DB,
    dbproto_set_max_timse_chapter = 0x07DB,
    dbproto_team_member_reward  = 0x07DC,
	dbproto_set_extern_grid_count = 0x0765,

	dbproto_set_power_user_info     = 0x07E2,
	dbproto_set_player_basic_attr   = 0x07E8,
	dbproto_set_player_stage_list   = 0x07E7,
	dbproto_upgrade_item			= 0x07C9,
	dbproto_clear_swap_action	   = 0x07DD,
	dbproto_set_role_vitality_point = 0x074D,
	dbproto_set_second_pro_exp      = 0x074E,
	dbproto_set_player_team_flg			= 0x074F,

	dbproto_login_report_info		= 0xA125,//= 0x3D00,
	//dbproto_add_session				= 0x0B01,
	dbproto_add_session				= 0x0A122,
	//dbproto_check_session		= 0x0B02,
	dbproto_check_session		= 0xA024,
	dbproto_get_user_public_info= 0x0022,
	
    //师徒
    dbproto_get_master_info         =   0x07CA,
    dbproto_get_prentice_info       =   0x07CB,
    dbproto_master_add_prentice     =   0x07CC,
    dbproto_master_del_prentice     =   0x07CD,
    dbproto_set_prentice_grade      =   0x07CE,

	dbproto_set_role_info			=	0x07E5,

	dbproto_query_restriction_list	= 0x0E04,
	dbproto_add_action_count		= 0x0E05,

	dbproto_get_player_hero_top     = 0x0E09,
	dbproto_set_player_hero_top     = 0x0E10,
	

	dbproto_notify_use_shopitem_log = 0x0E08,
	dbproto_notify_get_shopitem_log = 0x0E0A,
	dbproto_get_hunter_top_info     = 0x0E11,
	dbproto_get_hunter_self_info     = 0x0E18,
	dbproto_del_player_old_hero_top = 0x0E14,
	dbproto_get_donate_count    = 0x0E15,
	dbproto_add_donate          = 0x0E16,
	dbproto_get_ap_toplist      = 0x0E17,

	dbproto_join_contest_team   = 0X0E19,
	dbproto_get_contest_team_info = 0X0E1A,
	dbproto_get_contest_donate    = 0X0E1B,
	dbproto_contest_donate_plant  = 0X0E1C,

	dbproto_get_self_contest_info = 0X0E1D,
    dbproto_get_pvp_game_info  = 0x0E21,
    dbproto_save_pvp_game_data  = 0x0E22,
    dbproto_save_pvp_game_flower  = 0x0E23,
    dbproto_get_final_game_rank_list = 0x0E24,
	dbproto_get_summon_dragon_list = 0x0E25,
	dbproto_add_summon_dragon	= 0x0E26,
	dbproto_create_fight_team	= 0x0E27,
	dbproto_add_fight_team_member	= 0x0E28,
	dbproto_get_team_info			= 0x0E29,
	dbproto_del_team_member			= 0x0E2A,
	dbproto_del_team				= 0x0E2B,
	dbproto_team_info_changed		= 0x0E2C,
	dbproto_get_team_top10			= 0x0E2D,
	dbproto_contribute_team         = 0x0E2E,
	dbproto_fetch_team_coin			= 0x0E2F,
	dbproto_team_active_enter		= 0x0E34,
	dbproto_get_team_active_top100	= 0x0E35,
	dbproto_team_active_score_change= 0x0E36,
	dbproto_set_team_base_info      = 0x0E37,
	dbproto_set_memeber_right_lv    = 0x0E42,
	dbproto_change_team_name        = 0x0E43,
	dbproto_change_team_mcast       = 0x0E44,
	dbproto_search_team_info        = 0x0E46,
	dbproto_gain_team_exp           = 0x0E47,
	dbproto_team_tax_coin           = 0x0E48,

	dbproto_list_ranker         = 0x0E37,
	dbproto_insert_ranker       = 0x0E38,
	dbproto_save_random_reward      = 0x0E39,
	dbproto_get_reward_player_list  = 0x0E40,
	dbproto_set_player_reward_flag  = 0x0E41,

    dbproto_get_home_log		 = 0x062C,
	
	/*! for user feedback*/
	dbproto_submit_msg			=  0xF130,

	/*! for user on off line*/
	dbproto_report_user_onoff	=  0xF105,
	dbproto_report_msg      	=  0xF132,
	dbproto_report_chat_content	=  0xF133,
	dbproto_report_add_info 	=  0xF135,

	//offline mode 
	dbproto_get_offline_info    = 0x07F4,
	dbproto_set_offline_info    = 0x07EF,

	dbproto_get_other_info		= 0x062D,
	dbproto_set_other_info		= 0x0737,

	dbproto_get_player_community	= 0x0671,
	
	dbproto_check_update_trade_info = 0x075B,

	dbproto_exchange_coins_exploit    = 0x07EA,

	dbproto_list_simple_role_info    = 0x073A,

};

/**
  * @brief errno returned from dbproxy
  */
enum db_err_t {
	dberr_sys_error      = 1001,
	dberr_db_error       = 1002,
	dberr_net_error      = 1003,
	/*! no such user id */
	dberr_no_uid         = 1105,

	dberr_session_check  = 4331,
};

/**
  * @brief some constants
  */
enum {
	/*! max acceptable length in bytes for each package from/to dbproxy */
	dbproto_max_len	= 16 * 1024,
    player_guess_len = 16 * 1024,
};

#pragma pack(1)

/**
  * @brief db protocol type
  */
struct db_proto_t {
	/*! package length */
	uint32_t	len;
	/*! sequence number ((p->fd << 16) | p->waitcmd) */
	uint32_t	seq;
	/*! command id */
	uint16_t	cmd;
	/*! errno */
	uint32_t	ret;
	/*! user id */
	userid_t	id;
	/*! role create time */
	uint32_t	role_tm;
	/*! package body */
	uint8_t		body[];
};

#pragma pack()

/*! for packing protocol data and send to dbproxy */
extern uint8_t dbpkgbuf[dbproto_max_len];

/*! socket fd that is connecting to dbproxy */
extern int proxysvr_fd;

/**
  * @brief send a request to db proxy
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int  send_request_to_db(player_t* p, userid_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, uint32_t body_len);

/**
  * @brief send a UDP request to db server
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_udp_request_to_db(const player_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len, uint8_t index);

/**
  * @brief send a UDP request to db server
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_udp_onoff_to_db(const player_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);

/**
  * @brief send a request to db proxy
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_request_to_db_no_roletm(player_t* p, userid_t id, uint16_t cmd, const void* body_buf, uint32_t body_len);
/**
  * @brief send a request to seer db proxy
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_request_to_seerproxy(player_t* p, userid_t id, uint16_t cmd, const void* body, uint32_t body_len);

/**
  * @brief handle package return from dbproxy
  * @param dbpkg package from dbproxy
  * @param pkglen length of dbpkg
  */
void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen);

/**
  * @brief init UDP socket for db server
  */
int	init_dbsvr_udp_socket();

/**
  * @brief init UDP socket for db server
  */
int	init_dbsvr_onoff_socket();

/**
  * @brief init handles to handle protocol packages from dbproxy
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_db_proto_handles();


#endif // KF_DBPROXY_HPP_

