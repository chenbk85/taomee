/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_random_gain_gift.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/29/2011 02:29:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CROOMINFO_RANDOM_GAIN_GIFT_INC
#define CROOMINFO_RANDOM_GAIN_GIFT_INC


#include "proto.h"
#include "benchapi.h"
#include "CtableRoute10x10.h"

class Croominfo_random_gain_gift:public CtableRoute10x10
{
	public:
		Croominfo_random_gain_gift(mysql_interface *db);
		int get_times(userid_t userid, uint32_t type, uint32_t *times);
		int add_times(userid_t userid, uint32_t type, uint32_t times);
		int update_times(userid_t userid, uint32_t type, uint32_t times);
		int insert(userid_t userid, uint32_t type, uint32_t times);

};
#endif
