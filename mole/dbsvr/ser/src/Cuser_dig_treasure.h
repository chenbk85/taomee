/*
 * =====================================================================================
 *
 *       Filename:  Cuser_dig_treasure.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/18/2011 03:09:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_DIG_TREASURE_H
#define CUSER_DIG_TREASURE_H
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_dig_treasure:public CtableRoute100x10
{           
    private: 
		int get_event_happen_date (userid_t user_id, uint32_t map_id, uint32_t event_id, uint32_t *date);
    public:
        Cuser_dig_treasure(mysql_interface *db);
        int get_info_game_begin(userid_t userid, uint32_t map_id, uint32_t event_id, uint32_t *count, uint32_t *date);
		int add_user_dig_map(userid_t user_id, uint32_t map_id, uint32_t event_id, uint32_t count);
		int update_map_entern_count (userid_t user_id, uint32_t map_id, uint32_t event_id, uint32_t count);
		int get_random_event_counts(userid_t user_id, uint32_t map_id, user_get_dig_treasure_begin_out_item **pp_list,
									uint32_t *count);
};
    
#endif 
