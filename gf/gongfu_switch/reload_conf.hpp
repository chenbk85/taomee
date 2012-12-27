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

#ifndef PAIPAI_RELOAD_CONF_HPP_
#define PAIPAI_RELOAD_CONF_HPP_

extern "C" {
#include <stdint.h>
}

/**
  * @brief minor cmd for reloading configs
  */
enum {
	reload_item_config_cmd			= 1,
	reload_home_config_cmd			= 2,
	reload_task_config_cmd			= 3,
	reload_game_config_cmd			= 4,
	reload_move_config_cmd			= 6,
	reload_monster_config_cmd		= 7,
	reload_restriction_config_cmd	= 8,
	reload_bench_config_cmd			= 9,
};

/**
  * @brief reload configs
  */
void reload_conf_op(uint32_t cmd, const uint8_t* data, uint32_t datalen);

/**
  * @brief reload bench.conf configs
  */
void reload_bench_conf(void);

#endif // PAIPAI_RELOAD_CONF_HPP_

