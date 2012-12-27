/*
 * =====================================================================================
 *
 *       Filename:  Cuser_halloween_candy.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2011 01:57:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_HALLOWEEN_CANDY_INC
#define CUSER_HALLOWEEN_CANDY_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 

class Cuser_halloween_candy:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_halloween_candy(mysql_interface *db);
		int insert(userid_t userid, swap_candy_t *swap_history);
		int update_friends_times(userid_t userid, uint32_t datetime, uint32_t times, swap_candy_t *friends);
		int get_friends(userid_t userid, swap_candy_t *friends);
		int get_friends_times(userid_t userid, swap_candy_t *friends, uint32_t *times);
		int update_date_times(userid_t userid, uint32_t datetime, uint32_t times);
		int get_friends_times(userid_t userid, uint32_t *datetime, uint32_t *times, swap_candy_t *friends);
};
#endif
