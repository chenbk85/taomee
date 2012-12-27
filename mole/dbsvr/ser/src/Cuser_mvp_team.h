/*
 * =====================================================================================
 *
 *       Filename:  Cuser_mvp_team.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/20/2012 10:59:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSRR_MVP_TEAM_INCL
#define CUSRR_MVP_TEAM_INCL


#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_mvp_team: public CtableRoute100x10
{
	private:
	
	public:
		 Cuser_mvp_team(mysql_interface *db);
		int add(userid_t captain, uint32_t member);
		int get_count(uint32_t captain, uint32_t* count);
		int remove(userid_t captain, uint32_t memberid);
		int destory_team(userid_t captain);
		int get_memberlist(userid_t captain, user_mvp_deleteall_out_item **memberlist, uint32_t* count);
		int get_memberlist(userid_t captain, user_get_teaminfo_out_item **memberlist, uint32_t* count);
		/* int get_all(uint32_t captain, uint32_t* p_count, )*/
};
#endif
