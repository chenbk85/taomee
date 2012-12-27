/*
 * =====================================================================================
 *
 *       Filename:  Cuser_response_friend.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/18/2011 04:31:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER__RESPONSE_FRIEND_INC
#define CUSER__RESPONSE_FRIEND_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_response_friend:public CtableRoute100x10
{
	private:

	public:
		Cuser_response_friend(mysql_interface *db);
		int insert(userid_t userid, uint32_t pre_friend);
		int drop(userid_t userid, uint32_t pre_friend);
		int select(userid_t userid, uint32_t pre_friend, uint32_t*datetime);
		int get_oldest(userid_t userid, uint32_t *uid);
		int get_count(userid_t userid, uint32_t *count);
		int add(userid_t userid, uint32_t pre_friend);
};
#endif
