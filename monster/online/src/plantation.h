/*
 * =====================================================================================
 *
 *       Filename:  plantation.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年03月08日 10时44分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_PLANTATION_H_20120308
#define H_PLANTATION_H_20120308

#include <stdint.h>

#include "data_structure.h"

#include "user_manager.h"

int update_plant_growth(usr_info_t *p_user, uint32_t mon_happy, all_hole_info_t *p_all_plant, db_msg_update_plant_req_t *p_change, uint8_t enter_plantation);
int update_plant_info_in_cache(usr_info_t *p_user, db_msg_update_plant_req_t *p_change);
int update_plant_info_after_commit_puzzle_return(usr_info_t *p_user, puzzle_cache_t *p_puzzle_cache);
int update_plant_info_after_eat_food(usr_info_t *p_user, uint32_t old_happy, db_msg_update_plant_req_t *p_change);

int get_loop_maintain(uint8_t maintain_count);
int get_new_maintain();
int all_plant_maturity(all_hole_info_t *p_all_hole);
int pet_match(all_hole_info_t *p_all_hole, uint32_t *pet_id, int *p_seed_match);
int process_plantation_after_all_hole(usr_info_t *p_user, all_hole_info_t *p_all_hole);
int process_sun_reward_exp(usr_info_t *p_user, sun_reward_exp_cache_t *p_cache, uint32_t day_restrict);

#endif //H_PLANTATION_H_20120308
