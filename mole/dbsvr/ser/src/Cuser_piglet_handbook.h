/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_handbook.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/19/2011 05:37:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PIGLET_HANDBOOK_INC
#define CUSER_PIGLET_HANDBOOK_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_piglet_handbook: public CtableRoute100x10{
	private:
		int insert(userid_t userid, uint32_t breed);
		int select(userid_t userid, uint32_t breed, uint32_t* count);
	public:
		Cuser_piglet_handbook(mysql_interface *db);
		int add(userid_t userid, uint32_t breed);
		int get_all(userid_t userid, user_piglet_get_handbook_out_item** pp_list, uint32_t *p_count);
		
};
#endif
