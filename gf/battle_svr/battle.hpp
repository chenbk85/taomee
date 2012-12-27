/**
 *============================================================
 *  @file      battle.hpp
 *  @brief    battle related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_BATTLE_HPP_
#define KFBTL_BATTLE_HPP_

#include <set>

#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/log.h>

#include <libtaomee/project/constants.h>
#include <libtaomee/project/types.h>

#include <async_serv/dll.h>
}

#include "fwd_decl.hpp"
#include "player.hpp"
#include "battle_impl.hpp"

//-------------------------------------------------------------------------------


int player_btl_restart_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief sync_process
 * @return 0 on success, -1 on error
 */
int sync_process_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief cancel a battle
 * @return 0 on success, -1 on error
 */
int cancel_battle_cmd(Player* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief Attend a random battle, to battle with a randomly chosen player
  */
int lv_matching_battle_cmd(Player* p, uint8_t* body, uint32_t bodylen);

int tmp_team_pvp_cmd(Player* p, uint8_t* body, uint32_t bodylen);

int pvp_matching_battle_cmd(Player * p, uint8_t * body, uint32_t bodylen);

int pvp_invite_battle_cmd(Player *p, uint8_t * body, uint32_t bodylen);

int pve_matching_battle_cmd(Player * p, uint8_t * body, uint32_t bodylen);

int pve_invaite_battle_cmd(Player * p, uint8_t * body, uint32_t bodylen);
/**
  * @brief Attend a pve battle
  */
int pve_battle_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief a player informs server that he/she is ready to fight
  * @return 0 on success, -1 on error
  */
int player_ready_cmd(Player* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player move in a battle
 * @return 0 on success, -1 on error
 */
int player_move_cmd(Player* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player jumps in a battle
 * @return 0 on success, -1 on error
 */
int player_jump_cmd(Player* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player stop moving in a battle
 * @return 0 on success, -1 on error
 */
int player_stop_cmd(Player* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief responds a lag testing
 * @return 0 on success, -1 on error
 */
int player_rsp_lag_cmd(Player* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player attacks in a battle
 */
int player_attack_cmd(Player* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief summon attacks unique skill
 */
int summon_unique_skill_cmd(Player* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player uses a item in a battle
 */
int player_use_item_cmd(Player* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player pick a item in a battle
 */
int player_pick_item_cmd(Player* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief player enters a map in a battle
 */
int player_enter_map_cmd(Player* p, uint8_t* body, uint32_t bodylen);

//-----battle for group
/**
* @brief create a battle team
*/
int create_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief join a battle team
*/
int join_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief hot join a battle team
*/
int hot_join_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief set ready in team
*/
int ready_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief cancel ready in team
*/
int cancel_ready_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief set a battle team hot join flag
  */
int set_btl_team_hot_join_cmd(Player* p, uint8_t* body, uint32_t bodylen);


/**
  * @brief the team leader kick user from game room
  */
int kick_user_from_room_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief start a battle team
*/
int start_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/**
* @brief leave a battle team
*/
int leave_battle_team_cmd(Player* p, uint8_t* body, uint32_t bodylen);

//for test
int get_player_attr_cmd(Player* p, uint8_t* body, uint32_t bodylen);

int get_mon_attr_cmd(Player* p, uint8_t* body, uint32_t bodylen);

/** 
 * @brief  玩家更改机关状态
 * 
 */
int change_mechanism_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief  玩家机关状态更改完毕
 * 
 */
int change_mechanism_end_cmd(Player *p, uint8_t * body, uint32_t bodylen);

int contest_watch_yazhu_cmd(Player * p, uint8_t * body, uint32_t bodylen);

int contest_watch_out_cmd(Player * p, uint8_t * body, uint32_t bodylen);


int summon_ai_change_cmd(Player * p, uint8_t * body, uint32_t bodylen);

int player_talk_cmd(Player * p, uint8_t * body, uint32_t bodylen);

// db callback
/**
 * @brief get some btl info from db
 */
//int db_pve_get_player_btl_info_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);


#endif

