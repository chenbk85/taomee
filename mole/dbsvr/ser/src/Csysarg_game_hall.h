/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_game_hall.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/2012 05:01:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_GAME_HALL_INCL
#define CSYSARG_GAME_HALL_INCL

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_game_hall:Ctable
{
	public:
		Csysarg_game_hall(mysql_interface *db);

		int insert(uint32_t userid, char* nick, uint32_t exp, uint32_t gameid, uint32_t score, uint32_t datetime);
		int update(uint32_t userid, uint32_t exp, uint32_t datetime);
		int update_two_col(userid_t userid, const char* col_1, const char* col_2, uint32_t value_1, uint32_t value_2);
		int update_one_col(userid_t userid, const char* col, uint32_t value);
		int change_value(userid_t userid, uint32_t exp, uint32_t gameid, uint32_t score, uint32_t datetime);
		
		int get_recent_ten(sysarg_get_game_top_ten_out_item **pp_list, uint32_t* p_count);
		int change_hall_data(userid_t userid, sysarg_game_hall_change_in *p_in);
		int get_self_exp(userid_t userid, uint32_t *exp);
		int get_self_rank(userid_t userid, uint32_t *rank);
	private:
};
#endif
