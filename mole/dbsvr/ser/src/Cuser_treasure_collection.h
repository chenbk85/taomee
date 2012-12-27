/*
 * =====================================================================================
 *
 *       Filename:  Cuser_treasure_collection.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/09/2011 11:24:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_TREASURE_COLLECTION_INC
#define CUSER_TREASURE_COLLECTION_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_treasure_collection:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_treasure_collection(mysql_interface *db);
		int insert(userid_t userid, uint32_t login_first = 0);
		int update_background(userid_t userid, uint32_t background);
		int get_background(userid_t userid, uint32_t *background);
		/*  int update_visitlist(userid_t userid, )*/
		int get_visitlist(userid_t userid, user_treasure_collection_get_visitlist_out *visitlist);
		int update_visitlist(userid_t userid,user_treasure_collection_get_visitlist_out &visitlist );
		int get_grid_count(uint32_t level, uint32_t &grid_limit);
		int update_first_login(userid_t userid, uint32_t first);
		int get_first_login(userid_t userid, uint32_t &first);
		int get_first_mod(userid_t userid, uint32_t *first_mod);
		int update_first_mod(userid_t userid, uint32_t first_mod);
};
#endif
