/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_footprint.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/28/2010 04:34:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CROOMINFO_FOOTPRINT
#define CROOMINFO_FOOTPRINT

#include "proto.h"
#include "benchapi.h"
#include "CtableRoute10x10.h"

class Croominfo_footprint: public CtableRoute10x10
{
	public:
		Croominfo_footprint(mysql_interface *db);
		int insert(userid_t userid, uint32_t count);
		int update_inc(userid_t userid, uint32_t count);
		int update(userid_t userid, uint32_t count);
		int get_count(userid_t userid, roominfo_get_footprint_count_out &curr);
		int get_curr_count(userid_t userid, uint32_t &curr);
};
#endif
