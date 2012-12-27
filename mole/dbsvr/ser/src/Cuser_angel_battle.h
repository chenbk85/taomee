/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angel_battle.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/07/2011 07:12:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_ANGEL_BATTLE_INC
#define CUSER_ANGEL_BATTLE_INC 

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_angel_battle:public CtableRoute100x10
{
	public:
		Cuser_angel_battle(mysql_interface *db);
		int insert(userid_t userid, uint32_t exp, uint32_t vic_count,
			   	uint32_t day_count, uint32_t day_exp, uint32_t day_prize);
		int update_inc_info(userid_t userid, uint32_t inc_exp, 
				uint32_t inc_vic_count, uint32_t play_count, uint32_t day_prize);
		int select_statistic_columns(userid_t userid, uint32_t *victory, uint32_t *exp);
		int select_check_columns(userid_t userid, uint32_t &day_count, uint32_t &day_exp, 
				uint32_t &day_prize, uint32_t &date);
		int update_clear_day(userid_t userid, uint32_t inc_exp, 
				uint32_t inc_vic_count, uint32_t play_count, uint32_t day_prize);
		int get_exp(userid_t userid,  uint32_t *out);
		int cal_level(uint32_t exp, uint32_t *level);
};

#endif
