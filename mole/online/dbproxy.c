#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <libtaomee/bitmanip/bitmanip.h>
#include <libtaomee/conf_parser/config.h>
#include <statistic_agent/msglog.h>
#include <libtaomee/log.h>
#include <libtaomee/dataformatter/bin_str.h>
#include "tcp.h"
#include "service.h"
#include "central_online.h"
#include "communicator.h"
#include "mole_card.h"
#include "event.h"
#include "exclu_things.h"
#include "item.h"
#include "pet.h"
#include "proto.h"
#include "protoheaders.h"
#include "homeinfo.h"
#include "login.h"
#include "map.h"
#include "message.h"
#include "mole_messenger.h"
#include "sprite.h"
#include "tasks.h"
#include "wish_pool.h"
#include "market.h"
#include "mole_group.h"
#include "birthday.h"
#include "mibiserver.h"
#include "gold_bean_serv.h"
#include "mole_bank.h"
#include "party.h"
#include "spring.h"
#include "small_require.h"
#include "pet_item.h"
#include "mole_homeland.h"
#include "magic_code.h"
#include "mole_question.h"
#include "cache_svr.h"
#include "gift.h"
#include "mole_class.h"
#include "mole_pasture.h"
#include "mole_happy_card.h"
#include "mole_doctor.h"
#include "game_pk.h"
#include "lahm_diary.h"
#include "mole_work.h"
#include "mole_car.h"
#include "mole_class.h"
#include "race_car.h"
#include "dbproxy.h"
#include "mole_candy.h"
#include "hallowmas.h"
#include "thanksgiving.h"
#include "npc_event.h"
#include "mcast.h"
#include "exclu_things.h"
#include "lamu_skill.h"
#include "delicous_food.h"
#include "profession_work.h"
#include "street.h"
#include "delicous_food.h"
#include "lahm_sport.h"
#include "rand_itm.h"
#include "mole_dragon.h"
#include "fire_cup.h"
#include "add_sth_rand.h"
#include "swap.h"
#include "game_bonus.h"
#include "school_bar_cmd.h"
#include "friendship_box.h"
#include "race_bonus.h"
#include "lahm_classroom.h"
#include "charity_drive.h"
#include "give_item_rank.h"
#include "snowball_war.h"
#include "christmas_sock.h"
#include "chris_cake.h"
#include "foot_print.h"
#include "mole_angel.h"
#include "mole_dungeon.h"
#include "angry_lahm.h"
#include "hide_and_seek.h"
#include "pasture.h"
#include "angel_fight.h"
#include "sand_pool.h"
#include "final_boss_2011.h"
#include "moon_cake.h"
#include "lucky_lottery.h"
#include "mole_cutepig.h"
#include "week_activity.h"
#include "xhx_server.h"
#include "mole_avatar.h"
#include "super_guider.h"
#include "charitable_party.h"
#include "winter_vs_spring.h"
#include "spring_couplets.h"
#include "year_feast.h"
#include "lanterns_day.h"
#include "mole_cake.h"
#include "mole_ocean.h"


#define SPRITE_BODY_LEN		370	//benchapi.h中sprite结构体的已定义字段长度

int proxysvr_fd = -1;

int chatlog_fd;
struct sockaddr_in chatlog_addr;

int udp_dbserver_fd;
struct sockaddr_in udp_dbserver_addr;

int monitor_server_fd;
struct sockaddr_in monitor_server_addr;

int udp_submit_fd;
struct sockaddr_in udp_submit_addr;

//int udp_third_soft_fd;
//struct sockaddr_in udp_third_soft_addr;

int chat_monitor_fd;
struct sockaddr_in chat_monitor_addr;


uint32_t db_timeout_flag = 0;

enum {
	dberr_db_error								= 1002,
	dberr_sys_error								= 1003,
	dberr_not_found								= 1015,
	dberr_invalid_data							= 1016,
	dberr_user_id_not_found						= 1105,
	dberr_class_id_existed						= 1109,
	dberr_not_own_the_item						= 1111,
	dberr_class_id_max							= 1115,
	dberr_not_enough_xiaomee					= 1117,
	dberr_not_enough_items						= 1119,
	dberr_existed_in_blacklist					= 1126,
	dberr_already_set_birthday					= 1127,
	dberr_max_pet_num_exceeded					= 1130,
	dberr_card_add_max							= 1145,
	dberr_card_inited 							= 1146,
	dberr_mole_groupid_not_existed				= 1150,
	dberr_mole_groupid_existed					= 1151,
	dberr_not_molegroup_owner					= 1152,
	dberr_max_molegroup_exceeded				= 1153,
	dberr_max_owned_molegroup_exceeded 			= 1154,
	dberr_max_num_in_molegroup_exceeded			= 1155,
	dberr_user_not_in_molegroup					= 1156,
	dberr_user_already_in_molegroup				= 1157,

	//for mole bank, 1117 is also need
	dberr_record_big_max_err 					= 1170,
	dberr_xiaomee_less_threshold_err			= 1171,
	dberr_refuse_get_err 						= 1172,
	dberr_record_not_exist_err 					= 1173,
	dberr_record_exist_err 						= 1174,
	dberr_invalid_time_limit_err 				= 1175,

	dberr_taskid_not_exist 						= 1176,

	dberr_sell_item_limit						= 1190,
	dberr_max_home_itm_num_exceeded				= 1201,

	//for homeland
	dberr_not_find_fruit						= 1202,
	dberr_fruit_num_exceed						= 1203,
	dberr_fruit_overdue							= 1204,
	dberr_plant_number_not_exist				= 1206,
	dberr_max_planted_seed_exceeded				= 1207,
	dberr_not_in_thief_time						= 2522,
	dberr_already_thief_this_plant				= 2523,

	dberr_max_times_fertilize_maomaotree		= 1311,
	dberr_maomaotree_op_type_error				= 1312,
	dberr_max_times_vote_smc					= 1313,
	//for christmas
	dberr_already_get_christmas_wish			= 1320,
	dberr_already_set_christmas_wish			= 1321,
	dberr_not_set_christmas_wish				= 1322,

	dberr_postcard_not_found					= 1702,

	dberr_already_made_wish						= 1801,
	dberr_wish_not_found						= 1802,
	dberr_max_party_exceed_per_day  			= 1810,

	//main_login
	dberr_verify_channel_failed					= 2002,

	//for mole class
	dberr_moleclass_existed						= 2503,
	dberr_moleclass_not_exist					= 2504,
	dberr_moleclass_max							= 2505,
	dberr_moleclass_member_max					= 2506,
	dberr_moleclass_mem_existed					= 2507,
	dberr_moleclass_mem_not_existed				= 2508,
	dberr_moleclass_msgs_exceeded 				= 2520,
	dberr_has_req_superlamn						= 2524,
	dberr_req_superlamn_max						= 2525,
	dberr_req_superlamn_fail					= 2529,

	dberr_animal_not_adult						= 2526,
	dberr_no_item_in_pasture_store				= 2527,
	dberr_animal_id_not_exist					= 2528,
	dberr_pasture_fish_be_locked				= 2530,
	dberr_already_fish_this_pasture				= 2531,
	dberr_happy_card_not_enough					= 2532,
	dberr_happy_card_not_ready					= 2533,
	dberr_happy_card_target_error				= 2534,
	dberr_small_animal_num_exceed				= 2535,
	dberr_small_fish_num_exceed					= 2536,
	dberr_adult_animal_num_exceed				= 2537,
	dberr_max_feed_num_exceed					= 2538,
	dberr_already_mem_get_reward				= 2539,
	dberr_already_get_reward					= 2540,
	dberr_already_car_race_sign					= 2543,
	dberr_sports_task_start						= 2544,
	dberr_sports_task_not_start					= 2545,
	dberr_sports_task_fin						= 2546,
	dberr_pkee_score_too_small					= 2548,
	dberr_diary_be_locked						= 2549,
	dberr_diary_id_not_exist					= 2550,
	dberr_diary_already_lock					= 2551,
	dberr_diary_already_unlock					= 2552,
	dberr_not_time_to_get_task2					= 2553,
	dberr_have_not_done_task2					= 2554,
	dberr_alrdy_attend_magic_task				= 2555,
	dberr_no_right_attend_magic					= 2556,
	dberr_have_not_done_task1					= 2557,
	dberr_closet_reach_max						= 2558,
	dberr_not_enough_gift_times					= 2560,
	dberr_car_not_exist							= 2565,
	dberr_car_exist								= 2569,
	dberr_alrdy_follow_animal					= 2572,
	dberr_have_not_been_gaoguai					= 2573,
	dberr_gaoguai_the_tree_times_exceed			= 2574,
	dberr_alrdy_gaoguai_this_tree				= 2575,
	dberr_too_more_fudan						= 2582,
	dberr_already_fu_egg						= 2583,
	dberr_take_task_too_many_times				= 2587,
	dberr_have_not_done_task					= 2588,
	dberr_have_not_take_task					= 2589,
	dberr_npc_lovely_not_enough					= 2590,
	dberr_reach_fudan_times						= 2591,
	dberr_egg_not_mature						= 2592,
	dberr_limited_itm_no_more					= 2598,
	dberr_alrdy_get_ring_this_home				= 2599,
	dberr_insect_reach_limit					= 2600,
	dberr_animal_cannot_eat_grass				= 2601,
	dberr_fudan_count_limit						= 2602,
	dberr_itm_cnt_in_pasture_limit				= 2603,
	dberr_plant_pollinated_limit				= 2604,
	dberr_stage_cannot_pollinated				= 2605,
	dberr_pollinate_too_many_times				= 2606,
	dberr_not_enough_chris_gift					= 2607,
	dberr_no_chris_gift							= 2609,
	dberr_nengliang_cnt_day_limit				= 2610,
	dberr_state_cannt_fertilized				= 2611,
	dberr_no_chemical_fertilizer				= 2612,
	dberr_fertilize_times_day_limit				= 2613,
	dberr_plant_nbr_not_exist					= 2614,
	dberr_pet_task_attire_have_given			= 2615,
	dberr_pet_task_not_completed				= 2616,
	dberr_carrry_rock_too_many_today			= 2617,
	dberr_illegal_state_change_of_task			= 2624,
	dberr_illegal_state_change_1_1				= 2629,
	dberr_illegal_state_change_2_1				= 2628,
	dberr_max_home_msgs_exceeded				= 6020,
	dberr_cow_out_of_got_milk_times				= 2635,
	dberr_cow_have_got_milk_today				= 2634,
	dberr_illegal_roomid						= 2637,
	dberr_dish_eat_empty						= 2638,
	dberr_dish_is_cooking						= 2639,
	dberr_dish_not_exsit						= 2640,
	dberr_employee_count_limit					= 2641,
	dberr_dish_count_limit						= 2642,
	dberr_no_sys_employee						= 2643,
	dberr_cooking_time_not_enough				= 2644,
	dberr_all_employee_timeout					= 2645,
	dberr_this_location_used					= 2646,
	dberr_can_not_change_style					= 2647,
	dberr_cooking_bad_timeout					= 2648,
	dberr_need_more_resource					= 2656,
	dberr_already_set_team						= 2657,
	dberr_not_set_team							= 2658,
	dberr_you_team_hove_not_win					= 2662,
	dberr_you_have_got_sth						= 2663,
	dberr_not_enough_dish						= 2664,
	dderr_sport_medal_get_day_limit				= 2667,
	dderr_sport_diff_time_little				= 2668,
	dderr_no_have_gua_gua_card					= 2672,
	dberr_type_not_exsit						= 2673,
	dberr_new_card_have_init					= 2674,
	dberr_dragon_egg_id							= 2677,
	dberr_have_the_dragon						= 2678,
	dberr_hatch_time_not_enough					= 2686,
	dberr_have_egg_hatching						= 2687,
	dberr_session_check_error					= 4331,
	dberr_dbproxy_timeout						= 1017,
	dberr_max_limit_businessman_goods			= 2683,
	dberr_businessman_have_not_this_goods		= 2682,
	dberr_dragon_have_growth_max				= 2680,
	dberr_have_get_dragon_max					= 2685,
	dberr_have_no_dragon_err					= 2688,
	dberr_egg_have_become_dragon				= 2689,
	dberr_medal_limit_fire_cup					= 2690,
	dberr_have_not_enough_fire_cup				= 2691,
	dberr_box_six_item_limit					= 2692,
	dberr_box_no_item_to_share					= 2693,
	dberr_box_not_enough_item_to_share			= 2694,
	dberr_box_share_limit_today					= 2695,
	dberr_box_shared_today						= 2696,
	dberr_box_too_many_item						= 2697,
	dberr_get_firecup_prize_aready				= 2698,
	dberr_box_not_have_friendship_box			= 2700,
	dberr_lahm_classroom_not_exist				= 2701,
	dberr_points_too_small						= 2702,
	dberr_gift_num_not_exsit					= 2703,
	dberr_gift_already_get						= 2704,
	dberr_lahm_classroom_no_student				= 2705,
	dberr_lahm_classroom_courseid_err			= 2706,
	dberr_lahm_classroom_class_end_early		= 2707,
	dberr_lahm_classroom_class_begin_aready		= 2708,
	dberr_lahm_classroom_not_exam_state			= 2709,
	dberr_lahm_classroom_graduate_err			= 2710,
	dberr_lahm_classroom_add_course_hour_err	= 2712,
	dberr_lahm_classroom_class_hour_limit_err	= 2713,
	dberr_lahm_classroom_add_course_score_err	= 2714,
	dberr_lahm_classroom_max_student_err		= 2715,
	dberr_lahm_classroom_class_max_event_err	= 2716,
	dberr_charity_drive_guess_not_time_err		= 2718,
	dberr_charity_drive_guess_aready_err		= 2719,
	dberr_lahm_classroom_fellowship_max_limit	= 2720,
	dberr_lahm_classroom_fellowship_aready_in	= 2721,
	dberr_lahm_classroom_fellowship_not_student	= 2722,
	dberr_get_give_item_bonus_count_not_enough	= 2723,
	dberr_give_item_date_max_value				= 2725,
	dberr_give_item_aready_this_user			= 2724,
	dberr_not_enough_mibi_to_exchange			= 2726,
	dberr_christmas_send_sock_max_today			= 2727,
	dberr_christmas_can_not_send_sock			= 2728,
	dberr_chris_cake_score_not_enough			= 2729,
	dberr_have_make_this_cake					= 2730,
	dberr_have_give_cake_to_npc					= 2731,
	dberr_footprint_count_not_enough			= 2732,
	dberr_buy_pet_item_item_max					= 2734,
	dberr_farm_animal_use_item_max				= 2736,
	dberr_farm_animal_item_not_enough			= 2737,
	dberr_animal_use_skill_cold_time_limit		= 2738,
	dberr_animal_use_skill_max_time_limit		= 2739,
	dberr_use_skill_prize_item_over				= 2740,
	dberr_catch_lvl_animal_num_max				= 2741,
	dberr_nimsbus_value_err						= 2742,
	dberr_not_enough_attire_err					= 2743,
	dberr_not_correct_post_err					= 2744,
	dberr_not_all_angel_seed_pos_full_err		= 2745,
	dberr_plant_engel_alread_done				= 2746,
	dberr_angel_not_mature_err					= 2747,
	dberr_not_enough_engel_for_free				= 2748,
	dberr_exeed_every_inc_value_in_paradise_err	= 2749,
	dberr_angelid_mapping_to_dragonid_err		= 2750,
	dberr_dragon_cannot_follow_err				= 2751,
	dberr_no_fairy_followed_you_err				= 2752,
	dberr_followed_fairy_not_mature_err			= 2753,
	dberr_not_right_attire_exchange_nimsbus_err	= 2754,
	dberr_angel_seed_id_not_exist_err			= 2755,
	dberr_level_is_not_exsit_err				= 2756,
	dberr_user_not_attend_battle_err			= 2757,
	dberr_the_angel_already_mature_err			= 2758,
	dberr_angel_speed_tool_reached_used_limit_err	= 2759,
	dberr_exceed_one_battle_prize_err				= 2760,
	dberr_not_correct_angel_speed_tool_err			= 2761,
	dberr_capture_angel_fail_err					= 2762 ,
	dberr_not_correct_angel_capture_tool_err		= 2763,
	dberr_not_the_angel_already_variate_success_err	= 2764,
	dberr_not_the_angel_already_variate_err			= 2765,
	dberr_not_correct_angel_go_out_hospital_err		= 2766,
	dberr_the_angel_has_recovered_err				= 2767,
	dberr_the_tool_cannot_used_for_this_angel_err	= 2768,
	dberr_prop_used_for_angel_already_err		= 2769,
	dberr_regain_angel_no_angel_on_show			= 2770,
	dberr_show_angel_invalid_angel_id			= 2771,
	dberr_get_angel_honor_aready_err			= 2773,
	dberr_get_angel_honor_unlock_err			= 2772,
	dberr_compose_angel_level_err				= 2774,
	dberr_compose_angel_not_enough_material_err		= 2775,
	dberr_dungeon_not_correct_hp_recover_item_err	= 2776,
	dberr_dungeon_hp_is_full_err				= 2777,
	dberr_dungeon_invalid_exhibit_id_err		= 2778,
	dberr_dungeon_invalid_exhibit_state_err		= 2779,
	dberr_angry_lahm_invalid_task_id_err		= 2780,
	dberr_the_ground_already_plant_err			= 2781,
	dberr_not_enough_milk_to_purchase_err		= 2782,
	dberr_use_items_level_limit					= 2783,
	dberr_level_up_card_count_not_enough		= 2784,
	dberr_change_equip_count_limit 				= 2785,
	dberr_refresh_daily_task_wrong 				= 2786,
	dberr_time_send_mail_aready 				= 2787,
	dberr_master_apprentice_err 				= 2788,
	dberr_no_time_mail_err 						= 2789,
	dberr_no_time_item_err 						= 2790,
	dberr_cp_feed_invalid_fodder				= 2791,
	dberr_not_friend_userid_err					= 2792,
	dberr_already_get_candy_err					= 2793,
	dberr_not_enough_share_candy_err			= 2794,
	dberr_get_item_count_out_of_max				= 2795,
	dberr_get_sprint_festival_day_err		    = 2796,
	dberr_get_sprint_festival_bonus_aready_err	= 2797,
	dberr_cp_produce_machine_machine_lvl_limit_err	    = 2798,
	dberr_shake_dice_time_too_short_err	         = 2799,
	dberr_ocean_shell_coin_not_enough_err        =2801
};

// inline function declarations
static int
items_op_return(sprite_t *p, uint32_t id, char *buf, int len);
static inline int
set_task_callback(sprite_t* p, uint32_t id, char* buf, int len);
static inline int
chk_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len);


int send_request_to_chatlog_server(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];
	server_proto_t *pkg;
	int len;

	if ( body_len > sizeof(dbbuf) - sizeof(*pkg) )
		ERROR_RETURN (("package too large, uid=%u", p ? p->id : 0), -1);

	len = sizeof (server_proto_t) + body_len;
	pkg = (server_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	pkg->seq = 0;
	pkg->ret = 0;
	memcpy (pkg->body, body_buf, body_len);

	sendto(chatlog_fd, dbbuf, len, 0, &chatlog_addr, sizeof chatlog_addr);
	return 0;
}

int send_request_to_chat_monitor_server(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	if(chat_monitor_fd == -1)
	{
		return 0;
	}
	static uint8_t dbbuf[PAGESIZE];
	server_proto_t *pkg;
	int len;

	if ( body_len > sizeof(dbbuf) - sizeof(*pkg) )
		ERROR_RETURN (("package too large, uid=%u", p ? p->id : 0), -1);

	len = sizeof (server_proto_t) + body_len;
	pkg = (server_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	pkg->seq = 0;
	pkg->ret = 0;
	memcpy (pkg->body, body_buf, body_len);

	sendto(chat_monitor_fd, dbbuf, len, 0, &chat_monitor_addr, sizeof chat_monitor_addr);
	return 0;
}



int send_request_to_udp_dbserver(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];
	server_proto_t *pkg;
	int len;

	if ( body_len > sizeof(dbbuf) - sizeof(*pkg) )
		ERROR_RETURN (("package too large, uid=%u", p ? p->id : 0), -1);

	len = sizeof (server_proto_t) + body_len;
	pkg = (server_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	pkg->seq = 0;
	pkg->ret = 0;
	memcpy (pkg->body, body_buf, body_len);

	sendto(udp_dbserver_fd, dbbuf, len, 0, &udp_dbserver_addr, sizeof udp_dbserver_addr);
	return 0;
}

int send_request_to_monitor_dbserver(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];
	server_proto_t *pkg;
	int len;

	if ( body_len > sizeof(dbbuf) - sizeof(*pkg) )
		ERROR_RETURN (("package too large, uid=%u", p ? p->id : 0), -1);

	len = sizeof (server_proto_t) + body_len;
	pkg = (server_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	pkg->seq = 0;
	pkg->ret = 0;
	memcpy (pkg->body, body_buf, body_len);

	sendto(monitor_server_fd, dbbuf, len, 0, &monitor_server_addr, sizeof monitor_server_addr);
	return 0;
}

int send_request_to_udp_submit_server(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];
	server_proto_t *pkg;
	int len;

	if ( body_len > sizeof(dbbuf) - sizeof(*pkg) )
		ERROR_RETURN (("package too large, uid=%u", p ? p->id : 0), -1);

	len = sizeof (server_proto_t) + body_len;
	pkg = (server_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	pkg->seq = 0;
	pkg->ret = 0;
	memcpy (pkg->body, body_buf, body_len);

	sendto(udp_submit_fd, dbbuf, len, 0, &udp_submit_addr, sizeof udp_submit_addr);
	return 0;
}

static int db_return_expired(void* owner, void* data)
{
	sprite_t* p = owner;

	ERROR_LOG("DB TIMEOUT\t[uid=%u waitcmd=%u dbcmd=0x%X]", p->id, p->db_timer_waitcmd, p->db_timer_dbcmd);
	p->db_timer = NULL;
	return send_err_to_monitor(p->waitcmd, p->id, 1);
}

// function definitions
int send_request_to_db(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];

	server_proto_t *pkg;
	int len;

	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("dbproxy_svr"), 0, 65535, 1);
		//proxysvr_fd = connect_to_svr(config_get_strval("dbproxy_ip"), config_get_intval("dbproxy_port", 0), 65535, 1);
	}

	len = sizeof (server_proto_t) + body_len;
	if (proxysvr_fd == -1 || proxysvr_fd > epi.maxfd ||
			epi.fds[proxysvr_fd].cb.sndbufsz < epi.fds[proxysvr_fd].cb.sendlen + len ||
			body_len > sizeof(dbbuf) - sizeof(*pkg)) {
		if (p) {
			send_err_to_monitor(p->waitcmd, p->id, 5);
		}
		if (proxysvr_fd != -1) {
			ERROR_LOG("send to dbproxy failed, buflen=%d, fd=%d",
					epi.fds[proxysvr_fd].cb.sendlen, proxysvr_fd);
		}
		if (p && p->waitcmd != PROTO_LOGIN) {
			return send_to_self_error(p, p->waitcmd, -ERR_system_error, 1);
		}
		return -1;
	}

	if (db_timeout_flag && p && p->waitcmd != PROTO_LOGIN) {
		p->db_timer = ADD_TIMER_EVENT(p, db_return_expired, 0, get_now_tv()->tv_sec + 5);
		p->db_timer_waitcmd = p->waitcmd;
		p->db_timer_dbcmd = cmd;
	}

	pkg = (server_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	if (!p) pkg->seq = 0;
	else pkg->seq = (sprite_fd (p) << 16) | p->waitcmd;
	pkg->ret = 0;
	memcpy (pkg->body, body_buf, body_len);
	return net_send (proxysvr_fd, dbbuf, len, 0);
}

/*
 * flag:
 * 	del_unused_itm	0x00
 * 	add_itm		0x01
 *    unused_itm     0x02
 * 	used_itm		0x03
 * 	del_used_itm	0x04
 */

int db_single_item_op(sprite_t* p, uint32_t uid, uint32_t itmid, int count, int flag)
{
	int j;
	char buffer[16];

	const item_t* itm = get_item_prop(itmid);
	if (!itm) {
		ERROR_RETURN(("Item %u not found: uid=%u", itmid, (p ? p->id : uid)), -1);
	}

	j = 0;
	PKG_H_UINT32(buffer, itmid, j);
	PKG_H_UINT32(buffer, flag, j);
	PKG_H_UINT32(buffer, count, j);
	if (!flag || (flag == 4)) {
		PKG_H_UINT32(buffer, 99999, j);
	} else {
		PKG_H_UINT32(buffer, itm->max, j);
	}

	return send_request_to_db(SVR_PROTO_SET_ITEM, p, 16, buffer, uid);
}

/*
 * @prief 增加装扮，有最大值限制，如果数量大于最大值，设置为最大值
 */
int db_single_item_op_max_limit(sprite_t* p, uint32_t uid, uint32_t itmid, int count)
{
	int j;
	char buffer[16];
	const item_t* itm = get_item_prop(itmid);
	if (!itm) {
		ERROR_RETURN(("Item %u not found: uid=%u", itmid, (p ? p->id : uid)), -1);
	}

	j = 0;
	PKG_H_UINT32(buffer, itmid, j);
	PKG_H_UINT32(buffer, count, j);
	PKG_H_UINT32(buffer, itm->max, j);

	return send_request_to_db(SVR_PROTO_SET_ITEM_MAX_LIMIT, p, j, buffer, uid);
}

int db_single_item_op_exchange(sprite_t * p,uint32_t uid,uint32_t itmid,int count,int flag)
{
	int j = 0;
	uint8_t buf[pagesize];

	if(flag) {
		PKG_H_UINT32(buf, 0, j);
		PKG_H_UINT32(buf, 1, j);
		PKG_H_UINT32(buf, NOT_INCLUDE_MONEY_DAY_LIMIT, j);
		PKG_H_UINT32(buf, 0, j);

		if (pkg_item_kind(p, buf, itmid, &j) == -1) {
			return -1;
		}
		item_t *itm = get_item_prop(itmid);
		if(itm == NULL) {
			return -1;
		}
		PKG_H_UINT32(buf, itmid, j);
		PKG_H_UINT32(buf, count , j);
		PKG_H_UINT32(buf, itm->max, j);
	} else {
		PKG_H_UINT32(buf, 1, j);
		PKG_H_UINT32(buf, 0, j);
		PKG_H_UINT32(buf, 0, j);
		PKG_H_UINT32(buf, 0, j);

		if (pkg_item_kind(p, buf, itmid, &j) == -1) {
			return -1;
		}
		PKG_H_UINT32(buf, itmid, j);
		PKG_H_UINT32(buf, count, j);
	}
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buf, uid);
}

int pkg_item_kind(sprite_t *p, uint8_t* buf, uint32_t itmid, int* indx)
{
	item_kind_t* kind = find_kind_of_item(itmid);
	if (!kind) {
		ERROR_RETURN(("bad itmid\t"), -1);
	}
	uint32_t flag = 0;
	if (kind->kind == HOME_ITEM_KIND) {
		flag = 1;
	} else if (kind->kind == HOMELAND_ITEM_KIND) {
		flag = 2;
	} else if (kind->kind == HAPPY_CARD_KIND) {
		flag = 6;
	} else if (kind->kind == CAR_KIND) {
		flag = 8;
	} else if (kind->kind == CARD_ITEM_KIND) {
		flag = 10;
	} else if (kind->kind == RESTAURANT_ITEM_KIND) {
		flag = 26;
	} else if (kind->kind == DRAGON_KIND) {
		flag = 27;
	} else if (kind->kind == PET_ITEM_CLOTH_KIND) {	
		/*如果加的是拉姆装扮，传过去的FLAG为100+拉姆的ID号*/

        if (!p){
            return -1;
        }
        
		if ( p && p->followed == NULL ) {
			send_to_self_error(p, p->waitcmd, -ERR_the_lahm_not_followed, 1);
			ERROR_LOG("NOT FOLLOW PET %u", p->id);
			return -1;
		} else {
			flag = 100 + p->followed->id;
		}
	} else if (kind->kind == TEMP_ITEM_KIND) {
		flag = 99;
	} else if (kind->kind == FIGHT_ITEM_KIND || kind->kind == FIGHT_CARD_KIND || kind->kind == FIGHT_COLLECT_KIND){
		flag = 32;
	} else if (kind->kind == CUTE_PIG_KIND || kind->kind == CUTE_PIG_ITEM_KIND || kind->kind == CUTE_PIG_CLOTHES_KIND) {
		flag = 36;
	}
	PKG_H_UINT32(buf, flag, *indx);
	return 0;
}

int db_exchange_items(sprite_t* p, const exchange_item_t* exitm)
{
	uint8_t buf[1024];

	const exclu_things_t* exclu = NULL;
	if (p->waitcmd == PROTO_SET_STH_DONE) {
		exclu = *((const exclu_things_t**)(p->session));
	}

	int out_kind = (exitm->outkind_cnt ? (rand() % exitm->outkind_cnt) : 0);
	// No Daily Limit For VIP ATM (1503)
	int attr_chg_reason = ((exitm->id != 1503 && exitm->id != 1612) ? ATTR_CHG_task_fin : ATTR_CHG_roll_back);
	if (exitm->id == 1503) {
		uint32_t val = 1;
		msglog(statistic_logfile, 0x0207EEEC, get_now_tv()->tv_sec, &val, 4);
	}

	if (exitm->id < 600) {
		if (exits[exitm->id].not_limit) {
			attr_chg_reason = NOT_INCLUDE_MONEY_DAY_LIMIT;
		}
	}

	int i, j = 0;
	// Package For DB
	PKG_H_UINT32(buf, exitm->incnt, j);
	PKG_H_UINT32(buf, exitm->outcnt[out_kind], j);
	PKG_H_UINT32(buf, attr_chg_reason, j);
//	PKG_H_UINT32(buf, exitm->id, j);
	PKG_H_UINT32(buf, NOT_LIMIT_BY_OTHER_STH_MAX, j);
	for (i = 0; i != exitm->incnt; ++i) {
		if (pkg_item_kind(p, buf, exitm->in[i].itm->id, &j) == -1)
			return -1;
		PKG_H_UINT32(buf, exitm->in[i].itm->id, j);
		PKG_H_UINT32(buf, exitm->in[i].count, j);
	}
	memset(p->session + 9, 0, 20);
	//
	p->sess_len = sizeof(protocol_t);
	if (p->waitcmd == PROTO_SET_STH_DONE) {
		PKG_UINT32(p->session + 29, exclu->id, p->sess_len);
	}
	//
	PKG_UINT32(p->session + 29, exitm->outcnt[out_kind], p->sess_len);
	for (i = 0; i != exitm->outcnt[out_kind]; i++) {
		// Package For DB
		if (pkg_item_kind(p, buf, exitm->out[out_kind][i].itm->id, &j) == -1)
			return -1;
		PKG_H_UINT32(buf, exitm->out[out_kind][i].itm->id, j);
		if ((exitm->id == 1503) && (p->vip_level >= 6)) {
			PKG_H_UINT32(buf, exitm->out[out_kind][i].count * 2, j);
		} else {
			PKG_H_UINT32(buf, exitm->out[out_kind][i].count, j);
		}
		PKG_H_UINT32(buf, exitm->out[out_kind][i].itm->max, j);

		grow_profession(p, exitm->out[out_kind][i].itm->id, exitm->out[out_kind][i].count);
		add_foot_print_count(p, exitm->out[out_kind][i].itm->id, exitm->out[out_kind][i].count);

		// pack attr for adding on later

		if ((exitm->id == 1503) && (p->vip_level >= 6)) {
			pack_exchg_attr(p->session + 8, exitm->out[out_kind][i].itm->id, exitm->out[out_kind][i].count * 2);
		} else {
			pack_exchg_attr(p->session + 8, exitm->out[out_kind][i].itm->id, exitm->out[out_kind][i].count);
		}
		// Package For Client
		PKG_UINT32(p->session + 29, exitm->out[out_kind][i].itm->id, p->sess_len);

		if ((exitm->id == 1503) && (p->vip_level >= 6)) {
			PKG_UINT32(p->session + 29, exitm->out[out_kind][i].count * 2, p->sess_len);
		} else {
			PKG_UINT32(p->session + 29, exitm->out[out_kind][i].count, p->sess_len);
		}
	}
	init_proto_head(p->session + 29, p->waitcmd, p->sess_len);
	// To Set Task
	*((uint32_t*)p->session) = exitm->id;
	DEBUG_LOG("exchange set task %u %u", p->id, exitm->id);
	*((uint32_t*)(p->session + 4)) = exitm->flag_to_set[out_kind];

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buf, p->id);
}

int db_exchg_collected_items(sprite_t* p, const exchange_item_t* exitm, uint32_t item_num)
{
	int j = 16, out_count;

	if (pkg_item_kind(p, msg, exitm->in[0].itm->id, &j) == -1)
		return -1;
	PKG_H_UINT32(msg, exitm->in[0].itm->id, j);
	if (exitm->in[0].count == 9999) {
		PKG_H_UINT32(msg, item_num, j);
	} else {
		PKG_H_UINT32(msg, exitm->in[0].count, j);
	}

	int i = 0, itmcnt;
	p->sess_len = sizeof(protocol_t) + 4;
	memset(p->session + 9, 0, 20);
	int out_kind = (exitm->outkind_cnt ? (rand() % exitm->outkind_cnt) : 0);
	for (; i != MAX_EXCHANGE_ITEMS; ++i) {
		if (!exitm->out[out_kind][i].itm) break;
		// Package For DB
		if (pkg_item_kind(p, msg, exitm->out[out_kind][i].itm->id, &j) == -1)
			return -1;
		PKG_H_UINT32(msg, exitm->out[out_kind][i].itm->id, j);
		if (exitm->in[0].count == 9999) {
			itmcnt = exitm->out[out_kind][i].count * item_num;
			PKG_H_UINT32(msg, itmcnt, j);
		} else {
			itmcnt = exitm->out[out_kind][i].count;
			PKG_H_UINT32(msg, itmcnt, j);
		}
		PKG_H_UINT32(msg, exitm->out[out_kind][i].itm->max, j);
		// pack attr for adding on later
		pack_exchg_attr(p->session + 8, exitm->out[out_kind][i].itm->id, itmcnt);
		// Package For Client
		PKG_UINT32(p->session + 29, exitm->out[out_kind][i].itm->id, p->sess_len);
		PKG_UINT32(p->session + 29, itmcnt, p->sess_len);

	}
	out_count = i;

	i = 0;
	PKG_H_UINT32(msg, 1, i);          // in count
	PKG_H_UINT32(msg, out_count, i);  // out count
	PKG_H_UINT32(msg, ATTR_CHG_task_fin, i);
	PKG_H_UINT32(msg, exitm->id, i);
	// Package For Client
	i = sizeof(protocol_t);
	PKG_UINT32(p->session + 29, out_count, i);
	init_proto_head(p->session + 29, p->waitcmd, p->sess_len);

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, msg, p->id);
}

int db_items_op(sprite_t *p)
{
	int i;

	assert (p->sess_len);
	for (i = 0; i < *(int*)p->session; i++) {
		struct user_itm_use_buf *itms = (struct user_itm_use_buf *)
				&p->session[8 + i * sizeof(struct user_itm_use_buf)];
		if (db_single_item_op(p, p->id, itms->id, 1, itms->mask) == -1)
			return -1;
	}

	return 0;
}

int db_buy_items(sprite_t* p, const item_t* itm, int count, const item_kind_t* kind, int free, int ret_needed)
{
	int i, money;
	char buf[20];

	i = 0;
	uint32_t flag = 0;
	if (kind->kind == HOME_ITEM_KIND) {
		flag = 1;
	} else if (kind->kind == HOMELAND_ITEM_KIND) {
		flag = 2;
	} else if (kind->kind == HAPPY_CARD_KIND) {
		flag = 6;
	} else if (kind->kind == CUTE_PIG_KIND ||
		kind->kind == CUTE_PIG_ITEM_KIND ||
		kind->kind == CUTE_PIG_CLOTHES_KIND) {
		flag = 36;
	}
	PKG_H_UINT32(buf, flag, i);
	PKG_H_UINT32(buf, itm->id, i);
	PKG_H_UINT32(buf, count, i);
	PKG_H_UINT32(buf, itm->max, i);

	money = (free ? 0 : (itm->price * count));
	// judge if discount day
	const event_t* ev = get_event((kind->kind == HOME_ITEM_KIND || kind->kind == HOMELAND_ITEM_KIND) ? 10 : 5);
	if (ev->status) {
		if (kind->kind == HOME_ITEM_KIND || kind->kind == HOMELAND_ITEM_KIND || kind->kind == SUIT_ITEM_KIND) {
			money *= 0.5;
		}
		//money *= itm->discount;
	}

	PKG_H_UINT32(buf, money, i);

	return send_request_to_db(SVR_PROTO_BUY_ITEM, (ret_needed ? p : 0), 20, buf, p->id);
}

int db_buy_items_by_xiaomee(sprite_t* p, const item_t* itm, int count, const item_kind_t* kind, int xiaomee, int ret_needed)
{
	int i;
	char buf[20];

	i = 0;
	uint32_t flag = 0;
	if (kind->kind == HOME_ITEM_KIND) {
		flag = 1;
	} else if (kind->kind == HOMELAND_ITEM_KIND) {
		flag = 2;
	} else if (kind->kind == HAPPY_CARD_KIND) {
		flag = 6;
	}
	PKG_H_UINT32(buf, flag, i);
	PKG_H_UINT32(buf, itm->id, i);
	PKG_H_UINT32(buf, count, i);
	PKG_H_UINT32(buf, itm->max, i);
	PKG_H_UINT32(buf, xiaomee, i);

	return send_request_to_db(SVR_PROTO_BUY_ITEM, (ret_needed ? p : 0), 20, buf, p->id);
}

int db_buy_items_by_ocean_shells(sprite_t* p, const item_t* itm, int count, const item_kind_t* kind, int free, int ret_needed)
{
	int i = 0;
	char buf[20];
	uint32_t flag = 0;
		
	uint32_t money = (free ? 0 : (itm->shellprice * count));
	PKG_H_UINT32(buf, flag, i);
	PKG_H_UINT32(buf, itm->id, i);
	PKG_H_UINT32(buf, count, i);
	PKG_H_UINT32(buf, itm->max, i);
	PKG_H_UINT32(buf, money, i);

	return send_request_to_db(SVR_PROTO_BUY_OCEAN_THINGS, (ret_needed ? p : 0), 20, buf, p->id);
}

int db_exchange_single_item_op(sprite_t *p, uint32_t flag, uint32_t itmid, int count, int is_return)
{
	uint8_t buff[1024] = {};
	int j = 0;
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 1, j);
	PKG_H_UINT32(buff, flag, j);
	PKG_H_UINT32(buff, 0, j);
	if (pkg_item_kind(p, buff, itmid, &j) == -1)
	{
		return -1;
	}

	item_t* p_item = get_item_prop(itmid);
	if (!p_item)
	{
		return -1;
	}
	PKG_H_UINT32(buff, p_item->id, j);
	PKG_H_UINT32(buff, count, j);
	PKG_H_UINT32(buff, p_item->max, j);

	if (is_return)
	{
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);
	}
	else
	{
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, buff, p->id);
	}
	return 0;
}

int db_delete_single_item_op(sprite_t *p, uint32_t flag, uint32_t itmid, int count, int is_return)
{
	uint8_t buff[1024] = {};
	int j = 0;
	PKG_H_UINT32(buff, 1, j);
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, flag, j);
	PKG_H_UINT32(buff, 0, j);
	if (pkg_item_kind(p, buff, itmid, &j) == -1)
	{
		return -1;
	}

	item_t* p_item = get_item_prop(itmid);
	if (!p_item)
	{
		return -1;
	}
	PKG_H_UINT32(buff, p_item->id, j);
	PKG_H_UINT32(buff, count, j);

	if (is_return)
	{
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);
	}
	else
	{
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, buff, p->id);
	}
	return 0;
}

int db_add_single_item_op(uint32_t userid, uint32_t flag, uint32_t itmid, int count)
{
	uint8_t buff[1024] = {};
	int j = 0;
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 1, j);
	PKG_H_UINT32(buff, flag, j);
	PKG_H_UINT32(buff, 0, j);
	if (pkg_item_kind(NULL, buff, itmid, &j) == -1)
	{
		return -1;
	}

	item_t* p_item = get_item_prop(itmid);
	if (!p_item)
	{
		return -1;
	}
	PKG_H_UINT32(buff, p_item->id, j);
	PKG_H_UINT32(buff, count, j);
	PKG_H_UINT32(buff, p_item->max, j);

	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, buff, userid);
	return 0;
}


typedef struct ChgHomeItmReq {
	uint32_t	itmid;
	uint32_t	chg_flag; /* 0: Remove Unused, 1: Add Unused, 2: Remove Used */
	uint32_t	chg_cnt;
	uint32_t	max_cnt; /* Used Only If Flag==1 */
} __attribute__((packed)) chg_home_itm_req_t;

int db_chg_home_itm(sprite_t* p, userid_t uid, uint32_t itmid, uint32_t cnt, uint32_t chg_flag)
{
	chg_home_itm_req_t req;

	const item_t* itm = get_item_prop(itmid);
	if (itm) {
		req.itmid     = itmid;
		req.chg_flag  = chg_flag;
		req.chg_cnt   = cnt;
		req.max_cnt   = itm->max;

		DEBUG_LOG("CHG HOME ITM\t[er_uid=%u ee_uid=%u itmid=%u flag=0x%X cnt=%u]",
					p ? p->id : 0, uid, itmid, chg_flag, cnt);
		return send_request_to_db(SVR_PROTO_CHG_HOME_ITEM, p, sizeof req, &req, uid);
	}

	ERROR_RETURN(("Invalid ItmID=%u er_uid=%u ee_uid", itmid, p ? p->id : 0 ), -1);
}

static int
attr_op_return(sprite_t *p, uint32_t id, char *buf, int len)
{
typedef struct msglog_statistic{
	uint32_t yxb;
	uint32_t exp;
	uint32_t strong;
	uint32_t iq;
	uint32_t lovely;
	uint32_t one;
	uint32_t userid;
} __attribute__((packed)) msglog_statistic_t;

	CHECK_BODY_LEN(len, 20);

	int i, err;
	i = 0;
	UNPKG_H_UINT32(buf, p->exp, i);
	UNPKG_H_UINT32(buf, p->strong, i);
	UNPKG_H_UINT32(buf, p->iq, i);
	UNPKG_H_UINT32(buf, p->lovely, i);
	UNPKG_H_UINT32(buf, p->yxb, i);

	switch (p->waitcmd) {
	case PROTO_BUY_PET:
		assert(p->sess_len == sizeof (pet_t));
		err = db_add_pet(p, ((pet_t*)p->session));
		return err;
	case PROTO_PET_TRUST:
		err = do_trust_pet(p);
		break;
	case PROTO_PICK_PET_TASK:
		err = do_pick_pet_task(p);
		break;
	case PROTO_GAME_SCORE:
	{
		int i = sizeof(protocol_t);
		uint32_t* pInt = (void*)(p->session + i + 32);
		uint32_t gid = *(uint32_t*)(p->session + i);
		gid = ntohl(gid);
		msglog_statistic_t tmp;
		tmp.yxb = ntohl(pInt[4]);
		tmp.exp = ntohl(pInt[0]);
		tmp.strong = ntohl(pInt[1]);
		tmp.iq = ntohl(pInt[2]);
		tmp.lovely = ntohl(pInt[3]);
		tmp.one = 1;
		tmp.userid = id;
		msglog(statistic_logfile, 0x02070000 + gid, get_now_tv()->tv_sec, &tmp, sizeof(msglog_statistic_t));
		send_to_self(p, p->session, ntohl(*(int *)p->session), 1);
		DEBUG_LOG("send game %u", p->id);
		p->sess_len = 0;
		if( !tmp.yxb && !tmp.strong && !tmp.iq && !tmp.lovely && !tmp.exp)
		{
		    return 0;
		}
		return 	sendto_db_game_items(p, p->session + i + 32 + 20);
	}
	case PROTO_UPD_5BASIC_ATTR:
		notify_5basic_attr(p);
		break;
	case PROTO_COLLECT_ITEM:
		CHECK_BODY_LEN(p->sess_len, 25);
		send_to_self(p, p->session, p->sess_len, 1);
		p->sess_len = 0;
		break;
	case PROTO_SEND_POSTCARD:
		{
			const msg_send_buf_t* sendbuf = (void*)(p->session);
			db_send_postcard(p, &(sendbuf->card), sizeof(postcard_content_send_t) + sendbuf->card.msglen, sendbuf->rcvers[0]);
		}
		break;
	case PROTO_MAKE_WISH:
		db_add_wish(p, p->session);
		p->sess_len = 0;
		break;
	case PROTO_FETCH_REALIZED_WISH:
		response_proto_head(p, p->waitcmd, 0);
		break;
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}

	return 0;
}

int db_attr_op(sprite_t* p, int yxb, int exp, int strong, int iq, int lovely, uint32_t reason, uint32_t detailed_reason)
{
	if (!yxb && !strong && !iq && !lovely && !exp)
		return attr_op_return(p, p->id, (char*)&p->exp, 20);

	return do_db_attr_op(p, p->id, yxb, exp, strong, iq, lovely, reason, detailed_reason);
}

enum enum_update_user_type {
	UPDATE_USER_STAGE = 1,
	UPDATE_USER_NICK,
	UPDATE_USER_PASSWD,
	UPDATE_USER_OTHER
};

static inline int
uint32_return (sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN (len, 4);
	if(p->waitcmd != PROTO_POST_BLACKBOARD)
		ERROR_RETURN(("error waitcmd=%d, id=%u", p->waitcmd, p->id), -1);

	response_proto_uint32(p, p->waitcmd, *(uint32_t*)buf, 0);
	return 0;
}

static inline int
no_body_return(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief 任务完成获取物品，全场景可见
 */
void broad_cast_when_fini_task(sprite_t *p, int completed)
{
	uint8_t buf[PAGESIZE];
	int j = sizeof(protocol_t);
	PKG_UINT32(buf, 1, j); /*1表示任务类型*/
	PKG_UINT32(buf, p->id, j);
	memcpy(buf + j, p->session + 29 + sizeof(protocol_t), p->sess_len - sizeof(protocol_t));
	j += (p->sess_len - sizeof(protocol_t));
	init_proto_head(buf, PROTO_BROAD_CAST_WHEN_FINI_TASK, j);
	p->sess_len = 0;
	send_to_map(p, buf, j, completed);
	return;
}

static void
unpkg_board(char *buf)
{
	int i, j;
	uint32_t tmp;

	i = j = 0;
	//msgid
	UNPKG_H_UINT32(buf, tmp, i);
	PKG_UINT32(buf, tmp, j);
	//userid
	UNPKG_H_UINT32(buf, tmp, i);
	PKG_UINT32(buf, tmp, j);
	//time
	UNPKG_H_UINT32(buf, tmp, i);
	PKG_UINT32(buf, tmp, j);
	//hot
	UNPKG_H_UINT32(buf, tmp, i);
	PKG_UINT32(buf, tmp, j);
	//color
	UNPKG_H_UINT32(buf, tmp, i);
	PKG_UINT32(buf, tmp, j);
}

static inline int
find_board_return(sprite_t *p, uint32_t id, char *buf, int len)
{
	if (p->waitcmd != PROTO_FIND_BLACKBOARD || len != 1060)
		ERROR_RETURN(("error waitcmd=%d, id=%u, len=%d", p->waitcmd, p->id, len), -1);

	unpkg_board(buf);
	response_proto_str(p, p->waitcmd, len, buf, 0);
	return 0;
}

static int
get_board_return (sprite_t *p, uint32_t id, char *buf, int len)
{
	int i, count, total;
	if (p->waitcmd != PROTO_GET_BLACKBOARD || len < 4)
		ERROR_RETURN (("error waitcmd=%d, id=%u, len=%d", p->waitcmd, p->id, len), -1);

	i = 0;
	UNPKG_H_UINT32 (buf, total, i);
	UNPKG_H_UINT32 (buf, count, i);
	CHECK_BODY_LEN (len - 8, 236 * count);
	if (count > 6 || count < 0)
		ERROR_RETURN (("error id=%u, count=%d", p->id, count), -1);

	for (i = 0; i < count; i++)
		unpkg_board (buf + 8 + i * 236);
	*(uint32_t*)buf = htonl (total);
	*(uint32_t*)(buf + 4) = htonl (count);

	response_proto_str (p, p->waitcmd, len, buf, 0);
	return 0;
}

static int
get_passed_boardmsg_by_id_callback (sprite_t *p, uint32_t id, char *buf, int len)
{
	int i, count;
	CHECK_BODY_LEN_GE(len, 4);

	i = 0;
	UNPKG_H_UINT32 (buf, count, i);
	CHECK_BODY_LEN (len - 4, 236 * count);

	for (i = 0; i < count; i++)
		unpkg_board (buf + 4 + i * 236);
	*(uint32_t*)buf = htonl (count);

	response_proto_str (p, p->waitcmd, len, buf, 0);
	return 0;
}

static inline int
get_home_return (sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_INT_GE(len, 20);

	switch (p->waitcmd) {
	case PROTO_USER_HOME:
		response_proto_user_home(p, id, buf, len);
		break;
	default:
		break;
	}

	return 0;
}

static inline int
reset_home_callback (sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + count * 8);

	uint32_t itmid, itm_cnt;
	int k, i, j;
	j = 4;
	i = sizeof(protocol_t);
	PKG_UINT32 (msg, count, i);
	for (k = 0; k < count; k++) {

		UNPKG_H_UINT32 (buf, itmid, j);
		PKG_UINT32 (msg, itmid, i);
		UNPKG_H_UINT32 (buf, itm_cnt, j);
		PKG_UINT32 (msg, itm_cnt, i);

		i += 8;
		j += 8;
	}
	init_proto_head (msg, PROTO_RESET_HOME, i);

	DEBUG_LOG ("RESET HOME CALLBACK\t[%u %u %d]", p->id, id, count);
	return send_to_self (p, msg, i, 1);
}

static int set_vip_level_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN (len, 0);


	return 0;
}

static int
send_mail_return (sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN (len, 0);
	switch (p->waitcmd) {
	case PROTO_TALK:
		DEBUG_LOG ("MAIL TALK\t[%u %u]", p->id, id);
		break;
	case PROTO_FRIEND_ADD:
		DEBUG_LOG ("MAIL REQUEST\t[%u %u]", p->id, id);
		break;
	case PROTO_RSP_FRIEND_ADD:
	case 0:
		DEBUG_LOG ("MAIL RSPONSE\t[%u %u]", p->id, id);
		break;
	default:
		ERROR_RETURN (("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}
	p->waitcmd = 0;

	return 0;
}

static int
get_bklist_return (sprite_t *p, uint32_t id, char *buf, int len)
{
	int count, i, j, k;
	uint32_t bkuser;
	uint8_t out[1024];

	if (len < 4 || p->id != id || p->waitcmd != PROTO_BKLIST_GET)
		ERROR_RETURN (("error len=%d, id=%u,%u, waitcmd=%d", len, p->id, id, p->waitcmd), -1);

	i = 0;
	UNPKG_H_UINT32(buf, count, i);
	CHECK_BODY_LEN(len, 4 + count * 4);
	CHECK_INT_LE(count, 100);

	k = sizeof (protocol_t);
	PKG_UINT32 (out, count, k);
	for (j = 0; j < count; j++) {
		UNPKG_H_UINT32 (buf, bkuser, i);
		PKG_UINT32 (out, bkuser, k);
	}

	init_proto_head (out, PROTO_BKLIST_GET, k);
	return send_to_self (p, out, k, 1);
}


static int
user_check_in_black_callback (sprite_t *p, uint32_t id, char *buf, int len)
{
	int is_in = 0;
	int i = 0;

	UNPKG_H_UINT32(buf, is_in, i);

	switch(p->waitcmd)
	{
		case PROTO_FRIEND_ADD:
		{
			if (is_in)
			{
				return send_to_self_error(p, p->waitcmd, -ERR_user_in_user_black_list, 1);
			}

			uint32_t userid = *(uint32_t*)p->session;
			return send_request_to_db( SVR_PROTO_ADD_FRIEND_REQ, p, sizeof(userid), &userid, p->id );
		}
		case PROTO_MAP_ENTER:
		{
			uint32_t gdst,gsrc;
			map_id_t newmap, oldmap;
			newmap = *(uint64_t*)p->session;
			oldmap = *(uint64_t*)(p->session + 8);
			gdst = *(uint32_t*)(p->session + 16);
			gsrc = *(uint32_t*)(p->session + 20);
			if (is_in)
			{
				send_to_self_error(p, p->waitcmd, -ERR_user_in_other_user_black_list, 0);
			}
			return do_enter_map(p, newmap, oldmap,gdst,gsrc);
		}
		default:
		{
			ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
		 }
	}

}
static int
add_friend_req_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t result;
	CHECK_BODY_LEN( len, sizeof(result) );
	result = *(uint32_t*)buf;
	if( result == 1 )
	{
		uint32_t userid = *(uint32_t*)p->session;
		return do_add_friend_req(p, userid);
	}
	else
	{
		return send_to_self_error( p, p->waitcmd, ERR_add_friend_req_failed, 1 );
	}
}


static inline int
set_user_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN (len, 0);

	switch (p->waitcmd) {
	case PROTO_SET_STAGE:
		response_proto_get_sprite2(p, p->waitcmd, p, 1);
		break;
	case PROTO_NICK_MODIFY:
		response_proto_get_sprite2(p, p->waitcmd, 0, 1);
		break;
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}

	return 0;
}

static inline int
del_friend_return (sprite_t *p, uint32_t id, char *buf, int len)
{
	if (len != 0 || p->id != id)
		ERROR_RETURN(("error len=%d, id=%u,%u", len, p->id, id), -1);

	switch (p->waitcmd) {
	case PROTO_FRIEND_DEL:
		response_proto_head(p, PROTO_FRIEND_DEL, 0);
		break;
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}
	return 0;
}

static inline int
add_friend_return(sprite_t* p, uint32_t id, char* buf, int len)
{
	if (len != 0 || p->id == id)
		ERROR_RETURN(("error len=%d, id=%u,%u, waitcmd=%d", len, p->id, id, p->waitcmd), -1);

	response_proto_head(p, PROTO_RSP_FRIEND_ADD, 0);
	notify_add_friend_response(id, p, 1);
	return 0;
}

static int
items_op_return(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t itm, used, total;
	int *cnt, *req;
	struct user_itm_use_buf *itms;
	int i;

	if (len != 12 || ( p->id != id && p->waitcmd != PROTO_GET_CANDY_FROM_OTHER))
		ERROR_RETURN(("error len=%d, id=%u, %u", len, p->id, id), -1);

	i = 0;
	UNPKG_H_UINT32(buf, itm, i);
	UNPKG_H_UINT32(buf, used, i);
	UNPKG_H_UINT32(buf, total, i);

	switch (p->waitcmd) {
	case PROTO_RAND_ITEM_SWAP:
		return db_add_rand_item(p);
	case PROTO_SET_MAGIC_MAN_EAT_DISH:
		return send_request_to_db(SVR_PROTO_GIVE_DISH_NPC, p, 12, p->session, p->id);
	case PROTO_NEWYAER_ITEM_EXCHANGE:
		return do_newyear_exchange_items(p);
	case PROTO_GIVE_NPC_SOME_ITEMS:
		return do_give_npc_items(p);
	case PROTO_ITEM_DISCARD:
		response_proto_head(p, PROTO_ITEM_DISCARD, 0);
		break;
	case PROTO_COLOR_MODIFY:
		return db_set_color(p);
	case PROTO_MODIFY_PET_COLOR:
	{
		uint32_t petid = *(uint32_t*)p->session;
		uint32_t color = *(uint32_t*)(p->session + 4);
		return do_set_pet_color(p, petid, color);
	}
	//FIXME: prevent external plugin
	case PROTO_SHOW_ITEM_USE:
		do_show_item_use_callback(p, itm);
		break;
	case PROTO_THROW:
		assert (p->sess_len > 0);
		send_to_map(p, p->session, p->sess_len, 1);
		p->sess_len = 0;
		break;
	case PROTO_FIND_ITEM:
	case PROTO_COLLECT_ITEM:
		i = sizeof(protocol_t);
		PKG_UINT32(msg, itm, i);
		PKG_UINT32(msg, 1, i);
		init_proto_head(msg, p->waitcmd, i);
		send_to_self(p, msg, i, 1);
		break;
	case PROTO_USER_ITEM_USE:
		assert (p->id == id);
		assert (p->sess_len);

		cnt = (int *)p->session;
		req = (int *)(p->session + 4);
		assert (*cnt  > *req && *cnt > 0);

		//search item mask
		for (i = 0; i < *cnt; i++) {
			itms = (struct user_itm_use_buf *)(p->session + 8 + i * sizeof (struct user_itm_use_buf));
			if (itms->id == itm)
				break;
		}
		if (i == *cnt)
			ERROR_RETURN(("can't find item: cnt=%d, itm=%d, id=%u", *cnt, itm, p->id), -1);

		if ((itms->mask & ADD_ITEM_MASK) && wear_item(p, itm) == -1)
			return -1;
		if (!(itms->mask & ADD_ITEM_MASK) && unwear_item(p, itm) == -1)
			return -1;
		(*req)++;

		if (*cnt  == *req) {
			response_proto_use_items(p);
			broadcast_sprite_info(p);
			p->sess_len = 0;
		}
		break;
	case PROTO_FEED_ITEM_USE:
		return feed_pet_callback(p);
	case PROTO_GET_ITEM_FROM_BAOHE:
		return del_item_from_baohe(p);
	case PROTO_PASTURE_ADD_FEED:
		return db_pasture_add_feed(p, p->session + 4, *(uint32_t*)p->session);
	case PROTO_GET_SHEEP:
		return do_get_sheep2(p);
	case PROTO_GET_JIAZHAO:
		return db_set_drive_time(p, p->id);
	case PROTO_CHANGE_SELF_NANGUA:
		return do_change_self_nangua(p);
	case PROTO_EXCHANGE_PROP:
	{
		uint32_t tmp[] = {0, 1, p->driver_time};
		return send_request_to_db(SVR_PROTO_CANDY_PROP, p, 12, tmp, p->id);
	}
	case PROTO_GET_CANDY_FROM_OTHER:
	{
		db_single_item_op(0, p->id, 190441, *(uint32_t*)p->session, 1);
		response_proto_uint32(p, p->waitcmd, *(uint32_t*)p->session, 0);
		return 0;
	}
	case PROTO_GIFT_DRAW:
		return do_draw(p);
	case PROTO_WEIGHTING_FISH:
		return add_weight_to_total(p);
	case PROTO_ADD_NENG_LIANG_XING_CNT:
	{
		uint32_t real_cnt = *(uint32_t*)(p->session + 4);
		response_proto_uint32(p, p->waitcmd, real_cnt, 0);
		return 0;
	}
	case PROTO_GET_XIAO_XIONG_CAR:
	{
		return db_add_car(p, 1300010, 0, p->id);
	}
	case PROTO_EXCHG_MIBI_COUPON:
	{
		return add_mibi_consume_coupon(p, 500);
	}
	case PROTO_FERTILIZE_FLOWER:
	{
		userid_t userid = GET_UID_IN_JY(p->tiles->id);
		uint32_t* tmp_buf = (uint32_t*)p->session;
		DEBUG_LOG("FERTILIZE FRUIT\t[%u %u number=%u]", p->id, userid, tmp_buf[1]);
		return db_fertilize_flower(p, tmp_buf, userid);
	}
	case PROTO_EXCHANGE_RAND_ITEM:
	{
		uint32_t itemid = *(uint32_t*)p->session;
		return db_buy_pet_items(p, p->followed->id, itemid, 1, 1, p->id);
	}
	case PROTO_GET_MONEY_BY_GUA_GAU_CARD:
		return get_money_by_gua_gua_card_callback(p, id, buf, len);
	case PROTO_USE_LAHM_CLASSROOM_ITEMS:
	{
		uint32_t itemid = *(uint32_t*)p->session;
		uint32_t petid = *(uint32_t*)(p->session + 4);
		return lahm_classroom_use_item_callback(p, itemid, petid);
	}
	case PROTO_LD_ATTACK_SP_LANTERN:
	{
		return ld_attack_sp_lanterns_callback(p);
	}
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}
	return 0;
}

static inline int
use_body_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	int put_off_cnt, put_on_cnt;
	int j = 0;
	UNPKG_H_UINT32(buf, put_off_cnt, j);
	put_off_cnt = (put_off_cnt > 12) ? 12 : put_off_cnt;
	put_on_cnt = *(uint32_t*)p->session;

	p->item_cnt = put_on_cnt;
	DEBUG_LOG("USE BODY ITEM CALLBACK\t[%u %u %u]", p->id, put_off_cnt, put_on_cnt);
	int i = sizeof (protocol_t);
	PKG_UINT32 (msg, p->id, i);
	int k = i;
	i += 4;

	CHECK_BODY_LEN(len, 4 * (put_off_cnt + 1));
	uint32_t* itm_off = (uint32_t*)(buf + j);
	uint32_t* itm_on = (uint32_t*)(p->session + 4);
	int loop1;
	for (loop1 = 0; loop1 < put_off_cnt; loop1++) {
		int loop2;
		for (loop2 = 0; loop2 < put_on_cnt; loop2++) {
			if (itm_off[loop1] == itm_on[loop2]) {
				itm_off[loop1] = 0;
			}
		}
	}
	int loop, real_off_cnt = 0;
	for (loop = 0; loop < put_off_cnt; loop++) {
		if (itm_off[loop] != 0) {
			PKG_UINT32 (msg, itm_off[loop], i);
			PKG_UINT8 (msg, 0, i);
			real_off_cnt ++;
		}
	}
	for (loop = 0; loop < put_on_cnt; loop++) {
		p->items[loop] = itm_on[loop];
		PKG_UINT32 (msg, itm_on[loop], i);
		PKG_UINT8 (msg, 1, i);
	}
	PKG_UINT32 (msg, real_off_cnt + put_on_cnt, k);

	init_proto_head (msg, p->waitcmd, i);
	send_to_map (p, msg, i, 1);
	broadcast_sprite_info(p);
	return 0;
}

static int
get_items_return(sprite_t *p, uint32_t id, char *buf, int len)
{
	int count, i, k;
	sprite_t *who;

	if (len < 4)
		ERROR_RETURN (("error len=%d", len), -1);

	i = 0;
	UNPKG_H_UINT32(buf, count, i);

	int explen = count * 8, rcvlen = len - 4;
	CHECK_BODY_LEN(rcvlen, explen);

	switch (p->waitcmd) {
	case PROTO_USER_DETAIL:
	case PROTO_USER_SIMPLY:
		who = (sprite_t *)p->session;
		assert (who != NULL && who->id == id);

		for (i = 0, k = 4; i < count && i < MAX_ITEMS_WITH_BODY; i++, k += 4)
			UNPKG_H_UINT32(buf, who->items[i], k);
		who->item_cnt = i;
		if (count > MAX_ITEMS_WITH_BODY) {
			k += 8 * (count - MAX_ITEMS_WITH_BODY);
		}

		response_proto_get_sprite(who, p->waitcmd, p->waitcmd == PROTO_USER_DETAIL, p);

		p->sess_len = 0;
		break;

	case PROTO_ITEM_LIST:
	case PROTO_GET_HOMELAND_BOX_ITEM:
	case PROTO_PASTURE_GET_FEED_ITEM:
	case PROTO_GET_STOREHOUSE_ITEM:
	case PROTO_GET_LAHM_CLASSROOM_ITEMS:
	case PROTO_GET_FOOT_PRINT_ITEMS:
	case PROTO_GET_PROP_FOR_TD:
		return response_proto_list_items2 (p, id, (uint8_t*)buf + 4, count);
	case PROTO_CP_USE_CARD:
		{
			return cutepig_get_card_count_callback( p, id, buf, len );
		}
	case PROTO_AF_GET_DONATE_INFO:
		{
			uint32_t count;
			uint32_t item_id;
			uint32_t item_cnt = 0;
			int j = 0;
			UNPKG_H_UINT32( buf, count, j );
			if( count != 0 )
			{
				UNPKG_H_UINT32( buf, item_id, j );
				UNPKG_H_UINT32( buf, item_cnt, j );
			}
			response_proto_uint32( p, p->waitcmd, item_cnt, 0 );
			return 0;
		}
	default:
		ERROR_RETURN (("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}

	return 0;
}

static int
chk_items_return(sprite_t *p, uint32_t id, char *buf, int len)
{
	int count, i;

	if (len < 4)
		ERROR_RETURN (("error len=%d", len), -1);

	i = 0;
	UNPKG_H_UINT32(buf, count, i);
	int explen = count * 8, rcvlen = len - 4;
	CHECK_BODY_LEN(rcvlen, explen);

	switch (p->waitcmd) {
	case PROTO_GET_EAT_DISH_RAND:
	{
		uint32_t itmid,itmcn = 0;
		if(count > 0) {
			UNPKG_H_UINT32(buf, itmid, i);
			UNPKG_H_UINT32(buf, itmcn, i);
		}
		if(count != 1 || itmcn == 0 || itmid != 16011) {
			return send_to_self_error(p, p->waitcmd, -ERR_not_have_item_16011, 1);
		}
		return send_request_to_db(SVR_PROTO_GET_USER_DISH_ALL,p,0,NULL,p->id);
	}
	case PROTO_CHK_ITEM_AMOUNT:
		return response_proto_list_items2 (p, id, (uint8_t*)buf + 4, count);
	case PROTO_GET_LOTTERY:
		return calculate_lottery(p, (uint8_t*)buf + 4, count);
	case PROTO_GET_MAGIC_CARD_REWARD:
		return do_get_magic_card_reward(p, count);
	case PROTO_GET_CHAR_CLOTH:
		return do_get_char_cloth(p, count);
	case PROTO_EXCHG_XUANFENG_JIJILE:
		return do_exchg_xuanfeng_jijile(p, count);
	case PROTO_GET_SHEEP:
		return do_get_sheep(p, count, (uint8_t*)buf + 4);
	case PROTO_GET_XUANFENG_CAR:
	{
		return do_get_xuanfeng_car(p, count, (uint8_t*)buf + 4);
	}
	case PROTO_CANDY_EXCH:
	{
		uint32_t itmid,itmcn = 0;
		if (count > 0) {
			UNPKG_H_UINT32(buf, itmid, i);
			UNPKG_H_UINT32(buf, itmcn, i);
		}
		if ((count != 1 && count != 0) || (count ==1 && itmcn > 200)) {
			return send_to_self_error(p, p->waitcmd, -ERR_candy_total_count_reach, 1);
		}
		return send_request_to_db(SVR_PROTO_CANDY_GET, p, 0, NULL, p->id);
	}
	case PROTO_EXCHANGE_PROP:
	{
		*(uint32_t*)p->session = count;
		uint32_t tmp[] = {0, 1, p->driver_time};
		return send_request_to_db(SVR_PROTO_CANDY_PROP, p, 12, tmp, p->id);
	}
	case PROTO_GIFT_DRAW:
	{
		uint32_t items[] = {13053, 160516, 160517, 160518, 888888};
		uint32_t itmid = items[0];
		if (count > 0) {
			itmid = items[rand()%4 + 1];
		}
		if (itmid != items[4])
			db_buy_itm_freely(0, p->id, itmid, 1);
		response_proto_uint32(p, p->waitcmd, itmid, 0);
		return 0;
	}
	case PROTO_GET_VIP_INVATATION:
	{
		if (count > 0) {
			return db_set_sth_done(p, 210, 3, p->id);

		}
		db_single_item_op(0, p->id, -3, 1, 1);
		response_proto_uint32(p, p->waitcmd, -2, 0);
		return 0;
	}
	case PROTO_BEAUTY_COMPETE:
	{
		return do_beauty_dress_compete_update(p, buf, len);
	}
	case PROTO_GET_DAJIAOYIN_REWARD:
	{
		return do_get_dajiaoyin_reward(p, count);
	}
	case PROTO_GET_VIP_LEVEL_GIFTS:
	{
		return vip_level_mole_gifts_car(p, count);
	}
	case PROTO_FB_ENTER_TURRET_CMD:
	{
		return fb_enter_bomb_trret(p, count);
	}
	case PROTO_XHX_CHECK_USER:
	{
		return xhx_check_user_callback(p, id, buf, len);
	}
	case PROTO_JAIL_USER:
	{
		UNPKG_H_UINT32(buf, count, i);
		UNPKG_H_UINT32(buf, count, i);
		do_jail_user(p, *(uint32_t*)p->session, *(uint32_t*)(p->session+4), *(uint32_t*)(buf+8));
		response_proto_head(p, p->waitcmd, 0);
		return 0;
	}
	case PROTO_HANG_CHRISTMAS_SOCK:
	{
		return hang_christmas_scok_callback(p, id, buf, len);
	}
	
	default:
		ERROR_RETURN (("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}

	return 0;
}

static inline int
unpkg_sprite(sprite_t* who, char* buf)
{
	int i = 0, loop;
	uint32_t flag;

	UNPKG_H_UINT32(buf, flag, i); // not used now
	UNPKG_H_UINT32(buf, flag, i);
	flag = set_bit_off32(flag, 1);
	who->flag |= flag;
	UNPKG_H_UINT32(buf, who->color, i);
	UNPKG_H_UINT32(buf, who->birth, i);
	UNPKG_H_UINT32(buf, who->yxb, i);
	UNPKG_H_UINT32(buf, who->exp, i);
	UNPKG_H_UINT32(buf, who->strong, i);
	UNPKG_H_UINT32(buf, who->iq, i);
	UNPKG_H_UINT32(buf, who->lovely, i);
	UNPKG_H_UINT32(buf, who->game_king, i);
	UNPKG_H_UINT32(buf, who->engineer, i);
	UNPKG_H_UINT32(buf, who->fashion, i);
	UNPKG_H_UINT32(buf, who->cultivate_skill, i);
	UNPKG_H_UINT32(buf, who->breed_skill, i);
	UNPKG_H_UINT32(buf, who->cult_day_limit, i);
	UNPKG_H_UINT32(buf, who->breed_day_limit, i);
	for (loop = 0; loop < MAX_NPC; loop ++) {
		UNPKG_H_UINT16(buf, who->npc_lovely[loop], i);
	}

	UNPKG_H_UINT32(buf, who->olcount, i);
	UNPKG_H_UINT32(buf, who->oltoday, i);
	UNPKG_H_UINT32(buf, who->ollast, i);
	UNPKG_H_UINT32(buf, who->oltime, i);
	UNPKG_H_UINT32(buf, who->birthday, i);
	UNPKG_STR(buf, who->tasks, i, 150);
	UNPKG_H_UINT32(buf, who->pet_task, i);
	UNPKG_H_UINT32(buf, who->magic_task, i);
	UNPKG_H_UINT32(buf, who->inviter_id, i);
	UNPKG_H_UINT32(buf, who->invitee_cnt, i);
	UNPKG_H_UINT32(buf, who->old_invitee_cnt, i);
	UNPKG_H_UINT32(buf, who->dining_flag, i);
	UNPKG_H_UINT32(buf, who->dining_level, i);
	UNPKG_H_UINT32(buf, who->dragon_id, i);
	UNPKG_STR(buf, who->dragon_name, i, DRAGON_NICK_LEN);
	UNPKG_H_UINT32(buf, who->dragon_growth, i);
	UNPKG_H_UINT32(buf, who->teach_exp, i);
	UNPKG_H_UINT32(buf, who->exam_num, i);
	UNPKG_H_UINT32(buf, who->classroom_student, i);
	UNPKG_H_UINT32(buf, who->animal_count, i);
	UNPKG_H_UINT32(buf, who->dungeon_explore_exp, i);
	UNPKG_H_UINT32(buf, who->cp_level, i);
	UNPKG_H_UINT32(buf, who->cp_exp, i);
	UNPKG_H_UINT32(buf, who->mvp_team, i);
	UNPKG_H_UINT32(buf, who->paradise_leve, i);
	UNPKG_H_UINT32(buf, who->game_exp, i);
	return i;
}

static inline int
get_sprite_with_mail_return(sprite_t* p, uint32_t id, char* buf, int len)
{
#define MIN_MAIL_SIZE		(SPRITE_BODY_LEN + 8)

	if ( (len < MIN_MAIL_SIZE) || (p->waitcmd != PROTO_LOGIN) || (p->id != id)
		|| (len > (SPRITE_STRUCT_LEN - sizeof *p + SPRITE_BODY_LEN)) ) {
		ERROR_RETURN( ("error len=%d, id=%u,%u, %u, %lu waitcmd=%d", len, p->id, MIN_MAIL_SIZE, SPRITE_STRUCT_LEN, sizeof(*p), p->waitcmd), -1 );
	}

	unpkg_sprite(p, buf);
	//猪倌上次获取随机好运时间
	p->cp_last_lucky_time = get_now_tv()->tv_sec;
	//dancing, 
	p->water_bomb_time = get_now_tv()->tv_sec;
	//20120622
	uint8_t db_buff[256] = {0};
	int j = 0;
	PKG_STR(db_buff, p->nick, j, 16);
	PKG_H_UINT32(db_buff, 0, j);
	PKG_H_UINT32(db_buff, 0,j);
	PKG_H_UINT32(db_buff, p->game_exp, j);
	send_request_to_db(SVR_PROTO_MOLE_CHANGE_GAME_SCORE, NULL, j, db_buff, p->id);


	if (p->olcount == 0) {
#ifdef TW_VER
	send_postcard("公民管理處", 0, id, 1000107, "歡迎來到摩爾莊園，希望你在這裡能找到屬於自己的快樂和幸福！", 0);
#else
	send_postcard("公民管理处", 0, id, 1000107, "欢迎来到摩尔庄园，希望你在这里能找到属于自己的快乐和幸福!", 0);
#endif
	}
	CHECK_BODY_LEN(len, SPRITE_BODY_LEN + *(uint32_t*)(buf + SPRITE_BODY_LEN));
	p->sess_len = len - SPRITE_BODY_LEN;
	memcpy(p->session, buf + SPRITE_BODY_LEN, p->sess_len);

	uint32_t bank_daoqi;
	char txt[256];
	int txtlen;
	int i = SPRITE_BODY_LEN - 4;
	UNPKG_H_UINT32(buf, bank_daoqi, i);
#ifdef TW_VER
	txtlen = snprintf(txt, sizeof txt, "  %s\n",
					"接到摩爾銀行R5機器人的通知，你有存款已經到期，趕快去提取吧！還有利息哦");
#else
	txtlen = snprintf(txt, sizeof txt, "  %s\n",
					"接到摩尔银行R5机器人的通知，你有存款已经到期，赶快去提取吧！还有利息哦");
#endif
	(bank_daoqi) ? notify_system_message(p, txt, txtlen) : NULL;
	return db_get_pet_and_item_info(p, p->id);

#undef MIN_MAIL_SIZE
}

static int
get_sprite_return(sprite_t* p, uint32_t id, char* buf, int len)
{
	sprite_t* who;

	CHECK_BODY_LEN(len, SPRITE_BODY_LEN);

	switch (p->waitcmd) {
	//search any user's detail info
	case PROTO_USER_DETAIL:
	case PROTO_USER_SIMPLY:
	case PROTO_FRIEND_INFO:
		who = (sprite_t*)p->session;
		break;
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}

	who->id = id;
	unpkg_sprite(who, buf);
	if (p->waitcmd == PROTO_FRIEND_INFO) {
		response_proto_get_sprite2(who, PROTO_FRIEND_INFO, p, 1);
		return 0;
	}

	return db_get_items(p, SUIT_ITEM_KIND, 1, who->id, 0);
}

/*
 note: only vip can use this API
*/
static int calc_vip_level(int value)
{
	int vip_level;
	int vip_lvls[10]={1,311,776,1706,3566,5426,7286,9146,11006,14726};

	for(vip_level = 0; vip_level < 8 ; vip_level++) {
		if (value < vip_lvls[vip_level]) break;
	}

	return vip_level;
}

int change_vip_level(sprite_t* p, uint32_t change_val)
{
	p->vip_value += change_val;
	uint32_t vip_level = calc_vip_level(p->vip_value);
	if (vip_level > p->vip_level) {
		send_request_to_db(SVR_PROTO_SET_VIP_LEVEL, 0, 4, &(p->vip_level), p->id);
		char buff[2048] = {0};
		uint32_t leavetag[] = {0,1};
		uint32_t entertag[] = {1,0};
		msglog(statistic_logfile, 0x0207EEF0 + p->vip_level, now.tv_sec, leavetag, sizeof(leavetag));
		msglog(statistic_logfile, 0x0207EEF0 + vip_level, now.tv_sec, entertag, sizeof(entertag));

#ifdef TW_VER
		sprintf(buff,"恭喜！你的超級拉姆已經達到%d星級！快去城堡花園看看新道具和禮包吧!", vip_level);
		send_postcard("克勞神父", 0, p->id, 1000026, buff, 0);
#else
		sprintf(buff,"恭喜！你的超级拉姆已经达到%d星级！快去城堡花园看看新道具和礼包吧!", vip_level);
		send_postcard("克劳神父", 0, p->id, 1000026, buff, 0);
#endif
		p->vip_level = vip_level;
	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, p->vip_value, l);
	PKG_UINT32(msg, p->vip_level, l);
	init_proto_head(msg, PROTO_GET_VIP_VALUE_AND_LEVEL, l);
	return send_to_self(p, msg, l, 0);
}

#ifdef TW_VER
	#define VIP_END_TIME_FLAG	1265765011
#else
	#define VIP_END_TIME_FLAG	1264706788
#endif

#define IS_USED_VIP(flag_)	!!((flag_) & 2)

typedef struct UserCommonInfo {
	uint32_t	flag2; // 1 --> vip, 2 --> used to be a vip
	uint32_t	vip_month;
	uint32_t	vip_base_val;
	uint32_t	vip_time_start;
	uint32_t	vip_time_end;
	uint32_t	vip_last_level;
	uint32_t	vip_auto_payed;
	uint32_t	driver_time;
	char		nick[USER_NICK_LEN];
	uint32_t    my_first_classid;
	uint32_t    my_moleclass_cnt;
	uint32_t    moleclass[MAX_OWNED_MOLECLASS_NUM];
	uint32_t	my_owned_molegroup;
	uint32_t	molegroup[MAX_OWNED_MOLEGROUP_NUM];
	uint32_t	architect_exp;
	uint32_t	dietitian_exp;
	uint32_t	profession[50];
	uint32_t	only_one_flag[8];
	uint32_t	old_last_time;
	uint32_t	sendmail_flag;
} __attribute__((packed)) user_common_info_t;

void calc_vip_info(sprite_t *p, user_common_info_t *u)
{
	int value = u->vip_base_val;
	p->vip_value = p->vip_level = 0;

	if (now.tv_sec < u->vip_time_start) {
		ERROR_LOG("id=%u ignore vip time error start=%u, set start=%u(now)", p->id, (uint32_t )u->vip_time_start, (uint32_t)(now.tv_sec));
		u->vip_time_start = now.tv_sec;
	}

	if (u->vip_time_end < u->vip_time_start) {
		ERROR_LOG("id=%u ignore vip time error end=%u, set end = %u(start)", p->id,(uint32_t) u->vip_time_end, (uint32_t)u->vip_time_start);
		u->vip_time_end = u->vip_time_start;
	}

	if (u->vip_time_end < VIP_END_TIME_FLAG)
		u->vip_time_end = VIP_END_TIME_FLAG;
	if (u->vip_time_start < VIP_END_TIME_FLAG)
		u->vip_time_start = VIP_END_TIME_FLAG;

	if (ISVIP(u->flag2)) {
		if (u->vip_time_end < now.tv_sec) {
			ERROR_LOG("id=%u vip time error now=%u end=%u", p->id, (uint32_t)now.tv_sec,(uint32_t) u->vip_time_end);
		}// else {
			value += 5 * ((now.tv_sec - u->vip_time_start) / (3600 * 24));
			p->vip_value = value;
			p->vip_level = calc_vip_level(value);
			if (p->vip_level != 0) {
				msglog(statistic_logfile, 0x02030100 + p->vip_level, now.tv_sec, &p->id, sizeof(p->id));
			}
			if (u->vip_last_level != p->vip_level) {
				send_request_to_db(SVR_PROTO_SET_VIP_LEVEL, 0, 4, &(p->vip_level), p->id);
				char buff[2048] = {0};
				if (u->vip_last_level < p->vip_level) {
					uint32_t leavetag[] = {0,1};
					uint32_t entertag[] = {1,0};

					msglog(statistic_logfile, 0x0207EEF0 + u->vip_last_level, now.tv_sec, leavetag, sizeof(leavetag));
					msglog(statistic_logfile, 0x0207EEF0 + p->vip_level, now.tv_sec, entertag, sizeof(entertag));
#ifdef TW_VER
					sprintf(buff,"恭喜！你的超級拉姆已經達到%d星級！快去城堡花園看看新道具和禮包吧!",p->vip_level);
					send_postcard("克勞神父", 0, p->id, 1000026, buff, 0);
#else
					sprintf(buff,"恭喜！你的超级拉姆已经达到%d星级！快去城堡花园看看新道具和礼包吧!",p->vip_level);
					send_postcard("克劳神父", 0, p->id, 1000026, buff, 0);
#endif
				}
			}
//		}
	} else if(IS_USED_VIP(u->flag2)) {
		if (u->vip_time_end > now.tv_sec) {
			u->vip_time_end = now.tv_sec;
			ERROR_LOG("id=%u used vip time error now=%u end=%u", p->id, (uint32_t)now.tv_sec,(uint32_t) u->vip_time_end);
		}// else {
			//曾经是VIP的，现在不是，超拉值不减少5点了,20110916
			//value -= 5 * ((now.tv_sec - u->vip_time_end) / (3600 * 24));
			//if (value < 0) value = 0;
			p->vip_value = value;
			p->vip_level = calc_vip_level(value);
			if (p->vip_level != 0) {
#ifndef TW_VER
				msglog(statistic_logfile, 0x02030100, now.tv_sec, &p->id, sizeof(p->id));
#else
				msglog(statistic_logfile, 0x02030100 + p->vip_level, now.tv_sec, &p->id, sizeof(p->id));
#endif
			}
			if (u->vip_last_level != p->vip_level) {
				send_request_to_db(SVR_PROTO_SET_VIP_LEVEL, 0, 4, &(p->vip_level), p->id);

				char buff[2048] = {0};
				if (u->vip_last_level > p->vip_level) {
					uint32_t leavetag[] = {0,1};
					uint32_t entertag[] = {1,0};
					msglog(statistic_logfile, 0x0207EEF0 + u->vip_last_level, now.tv_sec, leavetag, sizeof(leavetag));
					msglog(statistic_logfile, 0x0207EEF0 + p->vip_level, now.tv_sec, entertag, sizeof(entertag));
#ifdef TW_VER
					sprintf(buff,"你的超級拉姆星級已經下降到%d星級,請及時續費，續費後將從%d星級繼續累積成長，並享受所有超級拉姆的特權道具，特權活動！",p->vip_level,calc_vip_level(u->vip_base_val));
					send_postcard("克勞神父", 0, p->id, 1000026, buff, 0);
#else
					sprintf(buff,"你的超级拉姆星级已经下降到%d星级,请及时续费，续费后将从%d星级继续累积成长，并享受所有超级拉姆的特权道具，特权活动！",p->vip_level,calc_vip_level(u->vip_base_val));
					send_postcard("克劳神父", 0, p->id, 1000026, buff, 0);
#endif
				}
			}
//		}
	}

	DEBUG_LOG("VIP INFO %u: Flag=%x Month=%u Level=%u Value=%u Time:S=%u E=%u T=%u L'=%u V'=%u",
		p->id, u->flag2, u->vip_month, u->vip_last_level, u->vip_base_val,
		(uint32_t )u->vip_time_start, (uint32_t)u->vip_time_end, (uint32_t)now.tv_sec, p->vip_level, p->vip_value);
}


static int
get_user_common_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int length = sizeof(user_common_info_t);
	CHECK_BODY_LEN_GE(len, length);
	DEBUG_LOG("GET MAIL RETURN %d %d %d", p->id, len, length);
	sprite_t* user;
	user_common_info_t* userinfo = (void*)buf;

	switch (p->waitcmd) {
	case PROTO_LOGIN:
		if (p->id == id) {
			user = p;
		} else {
			return 0;
		}
		break;
	case PROTO_FRIEND_INFO:
	case PROTO_USER_DETAIL:
	case PROTO_USER_SIMPLY:
		user         = (sprite_t*)(p->session);
		p->sess_len  = sizeof(sprite_t);
		memset(user, 0, sizeof *user);
		break;
	case PROTO_BECOME_SUPER_LAHM:
		if (ISVIP(userinfo->flag2)) {
			p->flag = set_bit_on32(p->flag, 1);
			PET_SET_SUPER_LAHM(p->followed);
			if(p->followed->sick_type) {
				p->followed->sick_type = 0;
				db_infect_pet(NULL, p->followed, p->id);
			}
			if (enter_five_stage(p->followed)) {
#ifndef TW_VER
				send_postcard("系统邮件", 0, p->id, 1000119, "恭喜你拥有超级拉姆，快带它到拉姆世界神殿学习水、火、木3系技能，收集更多建设房子的材料！", 0);
#else
				send_postcard("系統郵件", 0, p->id, 1000119, "恭喜你擁有超級拉姆，快帶它到拉姆世界神殿學習水、火、木3系技能，收集更多建設房子的材料！", 0);
#endif
			}

			typedef struct sl_msg
			{
				uint32_t tm;
				char	 nick[USER_NICK_LEN];
				char	 m_nick[USER_NICK_LEN];
				uint32_t is_new;
			}__attribute__((packed)) sl_msg_t;
			sl_msg_t sl_msglog = {};
			sl_msglog.tm = time(NULL);
			if (p->followed)
			{
				memcpy(sl_msglog.nick,p->followed->nick,USER_NICK_LEN);
			 }
			memcpy(sl_msglog.m_nick,p->nick,USER_NICK_LEN);
			sl_msglog.is_new = !(p->vip_autopay);
			send_msglog_to_bus(BECOME_SUPER_LAMU_CMD,sizeof(sl_msglog),&sl_msglog,p->id);


			return db_set_pet_arg(p, PET_FLAG2, p->followed->flag2, p->followed->id, p->id);
		}
		// fail to become super lahm coz @p is not a vip, response `uint8_t 0`
		response_proto_uint8(p, p->waitcmd, 0, 0);
		return 0;
	default:
		ERROR_RETURN(("Unexpected WaitCmd %d From %u", p->waitcmd, p->id), -1);
	}

	user->vip_month = userinfo->vip_month;
	user->driver_time = userinfo->driver_time;
	user->vip_end	= userinfo->vip_time_end;

	if (user == p) {
		calc_vip_info(user, userinfo);
	} else {
		user->vip_value = userinfo->vip_base_val;
		user->vip_level = userinfo->vip_last_level;
	}

	user->vip_autopay = userinfo->vip_auto_payed;
	memcpy(user->nick, userinfo->nick, USER_NICK_LEN);
	memcpy(user->my_molegroup_id, userinfo->molegroup, sizeof(user->my_molegroup_id));
	user->my_molegroup_num = userinfo->my_owned_molegroup;

	user->my_moleclass_first= userinfo->my_first_classid;
	user->my_moleclass_num = userinfo->my_moleclass_cnt > MAX_OWNED_MOLECLASS_NUM ? MAX_OWNED_MOLECLASS_NUM : userinfo->my_moleclass_cnt;
	memcpy(user->my_moleclass_id, userinfo->moleclass, sizeof(user->my_moleclass_id));
	DEBUG_LOG("CLASS CNT [%d %d]", user->id, user->my_moleclass_num);
	user->architect_exp = userinfo->architect_exp;
	user->dietitian_exp = userinfo->dietitian_exp;
	memcpy(user->profession, userinfo->profession, sizeof(user->profession));
	if (ISVIP(userinfo->flag2)) {
		user->flag = set_bit_on32(user->flag, 1);
	}
	memcpy(user->only_one_flag,userinfo->only_one_flag,sizeof(user->only_one_flag));
	user->old_last_time = userinfo->old_last_time;
	user->sendmail_flag = userinfo->sendmail_flag;


	if (IS_USED_VIP(userinfo->flag2)) {
		user->flag = set_bit_on32(user->flag, 6);
	}

	if (p == user) {
		return get_sprite_with_mail_return(p, id, buf + length, len - length);
	} else {
		return get_sprite_return(p, id, buf + length, len - length);
	}
}

static inline int
old_kind_type_change (uint32_t kind_type)
{
	uint32_t i = 0;
	for (i = 0; i < 32; i++)
	{
		if (kind_type == (1 << i))
		{
			return  i + 1;
		}
	}

	return -1;
}


int db_get_items(sprite_t* p, uint32_t type, uint8_t used, uint32_t id, uint8_t kind_flag)
{
	char buf[13];
	int i, k;
	uint32_t start, end;

	if (!kind_flag)
	{
		if (type == OLD_HOME_ITEM_KIND) {
			k = 1;
		} else if (type == OLD_HOMELAND_ITEM_KIND) {
			k = 2;
		} else if (type == OLD_TEMP_ITEM_KIND) {
			k = 99;
		} else {
			k = 0;
		}

		new_get_item_kind_range (type, &start, &end);
	}
	else
	{
		int kind_type = -1;
		kind_type = old_kind_type_change(type);
		if (kind_type < 0)
		{
			kind_type = SUIT_ITEM_KIND;
		}

		if (type == HOME_ITEM_KIND) {
			k = 1;
		} else if (type == HOMELAND_ITEM_KIND) {
			k = 2;
		} else if (type == TEMP_ITEM_KIND) {
			k = 99;
		} else if (type == FIGHT_ITEM_KIND || type == FIGHT_CARD_KIND || type == FIGHT_COLLECT_KIND) {
			k = 32;
		} else if (type == CUTE_PIG_KIND || type == CUTE_PIG_ITEM_KIND || type == CUTE_PIG_CLOTHES_KIND) {
			k = 36;
		}else {
			k = 0;
		}

		get_item_kind_range (type, &start, &end);

	}

	DEBUG_LOG("ITEM KIND\t[start=%u end=%u", start, end);
	i = 0;
	PKG_H_UINT32 (buf, k, i);
	PKG_H_UINT32 (buf, start, i);
	PKG_H_UINT32 (buf, end, i);
	PKG_UINT8 (buf, used, i);
	return send_request_to_db (SVR_PROTO_GET_ITEM, p, i, buf, id);
}

//-------------------- callbacks definitions begin --------------------
static inline int
chk_uid_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	switch (p->waitcmd) {
	case PROTO_FRIEND_ADD:
		req_add_friend_notifier(p, 0, id);
		response_proto_head(p, PROTO_FRIEND_ADD, 0);
		break;
	default:
		ERROR_RETURN( ("Unexpected WaitCmd=%d Uid=%u", p->waitcmd, p->id), -1);
	}

	return 0;
}

static inline int
set_sprite_return(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN (len, 0);
	switch (p->waitcmd) {
	case PROTO_COLOR_MODIFY:
		response_proto_get_sprite2 (p, p->waitcmd, NULL, 1);
		break;
	default:
		ERROR_RETURN (("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}
	return 0;
}

int exchg_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);

	int completed = 1, err = 0, send_now = 1;
	switch (p->waitcmd) {
	case PROTO_LOOK_STH_CNT:
		return look_sth_cnt_callback_1(p);
	case PROTO_EXCHANGE_ITEM:
		if (p->sess_len >= (sizeof(protocol_t) + 4)) {
			// add attr on
			add_exchg_attr(p, p->session + 8);
			// Send Aquired Items To Client
			send_exchg_itm_rsp(p, 0);
			broad_cast_when_fini_task(p, 0);
			// Set Task Completed
			uint32_t taskid = *((uint32_t*)p->session);
			int flag_to_set = *((int*)(p->session + 4));
			if (flag_to_set) {
				p->flag = set_bit_on32(p->flag, flag_to_set);
				db_set_user_flag(p, set_bit_on32(p->flag, 1), p->id);
			}
			//DEBUG_LOG("SET TASK\t[%u %u 0x%llx]", p->id, taskid, p->tasks);
			set_bit_off(p->tasks, sizeof p->tasks, taskid * 2 + 1);
			set_bit_on(p->tasks, sizeof p->tasks, taskid * 2 + 2);
			//p->waitcmd  = PROTO_SET_TASK;
			if (taskid == 112) { /*母亲节完成任务人数*/
				db_set_cnt(NULL, 134, 1);
			}
			uint32_t finish_task[2] = {0,1};
			if(taskid == 0){
			    msglog(statistic_logfile, 0x0409BEE5,get_now_tv()->tv_sec, finish_task, sizeof(finish_task));
			}
			else{
			    msglog(statistic_logfile, 0x02022000 + taskid,get_now_tv()->tv_sec, finish_task, sizeof(finish_task));
			}

			if (taskid == 390)
			{
			    uint32_t data = get_today();
			    uint32_t db_buff[2] = {1, data};
                send_request_to_db(SVR_PROTO_USER_AS_ADD_TYPE_INT_DATA, NULL, 8, db_buff, p->id);
			}

			DEBUG_LOG("SET TASK\t[%u %u ]", p->id, taskid );
			set_profession_by_taskid(p, taskid, p->id);
			return db_set_task(p, taskid, 2);
		} else {
			ERROR_RETURN(("invalid sess_len=%d expected>=%lu", p->sess_len, sizeof(protocol_t) + 8), -1);
		}
	case PROTO_SEND_ONE_GIFT:
	{
		send_one_t *lso = (send_one_t*)p->session;
		return send_request_to_db(SVR_PROTO_EXCHG_GIFT, p, lso->send_len, lso->send_buf, lso->frd_id);
	}
	case PROTO_GGJ_EXCHANGE_ITM:
	{
		response_proto_uint32(p, p->waitcmd, *(uint32_t*)(p->session + 4), 0);
		return 0;
	}
	case PROTO_EXCHG_CHAR_CLOTH:
	{
		response_proto_uint32(p, p->waitcmd, *(uint32_t*)(p->session), 0);
		return 0;
	}
	case PROTO_RAND_ITEM_SWAP:
	case PROTO_SEND_RAND_ITEM:
	{
		return db_add_rand_item_callback(p);
	}
	case PROTO_FIN_NPC_TASK:
		return fin_npc_task_to_self(p);
	case PROTO_EXCHG_COLLECTED_ITEM:
	case PROTO_EXCHG_ITEM2:
	case PROTO_GET_MAGIC_CARD_REWARD:
	case PROTO_EXCHG_XUANFENG_JIJILE:
	case PROTO_GET_PK_TRADE:
	case PROTO_ASK_FOR_ITMES:
	case PROTO_SET_STH_DONE:
	case PROTO_PET_WITHDRAW:
	case PROTO_COMPOUND_ITEM:
	// Temperarily Use Only
	case PROTO_PERSONAL_INFO:
		if ( p->sess_len < (sizeof(protocol_t) + 4) ) {
			ERROR_RETURN(("invalid sess_len=%d expected>=%lu", p->sess_len, sizeof(protocol_t) + 8), -1);
		}
		break;
	case PROTO_MSNGER_INVITEE_CHK_IN:
		err = mole_msnger_add_invitee(p, p->inviter_id);
		send_now = 0;
		break;
	// Temperarily Use Only
	case PROTO_SUBMIT_MAGIC_CODE:
		db_set_magic_code_used(p, p->session + 1024);
		break;
	case PROTO_CANDY_EXCH:
		response_proto_head(p, p->waitcmd, 0);
		return 0;
	case PROTO_CANDY_MAKE:
		return do_add_make_timer(p);
	case PROTO_BUILD_LAHM_TEMPLE:
	{
		uint32_t db_buf[2] = {0};
		db_buf[0] = *(uint32_t *)p->session;
		db_buf[1] = *(uint32_t *)(p->session + 4);
		return send_request_to_db(SVR_PROTO_LAHM_BUILD_TEMPLE, p, 8, db_buf, p->id);
	}
	case PROTO_EXCHANGE_RAND_ITEM:
	{
		uint32_t itemid;
		itemid = *((uint32_t*)p->session);
		response_proto_uint32(p, p->waitcmd, itemid, 0);
		DEBUG_LOG("EXCHANGE_RAND_ITEM id=%u GOT item %u", p->id, itemid);
		return 0;
	}
	case PROTO_EXCHANGE_GOLD_SILVER:
	{
		response_proto_head(p, p->waitcmd, 0);
		return 0;
	}
	case PROTO_SET_ONLY_ONE_FLAG:
	{
		response_proto_head(p, p->waitcmd, 0);
		return 0;
	}
	case PROTO_PROFESSION_DO_WORK:
	{
		do_profession_work_callback(p);
		return 0;
	}
	case PROTO_EXCHANGE_ONE_TO_ANOTHER:
		return exchange_one_thing_to_another_callback(p);
	case PROTO_ADD_STH_RAND:
		return get_sth_accord_rand_callback(p, p->id, buf, len);
	case PROTO_EXCHANGE_STH:
		return swap_callback(p);
	case PROTO_FIRE_CUP_ADD_GAME_BONUS:
		{
			uint32_t gameid = *(uint32_t*)p->session;
			uint32_t i = *(uint32_t*)(p->session + 4);
			send_game_bonus_to_client(p, gameid, i);
			return 0;
		}
	case PROTO_RACE_GAIN_RACE_BONUS:
	{
		uint32_t raceid = *(uint32_t*)p->session;
		uint32_t teamid = *(uint32_t*)(p->session + 4);
		send_race_bonus_to_client(p, raceid, teamid);
		return 0;
	}
	case PROTO_ADD_TEMP_ITEM:
		return add_temp_item_callback(p,id,buf, len);
	case PROTO_BUY_ITEM_BY_BEANS:
		return buy_item_by_beans_callback(p, id,buf, len);
	case PROTO_LAHM_CLASSROOM_LAHM_EXAM:
	{
		send_request_to_db(SVR_PROTO_USER_CLASSROOM_SET_PRIZE_ITEM, NULL, p->sess_len, p->session, p->id);
		response_proto_head(p, p->waitcmd, 0);
		return 0;
	}
	case PROTO_GAME_SCORE:
	{
		int i = sizeof(protocol_t);
		uint32_t* pInt = (void*)(p->session + i + 32);
		uint32_t gid = *(uint32_t*)(p->session + i);
		uint32_t msg_buff[7] = {pInt[0],pInt[1],pInt[2],pInt[3],pInt[4],1,p->id};
		msglog(statistic_logfile, 0x020F0000 + gid,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		if (gid == 88)
		{
			msglog(statistic_logfile, 0x04040B04,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}

		PKG_UINT32(p->session, gid, i);
		DEBUG_LOG("game1 %u", p->id);
		db_attr_op(p, pInt[4], pInt[0], pInt[1], pInt[2], pInt[3], ATTR_CHG_play_game, gid);
		i += 28;
		PKG_UINT32(p->session, pInt[0], i);
		PKG_UINT32(p->session, pInt[1], i);
		PKG_UINT32(p->session, pInt[2], i);
		PKG_UINT32(p->session, pInt[3], i);
		PKG_UINT32(p->session, pInt[4], i);
		DEBUG_LOG("game2 %u", p->id);
		return 0;
	}
	case PROTO_DONATE_ITEM_CHARITY_DRIVE:
	{
		uint32_t itemid = *(uint32_t *)p->session;
		uint32_t count = *(uint32_t *)(p->session + 4);
		uint32_t db_buf[2] = {itemid, count};
		if (itemid == 0)
		{
			p->yxb = p->yxb - count;
		}
		return send_request_to_db(SVR_PROTO_SYSARG_DONATE_ITEM_AND_XIAOMEE, p, 8, db_buf, p->id);
	}
	case PROTO_GIVE_ITEM_TO_FRIEND:
	{
		uint32_t userid = *(uint32_t *)p->session;
		int val = rand()%10;
		uint32_t itemid_u = 0;
		uint32_t count_u = 0;
		if (ISVIP(p->flag))
		{
			if (val < 8)
			{
				itemid_u = 1230056;
				count_u = 1;
			}
			else
			{
				itemid_u = 1230056;
				count_u = 2;
			}
		}
		else
		{
			if (val == 0)
			{
				itemid_u = 1230055;
				count_u = 2;
			}
			else if (val == 1)
			{
				itemid_u = 1230056;
				count_u = 1;
			}
			else
			{
				itemid_u = 1230055;
				count_u = 2;
			}
		}

		uint8_t db_buff[1024] = {};
		int i = 0;
		PKG_H_UINT32(db_buff, 0, i);
		PKG_H_UINT32(db_buff, 1, i);
		PKG_H_UINT32(db_buff, 202, i);
		PKG_H_UINT32(db_buff, 0, i);
		if (pkg_item_kind(p, db_buff, itemid_u, &i) == -1)
		{
			return -1;
		}
		PKG_H_UINT32(db_buff, itemid_u, i);
		PKG_H_UINT32(db_buff, count_u, i);
		item_t* item_u = get_item_prop(itemid_u);
		PKG_H_UINT32(db_buff, item_u->max, i);
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, i, db_buff, p->id);

		uint32_t s_buff[2] = {1, 1};
		send_request_to_db(SVR_PROTO_SYSARG_ADD_RANK_TYPE_COUNT, NULL, sizeof(s_buff), s_buff, p->id);
		uint32_t g_buff[2] = {2, 1};
		send_request_to_db(SVR_PROTO_SYSARG_ADD_RANK_TYPE_COUNT, NULL, sizeof(g_buff), g_buff, userid);
		uint32_t sr_buff[2] = {userid, 1};
		send_request_to_db(SVR_PROTO_ADD_GET_ITEM_RECORD, NULL, sizeof(sr_buff), sr_buff, p->id);
		uint32_t gr_buff[2] = {p->id, 10001};
		send_request_to_db(SVR_PROTO_ADD_GET_ITEM_RECORD, NULL, sizeof(gr_buff), gr_buff, userid);
		response_proto_uint32_uint32(p, p->waitcmd, itemid_u, count_u, 0);

		uint32_t smsg_buff[2] = {1,p->id};
		msglog(statistic_logfile, 0x020D2001,get_now_tv()->tv_sec, smsg_buff, sizeof(smsg_buff));

		uint32_t rmsg_buff[2] = {1,userid};
		msglog(statistic_logfile, 0x020D2002,get_now_tv()->tv_sec, rmsg_buff, sizeof(rmsg_buff));

		if (itemid_u == 1230055)
		{
			uint32_t msgbuff[2] = {count_u, 0};
			msglog(statistic_logfile, 0x020D2005,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
		else
		{
			uint32_t msgbuff[2] = {count_u, 0};
			msglog(statistic_logfile, 0x020D2006,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}

		return 0;
	}
	case PROTO_SNOWBALL_GET_GAME_BONUS:
	{
		send_snowball_game_bonus_to_client(p);
		return 0;
	}
	case PROTO_CHRISTMAS_SOCK_ADD_SOCK:
	{
		christmas_sock_add_count_to_db(p);
		return 0;
	}
	case PROTO_CHRISTMAS_SOCK_GET_BONUS:
	{
		christmas_sock_send_bonus_to_client(p);
		return 0;
	}
	case PROTO_GET_FOOT_PRINT_BONUS:
	{
		get_foot_print_bonus_mgr(p);
		return 0;
	}
	case PROTO_TRAIN_KAKU_NIAN:
	{
		uint32_t flag = *(uint32_t *)p->session;
		response_proto_uint32(p, p->waitcmd, flag, 0);
		send_request_to_db(SVR_PROTO_ADD_TRAIN_KAKU_NIAN_COUNT, NULL, sizeof(flag), &flag, p->id);
		return 0;
	}
	case PROTO_GAIN_GAME_ANGLES:
	{
		gain_game_angles_exchange_item(p);
		return 0;
	}
	case PROTO_CLEAN_ANGEL_GAME_BEGIN:
	{
		int i = sizeof (protocol_t);
		init_proto_head (msg, p->waitcmd, i);
		return send_to_self (p, msg, i, 1);
	}
	case PROTO_USE_PROP_FOR_TD:
	{
		int i = sizeof (protocol_t);
		init_proto_head (msg, p->waitcmd, i);
		return send_to_self (p, msg, i, 1);
	}
	case PROTO_AF_USE_ITEM:
	{
		return af_use_gift_package_callback(p);
	}
	case PROTO_CP_SALE:
	{
		return cutepig_sale_exchange_ok(p);
	}
	case PROTO_CP_BUY_ITEM:
	{
		return cutepig_buy_exchange_ok(p);
	}
	case PROTO_AF_DONATE_CARD:
	{
		response_proto_head(p, p->waitcmd, 0);
		return 0;
	}
	case PROTO_AF_EXCHANGE_CARD:
	{
		response_proto_head(p, p->waitcmd, 0);
		return 0;
	}
	case PROTO_ADD_MINE_MAP_PIECE:
	{
		uint32_t itemid = *(uint32_t *)p->session;
		return  send_request_to_db(SVR_PROTO_SYSARG_MATCHING_MAP, p, 4, &itemid, p->id);
	}
	case PROTO_DONATE_FOOD_ITEM:
	{
		return donate_food_item_callback(p, id, buf, len);
	}
	case PROTO_FINISH_GREAT_PRODUCE_ORDER:
	{
		DEBUG_LOG("-----is ok!");
		return send_request_to_db(SVR_PROTO_USER_THOUSAND_GREAT_PRODUCE_NUMBER, p, 0, NULL, p->id);
	}
	case PROTO_USER_CHANGE_LAHM_SPORT_TEAM:
	{
		return user_change_lahm_sport_team_callback(p, id, buf, len);
	}
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}

	// add attr on
	add_exchg_attr(p, p->session + 8);
	if (send_now) {
		send_exchg_itm_rsp(p, completed);
	}
	return err;
}

static inline int
set_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	uint32_t taskid = *((uint32_t*)p->session);
	if (taskid == 300 && p->inviter_id != 0 && test_bit_on(p->tasks, sizeof p->tasks, 300 * 2 + 2)) {
		 uint32_t count = 1;
		 msglog(statistic_logfile, 0x02022000, get_now_tv()->tv_sec, &count, sizeof(uint32_t));
		return mole_msnger_add_invitee(p, p->inviter_id);
	}

	if (taskid == 145 && p->waitcmd == PROTO_EXCHANGE_ITEM)
	{
		return create_lahm_classroom(p);//create lahm classroom
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, taskid, i);
	PKG_UINT32(msg, 600, i);
	i += pack_user_tasks(p, msg + i);
	init_proto_head(msg, PROTO_SET_TASK, i);
	send_to_self(p, msg, i, 1);
	return 0;
}

static inline int
chk_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);

	int j = 0;
	uint32_t type,count;
	UNPKG_H_UINT32(buf, type, j); // type
	UNPKG_H_UINT32(buf, count, j); // cnt
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, type, i);
	PKG_UINT32(msg, count, i);
	if(p->waitcmd == PROTO_LOOK_STH_CNT) {
		return look_sth_cnt_callback_0(p,type,count);
	}

	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

static inline int
get_cnt_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct CntElem {
	uint32_t type;
	int32_t  cnt;
} __attribute__((packed)) cnt_elem_t;

typedef struct CntList {
	uint32_t   cnt;
	cnt_elem_t elem[];
} __attribute__((packed)) cnt_list_t;

	CHECK_BODY_LEN_GE(len, 4);
	const cnt_list_t* cntlist = (void*)buf;
	CHECK_BODY_LEN(len, sizeof(cnt_list_t) + cntlist->cnt * sizeof(cnt_elem_t));
	uint32_t start, end;

	switch (p->waitcmd) {
	case PROTO_GET_CNT_LIST:
	{
		start = *(uint32_t*)p->session;
		end = *(uint32_t*)(p->session + 4);
		int i = sizeof(protocol_t), j;
		PKG_UINT32(msg, end - start + 1, i);
		int k;
		for(k = 0; k < end - start + 1; k++) {
			PKG_UINT32(msg, start + k, i);
			uint32_t count = 0;
			for (j = 0; j != cntlist->cnt; ++j) {
				if(cntlist->elem[j].type == start + k){
					count = cntlist->elem[j].cnt;
					break;
				}
			}
			PKG_UINT32(msg, count, i);
		}
		init_proto_head(msg, p->waitcmd, i);
		return send_to_self(p, msg, i, 1);
	}
	default:
		ERROR_RETURN(("bad waitcmd\t[%u]", p->id), -1);
	}
}

int get_all_fish_weight_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t weight = *(uint32_t*)buf;
	p->tmp_fish_weight = weight;
	//int type = 1000000021;
	switch (p->waitcmd) {
	case PROTO_GET_TOTAL_FISH_WEIGHT:
		DEBUG_LOG("GET FISH WEIGHT\t[%u %u]", p->id, weight);
		response_proto_uint32(p, p->waitcmd, weight, 0);
		return 0;
	case PROTO_LOGIN:
	{
		return send_request_to_db(SVR_PROTO_GET_SPORT_TEAM, p, 0, NULL, p->id);
	}

	default:
		ERROR_RETURN(("bad cmd type\t[%u]", p->id), -1);
	}
}

static inline int
get_day_limit_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct CntElem {
	uint32_t type;
	int32_t  cnt;
} __attribute__((packed)) cnt_elem_t;

typedef struct CntList {
	uint32_t   cnt;
	cnt_elem_t elem[];
} __attribute__((packed)) cnt_list_t;

	CHECK_BODY_LEN_GE(len, 4);
	const cnt_list_t* cntlist = (void*)buf;
	CHECK_BODY_LEN(len, sizeof(cnt_list_t) + cntlist->cnt * sizeof(cnt_elem_t));
	uint32_t start, end;

	switch (p->waitcmd) {
	case PROTO_LOGIN:
	{
		int j;
		start = START_DAY_LIMIT;
		end = END_DAY_LIMIT;
		for (j = 0; j != cntlist->cnt && j < MAX_DAILY_COUNT; ++j) {
			DEBUG_LOG("DAY lIMIT LIST\t[%u %u %u]", p->id, cntlist->elem[j].type, cntlist->elem[j].cnt);
			if(cntlist->elem[j].type < start || cntlist->elem[j].type > end) {
				continue;
			}
			p->daily_limit[cntlist->elem[j].type - 1001] = (uint8_t)(cntlist->elem[j].cnt);
		}
		return send_request_to_db(SVR_PROTO_GET_ALL_FISH, p, 0, NULL, p->id);
	}
	default:
		ERROR_RETURN(("bad waitcmd\t[%u]", p->id), -1);
	}
}

// For Temporarily Use Only
static inline int
submit_personal_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 0);
	return do_exchange_item(p, 1539, 0);
}

static inline int
chk_if_info_submitted_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	int i = sizeof(protocol_t);
	PKG_UINT8(msg, *((uint32_t*)buf), i);
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

static inline int
chk_vip_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
typedef struct VipTime {
	uint32_t	result;   // 0 success, 2205 no record, 2210 db error
	uint32_t	flag;     // 0 non-vip, 1 monthly vip, 2 auto-recharge vip
	uint32_t	begin_tm;
	uint32_t	end_tm;
	uint32_t	fee_flag;
} __attribute__ ((packed)) vip_time_t;

	CHECK_BODY_LEN(len, sizeof(vip_time_t));

	const vip_time_t* viptime = (void*)buf;
	struct tm end;
	struct tm cur = *get_now_tm();
	localtime_r((const time_t*)&(viptime->end_tm), &end);
	cur.tm_sec = end.tm_sec = cur.tm_min = end.tm_min = cur.tm_hour = end.tm_hour = 0;

	uint32_t days_left = 0;
	int flag = chk_has_super_lahm(p);
	switch(p->waitcmd) {
		case PROTO_GET_LOTTERY:
			{
				if(viptime->fee_flag == 18 && (get_now_tv()->tv_sec - viptime->begin_tm) <= 3600 * 24 * 3) {
					//DEBUG_LOG("MESSAGE VIP LOTTERY[%d %d %d]", p->id, get_now_tv()->tv_sec, viptime->begin_tm);
					response_proto_uint32(p, p->waitcmd, 0, 0);
					return 0;
				}
				int l = 0;
				uint8_t dbpkg[256];
				PKG_H_UINT32(dbpkg, 0, l);
				PKG_H_UINT32(dbpkg, 12751, l);
				PKG_H_UINT32(dbpkg, 12755, l);
				PKG_UINT8(dbpkg, 2, l);
				return send_request_to_db(SVR_PROTO_CHK_ITEM, p, l, dbpkg, p->id);
			}
			break;
		default:
			break;
	}
	int i = sizeof(protocol_t);
	if (viptime->result == 0) {
		days_left = ((viptime->flag == 2) ? 30 : ((mktime(&end) - mktime(&cur)) / 86400));
	}

	PKG_UINT32(msg, days_left, i);
	PKG_UINT32(msg, flag, i);
	init_proto_head(msg, p->waitcmd, i);
	send_to_self(p, msg, i, 1);
	DEBUG_LOG("VIP INFO\t[%u %u]", p->id, viptime->result);
	if(viptime->result == 0)
		return 0;
	else {
		ERROR_RETURN(("db error: uid=%u ret=%u", p->id, viptime->result), 0);
	}
}

/*
affect db protocol 0x1006
*/
int get_pet_and_item_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
#define PET_TASK_INFO_MAX_LEN		40

	assert(!p->login);

	if (id != p->id || p->item_cnt != 0) {
		return 0;
	}

	CHECK_BODY_LEN_GE(len, 12);

	uint32_t suit_item_cnt, pet_cnt, pet_suit_item_cnt, task_pet_cnt;
	int k = 0;
	UNPKG_H_UINT32 (buf, suit_item_cnt, k);
	UNPKG_H_UINT32 (buf, pet_cnt, k);
	UNPKG_H_UINT32 (buf, pet_suit_item_cnt, k);
	UNPKG_H_UINT32 (buf, task_pet_cnt, k); // equal to pet_cnt

	DEBUG_LOG("LOGIN PET NUM\t[%u %u %u %u %u]", p->id, suit_item_cnt, pet_cnt, pet_suit_item_cnt, task_pet_cnt);

	if (pet_cnt > MAX_PETS_PER_SPRITE)
		ERROR_RETURN(("too many pets\t[uid=%u]", p->id), -1);
	// init user home
	map_t* map = get_map(id);
	if (!map) {
		map = alloc_map(id);
	}
	// init user homeland
	map_t* jy_map = get_map(JY_MAP(id));
	if (!jy_map) {
		jy_map = alloc_map(JY_MAP(id));
	}

	CHECK_BODY_LEN(len, 16 + suit_item_cnt * 8 + pet_cnt * (sizeof(pet_t) - sizeof(pet_suppl_info_t)) \
		+ pet_suit_item_cnt * 12 + task_pet_cnt * 44);

	// get user cloth on body from DB
	int i, j;
	for (i = 0; i < suit_item_cnt && i < MAX_ITEMS_WITH_BODY; i++, k += 4) {
		UNPKG_H_UINT32 (buf, p->items[i], k);
	}
	p->item_cnt = i;
	if (suit_item_cnt > MAX_ITEMS_WITH_BODY) {
		ERROR_LOG("user suit item too many[%u %u]", suit_item_cnt, p->id);
		k += 8 * (suit_item_cnt - MAX_ITEMS_WITH_BODY);
	}

	// get pets infomation from DB
	for (j = 0; j != pet_cnt; ++j) {
		memcpy(&(map->pets[j]), buf + k + j * (sizeof(pet_t) - sizeof(pet_suppl_info_t)), sizeof(pet_t) - sizeof(pet_suppl_info_t));
		map->pets[j].suppl_info.uid = id;

		if( get_pet_level(&map->pets[j]) == 4 &&
			(map->pets[j].bit_flag & 1) == 0) {
			char p_msg[2048] = {0};
			int bytes = 0;
			map->pets[j].bit_flag |= 1;
			PKG_H_UINT32(p_msg,map->pets[j].id,bytes);
			PKG_H_UINT32(p_msg,map->pets[j].bit_flag,bytes);
			send_request_to_db(SVR_PROTO_SET_PET_BIT_FLAG,NULL,8,p_msg,p->id);

#ifndef TW_VER
			sprintf(p_msg,"你的拉姆%s已经长大啦，快带它去拉姆世界接受新的考验，进化为神力拉姆吧！",map->pets[j].nick);
			send_postcard("公民管理处",0,p->id,1000138,p_msg,0);
#else
			sprintf(p_msg,"你的拉姆%s已經長大啦，快帶它去拉姆世界接受新的考驗，進化為神力拉姆吧！",map->pets[j].nick);
			send_postcard("公民管理處",0,p->id,1000138,p_msg,0);
#endif
		}
		if (PET_IS_SUPER_LAHM(&map->pets[j])) {
			if ((!PET_TRUSTED(&map->pets[j])) && (!PET_ON_TASK(&map->pets[j])) && (!PET_EMPLOYED(&map->pets[j]))) {
				if (p->followed) {
					PET_RECOVER_FORM(p->followed);
				}
				p->followed = &map->pets[j];
			}
		}
	}
	k += pet_cnt * (sizeof(pet_t) - sizeof(pet_suppl_info_t));
	map->pet_loaded = 1;
	map->pet_cnt = pet_cnt;

	// Calculate the pets attribute
	for (j = 0; j != pet_cnt; j++) {
		calc_pet_attr(id, &(map->pets[j]), 0, 0, 0, 0);
	}
	DEBUG_LOG("LOGIN PET NUM\t[%u %u %u]", p->id, id, map->pet_cnt);
	// Get pets clothes on body from DB
	//for debug,when too many user come the same home
	int loop;
	for (loop = 0; loop < map->pet_cnt; loop++) {
		map->pets[loop].suppl_info.pet_cloth_cnt = map->pets[loop].suppl_info.pet_honor_cnt = 0;
	}
	for (loop = 0; loop < pet_suit_item_cnt; loop++) {

		uint32_t petid, itmid, cnt;
		UNPKG_H_UINT32 (buf, petid, k);
		UNPKG_H_UINT32 (buf, itmid, k);
		UNPKG_H_UINT32 (buf, cnt, k);
		for (j = 0; j < map->pet_cnt; j++) {
			if (map->pets[j].id == petid){
				item_kind_t* kind = find_kind_of_item(itmid);
				if (kind && kind->kind == PET_ITEM_CLOTH_KIND){
					pet_wear_cloth(p, &(map->pets[j]), itmid);
					DEBUG_LOG ("WEARED ID\t[%u %u %d]", p->id, petid, itmid);
				} else if (kind && kind->kind == PET_ITEM_HONOR_KIND){
					pet_wear_honor(p, &(map->pets[j]), itmid);
					DEBUG_LOG ("WEARED ID\t[%u %u %d]", p->id, petid, itmid);
				} else {
					ERROR_RETURN(("error item kind, item=%u, uid=%u petid=%u", itmid, p->id, petid), -1);
				}
				break;
			}
		}
	}
	for (loop = 0; loop < task_pet_cnt; loop++) {

		uint32_t petid;
		UNPKG_H_UINT32(buf, petid, k);
		pet_t* pet = get_pet(p->id, petid);
		if (!pet) {
			k += PET_TASK_INFO_MAX_LEN;
			continue;
		}
		pet->suppl_info.pti.count = 0;
		memcpy(pet->suppl_info.pti.taskinfo, (buf + k), PET_TASK_INFO_MAX_LEN);
		for (j = 0; j < PET_TASK_MAX_CNT; j++) {
			uint32_t tid = pet->suppl_info.pti.taskinfo[j].taskid;
			if (tid) {
				DEBUG_LOG("PET TASK\t[%u %u %u]", p->id, pet->id, tid);
				if ((PET_IS_SUPER_LAHM(pet) && (tid == 102 || tid == 103 || tid == 104))
					|| (!PET_IS_SUPER_LAHM(pet) && (tid == 101))) {
					uint32_t buff[2];
					buff[0] = pet->id;
					buff[1] = tid;
					db_cancel_magic_task(0, buff, p->id);
					DEBUG_LOG("CLEAN TASK\t[%u %u %u]", p->id, pet->id, tid);
					p->magic_task--;
				} else {
					pet->suppl_info.pti.count++;
				}
			}
		}
		k += PET_TASK_INFO_MAX_LEN;
	}
	//check if user not vip now
	DEBUG_LOG("CALL CANCEL SUPER LAHM uid=%u flag=%x", p->id, p->flag);
	cancel_super_lahm(p, map->pets, map->pet_cnt);

	//notify user his pet's sickness or dead
	notify_pet_status(p, map->pets, map->pet_cnt);
	return get_ask_answer_info(p);

}

//-------------------------------------------------------------


//-------------------- adminer operation definitions begin --------------------

// cmd 0xEA61 and beyond for adminer operations
static inline int is_adminer_op(int op)
{
	return (op > 0xEA60);
}

static inline int
kick_user_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	CHECK_BODY_LEN(len, 8);

	uint32_t uid;
	int i = 0, reason;

	UNPKG_H_UINT32(buf, uid, i);
	UNPKG_H_UINT32(buf, reason, i);

	DEBUG_LOG("KICK USER\t[uid=%u reason=%x]", uid, reason);

	int ret = 0;
	sprite_t* p = get_sprite(uid);
	if (p) {
		notify_user_exit(p, reason, 1);
	} else {
		ret = COERR_user_offline;
	}

	return ret;
}

static inline int
broadcast_msg_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 10);

	int i = 0;
	uint32_t uid, msg_type;
	uint16_t msg_len;

	UNPKG_H_UINT32(buf, uid, i);
	UNPKG_H_UINT32(buf, msg_type, i);
	UNPKG_H_UINT16(buf, msg_len, i);

	int len2 = len - 10;
	CHECK_BODY_LEN(len2, msg_len);
	CHECK_BODY_LEN_LE(msg_len, 400);

	int ret = 0;
	if (!uid) {
		send_to_all((void*)(buf + 10), msg_len);
	} else {
		sprite_t* p = get_sprite(uid);
		if (p) {
			send_to_self(p, ((uint8_t*)buf) + 10, msg_len, 0);
		} else {
			ret = COERR_user_offline;
		}
	}

	DEBUG_LOG("BROADCAST MSG\t[recvid=%u msgtype=%u msglen=%d offline=%d]", uid, msg_type, msg_len, ret);
	return ret;
}

static inline int
broadcast_bubble_msg_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 24);

	uint32_t uid;
	uint32_t map_id;
	uint32_t pic_id;
	uint32_t jump_map_id;
	uint32_t msg_type;
	uint32_t msg_len;
	uint32_t task_id;
	uint32_t task_state;

	int i = 0;
	UNPKG_H_UINT32(buf, uid, i);
	UNPKG_H_UINT32(buf, map_id, i);
	UNPKG_H_UINT32(buf, pic_id, i);
	UNPKG_H_UINT32(buf, task_id, i);
	UNPKG_H_UINT32(buf, task_state, i);
	UNPKG_H_UINT32(buf, jump_map_id, i);
	UNPKG_H_UINT32(buf, msg_type, i);
	UNPKG_H_UINT32(buf, msg_len, i);

	int len2 = len - 32;
	CHECK_BODY_LEN(len2, msg_len);
	CHECK_BODY_LEN_LE(msg_len, 400);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, pic_id, j);
	PKG_UINT32(msg, jump_map_id, j);
	PKG_UINT32(msg, task_id, j);
	PKG_UINT32(msg, task_state, j);
	PKG_UINT32(msg, msg_type, j);
	PKG_UINT32(msg, msg_len, j);
	PKG_STR(msg, buf + 32, j, msg_len);
	init_proto_head(msg, PROTO_SEND_BUBBLE_MSG, j);

	int ret = 0;
	/*给服务器上全员发送*/
	if (!uid && !map_id) {
		send_to_all((void*)msg, j);
	/*给某个地图上的发送*/
	} else if (!uid && map_id) {
		if (IS_NORMAL_MAP(map_id)) {
			send_to_map3(map_id, msg, j);
		} else {
			ERROR_LOG("NOT NORMAL MAP ID");
		}
	/*给某个用户发送*/
	} else {
		sprite_t* p = get_sprite(uid);
		if (p) {
			send_to_self(p, msg, j, 0);
		} else {
			ret = COERR_user_offline;
		}
	}

	return ret;
}

//-------------------- adminer operation definitions end --------------------
static int handle_admin_op(int fd, server_proto_t* dbpkg, int len, sprite_t** sp);
static int handle_db_return(int fd, server_proto_t* dbpkg, int len, sprite_t** sp);

int worker_handle_net(int fd, void* buf, int len, sprite_t** u)
{
	assert( len >= sizeof(server_proto_t) );

	server_proto_t* dbpkg = buf;
   	if (fd == mibi_buy_bean_fd) {
		return handle_mibi_buy_bean_return(fd, dbpkg, len, u);
	} else if (fd == goldbean_serv_fd) {
		return handle_goldbean_serv_return(fd, dbpkg, len, u);
	} else if (fd == getbean_serv_fd) {
		return handle_getbean_serv_return(fd, dbpkg, len, u);
	} else if (fd == magicserver_fd) {
		return handle_magicserver_return(fd, dbpkg, len, u);
	} else if (fd == couponserver_fd) {
		return handle_couponserver_return(fd, dbpkg, len, u);
	} else if (fd == central_online_fd) {
		*u = 0;
		return handle_admin_op(fd, dbpkg, len, u);
	} else if (fd == proxysvr_fd) {
		return handle_db_return(fd, dbpkg, len, u);
	} else if (fd == mc_server_fd) {
		return handle_mc_server_return(fd, dbpkg, len, u);
	} else if (fd == stampsvr_tcp_fd) {
		return handle_cs_server_return(fd, dbpkg, len, u);
	} else if (fd == xhx_server_fd) {
		return handle_xhxserver_return(fd, dbpkg, len, u);
	} else if (home_server_fd_idx(fd) != -1) {
		*u = 0;
		return handle_homeserv_pkg(buf, len);
	} else if (fd == ticket_serv_fd) {
		return handle_ticket_serv_return(fd, dbpkg, len, u);
	}

	return 0;
}

static int handle_admin_op(int fd, server_proto_t* dbpkg, int len, sprite_t** sp)
{
	if (dbpkg->seq) {
		int waitcmd = dbpkg->seq & 0xFFFF;
		int conn    = dbpkg->seq >> 16;
		// to prevent reclosing the connection, it returns 0
		if ( !(*sp = get_sprite_by_fd(conn)) || ((*sp)->waitcmd != waitcmd) ) {
			ERROR_RETURN(("connection has been closed, id=%u, cmd=0x%x,0x%x, seq=%u",
					dbpkg->id, dbpkg->cmd, waitcmd, dbpkg->seq), 0);
		}
	}

	int err = -1;
	DEBUG_LOG("Handle msg from switch: cmd[%d]", dbpkg->cmd);

#define DO_MESSAGE(n_, func_) \
	case n_: err = (func_)( *sp, dbpkg->id, dbpkg->body, len - sizeof(server_proto_t) ); break
	switch (dbpkg->cmd) {
		DO_MESSAGE(COCMD_kick_user, kick_user_op);
		DO_MESSAGE(COCMD_broadcast_msg, broadcast_msg_op);
		DO_MESSAGE(COCMD_broadcast_bubble_msg, broadcast_bubble_msg_op);
		DO_MESSAGE(COCMD_sync_vip, sync_vip_op);

		DO_MESSAGE(COCMD_create_npc, create_npc_op);

		DO_MESSAGE(COCMD_chat_across_svr, chat_across_svr_op);
		DO_MESSAGE(COCMD_attr_update_noti, attr_update_op);

		DO_MESSAGE(COCMD_chk_if_online, chk_online_across_svr_callback);
		DO_MESSAGE(COCMD_chk_user_where, chk_user_where_callback);
		DO_MESSAGE(COCMD_notify_join_blacklist, notify_join_blacklist_across_svr_op);
		DO_MESSAGE(COCMD_guess_start, notify_guess_item_begin_across_svr_op);
		DO_MESSAGE(COCMD_guess_end, notify_guess_item_end_across_svr_op);
		DO_MESSAGE(COCMD_sync_to_user_router,notify_syn_to_user_op);
		DO_MESSAGE(COCMD_notify_use_skill_prize, notify_use_skill_prize_across_svr_op);
		DO_MESSAGE(COCMD_xhx_user_set_value, xhx_user_set_value);
		DO_MESSAGE(COCMD_chat_monitor_warning_user, chat_monitor_warning_user);
		DO_MESSAGE(COCMD_cp_get_beauty_prize, cp_get_beauty_prize_across_svr_callback);
		DO_MESSAGE(COCMD_tell_flash_some_msg, tell_flash_some_msg_across_svr_callback);

#undef DO_MESSAGE
		default:
			ERROR_LOG("Unsupported CMD=0x%X", dbpkg->cmd);
			err = -1;
			break;
	}

	return err;
}

static int process_1301_error(int fd, server_proto_t* dbpkg, int len, sprite_t** sp)
{
	switch ((*sp)->waitcmd) {
	case PROTO_BUY_LIMITED_ITEM:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_reach_geren_limit, 1);
	case PROTO_POLL:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_polled_already, 1);
	case PROTO_LEARN_PET_SKILL:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_pet_skill_learnt_today, 1);
	case PROTO_SET_STH_DONE:
	{
		const exclu_things_t* exclu = *((const exclu_things_t**)((*sp)->session));
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, exclu->id, i);
		PKG_UINT32(msg, 0, i);
		init_proto_head(msg, (*sp)->waitcmd, i);
		return send_to_self(*sp, msg, i, 1);
	}
	case PROTO_GET_FIRE_EGG:
		DEBUG_LOG("ONLY CAN GET ONE EGG\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_client_not_proc, 1);
	case PROTO_VOTE_HOME:
		DEBUG_LOG("VOTE TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_vote_flower_mud_limit_exceeded, 1);
	case PROTO_FERTILIZE_WATER_TREE:
		DEBUG_LOG("YOU FERTILIZE OR WATER TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_fertilize_water_limit_exceeded, 1);
	case PROTO_VOTE_SMC:
		DEBUG_LOG("YOU VOTE SMC TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_vote_smc_limit_exceeded, 1);
	case PROTO_SEND_FU:
		DEBUG_LOG("YOU SEND FU TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_send_fu_limit_exceeded, 1);
	case PROTO_SET_MOLE_PARTY:
		DEBUG_LOG("YOU ADD PARTY TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_add_party_exceeded_per_day, 1);
	case PROTO_INC_RUN_TIMES:
		DEBUG_LOG("YOU RUN TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_run_times_exceeded_per_day, 1);
	case PROTO_SUBMIT_MAGIC_CODE:
		DEBUG_LOG("SUB MAGIC CODE TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_max_submit_magic_time_exceed, 1);
	case PROTO_GET_FRUIT_FREE:
		DEBUG_LOG("GET FREE FRUIT TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		response_proto_uint32(*sp, (*sp)->waitcmd, 0, 0);
		return 0;
	case PROTO_DISCOVER_RAND_ITEM:
	case PROTO_LAHM_FOOD_MACHINE:
	{
		DEBUG_LOG("TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		uint32_t* itemid = (uint32_t*)(*sp)->session;
		int j = sizeof(protocol_t);
		PKG_UINT32(msg, *itemid, j);
		PKG_UINT32(msg, 0, j);
		PKG_UINT32(msg, 0, j); //count
		init_proto_head(msg, (*sp)->waitcmd, j);
		return send_to_self((*sp), msg, j, 1);
	}
	case PROTO_GET_HAPPY_CARD_CLOTH:
		DEBUG_LOG("GET HAPPY CARD XIAOMEE TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_already_get_reward, 1);
	case PROTO_GET_500_XIAOME:
	{
		DEBUG_LOG("GET 50 XIAOMEE TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		uint32_t* itemid = (uint32_t*)(*sp)->session;
		int j = sizeof(protocol_t);
		PKG_UINT32(msg, *itemid, j);
		PKG_UINT32(msg, 0, j);
		PKG_UINT32(msg, 0, j); //count
		init_proto_head(msg, (*sp)->waitcmd, j);
		return send_to_self((*sp), msg, j, 1);
	}
	case PROTO_DOCTOR_CURE:
		cure_lahm_free(*sp);
	case PROTO_DOCTOR_DUTY:
		DEBUG_LOG("DOCTOR WORK TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		clean_all(*sp, 1, 0);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_doctor_duty_max, 1);
	case PROTO_GET_LOTTERY:
		DEBUG_LOG("LOTTERY TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_lottery_too_much, 1);
	case PROTO_SET_GAME_PK_INFO:
		DEBUG_LOG("PK TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_pk_too_many_times, 1);
	case PROTO_EDIT_LAHM_DIARY:
		DEBUG_LOG("EDIT LAHM DIARY TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_edit_lahm_diary_too_many, 1);
	case PROTO_SET_WORK_NUM:
		DEBUG_LOG("DO WORK TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_do_work_too_many, 1);
	case PROTO_GET_WORK_SALARY:
		DEBUG_LOG("GET WORK SALARY TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_already_get_work_salary, 1);
	case PROTO_SEND_DIARY_FLOWER:
		DEBUG_LOG("SEND DIARY FLOWER TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_send_diary_flower_too_many, 1);
	case PROTO_GGJ_EXCHANGE_ITM:
		DEBUG_LOG("GGJ TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_ggj_exchg_too_many, 1);
	case PROTO_GET_SHEEP:
		DEBUG_LOG("GET SHEEP TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_get_sheep_too_many, 1);
	case PROTO_SEND_PET_HOME:
		DEBUG_LOG("SEND PET HOME TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_get_sheep_too_many, 1);
	case PROTO_SEND_ONE_GIFT:
		DEBUG_LOG("SEND GIFT TOO MANY TIMES\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_send_gift_too_many, 1);

	case PROTO_RACE_CONFIRM_ADD_SCORE:
		DEBUG_LOG("THE LIMITE IS REACHED\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_reach_addscore_limit, 1);
	case PROTO_RACE_SPECIAL_AWARD:
		DEBUG_LOG("ALREADY GET SPECIAL AWARD\t[pid=%u ret=%u]", (*sp)->id, dbpkg->ret);
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_already_get_special_award, 1);
	case PROTO_CANDY_FROM_RECY:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_candy_reach_limit_recy, 1);
	case PROTO_SET_QUES_NAIRE:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_alrdy_wenjuan, 1);
	case PROTO_CANDY_PACKET_SEND:
		return do_already_get_packet( *sp);
	case PROTO_GET_CANDY_FROM_OTHER:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_get_candy_othr_too_many, 1);
	case PROTO_VIP_PATCH_WORK:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_vip_work_reach_limit, 1);
	case PROTO_UPDATE_NPC_LOVELY:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_add_npc_lovely_limit, 1);
	case PROTO_BEAUTY_COMPETE:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_reach_total_limit, 1);
	case PROTO_FISHING:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_fishing_reach_day_limit, 1);
	case PROTO_GET_10XIAOMEE_50TIMES:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_get_too_many_money_today, 1);
	case PROTO_OCCUPY_BOOTH:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_be_seller_too_many_times, 1);
	case PROTO_GET_CHRISTMAS_GIFTS:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_get_chris_gift_too_many_times, 1);
	case PROTO_COLLECT_RAIN_GIFT:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_collect_too_many_chris_gift, 1);
	case PROTO_FETCH_JINIAN_JIAOYIN:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_alrdy_fetch_this_jiaoyin, 1);
	case PROTO_RAND_ITEM_SWAP:
	case PROTO_SEND_RAND_ITEM:
	{
		uint32_t rand_flag = *(uint32_t*)((*sp)->session);
		uint32_t itmid = *(uint32_t*)((*sp)->session + 4);
		int j = sizeof(protocol_t);
		PKG_UINT32(msg, rand_flag, j);
		PKG_UINT32(msg, itmid, j);
		PKG_UINT32(msg, 0, j); //count
		init_proto_head(msg, (*sp)->waitcmd, j);
		return send_to_self((*sp), msg, j, 1);
	}
	case PROTO_PAY_MONEY:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_pay_five_times_today, 1);
	case PROTO_SET_EGG_HATCHS:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_egg_hatched_today, 1);
	case PROTO_WANT_ITEM_RAND:
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000+dbpkg->ret), 1);
	case PROTO_GIVE_NPC_SOME_ITEMS:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_reach_day_limit, 1);
	case PROTO_GET_SKILL_ITME_LIBAO:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_you_have_get_skill_libao, 1);
	case PROTO_PET_HELP_CIWEI:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_reach_week_limit, 1);
	case PROTO_GET_PIPI_OR_DOUYA:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_have_catch_enough, 1);
	case PROTO_ADD_SCORE_FOR_WO_WEI_HUI:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_add_score_many_times, 1);
	case PROTO_SUPER_LAMU_PARTY_GET:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_reach_day_limit, 1);
	case PROTO_TANTALIZE_CAT:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_tantalize_cat_many_times, 1);
	case PROTO_ADD_LAMU_GROWTH:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_use_tick_today, 1);
	case PROTO_ADD_STH_RAND:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_have_got_day_limit, 1);
	case PROTO_SNOWBALL_GET_GAME_BONUS:
	case PROTO_GAIN_USE_SKILL_PRIZE_ITEM:
	case PROTO_GAIN_MAP_POS_EGG:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_have_got_day_limit, 1);
	case PROTO_EXCHANGE_STH:
	{
		uint32_t type = 0;
		uint32_t mul = 0;
		uint32_t flag = 0;
		unpkg_host_uint32_uint32_uint32((*sp)->session, &type, &mul, &flag);
		
		if (type >= 1036 && type <= 1048)
		{
			return send_to_self_error(*sp, (*sp)->waitcmd, -Err_already_get_aword, 1);
		}

		if (flag)
		{
			response_proto_uint32_uint32((*sp),(*sp)->waitcmd,type,0,0);
			return 0;
		}
		else
		{
			if(type >= 1461 && type <= 1475 && (type %2 == 1)){
				return send_to_self_error(*sp, (*sp)->waitcmd,  -ERR_expedion_already_got_state , 1);
			}
			else{
				return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_have_got_day_limit, 1);
			}
			
		}
	}
	case PROTO_FIRE_CUP_ADD_GAME_BONUS:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_have_got_day_limit, 1);
	case PROTO_LOOK_STH_CNT:
	case PROTO_GAIN_GAME_ANGLES:
	case PROTO_GAIN_WEEKEND_BONUS:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_have_got_day_limit, 1);
	default:
		{
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_have_got_day_limit, 1);
		}
	}
}

static int handle_db_return(int fd, server_proto_t* dbpkg, int len, sprite_t** sp)
{
	int waitcmd = dbpkg->seq & 0xFFFF;
	int conn    = dbpkg->seq >> 16;
	if( dbpkg->cmd == SVR_PROTO_UPDATE_AUCTION_LIST )
	{
		*sp = 0;
		return charparty_auction_list_update_callback( 0, 0, dbpkg->body, len - sizeof (server_proto_t) );
	}
	if (!dbpkg->seq || waitcmd == PROTO_LOGOUT)
		return 0;

	//to prevent from reclose the connection, it return 0
	if ((!(*sp = get_sprite_by_fd(conn)) || (*sp)->waitcmd != waitcmd)) {
		ERROR_RETURN(("connection has been closed, id=%u, cmd=0x%4X %d seq=%u",
						dbpkg->id, dbpkg->cmd, waitcmd, dbpkg->seq), 0);
	}

	DEBUG_LOG("DB RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, dbpkg->id, waitcmd, dbpkg->cmd, dbpkg->ret);
	if ((*sp)->db_timer) {
		REMOVE_TIMER((*sp)->db_timer);
		(*sp)->db_timer = NULL;
	}
	if (dbpkg->ret == 1001 || dbpkg->ret == 1002 || dbpkg->ret == 1003) {
		send_err_to_monitor((*sp)->waitcmd, (*sp)->id, 1);
	}

	int err = -1;
	switch (dbpkg->ret) {
	case 0:
		break;
	case 1105:
		switch (waitcmd) {
		case PROTO_LOGIN:
			ERROR_RETURN(("User id not exsit: uid=%u", dbpkg->id), -1);
		case PROTO_SEND_ONE_GIFT:
			if (dbpkg->cmd == SVR_PROTO_EXCHG_GIFT) {
				send_one_t* lso = (send_one_t*)(*sp)->session;
				send_gift_fail_callback(*sp, lso);
			}
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_not_exist_user, 1);
		case PROTO_ITEM_BUY:
			if(dbpkg->cmd == SVR_PROTO_CHANGE_XIAOMEE) {
				del_class_item(*sp);
				return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
			}
		case PROTO_JOIN_CLASS:
		case PROTO_MONITOR_VERIFY:
			if(dbpkg->cmd == SVR_PROTO_JOIN_CLASS) {
				uint32_t memberid = *(uint32_t*)(*sp)->session;
				send_request_to_db(SVR_PROTO_DEL_CLASS_MEMBER, NULL, 4, &memberid, (*sp)->id);
				return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
			}
		case PROTO_GET_SHOP_INFO:
		case PROTO_GET_SHOP_BOARD_INFO:
		case PROTO_LIST_EMPLOYEE:
		case PROTO_CHANGE_BUILDING_NAME:
		case PROTO_CHANGE_BUILDING_STYLE:
		case PROTO_SET_BUILDING_INNER_STYLE:
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_have_no_shop, 1);
		case PROTO_DEL_EMPLOYEE:
		{
			int i = 0;
			uint32_t userid;
			uint32_t pet_id;
			UNPKG_H_UINT32((*sp)->session, userid, i);
			UNPKG_H_UINT32((*sp)->session, pet_id, i);

			i = sizeof(protocol_t);
			PKG_UINT32(msg,userid,i);
			PKG_UINT32(msg,pet_id,i);
			init_proto_head(msg, (*sp)->waitcmd, i);
			send_to_map((*sp), msg, i, 1);
			return 0;
		}
		case PROTO_FRIEND_INFO:
		{
			int i = 0;
			uint32_t userid = 0;
			UNPKG_H_UINT32((*sp)->session, userid, i);
			send_request_to_db(SVR_PROTO_DEL_FRIEND, NULL, 4, &userid, (*sp)->id);
            return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_get_friend_not_exist_user, 1);
		}
		case PROTO_GET_CLIENT_OBJ:
			return send_to_self_error(*sp, (*sp)->waitcmd, -(12621), 1);
		case PROTO_FIX_CARD_GAME_BUG:
			response_proto_head(*sp, (*sp)->waitcmd, 0);
			return 0;
		case PROTO_GET_NEW_CARD_EXP:
			return send_to_self_error(*sp, (*sp)->waitcmd, -(ERR_not_have_data), 1);
		default:
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_not_exist_user, 1);
		}
	case 1501:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_board_not_found, 1);
	case 1109:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_already_in_list, 1);
	case 1110:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_not_in_friend_list, 1);
	case 1006:
		switch (waitcmd) {
		case PROTO_SET_EGG_POS:
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_already_have_egg, 1);
		case PROTO_UPDATE_CANDY:
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_candy_reach_limit, 1);
		case PROTO_GAME_SCORE:
			{
				int i = sizeof(protocol_t);
				uint32_t* pInt = (void*)((*sp)->session + i + 32);
				pInt[5] = 0;
			}
			break;
		case PROTO_SHOW_ITEM_USE:
			ERROR_RETURN(("Not Enough Transform Items: uid=%u", (*sp)->id), -1);
		case PROTO_TALK:
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_offline_msg_limit_exceeded, 1);
		case PROTO_SEND_ONE_GIFT:
		{
			send_one_t* lso = (send_one_t*)(*sp)->session;
			send_gift_fail_callback(*sp, lso);
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_his_count_limited, 1);
		}
		case PROTO_ADD_TEMP_ITEM:
		{
			uint32_t itemid;
			(*sp)->sess_len = 0;
			UNPKG_UINT32((*sp)->session,itemid,(*sp)->sess_len);
			response_proto_uint32_uint32((*sp),(*sp)->waitcmd,itemid,0,0);
			(*sp)->sess_len = 0;
			return 0;
		}
		case PROTO_ITEM_BUY:
		if(dbpkg->cmd == SVR_PROTO_CHANGE_XIAOMEE) {
			del_class_item(*sp);
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_count_limited, 1);
		//}
		case PROTO_GET_ITEMS_CNT_IN_BAG:
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_record_count_limited, 1);
		}
		default:
			DEBUG_LOG("DB RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, dbpkg->id, waitcmd, dbpkg->cmd, dbpkg->ret);
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_count_limited, 1);
		}
		break;
	case 1114: // exceeds the limit of some of the items to be exchanged out
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_exchg_out_itm_limit_exceeded, 1);
	case 1115: // friend limit exceeded
		{
			sprite_t* p = get_sprite(dbpkg->id);
			if (p) {
				send_to_self_error(p, p->waitcmd, -ERR_friend_limit_exceeded, 0);
			}
			response_proto_head((*sp), (*sp)->waitcmd, 0);
			return 0;
		}
	case 1116:
		switch((*sp)->waitcmd) {
			case PROTO_ITEM_BUY:
				if(dbpkg->cmd == SVR_PROTO_CHANGE_XIAOMEE) {
					del_class_item(*sp);
				}
			default:
				return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_daily_money_limit_exceeded, 1);
		}
	case 1118: // number of invitee exceeds max limit
		DEBUG_LOG("INVITEE NUM EXCEEDS MAX LIMIT\t[inviter=%u invitee=%u ret=%u]", dbpkg->id, (*sp)->id, dbpkg->ret);
		send_exchg_itm_rsp(*sp, 1);
		return 0;
	case dberr_max_feed_num_exceed:
	{
		uint32_t uid = *(uint32_t*)((*sp)->session);
		if (uid != (*sp)->id) {
			uint32_t itmid = *(uint32_t*)((*sp)->session + 8);
			uint32_t cnt = *(uint32_t*)((*sp)->session + 12);
			db_single_item_op(0, (*sp)->id, itmid, cnt, 1);
		}
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}
	case dberr_state_cannt_fertilized:
	case dberr_fertilize_times_day_limit:
	{
		db_single_item_op(*sp, (*sp)->id, 190574, 1, 1);
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}
	// for mole party
	case dberr_max_party_exceed_per_day:
		if ((*sp)->waitcmd == PROTO_SET_MOLE_PARTY){
			db_clean_sth_done(0, 109, (*sp)->id);
		}
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	// For Temporarily Use Only
	case 1301:
		return process_1301_error(fd, dbpkg, len, sp);
	// For Wish Pool
	case dberr_already_made_wish:
		response_proto_uint32(*sp, (*sp)->waitcmd, 1, 0);
		return 0;
	case dberr_mole_groupid_not_existed:
		if ((*sp)->waitcmd == PROTO_GET_MOLEGROUP_DETAIL)
			db_del_molegroup_from_userinfo(NULL, &(dbpkg->id), (*sp)->id);
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	case dberr_max_molegroup_exceeded:
		if ((*sp)->waitcmd == PROTO_INVITE_JOIN_MOLEGROUP){
			uint32_t groupid = *(uint32_t*)(*sp)->session;
			molegroup_del_member_t tmp;
			tmp.memberid = dbpkg->id;
			tmp.groupownerid = (*sp)->id;
			db_del_member_from_molegroup(NULL, &tmp, groupid);
		}
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	//for mole class
	case dberr_moleclass_max:
	case dberr_moleclass_existed:
		if(dbpkg->cmd == SVR_PROTO_JOIN_CLASS) {
			uint32_t memberid = *(uint32_t*)(*sp)->session;
			send_request_to_db(SVR_PROTO_DEL_CLASS_MEMBER, NULL, 4, &memberid, (*sp)->id);
			return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
		}
	case dberr_moleclass_not_exist:
		clean_class((*sp), dbpkg->id);
	case dberr_moleclass_mem_existed:
	case dberr_moleclass_member_max:
	case dberr_moleclass_mem_not_existed:
	case dberr_moleclass_msgs_exceeded:
	case dberr_db_error:
		if ((*sp)->waitcmd == PROTO_LOGIN && dbpkg->cmd == 0x0B02) {
			check_session(*sp);
			return 0;
		}
		if((*sp)->waitcmd == PROTO_CREATE_BULDING) {
			sprite_t *p = *sp;
			return send_to_self_error(p, p->waitcmd, -ERR_already_create_shop, 1);
		}
	case dberr_sys_error:
		if ((*sp)->waitcmd == PROTO_LOGIN && dbpkg->cmd == 0x0B02) {
			check_session(*sp);
			return 0;
		} else if((*sp)->waitcmd == PROTO_LOGIN) {
			ERROR_RETURN(("db error: uid=%u dbcmd=0x%X", (*sp)->id, dbpkg->cmd), -1);
		}
	case dberr_not_enough_xiaomee:
		switch((*sp)->waitcmd) {
			case PROTO_ITEM_BUY:
				if(dbpkg->cmd == SVR_PROTO_CHANGE_XIAOMEE) {
					del_class_item(*sp);
				}
				break;
			default:
				break;
		}
	//the player which you are being added friend already in your blacklist
	case dberr_existed_in_blacklist:
	// for birthday airship
	case dberr_already_set_birthday:
	// For Pet
	case dberr_max_pet_num_exceeded:
	//For mole group
	case dberr_not_molegroup_owner:
	case dberr_max_owned_molegroup_exceeded:
	case dberr_max_num_in_molegroup_exceeded:
	case dberr_user_not_in_molegroup:
	case dberr_user_already_in_molegroup:
	// For Home
	case dberr_max_home_itm_num_exceeded:
	case dberr_max_home_msgs_exceeded:
	// maomao tree
	case dberr_max_times_fertilize_maomaotree:
	// vote smc
	case dberr_max_times_vote_smc:
	// for christmas
	case dberr_already_get_christmas_wish:
	case dberr_already_set_christmas_wish:
	case dberr_not_set_christmas_wish:
	//for card game
	case dberr_card_inited:
	{
		if ((*sp)->waitcmd == PROTO_CARD_DO_INIT){
			/*1表示已经领取过卡牌册*/
			*(uint32_t *)(*sp)->session = 1;
			return new_card_do_init(*sp, (*sp)->id);
		}
	}
	case dberr_card_add_max:
	//for mole bank
	case dberr_record_big_max_err:
	case dberr_xiaomee_less_threshold_err:
	case dberr_refuse_get_err:
	case dberr_record_not_exist_err:
		if((*sp)->waitcmd == PROTO_FOLLOW_ANIMAL) {
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_client_not_proc, 1);
		}
	case dberr_record_exist_err:
	case dberr_invalid_time_limit_err:
	//for task info
	case dberr_taskid_not_exist:
	//for homeland
	case dberr_not_find_fruit:
	case dberr_fruit_num_exceed:
	case dberr_fruit_overdue:
	case dberr_plant_number_not_exist:
	case dberr_max_planted_seed_exceeded:
	case dberr_not_in_thief_time:
	case dberr_already_thief_this_plant:
	//for pasture
	case dberr_animal_not_adult:
	case dberr_no_item_in_pasture_store:
	case dberr_animal_id_not_exist:
	case dberr_pasture_fish_be_locked:
	case dberr_already_fish_this_pasture:
	case dberr_small_animal_num_exceed:
	case dberr_small_fish_num_exceed:
	case dberr_adult_animal_num_exceed:
	case dberr_happy_card_target_error:
	case dberr_happy_card_not_ready:
	case dberr_happy_card_not_enough:
		if(dbpkg->cmd == SVR_PROTO_TRADE_HAPPY_CARD && (*sp)->id == dbpkg->id) {
			trade_hy_card_db_t trade_info;
			trade_info.identify = 2;
			trade_info.card = *(uint32_t*)((*sp)->session);
			trade_info.card_want = *(uint32_t*)((*sp)->session + 4);
			uint32_t target_uid = *(uint32_t*)((*sp)->session + 8);
			DEBUG_LOG("TRADE HAPPY CARD ROLLBACK [%d %d %d %d]", (*sp)->id, target_uid, trade_info.card, trade_info.card_want);
			send_request_to_db(SVR_PROTO_TRADE_HAPPY_CARD, NULL, sizeof(trade_hy_card_db_t), &trade_info, target_uid);
		}
	//for tmp vip
	case dberr_has_req_superlamn:
	case dberr_req_superlamn_max:
	case dberr_req_superlamn_fail:
	case dberr_already_get_reward:
	case dberr_already_mem_get_reward:
	case dberr_sell_item_limit:
	case dberr_already_car_race_sign:
	case dberr_sports_task_start:
	case dberr_sports_task_not_start:
	case dberr_sports_task_fin:
	case 2101:
	case 2102:
	case 2103:
	case dberr_not_enough_items:
		{
			switch((*sp)->waitcmd) {
			case PROTO_CHANGE_SELF_NANGUA:
				return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_change_self_error, 1);
			case PROTO_GET_CANDY_FROM_OTHER:
				return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_that_user_candy_not_enough, 1);
			case PROTO_FERTILIZE_FLOWER:
				return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_no_chemical_fertilizer, 1);
			case PROTO_COOKING_DISH:
				return send_to_self_error(*sp, (*sp)->waitcmd, -(10000+dberr_need_more_resource), 1);
			case PROTO_FINISH_GREAT_PRODUCE_ORDER:
				return send_to_self_error(*sp, (*sp)->waitcmd,  -ERR_order_cannot_finish_now, 1);
			case PROTO_EXCHANGE_STH:
				{
					uint32_t type = *(uint32_t*)(*sp)->session;
					if(type >= 1461 && type <= 1475 && (type %2 == 1)){
						return send_to_self_error(*sp, (*sp)->waitcmd,  -ERR_expedion_unlock_state , 1);
					}
				}
			}
			
		}


	case dberr_pkee_score_too_small:
	case dberr_diary_be_locked:
	case dberr_diary_id_not_exist:
	case dberr_diary_already_lock:
	case dberr_diary_already_unlock:
	case dberr_not_time_to_get_task2:
	case dberr_have_not_done_task2:
	case dberr_alrdy_attend_magic_task:
	case dberr_no_right_attend_magic:
	case dberr_have_not_done_task1:
	case dberr_closet_reach_max:
	case dberr_not_enough_gift_times:
	case dberr_car_not_exist:
		if((*sp)->waitcmd == PROTO_GET_SHOW_CAR) {
			response_proto_uint32_uint32(*sp, (*sp)->waitcmd, dbpkg->id, 0, 0);
			return 0;
		}
	case dberr_car_exist:
		if((*sp)->waitcmd == PROTO_GET_BUILDING_CERTIFY) {
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_aready_have_certify, 1);
		}
	case dberr_not_found:
	case dberr_alrdy_follow_animal:
	case dberr_have_not_been_gaoguai:
	case dberr_gaoguai_the_tree_times_exceed:
	case dberr_alrdy_gaoguai_this_tree:
	case dberr_already_fu_egg:
	case dberr_too_more_fudan:
	case dberr_egg_not_mature:
	case dberr_take_task_too_many_times:
	case dberr_have_not_done_task:
	case dberr_have_not_take_task:
	case dberr_npc_lovely_not_enough:
	case dberr_reach_fudan_times:
	case dberr_limited_itm_no_more:
	case dberr_alrdy_get_ring_this_home:
	case dberr_insect_reach_limit:
	case dberr_animal_cannot_eat_grass:
	case dberr_fudan_count_limit:
	case dberr_itm_cnt_in_pasture_limit:
	case dberr_plant_pollinated_limit:
	case dberr_stage_cannot_pollinated:
	case dberr_pollinate_too_many_times:
	case dberr_not_enough_chris_gift:
	case dberr_no_chris_gift:
	case dberr_nengliang_cnt_day_limit:
	case dberr_no_chemical_fertilizer:
	case dberr_plant_nbr_not_exist:
	//
	case dberr_pet_task_not_completed:
	case dberr_pet_task_attire_have_given:
	case dberr_illegal_state_change_of_task:
	case dberr_illegal_state_change_1_1:
	case dberr_illegal_state_change_2_1:
	case 3001:
	case 2618:
	case 2619:
	case 2620://CHAO GUO SHANG XIAN
	case 2621: //BIAN HAO BU CUN ZAI
	case 2622://BU NENG YANG ZHI DE DONG WU
	case 2623: //skill type error
	case 2627: //want get an not exsit record
	case 2625:
	case 1004:
	case 2654:
	case 2655:
	case 2684:
	case dberr_not_own_the_item:
	case dberr_cow_out_of_got_milk_times:
	case dberr_cow_have_got_milk_today:
	case dberr_dish_eat_empty:
	case dberr_dish_is_cooking:
	case dberr_dish_not_exsit:
	case dberr_not_set_team:
		if((*sp)->waitcmd == PROTO_GIVE_NPC_DISHS) {
			return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dberr_not_enough_dish), 1);
		}
	case dberr_no_sys_employee:
	case dberr_employee_count_limit:
	case dberr_dish_count_limit:
	case dberr_cooking_time_not_enough:
	case dberr_cooking_bad_timeout:
	case dberr_this_location_used:
	case dberr_all_employee_timeout:
	case dberr_can_not_change_style:
	case dberr_need_more_resource:
	case dberr_you_team_hove_not_win:
	case dberr_you_have_got_sth:
	case dberr_not_enough_dish:
		if((*sp)->waitcmd == PROTO_SET_MAGIC_MAN_EAT_DISH) {
			return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_not_enough_dish_to_sell, 1);
		}
	case dderr_sport_medal_get_day_limit:
	case dderr_sport_diff_time_little:
	case dderr_no_have_gua_gua_card:
	case dberr_type_not_exsit:
	case dberr_new_card_have_init:
	case dberr_max_limit_businessman_goods:
	case dberr_businessman_have_not_this_goods:
	case dberr_dragon_have_growth_max:
	case dberr_dragon_egg_id:
	case dberr_have_the_dragon:
	case dberr_have_egg_hatching:
	case dberr_hatch_time_not_enough:
	case dberr_have_get_dragon_max:
	case dberr_egg_have_become_dragon:
	case dberr_have_no_dragon_err:
	case dberr_have_not_enough_fire_cup:
	case dberr_medal_limit_fire_cup:
	case dberr_get_firecup_prize_aready:
	case dberr_box_no_item_to_share:
	case dberr_box_not_enough_item_to_share:
	case dberr_box_share_limit_today:
	case dberr_box_shared_today:
	case dberr_box_too_many_item:
	case dberr_box_not_have_friendship_box:
	case dberr_box_six_item_limit:
	case dberr_points_too_small:
	case dberr_gift_num_not_exsit:
	case dberr_gift_already_get:
	case dberr_lahm_classroom_no_student:
	case dberr_lahm_classroom_courseid_err:
	case dberr_lahm_classroom_class_end_early:
	case dberr_lahm_classroom_class_begin_aready:
	case dberr_lahm_classroom_not_exam_state:
	case dberr_lahm_classroom_not_exist:
	case dberr_lahm_classroom_graduate_err:
	case dberr_lahm_classroom_add_course_hour_err:
	case dberr_lahm_classroom_class_hour_limit_err:
	case dberr_lahm_classroom_add_course_score_err:
	case dberr_lahm_classroom_max_student_err:
	case dberr_lahm_classroom_class_max_event_err:
	case dberr_lahm_classroom_fellowship_max_limit:
	case dberr_lahm_classroom_fellowship_aready_in:
	case dberr_lahm_classroom_fellowship_not_student:
	case dberr_charity_drive_guess_aready_err:
	case dberr_charity_drive_guess_not_time_err:
	case dberr_give_item_date_max_value:
	case dberr_give_item_aready_this_user:
	case dberr_not_enough_mibi_to_exchange:
	case dberr_get_give_item_bonus_count_not_enough:
	case dberr_chris_cake_score_not_enough:
	case dberr_have_give_cake_to_npc:
	case dberr_have_make_this_cake:
	case dberr_footprint_count_not_enough:
	case dberr_buy_pet_item_item_max:
	case dberr_farm_animal_use_item_max:
	case dberr_farm_animal_item_not_enough:
	case dberr_animal_use_skill_cold_time_limit:
	case dberr_animal_use_skill_max_time_limit:
	case dberr_use_skill_prize_item_over:
	case dberr_catch_lvl_animal_num_max:
	case dberr_exeed_every_inc_value_in_paradise_err:
	case dberr_angelid_mapping_to_dragonid_err:
	case dberr_dragon_cannot_follow_err:
	case dberr_no_fairy_followed_you_err:
	case dberr_followed_fairy_not_mature_err:
	case dberr_not_right_attire_exchange_nimsbus_err:
	case dberr_angel_seed_id_not_exist_err:
	case dberr_level_is_not_exsit_err:
	case dberr_user_not_attend_battle_err:
	case dberr_the_angel_already_mature_err:
	case dberr_angel_speed_tool_reached_used_limit_err:
	case dberr_exceed_one_battle_prize_err:
	case dberr_not_correct_angel_speed_tool_err:
	case dberr_capture_angel_fail_err :
	case dberr_not_correct_angel_capture_tool_err :
	case dberr_not_the_angel_already_variate_success_err :
	case dberr_not_the_angel_already_variate_err :
	case dberr_not_correct_angel_go_out_hospital_err:
	case dberr_the_angel_has_recovered_err :
	case dberr_verify_channel_failed:
	case dberr_the_tool_cannot_used_for_this_angel_err:
	case dberr_prop_used_for_angel_already_err:
	case dberr_show_angel_invalid_angel_id:
	case dberr_regain_angel_no_angel_on_show:
	case dberr_get_angel_honor_aready_err:
	case dberr_get_angel_honor_unlock_err:
	case dberr_compose_angel_level_err:
	case dberr_compose_angel_not_enough_material_err:
	case dberr_dungeon_not_correct_hp_recover_item_err:
	case dberr_dungeon_hp_is_full_err:
	case dberr_dungeon_invalid_exhibit_id_err:
	case dberr_dungeon_invalid_exhibit_state_err:
	case dberr_angry_lahm_invalid_task_id_err:
	case dberr_the_ground_already_plant_err:
	case dberr_not_enough_milk_to_purchase_err:
	case dberr_use_items_level_limit:
	case dberr_level_up_card_count_not_enough:
	case dberr_change_equip_count_limit:
	case dberr_refresh_daily_task_wrong:
	case dberr_time_send_mail_aready:
	case dberr_master_apprentice_err:
	case dberr_no_time_mail_err:
	case dberr_no_time_item_err:
	case dberr_cp_feed_invalid_fodder:
	case dberr_not_friend_userid_err:
	case dberr_already_get_candy_err:
	case dberr_not_enough_share_candy_err:
	case dberr_get_item_count_out_of_max:
	case dberr_get_sprint_festival_day_err:
	case dberr_get_sprint_festival_bonus_aready_err:
	case dberr_cp_produce_machine_machine_lvl_limit_err:
	case dberr_shake_dice_time_too_short_err:
	case dberr_ocean_shell_coin_not_enough_err:
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	case dberr_illegal_roomid:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_room_id_not_exist, 1);
	case dberr_carrry_rock_too_many_today:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_carry_rock_many_today, 1);
	case 2577:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_pasture_no_nets, 1);
	case 2576:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_pasture_nets_old, 1);
	case 2578:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_too_few_to_exchange, 1);
	case 2579 :
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_vip_bu_gou_le, 1);
	case 2580 :
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_you_already_sub_vip, 1);
	case 2651:
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_add_score_many_times, 1);
	case dberr_session_check_error:
	{
#ifndef TW_VER
		ERROR_RETURN(("Failed to Verify session: uid=%u", (*sp)->id), -1);
		return -1;
#else
		break;
#endif
	}
	case dberr_dbproxy_timeout:
	{
		if ((*sp)->waitcmd == PROTO_LOGIN && dbpkg->cmd == 0x0B02) {
			check_session(*sp);
			return 0;
		}
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}
	case dberr_already_set_team:
		if((*sp)->waitcmd == PROTO_SET_LAHM_TEAM) {
			sprite_t *p = *sp;
			response_proto_uint32(p,p->waitcmd,0,0);
			return 0;
		}
	case dberr_christmas_send_sock_max_today:
	{
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_christmas_sock_add_max_today, 1);
	}
	case dberr_christmas_can_not_send_sock:
	{
		return send_to_self_error(*sp, (*sp)->waitcmd, -ERR_christmas_sock_get_yestoday_bonus_first, 1);
	}
	case dberr_plant_engel_alread_done:
	{
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}
	case dberr_not_correct_post_err:
	{
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}
	case dberr_not_all_angel_seed_pos_full_err:
	{
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}
	case dberr_angel_not_mature_err:
	{
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}
	case dberr_not_enough_attire_err:
	{
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}
	case dberr_not_enough_engel_for_free:
	{
		//DEBUG_LOG("=====dberr_not_enough_engel_for_free ======");
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}
	case dberr_nimsbus_value_err:
	{
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}
	default:
	{
		//ERROR_LOG("db error: ret=%d, id=%u, cmd=0x%x", dbpkg->ret, dbpkg->id, dbpkg->cmd);
		//goto failed;
		return send_to_self_error(*sp, (*sp)->waitcmd, -(10000 + dbpkg->ret), 1);
	}
	}

#define DO_MESSAGE(n, func) \
	case n: err = func(*sp, dbpkg->id, dbpkg->body, len - sizeof (server_proto_t)); break
	switch (dbpkg->cmd) {
	DO_MESSAGE(SVR_PROTO_CHK_UID, chk_uid_callback);
	//DO_MESSAGE(SVR_PROTO_GET_USER_COMMON_INFO, get_user_common_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_SPRITE, get_user_common_info_callback);
	DO_MESSAGE(SVR_PROTO_SET_COLOR, set_sprite_return);
	DO_MESSAGE(SVR_PROTO_SET_USER_FLAG, set_user_callback);
	DO_MESSAGE(SVR_PROTO_SET_USER_NICK, set_user_callback);

	DO_MESSAGE(SVR_PROTO_CHARGE, attr_op_return);

	DO_MESSAGE(SVR_PROTO_GET_ITEM, get_items_return);
	DO_MESSAGE(SVR_PROTO_CHK_ITEM, chk_items_return);
	DO_MESSAGE(SVR_PROTO_SET_ITEM, items_op_return);
	DO_MESSAGE(SVR_PROTO_BUY_ITEM, item_trading_callback);
	DO_MESSAGE(SVR_PROTO_SELL_ITEM, item_trading_callback);
	DO_MESSAGE(SVR_PROTO_EXCHG_ITEM, exchg_item_callback);
	DO_MESSAGE(SVR_PROTO_PUT_ON_CLOTHES, use_body_item_callback);

	DO_MESSAGE(SVR_PROTO_SET_TASK, set_task_callback);
	DO_MESSAGE(SVR_PROTO_GET_TASK_TMP_INFO, get_task_tmp_info_callback);
	DO_MESSAGE(SVR_PROTO_SET_TASK_TMP_INFO, no_body_return);
	DO_MESSAGE(SVR_PROTO_GET_TASK_INFO, task_info_callback);
	DO_MESSAGE(SVR_PROTO_SET_TASK_INFO, task_info_callback);
	DO_MESSAGE(SVR_PROTO_DEL_TASK_INFO, del_task_info_callback);

	DO_MESSAGE(SVR_PROTO_ADD_FRIEND, add_friend_return);
	DO_MESSAGE(SVR_PROTO_DEL_FRIEND, del_friend_return);
	DO_MESSAGE(SVR_PROTO_ADD_BKLIST, no_body_return);
	DO_MESSAGE(SVR_PROTO_DEL_BKLIST, no_body_return);
	DO_MESSAGE(SVR_PROTO_GET_BKLIST, get_bklist_return);

	DO_MESSAGE(SVR_PROTO_ADD_MAIL, send_mail_return);
	DO_MESSAGE(SVR_PROTO_SET_VIP_LEVEL, set_vip_level_callback);
	DO_MESSAGE(SVR_PROTO_GET_MAIL, get_user_common_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_PET_ITEM_INFO, get_pet_and_item_info_callback);

	DO_MESSAGE(SVR_PROTO_GET_HOME_I, get_home_return);
	DO_MESSAGE(SVR_PROTO_SET_HOME_I, no_body_return);
	DO_MESSAGE(SVR_PROTO_RESET_HOME_I, reset_home_callback);

	DO_MESSAGE(SVR_PROTO_ADD_PET, add_pet_return);
	DO_MESSAGE(SVR_PROTO_GET_PET, get_pet_return);
	DO_MESSAGE(SVR_PROTO_GET_ONE_PET, get_one_pet_callback);
	DO_MESSAGE(SVR_PROTO_SET_PET2, set_pet2_return);
	DO_MESSAGE(SVR_PROTO_SET_PETS_POS, set_pets_pos_return);
	DO_MESSAGE(SVR_PROTO_SET_PET_SKILL, set_pet_skill_callback);

	DO_MESSAGE(SVR_PROTO_SET_PET_FLAG2, set_pet_arg_callback);
	DO_MESSAGE(SVR_PROTO_SET_PET_NICK, set_pet_arg_callback);
	DO_MESSAGE(SVR_PROTO_SET_PET_COLOR, set_pet_arg_callback);
	DO_MESSAGE(SVR_PROTO_UPDATE_PET_ATTR, set_pet_arg_callback);
	DO_MESSAGE(SVR_PROTO_FEED_PET, set_pet_arg_callback);
	DO_MESSAGE(SVR_PROTO_TRUST_PET, set_pet_arg_callback);
	DO_MESSAGE(SVR_PROTO_PET_TASK, set_pet_arg_callback);
	//
	DO_MESSAGE(SVR_PROTO_GET_TASK2_LIST, get_task2_list_callback);
	DO_MESSAGE(SVR_PROTO_SET_TASK2, set_task2_callback);

	DO_MESSAGE(SVR_PROTO_SET_MAGIC_TASK, set_pet_magic_task_callback);
	DO_MESSAGE(SVR_PROTO_FIN_MAGIC_TASK, fin_pet_magic_task_callback);
	DO_MESSAGE(SVR_PROTO_CANCEL_MAGIC_TASK, cancel_pet_magic_task_callback);
	DO_MESSAGE(SVR_PROTO_SET_MAGIC_TASK_DATA, set_magic_task_data_callback);
	DO_MESSAGE(SVR_PROTO_GET_MAGIC_TASK_DATA, get_magic_task_data_callback);
	DO_MESSAGE(SVR_PROTO_GET_MAGIC_TASK_LIST, get_pet_magic_task_list_callback);

	DO_MESSAGE(SVR_PROTO_ADD_BOARD, uint32_return);
	DO_MESSAGE(SVR_PROTO_GET_BOARD, get_board_return);
	DO_MESSAGE(SVR_PROTO_FIND_BOARD, find_board_return);
	DO_MESSAGE(SVR_PROTO_GET_PASSED_BOARD_MSG, get_passed_boardmsg_by_id_callback);

	// For Mole Messenger
	DO_MESSAGE(SVR_PROTO_MOLE_MSNGER_ADD_INVITEE, mole_msnger_add_invitee_callback);
	DO_MESSAGE(SVR_PROTO_GET_MOLE_MSNGER_INFO, mole_msnger_chk_info_callback);

	// For Wish Pool
	DO_MESSAGE(SVR_PROTO_ADD_WISH, add_wish_callback);
	DO_MESSAGE(SVR_PROTO_CHK_IF_WISH_MADE, chk_if_wish_made_callback);
	DO_MESSAGE(SVR_PROTO_DEL_WISH, del_wish_callback);
	DO_MESSAGE(SVR_PROTO_GET_WISH_LIST, get_wish_list_callback);

	// For Postcard System
	DO_MESSAGE(SVR_PROTO_SEND_POSTCARD, send_postcard_callback);
	DO_MESSAGE(SVR_PROTO_READ_POSTCARD, read_postcard_callback);
	DO_MESSAGE(SVR_PROTO_GET_POSTCARDS, get_postcards_callback);
	DO_MESSAGE(SVR_PROTO_DEL_POSTCARD, del_postcard_callback);
	DO_MESSAGE(SVR_PROTO_GET_UNREAD_CARD_NUM, get_unread_card_num_callback);
	DO_MESSAGE(SVR_PROTO_SET_POSTCARD_GIFT_FLAG, set_postcard_gift_flag_callback);
	DO_MESSAGE(SVR_PROTO_GET_POSTCARDS_BASE_INFO, get_postcards_base_info_callback);
	DO_MESSAGE(SVR_PROTO_DEL_POSTCARDS, del_postcards_callback);

	//For home information, about hot, flower, mud,visitor, and neighbor
	DO_MESSAGE(SVR_PROTO_GET_HOT, get_home_hot_callback);
	DO_MESSAGE(SVR_PROTO_LIST_TOP_HOT, get_top_hot_homes_callback);
	DO_MESSAGE(SVR_PROTO_LIST_RECENT_VISITOR, get_recent_visitors_callback);
	DO_MESSAGE(SVR_PROTO_ADD_NEIGHBOR, add_neighbors_callback);
	DO_MESSAGE(SVR_PROTO_LIST_NEIGHBOR, get_neighbors_callback);
	DO_MESSAGE(SVR_PROTO_LEAVE_HOME_MSG, submit_message_callback);
	DO_MESSAGE(SVR_PROTO_GET_HOME_MSG, get_message_callback);
	DO_MESSAGE(SVR_PROTO_GET_MM_TREE_INFO, get_maomao_tree_info_callback);
	DO_MESSAGE(SVR_PROTO_OPERATE_HOME_TYPE_OBJ, operate_home_type_obj_callback);
	DO_MESSAGE(SVR_PROTO_LIST_OPERATED, list_operated_callback);
	DO_MESSAGE(SVR_PROTO_GET_HOME_TYPE_LIST, get_home_type_list_callback);
	DO_MESSAGE(SVR_PROTO_GET_INFO_BY_TYPE, get_info_by_type_callback);

	// for mole group
	DO_MESSAGE(SVR_PROTO_GET_GROUP_LIST, get_user_molegroup_list_callback);
	DO_MESSAGE(SVR_PROTO_ADD_GROUP_TO_USRINFO, add_molegroup_to_userinfo_callback);
	DO_MESSAGE(SVR_PROTO_RM_GROUP_FROM_USRINFO, del_molegroup_from_userinfo_callback);
	DO_MESSAGE(SVR_PROTO_APPLY_GROUPID, apply_molegroup_id_callback);
	DO_MESSAGE(SVR_PROTO_CREATE_GROUP, create_molegroup_callback);
	DO_MESSAGE(SVR_PROTO_ADD_MEMBER_TO_GROUP, add_member_to_molegroup_callback);
	DO_MESSAGE(SVR_PROTO_DEL_MEMBER_FROM_GROUP, del_member_from_molegroup_callback);
	DO_MESSAGE(SVR_PROTO_MOD_GROUP_INFO, mod_molegroup_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_GROUP_DETAIL, get_molegroup_detail_callback);
	DO_MESSAGE(SVR_PROTO_DEL_GROUP, del_molegroup_callback);
	DO_MESSAGE(SVR_PROTO_GET_OWNER_BY_GROUPID, db_get_owner_by_groupid_callback);
	DO_MESSAGE(SVR_PROTO_SET_GROUP_FLAG, set_molegroup_flag_callback);

	//For small requirement
	DO_MESSAGE(SVR_PROTO_GET_QUES_ANSWER, get_ques_answer_callback);
	DO_MESSAGE(SVR_PROTO_GET_QUES_NUM, get_ques_num_callback);
	DO_MESSAGE(SVR_PROTO_INC_RUN_TIMES, inc_run_times_callback);
	DO_MESSAGE(SVR_PROTO_GET_RUN_TIMES, get_run_times_callback);
	DO_MESSAGE(SVR_PROTO_GET_USER_FLAG, get_user_flag_callback);
	DO_MESSAGE(SVR_PROTO_GET_BAOHE_OP_LIST, get_baohe_op_list_callback);

	//For pet item iperation
	DO_MESSAGE(SVR_PROTO_GET_PET_ITEM_LIST, get_pet_items_cnt_callback);
	DO_MESSAGE(SVR_PROTO_BUY_PET_ITEM, buy_pet_item_callback);
	DO_MESSAGE(SVR_PROTO_SET_PET_ITEM, db_single_pet_item_op_callback);
	DO_MESSAGE(SVR_PROTO_GET_PET_WEARED, db_get_all_pet_weared_callback);

	//For HomeLand
	DO_MESSAGE(SVR_PROTO_GET_HOMELAND_ITEM, get_homeland_item_callback);
	DO_MESSAGE(SVR_PROTO_GET_HOMELAND_BOX_ITEM, get_homeland_box_item_callback);
	DO_MESSAGE(SVR_PROTO_SET_HOMELAND_ITEM, no_body_return);
	DO_MESSAGE(SVR_PROTO_HOMELAND_PLANT, homeland_plant_callback);
	DO_MESSAGE(SVR_PROTO_ROOT_OUT_PLANT, homeland_root_out_callback);
	DO_MESSAGE(SVR_PROTO_WATER_PLANT, homeland_water_plant_callback);
	DO_MESSAGE(SVR_PROTO_KILL_BUG, homeland_kill_bug_callback);
	DO_MESSAGE(SVR_PROTO_GET_ONE_PLANT, homeland_get_one_plant_callback);
	DO_MESSAGE(SVR_PROTO_HARVEST_FRUITS, homeland_harvest_fruits_callback);
	DO_MESSAGE(SVR_PROTO_LIST_RECENT_JY_VISITOR, get_recent_jy_visitors_callback);
	DO_MESSAGE(SVR_PROTO_SET_JY_USED_ITM, no_body_return);
	DO_MESSAGE(SVR_PROTO_CHK_IF_USER_EXIST, chk_if_user_exist_callback);
	DO_MESSAGE(SVR_PROTO_THIEF_FRUIT, homeland_thief_fruit_callback);
	DO_MESSAGE(SVR_PROTO_DEC_POLLINATE_CNT, homeland_dec_pollinate_callback);
	DO_MESSAGE(SVR_PROTO_POLLINATE_FLOWER, homeland_pollinate_flower_callback);
	DO_MESSAGE(SVR_PROTO_FERTILIZE_FLOWER, homeland_fertilize_plant_callback);
	// for birthday airship
	DO_MESSAGE(SVR_PROTO_SET_BIRTHDAY, set_birthday_callback);

	//for profession
	DO_MESSAGE(SVR_PROTO_GET_PROFESSION, get_profession_callback);
	DO_MESSAGE(SVR_PROTO_GET_MONTH_TASKS, get_month_tasks_callback);

	//for Send Gift
	DO_MESSAGE(SVR_PROTO_EXCHG_GIFT, exchg_gift_callback);
	DO_MESSAGE(SVR_PROTO_MODIFY_SEND_GIFT_CNT, modify_send_gift_times_callback);
	DO_MESSAGE(SVR_PROTO_GET_SEND_GIFT_CNT, get_gift_times_callback);

	//For Pasture
	DO_MESSAGE(SVR_PROTO_GET_PASTURE, get_pasture_callback);
	DO_MESSAGE(SVR_PROTO_CAPTURE_ANIMAL, pasture_capture_animal_callback);
	DO_MESSAGE(SVR_PROTO_PASTURE_ADD_FEED, pasture_add_feed_callback);
	DO_MESSAGE(SVR_PROTO_HERD_ANIMAL, pasture_herd_animal_callback);
	DO_MESSAGE(SVR_PROTO_PASTURE_GET_ANIMAL, pasture_get_animal_callback);
	DO_MESSAGE(SVR_PROTO_PASTURE_ADD_WATER, pasture_add_water_callback);
	DO_MESSAGE(SVR_PROTO_LIST_RECENT_PASTURE_VISITOR, get_recent_pasture_visitors_callback);
	DO_MESSAGE(SVR_PROTO_SET_PASTURE_ITEMS, set_pasture_items_callback);
	DO_MESSAGE(SVR_PROTO_GET_STOREHOUSE_ITEM, get_store_items_callback);
	DO_MESSAGE(SVR_PROTO_PASTURE_LOCK, pasture_lock_callback);
	DO_MESSAGE(SVR_PROTO_SHEEP_OUTPUT_NUM, get_sheep_reward_callback);
	DO_MESSAGE(SVR_PROTO_ANIMAL_OUTGO, follow_animal_callback);
	DO_MESSAGE(SVR_PROTO_ANIMAL_CHICAO, animal_chicao_callback);
	DO_MESSAGE(SVR_PROTO_RELEASE_ANIMAL, pasture_release_animal_callback);

	// For Game PK
	DO_MESSAGE(SVR_PROTO_GET_GRADE_LIST_BY_GID, get_frd_game_grade_list_callback);
	DO_MESSAGE(SVR_PROTO_GET_GRADE_LIST_BY_UID, get_game_grade_list_callback);
	DO_MESSAGE(SVR_PROTO_UPDATE_PK_FLAG, set_pk_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_PK_LIST, get_pk_history_list_callback);
	DO_MESSAGE(SVR_PROTO_GET_PK_SCORE, get_pk_score_callback);

	// Memory DB Operations Callbacks
	DO_MESSAGE(SVR_PROTO_CHK_IF_STH_DONE, chk_if_sth_done_callback);
	DO_MESSAGE(SVR_PROTO_SET_STH_DONE, set_sth_done_callback);
	DO_MESSAGE(SVR_PROTO_SET_MONTH_TASK, set_month_task_callback);
	//
	DO_MESSAGE(SVR_PROTO_GET_COUNT, chk_cnt_callback);
	DO_MESSAGE(SVR_PROTO_GET_CNT_LIST, get_cnt_list_callback);
	DO_MESSAGE(SVR_PROTO_GET_DAY_LIMIT_LIST, get_day_limit_list_callback);

	// For VIP
	DO_MESSAGE(SVR_PROTO_CHK_VIP_INFO, chk_vip_info_callback);

	// For mole party
	DO_MESSAGE(SVR_PROTO_GET_MOLE_PARTY, get_mole_party_callback);
	DO_MESSAGE(SVR_PROTO_SET_MOLE_PARTY, set_mole_party_callback);
	DO_MESSAGE(SVR_PROTO_GET_OWN_MOLE_PARTY, get_own_mole_party_callback);
	DO_MESSAGE(SVR_PROTO_GET_PARTY_COUNT, get_party_count_callback);

	// For spring festival
	DO_MESSAGE(SVR_PROTO_GET_SPRING_MSG, get_spring_msg_callback);

	// For mibi sys
	DO_MESSAGE(SVR_PROTO_IS_SET_PAY_PASSWD, is_set_pay_passwd_callback);

	// Temperarily Use Only
	DO_MESSAGE(SVR_PROTO_SUBMIT_PERSONAL_INFO, submit_personal_info_callback);
	DO_MESSAGE(SVR_PROTO_CHK_IF_INFO_SUBMITTED, chk_if_info_submitted_callback);
	DO_MESSAGE(SVR_PROTO_CHK_MAGIC_CODE_USED, chk_magic_code_used_callback);
	//
	DO_MESSAGE(SVR_PROTO_BUY_REAL_ITEM, buy_real_item_callback);

	//card books
	DO_MESSAGE(SVR_PROTO_CARD_DO_INIT, card_do_init_callback);
	DO_MESSAGE(SVR_PROTO_CARD_GET_INFO, card_get_info_callback);
	DO_MESSAGE(SVR_PROTO_CARD_ADD_BASIC, add_basic_card_callback);
	DO_MESSAGE(SVR_PROTO_CARD_ADD_AVAIL, add_basic_card_avail_callback);

	//For mole bank
	DO_MESSAGE(SVR_PROTO_GET_DEPOSIT_INFO, get_deposit_info_callback);
	DO_MESSAGE(SVR_PROTO_DEPOSIT_BEAN, deposit_bean_callback);
	DO_MESSAGE(SVR_PROTO_DRAW_BEAN, draw_bean_callback);
	DO_MESSAGE(SVR_PROTO_GET_ASK_ANSWER_INFO, get_ask_answer_info_callback);
	//For mole class
	DO_MESSAGE(SVR_PROTO_CREATE_CLASS, create_class_callback);
	DO_MESSAGE(SVR_PROTO_GET_CLASS_INFO, get_class_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_CLASS_SIMP_INFO, get_class_simple_info_callback);
	DO_MESSAGE(SVR_PROTO_SET_CLASS_INFO, modify_class_info_callback);
	DO_MESSAGE(SVR_PROTO_ADD_CLASS_MEMBER, add_class_member_callback);
	DO_MESSAGE(SVR_PROTO_DEL_CLASS_MEMBER, delete_class_member_callback);
	DO_MESSAGE(SVR_PROTO_CLASS_ADD_ITEM, add_class_itm_callback);
	DO_MESSAGE(SVR_PROTO_GET_CLASS_LOGO_INFO, get_class_logo_info_callback);
	DO_MESSAGE(SVR_PROTO_SET_CLASS_ITEM, set_itm_class_callback);
	DO_MESSAGE(SVR_PROTO_JOIN_CLASS, join_class_callback);
	DO_MESSAGE(SVR_PROTO_QUIT_CLASS, quit_class_callback);
	DO_MESSAGE(SVR_PROTO_GET_FIRST_CLASS, get_first_class_callback);
	DO_MESSAGE(SVR_PROTO_SET_FIRST_CLASS, set_first_class_callback);
	DO_MESSAGE(SVR_PROTO_CHANGE_XIAOMEE, change_xiaomee_callback);
	DO_MESSAGE(SVR_PROTO_DEL_CLASS, delete_class_callback);
	DO_MESSAGE(SVR_PROTO_CLASS_LIST, get_usr_class_list_callback);
	DO_MESSAGE(SVR_PROTO_GET_CLASS_HONOR, get_class_honor_callback);
	DO_MESSAGE(SVR_PROTO_GET_CLASS_QUES_REWARD, get_class_ques_reward_callback);
	DO_MESSAGE(SVR_PROTO_SET_VISIT_FLAG, set_class_visit_flag_callback);
	DO_MESSAGE(SVR_PROTO_GET_CLASS_ITM_UNUSED, get_class_unused_itm_callback);
	DO_MESSAGE(SVR_PROTO_DELETE_CLASS_ITM_UNUSED, del_class_unused_itm_callback);
	DO_MESSAGE(SVR_PROTO_GET_CLASS_MEMBER_LIST, get_member_list_callback);
	DO_MESSAGE(SVR_PROTO_LEAVE_CLASS_MSG, submit_message_callback);
	DO_MESSAGE(SVR_PROTO_GET_CLASS_MSG, get_message_callback);

	DO_MESSAGE(SVR_PROTO_SET_HAPPY_CARD, set_happy_card_callback);
	DO_MESSAGE(SVR_PROTO_GET_HAPPY_CARD, get_happy_card_callback);
	DO_MESSAGE(SVR_PROTO_TRADE_HAPPY_CARD, trade_happy_card_callback);
	DO_MESSAGE(SVR_PROTO_GET_TRADE_HAPPY_CARD_INFO, get_trade_happy_card_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_HAPPY_CLOTH, get_happy_card_cloth_callback);
	DO_MESSAGE(SVR_PROTO_SET_CLASS_MEMBER_REWARD, set_class_member_reward_callback);
	DO_MESSAGE(SVR_PROTO_SET_CLASS_REWARD, set_class_reward_callback);
	DO_MESSAGE(SVR_PROTO_SET_PET_SICKTYPE, set_pet_sicktype_callback);

	// lahm diary
	DO_MESSAGE(SVR_PROTO_SET_LAHM_DIARY, edit_diary_callback);
	DO_MESSAGE(SVR_PROTO_GET_DIARY_TITLE, get_diarys_title_callback);
	DO_MESSAGE(SVR_PROTO_DELETE_DIARY, delete_diary_callback);
	DO_MESSAGE(SVR_PROTO_SET_DIARY_LOCK, set_diary_lock_state_callback);
	DO_MESSAGE(SVR_PROTO_SEND_FLOWER, send_diary_flower_callback);
	DO_MESSAGE(SVR_PROTO_GET_LAHM_DIARY, get_diary_content_callback);
	DO_MESSAGE(SVR_PROTO_SET_BIG_LOCK, set_big_lock_callback);
	DO_MESSAGE(SVR_PROTO_GET_BIG_LOCK, get_big_lock_callback);
	DO_MESSAGE(SVR_PROTO_GET_DIARY_NUM, get_diary_num_callback);

	// work system
	DO_MESSAGE(SVR_PROTO_ADD_WORK_NUM, set_work_num_callback);
	DO_MESSAGE(SVR_PROTO_GET_WORK_NUM, get_work_num_callback);
	DO_MESSAGE(SVR_PROTO_SET_ENGINEER_LEVEL, set_engineer_level_callback);

	//tmp: class question
	DO_MESSAGE(SVR_PROTO_GET_CLASS_QUESTION_STAT, get_class_ques_stat_callback);
	DO_MESSAGE(SVR_PROTO_SUBMIT_CLASS_QUESTION, submit_class_ques_callback);
	DO_MESSAGE(SVR_PROTO_GET_CLASS_QUESTION_INFO, get_class_ques_info_callback);
	DO_MESSAGE(SVR_PROTO_ENABLE_TMP_SUPERLAMN, enable_tmp_superlamn_callback);

	DO_MESSAGE(SVR_PROTO_SET_CLOSET, set_closet_callback);
	DO_MESSAGE(SVR_PROTO_GET_CLOSET, get_closet_callback);
	DO_MESSAGE(SVR_PROTO_UNSET_CLOSET, unset_closet_callback);

	DO_MESSAGE(SVR_PROTO_CLASS_GET_SCORE, get_class_score_callback);
	DO_MESSAGE(SVR_PROTO_GET_CANDY_TIMES, fetch_candy_callback);
	DO_MESSAGE(SVR_PROTO_CAR_INFO, car_info_callback);
	DO_MESSAGE(SVR_PROTO_GARAGE_LIST, list_cars_callback);
	DO_MESSAGE(SVR_PROTO_SET_SHOW_CAR, set_show_car_callback);
	DO_MESSAGE(SVR_PROTO_SHOW_CAR_INFO, get_show_car_callback);
	DO_MESSAGE(SVR_PROTO_BUY_CAR, buy_car_callback);
	DO_MESSAGE(SVR_PROTO_CAR_REFUEL, refuel_callback);
	DO_MESSAGE(SVR_PROTO_CHECK_CAR, check_car_callback);
	DO_MESSAGE(SVR_PROTO_SET_DRIVE_TIME, set_drive_time_callback);

	DO_MESSAGE(SVR_QUERY_ITEM_COUNT, query_item_cnt_callback);
	DO_MESSAGE(SVR_PROTO_CANDY_GET, get_candy_callback);
	DO_MESSAGE(SVR_PROTO_CANDY_MAKE, update_candy_callback);
	DO_MESSAGE(SVR_PROTO_CANDY_STATUS, get_candy_status_callback);

	DO_MESSAGE(SVR_PROTO_PASTURE_GET_NETS, pasture_get_nets_callback);
	DO_MESSAGE(SVR_PROTO_PASTURE_GET_NETS_STATUS, pasture_get_nets_status_callback);
	DO_MESSAGE(SVR_PROTO_PASTURE_CATCH_FISH, pasture_catch_fish_callback);

	DO_MESSAGE(SVR_PROTO_CANDY_PROP, change_candy_prop_callback);
	DO_MESSAGE(SVR_PROTO_GET_CANDY_COUNT, get_candy_count_callback);
	DO_MESSAGE(SVR_PROTO_SUB_VIP_CANDY, sub_vip_candy_callback);
	DO_MESSAGE(SVR_PROTO_GET_BIBO_INFO, get_bibo_date_info_callback);

	DO_MESSAGE(SVR_PROTO_ADD_CULTI_BREED_EXP, add_culti_breed_exp_callback);
	DO_MESSAGE(SVR_PROTO_GET_USER_SKILL, pasture_get_level_ex_callback);

	DO_MESSAGE(SVR_PROTO_SET_DAN, set_egg_pos_callback);
	DO_MESSAGE(SVR_PROTO_FUDAN, process_fudan_callback);
	DO_MESSAGE(SVR_PROTO_GET_FUDAN_TIMES, get_fudan_times_callback);
	DO_MESSAGE(SVR_PROTO_CUT_YANGMAO, cut_yangmao_callback);
	DO_MESSAGE(SVR_PROTO_IS_RABIT_SHENG, check_is_sheng_rabit_callback);
	DO_MESSAGE(SVR_PROTO_GET_LITTLE_RABIT_INFO, get_little_rabit_callback);
	DO_MESSAGE(SVR_PROTO_BREAK_EGG, break_fire_egg_callback);

	DO_MESSAGE(SVR_PROTO_TAKE_NPC_TASK, take_npc_task_callback);
	DO_MESSAGE(SVR_PROTO_FIN_NPC_TASK, fin_npc_task_callback);
	DO_MESSAGE(SVR_PROTO_SET_NT_CLIENT_DATA, set_nt_client_data_callback);
	DO_MESSAGE(SVR_PROTO_GET_NT_CLIENT_DATA, get_nt_client_data_callback);
	DO_MESSAGE(SVR_PROTO_GET_NPC_TASK_STATUS, get_npc_task_status_callback);
	DO_MESSAGE(SVR_PROTO_GET_NPC_TASKS, get_npc_tasks_callback);
	DO_MESSAGE(SVR_PROTO_CANCEL_NPC_TASK, cancel_npc_task_callback);

	DO_MESSAGE(SVR_PROTO_GET_LITTLE_RABIT, fetch_little_rabit_callback);
	DO_MESSAGE(SVR_PROTO_UPDATE_SCORE, beauty_dress_compete_update_callback);
	DO_MESSAGE(SVR_PROTO_GET_SCORE, beauty_dress_compete_callback);
	DO_MESSAGE(SVR_PROTO_SET_PHOTO_DRESS, set_photo_dress_callback);
	DO_MESSAGE(SVR_PROTO_GET_PHOTO_DRESS, get_photo_dress_callback);
	DO_MESSAGE(SVR_PROTO_SET_MODEL_WEAR, set_model_wear_callback);
	DO_MESSAGE(SVR_PROTO_GET_MODEL_WEAR, get_model_wear_callback);
	DO_MESSAGE(SVR_PROTO_SET_MOLE_MODEL_WEAR, set_model_wear_to_mole_callback);
	DO_MESSAGE(SVR_PROTO_SET_MODEL_MOLE_WEAR, set_mole_wear_to_model_callback);
	DO_MESSAGE(SVR_PROTO_SET_CAKE_TIMES, get_rabit_master_cloth_callback);
	DO_MESSAGE(SVR_QUERY_MODLE_INDEX, show_query_model_index_callback);
	DO_MESSAGE(SVR_PROTO_SET_NUM_TYPE, echo_set_num_type_callback);
	DO_MESSAGE(SVR_PROTO_GET_NUM_TYPE, echo_get_num_type_callback);
	DO_MESSAGE(SVR_PROTO_GET_ALL_FISH, get_all_fish_weight_callback);
	DO_MESSAGE(SVR_PROTO_GET_TUJIAN, get_all_tujian_callback);
	DO_MESSAGE(SVR_PROTO_WEIGHTING, weighing_fish_callback);
	DO_MESSAGE(SVR_PROTO_UPDATE_MAX_WEIGHT, update_max_fish_weight_callback);
	DO_MESSAGE(SVR_PROTO_PUT_ON_PET_ITEM, use_pet_item_callback);
	DO_MESSAGE(SVR_PROTO_YOUYOU_ADD_ANIMAL, buy_limited_item_callback);
	DO_MESSAGE(SVR_PROTO_GET_LIMIT_ITEM_NUM, get_limited_item_callback);
	DO_MESSAGE(SVR_PROTO_GET_RING_FRON_OTHER, get_ring_from_other_callback);
	DO_MESSAGE(SVR_PROTO_UPDATE_CHRISTMAS_WISH, set_christmas_wish_callback);
	DO_MESSAGE(SVR_PROTO_GET_CHRISTMAS_WISH, get_christmas_wish_callback);
	DO_MESSAGE(SVR_PROTO_GET_GIFT_NUM, get_christmas_gift_num_callback);
	DO_MESSAGE(SVR_PROTO_CHANGE_GIFT_NUM, change_chris_gifts_num_callback);
	DO_MESSAGE(SVR_PROTO_ADD_SEND_GIFT_LIST, add_op_to_chris_callback);
	DO_MESSAGE(SVR_PROTO_GET_GIFT_FROM_BILU, get_chris_gift_from_bilu_callback);
	DO_MESSAGE(SVR_PROTO_ADD_NENGLIANG_CNT, add_nengliang_callback);
	DO_MESSAGE(SVR_PROTO_GET_NENGLIANG_CNT, get_nengliang_cnt_callback);
	DO_MESSAGE(SVR_PROTO_SET_PHOTO_DAHEZHAO, set_photo_dahezhao_callback);
	DO_MESSAGE(SVR_PROTO_GET_PHOTO_DAHEZHAO, get_photo_dahezhao_callback);
	DO_MESSAGE(SVR_PROTO_GET_FRUIT_FROM_NBR, get_fruit_from_nbr_callback);
	DO_MESSAGE(SVR_PROTO_GET_PET_TASK_ATTIRE_INFO, get_pet_task_attire_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_EGG_HATCH_TIMES, get_penguin_egg_hatch_times_callback);
	DO_MESSAGE(SVR_PROTO_SET_EGG_HATCH_TIMES, set_penguin_egg_hatch_times_callback);
	DO_MESSAGE(SVR_PROTO_BUY_SOME_ITEMS, buy_some_cloths_callback);
	DO_MESSAGE(SVR_PROTO_RAND_TIMES, get_rand_times_callback);
	DO_MESSAGE(SVR_PROTO_RAND_ITEM_WANT, rand_item_want_callback);
	DO_MESSAGE(SVR_PROTO_GET_PET_ATTIRE_CNT,get_pet_attries_count_callback);
	DO_MESSAGE(SVR_PROTO_GET_ITEMS_CNT_IN_BAG, get_items_cnt_in_bag_callback);
	DO_MESSAGE(SVR_PROTO_GET_CRRRY_ROCK_NUM, mole_get_carry_rock_num_callback);
	DO_MESSAGE(SVR_PROTO_SET_CRRRY_ROCK_NUM, mole_set_carry_rock_num_callback);
	DO_MESSAGE(SVR_PROTO_ADD_LAMU_GROWTH, add_lamu_growth_callback);
	DO_MESSAGE(SVR_PROTO_SET_PET_SKILL_EX, set_pet_skill_ex_callback);
	DO_MESSAGE(SVR_PROTO_SET_PET_HOT_SKILL, set_pet_hot_skill_callback);
	DO_MESSAGE(SVR_PROTO_GET_LAHM_TASK_STATE_EX, get_lahm_task_state_callback);
	DO_MESSAGE(SVR_PROTO_SET_LAHM_TASK_STATE_EX, set_lahm_task_state_callback);
	DO_MESSAGE(SVR_PROTO_GET_LAHM_TASK_DATA_EX, get_lahm_task_data_callback);
	DO_MESSAGE(SVR_PROTO_SET_LAHM_TASK_DATA_EX, set_lahm_task_data_callback);
	DO_MESSAGE(SVR_PROTO_LAHM_BUILD_TEMPLE, build_temple_callback);
	DO_MESSAGE(SVR_PROTO_GET_PET_TASK_LIST_EX, get_pet_list_by_task_callback);
	DO_MESSAGE(SVR_PROTO_GET_TEMPLE,get_build_temple_callback);
	DO_MESSAGE(SVR_PROTO_GET_ITEM_BY_USER_SKILL, get_item_by_skill_callback);
	DO_MESSAGE(SVR_PROTO_GET_USER_DATA, get_user_data_callback);
	DO_MESSAGE(SVR_PROTO_SET_USER_DATA, set_user_data_callback);
	DO_MESSAGE(SVR_PROTO_SET_ANIMAL_FLAG, get_little_tiger_callback);
	DO_MESSAGE(SVR_PROTO_SET_YUANXIAO_WISH, set_yuanxiao_wish_callback);
	DO_MESSAGE(SVR_PROTO_GET_YUANXIAO_WISH, get_yuanxiao_wish_callback);
	DO_MESSAGE(SVR_PROTO_SET_PET_SKILL_TYPE,set_pet_skill_type_callback);
	DO_MESSAGE(SVR_PROTO_EXCHANGE_ALL_TO_ANOTHER,exchange_thing_to_another_callback);
	DO_MESSAGE(SVR_PROTO_QUIT_MACHINE_DOG, quit_machine_dog_callback);
	DO_MESSAGE(SVR_PROTO_FEED_MACHINE_DOG, feed_machine_dog_callback);
	DO_MESSAGE(SVR_PROTO_GET_DOG_DO_THING_INFO, get_machine_dog_do_thing_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_MILK_FROM_COW, get_milk_from_cow_callback);
	DO_MESSAGE(SVR_PROTO_UPDATE_WO_WEI_HUI_SCORE, add_score_for_wo_wei_hui_callback);
	DO_MESSAGE(SVR_PROTO_GET_WO_WEI_HUI_SCORE, get_score_for_wo_wei_hui_callback);
	DO_MESSAGE(SVR_PROTO_GET_DELICOUS_FOOD, get_delicous_food_callback);
	DO_MESSAGE(SVR_PROTO_CREATE_BUILDING, create_building_callback);
	DO_MESSAGE(SVR_PROTO_GET_MAP_BUILDING_INFO, list_map_building_info_callback);
	DO_MESSAGE(SVR_PROTO_CHANGE_BUILDING_NAME, set_building_name_callback);
	DO_MESSAGE(SVR_PROTO_CHANGE_BUILDING_STYLE, set_building_style_callback);
	DO_MESSAGE(SVR_PROTO_GET_MAP_LAST_GRID_ID, get_map_last_grid_callback);
	DO_MESSAGE(SVR_PROTO_SET_INNER_STYLE_IN_USER,set_building_inner_style_callback);
	DO_MESSAGE(SVR_PROTO_GET_SHOP_INFO, get_shop_info_callback);
	DO_MESSAGE(SVR_PROTO_LIST_EMPLOYEE,get_employee_list_callback);
	DO_MESSAGE(SVR_PROTO_GET_BOARD_INFO,get_shop_board_info_callback);
	DO_MESSAGE(SVR_PROTO_ADD_EMPLOYEE, add_employee_callback);
	DO_MESSAGE(SVR_PROTO_DEL_EMPLOYEE, del_employee_callback);
	DO_MESSAGE(SVR_PROTO_COOKING_DISH, cooking_dish_callback);
	DO_MESSAGE(SVR_PROTO_EAT_DISH, eat_dish_callback);
	DO_MESSAGE(SVR_PROTO_CLEAN_DISH,clean_dish_callback);
	DO_MESSAGE(SVR_PROTO_CHANGE_DISH_STATE, change_dish_state_callback);
	DO_MESSAGE(SVR_PROTO_PUT_DISH_IN_BOX,put_dish_in_box_callback);
	DO_MESSAGE(SVR_PROTO_SUB_EVALUATE, sub_shop_evaluate_callback);
	DO_MESSAGE(SVR_PROTO_GET_SHOP_COUNT, get_shop_count_callback);
	DO_MESSAGE(SVR_PROTO_GET_PET_EMPLOYED_LIST,get_pet_employed_list_callback);
	DO_MESSAGE(SVR_PROTO_GET_REPUTATION, get_reputation_callback);
	DO_MESSAGE(SVR_PROTO_SHARE_DELICOUS_FOOD, sub_reputation_callback);
	DO_MESSAGE(SVR_PROTO_CHECK_ANIMAL,tantalize_cat_check_callback);
	DO_MESSAGE(SVR_PROTO_SET_LAHM_TEAM,set_lahm_team_callback);
	DO_MESSAGE(SVR_PROTO_GET_LAHM_TEAM,get_lahm_team_callback);
	DO_MESSAGE(SVR_PROTO_GET_CLIENT_OBJ,get_client_object_callback);
	DO_MESSAGE(SVR_PROTO_SET_CLIENT_OBJ,set_client_object_callback);
	DO_MESSAGE(SVR_PROTO_ADD_TEAM_SPORT_SCORE,ding_pigu_add_score_callback);
	DO_MESSAGE(SVR_PROTO_GET_SPORT_TEAM_SCORE,get_team_score_callback);
	DO_MESSAGE(SVR_PROTO_GET_SPORT_TEAM_MEDAL,get_team_medal_callback);
	DO_MESSAGE(SVR_PROTO_GET_STH_FOR_MAX_TEAM,add_sh_for_wim_tem_member_callback);
	DO_MESSAGE(SVR_PROTO_ADD_MEDAL_FOR_SELF,add_medal_by_play_game_callback);
	DO_MESSAGE(SVR_PROTO_GET_TOP_PET_SCORE,get_top_10_pet_score_callback);
	DO_MESSAGE(SVR_PROTO_GET_USER_DISH_ALL,get_user_dish_all_callback);
	DO_MESSAGE(SVR_PROTO_GET_DISH_COUNT,get_dish_count_callback);
	DO_MESSAGE(SVR_PROTO_GIVE_DISH_NPC,give_npc_shop_dish_callback);
	DO_MESSAGE(SVR_PROTO_GET_SELF_MEDAL_NUM,get_myself_medal_num_callback);
	DO_MESSAGE(SVR_PROTO_GET_TEAM_PRIZE_1,get_lahm_team_prize_1_callback);
	DO_MESSAGE(SVR_PROTO_GET_TEAM_PRIZE_2,get_lahm_team_prize_2_callback);
	DO_MESSAGE(SVR_PROTO_GET_DISH_LEVEL, get_dish_count_and_level_callback);
	DO_MESSAGE(SVR_PROTO_GET_DISH_LEVEL_ALL,get_dish_count_and_level_all_callback);
	DO_MESSAGE(SVR_PROTO_GET_MONEY_BY_CARD,get_money_by_gua_gua_card_callback);
	DO_MESSAGE(SVR_PROTO_GET_TYPE_DATA,get_type_data_callback);
	DO_MESSAGE(SVR_PROTO_SET_TYPE_DATA,set_type_data_callback);
	DO_MESSAGE(SVR_PROTO_GET_SESSION,get_session_callback);
	DO_MESSAGE(SVR_PROTO_ADD_SHOP_EVENT_COUNT,add_shop_event_count_callback);
	DO_MESSAGE(SVR_PROTO_GET_NEW_CARD_EXP,get_new_card_exp_callback);
	DO_MESSAGE(SVR_PROTO_INIT_NEW_CARD,new_card_do_init_callback);
	DO_MESSAGE(SVR_PROTO_GET_NEW_CARD, get_new_card_callback);
	DO_MESSAGE(SVR_PROTO_CHECK_SESSION, check_session_by_session_server_callback);
	DO_MESSAGE(SVR_PROTO_CHECK_SESSION_TW, check_session_by_session_server_tw_callback);
	DO_MESSAGE(SVR_PROTO_CHECK_USERID_EXSIT,check_userid_exsit_callback);
	DO_MESSAGE(SVR_PROTO_GET_BUSINESSMAN_GOODS, get_businessman_goods_callback);
	DO_MESSAGE(SVR_PROTO_BUY_BUSINESSMAN_GOODS, buy_businessman_goods_callback);
	DO_MESSAGE(SVR_PROTO_GET_DRAGON_BAG, get_dragon_bag_callback);
	DO_MESSAGE(SVR_PROTO_CALL_DRAGON, call_dragon_callback);
	DO_MESSAGE(SVR_PROTO_RELEASE_DRAGON, release_dragon_callback);
	DO_MESSAGE(SVR_PROTO_SET_DRAGON_NAME, set_dragon_name_callback);
	DO_MESSAGE(SVR_PROTO_FEED_DRAGON, feed_dragon_callback);
	DO_MESSAGE(SVR_PROTO_BROOD_DRAGON_EGG, brood_dragon_egg_callback);
	DO_MESSAGE(SVR_PROTO_QUICK_BROOD_EGG, accelerate_dragon_brood_callback);
	DO_MESSAGE(SVR_PROTO_TAKE_LITTLE_DRAGON, get_baby_dargon_callback);
	DO_MESSAGE(SVR_PROTO_GET_BROOD_EGG_TIME, get_brood_dragon_egg_time_callback);
	DO_MESSAGE(SVR_PROTO_GET_CALLED_DRAONG_INFO, get_call_dragon_info_callback);
	DO_MESSAGE(SVR_PROTO_SPORT_SIGN, sign_fire_cup_callback);
	DO_MESSAGE(SVR_PROTO_GET_SPORT_TEAM, get_team_id_callback);
	DO_MESSAGE(SVR_PROTO_SPORT_ADD_MEDAL, add_medal_num_callback);
	DO_MESSAGE(SVR_PROTO_GET_FIRE_CUP_NUM, get_sprite_medal_num_callback);
	DO_MESSAGE(SVR_PROTO_GET_FIRE_CUP_TEAM_MEDAL, get_team_medal_num_callback);
	DO_MESSAGE(SVR_PROTO_PUT_ITEM_TO_FRIEND_BOX, add_item_in_friendship_box_callback);
	DO_MESSAGE(SVR_PROTO_CHECK_ITEM_FROM_FRIEND, check_item_from_friendship_box_callback);
	DO_MESSAGE(SVR_PROTO_GET_ITEM_FROM_FRIEND_BOX, get_item_from_friendship_box_callback);
	DO_MESSAGE(SVR_PROTO_GET_FRIEND_BOX_ITEMS, query_items_from_friendship_box_callback);
	DO_MESSAGE(SVR_PROTO_GET_FRIEND_BOX_HISTORY, get_item_history_from_friendship_box_callback);
	DO_MESSAGE(SVR_PROTO_GET_FIRE_CUP_PRIZE, get_race_teamid_flag_callback);
	DO_MESSAGE(SVR_PROTO_USER_CHECKIN_BLACK, user_check_in_black_callback);
	DO_MESSAGE(SVR_PROTO_GET_LAMU_COUNT_STATE, get_lahm_count_accord_state_callback);

	DO_MESSAGE(SVR_PROTO_CREATE_CLASSROOM, create_classroom_callback);
	DO_MESSAGE(SVR_PROTO_GET_LAST_FRAMEID, get_classroom_last_grid_callback);
	DO_MESSAGE(SVR_PROTO_GET_FRAME_CLASSROOM, get_grid_classroom_info_callback);
	DO_MESSAGE(SVR_PROTO_SET_CLASSROOM_NAME, set_classroom_name_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_SET_INNER_STYLE, set_classroom_inner_style_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_GET_CLASS_INFO, get_classroom_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_LAMU_CLASSROOM_ADD_LAMUS, add_lahm_students_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_ABANDON_LAMUS, del_lahm_students_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_GET_LAMUS_INFO, get_lahm_students_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_GET_TEACHER_INFO, get_lahm_teacher_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_GET_TEACH_PLAN, get_teach_plan_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_CLASS_BEGIN, set_classroom_class_begin_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_SET_TEACH_MODE, set_classroom_class_mode_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_GET_BASE_TEACH_INFO, query_all_students_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_CLASS_END, set_classroom_class_end_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_GET_COURSE_NUM, get_classroom_course_level_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_GET_COURSE_INFO, get_classroom_course_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_LAMU_CLASSROOM_EXAM, lahm_classroom_exam_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_GET_EXAM_INFO, get_lahm_teacher_exam_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_GET_CLASSROOM_COUNT, get_lahm_classroom_count_callback);
	DO_MESSAGE(SVR_PROTO_GET_RAND_USERID, get_classroom_rand_userid_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_GET_CLASS_FLAG, get_classroom_class_flag_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_CLASS_GRADUATE, lahm_classroom_class_graduate_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_GET_REMEMBRANCE, lahm_classroom_teach_remembrance_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_CHECK_ADD_COURSE, lahm_classroom_check_add_course_callback);
	DO_MESSAGE(SVR_PROTO_GET_FRIEND_CLASSROOM_INFO, get_friend_classroom_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_ADD_COURSE_SCORE, add_lahm_classroom_game_score_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_ADD_LESSON_EVENT, add_lahm_classroom_lesson_event_callback);
	DO_MESSAGE(SVR_PROTO_USER_CLASSROOM_DO_FELLOWSHIP, on_lahm_classroom_fellowship_callback);

	DO_MESSAGE(SVR_PROTO_SYSARG_AUCTION_ADD_ONE_RECORD, guess_item_price_charity_drive_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_AUCTION_GET_ATTIREID_AND_TIME, query_guess_item_charity_drive_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_DONATE_ITEM_AND_XIAOMEE, donate_item_charity_drive_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_CONTRIBUTE_GET_RANK, query_charity_drive_type_rank_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_QUERY_CHARITY_DRIVE, query_charity_drive_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_JY_ANIMAL_ITEMS, get_jy_anmimal_items_callback);
	DO_MESSAGE(SVR_PROTO_GET_EVENT_RECHARGE_MONTHS,get_the_event_recharge_months_callback);

	DO_MESSAGE(SVR_PROTO_CHECK_ITEM_GIVE_FRIEND,check_item_give_friend_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_TYPE_RANK,query_type_rank_callback);
	DO_MESSAGE(SVR_PROTO_GET_ITEM_HISTORY_RECORD,query_get_item_history_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_DEC_RANK_TYPE_COUNT,get_type_count_item_bonus_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_TYPE_RANK_COUNT_INFO,get_type_rank_count_info_callback);

	DO_MESSAGE(SVR_PROTO_GET_BEAN_LOTTERY_COUNT, get_bean_lottery_count_callback);
	DO_MESSAGE(SVR_PROTO_GET_USED_MIBI_COUNT, get_used_mibi_count_callback);
	DO_MESSAGE(SVR_PROTO_DEC_USED_MIBI, dec_used_mibi_callback);
	DO_MESSAGE(SVR_PROTO_ROOMINFO_GET_CHRISTMAS_SOCK_INFO, get_christmas_sock_info_callback);
	DO_MESSAGE(SVR_PROTO_ROOMINFO_GET_CHRIS_CAKE_INFO, get_chris_cake_info_callback);
	DO_MESSAGE(SVR_PROTO_ROOMINFO_GET_CHRIS_CAKE_SCORE, get_chris_cake_score_callback);
	DO_MESSAGE(SVR_PROTO_ROOMINFO_EXCHANGE_STH_BY_CAKE_SCORE, exchange_sth_by_cake_score_callback);
	DO_MESSAGE(SVR_PROTO_ROOMINFO_GET_FOOTPRINT_COUNT, get_foot_print_count_info_callback);
	DO_MESSAGE(SVR_PROTO_ROOMINFO_EXCHANGE_FOOTPRINT, exchange_sth_by_footprint_count_callback);
	DO_MESSAGE(SVR_PROTO_ROOMINFO_GIVE_CAKE_TO_NPC, give_cake_to_npc_callback);
	DO_MESSAGE(SVR_PROTO_ROOMINFO_GET_LOGIN_BONUS_INFO, get_continue_login_bonus_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_QUES_NAIRE_INFO, get_ques_naire_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_ANIMAL_LVL, get_animal_lvl_callback);
	DO_MESSAGE(SVR_PROTO_FARM_ANIMAL_USE_ITEM, farm_animal_use_item_callback);
	DO_MESSAGE(SVR_PROTO_GET_ANIMAL_USED_SKILL_INFO, get_animal_used_skill_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_ANIMAL_BASE_INFO, get_animal_base_info_callback);
	DO_MESSAGE(SVR_PROTO_FARM_ANIMAL_USE_SKILL, farm_animal_use_skill_callback);
	DO_MESSAGE(SVR_PROTO_GET_TRAIN_KAKU_NIAN_INFO, get_train_kaku_nian_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_VERY_GOOD_ANIMAL_COUNT, get_very_good_animal_count_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_ADD_ANIMAL_SKILL_COUNT, animal_add_use_skill_count_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_DEC_ANIMAL_SKILL_COUNT, animal_dec_use_skill_count_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_USE_SKILL_INFO, get_use_skill_prize_info_callback);

	DO_MESSAGE(SVR_PROTO_USER_GET_PARADISE, get_paradise_callback);
	DO_MESSAGE(SVR_PROTO_USER_PARADISE_STORE_ITEM, get_paradise_store_items_callback);
	DO_MESSAGE(SVR_PROTO_USER_PARADISE_SET_NIMBUS, add_nimsbus_callback);
	DO_MESSAGE(SVR_PROTO_USER_CHANGE_ANGEL_POS, feed_angle_seed_callback);
	DO_MESSAGE(SVR_PROTO_USER_ANGEL_SIGN_CONTRACT, build_angle_contract_callback);
	DO_MESSAGE(SVR_PROTO_USER_ANGEL_FREE, free_angle_in_paradise_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_CONTRACT_ANGLES, get_contract_angles_callback);
	DO_MESSAGE(SVR_PROTO_USER_TERMINATE_CONTRACT, free_captured_angle_in_paradise_callback);
	DO_MESSAGE(SVR_PROTO_MAKE_ANGEL_FOLLOWED, make_angle_fallowed_callback);
	DO_MESSAGE(SVR_PROTO_CHANGE_TO_ANGEL, change_to_angle_callback);
	DO_MESSAGE(SVR_PROTO_USER_CHANGE_PARADISE_BACKGOUND, no_body_return);
	DO_MESSAGE(SVR_PROTO_GET_FRIEND_PARAIDSE_INFO, get_friend_paradise_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_PARADISE_GET_VISITLIST, get_visitor_paradise_list_callback);
	DO_MESSAGE(SVR_PROTO_GET_VISTOR_ANGEL_INFO, get_visitor_paradise_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_HOT_ITEM, get_hot_item_info_callback);
	DO_MESSAGE(SVR_PROTO_ROOMINFO_GET_LOGIN_WEEKEND_COUNT, get_weekend_continue_login_count_callback);
	DO_MESSAGE(SVR_USE_PARADISE_PROP, use_prop_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_FIGHT_ANGEL, get_angel_info_for_td_callback);
	DO_MESSAGE(SVR_PROTO_FIGHT_LEVEL, get_fight_level_callback);
	DO_MESSAGE(SVR_PROTO_FIGHT_SCORE, get_fight_score_callback);
	DO_MESSAGE(SVR_PROTO_FIGHT_END, td_game_end_callback);
	DO_MESSAGE(SVR_PROTO_USER_CAPTURE_ANGEL_CMD, clean_black_angel_sucess_callback);
	DO_MESSAGE(SVR_PROTO_ADD_ANGEL, add_angel_callback);
	DO_MESSAGE(SVR_PROTO_ANGEL_IN_HOSPITAL, get_angel_in_hospital_callback);
	DO_MESSAGE(SVR_PROTO_USE_TRAR, use_tear_of_jack_callback);
	DO_MESSAGE(SVR_PROTO_LEAVE_HOSPITAL, angel_leave_hospotal_callback);
	DO_MESSAGE(SVR_PROTO_ANGEL_SHOW, angel_show_callback);
	DO_MESSAGE(SVR_PROTO_ANGEL_GET_FAVORITE_INFO, angel_get_favorite_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_BACKGROUND_ID, get_angel_paradise_background_callback);
	DO_MESSAGE(SVR_PROTO_EXCHANGE_DAVIS_BEAN, exchange_davis_bean_callback);
	DO_MESSAGE(SVR_PROTO_GET_ANGEL_HONORS, get_angel_paradise_honors_callback);
	DO_MESSAGE(SVR_PROTO_SET_ANGEL_HONORS, angel_get_honor_bonus_callback);
	DO_MESSAGE(SVR_PROTO_GET_ANGEL_COMPOSE, get_angel_compose_material_callback);
	DO_MESSAGE(SVR_PROTO_USER_COMPOSE_NEW_ANGEL,user_compose_new_angel_callback);
	//dungeon explore
	DO_MESSAGE(SVR_PROTO_DUNGEON_GET_BAG_INFO, dungeon_get_bag_info_callback);
	DO_MESSAGE(SVR_PROTO_DUNGEON_USE_ITEM, dungeon_use_item_callback);
	DO_MESSAGE(SVR_PROTO_DUNGEON_BEGIN_EXPLORE, dungeon_begin_explore_callback);

	//dungeon exhibit
	DO_MESSAGE(SVR_PROTO_DUNGEON_GET_EXHIBIT_INFO, dungeon_get_exhibit_info_callback);
	DO_MESSAGE(SVR_PROTO_DUNGEON_GET_STORAGE_INFO, dungeon_get_storage_info_callback);
	DO_MESSAGE(SVR_PROTO_DUNGEON_SET_EXHIBIT_ITEM, dungeon_set_exhibit_item_callback);
	DO_MESSAGE(SVR_PROTO_DUNGEON_SWAP_EXHIBIT_ITEM, dungeon_swap_exhibit_item_callback);
	DO_MESSAGE(SVR_PROTO_DUNGEON_GET_FRIEND_INFO, dungeon_get_friend_info_callback);
	DO_MESSAGE(SVR_PROTO_DUNGEON_GET_VISITOR_INFO, dungeon_get_visitor_info_callback);
	DO_MESSAGE(SVR_PROTO_DUNGEON_GET_VISITOR_FULL_INFO, dungeon_get_visitor_full_info_callback);
	DO_MESSAGE(SVR_PROTO_DUNGEON_SET_BACKGROUND, dungeon_set_background_map_callback);
	DO_MESSAGE(SVR_PROTO_DUNGEON_SET_EXHIBIT_STATE, dungeon_set_exhibit_state_callback);
	//angry lahm
	DO_MESSAGE(SVR_PROTO_AL_GET_TASK_INFO, al_get_task_info_callback);
	DO_MESSAGE(SVR_PROTO_AL_UPDATE_SCORE, al_update_score_callback);

	DO_MESSAGE(SVR_PROTO_GET_ITEM_ARRAY, get_item_array_callback);
	DO_MESSAGE(SVR_PROTO_GET_BEAN_MALL_LOGIN_TIMES, get_bean_mall_login_times_callback);

	DO_MESSAGE(SVR_PROTO_HAS_GAME_OVER, has_game_over_callback);

	DO_MESSAGE(SVR_PROTO_GET_MONGOLIA_COW_PASTURE, get_mongolia_cow_pasture_callback);
	DO_MESSAGE(SVR_PROTO_PLAY_WITH_MILK_COW, play_with_milk_cow_callback);
	DO_MESSAGE(SVR_PROTO_PLANT_PASTURE_GRASS, plant_pasture_grass_callback);
	DO_MESSAGE(SVR_PROTO_PURCHASE_PASTURE_MILK, purchase_pasture_milk_callback);
	DO_MESSAGE(SVR_PROTO_ROOMINFO_GET_RANDOM_GAIN_TIMES, get_random_gain_times_callback);
	//for angel fight
	DO_MESSAGE(SVR_PROTO_AF_GET_USER_BASE_INFO, af_get_user_base_info_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_USER_PACKAGE, af_get_user_package_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_ALL_SKILL, af_get_fight_skill_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_ALL_CARD, af_get_fight_card_callback);
	DO_MESSAGE(SVR_PROTO_AF_USE_ITEM, af_use_item_callback);
	DO_MESSAGE(SVR_PROTO_AF_USE_CARD, af_use_card_callback);
	DO_MESSAGE(SVR_PROTO_AF_LEVEL_UP, af_user_skill_level_up_callback);
	DO_MESSAGE(SVR_PROTO_AF_CHANGE_EQUIP, af_user_change_equip_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_ALL_FRIENDS, af_get_all_friends_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_ALL_FRIENDS_FIGHT, af_get_all_friends_fight_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_MAP_PASS_POINT, af_get_map_pass_point_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_ONE_SKILL, af_get_one_skill_level_callback);
	DO_MESSAGE(SVR_PROTO_AF_CHECK_GET_PRIZE, af_check_user_get_prize_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_GET_PRIZE, af_user_get_prize_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_GET_SOME_FRIENDS, af_user_get_some_friends_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_GET_SOME_FRIENDS_INFO, af_user_get_some_friends_info_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_SET_WISHING, af_user_set_wishing_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_CHECK_WISHING, af_user_check_wishing_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_FRIEND_WISHING_CNT, af_get_friend_wishing_cnt_callback);
	DO_MESSAGE(SVR_PROTO_AF_DEAL_FRIEND_WISHING, af_deal_friend_wishing_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_FRIEND_EVENT_CNT, af_get_friend_event_callback);
	DO_MESSAGE(SVR_PROTO_AF_DEAL_FRIEND_EVENT, af_deal_friend_event_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_WISH_ITEMID, af_user_get_wishing_itemid_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_GET_FIGHT_RECORD, af_user_get_fight_record_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_GET_WISHING_COLLECT, af_user_get_wishing_collect_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_DAILY_TASK, af_user_get_daily_task_callback);
	DO_MESSAGE(SVR_PROTO_AF_CHECK_FINISH_TASK, af_user_check_finish_task_callback);
	DO_MESSAGE(SVR_PROTO_AF_ADD_FIGHT_EXP, af_user_add_fight_exp_callback);
	DO_MESSAGE(SVR_PROTO_AF_CHECK_REFRESH_TASK, af_user_check_refresh_task_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_FIGHT_BOSS_COUNT, af_user_get_fight_boss_count_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_MASTER_AND_APPRENTICE, af_get_master_and_apprentice_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_APPRENTICE_MARKET, af_user_get_apprentice_market_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_APPRENTICE_INFO, af_user_get_apprentice_info_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_MASTER_INFO, af_user_get_master_info_callback);
	DO_MESSAGE(SVR_PROTO_AF_CHECK_GET_APPRENTICE, af_check_get_an_apprentice_callback);
	DO_MESSAGE(SVR_PROTO_AF_CHECK_APPRENTICE_GET_MASTER, af_check_apprentice_get_master_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_APPRENTICE_TRAINING, af_get_apprentice_trainning_info_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_TRAIN_APPRENTICE, af_user_train_apprentice_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_EXPEL_APPRENTICE, af_user_expel_apprentice_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_DELETE_MASTER, af_apprentice_retray_master_callback);
	DO_MESSAGE(SVR_PROTO_AF_APPRENTICE_RESPECT_MASTER, af_apprentice_respect_master_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_RESPECT_CASH, af_user_get_respect_cash_callback);
	DO_MESSAGE(SVR_PROTO_AF_APPRENTICE_CHECK_FINISH, af_apprentice_check_finish_callback);
	DO_MESSAGE(SVR_PROTO_AF_MASTER_FINISH_APPRENTICE, af_master_finish_apprentice_callback);
	DO_MESSAGE(SVR_PROTO_AF_APPRENTICE_FINISH_APPRENTICE, af_apprentice_finish_apprentice_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_MASTER_RECORD, af_user_get_master_record_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_GET_LEVEL, af_user_get_level_callback);
	DO_MESSAGE(SVR_PROTO_AF_USER_ADD_APPRENTICE, af_user_add_apprentice_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_PVP_FIGHT_RECORD, af_get_pvp_fight_record_callback);
	DO_MESSAGE(SVR_PROTO_AF_GET_PVP_WIN_PRIZE, af_get_pvp_win_prize_callback);

	DO_MESSAGE(SVR_PROTO_USER_SEND_TIME_MAIL, send_time_mail_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_TIME_MAIL, get_time_mail_callback);
	DO_MESSAGE(SVR_PROTO_USER_SEND_TIME_ITEM, send_time_item_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_TIME_ITEM, get_time_item_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_MAIL_ITEM_COUNT, get_time_mail_item_count_callback);
	DO_MESSAGE(SVR_PROTO_DEL_MUL_FRIENDS, del_mul_friends_callback);
	DO_MESSAGE(SVR_PROTO_USER_EAT_MOONCAKE, user_eat_mooncake_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_MOONCAKE, user_get_mooncake_callback);
	DO_MESSAGE(SVR_PROTO_GET_USER_TASK_EX_IDLIST, get_book_list_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_LUCKY_TICKET, user_get_lucky_ticket_callback);
	DO_MESSAGE(SVR_PROTO_USER_CHECK_GET_TICKET, user_check_get_ticket_callback);
	DO_MESSAGE(SVR_PROTO_USER_SWAP_TICKET_PRIZE, user_swap_ticket_prize_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_HISTORY_TICKET, user_look_history_prize_ticket_callback);
	DO_MESSAGE(SVR_PROTO_USER_NATIONAL_DAY_LOGIN, user_get_national_day_login_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_NATIONAL_DAY_GIFT, user_get_national_day_gift_callback);

	DO_MESSAGE(SVR_PROTO_CUTE_PIG_GET_INFO, cutepig_get_game_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_PIG_INFO, cutepig_get_pig_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_KNAPSACK_INFO, cutepig_get_knapsack_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_RAISE_NEW_PIG, cutepig_raise_pig_callback);
	DO_MESSAGE(SVR_PROTO_CP_FEED, cutepig_feed_callback);
	DO_MESSAGE(SVR_PROTO_CP_FEED_GET_INFO, cutepig_feed_get_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_BATHE, cutepig_bathe_callback);
	DO_MESSAGE(SVR_PROTO_CP_TICKLE, cutepig_tickle_callback);
	DO_MESSAGE(SVR_PROTO_CP_TRAIN, cutepig_training_callback);
	DO_MESSAGE(SVR_PROTO_CP_SET_NAME, cutepig_set_name_callback);
	DO_MESSAGE(SVR_PROTO_CP_SET_FORMATION, cutepig_set_formation_callback);
	DO_MESSAGE(SVR_PROTO_CP_SALE_PIG, cutepig_sale_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_MALE_PIG_INFO, cutepig_get_players_male_pig_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_FRIEND_INFO, cutepig_get_friend_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_TASK_INFO, cutepig_get_task_list_callback);
	DO_MESSAGE(SVR_PROTO_CP_ACCEPT_TASK, cutepig_accept_task_callback);
	DO_MESSAGE(SVR_PROTO_CP_SUBMIT_TASK, cutepig_submit_task_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_NOTICE_INFO, cutepig_get_notice_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_PROCESS, cutepig_process_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_RAND_LUCKY, cutepig_get_rand_lucky_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_ILLUSTRATE, cutepig_get_illustrate_callback);
	DO_MESSAGE(SVR_PROTO_CP_UPGRADE_BUILDING, cutepig_upgrade_building_callback);
	DO_MESSAGE(SVR_PROTO_CP_USE_ITEM, cutepig_use_item_callback);
	DO_MESSAGE(SVR_PROTO_CP_MATE, cutepig_mate_callback);
	DO_MESSAGE(SVR_PROTO_CP_MATE_CHECK, cutepig_mate_check_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_TICKET, cutepig_get_ticket_callback);
	DO_MESSAGE(SVR_PROTO_CP_USE_CARD, cutepig_use_card_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_CARD_INFO, cutepig_get_card_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_SET_BACKGROUND, cutepig_set_background_callback);
	//for 2011.11.18
	DO_MESSAGE(SVR_PROTO_CP_FOLLOW, cutepig_set_pig_follow_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_SHOW_STAGE_INFO, cutepig_get_show_stage_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_SHOW, cutepig_beauty_show_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_2ND__GAME_INFO, cutepig_get_secondary_game_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_USE_SHOW_STAGE_ITEM, cutepig_use_show_item_callback);
	DO_MESSAGE(SVR_PROTO_CP_SWAP_PIG, cutepig_swap_callback);
	DO_MESSAGE(SVR_PROTO_CP_SET_CLOTHES, cutepig_set_clothes_callback);
	//end for
	DO_MESSAGE(SVR_PROTO_ADD_FRIEND_REQ, add_friend_req_callback);
	DO_MESSAGE(SVR_PROTO_ADD_FRIEND_REP, reply_add_friend_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_DAREN_VOTE, get_feifei_daren_prize_callback);
	DO_MESSAGE(SVR_PROTO_CP_SHOW_GET_ACHIEVE_INFO, cutepig_get_bs_npc_pk_achieve_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_SHOW_GET_ACHIEVE, cutepig_get_bs_npc_pk_achieve_callback);
	DO_MESSAGE(SVR_PROTO_CP_ILLUSTRATE_CHECK, cutepig_get_pig_by_illustrate_callback);
	DO_MESSAGE(SVR_PROTO_GET_CANDY_FREIND_CNT, get_candy_friend_cnt_callback);
	DO_MESSAGE(SVR_PROTO_GET_CANDY_FROM_FREIND, get_candy_from_friend_callback);
	DO_MESSAGE(SVR_PROTO_GET_CANDY_SHARE_BONUS, get_candy_share_bonus_callback);
	DO_MESSAGE(SVR_PROTO_XHX_GET_TASK, xhx_get_tast_callback);
	DO_MESSAGE(SVR_PROTO_GET_GUIDER_PRIZE, sg_get_guider_prize_callback);
	DO_MESSAGE(SVR_PROTO_LOOK_GUIDER_PRIZE, sg_look_guider_prize_callback);
	DO_MESSAGE(SVR_SYSARG_JOIN_BEAUTY_CONTEND, cutepig_get_beauty_game_prize_callback);
	DO_MESSAGE(SVR_SYSARG_GET_BEAUTY_GAME_RECORD, cutepig_get_beauty_game_record_callback);
	DO_MESSAGE(SVR_SYSARG_GET_BEAUTY_GAME_GIFT, cutepig_check_beauty_game_time_callback);
	DO_MESSAGE(SVR_PROTO_SAVE_AVATAR, save_avatar_callback);
	DO_MESSAGE(SVR_PROTO_REMOVE_AVATAR, remove_avatar_callback);
	DO_MESSAGE(SVR_PROTO_GET_AVATAR_LIST, get_avatar_list_callback);
	DO_MESSAGE(SVR_PROTO_GET_TASK_LIST_INFO, get_task_list_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_AS_ADD_TYPE_INT_DATA, add_as_type_int_data_callback);
	DO_MESSAGE(SVR_PROTO_USER_AS_QUERY_TYPE_INT_DATA, query_as_type_int_data_callback);
	DO_MESSAGE(SVR_PROTO_CHECK_CHRISTMAS_SOCK_CNT, check_christmas_sock_cnt_callback);
	DO_MESSAGE(SVR_PROTO_GET_CHRISTMAS_SOCR_RECORD, get_christmas_sock_record_callback);
	DO_MESSAGE(SVR_PROTO_GET_CHRISTMAS_SOCK_TOP10, get_christmas_sock_top10_callback);
	DO_MESSAGE(SVR_PROTO_CHECK_CHRISTMAS_GIFT, check_christmas_sock_gift_callback);
	DO_MESSAGE(SVR_PROTO_GET_VIP_ANGEL_INFO, get_vip_angel_info_callback);
	DO_MESSAGE(SVR_PROTO_EXCHANGE_VIP_ANGEL, exchange_vip_angel_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_MATCHING_MAP, add_mine_map_piece_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_MAP, get_mine_map_pieces_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_CHECK_PLAYER_NUM, get_all_chance_count_callback);
	DO_MESSAGE(SVR_PROTO_USER_EXPLORE_QUERY_STATE, query_mine_key_chance_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_KEY_CHANCE, get_mine_key_chance_callback);
	DO_MESSAGE(SVR_PROTO_USER_EXPLORE_CLEAR_STATE, get_mine_map_key_callback);	
	DO_MESSAGE(SVR_PROTO_DONATE_ITEM, charparty_donate_item_callback);
	DO_MESSAGE(SVR_PROTO_GET_AUCTION_LIST, charparty_get_auction_list_callback);
	DO_MESSAGE(SVR_PROTO_AUCTION_ITEM, charparty_auction_item_callback);
	DO_MESSAGE(SVR_PROTO_CP_GET_ALL_HONOR, cutepig_get_all_honor_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_DAYTYPE_COUNT, get_day_type_count_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_GOLD_COMPASS_DATA, get_gold_compass_chance_data_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_GOLD_USERS_INFO, get_gold_compass_users_info_callback);
	DO_MESSAGE(SVR_PROTO_WVS_GET_TEAM_POWER, wvs_get_team_power_callback);	
	DO_MESSAGE(SVR_PROTO_USER_SUBMIT_SPRING_COUPLETS, user_submit_spring_couplets_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_SPRING_COUPLETS, user_get_spring_couplets_callback);
	DO_MESSAGE(SVR_PROTO_USER_SET_COUPLETS_PRIZE, user_set_couplets_prize_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_COUPLETS_USERID_LIST, user_get_couplets_userid_list_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_COUPLETS_PRIZE, user_get_couplets_prize_callback);
	DO_MESSAGE(SVR_PROTO_YEAR_FEAST_GET_PRIZE, year_feast_get_prize_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GUESS_RIGHT_USER_INFO, get_guess_right_user_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_SPRINT_FESTIVAL_LOGIN_INFO, get_sprint_festival_login_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_SPRINT_FESTIVAL_LOGIN_BONUS, get_sprint_festival_login_bonus_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_VIP_CONTINUE_WEEK_LOGIN_INFO,get_vip_continue_week_login_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_VIP_WEEK_LOGIN_AWARD, get_vip_week_login_award_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_WEEKEND_LOGIN_INFO, get_another_weekend_login_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_CUTEPIG_GOLD_KEY, get_user_cutepig_gold_key_callback);
	DO_MESSAGE(SVR_PROTO_USER_SET_ITEMID_SCORE, add_user_item_score_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_ITEMID_SCORE, get_user_items_score_callback);
	DO_MESSAGE(SVR_PROTO_USER_CHANGE_ICE_DRAGON, change_ice_dragon_callback);
	DO_MESSAGE(SVR_PROTO_USER_EXCHANGE_PIGLET_HOUSE, exchange_user_piglet_house_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_PIGLET_MACHINE_WORK, cutepig_get_piglet_machine_work_callback);
	DO_MESSAGE(SVR_PROTO_USER_PIG_EXPLOR_MAP_MINE, cutepig_user_pig_explor_mine_callback);
	DO_MESSAGE(SVR_PROTO_GET_USER_EXPLOR_MINE_INFO, cutepig_get_explor_mine_info_callback);
	DO_MESSAGE(SVR_PROTO_CP_USER_PIGLET_MELT_ORE, cutepig_user_piglet_melt_ore_callback);
	DO_MESSAGE(SVR_PROTO_CP_USER_PIGLET_PRODUCE_MACHINE_PART, cutepig_user_produce_machine_part_callback);
    DO_MESSAGE(SVR_PROTO_USER_PIGLET_FINISH_WORK, cutepig_user_pig_finish_work_callback);
    DO_MESSAGE(SVR_PROTO_USER_GET_PIGLET_WORK_MACHINE, cutepig_get_user_piglet_work_machine_callback);
    DO_MESSAGE(SVR_PROTO_USER_USE_ACCELERATE_TOOL, cutepiglet_use_accelerate_machine_tool_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_PIGLET_GUIDER_STEP, cute_piglet_guider_get_work_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_GREAT_PRODUCE_ORDER, get_majesty_or_urgen_order_callback);
	DO_MESSAGE(SVR_PROTO_USER_FINISH_GREAT_PRODUCE_ORDER, finish_majesty_or_urgent_order_callback);
	DO_MESSAGE(SVR_PROTO_USER_THOUSAND_GREAT_PRODUCE_NUMBER, get_greate_produce_fisrt_thousand_callback);
    DO_MESSAGE(SVR_PROTO_USER_CP_PRODUCE_SPECIAL_MACHINE, cutepig_user_produce_special_machine_callback);
	DO_MESSAGE(SVR_PROTO_USER_CP_RANDOM_MACHINE_PRODUCT, cutepig_use_give_random_machine_something_callback);
	DO_MESSAGE(SVR_PROTO_GET_KFC_DECORATION, get_user_KFC_decoration_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_POLL_DECORATION, get_poll_friend_decoration_callback);
    DO_MESSAGE(SVR_PROTO_MW_PUT_ITEM_TO_FRIEND_BOX, mw_add_item_in_friendship_box_callback);
	DO_MESSAGE(SVR_PROTO_MW_CHECK_ITEM_FROM_FRIEND, mw_check_item_from_friendship_box_callback);
	DO_MESSAGE(SVR_PROTO_MW_GET_ITEM_FROM_FRIEND_BOX, mw_get_item_from_friendship_box_callback);
	DO_MESSAGE(SVR_PROTO_MW_GET_FRIEND_BOX_ITEMS, mw_query_items_from_friendship_box_callback);
	DO_MESSAGE(SVR_PROTO_MW_GET_FRIEND_BOX_HISTORY, mw_get_item_history_from_friendship_box_callback);
	DO_MESSAGE(SVR_PROTO_USER_CATCH_BUTTERFLY_ANGEL, user_catch_butterfly_angel_callback);
	DO_MESSAGE(SVR_PROTO_USER_CREATE_MVP_TEAM, create_lahm_sport_mvp_team_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_MVP_GET_TEAMID, get_sysarg_mvp_teamid_callback);
	DO_MESSAGE(SVR_PROTO_USER_ENTER_MVP_TEAM, enter_lahm_sport_mvp_team_callback);
	DO_MESSAGE(SVR_PROTO_USER_LEAVE_MVP_TEAM, leave_lahm_sport_mvp_team_callback);
	DO_MESSAGE(SVR_PROTO_USER_MVP_GET_TEAMID, user_get_sport_mvp_team_callback);
	DO_MESSAGE(SVR_PROTO_USER_DISSOLVE_MVP_TEAM, dissolve_lahm_sport_mvp_team_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_MVP_TEAM_INFO, get_lahm_sport_mvp_team_info_callback);
	DO_MESSAGE(SVR_PROTO_GET_MVP_TEAM_RANKING, user_get_mvp_team_ranking_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_TWO_TEAM_MEDAL_CNT, get_two_team_medal_cnt_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_MVP_TEAM_INFO, get_lahm_sport_sysarg_mvp_team_callback);
	DO_MESSAGE(SVR_PROTO_QUERY_WHICH_PIGLET_CHAMPOIN, check_cute_piglet_champion_activity_callback);
	DO_MESSAGE(SVR_PROTO_GET_PIGLET_CHAMPOIN_AWARD, get_piglet_champion_activity_award_callback);
	DO_MESSAGE(SVR_PROTO_SET_TYPE_PHOTO_HEZHAO, set_type_photo_hezhao_callback);
	DO_MESSAGE(SVR_PROTO_GET_TYPE_PHOTO_HEZHAO, get_type_photo_hezhao_callback);
	DO_MESSAGE(SVR_PROTO_SET_MISS_NOTE, mole_set_miss_note_callback);
	DO_MESSAGE(SVR_PROTO_GET_ENERGY_STONE_ROUND, mole_get_enery_rock_stones_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_USER_WATER, user_water_jack_and_modou_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_WATER_TIME, get_water_time_jack_and_modou_callback);
	DO_MESSAGE(SVR_PROTO_USER_SHAKE_DICE, user_shake_dice_for_vip_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_SHAKE_DICE_LEFT_TIME, user_get_shake_dice_left_time_callback);
	DO_MESSAGE(SVR_PROTO_CHECK_EXPEDITION_LOCK, mole_check_expedition_lock_callback);
	DO_MESSAGE(SVR_PROTO_GET_CHAPTER_STATE, mole_get_chapter_state_callback);
	DO_MESSAGE(SVR_PROTO_GET_MAKE_SHIP, mole_get_make_ship_callback);
	DO_MESSAGE(SVR_PROTO_SET_CHAPTER_STATE, mole_funfly_seahorse_callback);
	DO_MESSAGE(SVR_PROTO_GUESS_SSCLOTH, user_guess_stone_scissors_cloth_callback);
	DO_MESSAGE(SVR_PROTO_CHECK_PRIOR_DICE, check_prior_dice_callback);
	DO_MESSAGE(SVR_PROTO_GET_FREE_VIP_PLAYER, get_free_player_info_callback);
	DO_MESSAGE(SVR_PROTO_MERMAN_KINGDOM_EXPEDITION, mole_merman_kingdom_expedition_callback);
	DO_MESSAGE(SVR_PROTO_OCENA_GET_ONLINE_DROP_SHELLS, user_get_online_shells_callback);
	DO_MESSAGE(SVR_PROTO_SALE_OCEAN_FISH, user_sale_ocean_fish_callback);
	DO_MESSAGE(SVR_PROTO_BUY_OCEAN_THINGS, user_buy_ocean_things_callback);
	DO_MESSAGE(SVR_PROTO_GET_OCEAN_HANDBOOK, mole_get_ocean_handbook_callback);
	DO_MESSAGE(SVR_PROTO_GET_OCEAN_FRIEND_LEVEL, mole_get_friend_ocean_info_callback);


	DO_MESSAGE(SVR_PROTO_USER_GET_USER_OCEAN_INFO, get_user_ocean_info_callback);
	DO_MESSAGE(SVR_PROTO_USER_PUT_OCEAN_ANIMAL_IN_OCEAN, put_user_ocean_animal_in_ocean_callback);
	DO_MESSAGE(SVR_PROTO_USER_FEED_OCEAN_ANIMAL, user_feed_ocean_animal_callback);
	DO_MESSAGE(SVR_PROTO_USER_USE_OCEAN_TOOL, user_use_ocean_tool_callback);
	DO_MESSAGE(SVR_PROTO_USER_EXPAND_OCEAN_CAPACITY, user_expand_ocean_capacity_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_OCEAN_ITEMS, get_user_ocean_items_callback);
	DO_MESSAGE(SVR_PROTO_USER_DIY_SELF_OCEAN, user_diy_self_ocean_callback);
	DO_MESSAGE(SVR_PROTO_USER_GET_OFFLINE_SHELLS, user_get_offline_shells_callback);
	DO_MESSAGE(SVR_PROTO_SET_PRAY_ACITIVITY, mole_set_pray_activity_callback);
	DO_MESSAGE(SVR_PROTO_GET_PRAY_ACITIVITY, mole_get_pray_activity_callback);
	DO_MESSAGE(SVR_PROTO_USER_JOIN_COSPLAY_RACE, user_join_cosplay_race_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_COS_RANK_INFO, sysarg_get_cos_rank_info_callback);
	DO_MESSAGE(SVR_PROTO_SYSARG_GET_COS_RANK_FLAG, get_user_cos_rank_flag_callback);
	DO_MESSAGE(SVR_PROTO_SET_OCEAN_ADVENTURE, mole_set_ocean_adventure_callback);
	DO_MESSAGE(SVR_PROTO_GET_OCEAN_ADVENTURE, mole_get_ocean_adventure_callback);
	DO_MESSAGE(SVR_PROTO_MOLE_GET_SELF_GAMES, get_mole_self_games_callback);
	DO_MESSAGE(SVR_PROTO_MOLE_ENTER_GAME_HALL, mole_enter_game_hall_callback);
	DO_MESSAGE(SVR_PROTO_SCROLL_MAP_STAT, get_srcoll_map_state_callback);
	DO_MESSAGE(SVR_PROTO_SCROLL_FRAGMENT, mole_get_scroll_fragment_callback);
	DO_MESSAGE(SVR_PROTO_GET_GAME_EXP_RANK, get_mole_self_game_rank_callback);
	DO_MESSAGE(SVR_PROTO_FINISH_KINGDOM_EXPEDITION, mole_finish_merman_kingdom_expedition_callback);
	

#undef DO_MESSAGE
	default:
		ERROR_LOG("unknow cmd=0x%x, id=%u, waitcmd=%d", dbpkg->cmd, (*sp)->id, (*sp)->waitcmd);
		break;
	}

//failed:
	return err;
}
