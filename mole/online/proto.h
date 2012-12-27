#ifndef _PROTO_H_
#define _PROTO_H_

// C89
#include <string.h>
// C99
#include <stdint.h>
// Posix
#include <arpa/inet.h>

#include "benchapi.h"
#include "map.h"
#include "service.h"

#define DES_KEY ">ta:mee<"
#ifdef TW_VER
#define DES_GAME_SCORE_KEY ",.ta0me>"
#else
#define DES_GAME_SCORE_KEY DES_KEY
#endif

enum {
	ERR_client_not_proc					= 10000,
	ERR_invalid_session					= 10001,
	ERR_inactive_user					= 10002,
	ERR_user_offline					= 10003,
	ERR_not_exist_user					= 10004,
	ERR_not_exist_item					= 10005,
	ERR_system_error					= 10006,
	ERR_occupied_pos					= 10007,
	ERR_too_many_user					= 10008,
	ERR_system_busy						= 10009,
	ERR_multi_login						= 10010,

	ERR_already_in_list					= 10011,
	ERR_dirty_word						= 10012,
	ERR_jail_limited					= 10013,
	ERR_login_banned					= 10014,
	ERR_count_limited					= 10015,
	ERR_board_not_found					= 10016,

	// For Temporarily Use Only
	ERR_fetched_already					= 10017,
	ERR_wrong_time						= 10018,
	//----------------------------

	ERR_friend_limit_exceeded			= 10019,
	ERR_offline_msg_limit_exceeded		= 10020,
	ERR_daily_money_limit_exceeded		= 10021,
	ERR_exchg_out_itm_limit_exceeded	= 10022,
	ERR_vote_flower_mud_limit_exceeded	= 10023,
	ERR_forbid_user_24_hours			= 10024,
	ERR_user_in_other_user_black_list	= 10025,
	ERR_user_in_user_black_list			= 10026,

	ERR_cannot_talk_time_limit			= 10027,
	ERR_cannot_talk_count_limit			= 10028,
	ERR_cannot_talk_has_been_ban		= 10029,

	ERR_cannot_ask_for_this_item		= 10512,

	ERR_fertilize_water_limit_exceeded	= 10913,
	ERR_vote_smc_limit_exceeded			= 10918,
	ERR_send_fu_limit_exceeded			= 10991,

	ERR_add_party_exceeded_per_day		= 10972,
	ERR_not_in_friend_list				= 11110,
	ERR_run_times_exceeded_per_day		= 11111,

	ERR_db_error						= 11002,
	ERR_db_sys_error					= 11003,
	ERR_not_enough_xiaomee				= 11117,

	//for birthday airship
	ERR_already_set_birthday			= 11127,

	// For Pet
	ERR_max_pet_num_exceeded			= 11130,
	ERR_can_not_follow_pet				= 11131,
	ERR_pet_skill_learnt_today			= 11132,

	//for lahm sport
	ERR_no_badge						= 11140,

	ERR_mole_groupid_not_existed		= 11150,
	ERR_mole_groupid_existed			= 11151,
	ERR_not_molegroup_owner				= 11152,
	ERR_max_molegroup_exceeded			= 11153,
	ERR_max_owned_molegroup_exceeded	= 11154,
	ERR_max_num_in_molegroup_exceeded	= 11155,
	ERR_user_not_in_molegroup			= 11156,
	ERR_user_already_in_molegroup		= 11157,

	ERR_taskid_not_exist				= 11176,

	//for homeland
	ERR_not_find_fruit					= 11202,
	ERR_fruit_num_exceed				= 11203,
	ERR_fruit_overdue					= 11204,
	ERR_plant_number_not_exist			= 11206,
	ERR_max_planted_seed_exceeded		= 11207,
	ERR_not_in_thief_time				= 12522,
	ERR_already_thief_this_plant		= 12523,

	ERR_diary_be_locked					= 12549,
	ERR_diary_id_not_exist				= 12550,
	ERR_diary_already_lock				= 12551,
	ERR_diary_already_unlock			= 12552,
	ERR_not_time_to_get_task2 			= 12553,
	ERR_have_not_done_task2				= 12554,
	ERR_alrdy_attend_magic_task			= 12555,
	ERR_no_right_attend_magic 			= 12556,
	ERR_have_not_done_task1				= 12557,
	ERR_alrdy_get_this_car				= 12569,

	// For Poll
	ERR_polled_already					= 11301,

	// for christmas
	ERR_already_get_christmas			= 11320,
	ERR_already_set_christmas			= 11321,
	ERR_not_set_christmas				= 11322,

	ERR_not_enough_golden				= 11323,
	ERR_not_enough_silver				= 11324,
	ERR_golden_num_exceed_per_day		= 11325,
	ERR_silver_num_exceed_per_day		= 11326,
	ERR_already_set_yuanbao_box_flag	= 11327,
	ERR_not_set_yuanbao_box_flag		= 11328,

	// For Wishing Pool
	ERR_already_made_wish				= 11801,
	ERR_wish_not_found					= 11802,

	// For mole party
	ERR_max_party_exceed_per_day		= 11810,

	ERR_max_home_item_num_exceeded		= 11201,
	// Temporarily Use Only
	ERR_magic_code_not_found			= 12101,
	ERR_magic_code_already_used			= 12102,
	ERR_magic_code_overdue				= 12103,
	//
	ERR_failed_to_buy_real_item			= 13001,
	//for card game
	ERR_card_inited						= 11146,

	//for mole bank
	ERR_XIAOMEE_LESS_THRESHOLD_ERR		= 11171,
	ERR_INVALID_TIME_LIMIT_ERR          = 11175,

    //for mole class
    ERR_class_already_created			= 12503,
    ERR_class_not_exist					= 12504,
    ERR_reach_max_class					= 12505,
    ERR_reach_max_class_member			= 12506,
    ERR_class_member_exist				= 12507,

	// for mole pasture
	ERR_animal_not_adult				= 12526,
	ERR_no_item_in_pasture_store		= 12527,
	ERR_animal_id_not_exist				= 12528,
	ERR_pasture_fish_be_locked			= 12530,
	ERR_already_fish_this_pasture		= 12531,
	ERR_small_animal_num_exceed			= 12535,
	ERR_small_fish_num_exceed			= 12536,
	ERR_adult_animal_num_exceed			= 12537,
	ERR_max_feed_num_exceed				= 12538,
	ERR_already_sport_sign				= 12543,
	ERR_pkee_score_too_small			= 12548,
	ERR_not_enough_gift_times			= 12560,
	ERR_no_chemical_fertilizer			= 12612,

	//for magic server
	ERR_magic_server_system_base		= 40000,
	ERR_magic_server_system_last		= 41000,

	//for mibi server
	ERR_mibi_inquire_max_count_exceeded = 50009,
	ERR_mibi_bad_product_id				= 50010,
	ERR_mibi_user_not_exist				= 50011,
	ERR_mibi_pay_passwd_error			= 50012,
	ERR_mibi_curr_count_not_enough		= 50013,
	ERR_mibi_must_be_vip				= 50014,
	ERR_mibi_req_count_exceed_limit		= 50015,
	ERR_mibi_add_product_error			= 50030,
	ERR_mibi_not_enough_money			= 50105,
	ERR_mibi_exceed_month_consume_limit	= 50107,
	ERR_mibi_exceed_per_consume_limit	= 50108,

	ERR_uid_not_exist_or_usr_alrdy_have	= 51001,
	ERR_magic_code_not_exist			= 51002,
	ERR_magic_code_not_active			= 51003,
	ERR_magic_code_out_of_date			= 51004,
	ERR_magic_code_frozen				= 51005,
	ERR_magic_code_already_used2		= 51006,
	ERR_magic_code_location_err			= 51100,

	//for gold bean server: start 52000
	ERR_gold_bean_inquire_max_count_exceeded = 52009,
	ERR_gold_bean_bad_product_id		= 52010,

	ERR_cannot_change_oth_in_this_map	= 100001,
	ERR_max_submit_magic_time_exceed	= 100002,
	ERR_his_count_limited				= 100003,
	ERR_already_get_smc_salary			= 100004,
    ERR_class_not_vip_ambassador		= 100005,
    ERR_not_class_member				= 100006,
    ERR_already_vip						= 100007,
    ERR_already_get_reward				= 100008,
    ERR_pet_infected_cannot_feed		= 100009,
    ERR_get_500_xiaomee_time_limit		= 100012,
    ERR_sickbed_already_sit				= 100013,
    ERR_sickbed_not_sit					= 100014,
    ERR_sickbed_already_occupied		= 100015,
    ERR_doctor_duty_max					= 100016,
    ERR_lahm_no_sick					= 100017,
    ERR_medicine_wrong					= 100018,
    ERR_boss_blood_up_again				= 100019,
    ERR_sports_not_sign					= 100020,
    ERR_lottery_too_much				= 100021,
    ERR_sports_encourage				= 100022,
    ERR_sports_no_task					= 100023,
    ERR_already_get_item				= 100024,
    ERR_sports_reward					= 100025,
    ERR_sports_no_medal					= 100026,
    ERR_sports_chance_cycle				= 100027,
    ERR_sports_reach_max				= 100028,
    ERR_sports_already_chance_cycle		= 100029,
    ERR_pk_too_many_times				= 100030,
    ERR_fire_cup_card_too_many			= 100031,
    ERR_happy_card_not_match			= 100032,
    ERR_edit_lahm_diary_too_many		= 100033,
    ERR_cannot_buy_itm					= 100034,
    ERR_do_work_too_many				= 100035,
    ERR_already_get_work_salary			= 100036,
    ERR_already_engineer_or_cannot		= 100037,
    ERR_db_return_timeout				= 100038,
    ERR_send_diary_flower_too_many		= 100039,
    ERR_pk_level_unreached				= 100040,
    ERR_ggj_exchg_too_many				= 100041,
    ERR_get_sheep_too_many				= 100042,
    ERR_pet_already_in_home				= 100046,
    ERR_send_pet_home_too_many			= 100047,
    ERR_send_gift_too_many				= 100048,
    ERR_get_too_many_tangguo_today		= 100049,

    ERR_reach_addscore_limit  			= 100050,
    ERR_race_user_not_sign				= 100053,
    ERR_max_num_in_ship					= 100054,
    ERR_not_enough_yinghuocao			= 100055,
    ERR_cannot_invite_to_this_map		= 100056,
    ERR_not_driver						= 100057,
    ERR_already_get_special_award		= 100058,

    ERR_already_get_xiaome				= 100059,
    ERR_score_not_reach					= 100060,
    ERR_you_are_not_leader				= 100061,
    ERR_already_get_daoju 				= 100062,
    Err_already_get_aword				= 100063,
    ERR_race_not_have_key				= 100064,
    ERR_not_drive_zhaijisong			= 100065,
    ERR_no_super_lahm					= 100066,

    ERR_candy_no_sit					= 100067,
    ERR_candy_already_sit				= 100068,
    ERR_candy_wrong_id					= 100069,
    ERR_candy_not_ok					= 100070,
    ERR_candy_reach_limit				= 100071,
    ERR_candy_only_self					= 100072,
    ERR_candy_reach_limit_recy			= 100073,
    ERR_candy_nothing					= 100074,
    ERR_candy_total_count_reach			= 100075,
    ERR_candy_invaild_id				= 100076,
    ERR_candy_invaild_sit				= 100077,
    ERR_alrdy_wenjuan					= 100078,
    ERR_super_lahm_on_trust				= 100079,

	ERR_pasture_no_nets 				= 100080,
    ERR_pasture_nets_old				= 100081,
    ERR_pasture_already_have_nets 		=100082,

	ERR_too_few_to_exchange				=100083,
	ERR_change_self_error				= 100084,
	ERR_that_user_candy_not_enough		= 100085,
	ERR_get_candy_othr_too_many			= 100086,
	ERR_you_already_sub_vip				= 100087,
	ERR_vip_bu_gou_le					= 100088,
	ERR_user_not_online					= 100089,
	ERR_vip_work_reach_limit			= 100090,

	ERR_invalid_npc_type				= 100091,
	ERR_query_count_too_many			= 100092,
	ERR_already_has_one_of_two			= 100093,
	ERR_invalid_item_id					= 100094,

	ERR_zhongzhi_level_not_enough		= 100095,
	ERR_yangzhi_level_not_enough		= 100096,
	ERR_this_scene_closed				= 100097,
	ERR_you_are_too_greedy				= 100098,
	ERR_pls_tack_your_yang				= 100099,
	ERR_invalid_op						= 100100,
	ERR_reach_total_limit				= 100101,
	ERR_reach_geren_limit				= 100102,
	ERR_not_rabit						= 100103,
	ERR_already_have_egg				= 100104,
	ERR_rabit_too_little				= 100105,
	ERR_egg_is_not_yours				= 100106,
	ERR_vip_npc_task					= 100107,
	ERR_candy_no_sit_ex					= 100108,
	ERR_already_cut						= 100109,
	ERR_candy_no_sit_ex_ex				= 100110,
	ERR_this_pos_no_cake				= 100111,
	ERR_fishing_reach_day_limit			= 100112,
	ERR_add_npc_lovely_limit			= 100113,
	ERR_get_too_many_money_today		= 100114,
	ERR_alrdy_sell_to_other				= 100115,
	ERR_get_too_many_fish_from_other	= 100116,
	ERR_be_seller_too_many_times		= 100117,
	ERR_get_chris_gift_too_many_times	= 100118,
	ERR_chris_gift_alrdy_collected		= 100119,
	ERR_collect_too_many_chris_gift		= 100120,
	ERR_alrdy_fetch_this_jiaoyin		= 100121,
	ERR_super_lahm_on_task				= 100122,
	ERR_get_too_many_item_today	    	= 100123,
	ERR_pay_five_times_today			= 100124,
	ERR_not_fini_pre_taskid				= 100125,
	ERR_not_have_garage					= 100126,
	ERR_carry_rock_many_today			= 100127,
	ERR_have_not_fini_task				= 100128,
	ERR_egg_hatched_today				= 100129,
	ERR_you_have_change					= 100130,
	ERR_record_count_limited			= 100131,
	ERR_the_lahm_not_followed			= 100132,
	ERR_the_lahm_level_too_low			= 100133,
	ERR_lahm_basic_attire_not_enough	= 100134,
	ERR_lahm_have_not_this_skill		= 100135,
	ERR_want_set_task_state_0			= 100136,
	ERR_action_have_fini				= 100137,
	ERR_pls_tack_your_cow				= 100138,
	ERR_already_get_cow_milk			= 100139,
	ERR_reach_day_limit					= 100140,
	ERR_pls_tack_your_tiger				= 100141,
	ERR_already_get_tiger				= 100142,
	ERR_not_build_all_temple			= 100143,
	ERR_not_study_basic_skills			= 100144,
	ERR_you_have_get_skill_libao		= 100145,
	ERR_five_stage_cannot_clothe		= 100146,
	ERR_the_action_not_fit				= 100147,
	ERR_not_water_lahm					= 100148,
	ERR_not_fin_your_profession			= 100149,
	ERR_lahm_is_sick					= 100150,
	ERR_the_pet_is_super_lamu			= 100151,
	ERR_reach_week_limit				= 100152,
	ERR_have_got_five_stage_libo		= 100153,
	ERR_have_not_right_to_do			= 100154,
	ERR_have_catch_enough				= 100155,
	ERR_you_have_chose_wo_wei_hui		= 100156,
	ERR_not_chose_wo_wei_hui			= 100157,
	ERR_add_score_many_times			= 100158,
	ERR_out_of_times_get_yingtao		= 100159,
	ERR_not_enough_yumi					= 100160,
	ERR_have_fini_tiyan_dinig_room		= 100161,
	ERR_tantalize_cat_many_times		= 100162,
	ERR_have_get_sth_reputation			= 100163,
	ERR_have_not_choose_team			= 100164,
	ERR_already_get_the_prize			= 100165,
	ERR_lahm_is_feel_bad				= 100166,
	ERR_already_get_pig					= 100167,
	ERR_lahm_not_dead_seven_day			= 100168,
	ERR_can_not_be_delete				= 100168,
	ERR_employee_count_limit			= 12641,
	ERR_aready_have_certify				= 100170,
	ERR_no_more_certify_today			= 100171,
	ERR_not_have_certify				= 100172,
	ERR_not_open_time					= 100173,
	ERR_room_id_not_exist				= 100174,
	ERR_not_resturant_type				= 100175,
	ERR_already_create_shop				= 100176,
	ERR_have_no_shop					= 100177,
	ERR_the_shop_not_yours				= 100178,
	ERR_lahm_is_employed				= 100179,
	ERR_shop_level_too_low				= 100180,
	ERR_shop_location_limit				= 100181,
	ERR_restuarant_closed				= 100182,
	ERR_food_item_locked				= 100183,
	ERR_not_select_team					= 100184,
	ERR_style_item_err					= 100185,
	ERR_style_item_locked				= 100186,
	ERR_already_select_team				= 100187,
	ERR_have_get_shop_packge			= 100188,
	ERR_use_tick_today					= 100189,
	ERR_not_recv_task					= 100190,
	ERR_not_have_item_16011 			= 100191,
	ERR_not_enough_dish_to_sell 		= 100192,
	ERR_no_dish_to_sell					= 100193,
	ERR_not_have_data					= 100194,
	ERR_have_get_card_from_tulin		= 100195,
	ERR_have_get_dragon_egg				= 100196,
	ERR_have_got_day_limit				= 100197,
	ERR_not_enough_medal				= 100198,
	ERR_invalid_game_id					= 100199,
	ERR_invalid_raceid					= 100200,
	ERR_friendbox_item_limit			= 100201,
	ERR_have_get_open_school_gift		= 100202,
	ERR_mole_level_not_enough			= 100203,
	ERR_the_classroom_not_yours			= 100204,
	ERR_can_not_sell_too_many			= 100205,
	ERR_the_classroom_grade_error		= 100206,
	ERR_not_vip							= 100207,
	ERR_lahm_classroom_closed			= 100208,
	ERR_too_much_to_exchange			= 100209,
	ERR_snowball_war_player_max			= 100210,
	ERR_snowball_war_pos_aready_ocupy	= 100211,
	ERR_mibi_count						= 100212,
	ERR_christmas_sock_add_max_today	= 100213,
	ERR_christmas_sock_get_yestoday_bonus_first	= 100214,
	ERR_christmas_sock_cannot_get_bonus	= 100215,
	ERR_farm_animal_get_map_pos_egg_over	= 100216,
	ERR_have_add_angle					= 100217,
	ERR_dungeon_have_not_begin			= 100218,
	ERR_dungeon_invalid_map				= 100219,
	ERR_dungeon_invalid_area			= 100220,
	ERR_dungeon_not_enough_hp			= 100221,
	ERR_dungeon_has_been_picked 		= 100222,
	ERR_dungeon_has_been_explored		= 100223,
	ERR_dungeon_no_item_to_pick			= 100224,
	ERR_dungeon_invalid_boss			= 100225,
	ERR_compose_angel_id_wrong			= 100226,
	ERR_dungeon_friend_count_too_large	= 100227,

	//Anger Lahm
	ERR_al_invalid_task_id				= 100228,

	//hide and seek
	ERR_has_the_queue_is_full			= 100229,
	ERR_has_invalid_map					= 100230,
	ERR_has_invalid_force				= 100231,
	ERR_has_have_not_begin				= 100232,
	ERR_has_you_are_busy_now			= 100233,
	ERR_has_you_get_too_much			= 100234,

	ERR_answer_tv_time_wrong			= 100235,
	ERR_af_limit_fight_level			= 100236,
	ERR_af_fight_id_wrong				= 100237,
	ERR_af_can_not_get_prize			= 100238,
	ERR_af_deal_event_wrong				= 100239,
	ERR_af_fight_task_wrong				= 100240,
	ERR_af_master_apprentice_wrong		= 100241,

	ERR_fb_invalid_cmd					= 100242,
	ERR_fb_enter_turret_failed			= 100243,
	ERR_fb_boss_not_alive				= 100244,
	ERR_del_mul_friend_error			= 100245,

	ERR_swap_mooncake_state_err			= 100246,
	ERR_swap_mooncake_not_enough		= 100247,
	//cute pig
	ERR_cutepig_raise_invalid_pig		= 100248,
	ERR_cutepig_invalid_item			= 100249,
	ERR_cutepig_invalid_task			= 100250,
	ERR_cutepig_invalid_process			= 100251,
	ERR_cutepig_invalid_map				= 100252,
	ERR_cutepig_can_not_buy_item		= 100253,

	ERR_get_ticket_time_err				= 100254,
	ERR_have_got_ticket_err				= 100255,
	ERR_is_open_prize_time				= 100256,
	ERR_add_friend_req_failed			= 100257,
	ERR_add_friend_rep_invalid			= 100256,
	ERR_cutepig_exchange_pig_failed		= 100257,
	ERR_not_super_guider 				= 100258,
	ERR_sg_has_got_the_prize 			= 100259,
	ERR_sg_not_enough_madel_count 		= 100260,

	ERR_cp_show_invalid_info			= 100261,
	ERR_cp_follow_invalid_pig			= 100262,
	ERR_cp_beauty_game_time_err			= 100263,
	ERR_cp_has_get_beauty_gift			= 100264,
	ERR_cp_use_npc_pig_limit			= 100265,
	ERR_cp_npc_weight_pk_invalid_time	= 100266,
	ERR_cp_ie_invalid_exchange_id		= 100267,
	ERR_cp_ie_you_had_got_one			= 100268,
	ERR_td_can_not_get_award			= 100269,
	ERR_mine_map_chance_limit_max		= 100270,
	ERR_chp_invalid_cmd					= 100271,

	ERR_wvs_invalid_player_id			= 100272,
	ERR_wvs_invalid_team_id				= 100273,
	ERR_wvs_hadnt_join_a_team			= 100274,
	ERR_wvs_switch_same_team			= 100275,
	ERR_donate_food_item_limit			= 100276,
	ERR_guess_item_price_time_wrong		= 100277,
	ERR_ld_tar_invalid_attack			= 100278,

	ERR_weekend_login_award_over		= 100279,

	ERR_not_valid_map					= 100280,
	ERR_fp_set_flower_failed			= 100281,
	ERR_fp_get_flower_failed			= 100282,
	ERR_fp_vip_flower_was_already_open	= 100283,
	ERR_fp_can_not_get_award			= 100284,

	ERR_invalid_player					= 100285,
	ERR_invalid_team					= 100286,
	ERR_ac_invalid_sit					= 100287,
	ERR_ac_invalid_op					= 100288,
	ERR_ac_invalid_interactive_tar		= 100289,
	ERR_week_activity_speech_aready_begin		= 100290,
	ERR_week_activity_speech_canmot_continuous		= 100291,
	ERR_cutepig_guide_step_invalid_pig  = 100292,
	ERR_order_cannot_receive_now        = 100293,
	ERR_order_cannot_finish_now         = 100294,
	ERR_use_acc_machine_tool            = 100295,
	ERR_day_limit_vote                  = 100296,
	ERR_save_birthday_decorate          = 100297,
	ERR_use_glue_to_machine_day_limit   = 100298,
	ERR_use_not_enter_lahm_sport_mvp_team   = 100299,
	ERR_use_not_enter_lahm_sport_team       =  100300,
	ERR_change_lahm_sport_need_bean         =  100301,
	ERR_get_lahm_sport_team_clothes         =  100302,
	ERR_too_many_member                     =  100303,
	ERR_mvp_team_not_exist                   =  100304,
	ERR_day_limit_catch_butterfly            =  100305,
	ERR_cheer_team_user_act_error            =  100306,
	ERR_cheer_team_pos_user_exist            =  100307,
	ERR_cheer_team_user_in_other_pos        =  100308,
	ERR_mvp_team_user_not_exist_yet         =  100309,
	ERR_piglet_rush_acitvity_day            =  100310,
	ERR_miss_note_acitvity_max_day          =  100311,
	ERR_expedion_unlock_state               =  100312,
	ERR_expedion_already_got_state          =  100313,
	ERR_get_friend_not_exist_user			=  100314,
};


typedef struct protocol {
	uint32_t	len;
	uint8_t		ver;
	uint32_t	cmd;
	userid_t	id;
	uint32_t	ret;
	uint8_t		body[];
} __attribute__((packed)) protocol_t;

enum {
	PROTO_UPDATE_STAMP						= 7,

	PROTO_VERSION							= 200,
	PROTO_LOGIN								= 201,
	PROTO_LOGOUT							= 202,
	PROTO_USER_SIMPLY						= 203,
	PROTO_USER_DETAIL						= 204,
	PROTO_ACCOUNT_INFO						= 205,
	PROTO_GROUP_INFO						= 206,
	PROTO_FRIEND_INFO						= 207,
	PROTO_NICK_MODIFY						= 208,
	PROTO_COLOR_MODIFY						= 209,
	PROTO_SET_STAGE							= 210,

	PROTO_BUY_PET							= 211,
	PROTO_GET_PET							= 212,
	//PROTO_DISCARD_PET						= 213,
	PROTO_GET_PET_CNT						= 214,
	PROTO_FOLLOW_PET						= 215,
	PROTO_SET_PET_NICK						= 218,
	PROTO_PLAY_WITH_PET						= 219,
	PROTO_SET_PET_POS						= 220,
	PROTO_PET_DISAPPEAR_NOTI				= 221,
	PROTO_PET_TRUST							= 222,
	PROTO_PET_WITHDRAW						= 223,
	PROTO_GET_TRUSTED_PET					= 224,
	PROTO_PICK_PET_TASK						= 225,
	PROTO_TERMINATE_PET_TASK				= 226,
	PROTO_SET_PET_TASK_FIN					= 227,
	PROTO_GET_PET_TASK_LIST					= 228,
	PROTO_GET_PETS_ON_TASK					= 229,
	PROTO_ASK_FOR_A_PET						= 230,
	PROTO_BECOME_SUPER_LAHM					= 231,
	PROTO_HAS_SUPER_LAHM					= 232,
	PROTO_GET_OUTSIDE_PETS					= 233,
	PROTO_REP_OUTSIDE_PETS					= 234,
	PROTO_FOLLOW_OUTSIDE_PET				= 235,
	PROTO_LEARN_PET_SKILL					= 236,
	PROTO_PET_HOUSEWORK						= 237,
	PROTO_MODIFY_PET_COLOR					= 238,
	PROTO_SEND_PET_HOME						= 239,
	PROTO_GET_ALL_PET_INFO					= 240,

	PROTO_GET_TASK							= 216,
	PROTO_SET_TASK							= 217,

	PROTO_SEARCH_USER						= 241,

	PROTO_GET_TASK_TMP_INFO					= 242,
	PROTO_SET_TASK_TMP_INFO					= 243,
	PROTO_GET_TASK_INFO 					= 244,
	PROTO_SET_TASK_INFO 					= 245,
	PROTO_DEL_TASK_INFO 					= 246,

	PROTO_GET_PET_TASK_ATTIRE_INFO			= 248,

	PROTO_CALL_SUPER_LAHM 					= 251,

	PROTO_ACTION2							= 301,
	PROTO_TALK								= 302,
	PROTO_WALK								= 303,
	PROTO_PAOPAO							= 304,
	PROTO_ACTION							= 305,
	PROTO_THROW								= 306,
	// 307 not used now
	PROTO_FIND_ITEM							= 308,
	PROTO_EXCHANGE_ITEM						= 309, // exchange item and set task
	PROTO_JAIL_USER							= 310,
	PROTO_POST_MESSAGE						= 311,
	PROTO_POST_BLACKBOARD					= 312,
	PROTO_GET_BLACKBOARD					= 313,
	PROTO_FIND_BLACKBOARD					= 314,
	PROTO_SPECIAL_ACTION					= 315,
	PROTO_ADD_FLOWER_TO_BLKBOARD			= 316,

	PROTO_RAND_ITEM_INFO					= 317,
	PROTO_COLLECT_ITEM						= 318,
	PROTO_EXCHG_COLLECTED_ITEM				= 319,
	PROTO_DEMAND_RANDOM_ITEM				= 320,
	PROTO_EXCHG_ITEM2						= 321, // exchange item but not set task

	PROTO_GET_PASSED_BOARD_MSG_BY_ID		= 322, // get passed messages by user id

	PROTO_MAP_ENTER							= 401,
	PROTO_MAP_LEAVE							= 402,
	PROTO_GAME_ENTER						= 403,
	PROTO_GAME_LEAVE						= 404,
	PROTO_USER_LIST							= 405,
	PROTO_MAP_INFO							= 406,
	PROTO_GAME_SCORE						= 407,
	PROTO_MAPS_USER							= 408,
	PROTO_USER_HOME							= 409,
	PROTO_SET_HOME_ITEM						= 410,
	PROTO_GET_HOME_LIST 					= 412,
	PROTO_GET_VIP_HOME_LIST					= 413,
	PROTO_GET_RECENT_SPRITE					= 414,
	PROTO_RESET_HOME						= 415,

	PROTO_GAMEGRP_INFO						= 421,
	PROTO_MAPGAME_INFO						= 422,
	PROTO_SIGPL_GAME_MSG					= 423,
	PROTO_OTHERS_GAME_STATUS				= 424,
	PROTO_SCENEGAME_INFO					= 425,

	PROTO_GET_SESSION						= 426,

	// CMDID 431 to 460 Reserved For Scene Game Info Interchange
	PROTO_FOOTBALL_TEAM_NO					= 431,
	PROTO_FOOTBALL_GOAL						= 432,
	PROTO_FOOTBALL_BALL_STATUS				= 433,
	PROTO_FOOTBALL_1MIN_REMAIN				= 434,
	PROTO_FOOTBALL_PLAYER_ACTION			= 435,

    //For cutting picture
	PROTO_PIC_SRV_IP_PORT					= 461,
	PROTO_TUYA_SRV_IP_PORT					= 462,

	PROTO_ITEM_BUY							= 501,
	PROTO_MAP_ITEM_USE						= 502,
	PROTO_USER_ITEM_USE						= 503,
	PROTO_SHOW_ITEM_USE						= 504,
	PROTO_FEED_ITEM_USE						= 505,
	PROTO_FUNC_ITEM_USE						= 506,
	PROTO_ITEM_LIST							= 507,
	PROTO_ITEM_DISCARD						= 508,
	//PROTO_ITEM_XML						= 509,
	PROTO_ITEM_SHOW_EXPIRE					= 510,
	PROTO_CHK_ITEM_AMOUNT					= 511,
	PROTO_ASK_FOR_ITMES						= 512,
	PROTO_BARGAINING						= 513,
	PROTO_SELL_ITMES						= 514,
	PROTO_COMPOUND_ITEM						= 515,
	PROTO_CANCEL_CHANGE_FORM				= 516,

	PROTO_GET_ITEM_ARRAY					= 517,
	//580 has been used in TW_VER
	//CMDID 580 as send message log
	//only for TW_VER
	PROTO_AS_SEND_MSGLOG		= 580, 
	//End
	PROTO_FRIEND_INVITE		= 601,
	PROTO_RSP_FRIEND_INVITE	= 602,
	PROTO_FRIEND_ADD		= 603,
	PROTO_RSP_FRIEND_ADD	= 604,
	PROTO_FRIEND_DEL		= 605,
	PROTO_GET_FRIENDS		= 606,
	PROTO_BKLIST_ADD		= 607,
	PROTO_BKLIST_DEL		= 608,
	PROTO_BKLIST_GET		= 609,
	PROTO_USERS_ONLINE		= 610,
	PROTO_CHK_JY_PET_CHANGE		= 611,
	PROTO_DEL_MUL_FRIENDS	= 612,

	// For Mole Messenger
	PROTO_MSNGER_INVITEE_CHK_IN				= 701,
	PROTO_CHK_MSNGER_INFO					= 702,

	// For Wishing Pool
	PROTO_MAKE_WISH							= 751,
	PROTO_FETCH_REALIZED_WISH 				= 752,
	PROTO_GET_REALIZED_WISHES				= 753,
	PROTO_CHK_IF_WISH_MADE					= 754,

	// For Message System
	PROTO_SEND_POSTCARD						= 801,
	PROTO_GET_POSTCARDS						= 802,
	PROTO_READ_POSTCARD						= 803,
	PROTO_DEL_POSTCARD						= 804,
	PROTO_GET_UNREAD_CARD_NUM				= 805,
	PROTO_DEL_ALL_POSTCARD					= 806,
	PROTO_GET_NEW_CARD_EXP					= 807,
	PROTO_SET_POSTCARD_GIFT_FLAG			= 808,
	PROTO_GET_POSTCARDS_BASE_INFO			= 809,
	PROTO_DEL_POSTCARDS						= 810,

	PROTO_GET_HOME_HOT						= 901,
	PROTO_GET_TOP_HOMES						= 902,
	PROTO_VOTE_HOME							= 903,
	PROTO_GET_RECENT_VISITORS				= 904,
	PROTO_GET_HOME_NEIGHBOR					= 905,
	PROTO_ADD_HOME_NEIGHBOR					= 906,
	PROTO_LEAVE_HOME_MESSAGE				= 907,
	PROTO_VERIFY_HOME_MESSAGE				= 908,
	PROTO_GET_HOME_MESSAGES					= 909,
	PROTO_DEL_HOME_MESSAGE 					= 910,
	PROTO_REPLY_HOME_MESSAGE				= 911,
	PROTO_GET_MM_TREE_INFO					= 912,
	PROTO_FERTILIZE_WATER_TREE 				= 913,
	PROTO_GET_FERTILIZED_MAN_LIST			= 914,
	PROTO_PICK_FRUIT						= 915,
	PROTO_GET_HOME_TYPE_LIST				= 916,
	PROTO_GET_SMC_INFO						= 917,
	PROTO_VOTE_SMC							= 918,
	PROTO_GET_ARCHITECT_EXP					= 919,

	// for birthday airship
	PROTO_SET_BIRTHDAY						= 951,
	PROTO_DRESS_BIRTHDAY_CLOTHES			= 952,
	PROTO_BIRTHDAY_CLOTHES_EXPIRE			= 953,

	//PROTO_SET_CHRISTMAS_WISH 				= 961,
	//PROTO_IS_SET_CHRISTMAS_WISH			= 962,
	//PROTO_GET_CHRISTMAS_WISH				= 963,
	//PROTO_IS_GET_CHRISTMAS_WISH			= 964,
	//PROTO_DRESS_CHRISTMAS_HAT				= 965,
	//PROTO_UNDRESS_CHRISTMAS_HAT			= 966,
	PROTO_SET_YUANXIAO_WISH					= 967,
	PROTO_GET_YUANXIAO_WISH					= 968,
	// for mole party
	PROTO_GET_MOLE_PARTY					= 971,
	PROTO_SET_MOLE_PARTY					= 972,
	PROTO_GET_OWN_MOLE_PARTY				= 973,
	PROTO_GET_PARTY_COUNT					= 974,

	// for spring festival
	PROTO_SET_SPRING_MSG					= 981,
	PROTO_GET_SPRING_MSG					= 982,
	PROTO_GET_YUANBAO_VALUE					= 983,
	PROTO_CHANGE_YUANBAO_VALUE				= 984,
	PROTO_SET_YUANBAO_BOX_FLAG				= 985,
	PROTO_BUY_ITEM_USE_YUANBAO				= 986,
	PROTO_GET_FU_INFO						= 990,
	PROTO_SEND_FU							= 991,
	PROTO_LIST_FU_SENT						= 992,
	PROTO_PET_CHANGE_FORM					= 993,
	PROTO_PET_RECOVER_FORM					= 994,

	PROTO_GET_MAP_BUILDING_INFO 			= 995,
	PROTO_CREATE_BULDING					= 996,
	PROTO_GET_BUILDING_CERTIFY				= 997,
	PROTO_CHANGE_BUILDING_NAME				= 998,
	PROTO_CHANGE_BUILDING_STYLE 			= 1049,
	PROTO_GET_MAP_LAST_GRID 				= 1000,

	//for group
	PROTO_CREATE_MOLEGROUP					= 1001,
	PROTO_INVITE_JOIN_MOLEGROUP				= 1002,
	PROTO_APPLY_FOR_JOINING_MOLEGROUP 		= 1003,
	PROTO_REPLY_TO_JOIN_MOLEGROUP			= 1004,
	PROTO_LEAVE_MOLEGROUP					= 1005,
	PROTO_KICK_OUT_OF_MOLEGROUP				= 1006,
	PROTO_DEL_MOLEGROUP						= 1007,
	PROTO_MOD_MOLEGROUP_INFO				= 1008,
	PROTO_CHAT_IN_MOLEGROUP 				= 1009,
	PROTO_GET_USER_MOLEGROUP_LIST 			= 1010,
	PROTO_GET_MOLEGROUP_DETAIL				= 1011,
	PROTO_SET_MOLEGROUP_FLAG				= 1012,

	PROTO_SET_BUILDING_INNER_STYLE			= 1013,
	PROTO_GET_SHOP_INFO						= 1014,
	PROTO_ADD_EMPLOYEE						= 1015,
	PROTO_DEL_EMPLOYEE						= 1016,
	PROTO_COOKING_DISH						= 1017,
	PROTO_EAT_DISH							= 1018,
	PROTO_CLEAN_DISH						= 1019,
	PROTO_CHANGE_DISH_STATE					= 1020,
	PROTO_PUT_DISH_IN_BOX					= 1021,
	PROTO_LIST_EMPLOYEE						= 1022,
	PROTO_GET_SHOP_BOARD_INFO				= 1023,
	PROTO_GET_SHOP_ITEM_INFO				= 1024,
	PROTO_SUB_EVALUATE						= 1025,
	PROTO_NOTIFY_SHOP_INFO_CHANGE			= 1026,
	PROTO_GET_SHOP_COUNT					= 1027,
	PROTO_GET_PET_EMPLOYED_LIST				= 1028,
	PROTO_NOTIFY_HONOR_ID					= 1029,
	PROTO_GET_SHOP_HONOR_LIST				= 1030,
	PROTO_GET_SHOP_EVENT					= 1031,
	PROTO_SET_SHOP_EVENT					= 1032,
	PROTO_GET_USER_IN_MAP					= 1033,
	PROTO_GET_FRIEND_SHOP_INFO				= 1034,
	PROTO_GIVE_NPC_DISHS					= 1035,
	PROTO_GET_SPORT_TEAM_PRIZE				= 1036,
	PROTO_GET_SPORT_PERSON_PRIZE			= 1037,
	PROTO_GET_SPORT_TEAM_RANK				= 1038,
	PROTO_GET_DISH_LEVEL_COUNT				= 1039,
	PROTO_GET_DISH_COUNT					= 1040,
	PROTO_GET_TYPE_DATA 					= 1041,
	PROTO_SET_TYPE_DATA						= 1042,
	PROTO_GET_EAT_DISH_RAND 				= 1043,
	PROTO_SET_MAGIC_MAN_EAT_DISH 			= 1044,
	PROTO_GET_SHOP_ITEM_LOCK				= 1045,
	PROTO_GET_DISH_LEVEL_COUNT_ALL			= 1046,
	PROTO_NOTIFY_DISH_LEVEL_UP				= 1047,

	PROTO_GET_CLIENT_OBJ					= 1098,
	PROTO_SET_CLIENT_OBJ					= 1099,
	PROTO_NOTIFY_FRIEND_USER_LOGIN			= 1100,

	PROTO_GET_QUES_ANSWER					= 1110,
	PROTO_INC_RUN_TIMES						= 1111,
	PROTO_GET_QUES_NUM						= 1112,
	PROTO_GET_RUN_TIMES						= 1113,
	PROTO_LAHM_PLAY							= 1114,
	//PROTO_SEND_RAND_ITEM					= 1115,
	PROTO_DISCOVER_RAND_ITEM				= 1116,
	PROTO_SEND_RAND_ITEM					= 1117,
	PROTO_RAND_ITEM_SWAP					= 1118,

	PROTO_GET_PET_ITEM_COUNT				= 1120,
	PROTO_PET_CLOTHES_USE					= 1121,
	PROTO_PET_HONOR_USE						= 1122,
	PROTO_BUY_PET_ITEM						= 1123,
	PROTO_GET_PET_ITEMS_CNT					= 1124,
	PROTO_SUPER_LAMU_GET_ZHANPAO			= 1125,
	PROTO_ADD_LAMU_GROWTH					= 1126,
	PROTO_SET_PET_SKILL_EX 					= 1127,
	PROTO_EXCHANGE_RAND_ITEM				= 1128,

	PROTO_DONATE_ITEM_CHARITY_DRIVE			= 1129,
	PROTO_QUERY_CHARITY_DRIVE_TYPE_RANK		= 1130,
	PROTO_QUERY_CHARITY_DRIVE_INFO			= 1131,
	PROTO_GET_JY_ANIMAL_ITEMS				= 1192,

	PROTO_GET_DEPOSIT_INFO					= 1201,
	PROTO_DEPOSIT_BEAN						= 1202,
	PROTO_DRAW_BEAN							= 1203,

	PROTO_GET_LAHM_TASK_STATE				= 1204,
	PROTO_SET_LAHM_TASK_STATE				= 1205,
	PROTO_GET_LAHM_TASK_DATA				= 1206,
	PROTO_SET_LAHM_TASK_DATA				= 1207,
	PROTO_BUILD_LAHM_TEMPLE					= 1208,
	PROTO_GET_ITEM_BY_USE_SKILL				= 1209,
	PROTO_GET_BUILD_TEMPLE					= 1210,
	PROTO_GET_LAMU_ONE_STAT_TASK			= 1211,
	PROTO_LAMU_ACTION						= 1212,
	PROTO_GET_PET_TASK_LIST_EX				= 1213,
	PROTO_REMOVE_PET_ACTION					= 1214,

	PROTO_GET_USER_DATA						= 1215,
	PROTO_SET_USER_DATA						= 1216,
	PROTO_SET_PET_SKILL_TYPE				= 1217,
	PROTO_SET_PET_HOT_SKILL					= 1218,
	PROTO_PET_HELP_CIWEI					= 1219,
	PROTO_PET_USE_SKILL_DO_WORK				= 1220,
	PROTO_FIX_CARD_GAME_BUG					= 1221,
	PROTO_GET_SIG_CARD_FROM_NPC				= 1222,
	PROTO_CHECK_HAVE_BASIC_CARD				= 1223,
	PROTO_GET_BUSINESSMAN_GOODS				= 1224,
	PROTO_BUY_BUSINESSMAN_GOODS				= 1225,
	PROTO_GET_DRAGON_EGG_BY_TASK			= 1226,
	PROTO_GET_DRAGON_BAG					= 1227,
	PROTO_CALL_DRAGON						= 1228,
	PROTO_RELEASE_DRAGON					= 1229,
	PROTO_SET_DRAGON_NAME					= 1230,
	PROTO_FEED_DRAGON						= 1231,
	PROTO_EXCHANGE_ONE_TO_ANOTHER			= 1232,
	PROTO_BROOD_DRAGON_EGG					= 1233,
	PROTO_QUICK_BROOD_DRAGON_EGG			= 1234,
	PROTO_TAKE_LITTLE_DRAGON				= 1235,
	PROTO_GET_PIG							= 1236,
	PROTO_GET_BROOD_EGG_TIME				= 1237,
	PROTO_GET_CALLED_DRAGON_INFO			= 1238,
	PROTO_GET_NEW_CARD_FLAG					= 1239,
	PROTO_SIGN_FIRE_CUP						= 1240,
	PROTO_SIGN_GET_TEAM_ID					= 1241,
	PROTO_ADD_STH_RAND						= 1242,
	PROTO_EXCHANGE_STH						= 1243,
	PROTO_GET_FIRE_CUP_TEAM_MEDAL			= 1244,
	PROTO_GET_FIRE_CUP_SPRITE_MEDAL_NUM		= 1245,
	PROTO_FIRE_CUP_ADD_GAME_BONUS			= 1246,
	PROTO_ADD_ITEM_IN_FRIENDSHIP_BOX		= 1247,
	PROTO_GET_ITEM_FROM_FRIENDSHIP_BOX		= 1248,
	PROTO_QUERY_ITEMS_FROM_FRIENDSHIP_BOX		= 1249,
	PROTO_GET_ITEM_HISTORY_FROM_FRIENDSHIP_BOX	= 1250,
	PROTO_RACE_GAIN_RACE_BONUS				= 1251,
	PROTO_EXPAND_DRAGON_BAG					= 1252,
	PROTO_SEND_BUBBLE_MSG					= 1253,
	PROTO_GET_OPEN_SCHOOL_GIFT				= 1254,
	PROTO_GET_LAHM_COUNT_ACCORD_STATE		= 1255,
	//PROTO_DEL_DEAD_LAHM					= 1256,
	PROTO_BUY_ITEM_BY_BEANS					= 1256,
	PROTO_DEL_PET_DEAD						= 1257,
	PROTO_ADD_TEMP_ITEM 					= 1258,
	PROTO_LOOK_STH_CNT 						= 1259,
	PROTO_GUESS_ITEM_PRICE_CHARITY_DRIVE	= 1260,
	PROTO_QUERY_GUESS_ITEM_CHARITY_DRIVE	= 1261,
	PROTO_GUESS_ITEM_END_CHARITY_DRIVE		= 1262,

    //lahm classroom
    PROTO_GET_LAHM_CLASSROOM_INNER_STYLE_LIST	= 1263,
    PROTO_GET_LAHM_CLASSROOM_HONOR_LIST		= 1264,
    PROTO_NOTIFY_LAHM_CLASSROOM_HONOR_ID	= 1265,
    PROTO_ANSWER_LAHM_CLASSROOM_LESSON_QUESTION	= 1266,
    PROTO_ADD_LAHM_CLASSROOM_GAME_SCORE		= 1267,
    PROTO_GET_LAHM_CLASSROOM_FRIEND_INFO	= 1268,
	PROTO_GET_USE_PROFESSION_EXP_LEVEL		= 1269,
    PROTO_USE_LAHM_CLASSROOM_ITEMS			= 1270,
    PROTO_GET_LAHM_CLASSROOM_ITEMS			= 1271,
    PROTO_GET_LAHM_CLASSROOM_TEACH_REM		= 1272,
    PROTO_LAHM_CLASSROOM_CLASS_GRADUATE		= 1273,
    PROTO_BUILD_LAHM_CLASSROOM				= 1274,
	PROTO_GET_LAHM_CLASSROOM_LAST_GRID		= 1275,
	PROTO_GET_LAHM_CLASSROOM_GRID_INFO		= 1276,
	PROTO_SET_LAHM_CLASSROOM_NAME			= 1277,
	PROTO_SET_LAHM_CLASSROOM_INNER_STYLE	= 1278,
	PROTO_GET_USER_LAHM_CLASSROOM_INFO		= 1279,
	PROTO_QUERY_CLASSROOM_All_STUDENTS_INFO	= 1280,
	PROTO_ADD_CLASSROOM_STUDENTS_INFO		= 1281,
	PROTO_DEL_CLASSROOM_STUDENTS_INFO		= 1282,
	PROTO_GET_CLASSROOM_STUDENTS_INFO		= 1283,
	PROTO_GET_CLASSROOM_TEACHER_INFO		= 1284,
	PROTO_GET_CLASSROOM_TEACH_PLAN_INFO		= 1285,
	PROTO_SET_CLASSROOM_CLASS_BEGIN			= 1286,
	PROTO_SET_CLASSROOM_CLASS_MODE			= 1287,
	PROTO_SET_CLASSROOM_CLASS_END			= 1288,
	PROTO_GET_CLASSROOM_COURSE_LEVEL		= 1289,
	PROTO_NOTIFY_CLASSROOM_TEACHER_INFO		= 1290,
	PROTO_NOTIFY_CLASSROOM_EVENT_INFO		= 1291,
	PROTO_LAHM_CLASSROOM_LAHM_EXAM			= 1292,
	PROTO_GET_CLASSROOM_TEACHER_EXAM_INFO	= 1293,
	PROTO_GET_USER_CLASSROOM_COUNT			= 1294,
	PROTO_GET_USER_CLASSROOM_RAND_USERID	= 1295,
	PROTO_GET_USER_CLASSROOM_CLASS_FLAG		= 1296,

	PROTO_FERTILIZE_FLOWER					= 1297,
	PROTO_POLLINATE_FLOWER					= 1298,
	PROTO_PLANT_DISAPPEAR					= 1299,
	PROTO_UPDATE_PLANT_INFO					= 1300,
	PROTO_GET_HOMELAND_INFO					= 1301,
	PROTO_SET_HOMELAND_ITEM					= 1302,
	PROTO_GET_HOMELAND_BOX_ITEM				= 1303,
	PROTO_HOMELAND_PLANT_SEED				= 1304,
	PROTO_HOMELAND_ROOT_OUT_PLANT			= 1305,
	PROTO_HOMELAND_WATER_PLANT				= 1306,
	PROTO_HOMELAND_KILL_BUG					= 1307,
	PROTO_HOMELAND_GET_ONE_PLANT			= 1308,
	PROTO_HOMELAND_HARVEST_FRUITS			= 1309,
	PROTO_SELL_FRUITS						= 1310,
	PROTO_GET_RECENT_JY_VISITOR				= 1311,
	PROTO_SET_HOMELAND_USED_ITEM			= 1312,

	PROTO_CHK_IF_USER_EXIST					= 1313,
	PROTO_GET_USER_FLAG						= 1314,
	PROTO_TOO_LONG_IN_GAME					= 1315,
	PROTO_GET_FRUIT_FREE					= 1316,
	PROTO_GET_ITEM_FROM_BAOHE				= 1318,
	PROTO_GET_BAOHE_OP_LIST					= 1319,

	PROTO_NOTIFY_CJ_FIRE_VALUE				= 1320,
	PROTO_KILL_CJ_FIRE						= 1321,
	PROTO_GET_CJ_FIRE_VALUE					= 1322,
	PROTO_GET_JY_FIRE_VALUE					= 1323,
	PROTO_KILL_JY_FIRE						= 1324,
	PROTO_NOTIFY_JY_FIRE_VALUE				= 1325,
	PROTO_KILL_FIRE_RAND_ITEM				= 1326,
	PROTO_SET_QUES_NAIRE					= 1327,
	PROTO_GET_PROFESSIONS					= 1328,
	PROTO_GET_SMC_SALARY					= 1329,
	PROTO_CHK_SMC_SALARY					= 1330,

	PROTO_NOTIFY_ONLINE_TIME				= 1334,
	PROTO_NOTIFY_REMAIN_TIME				= 1335,

	PROTO_THIEF_FRUIT						= 1341,

	PROTO_SEND_ONE_GIFT						= 1352,
	PROTO_GET_ADDITINAL_ITM					= 1353,
	PROTO_GET_GIFT_TIMES					= 1354,

	PROTO_GET_PASTURE						= 1361,
 	PROTO_GET_STOREHOUSE_ITEM				= 1362,
 	PROTO_CAPTURE_ANIMAL					= 1363,
 	PROTO_PASTURE_ADD_FEED					= 1364,
 	PROTO_HERD_ANIMAL						= 1365,
 	PROTO_PASTURE_GET_FISH					= 1366,
 	PROTO_PASTURE_ADD_WATER					= 1367,
 	PROTO_PASTURE_GET_RECENT_VISITOR		= 1368,
 	PROTO_PASTURE_GET_FEED_ITEM				= 1369,
    PROTO_SET_PASTURE_BOX_TIEM				= 1370,
	PROTO_SET_PASTURE_LOCK					= 1371,
	PROTO_GET_SHEEP							= 1372,
	PROTO_GET_SHEEP_REWARD					= 1373,
	PROTO_FOLLOW_ANIMAL						= 1374,
	PROTO_ANIMAL_CHICAO						= 1375,
	PROTO_REPORT_ANIMAL_HUNGER				= 1376,
	PROTO_RELEASE_ANIMAL					= 1377,
	//PROTO_GET_DEFENSE_VALUE				= 1381,
	//PROTO_SET_DEFENSE_VALUE				= 1382,
	//PROTO_GET_FIVE_ROCK 					= 1383,
	//PROTO_SET_FIVE_ROCK 					= 1384,
	PROTO_GET_500_XIAOME					= 1385,
	PROTO_PAY_MONEY							= 1386,
	PROTO_GET_EGG_HATCHS					= 1387,
	PROTO_SET_EGG_HATCHS					= 1388,
	PROTO_BUY_SOME_SUITS					= 1389,
	PROTO_GET_RAND_TIMES					= 1390,

	PROTO_GIVE_JIJILE_CARD_FREE				= 1391,
	PROTO_WANT_ITEM_RAND					= 1392,
	PROTO_GET_PET_ATTIRE_CNT				= 1393,
	PROTO_GET_ITEMS_CNT_IN_BAG				= 1394,
    PROTO_SET_HAPPY_CARD					= 1400,
    PROTO_GET_HAPPY_CARD					= 1401,
    PROTO_TRADE_HAPPY_CARD					= 1402,
    PROTO_GET_TRADE_HAPPY_CARD_INFO			= 1403,
    PROTO_GET_HAPPY_CARD_CLOTH				= 1404,
    //PROTO_GET_FIRE_CUP_CARD				= 1405,
    PROTO_GET_MAGIC_CARD_REWARD				= 1406,
    PROTO_NOTIFY_ANIMAL_USED_SKILL			= 1407,
    PROTO_GET_MAP_RAIN_EGG_INFO				= 1408,
    PROTO_GAIN_MAP_POS_EGG					= 1409,
    PROTO_NOTIFY_ALL_USE_SKILL_PRIZE		= 1410,
    PROTO_GAIN_USE_SKILL_PRIZE_ITEM			= 1411,
    PROTO_GET_USE_SKILL_PRIZE_INFO			= 1412,
    PROTO_GET_ITEMS_COUNT_NEW				= 1413,
    PROTO_TELL_FLASH_SOME_MSG				= 1414,

	PROTO_GET_FRIEND_HOME_HOT				= 1456,

	PROTO_GET_FRD_GAME_GRADE_LIST			= 1461,
	PROTO_GET_SELF_GAME_GRADE_LIST			= 1462,
	PROTO_SET_GAME_PK_INFO					= 1463,
	PROTO_GET_HISTORY_PK_LIST				= 1464,
	PROTO_GET_PK_SCORE						= 1465,
	PROTO_GET_PK_TRADE						= 1466,
	PROTO_GET_ANIMAL_LVL					= 1467,
	PROTO_FARM_ANIMAL_USE_ITEM				= 1468,
	PROTO_GET_ANIMAL_USED_SKILL_INFO		= 1469,
	PROTO_FARM_ANIMAL_USED_SKILL			= 1470,

	PROTO_EDIT_LAHM_DIARY					= 1471,
	PROTO_GET_DIARY_TITLES					= 1472,
	PROTO_DELETE_LAHM_DIARY					= 1473,
	PROTO_SET_DIARY_LOCK_STATE				= 1474,
	PROTO_SEND_DIARY_FLOWER					= 1475,
	PROTO_GET_DIARY_CONTENT					= 1476,
	PROTO_SET_BIG_LOCK						= 1477,
	PROTO_GET_BIG_LOCK						= 1478,
	PROTO_GET_DIARY_NUM						= 1479,
	PROTO_OCTOPUS_CAR_GAME					= 1480,

	PROTO_SET_WORK_NUM						= 1481,
	PROTO_GET_WORK_NUM						= 1482,
	PROTO_GET_WORK_SALARY					= 1483,
	//PROTO_SET_ENGINEER_LEVEL				= 1484,
	PROTO_SET_WORKER						= 1485,
	PROTO_GET_GOOD_ANIMAL_NUM				= 1486,

	PROTO_PICK_PET_MAGIC_TASK				= 1491,
	PROTO_FIN_PET_MAGIC_TASK				= 1492,
	PROTO_CANCEL_PET_MAGIC_TASK				= 1493,
	PROTO_SET_MAGIC_TASK_DATA				= 1494,
	PROTO_GET_MAGIC_TASK_DATA				= 1495,
	PROTO_GET_PET_MAGIC_TASK				= 1496,
	PROTO_GET_MAGIC_ITEMS					= 1497,
	PROTO_MAGIC_PASSWD_LOOKUP				= 1498,

    PROTO_GET_SICKBED_INFO					= 1500,
    PROTO_DOCTOR_SIT						= 1501,
    PROTO_PATIENT_SIT						= 1502,
    PROTO_DOCTOR_QUIT						= 1503,
    PROTO_PATIENT_QUIT						= 1504,
    PROTO_DOCTOR_CHECK						= 1505,
    PROTO_DOCTOR_CURE						= 1506,
    PROTO_DOCTOR_DUTY						= 1507,
    PROTO_MEDICINE_CURE_LAHM				= 1508,

	PROTO_GGJ_EXCHANGE_ITM					= 1521,
	PROTO_NEWYAER_ITEM_EXCHANGE				= 1522,
	PROTO_GIVE_NPC_SOME_ITEMS				= 1523,

	PROTO_GET_EVENT_RECHARGE_MONTHS			= 1524,
	PROTO_GET_EVENT_RECHARGE_REPLAY			= 1525,
	PROTO_NOTIFY_RECHARGE					= 1526,

	PROTO_GET_MATCHED_STRING				= 1531,
	PROTO_GET_CHAR_CLOTH					= 1532,
	PROTO_CATCH_PLAYER_BY_WORD				= 1533,
	PROTO_EXCHG_CHAR_CLOTH					= 1534,
	PROTO_GET_CLASS_SCORE					= 1535,
	PROTO_GET_TANGGUO_SCORE_TOTAL			= 1536,
	PROTO_ENGLISH_LAHM_GET_CLOTH			= 1537,
	PROTO_GET_CHRIS_CAKE_INFO				= 1538,
	PROTO_END_MAKE_ONE_CAKE					= 1539,
	PROTO_GIVE_CAKE_TO_NPC					= 1540,
	PROTO_GET_CHRIS_CAKE_SCORE				= 1541,


	PROTO_GET_pos_info						= 60019,
	PROTO_GET_cut_sheep_cheer				= 60013,
	PROTO_GET_fisher_cheer					= 60014,
	PROTO_GET_chris_tree_stat				= 60015,
	PROTO_GET_barber_cheer					= 60016,

	PROTO_RACE_SING							= 1551,
	PROTO_RACE_CONFIRM_ADD_SCORE			= 1552,
	PROTO_RACE_GET_SELF_SCORE				= 1553,
	PROTO_RACE_UPDATE_TEAM_RACE_VALUE		= 1554,
	PROTO_RACE_GET_TEAM_RACE_VALUE_LIST		= 1555,
	PROTO_RACE_GET_TEAM_SCORE_VALUE_LIST	= 1556,
	PROTO_RACE_ADD_RENQI					= 1557,
	PROTO_RACE_GET_TRACK_CNT				= 1558,
	PROTO_RACE_GET_USER_TODAY_SCORE			= 1559,
	PROTO_RACE_START_RENQI					= 1560,
	PROTO_RACE_THIS_TIME_RENQI				= 1561,
	PROTO_RACE_GET_RENQI_FLAG				= 1562,
	PROTO_RACE_SPECIAL_AWARD				= 1563,
	PROTO_SEND_MOTHERLAND_FLOWER			= 1565,
	PROTO_GET_MOTHERLAND_FLOWER_CNT			= 1566,
	PROTO_EXCHG_XUANFENG_JIJILE				= 1567,
	PROTO_GET_XUANFENG_CAR					= 1568,
	PROTO_PLANT_DOWN						= 1569,
	PROTO_GET_USER_NUM_IN_SHIP				= 1570,
	PROTO_GET_NEXT_FLIGHT_TIME				= 1571,
	PROTO_NOTIFY_RENQI_HUIZHANG				= 1572,

	PROTO_RACE_AWORD_COMMEMORATE			= 1573,
	/***/
	PROTO_ENGLISH_CLASS						= 1574,
	PROTO_ENGLISH_CAN_GET_XIAOME			= 1576,

	/***/
	PROTO_RACE_AWARD						= 1577,
	PROTO_RACE_WINNER_OR_NO					= 1579,

	PROTO_MAP_OPEN_CLOSE					= 1581,

    PROTO_SET_CLOSET						= 1600,
    PROTO_UNSET_CLOSET						= 1601,
    PROTO_GET_CLOSET						= 1602,

	PROTO_GET_JIAZHAO						= 1699,
    PROTO_DRIVE_OUT							= 1700,
    PROTO_DRIVE_BACK						= 1701,
    PROTO_LIST_CAR							= 1702,
    PROTO_SET_SHOW_CAR						= 1703,
    PROTO_GET_SHOW_CAR						= 1704,
    PROTO_CAR_REFUEL						= 1705,
    PROTO_BUY_CAR							= 1706,
    PROTO_CHECK_CAR							= 1707,
    //PROTO_CHANGE_CAR_ATTR					= 1706,
    //PROTO_CHANGE_CAR_SLOT					= 1707,
    //PROTO_CHANGE_CAR_ADDON				= 1708,
    PROTO_RENT_ZHAIJISONG					= 1709,
    PROTO_ON_OFF_NANGUA						= 1710,

	PROTO_CANDY_POSITION					= 1800,
	PROTO_CANDY_EXCH						= 1801,
	PROTO_CANDY_FROM_RECY					= 1802,
	PROTO_CANDY_POSITION_EX					= 1803,
	PROTO_CANDY_MAKE						= 1804,
	PROTO_UPDATE_CANDY						= 1805,
	PROTO_CANDY_HAVE_ONE					= 1806, //is candy take?

	PROTO_BECOME_PUMPKIN					= 1851,
	PROTO_PUMPKIN_EXPIRED					= 1852,
	PROTO_BLACK_CAT_APPEAR					= 1853,
	PROTO_GET_BLACK_CAT_STATUS				= 1854,
	PROTO_POISON_GAS						= 1855,
	PROTO_GET_CANDY_FROM_OTHER				= 1856,

	PROTO_PASTURE_GET_NETS					= 1900,
	PROTO_PASTURE_NETS_STATUS				= 1901,
	PROTO_PASTURE_CATCH_FISH				= 1902,

	PROTO_CANDY_PACKET_SEND					= 1903,
	PROTO_CHANGE_SELF_NANGUA				= 1904,
	PROTO_BECOME_NANGUA_EXPIRE				= 1905,
	PROTO_SUB_CANDY_COUNT					= 1906,
	PROTO_GET_CANDY_COUNT					= 1907,
	PROTO_EXCHANGE_PROP						= 1908,

	PROTO_VIP_PATCH_WORK					= 1909,
	PROTO_GET_BIBO_INFO						= 1910,
	PROTO_PASTURE_GET_LEVEL					= 1911,
	PROTO_GET_NPC_LOVELY					= 1912,
	PROTO_GIFT_DRAW							= 1913,
	PROTO_UPDATE_NPC_LOVELY					= 1914,
	PROTO_GET_ITEMS_COUNT					= 1915,
	PROTO_SEND_ONE_OF_TWO					= 1916,

	PROTO_GET_VIP_INVATATION				= 1917,
	PROTO_SET_EGG_POS						= 1918,
	PROTO_PROCESS_FUDAN						= 1919,
	PROTO_GET_FUDAN_TIMES					= 1920,

	PROTO_CUT_YANGMAO						= 1921,
	PROTO_GET_FIRE_EGG						= 1922,
	PROTO_RABIT_OCCUPY_POS					= 1923,
	PROTO_BROADCAST_RABIT_POS				= 1924,
	PROTO_BUY_LIMITED_ITEM					= 1925,
	PROTO_GET_LITTLE_RABIT_INFO				= 1926,
	PROTO_SEND_GIFT_TWO						= 1927,
	PROTO_BREAK_EGG							= 1928,
	PROTO_FETCH_LITTLE_TU					= 1929,
	PROTO_OCCUPY_CUT_POS					= 1930,
	PROTO_GET_CUT_POS						= 1931,
	PROTO_BEAUTY_COMPETE					= 1932,

	PROTO_SET_PHOTO_DRESS					= 1933,
	PROTO_GET_PHOTO_DRESS					= 1934,
	PROTO_BROADCAST_SHOW_STAT				= 1935,
	PROTO_GET_SHOW_STATUS					= 1936,
	PROTO_GET_ITEM_PRICE					= 1937,
	PROTO_SET_MODEL_WEAR					= 1938,
	PROTO_GET_MODEL_WEAR					= 1939,
	PROTO_SET_MOLE_MODEL_WEAR				= 1940,
	PROTO_SET_MODEL_MOLE_WEAR				= 1941,
	PROTO_SET_CAKE							= 1946,
	PROTO_GET_CAKE_INFO						= 1947,
	PROTO_GET_RABIT_MASTER_CLOTH			= 1948,
	PROTO_DAFEN_OVER						= 1949,
	PROTO_GET_SHOW_SCORE					= 1950,
	PROTO_UP_LEVEL							= 1951,
	PROTO_SET_ECHO_NUM_TYPE					= 1952,
	PROTO_BROADCAST_NUM						= 1953,
	PROTO_GET_ECHO_NUM_TYPE					= 1954,
	PROTO_GET_TUJIAN						= 1955,
	PROTO_WEIGHTING_FISH					= 1956,
	PROTO_FISHING							= 1957,
	PROTO_GET_TOTAL_FISH_WEIGHT				= 1958,
	PROTO_GET_LIMIT_ITEM_NUM				= 1959,
	PROTO_GET_RING_FROM_OTHER				= 1960,
	PROTO_GET_10XIAOMEE_50TIMES				= 1961,

	PROTO_GET_ALL_SELLER_INFO	 			= 1962,
	PROTO_OCCUPY_BOOTH						= 1963,
	PROTO_LEAVE_BOOTH						= 1964,
	PROTO_RMOVE_SELLER			 			= 1965,
	PROTO_BUY_FURNI_FROM_BOOTH	 			= 1966,
	PROTO_SET_CHRISTMAS_WISH				= 1967,
	PROTO_GET_CHRISTMAS_WISH				= 1968,
	PROTO_GET_CHRISTMAS_GIFTS				= 1969,
	PROTO_GET_CHRIS_GIFTS_NUM				= 1970,
	PROTO_SEND_CHRIS_GIFTS_BILU				= 1971,
	PROTO_GET_CHRIS_GIFT_FROM_BILU			= 1972,
	PROTO_GET_XIAO_XIONG_CAR				= 1973,
	PROTO_GIFT_RAIN_BEGAIN					= 1974,
	PROTO_COLLECT_RAIN_GIFT					= 1975,
	PROTO_SEND_CHRIS_GIFTS_TO_NPC			= 1976,
	PROTO_ADD_NENG_LIANG_XING_CNT			= 1977,
	PROTO_GET_NENG_LIANG_XING_CNT			= 1978,
	PROTO_SET_PHOTO_DAHEZHAO				= 1979,
	PROTO_GET_PHOTO_DAHEZHAO				= 1980,
	PROTO_GET_DAJIAOYIN_REWARD				= 1981,
	PROTO_EXCHG_MIBI_COUPON					= 1982,
	PROTO_FETCH_JINIAN_JIAOYIN				= 1983,
	PROTO_NOTIFY_SKILL_UPDATE				= 1984,
	PROTO_GET_LITTLE_FEMALE_TIGER			= 1985,
	PROTO_EXCHANGE_GOLD_SILVER				= 1986,
	PROTO_EXCHANGE_ALL_TO_ANOTHER			= 1987,
	PROTO_GET_SKILL_ITME_LIBAO				= 1988,
	PROTO_QUIT_MACHINE_DOG					= 1989,
	PROTO_FEED_MACHINE_DOG					= 1990,
	PROTO_GET_DOG_DO_THING_INFO				= 1991,
	PROTO_GET_PIPI_OR_DOUYA					= 1992,
	PROTO_GET_MILK_FROM_COW					= 1993,
	PROTO_SET_ONLY_ONE_FLAG					= 1994,
	PROTO_PROFESSION_DO_WORK				= 1995,
	PROTO_ADD_SCORE_FOR_WO_WEI_HUI			= 1996,
	//PROTO_GIVE_YUMI_GET_YINGTAO			= 1997,

	PROTO_GET_VIP_LEVEL_LAHM_GIFTS			= 1998,
	PROTO_GET_VIP_LEVEL_GIFTS				= 1999,
	PROTO_IS_SET_PAY_PASSWD 				= 2000,
	PROTO_INQUIRE_ONE_ITEM_PRICE			= 2001,
	PROTO_INQUIRE_MULTI_ITEMS_PRICE			= 2002,
	PROTO_INQUIRE_ONE_ITEM_DETAIL			= 2003,
	PROTO_BUY_ITEM_USE_MIBI					= 2004,
	PROTO_BUY_ITEM_USE_MIBI_COUPON			= 2005,
	PROTO_GET_MIBI_COUPON_INFO				= 2006,
	PROTO_FOR_WU_WEI_HUI_SCORE				= 2007,
	PROTO_GET_SUPPORT_NPC					= 2008,
	PROTO_BROAD_CAST_WHEN_FINI_TASK			= 2009,
	PROTO_GET_LOGIN_SESSION					= 2010,
	PROTO_TANTALIZE_CAT						= 2011,
	//PROTO_GET_DELICOUS_FOOD				= 2012,
	//PROTO_SHARE_DELICOUS_FOOD				= 2013,
	//PROTO_EXCHANGE_WITH_REPURATION		= 2014,
	//PROTO_GET_REPUTATION					= 2015,
	//PROTO_GET_STH_ACCORD_REPUTATION		= 2016,
	PROTO_TEAM_SCORE_GET_MAX				= 2017,
	PROTO_ADD_TEAM_SPORT_SCORE				= 2018,
	PROTO_GET_SPROT_TEAM_SCORE				= 2019,
	PROTO_GET_TEAM_MEDAL_NUM				= 2020,
	//PROTO_ADD_STH_FOR_MAX_SCORE_TEAM		= 2021,
	PROTO_LAHM_SPORT_GET_DIFF_TIME			= 2022,
	PROTO_LAHM_SPORT_START_AND_END			= 2023,
	PROTO_LAHM_SPORT_ADD_MEDAL				= 2024,
	PROTO_LAHM_SPORT_GET_TOP_PET_SCORE		= 2025,
	PROTO_LAHM_SPORT_GET_SELF_MEDAL_NUM		= 2026,
	PROTO_GET_MONEY_BY_GUA_GAU_CARD 		= 2028,

	//for gold bean server
	PROTO_GOLD_BEAN_INQUIER_MUL_ITEM_PRICE	= 2029,
	PROTO_GOLD_BEAN_INQUIER_ONE_ITEM_PRICE	= 2030,
	PROTO_GOLD_BEAN_BUY_ITEM_USE_GOLD_BEAN	= 2031,
	PROTO_GOLD_BEAN_GET_GOLD_BEAN_INFO		= 2032,
	PROTO_BUY_GOLD_BEAN_USE_MIBI			= 2033,

	PROTO_GET_BEAN_MALL_LOGIN_TIMES			= 2040,

	//得到vip包月抵用券信息
	PROTO_GET_VIP_MONTH_TICKET_INFO			= 2050,

	//free super lahm vip
	PROTO_SET_FREE_VIP_INFO                 = 2051,
	PROTO_GET_FREE_VIP_INFO                 = 2052,

	PROTO_QUERY_MAGIC_CODE_GIFT				= 2101,
	PROTO_CONSUME_MAGIC_CODE				= 2102,
	//2800 ---> 2899 protocol id will be used in oversea version

	//card operation
	PROTO_CARD_DO_INIT 						= 3000,
	PROTO_CARD_GET_INFO 					= 3001,
	PROTO_CARD_ADD_BASIC 					= 3002,
	PROTO_CARD_CHECK_AVAIL_NUM				= 3003,

	PROTO_TAKE_NPC_TASK						= 3101,
	PROTO_FIN_NPC_TASK						= 3102,
	PROTO_SET_NT_CLIENT_DATA				= 3103,
	PROTO_GET_NT_CLIENT_DATA				= 3104,
	PROTO_GET_NPC_TASK_STATUS				= 3105,
	PROTO_GET_NPC_TASKS						= 3106,
	PROTO_CANCEL_NPC_TASKS					= 3107,
    //answer question
    PROTO_SEND_QUESTION						= 4000,
    PROTO_ANSWER_QUESTION					= 4001,
	PROTO_GET_TV_QUESTION					= 4002,
	PROTO_ANSWER_TV_QUESTION				= 4003,

	//中秋月饼下架
	PROTO_USER_EAT_MOONCAKE					= 4100,
	PROTO_USER_GET_MOONCAKE					= 4101,
	PROTO_USER_SWAP_MOONCAKE				= 4102,

	PROTO_USER_GET_LUCKY_TICKET				= 4103,
	PROTO_USER_SWAP_TICKET_PRIZE			= 4104,
	PROTO_USER_LOOK_TODAY_TICKET			= 4105,
	PROTO_USER_LOOK_HISTORY_TICKET			= 4106,
	PROTO_USER_NATIONAL_DAY_LOGIN			= 4108,
	PROTO_USER_GET_NATIONAL_DAY_GIFT		= 4109,
	PROTO_XHX_GET_TASK 						= 4110,
	PROTO_XHX_CHECK_USER					= 4111,
	PROTO_XHX_ADD_ITEMS						= 4112,
	PROTO_SG_GET_PATROL_TESK				= 4113,
	PROTO_SG_USER_ASK_FOR_HELP				= 4114,
	PROTO_SG_GUIDER_HELP_OTHERS				= 4115,
	PROTO_SG_GET_GUIDER_PRIZE				= 4116,
	PROTO_SG_LOOK_GUIDER_PRIZE				= 4117,
	PROTO_SG_USER_CHANGE_GUIDER_STATE		= 4118,
	PROTO_HANG_CHRISTMAS_SOCK				= 4119,
	PROTO_GET_CHRISTMAS_SOCR_RECORD			= 4120,
	PROTO_GET_CHRISTMAS_SOCK_TOP10			= 4121,
	PROTO_GET_CHRISTMAS_SOCK_GIFTBOX		= 4122,
	PROTO_GET_CHRISTMAS_SOCK_GIFT			= 4123,
	PROTO_USER_SUBMIT_SPRING_COUPLETS		= 4124,
	PROTO_USER_GET_SPRING_COUPLETS			= 4125,
	PROTO_USER_GET_COUPLETS_USERID_LIST 	= 4126,
	PROTO_USER_GET_COUPLETS_PRIZE			= 4127,
	PROTO_USER_SET_COUPLETS_PRIZE			= 4128,
	PROTO_USER_LOOK_ONE_COUPLETS 			= 4129,

	//mole class
    PROTO_CREATE_CLASS						= 5000,
    PROTO_MODIFY_CLASS						= 5001,
    PROTO_GET_CLASS_DETAIL					= 5002,
    PROTO_JOIN_CLASS						= 5003,
    PROTO_QUIT_CLASS						= 5004,
    PROTO_REQ_MONITOR_VERIFY				= 5005,
    PROTO_MONITOR_VERIFY					= 5006,
    PROTO_DEL_CLASS_MEMBER					= 5007,
    PROTO_DEL_CLASS							= 5008,
    PROTO_GET_CLASS_LOGO_INFO				= 5009,
    PROTO_SET_FIRST_CLASS					= 5010,
    PROTO_GET_FIRST_CLASS					= 5011,
    PROTO_GET_CLASS_ITM_UNUSED				= 5012,
    PROTO_SET_CLASS_ITEM					= 5013,
    PROTO_GET_CLASS_LIST					= 5014,
    PROTO_LOCK_CLASS						= 5015,
    PROTO_UNLOCK_CLASS						= 5016,
    PROTO_GET_CLASS_SIMPLE					= 5017,
    PROTO_GET_CLASS_MEMBER_LIST				= 5018,
    PROTO_GET_TOP_CLASS						= 5019,
    PROTO_GET_CLASS_HONOR					= 5020,

    PROTO_GET_CLASS_QUESTION_STAT			= 5400,
    PROTO_GET_CLASS_QUESTION				= 5401,
    PROTO_ANSWER_CLASS_QUESTION				= 5402,
    PROTO_GET_CLASS_QUESTION_INFO			= 5403,
    PROTO_GET_CLASS_QUES_REWARD				= 5404,
    PROTO_GET_CLASS_QUES_MEMBER_REWARD		= 5405,

    PROTO_REQ_TMP_SUPERLAMN					= 5500,
    PROTO_REQ_TMP_SUPERLAMN_STAT			= 5501,
    PROTO_GET_LOTTERY						= 5502,
	PROTO_GET_CARRY_ROCK_NUM				= 5503,
	//PROTO_SET_CARRY_ROCK_NUM				= 5504,

    //PROTO_SPORT_SIGN_UP					= 5600,
    //PROTO_SPORT_FIN_PRE_TASK				= 5601,
    //PROTO_SPORT_ENCOURAGE					= 5602,
    //PROTO_SPORT_RAND_SIGN					= 5603,
    //PROTO_SPORT_GET_TASK					= 5604,
    //PROTO_SPORT_START_TASK				= 5605,
    //PROTO_SPORT_SET_TASK					= 5606,
    //PROTO_SPORT_TEAM_HISTORY				= 5607,
    //PROTO_SPORT_WATERING_CNT				= 5608,
    //PROTO_SPORT_TEAM_CLOTH				= 5609,
    //PROTO_SPORT_BREAK						= 5610,
    //PROTO_SPORT_REACH_MAX					= 5611,
    //PROTO_SPORT_CHANCE_CYCLE				= 5612,
    //PROTO_SPORT_SELF_INFO					= 5613,
    //PROTO_SPORT_FIRE_STAT					= 5614,
    //PROTO_SPORT_FIRE_START				= 5615,
    //PROTO_GET_SPORT_REWARD				= 5616,
	PROTO_SUPER_LAMU_PARTY_GET				= 5700,
	PROTO_LAHM_FOOD_MACHINE					= 6001,

	PROTO_LAHM_CLASSROOM_ON_FELLOWSHIP		= 6002,
	PROTO_GIVE_ITEM_TO_FRIEND				= 6003,
	PROTO_QUERY_TYPE_RANK					= 6004,
	PROTO_QUERY_TYPE_HISTORY_RECORD			= 6005,
	PROTO_GET_TYPE_COUNT_ITEM_BONUS			= 6006,
	PROTO_GET_TYPE_RANK_COUNT_INFO			= 6007,
	PROTO_NOTIFY_SNOWBALL_BEGIN_END   		= 6008,
	PROTO_SNOWBALL_ENTER_QUIT   			= 6009,
	PROTO_SNOWBALL_ATTACK_ENEMY 			= 6010,
	PROTO_SNOWBALL_QUERY_TEAMS_INFO 		= 6011,
	PROTO_SNOWBALL_OCUPY_POS 				= 6012,
	PROTO_SNOWBALL_GET_GAME_BONUS 			= 6013,
	PROTO_SNOWBALL_GET_LEFT_TIME 			= 6014,
	PROTO_SNOWBALL_CHECK_CHRISTMAS_GIFT 	= 6035,
	PROTO_SNOWBALL_LAHM_ATTACK				= 6036,
	PROTO_SNOWBALL_GET_GAME_NEW_BONUS 		= 6037,

	PROTO_GET_USED_MIBI_COUNT				= 6015,
	PROTO_EXCHANGE_BEANS					= 6016,
	PROTO_CHRISTMAS_SOCK_ADD_SOCK			= 6017,
	PROTO_CHRISTMAS_SOCK_GET_SOCK_INFO		= 6018,
	PROTO_CHRISTMAS_SOCK_GET_BONUS			= 6019,
	PROTO_TIME_SYN_ONE_MINUTE				= 6020,
	PROTO_GET_FOOT_PRINT_BONUS				= 6021,
	PROTO_GET_FOOT_PRINT_ITEMS				= 6022,
	PROTO_GET_FOOT_PRINT_COUNT_INFO			= 6023,
	PROTO_GET_CONTINUE_LOGIN_BONUS_INFO		= 6024,
	PROTO_GET_QUES_NAIRE_INFO				= 6025,
	PROTO_GET_30_DAY_LOGIN_BONUS_INFO		= 6026,
	PROTO_TRAIN_KAKU_NIAN					= 6027,
	PROTO_GET_TRAIN_KAKU_NIAN_INFO			= 6028,
	PROTO_LEAVE_HERO_GAME					= 6029,
	PROTO_GET_IN_HERO_USERID_LIST			= 6030,
	PROTO_GAIN_GAME_ANGLES					= 6031,
	PROTO_GAIN_WEEKEND_BONUS				= 6032,
	PROTO_GET_WEEKEND_CONTINUE_LOGIN_COUNT	= 6033,
	PROTO_ADD_CLIENT_MSG_LOG				= 6034,

	PROTO_GET_PARADISE						= 7001,
	PROTO_GET_PRARDISE_STORE_ITEMS			= 7002,
	PROTO_GET_HOT_ITEM						= 7003,
	PROTO_GET_FIGHT_LEVEL					= 7004,
	PROTO_GET_FIGHT_SCORE					= 7005,
	PROTO_GET_ANGEL_IN_HOSPITAL				= 7006,
	PROTO_GET_PROP_FOR_TD					= 7007,
	PROTO_ADD_NIMSBUS						= 7010,
	PROTO_LEAVE_HOSPITAL					= 7011,
	PROTO_FEED_ANGLE_SEED					= 7020,
	PROTO_USE_PARADISE_PROP					= 7021,
	PROTO_CHANGE_BACKGROUND					= 7022,
	PROTO_USE_TEAR_FOR_ANGEL				= 7023,
	PROTO_USE_PROP_FOR_TD					= 7024,
	PROTO_BUILD_ANGLE_CONTRACT				= 7030,
	PROTO_FREE_ANGLE_IN_PARADISE			= 7031,
	PROTO_CHANGE_TO_ANGEL					= 7032,
	PROTO_GET_CONTRACT_ANGLES				= 7040,
	PROTO_FREE_CAPTURED_ANGEL				= 7041,
	PROTO_MAKE_ANGLE_FALLOWED				= 7042,
	PROTO_PARADISE_LEVEL_UP					= 7050,
	PROTO_GET_VISITOR_PARADISE_INFO			= 7060,
	PROTO_GET_FRIEND_PARADISE_INFO			= 7061,
	PROTO_GET_FIGHT_ANGEL					= 7070,
	PROTO_GET_TD_START						= 7071,
	PROTO_GET_TD_END						= 7072,
	PROTO_CLEAN_ANGEL_GAME_BEGIN			= 7073,
	PROTO_CLEAN_ANGEL_SUCESS				= 7074,
	PROTO_PUSH_CLEAN_ANGEL_GAME_END			= 7075,
	PROTO_CLEAN_ANGEL_GAME_END				= 7076,
	PROTO_KILL_MONSTER						= 7077,
	PROTO_ADD_ANGEL							= 7078,
	PROTO_ANGEL_SHOW						= 7079,
	PROTO_ANGEL_REGAIN						= 7080,
	PROTO_ANGEL_GET_FAVORITE_INFO			= 7081,
	PROTO_GET_PARADISE_BACKGROUND_ID		= 7082,
	PROTO_EXCHANGE_DAVIS_BEAN				= 7083,
	PROTO_GET_PARADISE_HONORS				= 7084,
	PROTO_GET_HONOR_ITEM					= 7085,
	PROTO_NOTIFY_USER_GET_HONOR				= 7086,
	PROTO_SAVE_MONSTER_GAME					= 7087,
	PROTO_GET_ANGEL_COMPOSE_MATERIAL		= 7089,
	PROTO_USER_COMPOSE_NEW_ANGEL			= 7090,

	PROTO_ANGEL_CHALLENGE_GET_AWARD			= 7091,

	PROTO_USER_CATCH_BUTTERFLY_ANGEL		= 7092,
	PROTO_USER_LAHM_SPORT_CREATE_MVP_TEAM	    = 7093,
	PROTO_USER_LAHM_SPORT_ENTER_MVP_TEAM	    = 7094,
	PROTO_USER_LAHM_SPORT_LEAVE_MVP_TEAM	    = 7095,
	PROTO_USER_LAHM_SPORT_DISSOLVE_MVP_TEAM	    = 7096,
	PROTO_USER_LAHM_SPORT_GET_MVP_TEAM_INFO	    = 7097,
	PROTO_USER_CHANGE_LAHM_SPORT_TEAM           = 7098,
	PROTO_USER_GET_MVP_TEAM_RANKING             = 7099,

	PROTO_USER_CHEER_TEAM_ACT                    = 7100,
	PROTO_USER_CHEER_TEAM_TURN_END              = 7101,
	PROTO_GET_USER_CHEER_TEAM_INFO              = 7102,
	PROTO_GET_LAHM_SPORT_TEAMS_MEDALS           = 7103,
	PROTO_USER_LAHM_SPORT_GET_SYSARG_MVP_TEAM	= 7104,
	PROTO_CHOICE_PIGLET_RUSH_ACTIVITY           = 7105,
	PROTO_CHECK_PIGLET_CHAMPION_ACTIVITY        = 7106,
	PROTO_GET_PIGLET_CHAMPION_ACTIVITY_AWRD     = 7107,

	PROTO_DUNGEON_GET_EXHIBIT_INFO			= 8001,
	PROTO_DUNGEON_GET_STORAGE_INFO			= 8002,
	PROTO_DUNGEON_SET_EXHIBIT_ITEM			= 8003,
	PROTO_DUNGEON_SWAP_EXHIBIT_ITEM			= 8004,
	PROTO_DUNGEON_GET_FRIEND_INFO			= 8005,
	PROTO_DUNGEON_GET_VISITOR_INFO			= 8006,
	PROTO_DUNGEON_SET_BACKGROUND			= 8007,
	PROTO_DUNGEON_SET_EXHIBIT_STATE			= 8008,

	PROTO_DUNGEON_BEGIN_EXPLORE				= 8021,
	PROTO_DUNGEON_GET_BAG_INFO				= 8022,
	PROTO_DUNGEON_EXPLORE_AREA				= 8023,
	PROTO_DUNGEON_PICK_ITEM					= 8024,
	PROTO_DUNGEON_USE_ITEM					= 8025,
	PROTO_DUNGEON_END_EXPLORE				= 8026,
	PROTO_DUNGEON_SYN_HP					= 8027,

	//挖掘矿点时随机获取地图碎片
	PROTO_DUNGEON_GET_SYS_AWARD				= 8028,

	//Angry Lahm
	PROTO_AL_GET_TASK_INFO					= 8040,
	PROTO_AL_UPDATE_SCORE					= 8041,

	//hide and seek
	PROTO_HAS_JOIN_GAME						= 8060,
	PROTO_HAS_QUIT							= 8061,
	PROTO_HAS_GET_QUEUE_INFO				= 8062,
	PROTO_HAS_GAME_START 					= 8063,
	PROTO_HAS_GAME_OVER						= 8064,

	//sand pool
	PROTO_SEND_TIME_MAIL					= 8065,
	PROTO_GET_TIME_MAIL						= 8066,
	PROTO_SEND_TIME_ITEM					= 8067,
	PROTO_GET_TIME_ITEM						= 8068,
	PROTO_GET_MAIL_ITEM_COUNT				= 8069,

	//play drum
	PROTO_USER_PLAY_DRUM					= 8070,
	PROTO_NOTIFY_PLAY_DRUM_BEGIN_END		= 8071,
	PROTO_GET_PLAY_DRUM_LEFT_TIME			= 8072,
	PROTO_GET_PLAY_DRUM_TEAM_CNT			= 8073,

	PROTO_GET_BOOK_INFO						= 8074,
	PROTO_SET_BOOK_INFO						= 8075,
	PROTO_GET_BOOK_LIST						= 8076,

	PROTO_GET_CANDY_FREIND_CNT				= 8077,
	PROTO_GET_CANDY_FROM_FREIND				= 8078,
	PROTO_GET_CANDY_SHARE_BONUS				= 8079,
	PROTO_GET_TASK_LIST_INFO				= 8080,
	PROTO_AS_ADD_TYPE_INT_DATA				= 8081,
	PROTO_AS_QUERY_TYPE_INT_DATA			= 8082,

	PROTO_ADD_MINE_MAP_PIECE				= 8083,
	PROTO_GET_MINE_MAP_PIECES				= 8084,
	PROTO_GET_MINE_KEY_CHANCE				= 8085,
	PROTO_GET_MINE_MAP_KEY					= 8086,
	PROTO_QUERY_MINE_KEY_CHANCE				= 8087,
	PROTO_QUERY_MINE_MAP_EXPLOR_TIME		= 8088,

	PROTO_GET_DAYTYPE_COUNT_LIST		= 8089,
	PROTO_GET_GOLD_COMPASS_USERS_INFO	= 8090,
	PROTO_GUESS_DATE_ITEM_PRICE	= 8091,
	PROTO_GUESS_RIGHT_USERS_INFO	= 8092,
	PROTO_GET_SPRINT_FESTIVAL_LOGIN_INFO	= 8093,
	PROTO_GET_SPRINT_FESTIVAL_LOGIN_BONUS	= 8094,
	PROTO_GET_VIP_CONTINUE_WEEK_LOGIN_INFO  = 8095,
	PROTO_GET_VIP_WEEK_LOGIN_AWARD          = 8096,
	PROTO_GET_ANOTHER_WEEKEND_LOGIN_INFO    = 8097,
	PROTO_GET_ANOTHER_WEEKEND_LGOIN_BONUS   = 8098,
	
	//Mongolian Cow pasture
	PROTO_GET_MONGOLIA_COW_PASTURE			= 8100,
	PROTO_PLANT_PASUTURE_GRASS				= 8101,
	PROTO_PLAY_WITH_MILK_COW				= 8102,
	PROTO_PURCHASE_PASTURE_MILK				= 8103,

	PROTO_ADD_USER_ITEMS_SCORE				= 8104,
	PROTO_GET_USER_ITEMS_SCORE				= 8105,
	PROTO_CHANGE_USER_ICE_DRAGON				= 8106,
	PROTO_EXCHANGE_USER_PIGLET_HOUSE		= 8107,
	PROTO_GET_USER_CUTEPIG_GOLD_KEY		    = 8108,
	PROTO_SPEECH_USER_ENTER_OR_LEAVE		= 8109,
	PROTO_SPEECH_GET_USER_INFO		        = 8110,

	//angel fight
	PROTO_AF_GET_USER_BASE_INFO				= 8200,
	PROTO_AF_GET_USER_PACKAGE				= 8201,
	PROTO_AF_GET_USER_SKILL					= 8202,
	PROTO_AF_GET_CARD_BOOK					= 8203,
	PROTO_AF_USE_ITEM						= 8204,
	PROTO_AF_USE_CARD						= 8205,
	PROTO_AF_SKILL_LEVEL_UP					= 8206,
	PROTO_AF_CHANGE_EQUIP					= 8207,
	PROTO_AF_GET_ALL_FRIENDS				= 8208,
	PROTO_AF_GET_ALL_FRIENDS_FIGHT			= 8209,
	PROTO_AF_GET_MAP_PASS_POINT				= 8210,
	PROTO_AF_GET_ONE_SKILL					= 8211,
	PROTO_AF_CHECK_USER_GET_PRIZE			= 8212,
	PROTO_AF_USER_GET_PRIZE					= 8213,
	PROTO_AF_USER_SET_WISHING				= 8214,
	PROTO_AF_DEAL_FRIEND_WISHING			= 8215,
	PROTO_AF_GET_WISHING_STATE				= 8216,
	PROTO_AF_DEAL_FRIEND_EVENT				= 8217,
	PROTO_AF_GET_SOME_FRIEND_INFO			= 8218,
	PROTO_AF_USER_GET_FIGHT_RECORD			= 8219,
	PROTO_AF_GET_WISHING_COLLECT			= 8220,
	PROTO_AF_GET_DAILY_TASK					= 8221,
	PROTO_AF_FINISH_ONE_TASK				= 8222,
	PROTO_AF_REFRESH_ONE_TASK				= 8223,
	PROTO_AF_GET_FIGHT_BOSS_COUNT			= 8224,
	PROTO_AF_GET_APPRENTICE_MARKET			= 8225,
	PROTO_AF_GET_MASTER_APPRENTICE_INFO		= 8226,
	PROTO_AF_CHECK_GET_APPRENTICE			= 8227,
	PROTO_AF_GET_APPRENTICE_TRAINING		= 8228,
	PROTO_AF_USER_TRAIN_APPRENTICE			= 8229,
	PROTO_AF_USER_EXPEL_APPRENTICE			= 8230,
	PROTO_AF_APPRENTICE_RESPECT_MASTER		= 8231,
	PROTO_AF_GET_RESPECT_CASH				= 8232,
	PROTO_AF_APPRENTICE_RETRAY_MASTER		= 8233,
	PROTO_AF_FINISH_APPRENTICE				= 8234,
	PROTO_AF_GET_MASTER_RECORD				= 8235,
	PROTO_AF_USER_ADD_APPRENTICE			= 8236,
	PROTO_AF_GET_PVP_FIGHT_RECORD 			= 8237,
	PROTO_AF_GET_PVP_WIN_PRIZE				= 8238,
	PROTO_AF_DONATE_CARD					= 8239,
	PROTO_AF_EXCHANGE_CARD					= 8240,
	PROTO_AF_GET_DONATE_INFO				= 8241,

	//cute pig
	PROTO_CP_GET_GAME_INFO					= 8250,
	PROTO_CP_GET_PIG_INFO					= 8251,
	PROTO_CP_GET_MALE_PIG_INFO				= 8252,
	PROTO_CP_GET_KNAPSACK_INFO				= 8253,
	PROTO_CP_BUY_ITEM						= 8254,
	PROTO_CP_RAISE_PIG						= 8255,
	PROTO_CP_FEED							= 8256,
	PROTO_CP_BATHE							= 8257,
	PROTO_CP_TICKLE							= 8258,
	PROTO_CP_TRAINING						= 8259,
	PROTO_CP_SET_NAME						= 8260,
	PROTO_CP_SET_FORMATION					= 8261,
	PROTO_CP_SALE							= 8262,
	PROTO_CP_MATE							= 8263,
	PROTO_CP_LEVEL_UP_NOTICE				= 8264,
	PROTO_CP_GET_FRIEND_INFO				= 8265,
	PROTO_CP_GET_TASK_INFO					= 8266,
	PROTO_CP_ACCEPT_TASK					= 8267,
	PROTO_CP_SUBMIT_TASK					= 8268,
	PROTO_CP_GET_NOTICE_INFO				= 8269,
	PROTO_CP_PROCESS						= 8270,
	PROTO_CP_GET_RAND_LUCKY					= 8271,
	PROTO_CP_GET_ILLUSTRATE					= 8272,
	PROTO_CP_UPGRADE_BUILDING				= 8273,
	PROTO_CP_USE_ITEM						= 8274,
	PROTO_CP_GET_TICKET						= 8275,

	PROTO_CP_USE_CARD						= 8277,
	PROTO_CP_GET_CARD_INFO					= 8278,
	PROTO_CP_EXCHANGE_PIG					= 8279,
	PROTO_CP_SET_BACKGROUND					= 8280,

	PROTO_CP_PIG_FOLLOW						= 8281,
	PROTO_CP_GET_SHOW_STAGE_INFO			= 8282,
	PROTO_CP_SHOW_PIG						= 8283,
	PROTO_CP_GET_2ND_GAME_INFO				= 8284,
	PROTO_CP_SWAP_PIG						= 8285,
	PROTO_GET_FEIFEI_DAREN_PRIZE			= 8276,
	PROTO_CP_GET_BEAUTY_GAME_PRIZE			= 8286,
	PROTO_CP_GET_BEAUTY_GAME_RECORD			= 8287,
	PROTO_CP_TELL_BEAUTY_LIMIT_GIFT			= 8288,
	PROTO_CP_GET_BEAUTY_LIMIT_GIFT			= 8289,
	PROTO_CP_BEAUTY_SHOW_WITH_NPC			= 8290,

	PROTO_CP_BEAUTY_SHOW_CHECK_ACHIEVE		= 8291,
	PROTO_CP_BEAUTY_SHOW_GET_ACHIEVE		= 8292,
	PROTO_CP_NPC_WEIGHT_PK					= 8293,
	PROTO_CP_NPC_WEIGHT_PK_CHECK			= 8294,

	PROTO_CP_ILLUSTRATE_EXCHANGE_PIG		= 8295,
	PROTO_CP_GET_ALL_HONOR					= 8296,

	PROTO_SAVE_AVATAR						= 8300,
	PROTO_REMOVE_AVATAR						= 8301,
	PROTO_GET_AVATAR_LIST					= 8302,


	PROTO_GET_VIP_ANGEL_INFO                = 8320,
	PROTO_EXCHANGE_VIP_ANGEL                = 8321,

	PROTO_CHP_DONATE						= 8350,
	PROTO_CHP_DONATE_NOTICE					= 8351,
	PROTO_CHP_GET_AUCTION_LIST				= 8352,
	PROTO_CHP_AUCTION						= 8353,
	PROTO_CHP_AUCTION_RESULT				= 8354,
	PROTO_DONATE_FOOD_ITEM					= 8355,
	PROTO_YEAR_FEAST_GET_ALL_INFO			= 8356,
	PROTO_YEAR_FEAST_WAHS_FOOD				= 8357,
	PROTO_YEAR_FEAST_COOK_FIRE				= 8358,
	PROTO_YEAR_FEAST_COOK_FOOD				= 8359,
	PROTO_YEAR_FEAST_SET_CAKE_POS			= 8360,
	PROTO_YEAR_FEAST_SET_TABLE_POS			= 8361,
	PROTO_YEAR_FEAST_GET_PRIZE				= 8362,
	PROTO_YEAR_FEAST_NEW_START				= 8363,

	PROTO_WVS_SWITCH_TEAM					= 8400,
	PROTO_WVS_GET_TEAM_POWER				= 8401,

	PROTO_LD_GET_AWARD						= 8450,
	PROTO_LD_ATTACK_SP_LANTERN				= 8451,
	PROTO_LD_GET_SP_LANTERN_INFO			= 8452,
	PROTO_LD_PLAYER_BE_ATTACKED				= 8453,

	PROTO_FP_GET_FLOWER						= 8480,
	PROTO_FP_SET_FLOWER						= 8481,
	PROTO_FP_OPEN_VIP_FLOWER				= 8482,
	PROTO_FP_VIP_FLOWER_CLOSE				= 8483,
	PROTO_FP_GET_AWARD						= 8484,
	PROTO_FP_GET_GAME_INFO					= 8485,
	PROTO_FP_BOUQUET_UPDATE					= 8486,

	PROTO_AC_CHANGE_FLAG					= 8500,
	PROTO_AC_AWARD							= 8501,
	PROTO_AC_AREA_REFRESH					= 8502,
	PROTO_AC_SET_SIT						= 8503,
	PROTO_AC_GET_GAME_INFO					= 8504,
	PROTO_AC_GET_AWARD						= 8505,

	PROTO_GET_USER_CUTEPIG_MACHINE_WORK		= 8506,
	PROTO_USER_CUTEPIG_EXPLOR_MAP_MINE		= 8507,
	PROTO_GET_USER_CUTEPIG_EXPLOR_MINE_INFO		= 8508,
	PROTO_CP_USER_PIGLET_MELT_ORE		        = 8509,
	PROTO_CP_USER_PRODUCE_MACHINE_PART		    = 8510,
	PROTO_CP_USER_PIGLET_FINISH_WORK		    = 8511,
	PROTO_CP_USER_PIGLET_WORK_MACHINE		    = 8512,
	PROTO_CP_USER_PIGLET_USE_ACCELERATE_TOOL    = 8513,
	PROTO_CP_USER_PIGLET_GUIDER_STEP_GET        = 8514,
	PROTO_CP_USER_PIGLET_GUIDER_STEP_SET        = 8515,
	PROTO_GET_GREAT_PRODUCE_ORDER               = 8516,
	PROTO_PUT_GREAT_PRODUCE_ORDER               = 8517,
	PROTO_FINISH_GREAT_PRODUCE_ORDER            = 8518,
	PROTO_CP_USER_PIGLET_PRODUCE_SPECIAL_MACHINE  = 8519,
	PROTO_MW_ADD_ITEM_IN_FRIENDSHIP_BOX		= 8520,
	PROTO_MW_GET_ITEM_FROM_FRIENDSHIP_BOX		= 8521,
	PROTO_MW_QUERY_ITEMS_FROM_FRIENDSHIP_BOX		= 8522,
	PROTO_MW_GET_ITEM_HISTORY_FROM_FRIENDSHIP_BOX	= 8523,
	PROTO_CP_USER_PIGLET_RANDOM_MACHINE_PRODUCT   = 8524,
	PROTO_PUT_USER_KFC_DECORATION               = 8525,
	PROTO_GET_USER_KFC_DECORATION               = 8526,
	PROTO_VOTE_KFC_FRIEND_DECORATION            = 8527,
	PROTO_GET_POLL_FRIEND_DECORATION            = 8528,
	PROTO_CP_USER_TAKE_BACK_MINING_PIGLET       = 8529,
	PROTO_MAKE_BIRHTHDAY_CAKE_ACTIVIEY			= 8530,
	PROTO_GET_CURRENT_BIRTHDAY_CAKE_INFO        = 8531,
	PROTO_GET_MAKE_BIRTHDAY_CAKE_AWARD          = 8532,
	PROTO_MAKE_BIRHTHDAY_START_CAKE_ACTIVIEY	= 8533,
	PROTO_JOIN_CRYSTAL_CELEBRATE_DANCE          = 8534,
	PROTO_CRYSTAL_WHOLE_DNANCE_SCENE         	= 8535,
	PROTO_TELL_CLIENT_TO_GET_DANCE_AWARD        = 8536,
	PROTO_TELL_ALL_PLAYERS_ROUND_CHAMPION_DANCE = 8537,
	PROTO_GET_RANDOM_DANCING_AWARD              = 8538,
	PROTO_CHECK_DANCING_THREE_TIMES              = 8539,
	PROTO_SET_TYPE_PHOTO_HEZHAO				     = 8540,
	PROTO_GET_TYPE_PHOTO_HEZHAO				     = 8541,
	PROTO_LIGHT_CELEBRATE_FOURYEAR_BOOK          = 8542,
	///
	PROTO_USER_WATER_JACK_AND_MODOU              = 8543,
	PROTO_GET_WATER_TIME_JACK_AND_MODOU          = 8544,
	///
	PROTO_SET_MOLE_MISS_NOTE                     = 8545,
	PROTO_MOLE_GET_ENERGY_STONE                  = 8546,
	PROTO_MOLE_GET_MISS_NOTE                     = 8547,
	PROTO_USER_SHAKE_DICE                        = 8548,
	PROTO_USER_GET_SIGNAL_FLARE                  = 8549,
	PROTO_USER_GET_SHAKE_DICE_LEFT_TIME          = 8550,
	PROTO_MOLE_CHECK_EXPEDITION_LOCK             = 8551,
	PROTO_MOLE_GET_MAKE_SHIP					 = 8552,
	PROTO_MOLE_FUNFLY_SEAHORSE                   = 8553,
	PROTO_USER_GUESS_SSCLOTH                     = 8554,
	PROTO_USER_GET_FREE_VIP_FLAYER               = 8555,
	PROTO_MOLE_MERMAN_KINGDOM                    = 8556,
	PROTO_USER_GET_LOTTERY_LEFT_TIME             = 8557,

	PROTO_USER_CONTEND_FOR_PRAY                  = 8558,
	PROTO_USER_GET_PRAY_INFO                     = 8559,
	PROTO_USER_SET_PRAY_STEP_INFO                = 8560,
	PROTO_USER_GET_PRAY_STEP_INFO                = 8561,
	PROTO_MOLE_GET_INVADE_MONSTER                = 8562,
	PROTO_MOLE_ATTACK_INVADE_MONSTER             = 8563,
	PROTO_MOLE_GET_SELF_GAMES                    = 8564,
	PROTO_MOLE_ENTER_GAME_HALL                   = 8565,
    PROTO_MOLE_GET_SCROLL_FRAGMENT               = 8566,
	PROTO_MOLE_GET_SCROLL_MAP_STATE              = 8567,
	PROTO_MOLE_DO_WORK_FOR_OTHERS                = 8568,
	PROTO_MOLE_MEET_MERMAID_AGAIN                = 8569,
	PROTO_MOLE_FINISH_MERMAN_KINGDOM             = 8570,



	PROTO_USER_GET_OCEAN_INFO                    = 8800,
	PROTO_USER_PUT_ANIMAL_IN_OCEAN               = 8801,
	PROTO_USER_FEED_OCEAN_ANIMAL                 = 8802,
	PROTO_USER_USE_OCEAN_TOOL                    = 8803,
	PROTO_USER_EXPAND_OCEAN_CAPACITY             = 8804,
	PROTO_USER_GET_OCEAN_ITEMS                   = 8805,
	PROTO_USER_DIY_SELF_OCEAN                    = 8806,
	PROTO_USER_GET_OFFLINE_SHELLS                = 8807,
	PROTO_USER_GET_ONLINE_SHELLS                 = 8808,
	PROTO_USER_PICK_ONLINE_SHELLS                = 8809,
	PROTO_USER_SALE_OCEAN_FISH                   = 8810,
	PROTO_USER_BUY_OCEAN_THINGS                   = 8811,
	PROTO_MOLE_GET_OCEAN_ADVENTURE               = 8812,
	PROTO_MOLE_SET_OCEAN_ADVENTURE               = 8813,
	PROTO_MOLE_GET_OCEAN_HANDBOOK                = 8814,
	PROTO_MOLE_GET_OCEAN_FRIEND_LEVEL            = 8815,





	PROTO_USER_JOIN_COSPLAY_RACE                 = 8820,
	PROTO_SYSARG_GET_COSPLAY_RANK                = 8821,
	PROTO_SYSARG_GET_COSPLAY_RANK_FALG          = 8822,

	PROTO_EXIT_NOTIFY						= 10001,
	PROTO_RELOGIN_NOTIFY					= 10002,
	PROTO_TEXT_NOTIFY						= 10003,
	PROTO_URL_NOTIFY						= 10004,
	PROTO_GAME_START_NOTIFY					= 10005,
	PROTO_GAME_END_NOTIFY					= 10006,
	PROTO_REDIR_TO_GAMESERV_NOTIFY			= 10007,
	PROTO_MAPITEM_CHANGE_NOTIFY				= 10008,
	PROTO_HOME_USERS_CHANGE_NOTIFY			= 10009,
	PROTO_PROMOTION_TIPS					= 10010,

	PROTO_EVENT_STATUS						= 10011,
	PROTO_UPD_ATTR_NOTI						= 10012,
	PROTO_UPD_5BASIC_ATTR					= 10013,
	PROTO_NOTIFY_JOIN_BLACK					= 10015,

	PROTO_CHK_IF_STH_DONE					= 10101,
	PROTO_SET_STH_DONE						= 10102,
	PROTO_CHK_STH_CNT						= 10103,
	PROTO_GET_CNT_LIST						= 10104,

	PROTO_POLL								= 10201,

	PROTO_GET_SYSTIME						= 10301,
	PROTO_GET_MD5_SESSION					= 10302,
	PROTO_CHK_VIP_INFO						= 10303,
	PROTO_GET_VIP_VALUE_AND_LEVEL			= 10304,

	PROTO_FB_NOTIFY_ATTACK					= 10401,
	PROTO_FB_NOTIFY_BOSS_HP					= 10402,
	PROTO_FB_NOTIFY_BOSS_ATTACK				= 10403,
	PROTO_FB_ENTER_TURRET_CMD				= 10404,
	PROTO_FB_LEAVE_TURRET_CMD				= 10405,
	PROTO_FB_GET_BOSS_HP_CMD				= 10406,
	PROTO_FB_NOTIFY_BOSS_DIE				= 10407,
	PROTO_FB_NOTIFY_BOSS_REVIVE				= 10408,

	// For Temporary Use Only
	PROTO_PERSONAL_INFO						= 11001,
	PROTO_IF_INFO_SUBMITTED					= 11002,
	PROTO_SUBMIT_MAGIC_CODE					= 11003,
	PROTO_BUY_REAL_ITEM						= 11004,
	PROTO_SET_GUARD_TASK_STATUS				= 11005,
	PROTO_SUBMIT_ONCE_STAND					= 11006,
	PROTO_UPDATE_MONEY						= 11007,
    //alive test
    PROTO_TEST_ALIVE						= 30000,
	// Mole Cup
	//PROTO_SET_TORCH_ON					= 11101,
	//PROTO_CHK_CUP_INFO					= 11102,
	//PROTO_CHK_CUP_HISTORY					= 11103,
	//PROTO_CHK_RECENT_MEDAL				= 11104,
	//PROTO_GET_CUP_PRISE					= 11105,

	// CmdID 60001 and Above Reserved for Scene Game
	// 60001 - 60005 Reserved For Seesaw
	PROTO_SEESAW_INFO						= 60001,
	// 60006 - 60010 Reserved For Mimic Show
	PROTO_MIMIC_SHOW_INFO					= 60006,
	//60011-60015 Reserve For Cheer Squad show
};

struct shm_block;

int init_proc_msg();
int parse_protocol(uint8_t *data, int rcvlen, fdsession_t* fdsess);

void rsp_search_user(sprite_t* chker, const sprite_t* chkee, uint16_t online_id);

void response_proto_get_sprite(sprite_t* p, int cmd, int is_detail, sprite_t* req);
void response_proto_get_sprite2(sprite_t* p, int cmd, sprite_t* req, int completed);
int  response_proto_list_items(sprite_t* p, uint32_t id, uint32_t* itms, int count);
int  response_proto_list_items2(sprite_t* p, uint32_t id, uint8_t* buf, int count);
void response_proto_use_items(sprite_t* p);
void response_proto_leave_map(sprite_t* p, int);
int  response_proto_user_home(sprite_t* p, uint32_t id, const char* data, int len);
int  pkg_proto_throw_item(uint8_t* buf, sprite_t* p, uint32_t itemid, int x, int y);

void notify_system_message(sprite_t* p, const char* txt, int len);
void new_notify_system_message(sprite_t *p, const char *txt, int len, int cmd);

void notify_add_friend_response(uint32_t id, sprite_t* rep, uint8_t accept);
void notify_user_exit(sprite_t* p, int reason, int del_node);
void notify_home_users_change(sprite_t* p, uint32_t count, int completed);
int  notify_systime(sprite_t* p, map_id_t* maps);
int send_msglog_to_bus(int cmd, int body_len, const void* body_buf, uint32_t id);

int as_send_msglog_cmd(sprite_t* p, uint8_t* body, int len);
int reply_add_friend_callback(sprite_t* p, uint32_t id, char* buf, int len);


extern uint8_t msg[2<<20];
extern char    admin[16];

static inline void
init_proto_head(void* buf, int cmd, int len)
{
#ifdef __cplusplus
	protocol_t* p = reinterpret_cast<protocol_t*>(buf);
#else
	protocol_t* p = buf;
#endif
	p->len = htonl(len);
	p->cmd = htonl(cmd);
	p->ver = 0x20;
	p->ret = 0;
}

static inline int
pack_user_tasks(const sprite_t* p, uint8_t* buf)
{
	int i, len = 0;
	for (i = 0; i != sizeof p->tasks; ++i) {
		int offset;
		for (offset = 0; offset != 8; offset += 2) {
			PKG_UINT8(buf, ((p->tasks[i] >> offset) & 0x3), len);
		}
	}

	return len;
}

static inline void
response_proto_head(sprite_t *p, int cmd, int broadcast)
{
	uint8_t msg[sizeof (protocol_t)];

	init_proto_head(msg, cmd, sizeof (protocol_t));
	if (broadcast)
		send_to_map(p, msg, sizeof (protocol_t), 1);
	else
		send_to_self(p, msg, sizeof (protocol_t), 1);
}

static inline void
response_proto_head_not_complete(sprite_t *p, int cmd, int broadcast)
{
	uint8_t msg[sizeof (protocol_t)];

	init_proto_head(msg, cmd, sizeof (protocol_t));
	if (broadcast)
		send_to_map(p, msg, sizeof (protocol_t), 0);
	else
		send_to_self(p, msg, sizeof (protocol_t), 0);
}

static inline void
response_proto_str(sprite_t *p, int cmd, uint32_t len, const void* buf, int broadcast)
{
	memcpy(msg + sizeof (protocol_t), buf, len);
	init_proto_head(msg, cmd, sizeof (protocol_t) + len);
	if (broadcast)
		send_to_map(p, msg, sizeof (protocol_t) + len, 1);
	else
		send_to_self(p, msg, sizeof (protocol_t) + len, 1);
}

static inline void
response_proto_uint8(sprite_t* p, int cmd, uint8_t val, int broadcast)
{
	*(uint8_t*)(msg + sizeof(protocol_t)) = val;
	init_proto_head(msg, cmd, sizeof(protocol_t) + 1);
	if (broadcast) {
		send_to_map(p, msg, sizeof(protocol_t) + 1, 1);
	} else {
		send_to_self(p, msg, sizeof(protocol_t) + 1, 1);
	}
}

static inline void
response_proto_uint32(sprite_t* p, int cmd, uint32_t val, int broadcast)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 4);
	if (broadcast) {
		send_to_map(p, msg, sizeof(protocol_t) + 4, 1);
	} else {
		send_to_self(p, msg, sizeof(protocol_t) + 4, 1);
	}
}

static inline void
response_proto_uint32_uint32(sprite_t* p, int cmd, uint32_t val1, uint32_t val2, int broadcast)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val1);
	*(uint32_t*)(msg + sizeof(protocol_t) + 4) = htonl(val2);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 8);
	if (broadcast) {
		send_to_map(p, msg, sizeof(protocol_t) + 8, 1);
	} else {
		send_to_self(p, msg, sizeof(protocol_t) + 8, 1);
	}
}

static inline void
response_proto_uint32_uint32_uint32(sprite_t* p, int cmd, uint32_t val1, uint32_t val2, uint32_t val3, int broadcast)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val1);
	*(uint32_t*)(msg + sizeof(protocol_t) + 4) = htonl(val2);
	*(uint32_t*)(msg + sizeof(protocol_t) + 8) = htonl(val3);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 12);
	if (broadcast) {
		send_to_map(p, msg, sizeof(protocol_t) + 12, 1);
	} else {
		send_to_self(p, msg, sizeof(protocol_t) + 12, 1);
	}
}

static inline void
response_proto_uint32_uint32_not_complete(sprite_t* p, int cmd, uint32_t val1, uint32_t val2, int broadcast)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val1);
	*(uint32_t*)(msg + sizeof(protocol_t) + 4) = htonl(val2);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 8);
	if (broadcast) {
		send_to_map(p, msg, sizeof(protocol_t) + 8, 0);
	} else {
		send_to_self(p, msg, sizeof(protocol_t) + 8, 0);
	}
}

static inline void
response_str_not_complete(sprite_t *p, int cmd, uint32_t len, const void* buf, int broadcast)
{
	memcpy(msg + sizeof (protocol_t), buf, len);
	init_proto_head(msg, cmd, sizeof (protocol_t) + len);
	if (broadcast)
		send_to_map(p, msg, sizeof (protocol_t) + len, 0);
	else
		send_to_self(p, msg, sizeof (protocol_t) + len, 0);
}

static inline void
response_uint8_not_complete(sprite_t* p, int cmd, uint8_t val, int broadcast)
{
	*(uint8_t*)(msg + sizeof(protocol_t)) = val;
	init_proto_head(msg, cmd, sizeof(protocol_t) + 1);
	if (broadcast) {
		send_to_map(p, msg, sizeof(protocol_t) + 1, 0);
	} else {
		send_to_self(p, msg, sizeof(protocol_t) + 1, 0);
	}
}

static inline void
response_uint32_not_complete(sprite_t* p, int cmd, uint32_t val, int broadcast)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 4);
	if (broadcast) {
		send_to_map(p, msg, sizeof(protocol_t) + 4, 0);
	} else {
		send_to_self(p, msg, sizeof(protocol_t) + 4, 0);
	}
}

static inline void
response_proto_head_map(map_id_t mapid, int cmd)
{
	init_proto_head(msg, cmd, sizeof(protocol_t));
	send_to_map3(mapid, msg, sizeof(protocol_t));
}

static inline void
response_proto_uint32_map(map_id_t mapid, int cmd, uint32_t val)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 4);

	send_to_map3(mapid, msg, sizeof(protocol_t) + 4);
}

static inline void
response_proto_uint32_uint32_map(map_id_t mapid, int cmd, uint32_t val1, uint32_t val2)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val1);
	*(uint32_t*)(msg + sizeof(protocol_t) + 4) = htonl(val2);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 8);

	send_to_map3(mapid, msg, sizeof(protocol_t) + 8);
}

static inline uint32_t
calc_ver(uint32_t pkglen, uint8_t ver, uint32_t cmd)
{
    return ver - ver/7 + 147 + pkglen%21 + cmd%13;
}

static inline uint32_t
calc_ver_crc(uint32_t pkglen, uint8_t ver, uint32_t cmd, uint8_t crc8_val)
{
    return ver - ver/7 + 147 + pkglen%21 + cmd%13 + crc8_val;
}

static inline uint8_t
calc_crc8(uint8_t* body, uint32_t body_len)
{
    uint8_t crc8_val = 0;
    uint32_t i = 0;
    for (i = 0; i < body_len; i++){
        crc8_val ^= (body[i] & 0xFF);
    }
    return crc8_val;
}

static inline uint8_t cmd_in_cmdlist(uint32_t cmd)
{
    if (cmd == 1243){
        return 1;
    }
    else{
        return 0;
    }
}


#endif
