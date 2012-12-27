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

//#include "Player_new.hpp"
//#include "Home.hpp"
//#include "OnlineTask.hpp"
/**
  * @brief compare methods
  */
enum cmp_t {
	/*! len must equal to another given len */
	cmp_must_eq		= 1,
	/*! len must be greater or equal to another given len */
	cmp_must_ge		= 2
};

struct player_t;
class OnlineManage;
class HomeManage;
class GlobalPlayer;

typedef uint64_t map_id_t;
typedef userid_t home_id_t;
/**
  * @brief common constants
  */
enum common_constant {
	/*! size in bytes of a nick name */
	max_nick_size	= 16,

	/*! max number of clothes a player can put on */
	max_clothes_on	= 10,
};

enum logout_reason {
	normal_logout = 1,
	disconnect,
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

extern OnlineManage* g_online;
extern HomeManage* g_home;
extern GlobalPlayer* g_player;
#endif // KF_FWD_DECL_HPP_

