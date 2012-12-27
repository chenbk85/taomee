/*
 * =====================================================================================
 *
 *       Filename:  Cuser_dig_treasure_info.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/18/2011 03:10:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#ifndef CUSER_DIG_TREASURE_INFO_H
#define CUSER_DIG_TREASURE_INFO_H 
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_dig_treasure_info:public CtableRoute100x10
{                                                 
    private:                                      

    public:
        Cuser_dig_treasure_info(mysql_interface *db);
        int get_info_game_begin(userid_t userid, uint32_t *health, uint32_t *exp, 
				uint32_t *lasttime, uint32_t *dig_counts);
		int get_hp (userid_t user_id, uint32_t &health);
        int init_user(userid_t user_id);
		int update(userid_t user_id, uint32_t hp, uint32_t exp, uint32_t tim, uint32_t dig_counts);
		int update_hp(userid_t user_id, uint32_t value, uint32_t tim);
		int get_hp_boundary(uint32_t level, uint32_t &hp_boundary);
		int get_level(uint32_t exp, uint32_t &level);
		int get_exp (userid_t user_id, uint32_t &exp);
		int update_dig_counts(userid_t userid, uint32_t dig_counts);
		int get_lasttime (userid_t user_id, uint32_t &tim);
};      
#endif
