/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_login_gifts.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/10/2011 02:35:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CROOMINFO_LOGIN_GIFTS
#define CROOMINFO_LOGIN_FIFTS

#include "proto.h"
#include "benchapi.h"
#include "CtableRoute10x10.h"

class Croominfo_login_gifts : public CtableRoute10x10
{
	public:
		Croominfo_login_gifts(mysql_interface *db);
		int insert(userid_t userid, uint32_t count, uint32_t total, uint32_t round);
		int update(userid_t userid, roominfo_query_login_gifts_out *p_out);
		int query(userid_t userid, roominfo_query_login_gifts_out  *p_out);

};

#endif
