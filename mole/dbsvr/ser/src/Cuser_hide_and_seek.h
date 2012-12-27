/*
 * =====================================================================================
 *
 *       Filename:  Cuser_hide_and_seek.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/02/2011 04:26:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_HIDE_AND_SEEK_INC
#define CUSER_HIDE_AND_SEEK_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_hide_and_seek:public CtableRoute100x10
{
	private:

	public:
		Cuser_hide_and_seek(mysql_interface *db);
		int insert(userid_t userid, uint32_t badge_cnt, uint32_t role);
		int update(userid_t userid, uint32_t count,uint32_t role);
		int select(userid_t userid, uint32_t *count, uint32_t *date , uint32_t role);
		int update_date_cnt(userid_t userid, uint32_t count, uint32_t today, uint32_t role);
};

#endif
