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

#ifndef KFBTL_RELOAD_CONF_HPP_
#define KFBTL_RELOAD_CONF_HPP_

extern "C" {
#include <stdint.h>
}

/**
  * @brief minor cmd for reloading configs
  */
enum {
	reload_skills_cmd = 1,
	reload_monst_cmd,
	reload_stages_cmd,
	reload_quality_cmd,
	reload_items_cmd,
};

/**
  * @brief reload configs
  */
void reload_conf_op(uint32_t cmd, const uint8_t* data, uint32_t datalen);


#endif //KFBTL_RELOAD_CONF_HPP_

