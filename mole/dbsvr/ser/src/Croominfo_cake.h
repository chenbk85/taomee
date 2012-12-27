/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_cake.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/27/2010 02:14:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CROOMINFO_CAKE_INCL
#define CROOMINFO_CAKE_INCL
#include "proto.h"
#include "benchapi.h"
#include "CtableRoute10x10.h"

class Croominfo_cake : public CtableRoute10x10
{
	public:
		Croominfo_cake(mysql_interface *db);
		int insert(userid_t userid, uint32_t score);
		int update(userid_t userid, uint32_t score);
		int increase(userid_t userid, uint32_t score);
		int query_score(userid_t userid, uint32_t *score);

};
#endif
