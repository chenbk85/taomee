/**
 *============================================================
 *  @file      reload_conf.hpp
 *  @brief    reload some given configs at receiving of some given multi-cast packages
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_RELOAD_CONF_HPP_
#define KF_RELOAD_CONF_HPP_

extern "C" {
#include <stdint.h>
}

/**
  * @brief minor cmd for reloading configs
  */
enum {
	reload_item_config_cmd			= 1,
	reload_battle_config_cmd    = 2,
	reload_daily_activity_cmd   = 3,
	reload_task_cmd   = 4,
	reload_active_stage_cmd = 5,
	reload_swap_action_cmd	= 6,
	reload_active_data_cmd = 7,
};

/**
  * @brief reload configs
  */
void reload_conf_op(uint32_t cmd, const uint8_t* data, uint32_t datalen);

/**
  * @brief reload bench.conf configs
  */
void reload_bench_conf(void);

/**
  * @brief reload items configs
  */
inline void reload_items();

/**
 * @brief reload dailyactivity.xml
 */
inline void reload_daily_activity();

/**
 * @brief reload tasks.xml
 */
inline void reload_tasks();

#endif //KF_RELOAD_CONF_HPP_
