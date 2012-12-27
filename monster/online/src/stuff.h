/*
 * =====================================================================================
 *
 *       Filename:  stuff.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年03月08日 18时13分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_STUFF_H_20120308
#define H_STUFF_H_20120308

#include <stdint.h>

#include "data_structure.h"

#include "user_manager.h"

int process_buy_stuff_after_get_restrict(usr_info_t *p_user, commit_stuff_cache_t *p_cm_stuff,  uint32_t day_restrict);
int update_plant_info_after_buy_stuff(usr_info_t *p_user, commit_stuff_cache_t *p_sf_cache);
int unlock_map_item(usr_info_t *p_user, uint16_t mon_level);


#endif //H_STUFF_H_20120308
