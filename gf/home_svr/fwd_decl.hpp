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

#ifndef KFTRD_FWD_DECL_HPP_
#define KFTRD_FWD_DECL_HPP_

#include <ctime>
#include <set>

extern "C" {
#include <glib.h>

#include <libtaomee/list.h>
#include <libtaomee/project/types.h>
#include <libtaomee/dirtyword/dirtyword.h>
#include <libtaomee/project/stat_agent/msglog.h>
#include <async_serv/net_if.h>
#include <libtaomee/timer.h>
}
#include <libtaomee++/time/time.hpp>
#include <libtaomee++/event/event_mgr.hpp>


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
class Player;

/**
  * @brief common constants
  */
enum common_constant {
	/*! size in bytes of a nick name */
	max_nick_size			= 16,
	/*! max lv can get exp */
	max_exp_lv		= 55,
	/*! size of once_bit size */
	max_once_bit_size = 40,
	/*! max number of clothes a player can put on */
	max_clothes_on			= 10,
	/*! max number of skills aplayer can take into battle */
	max_skills_bind			= 6,
	/*! the num of normal attack of player */
	normal_attack_num		= 5,
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
	max_stage_difficulty_type = 4,

	max_stage_grade			= 6,

	max_fumo_points_daily_got = 300,

	max_suit_id				= 400,

	max_suit_step			= 7,

	min_fight_value			= 10,

	max_summon_num		= 50, 

	max_home_player     = 10,



	max_clothes_num         = 10,
	max_decoration_num      = 10,


	//stat log msg id
	stat_log_shop_last_time 	= 0x09020A01,
	stat_log_setup_shop_times 	= 0x09020A02,
	stat_log_shop_exchange_times= 0x09020A03,
	stat_log_shop_daily_exchange_times = 0x09020A04,
	stat_log_shop_daily_exchange_coins = 0x09020A05,
};


/**
  * @brief hold timer list
  */
struct timer_head_t {
	list_head_t	timer_list;
};

/*! hold common timers */
extern timer_head_t g_events;

/*! pathname where to store the statistic log info */
extern char* statistic_logfile;
/*! event manager */
extern taomee::EventMgr ev_mgr;



//stat log func
inline void do_stat_log(uint32_t msgid, uint32_t num)
{
	msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, &num, sizeof(num));
}
inline void do_stat_log2(uint32_t msgid, uint32_t id, uint32_t num)
{
	uint32_t tmp[2] = { id, num };
	msglog(statistic_logfile, msgid, get_now_tv()->tv_sec, tmp, sizeof(tmp));
}

#endif // KF_FWD_DECL_HPP_

