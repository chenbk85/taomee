/*
 * =====================================================================================
 *
 *       Filename:  Cuser_dragon_expolre.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/26/2011 03:24:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_DRAGON_EXPLORE_INCL
#define CUSER_DRAGON_EXPLORE_INCL


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_dragon_explore:public CtableRoute100x10{
	public:
		Cuser_dragon_explore(mysql_interface *db);
		int insert(userid_t userid, uint32_t contribute, uint32_t state);
		int query(userid_t userid, uint32_t *contribute, uint32_t *state);
		int update_all(userid_t userid, uint32_t contribute, uint32_t state);
		
		int update_inc_col(userid_t userid, const char *col, uint32_t value);

		int update(userid_t userid, const char* col, uint32_t value);
};
#endif
