/*
 * =====================================================================================
 *
 *       Filename:  Cuser_self_games.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/2012 05:15:37 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_SELF_GAMES_INCL
#define CUSER_SELF_GAMES_INCL


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_self_games:public CtableRoute100x10
{
	private:

		int insert(userid_t userid, uint32_t gameid, uint32_t exp);
		int update(userid_t userid, uint32_t gameid, uint32_t exp);
	public:
		Cuser_self_games(mysql_interface *db);
		int add(userid_t userid, uint32_t gameid, uint32_t exp);
		int get_all(userid_t userid, user_get_games_info_out_item **pp_list, uint32_t* p_count);
		int get_score_by_gameid(userid_t userid, uint32_t gameid, uint32_t *score);
		int cal_exp(userid_t userid, uint32_t gameid, uint32_t score, uint32_t *exp);
};

#endif
