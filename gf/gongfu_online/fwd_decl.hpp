/**
 *============================================================
 *  @file      fwd_decl.hpp
 *  @brief    forward declarations
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_FWD_DECL_HPP_
#define KF_FWD_DECL_HPP_

#include <ctime>

extern "C" {
#include <glib.h>

#include <libtaomee/list.h>
#include <libtaomee/project/types.h>
#include <libtaomee/dirtyword/dirtyword.h>
}

#define DEV_SUMMON


const char* get_date_str();

#define LONG_LOG(fmt, args...) \
    do { NOTI_LOG("%s\t"fmt, get_date_str(), ##args); } while(0)


/**
  * @brief data type for map id
  */
typedef uint64_t map_id_t;

/**
  * @brief compare methods
  */
enum cmp_t {
	/*! len must equal to another given len */
	cmp_must_eq		= 1,
	/*! len must be greater or equal to another given len */
	cmp_must_ge		= 2
};

//-----------------------
// Forward Declarations
//
struct battle_grp_t;
struct player_t;
struct map_t;

/**
  * @brief common constants
  */
enum common_constant {
	/*! version */
	version_taomee = 0,
	version_xiaoba = 1,
	vsersion_kaixin = 2,
	vsersion_taiwan = 3,
	/*! size in bytes of a nick name */
	max_role_type	= 4,
	
	/*! size in bytes of a nick name */
	max_nick_size	= 16,

	max_client_buf_len = 40,
    /*! size in bytes of summon skills */
    max_summon_skills_len = 50,
#ifdef VERSION_KAIXIN
	/*! max lv can get exp */
	max_exp_lv		= 80,
#else
	max_exp_lv		= 80,
#endif
	
	max_ironsmith_lv = 7,
	
	/*! size of once_bit size */
	max_once_bit_size = 40,

    /*! player limit active */
    max_limit_active_times = 40,

	/*! max number of clothes a player can put on */
	max_clothes_on	= 11,

	max_client_len = 40,

	/*! max number of clothes can be repaired (sum of used and unused ) */
	max_repair_cnt = 10 + 50,

	/*! max number of skills player can bind */
	max_skills_bind	= 6,

	/*! max number of items player can bind */
	max_itembind_len = 40,

	/*! min skill level the player can get */
	min_skill_level  = 1,
	
	/*! max skill level the player can get */
	max_skill_level  = 20,
	
	/*! max skill num every player can get */
	max_skill_num  = 1000,

	/*! max summon monster num every player can get */
	max_summon_num  = 30,

	/*! max preskill num needed by other skill */
	max_pre_skill_cnt = 20,

	/*! boundary for role types*/
	skill_id_boundary = 100000,

	/*! max item num needed when compose a product */
	max_product_need_item  = 15,
	max_output_compose_attire = 15,

    /*! max attire level */
    max_attire_level = 12,

	/*!max steps of suit child attr*/
	max_suit_step	= 7,

	/*!max suit sorts*/
	max_suit_id = 500,
	
	/*! max task take into battle */
	max_task_items  = 20,
	
	/*! max task take into battle */
	max_task_monsters  = 10,
	
	/*! the compute ratio of clothes duration */
	clothes_duration_ratio  = 50,

	/*! max level of a player */
	player_max_level        = 80,
	
	/*! size in bytes of the stage list */
	c_stagelist_size		= 200,

	/*! size of item bit buf*/
	c_unique_item_bit_size	= 20,

	/*! max number of the stage */
	max_stage_num			= 1600,

	/*! the max item num of pack */
	max_pack_items_cnt		= 75,

	/*! the max clothes num of pack */
	max_pack_clothes_cnt	= 75,
	
	/*! the max num of daily restriction */
	max_daily_restriction_num = 120,

	/*! time interval for sending oltoday to client */
	report_oltoday_int		= 900,

	/*! deduct experience time limit*/
	exp_half_time_limit     = 7200,

	/*! type of stage difficulty:  1:easy  2:normal  3:hard  4:hellish*/
	max_stage_difficulty_type	= 6,

	/*! the max num of ambassador reward num */
	max_ambassador_reward_num = 10,
	
	/*! the max num of ambassador detail reward num */
	max_ambassador_detail_reward_num = 10,

	/*! max number of restrictions */
	max_restr_count = 3000,
    /*! max number of exchange */
    max_coupons_exchange = 1000,
	/* max goods's prie */
	max_goods_price = 20000000,

	/*max trade_mcast info len*/
	max_trade_mcast_size = 512,

    /*max official_mcast info len*/
	max_official_mcast_size = 500,

    max_event_trigger_time_size = 10,
    max_event_trigger_block_size = 10,


	min_hero_top_lv = 55,
	min_hero_top_sort = 1000,
	default_hero_sort = 2000,

	double_exp_daily_action	= 101,
	double_exp_daily_use_item = 102,

	max_team_member_cnt = 25,

	max_server_config_id = 100,

	max_tmp_session_len = 2 * 6000,
	max_nor_buff_type = 900000,

	pvp_team_score_limit_cnt = 15,
	//stat_log_user_come_back		 =  0x00000000,
	//
	max_uint32_value			= 0xffffffff,
};

enum {
#ifndef VERSION_KAIXIN
	/*! for stat log */
	stat_log_user_on_off		= 0x09010001,
	stat_log_dx_user_on_off		= 0x09010051,
	stat_log_wt_user_on_off		= 0x09010052,
	stat_log_kx_user_on_off		= 0x0F500002,
	stat_log_scan_times_chapter	= 0x0FA00004,
	stat_log_scan_times_chapter1= 0x09010004,
	stat_log_yulingtu       	= 0x09010005,
	stat_log_liuguanglu      	= 0x09010006,
	stat_log_baiyuhu          	= 0x09010007,
	stat_log_use_time_sack  	= 0x09010020,
	stat_log_use_lv_change  	= 0x09010600,
    stat_log_role_exp           = 0x09011000,
    stat_log_tonglingmao        = 0x09012001,
    stat_log_feichu             = 0x09012002,
    stat_log_strengthen_succ    = 0x09020100,
    stat_log_strengthen_fail    = 0x09020110,
    stat_log_material_consume_1 = 0x09020201,
    stat_log_material_consume_2 = 0x09020401,
    stat_log_strengthen_level   = 0x09020500,
	stat_log_major_task		    = 0x090D0000,
	stat_log_minor_task		    = 0x090E0000,
	stat_log_got_amber   		= 0x09090000,
	stat_log_learn_skill   		= 0x09093000,
	stat_log_used_amber     	= 0x090A0000,
	stat_log_used_skillpoints	= 0x090C0000,
	stat_log_used_boost_num 	= 0x090C4000,
	stat_log_fumo_gain          = 0x09200100,
    stat_log_fumo_consume       = 0x09200101,
    stat_log_fumo_swap_drawing_40 = 0x09200200,
    stat_log_fumo_swap_drawing_41 = 0x09200300,
    stat_log_fumo_swap_attire   = 0x09300000,
    stat_log_fumo_swap_drug     = 0x09400000,

    stat_log_exchange_base_cmd  = 0x09500000,

	stat_log_get_amb_reward		= 0x09500010,
	stat_log_achieve_child_num	= 0x09500201,
	stat_log_hatch_summon_mon	= 0x090C3001,
    stat_log_use_plugin         = 0x090C4003,
	stat_log_learn_yaoshi_secondary_pro = 0x09500301,
	stat_log_yaoshi_secondary_pro_level_change = 0x09500302,
    stat_log_gongfucheng_caiji = 0x09500303,
	stat_log_yiqixueyuan_caiji = 0x09500304,
	stat_log_kuangshazheng_caiji = 0x09500305,
	stat_log_gongfuchengxi_caiji = 0x09500306,
	stat_log_shizilukou_caiji    = 0x09500307,
	stat_log_zhujiangu_caiji     = 0x09500308,
	stat_log_qimianzu_caiji      = 0x09500309,
	stat_log_huqiu_caiji         = 0x0950030a,

	stat_log_jinshui_caiji     = 0x09500331,
	stat_log_pihuweiba         = 0x09500332,
	
	stat_log_player_little_game = 0x09500341,

	stat_log_yaoshi_level_up1  =  0x09500351,
	stat_log_yaoshi_level_up2  =  0x09500352,
	stat_log_yaoshi_level_up3  =  0x09500353,
	stat_log_yaoshi_level_up4  =  0x09500354,
	stat_log_yaoshi_level_up5  =  0x09500355,
	stat_log_yaoshi_level_up6  =  0x09500356,
	stat_log_yaoshi_level_up7  =  0x09500357,
	
	stat_log_pvp_times			= 0x0950100A,
    stat_log_pvp_fag			= 0x0950100B,
    stat_log_yaoshi_game		= 0x0950102B,
    stat_log_questionnaire		= 0x09501050,
    stat_log_accept_up_level	= 0x09503002,
    stat_log_finish_up_level	= 0x09503003,
	
	stat_log_daily_task_finish		 = 0x09020001,
	stat_log_daily_task_5_finish 	 = 0x09020002,
	stat_log_daily_task_10_finish	 = 0x09020003,
	stat_log_daily_task_lucky_take  = 0x09020004,
	stat_log_daily_task_lucky_finish = 0x09020005,

	stat_log_daily_get_reward_times = 0x09021101,
	stat_log_relive_item1 = 	0x09522065,
	stat_log_relive_item2 = 	0x09522066,
	stat_log_get_coupons_item = 0x09530011,
	stat_log_produce_coins		 = 0x09520001,
	stat_log_destroy_coins		 = 0x09520002,
	stat_log_get_vip_box	     =  0x09530012,
	stat_log_get_nor_box		 =  0x09530013,
	stat_log_peach_got			 =  0x09508001,
	stat_log_peach_daily		 =  0x09510000,
	stat_log_element_daily		 =  0x09520000,
	stat_log_element_cost		 =  0x09521001,
	stat_log_element_join		 =  0x09521002,
	stat_log_relive_item_use_lv  =  0x09522000,
	stat_log_enter_stage		 =  0x09522101,
	
	stat_log_lingli_neihe		 =  0x09524001,
	stat_log_exchange_times_1		 =  0x09523000,
    stat_log_qiannian_jinghun   = 0x09526001,

    stat_log_vip_level_distribution = 0x0901040C,
	stat_log_new_lv_up			 = 0x09010400,

	stat_log_specal_stage		 = 0x09130000,

    stat_log_online_people_tw   = 0x0FA00001,
    stat_log_on_off_people_tw   = 0x0FA00002,
    stat_log_title_base_cmd      = 0x09820001,
    stat_log_role_login_		= 0x09012006,
#else
	/*! for stat log */
	stat_log_role_login_		= 0,
	stat_log_title_base_cmd = 0,
	stat_log_qiannian_jinghun = 0,
	stat_log_vip_level_distribution = 0,
	stat_log_user_on_off		= 0,
	stat_log_dx_user_on_off		= 0,
	stat_log_wt_user_on_off		= 0,
	stat_log_kx_user_on_off		= 0x0F500002,
	stat_log_scan_times_chapter	= 0,
	stat_log_yulingtu       	= 0,
	stat_log_liuguanglu      	= 0,
	stat_log_baiyuhu          	= 0,
	stat_log_use_time_sack  	= 0,
	stat_log_use_lv_change  	= 0,
    stat_log_role_exp           = 0,
    stat_log_tonglingmao        = 0,
    stat_log_feichu             = 0,
    stat_log_strengthen_succ    = 0,
    stat_log_strengthen_fail    = 0,
    stat_log_material_consume_1 = 0,
    stat_log_material_consume_2 = 0,
    stat_log_strengthen_level   = 0,
	stat_log_major_task		    = 0,
	stat_log_minor_task		    = 0,
	stat_log_got_amber   		= 0,
	stat_log_learn_skill   		= 0,
	stat_log_used_amber     	= 0,
	stat_log_used_skillpoints	= 0,
	stat_log_used_boost_num 	= 0,
	stat_log_fumo_gain          = 0,
    stat_log_fumo_consume       = 0,
    stat_log_fumo_swap_drawing_40 = 0,
    stat_log_fumo_swap_drawing_41 = 0,
    stat_log_fumo_swap_attire   = 0,
    stat_log_fumo_swap_drug     = 0,

    stat_log_exchange_base_cmd  = 0,

	stat_log_get_amb_reward		= 0,
	stat_log_achieve_child_num	= 0,
	stat_log_hatch_summon_mon	= 0,
    stat_log_use_plugin         = 0,
	stat_log_learn_yaoshi_secondary_pro = 0,
	stat_log_yaoshi_secondary_pro_level_change = 0,
    stat_log_gongfucheng_caiji = 0,
	stat_log_yiqixueyuan_caiji = 0,
	stat_log_kuangshazheng_caiji = 0,
	stat_log_gongfuchengxi_caiji = 0,
	stat_log_shizilukou_caiji    = 0,
	stat_log_zhujiangu_caiji     = 0,
	stat_log_qimianzu_caiji      = 0,
	stat_log_huqiu_caiji         = 0,

	stat_log_jinshui_caiji     = 0,
	stat_log_pihuweiba         = 0,
	
	stat_log_player_little_game = 0,

	stat_log_yaoshi_level_up1  =  0,
	stat_log_yaoshi_level_up2  =  0,
	stat_log_yaoshi_level_up3  =  0,
	stat_log_yaoshi_level_up4  =  0,
	stat_log_yaoshi_level_up5  =  0,
	stat_log_yaoshi_level_up6  =  0,
	stat_log_yaoshi_level_up7  =  0,
	
	stat_log_pvp_times			= 0,
    stat_log_pvp_fag			= 0,
    stat_log_yaoshi_game		= 0,
    stat_log_questionnaire		= 0,
    stat_log_accept_up_level	= 0,
    stat_log_finish_up_level	= 0,
	
	stat_log_daily_task_finish		 = 0,
	stat_log_daily_task_5_finish 	 = 0,
	stat_log_daily_task_10_finish	 = 0,
	stat_log_daily_task_lucky_take  = 0,
	stat_log_daily_task_lucky_finish = 0,

	stat_log_daily_get_reward_times = 0,
	stat_log_relive_item1 = 0,
	stat_log_relive_item2 = 0,
	stat_log_get_coupons_item = 0,
	stat_log_produce_coins		 = 0,
	stat_log_destroy_coins		 = 0,
	stat_log_get_vip_box	     =  0,
	stat_log_get_nor_box		 =  0,
	stat_log_peach_got			 =  0,
	stat_log_peach_daily		 =  0,
	stat_log_element_daily		 =  0,
	stat_log_element_cost		 =  0,
	stat_log_element_join		 =  0,
	stat_log_relive_item_use_lv  =  0,
	stat_log_enter_stage		 =  0,
	
	stat_log_lingli_neihe		 =  0,
	stat_log_exchange_times_1		 =  0,
	stat_log_specal_stage		 = 0,

	stat_log_new_lv_up			= 0x0F500003,
	kaixin_stat_log_gongfuquan_shop = 0x0F520000,
#endif
};


enum trade_sys_config_t{
	/*for trade */
	max_trade_svr_num = 16,
	max_market_num  = 420,
	max_market_pre_online = 5,
	max_old_online_id = 300,
	min_freshman_online_id = 1000,
	max_freshman_online_id = 1340,
};

/**
  * @brief vip status position
  */
enum vip_stat_pos_t {
	cur_vip = 1, // bit 1
	prev_vip = 2, //bit 2
    year_vip = 4, // bit 4
};

/**
 * @brief realtime active record pos
 */
enum active_record_pos_t {
    act_record_pos = 8,
	act_lefanfan_pos = 6,
};

/**
  * @brief with what for the purchase
  */
enum buy_type_t {
	buy_with_coins 		 = 0,
	buy_with_exploit 	 = 1,
	buy_with_fumo_points = 2
};

/**
  * @brief buff type which storage in db
  */
enum db_buff_type_t {
	db_buff_type_db_exp = 1,
};

/**
  * @brief pos of once_bit in player_t
  */
enum flag_pos_t {
	flag_pos_ambassador = 0,
	flag_pos_achieve_lv	= 11,

	/*! for daily action */
	flag_pos_daily_active_start = 20,

	flag_pos_shop_limit_buy = max_once_bit_size * 8 - 1,
	flag_pos_shop_tmp_buy = max_once_bit_size * 8,
};


/*! number of buckets for a hash table */
const int c_bucket_size = 200;

#define likely(x)        __builtin_expect(!!(x), 1)
#define unlikely(x)        __builtin_expect(!!(x), 0)


/**
  * @brief hold timer list
  */
struct timer_head_t {
	list_head_t	timer_list;
};

#pragma pack(1)

/**
  * @brief user id list
  */
struct uid_list_t {
	/*! number of user id */
	uint32_t	cnt;
	/*! array of user id */
	userid_t	id[];
};

#pragma pack()

/*! hold common timers */
extern timer_head_t g_events;
/*! pathname where to store the statistic log info */
extern char* statistic_logfile;
/*! current day time limit */
extern uint32_t battle_time_limit;


enum {
	send_bsw_keepalive_pkg_idx = 1, 
	connect_to_battle_switch_timely_idx = 2,
	connect_to_trade_switch_timely_idx = 3, 
	keep_players_alive_idx = 4, 
	set_app_buf_end_idx = 5, 
	kick_all_users_offline_idx = 6,  
	send_online_tm_notification_idx = 7, 
	limit_players_online_time_idx = 8, 
	send_sw_keepalive_pkg_idx = 9, 
	connect_to_switch_timely_idx = 10, 
};

extern const char* channel_string_npc_shop; 								//商店买卖
extern const char* channel_string_storage_shop;            //道具商城买卖
extern const char* channel_string_trade;                            //交易获得
extern const char* channel_string_task;                           //任务
extern const char* channel_string_active;                         //各种活动兑换获得
extern const char* channel_string_pick;                            //战斗中减去
extern const char* channel_string_box;                             //宝箱获得
extern const char* channel_string_use;                              //使用掉
extern const char* channel_string_discard;                        //用户自己丢弃销毁
extern const char* channel_string_other;                            //其它
extern const char* channel_string_sec_pro;
extern const char* channel_string_magic_num;
extern const char* channel_string_home_pick;
extern const char* channel_string_use_tongbao_item;
extern const char* channel_string_decompose;
extern const char* channel_string_compose;
extern const char* channel_string_upgrade_item;
extern const char* channel_string_mail;
extern const char* channel_string_strengthen;
extern const char* channel_string_home;

#endif // KF_FWD_DECL_HPP_

