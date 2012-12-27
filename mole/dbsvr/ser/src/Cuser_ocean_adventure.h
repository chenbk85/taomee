/*
 * =====================================================================================
 *
 *       Filename:  Cuser_ocean_adventure.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/11/2012 11:18:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_OCEAN_ADVENTURE_INC
#define CUSER_OCEAN_ADVENTURE_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_ocean_adventure:public CtableRoute100x10
{
	private:

	public:
		Cuser_ocean_adventure(mysql_interface *db);
		int add(userid_t userid, uint32_t prime, uint32_t sencondary);

		int get(userid_t userid, uint32_t *p_count, user_get_mole_adventure_pass_out_item_1** pp_list);
		int select_max(userid_t userid, uint32_t &prime, uint32_t &secondary);
		int clear(userid_t userid, uint32_t prime);
		int get_secondary(userid_t userid, uint32_t prime, uint32_t** pp_list, uint32_t* p_count);

};
#endif
