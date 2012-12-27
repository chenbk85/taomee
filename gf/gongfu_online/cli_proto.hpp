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

#ifndef KF_CLI_PROTO_HPP_
#define KF_CLI_PROTO_HPP_

#include <libtaomee++/inet/pdumanip.hpp>

extern "C" {
#include <libtaomee/project/types.h>
}

#include "map.hpp"
#include "fwd_decl.hpp"
#include "player.hpp"
//#include "magic_number.hpp"

/**
  * @brief command id for client protocol
  */
enum cli_cmd_t {
	/*! starting command id */
	cli_proto_cmd_start	= 1001,

	cli_proto_login		= 1001,
	cli_proto_get_encryption_session = 1002,
	cli_proto_verify_pay_passwd	= 1003,
	cli_proto_user_public_info	= 1004,
	cli_proto_get_ranged_svr_list = 1012,
	cli_proto_login_new_svr     = 1013,
	cli_proto_enter_new_svr     = 1014,
	//cli_proto_redirect_online   = 1015,
	cli_proto_is_set_pay_passwd = 1016,

	cli_proto_enter_map	= 2001,
	cli_proto_leave_map	= 2002,
	cli_proto_list_user	= 2003,
	cli_proto_chg_app	= 2004,
	cli_proto_get_home_list_pet = 2005,
    cli_proto_up_power_user         = 2006,
    cli_proto_synchro_power_user    = 2007,
    cli_proto_notify_client_play_cartoon = 2008,
    cli_proto_client_rebuild_gf_fireworks = 2009,
	cli_proto_get_usr_simple_info	= 2051,
	cli_proto_get_usr_detail_info	= 2052,
	cli_proto_get_hunter_top_info	= 2053,
	cli_proto_get_hunter_self_info	= 2054,
	cli_proto_change_usr_nickname	= 2061,
	cli_proto_get_amb_task_info		= 2071,
	cli_proto_set_amb_task_status	= 2072,
	cli_proto_get_amb_reward		= 2073,
	cli_proto_get_daily_hatch_summon_item = 2075,
	cli_proto_daily_hatch_summon          = 2076,
	cli_proto_check_task_extra_logic = 2077,

	cli_proto_get_max_times_chapter	= 2081,
	cli_proto_set_max_times_chapter	= 2082,
	cli_proto_finger_guessing   	= 2083,
	cli_proto_get_finger_guessing_data	= 2084,

	cli_proto_user_vip_info         = 2100,	
	cli_proto_walk					= 2101,
	cli_proto_talk					= 2102,
	cli_proto_walk_keyboard			= 2103,
	cli_proto_stand					= 2104,
	cli_proto_jump					= 2105,

	cli_proto_get_timestamp			= 2110,
	cli_proto_send_block_info	    = 2111,
	cli_proto_send_block_result     = 2112,

	
	/*! for friend command */
	cli_proto_add_friend			= 2151,
	cli_proto_reply_add_friend		= 2152,
	cli_proto_del_friend			= 2153,
	cli_proto_add_blacklist			= 2154,
	cli_proto_del_blacklist			= 2155,
	cli_proto_get_blacklist			= 2156,
	cli_proto_check_users_onoff		= 2157,
	cli_proto_friend_login          = 2158,
	cli_proto_forbiden_add_friend   = 2160,

    /*! for god */
	cli_proto_get_numen_list        = 2161,
	cli_proto_invite_numen          = 2162,
	cli_proto_change_numen_status   = 2163,
	cli_proto_god_use_skills_scroll = 2164,
	cli_proto_make_numen_sex        = 2165,
	cli_proto_change_numen_nick     = 2166,
	cli_proto_list_simple_role_info = 2167,

	/*! for player's tasks */
	cli_proto_undertake_task			= 2201,
	cli_proto_finish_task				= 2202,
	cli_proto_get_task_buf				= 2203,
	cli_proto_set_task_buf				= 2204,
	cli_proto_task_buf_change_notify	= 2205,
	cli_proto_cancel_task				= 2206,
	cli_proto_get_killed_boss			= 2207,
	cli_proto_undertake_daily_task		= 2208,
	cli_proto_restrict_action			= 2209,
	cli_proto_double_exp_action			= 2210,
	cli_proto_double_exp_action_use_item = 2212,
	cli_proto_get_restrict_action_times = 2213,
	cli_proto_get_buff_skill			= 2214,
	cli_proto_coupons_exchange			= 2215,
	cli_proto_recv_restrict_reward		= 2217,
	cli_proto_show_restrict_reward		= 2216,
    cli_proto_team_member_reward        = 2218,
    cli_proto_get_swap_action_detail_info = 2219,
    cli_proto_get_buff_skill_list		= 2220,

	//for offline mode
	cli_proto_insert_offline_fumo_coins = 2222,
	cli_proto_start_offline_mode         = 2223,
	cli_proto_get_offline_info           = 2224,
	cli_proto_end_player_offline         = 2225,
	cli_proto_end_summon_offline         = 2226,

	//for fight team
	cli_proto_create_fight_team         = 2227,
	cli_proto_invite_team_member        = 2228, //inviter - server
	cli_proto_send_invite_team_member   = 2229, //server - invitee
	cli_proto_reply_invite_team_member        = 2230, //invitee - server
	cli_proto_send_reply_invite_team_member   = 2231, //server - invitee 
	cli_proto_get_team_info				      = 2232,
	cli_proto_del_team_member			      = 2233,
	cli_proto_del_team					      = 2234,
	cli_proto_send_team_member_changed		  = 2235,
	cli_proto_send_team_member_onoff		  = 2236,
	cli_proto_notify_team_info				  = 2237,
	cli_proto_get_team_top10				  = 2238,
	cli_proto_contribute_team				  = 2239,
	cli_proto_fetch_team_coin				  = 2240,
	cli_proto_team_active_enter				  = 2241,
	cli_proto_get_team_active_top100		  = 2242,
	cli_proto_synchro_team_info     		  = 2243,
	cli_proto_set_team_member_lv              = 2244,
	cli_proto_change_team_name                = 2245,
	cli_proto_change_team_mcast               = 2246,
	cli_proto_search_team_info          = 2247,
	
	//团队通用活动排名榜
	cli_proto_list_team_active_ranker   = 2248,

	cli_proto_list_single_active_ranker = 2249,
 

    //师徒
    cli_proto_invite_prentice           = 2250, //inviter - server
    cli_proto_send_invite_prentice      = 2251, //server - invitee
    cli_proto_reply_invite_prentice     = 2252, //invitee - server
    cli_proto_send_reply_invite_prentice= 2253, //server - invitee
    cli_proto_get_master_info           = 2254,
    cli_proto_get_prentice_info         = 2255,
    cli_proto_kick_prentice             = 2256,
    cli_proto_master_notice             = 2257,

    cli_proto_get_reward_player         = 2260,

	cli_proto_restrict_action_ex		= 2261,
	cli_proto_swap_action_ex			= 2262,
	
#ifdef DEV_SUMMON	
	/*! commands for summon monster */
	cli_proto_hatch_summon_mon		= 2301,
	cli_proto_set_summon_nick		= 2302,
	cli_proto_feed_summon_mon		= 2303,
	cli_proto_set_fight_summon		= 2304,
	cli_proto_get_summons_list		= 2305,
	cli_proto_call_summon			= 2306,
	cli_proto_evolve_summon			= 2307,
	cli_proto_disable_summon_skill	= 2308,
	cli_proto_summon_change_noti	= 2309,
	cli_proto_allocate_exp_to_summon = 2310,
	cli_proto_get_allocator_exp		= 2311,
	cli_proto_get_summons_attr_list = 2312,
	cli_proto_use_summon_skills_scroll = 2313,
	cli_proto_renew_summon_skill    = 2314,
	cli_proto_fresh_summon_attr     = 2221,
#endif

	/*! commands for battle */
	cli_proto_initiate_battle		= 2401,
	cli_proto_cancel_battle			= 2402,
	cli_proto_rsp_battle_invit		= 2403,
	cli_proto_player_ready			= 2404,
	cli_proto_lv_matching_btl		= 2405,
	cli_proto_pve_btl				= 2406,
	cli_proto_pve_btl_restart		= 2407,
	cli_proto_sync_process			= 2408,
	cli_proto_pve_btl_stage_jump	= 2410,

	cli_proto_btl_action_start		= 2451,

	cli_proto_player_move			= 2451,
	cli_proto_npc_move				= 2452,
	cli_proto_player_rsp_lag		= 2453,
	cli_proto_player_attack			= 2454,
	cli_proto_player_enter_map		= 2455,
	cli_proto_player_leave_btl		= 2456,
	cli_proto_player_pick_item		= 2457,
	cli_proto_summon_unique_skill	= 2459,
	cli_proto_player_gain_item      = 2462,

	cli_proto_btl_action_end		= 2480,

//	cli_proto_btl_invitation		= 2501,
//	cli_proto_battle_inv_rsp		= 2502,
	cli_proto_battle_ready			= 2503,
	cli_proto_battle_start			= 2504,
	cli_proto_battle_end			= 2505,
	cli_proto_battle_test_lag		= 2506,
	cli_proto_players_hit			= 2507,
	cli_proto_players_info			= 2508,
	cli_proto_player_attr_chg		= 2509,
	cli_proto_player_hpmp_chg		= 2510,
	cli_proto_player_item_drop		= 2511,
	cli_proto_player_lvup_noti		= 2515,
	cli_proto_players_buf_hit		= 2516,
	cli_proto_player_open_treasure_box		= 2520,
	cli_proto_player_honor_up_noti	= 2523,
	cli_proto_del_map_summon_obj 	= 2526,

	cli_proto_players_status_info = 2527,
	cli_proto_players_aura_event_noti = 2528,

	cli_proto_player_change_mechanism = 2529,
	cli_proto_player_get_mechanism_drop = 2530,
	   

	cli_proto_list_pvp_point_user 	= 2531,
	cli_proto_stand_pvp_point   	= 2532,
	cli_proto_leave_pvp_point   	= 2533,


	cli_proto_join_contest_team     = 2541,
	cli_proto_get_contest_team_info = 2542,
	cli_proto_get_contest_donate    = 2543,
	cli_proto_contest_donate_plant  = 2544,
	cli_proto_get_self_contest      = 2545,
    cli_proto_get_taotai_game_info  = 2546,
	cli_proto_get_advance_game_info = 2547,


	cli_proto_get_contest_pvp_list  = 2548,
	cli_proto_contest_pvp_yazhu     = 2549,
	cli_proto_contest_over_for_watch = 2552,
	cli_proto_get_final_game_info   = 2554,
	cli_proto_get_final_game_rank_list = 2555,
	cli_proto_contest_cancel_yazhu   = 2556,

	cli_proto_get_challenge_status   = 2557,


	//for card
	cli_proto_player_get_card_list     = 2560,
	cli_proto_player_insert_card       = 2561,
	cli_proto_player_erase_card        = 2562,
	cli_proto_card_set_reward          = 2563,

	cli_proto_buy_item				= 2601,
	cli_proto_buy_clothes			= 2602,
	cli_proto_wear_clothes			= 2604,
	cli_proto_get_item_list			= 2605,
	cli_proto_get_clothes_list		= 2606,
	cli_proto_use_skill_book		= 2607,
	cli_proto_set_itembind			= 2608,
	//cli_proto_get_itembind			= 2609,
	cli_proto_use_item				= 2609,
	cli_proto_discard_item			= 2610,
	cli_proto_discard_clothes		= 2611,
	cli_proto_get_clothes_duration	= 2612,

	cli_proto_sell_item				= 2613,
	cli_proto_sell_clothes			= 2614,
	cli_proto_repair_all_clothes	= 2615,

	cli_proto_decompose_attire		= 2616,
	cli_proto_compose_attire		= 2617,

    cli_proto_get_warehouse_item_list    	= 2618,
	cli_proto_move_item_bag_to_warehouse    = 2619,
	cli_proto_move_item_warehouse_to_bag    = 2620,

	cli_proto_get_warehouse_clothes_item_list  = 2621,
	cli_proto_move_clothes_item_bag_to_warehouse = 2622,
	cli_proto_move_clothes_item_warehouse_to_bag = 2623,

	cli_proto_learn_secondary_pro = 2624,
	cli_proto_secondary_pro_fuse = 2625,
	cli_proto_get_strengthen_material_list = 2626,
	cli_proto_strengthen_attire = 2627,
	cli_proto_strengthen_witout_material = 2650,
	cli_proto_upgrade_item = 2628,
	cli_proto_swap_action = 2629,
	cli_proto_get_swap_action = 2630,
	cli_proto_use_items = 2631,
	cli_proto_vitality_point_change = 2632,
	cli_proto_second_pro_exp_lv_change = 2633,
	cli_proto_set_app_buff_end = 2634,
	cli_proto_batch_sell_item = 2635,

	cli_proto_mail_list = 2671,
	cli_proto_mail_body = 2672,
	cli_proto_delete_mail = 2673,
	cli_proto_take_mail_enclosure = 2674,
	cli_proto_send_mail = 2675,
	cli_proto_new_mail = 2676,
	
	/*!for skills*/
	cli_proto_skill_bind 			= 2701,
	cli_proto_get_skills 			= 2702,
	//cli_proto_skill_upgrade 		= 2703,
	cli_proto_learn_new_skill 		= 2704,
	cli_proto_skill_upgrade 		= 2705,
	

	/*! for team battle */
	cli_proto_create_btl_team		= 2711,
	cli_proto_join_btl_team			= 2712,
	cli_proto_ready_btl_team		= 2713,
	cli_proto_start_btl_team		= 2714,
	cli_proto_hot_join_btl_team		= 2715,
	cli_proto_leave_btl_team		= 2716,
	cli_proto_cancel_ready_btl_team = 2717,
	cli_proto_set_btl_team_hot_join = 2718,
	cli_proto_kick_user_from_room   = 2719,

	cli_proto_roll_item             = 2729,
	cli_proto_roll_item_list        = 2730,
	cli_proto_get_roll_item         = 2731,


	cli_proto_invite_friend_btl_team = 2732,
	cli_proto_reply_friend_btl_team  = 2733,
	cli_proto_check_cheat		     = 2735,


    /* */
	cli_proto_ts_view_magic_gift    = 2801,
	cli_proto_ts_consume_magic_gift = 2802,
	cli_proto_ts_view_cryptogram_gift    = 2803,
	cli_proto_ts_consume_cryptogram_gift = 2804,
    cli_proto_query_gcoin_balance   = 2811,
    cli_proto_query_mb_balance      = 2812,
    cli_proto_mb_trade              = 2813,
    cli_proto_query_user_store_item = 2815,
    cli_proto_gcoin_trade           = 2816,
    cli_proto_getback_outdated_attire = 2818,
	/*!achievement*/
	cli_proto_get_achievement_data_list = 2820,
	cli_proto_add_achievement_data      = 2821,
	cli_proto_get_achieve_title         = 2822,
	cli_proto_notify_achieve_title      = 2823,
	cli_proto_set_player_achieve_title  = 2824,
	cli_proto_notify_achieve_get_mcast  = 2844,
	cli_proto_achievement_stat_info     = 2843,

	cli_proto_set_client_buf			= 2825,
	cli_proto_get_client_buf			= 2826,
	cli_proto_get_global_limit_data		= 2827,
	cli_proto_summon_dragon				= 2828,
	cli_proto_get_summon_dragon_list	= 2829,

	cli_proto_invite_player_2_tmp_team		= 2830,
	cli_proto_tmp_team_invite_msg_2_invitee			= 2831,
	cli_proto_reply_tmp_team_invite			= 2832,
	cli_proto_tmp_team_noti_reply_result	= 2833,
	cli_proto_noti_tmp_team_info_changed	= 2834,
	cli_proto_tmp_team_member_leave	= 2835,
	cli_proto_tmp_team_member_leave_msg_2_leader	= 2836,
	cli_proto_tmp_team_invite_pvp				= 2837,
	cli_proto_send_tmp_team_pvp_invite_2_member		= 2838,
	cli_proto_tmp_team_reply_pvp_invite		= 2839,
	cli_proto_tmp_send_team_reply_pvp_invite		= 2840,
	cli_proto_tmp_team_pvp_start	= 2841,
	cli_proto_tmp_team_status	= 2842,

    cli_proto_get_player_other_active_info  = 2901,
    cli_proto_get_player_single_other_active    = 2902,
    cli_proto_player_zhanbo_fate        = 2911,
    cli_proto_player_select_fate        = 2912,
    cli_proto_upgrade_god_guard         = 2913,
    cli_proto_set_god_guard_position    = 2914,
    cli_proto_get_god_guard_position    = 2915,
    cli_proto_player_bet                = 2917,
    cli_proto_player_fatch_bet          = 2918,

	/*!for home*/
	cli_proto_enter_own_home			= 3001,
	cli_proto_enter_other_home			= 3002,
	cli_proto_leave_home                = 3003,
	cli_proto_pet_move                  = 3004,
	cli_proto_pet_home_call             = 3005, 
	cli_proto_pet_home_call_broadcast   = 3006,
	cli_proto_pve_syn_home_info         = 3007,
	cli_proto_home_owner_off_line       = 3008,
	cli_proto_home_kick_off_guest       = 3009,

	cli_proto_get_plants_list			= 3010,
	cli_proto_sow_plant_seed			= 3011,
	cli_proto_set_plant_status          = 3012,
	cli_proto_ferlitizer_plant          = 3013,
	cli_proto_pick_fruit				= 3017,
	cli_proto_get_home_log				= 3018,

	cli_proto_enter_other_home_random   = 3014,
	cli_proto_get_friend_summon_list    = 3015,
	cli_proto_get_home_attr             = 3020,

	cli_proto_insert_vitality_2_home    = 3024,
	cli_proto_home_level_up             = 3021,
	cli_proto_summon_ai_change          = 3023,

	cli_proto_get_decorate_list         = 3030,
	cli_proto_set_decorate              = 3031,
	cli_proto_add_decorate_lv           = 3032,
	cli_proto_get_decorate_reward       = 3033,
	
	//for contest group
	cli_proto_auto_join_contest         = 3041,
	cli_proto_list_contest_group_info   = 3042,
	cli_proto_contest_group_auto_msg    = 3043,
	cli_proto_contest_start             = 3044,
	cli_proto_contest_all_over          = 3045,
	cli_proto_contest_end_msg           = 3046,

	cli_proto_leave_contest_group       = 3047,
	cli_proto_list_all_passed_contest       = 3048,

	cli_proto_contest_guess_champion    = 3049,
	cli_proto_list_all_contest_player   = 3050,
	cli_proto_re_join_contest           = 3051,

	/*for trade*/
	cli_proto_trade_enter_market    = 4001,
	cli_proto_trade_leave_market    = 4002,
	cli_proto_trade_setup_shop      = 4003,
	cli_proto_trade_drop_shop       = 4004,
	cli_proto_trade_pause_shop      = 4005,
	cli_proto_trade_update_shop		= 4006,
	cli_proto_trade_buy_goods		= 4007,
	cli_proto_trade_sell_goods      = 4008,
	cli_proto_trade_list_shop       = 4009,
	cli_proto_trade_shop_detail     = 4010,
	cli_proto_trade_decorate_shop   = 4011,
	cli_proto_trade_change_shop_name  = 4012,
	cli_proto_trade_shop_status_change = 4013,
	cli_proto_trade_get_sell_log	 = 4014,
	cli_proto_trade_enter_aim_market = 4015,
	//cli_proto_trade_mcast_message    = 4016,
	cli_proto_player_mcast_message    = 4016,
	cli_proto_trade_mcast_detail_info     = 4017,
	cli_proto_chat				     = 4018,

    cli_proto_invite_safe_trade         = 4020,
    cli_proto_reply_safe_trade          = 4021,
    cli_proto_cancel_safe_trade         = 4022,
    cli_proto_safe_trade_set_item       = 4023,
    cli_proto_safe_trade_action_agree   = 4024,

	//for city battle
	cli_proto_get_city_battle_info            = 4237,

	//for wuseng
       
	cli_proto_list_other_active_info           = 4238,
	cli_proto_list_test_room_info              = 4239,

	cli_proto_set_book_for_fault               = 4240,

	cli_proto_10_second_msg                    = 4241,

	cli_proto_question_info                    = 4242,

	cli_proto_cur_question_answer_info         = 4243,

	cli_proto_try_enter_test_room              = 4244,

	cli_proto_player_list_ranker_info           = 4245,

	cli_proto_player_get_other_info				  = 4246,

	cli_proto_player_item_2_score                 = 4247,

	cli_proto_get_player_community				  = 4248,

	cli_proto_exchange_coins_exploit			= 4249,

	cli_proto_banner_info_sync                 = 4250,
	cli_proto_protect_banner                   = 4251,
	cli_proto_rob_banner                       = 4252,
	cli_proto_banner_btl_over              = 4253,
	cli_proto_protect_banner_over          = 4254,
	cli_proto_list_all_banner_info         = 4255,
	cli_proto_cancel_protect_banner        = 4256,

	cli_proto_generate_magic_number        = 4257,
	cli_proto_bind_magic_number = 4258,
	cli_proto_list_magic_invite_info  = 4259,

	cli_proto_captain_team_contest         = 4260,
	cli_proto_get_team_contest_room        = 4261,
	cli_proto_team_contest_status          = 4262,
	cli_proto_contest_trigger_info         = 4263,
	cli_proto_btl_over_notify              = 4264,
	cli_proto_team_contest_ready_info       = 4265,

	cli_proto_distract_clothes_strength     = 4267,

	/*! for user feedback*/
	cli_proto_submit_msg			= 7001,
	cli_proto_report_msg			= 7002,
	cli_proto_get_questionnaire		= 7003,
	cli_proto_market_user_info		= 7004,
	/*! for notification*/
	cli_proto_simple_notify			= 8001,
	cli_proto_detailed_noti			= 8002,
	cli_proto_keepalive_noti		= 8003,
	cli_proto_noti_online_tm		= 8004,
	cli_proto_rltm_notify			= 8005,
	cli_proto_reply_btl_invite_notify	= 8006,
	cli_proto_query_home_player_count   = 8007,

	//cli_proto_mcast_player_notice	= 8011,4017
	cli_proto_mcast_official_notice	= 8012,

	/*! for player show state */
	cli_proto_set_player_show_state = 8110,
	cli_proto_get_donate_count      = 8111,
	cli_proto_get_buy_item_limit_list = 8112,
	cli_proto_stat_log				= 8113,
	/*! for ap_point toplist */
	cli_proto_get_ap_toplist        = 8114,
	/*! for ring task  */
	cli_proto_get_master_ring_task_count = 8115,

	cli_proto_get_xunlu_reward        = 8013,
	cli_proto_get_xunlu_active_info  = 8014,

	cli_proto_random_master         = 8015,
	cli_proto_change_master         = 8016,
	
	//for test
	cli_proto_get_player_attr		= 9001,
	cli_proto_get_mon_attr			= 9002,

	cli_proto_notify_dirty_words	= 9003,
	/** 
	 * @brief for hero top
	 */
	cli_proto_get_player_hero_top  = 9100,

    cli_proto_change_game_battle_value = 9901,
	/*! ending command id */
	cli_proto_cmd_end,

	/*! for testing if this server is OK */
	cli_proto_probe		= 30000,
	/*! max number of commands supported */
	cli_proto_cmd_max	= 30001
};

/**
  * @brief errno for client protocol
  */
enum cli_err_t {
// errnos from 10001 to 50000 are reserved for common errors
cli_err_invalid_session	= 10001,
cli_err_system_error	= 10002,
cli_err_system_busy		= 10003,
cli_err_multi_login		= 10004,
cli_err_dirtyword		= 10005,
cli_err_user_offline	= 10006,
cli_err_get_timestamp	= 10007,
cli_err_limit_time_btl	= 10008,
cli_err_closed_time		= 10009,
cli_err_set_amb_status	= 10010,
cli_err_get_amb_reward	= 10011,
//pack
cli_err_bag_no_space	= 10021,
cli_err_bag_no_space_2_mail = 10022,

cli_err_vip_only        = 10031,
cli_err_has_one_yuelingtu	= 10041,
cli_err_nonvip_operation_forbidden	= 10042,
cli_err_vipyear_exclusive_right	= 10043,
cli_err_notenough_coupons	= 10044,
cli_err_undertake_outmoded_task	= 10045,
cli_err_nick_change_in_timelimit = 10046,
cli_err_fight_team_err			 = 10047,
cli_err_get_top10_team_cd_err	= 10048,
cli_err_master_prentice_err	= 10049,
cli_err_master_prentice_err2	= 10220,
cli_err_player_have_master  	= 10221,
cli_err_not_in_offline_mode     = 10222,
cli_err_full_right_lv           = 20006,
cli_err_not_the_captain          = 20007,
cli_err_no_such_team_member     = 20008,




cli_err_clothes_cannot_wear	= 10101,
cli_err_no_clothes			= 10102,
cli_err_failed_to_buy		= 10103,
cli_err_no_enough_coins		= 10104,

cli_err_no_item				= 10105,
cli_err_less_lv				= 10106,
cli_err_no_enough_point		= 10107,
cli_err_no_skill_id			= 10108,
cli_err_skill_lv_max		= 10109,

cli_err_not_need_repair		= 10110,
cli_err_failed_to_sell		= 10111,
cli_err_failed_wear_inbtl	= 10112,
cli_err_pvp_not_available	= 10113,
cli_err_no_enough_exploit	= 10114,
cli_err_buy_with_exploit    = 10115,
cli_err_failed_to_decompose = 10116,
cli_err_failed_to_compose   = 10117,
cli_err_buy_with_fumo		= 10118,
cli_err_no_enough_fumo		= 10119,

// error no for task
cli_err_task_had_finished	= 10120,
cli_err_cancel_primary_task	= 10121,
cli_err_reundertake_task	= 10122,
cli_err_no_undertake_task	= 10123,
cli_err_too_many_task		= 10124,
cli_err_double_exp_action	= 10125,
cli_err_unable_in_battle	= 10126,
cli_err_less_vip_lv			= 10127,
cli_err_cannot_use_in_btl		= 10128,
cli_err_cannot_already_have_dexp = 10129,

cli_err_no_treasure_task_times = 10148,
cli_err_has_going_treasure_task	= 10149,

#ifdef DEV_SUMMON	
//error for summon
cli_err_no_summon_mon		= 10130,
cli_err_cant_set_fight		= 10131,
cli_err_less_fight_value	= 10132,
cli_err_summon_cannot_evolve= 10133,
cli_err_same_summon_exist	= 10134,
cli_err_allocate_exp_summon_not_exit  = 10136,
cli_err_allocate_exp_noenough_exp     = 10137,
cli_err_allocate_exp_need_evolve      = 10138,
cli_err_allocate_exp_lv_over_range    = 10139,
cli_err_summon_no_final_status        = 10201,
cli_err_summon_no_learn_this_skill    = 10202,
cli_err_summon_refresh_hatch_item     = 105137,
#endif

cli_err_contest_stage_over          = 10211,
cli_err_stage_limit_time_act_935	  = 10212,
cli_err_swap_action_illegal         = 10213,
cli_err_team_active_have_finish     = 10214,
cli_err_team_coin_not_enough        = 10215,
cli_err_not_team_captain            = 10216,
cli_err_not_team_member             = 10217,
cli_err_team_coins_limit            = 10218,
cli_err_not_in_correct_map          = 10223,


cli_err_pre_skill_requst			= 10140,

cli_err_cannot_already_have_bless = 10141,

cli_err_cannot_take_in_taotai_game = 10142,
cli_err_cannot_take_gift_taotai_game = 10143,
cli_err_cannot_take_in_advance_game = 10144,
cli_err_cannot_take_gift_advance_game = 10145,
cli_err_cannot_take_in_final_game = 10146,
cli_err_cannot_evolve_summ_ex  = 10147,

cli_err_cannot_dec_attire	= 10135,
//error for daily restr
cli_err_limit_time_act		= 10150,

cli_err_limit_vip_act		= 10151,
cli_err_limit_time_double_exp   = 10152,

cli_err_day_limit_time_act 		= 10153,
cli_err_week_limit_time_act 	= 10154,
cli_err_month_limit_time_act 	= 10155,
cli_err_year_limit_time_act 	= 10156,
cli_err_ever_limit_time_act 	= 10157,
cli_err_stage_limit_time_act_908 = 10158,
cli_err_stage_limit_time_act_909 = 10159,
cli_err_stage_limit_time_act_910 = 10160,
cli_err_large_lv                = 10161,
cli_err_active_over             = 10162,
cli_err_stage_limit_time_act_914 = 10163,
cli_err_stage_limit_time_act_916 = 10164,
cli_err_stage_limit_take_summon  = 10165,

cli_err_stage_limit_time_act_918 = 10166,
cli_err_max_exp_lv				 			 = 10167,
cli_err_market_limit_enter       = 10168,
cli_err_fumo_stage_limit_take_summon  = 10169,	


cli_err_strengthen_material_notenough = 10170,
cli_err_strengthen_material_quality   = 10171,
cli_err_strengthen_material_level     = 10172,
cli_err_strengthen_stone_type         = 10173,
cli_err_strengthen_stone_quality      = 10174,
cli_err_strengthen_stone_level        = 10175,
cli_err_attire_cannot_strengthen      = 10176,


cli_err_shilian_stage_limit_take_summon = 10177,
cli_err_in_trade_svr					= 10178,
cli_err_in_battle_svr					= 10179,
/*for trade */
cli_err_trade_market_full 			  = 10180,
cli_err_trade_shop_busy  			  = 10181,
cli_err_trade_not_in_market			  = 10182,
cli_err_untradable_item               = 10183,
cli_err_goods_price_error             = 10184,
cli_err_fumo_have_got_reward          = 10185,
cli_err_trade_err_market              = 13001,
cli_err_trade_not_a_shopkeeper        = 13005,
cli_err_trade_mcast_in_30s            = 10186,
cli_err_trade_mcast_in_3min           = 10187,
cli_err_stage_limit_time_act_925	  = 10188,
cli_err_stage_limit_time_act_926	  = 10189,
cli_err_stage_limit_time_act_927	  = 10190,
cli_err_add_friend_invalid_id         = 10191,
cli_err_add_friend_fobiden            = 10192,

cli_err_cannot_safe_trade               = 10225,
cli_err_in_safe_trade               = 10226,

cli_err_strengthen_stone_count        = 10193,
cli_err_stage_limit_time_act_939	  = 10194,


cli_err_nangua_cd_tm				  = 10219,


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
cli_err_battle_room_not_exist	= 11013,
cli_err_battle_no_room			= 11015,
cli_err_battle_footprint_full	= 11016,
cli_err_battle_pvp_room_not_exist	= 11017,
// achieve title
cli_err_achieve_title_not_exist   = 11040,
cli_err_achieve_title_not_get     = 11041,
// error for warehouse
cli_err_warehouse_not_enough_money  = 11050,
cli_err_warehouse_warehouse_full    = 11051,
cli_err_warehouse_item_bag_full     = 11052,
cli_err_warehouse_clothes_bag_full  = 11053,
cli_err_warehouse_can_not_move_item = 11054,

//error for secondary_profession   
cli_err_secondary_pro_invalid_pro_type = 11055,
cli_err_secondary_pro_exist = 11056,
cli_err_secondary_pro_not_exist = 11057,
cli_err_secondary_pro_invalid_formulation_id = 11058,
cli_err_secondary_pro_low_level = 11059,
cli_err_secondary_pro_not_enough_money = 11060,
cli_err_secondary_pro_item_count_err = 11061,
cli_err_secondary_pro_player_low_level = 11062,
cli_err_secondary_pro_bag_full = 11063,
cli_err_secondary_pro_not_enough_point = 11073,
//不是新秀不能加入战队
cli_err_you_are_not_hero   = 11064,
cli_err_not_enough_plant = 11065,
cli_err_join_no_team = 11066,
cli_err_not_enough_flower = 11067,
cli_err_swap_action_not_in_time = 11068,
cli_err_not_in_ranking_list     = 11069,
cli_err_not_victory_team        = 11070,
cli_err_not_the_first_of_world  = 11071,
cli_err_action_cd_limited		= 11072,
cli_err_contest_donate_over		= 11074,
cli_err_yanhua_cdtime		= 10224,
cli_err_accept_vip_reward_task  = 10227,

//error for mail 
cli_err_mail_id_exist = 11080,
cli_err_mail_id_not_exist = 11081,
cli_err_mail_bag_full = 11082,
cli_err_not_enough_money = 11083,
cli_err_invalid_receive_id = 11084,
cli_err_mail_no_enclosure = 11085,
//error for toplist
cli_err_ap_toplist_para = 11086,

cli_err_have_summoned_dragon = 11101,
cli_err_cannot_summoned_dragon = 11102,
cli_err_have_get_summoned_dragon_rewards = 11103,
cli_err_summoned_dragon_rewards_type_err = 11104,
cli_err_have_not_summoned_dragon		 = 11105,
cli_err_not_give_nimbus					 = 11106,
cli_err_cannot_summoned_dragon_err_time	 = 11107,
cli_err_pay_passwd_err_too_many			 = 11109,
cli_err_enter_market_err_tm				 = 11110,
cli_err_coins_too_much					 = 11111,


//error for buy
cli_err_limit_to_buy = 11090,
/*! for home*/
cli_err_home_system_err	= 12001,

cli_err_action_time_limited = 12002,

cli_err_action_global_limited = 12003,


//不在侠士团比赛服务器,
cli_err_action_svr_not_pvp    = 130004,
//已经报过名
cli_err_team_contest_again    = 130005,
//名额已满，请换线
cli_err_team_contest_no_room  = 130006,
//队长没有报名 
cli_err_captain_not_ready     = 130007,
//不是比赛进入时间
cli_err_not_contest_enter_time      = 130008,
//奖品被人抢先一步获得
cli_err_no_reward_item_left       = 130009,
//现在还不能报名
cli_err_not_in_captain_ready_time  = 130010,

//荣誉之战没有找到对手
cli_err_contest_team_match_fail = 130011,

//所有的服务器都满了
cli_err_contest_team_contest_full = 130012,


//
cli_err_join_team_once_week    = 130012,

//旧的装备的强化等级太低
cli_err_old_clothes_lv_too_low  = 130013,
//被传承的装备等级必须高于传承装备
cli_err_old_must_high_than_new   = 130014,
//物品品质必须一样
cli_err_distract_quality_diff    = 130015,
//武器只能和武器相互传承
cli_err_distract_weapon_2_clothes  = 130016,
//衣服不能传承给武器
cli_err_distract_clothes_2_weapon  = 130017,

cli_err_action_refused	= 130018,
cli_err_2v2_reward_err  = 130019,

cli_err_role_lv_more_then_20  = 130021,
cli_err_already_bind_magic_number = 130023,
cli_err_invaild_magic_number = 130022,
cli_err_celebration_goods_full = 130024,

cli_err_cannot_pick		 = 14001,

cli_err_userid_error		  = 20004,
cli_err_tmp_baotu_have_regedit = 20005,

cli_err_all_ready_in_group = 20009,
cli_err_all_not_enough_exploit = 20010,
cli_err_can_not_guess_now = 20011,
cli_err_no_group_now = 20012,
cli_err_system_repair = 20013,
cli_err_not_in_contest_time = 20014,

// we return errno from dbproxy by plusing 100000 to dberr (100000 + dberr)	
cli_err_base_dberr		= 100000,
/* we return errno from magic code svr by plusing 200000 to mcerr (200000 + mcerr) */	
cli_err_base_mcerr		= 200000,
/* we return errno from store vipsvr by plusing 300000 to storerr (300000 + storerr)*/
cli_err_base_storerr    = 300000,
cli_err_end
};

void proc_cached_pkgs();

/**
  * @brief cache a package
  */
struct cached_pkg_t {
	uint16_t	len;
	uint8_t		pkg[];
};


/**
  * @brief some constants
  */
enum {
	/*! max acceptable length in bytes for each client package */
	cli_proto_max_len	= 8 * 1024,
};

#pragma pack(1)

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
	/*! errno: 0 for OK */
	uint32_t	ret;
	/*! body of the package */
	uint8_t		body[];
};

#pragma pack()


extern uint8_t pkgbuf[1 << 21];

/**
  * @brief send a package to player p
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_to_player(player_t* p, void* pkg, uint32_t len, int completed);

int send_to_player_by_uid(uint32_t uid, fdsession_t* fdsess, void* pkgbuf, uint32_t len, int completed);


int send_header_to_player_2(uint32_t uid, fdsession_t* fdsess, uint32_t cmd, uint32_t err, int completed);
/**
  * @brief send a package to all the players
  * @param pkgbuf the pkg to send
  * @param len length of the package
  */
void send_to_all(void* pkgbuf, uint32_t len);


/**
  * @brief send a package to all players that are in the map 'm'
  * @param m
  * @param pkg package to send
  * @param len length of the package
  */
void do_send_to_map(map_t* m, void* pkg, uint32_t len);
/**
  * @brief send a package to all players that are in the same map as 'p' is
  * @param p
  * @param pkg package to send
  * @param len length of the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
void send_to_map(player_t* p, void* pkg, uint32_t len, int completed);

/**
  * @brief send a package to all players that are in the same map as 'p' is
  * @param p
  * @param pkg package to send
  * @param len length of the package
  * @return 0 on success, -1 on error
  */
void send_to_map_except_self(player_t* p, void* pkg, uint32_t len);
/**
  * @brief send a package header to player p
  * @param p player to send a package to
  * @param cmd command id of the package
  * @param err errno to be set into the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_header_to_player(player_t* p, uint16_t cmd, uint32_t err, int completed);

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
  * @return value that was returned by the corresponding protocol handler
  */
int dispatch(void* data, fdsession_t* fdsess, bool first_tm = true);

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
inline void init_cli_proto_head_full(void* header, userid_t uid, uint32_t seqno, uint16_t cmd, uint32_t len, uint32_t ret)
{
	cli_proto_t* proto = reinterpret_cast<cli_proto_t*>(header);

	proto->id  = taomee::bswap(uid);
	proto->len = taomee::bswap(len);
	proto->cmd = taomee::bswap(cmd);
	proto->ret = taomee::bswap(ret);
	proto->seqno = taomee::bswap(seqno);
}

/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  */
inline void init_cli_proto_head(void* header, const player_t* p, uint16_t cmd, uint32_t len)
{
	if (p) {
		init_cli_proto_head_full(header, p->id, p->seqno, cmd, len, 0);
	} else {
		init_cli_proto_head_full(header, 0, 0, cmd, len, 0);
	}
}

bool init_timer_callback_type();

bool is_time_limit_1(player_t *p);

bool is_time_limit_2(player_t *p);
bool is_time_limit_3(player_t *p);


#endif // KF_CLI_PROTO_HPP_

