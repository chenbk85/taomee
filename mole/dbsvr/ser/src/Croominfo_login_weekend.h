/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_login_weekend.h
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

#ifndef CROOMINFO_LOGIN_WEEKEND
#define CROOMINFO_LOGIN_WEEKEND

#include "proto.h"
#include "benchapi.h"
#include "CtableRoute10x10.h"

struct date_week_info {
	uint32_t date;
	uint32_t week;
}__attribute__((packed));


class Croominfo_login_weekend : public CtableRoute10x10
{
	public:
		Croominfo_login_weekend(mysql_interface *db);
		int insert(userid_t userid, uint32_t week_count, uint32_t last_week);
		int update(userid_t userid, uint32_t week_count, uint32_t last_week );
		int query(userid_t userid, uint32_t* week_count, uint32_t* last_week);
		int update_week_count(userid_t userid);
};

#endif
