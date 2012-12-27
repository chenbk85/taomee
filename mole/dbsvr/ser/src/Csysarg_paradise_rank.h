/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_paradise_rank.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/01/2011 09:44:50 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_PARADISE_RANK
#define CSYSARG_PARADISE_RANK

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"


class Csysarg_paradise_rank:public Ctable
{
	public:
		Csysarg_paradise_rank(mysql_interface *db);
		int insert(userid_t userid, uint32_t exp, uint32_t count);
		int select(userid_t userid, uint32_t count, sysarg_get_paradise_friend_rank_in_item 
				*p_in_item, uint32_t *p_out_count,sysarg_get_paradise_friend_rank_out_item **pp_out_item);
		int update(userid_t userid, uint32_t exp, uint32_t count);
		int update(userid_t userid, char *column, uint32_t value);
		int select(userid_t userid, char *column, uint32_t &value);
		int cal_level(uint32_t exp);
		int select(userid_t count,sysarg_get_paradise_access_rank_in_item *p_in_item, 
				uint32_t *p_out_count, sysarg_get_paradise_access_rank_out_item **pp_out_item);
};

#endif
