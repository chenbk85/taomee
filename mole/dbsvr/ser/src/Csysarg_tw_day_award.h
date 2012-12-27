/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_tw_day_award.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/17/12 18:16:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_TW_DAY_AWARD_INCL
#define CSYSARG_TW_DAY_AWARD_INCL


#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_tw_day_award:Ctable
{
	public:
		Csysarg_tw_day_award(mysql_interface *db);
		int insert(userid_t userid, uint32_t date);
		int get_count(userid_t userid, uint32_t &count, uint32_t day);
};
#endif
