/*
 * =====================================================================================
 *
 *       Filename:  Cuser_strike_benguin.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/23/2011 11:01:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_STRIKE_PENGUIN_INC
#define CUSER_STRIKE_PENGUIN_INC

#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"


class Croominfo_strike_penguin:public CtableRoute10x10
{
	public:
		Croominfo_strike_penguin(mysql_interface *db);
		int insert(userid_t userid, uint32_t barrier, uint32_t score, uint32_t is_award,
				uint32_t is_pass);
		int update_score(userid_t userid, uint32_t barrier, uint32_t score);
		int update_date_award(userid_t userid, uint32_t barrier, uint32_t date, uint32_t award_flag);
		int select_history(userid_t userid, roominfo_strike_penguin_get_info_out_item 
				**pp_list, uint32_t *p_count);
		int get_max_pass_berrier(userid_t userid, uint32_t *max_pass_tollgate);
		int update_score_date(userid_t userid, uint32_t barrier, uint32_t score, uint32_t date);
		int select(userid_t userid, uint32_t barrier, uint32_t * score, uint32_t *date,
				uint32_t *pass_flag, uint32_t *award_flag);
		int update_ispass(userid_t userid, uint32_t barrier, uint32_t pass_flag);
};

#endif
