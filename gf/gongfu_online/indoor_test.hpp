/**
 *============================================================
 *  @file     indoor_test.hpp
 *  @brief    message related functions are declared here. (talk_cmd, ...)
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef __KF_INDOOR_TEST__
#define __KF_INDOOR_TEST__

#include "fwd_decl.hpp"

/**
  * @brief player change game battle value
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int change_game_battle_value_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

#endif // __KF_INDOOR_TEST__
