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

#ifndef KFBTL_FWD_DECL_HPP_
#define KFBTL_FWD_DECL_HPP_

#include <ctime>
#include <set>

extern "C" {
#include <glib.h>

#include <libtaomee/list.h>
#include <libtaomee/project/types.h>
//#include <libtaomee/dirtyword/dirtyword.h>
}

#include <libtaomee++/pathfinder/astar/astar.hpp>
#include <libtaomee++/event/event_mgr.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>


#define DEV_SUMMON
#define UPDATE_NOR_PROXY
typedef taomee::AStar<> KfAstar;

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
class  Player;
//class  AIInterface;
class  Telegram;
class  Battle;
class  ItemDrop;
struct map_t;
struct player_attr_t;
struct player_skill_t;
struct player_task_item_t;
struct player_task_monster_t;
//struct player_task_step_t;
struct player_hpmp_chg_info_t;

/**
  * @brief pos of once_bit in player_t
  */
enum flag_pos_t {
	flag_pos_ambassador = 0,
	flag_pos_achieve_lv	= 11,
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
  * @brief common constants
  */
enum common_constant {
	/*! size in bytes of a nick name */
	max_nick_size			= 16,
#ifdef VERSION_KAIXIN
	/*! max lv can get exp */
	max_exp_lv		= 80,
#else
	max_exp_lv		= 80,
#endif
	/*! size of once_bit size */
	max_once_bit_size = 40,
	/*! max number of clothes a player can put on */
	max_clothes_on			= 11,
	/*! max number of skills aplayer can take into battle */
	max_skills_bind			= 6,
	/*! the num of normal attack of player */
	normal_attack_num		= 6,
	/*! max number of skill a player can use at a battle */
	player_max_skill_num	= max_skills_bind + normal_attack_num,
	/*! max level of a player */
	player_max_level		= 100,
	/*! max task item */
	max_task_items			= 20,
	/*! max task item */
	max_task_monsters		= 10,
	/*! max task step */
	max_task_step			= 10,
	/*! the len of serv */
	serv_buf_len			= 20,
	/*! max drop item */
	max_drop_num			= 10,
	/*! max drop item */
	max_drop_lv				= 100,
	/*! compute drop odds */
	drop_odds_multiple		= 10000,
	/*! the compute ratio of clothes duration */
	clothes_duration_ratio  = 50,
	/*! max number of the stage */
	max_stage_num           = 1600,

	/*! type of stage difficulty:  1:easy  2:normal  3:hard  4:hellish*/
	max_stage_difficulty_type = 11,

	max_stage_grade			= 6,

	max_fumo_points_daily_got = 300,

	max_suit_id				= 500 ,

	max_suit_step			= 7,

	min_fight_value			= 10,

	max_summon_skills		= 10,

	min_hero_top_lv         = 51,

	max_fumo_top_floor		= 11,

#ifndef VERSION_KAIXIN
	/*! for stat log */
    stat_log_role_exp           = 0x09011000,
	stat_log_role_dead			= 0x09010400,
	stat_log_role_relive		= 0x09010403,
	stat_log_stage_login		= 0x09050000,
    stat_log_stage_login_2      = 0x0901040B,
	stat_log_stage_finish		= 0x090F0000,
	stat_log_stage_room_finish	= 0x09060000,
	stat_log_got_skillpoints 	= 0x09040000,
	stat_log_killed_boss        = 0x09070000,
	stat_log_killed_monster     = 0x09080000,
	stat_log_got_amber 	    	= 0x09090000,
    stat_log_stage_user_lv      = 0x09100000,
    stat_log_spe_stage_userlv_finish      = 0x09110000,
    stat_log_boss_drop          = 0x09110000,
    stat_log_stage_difficulty_login   = 0x09120000,
    stat_log_fumo_gain          = 0x09200100,
	stat_log_achieve_child_num	= 0x09500201,
	stat_log_boost_num			= 0x090C4000,
	stat_log_fumo_grade			= 0x09010008,
	stat_log_fumo_time_use		= 0x0901000d,
	stat_log_fumo_reward		= 0x0901000C,
	stat_log_fumo_enter			= 0x09150001,
	stat_log_fumo_enter_new		= 0x09154000,
	stat_log_fumo_finished		= 0x09150002,
	stat_log_fumo_dead			= 0x09151000,
	stat_log_fumo_relive		= 0x09152000,
	stat_log_fumo_pass_tm		= 0x09153000,

	stat_log_pvp_join			= 0x0901000f,
	stat_log_pvp_honor			= 0x09010010,

    stat_log_invite_pvp_role    = 0x09220000,
    stat_log_invite_pvp_times   = 0x09220112,
    stat_log_footprint_pvp_lv   = 0x09220200,
    stat_log_footprint_pvp_times= 0x09220312,
    stat_log_invite_pvp_lv      = 0x09221000,

	stat_log_lag_info			= 0x09501001,
	stat_log_load_lag			= 0x0950100C,
	stat_log_user_stage_login	= 0x09600000,
	stat_log_userlv_spe_stage_login	= 0x09610000,
	stat_log_pvp_exploit		= 0x09010013,
	stat_log_fumo_exploit		= 0x09010014 ,

	stat_log_pvp_lv1_honor			= 0x09010015,
	stat_log_pvp_lv2_honor			= 0x09010016,

	stat_log_team_btl_count			= 0x090F3001,
	stat_log_team_btl_time  		= 0x090F3011,
	stat_log_team_btl_injured		= 0x090F3021,
	stat_log_produce_coins		 = 0x09520000,
	stat_log_destroy_coins		 = 0x09520001,
	stat_log_enter_stage		 =  0x09522101,
	stat_log_peach_got			 =  0x09508001,

	stat_log_lingli_neihe		 =  0x09524001,

	stat_log_specal_stage		 =  0x09130000,
	stat_log_open_box            =  0x09627000,
	stat_log_open_box_monster    =  0x09623000,
#else
	/*! for stat log */
    stat_log_role_exp           = 0,
	stat_log_role_dead			= 0,
	stat_log_role_relive		= 0,
	stat_log_stage_login		= 0,
    stat_log_stage_login_2      = 0,
	stat_log_stage_finish		= 0,
	stat_log_stage_room_finish	= 0,
	stat_log_got_skillpoints 	= 0,
	stat_log_killed_boss        = 0,
	stat_log_killed_monster     = 0,
	stat_log_got_amber 	    	= 0,
    stat_log_stage_user_lv      = 0,
    stat_log_boss_drop          = 0,
    stat_log_fumo_gain          = 0,
	stat_log_achieve_child_num	= 0,
	stat_log_boost_num			= 0,
	stat_log_fumo_grade			= 0,
	stat_log_fumo_time_use		= 0,
	stat_log_fumo_reward		= 0,
	stat_log_fumo_enter			= 0,
	stat_log_fumo_enter_new		= 0,
	stat_log_fumo_finished		= 0,
	stat_log_fumo_dead			= 0,
	stat_log_fumo_relive		= 0,
	stat_log_fumo_pass_tm		= 0,

	stat_log_pvp_join			= 0,
	stat_log_pvp_honor			= 0,

    stat_log_invite_pvp_role    = 0,
    stat_log_invite_pvp_times   = 0,
    stat_log_footprint_pvp_lv   = 0,
    stat_log_footprint_pvp_times= 0,
    stat_log_invite_pvp_lv      = 0,

	stat_log_lag_info			= 0,
	stat_log_load_lag			= 0,
	stat_log_user_stage_login	= 0,
	stat_log_pvp_exploit		= 0,
	stat_log_fumo_exploit		= 0 ,

	stat_log_pvp_lv1_honor			= 0,
	stat_log_pvp_lv2_honor			= 0,

	stat_log_team_btl_count			= 0,
	stat_log_team_btl_time  		= 0,
	stat_log_team_btl_injured		= 0,
	stat_log_produce_coins		 = 0,
	stat_log_destroy_coins		 = 0,
	stat_log_enter_stage		 =  0,
	stat_log_peach_got			 =  0,

	stat_log_lingli_neihe		 =  0,	
	stat_log_enter_stage_kaixin 	= 0x0F510000,

	stat_log_userlv_spe_stage_login = 0,
	stat_log_stage_difficulty_login = 0,
	stat_log_spe_stage_userlv_finish = 0,
	stat_log_specal_stage			= 0,
	stat_log_open_box            =  0,
    stat_log_open_box_monster    =  0,	

#endif
};

/**
  * @brief type of a player set
  */
typedef std::set<Player*>	PlayerSet;
typedef std::vector<Player*>	PlayerVec;
/**
  * @brief hold timer list
  */
struct timer_head_t {
	list_head_t	timer_list;
};


struct mon_drop_time_limit_t
{
	uint32_t week_day;
	uint32_t from_date;
	uint32_t to_date;
	uint32_t from_date_sec;
	uint32_t to_date_sec;
	mon_drop_time_limit_t()
	{
		from_date = 0;
		to_date = 0;
		from_date_sec = 0;
		to_date_sec = 0;	
	}
};


class SpecialDropTimeInfo {
	public:
		void init(const char* xml_file);
		uint32_t id;
		uint32_t time_type;
		std::vector<mon_drop_time_limit_t> limit_info_arr;

	public:
		SpecialDropTimeInfo()
		{
			time_type = 0;
		}
};

class SpecialTimeMrg {
	public:
		bool is_in_time(uint32_t time_id, time_t tv_sec);
		
		void init();
		int load_special_time(xmlNodePtr cur);
	private:
		std::map<uint32_t, SpecialDropTimeInfo> time_limit_map;

	public:
		SpecialTimeMrg()
		{
			init();
		}
};


/*! hold common timers */
extern timer_head_t g_events;
/*! pathname where to store the statistic log info */
extern char* statistic_logfile;
/*! event manager */
extern taomee::EventMgr ev_mgr;

#endif // KF_FWD_DECL_HPP_

