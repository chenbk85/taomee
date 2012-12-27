/*
 * =====================================================================================
 *
 *       Filename:  Cuser_ocean_handbook.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/2012 06:56:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_OCEAN_HANDBOOK_INC
#define CUSER_OCEAN_HANDBOOK_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_ocean_handbook: public CtableRoute100x10{
	private:

	public:
		Cuser_ocean_handbook(mysql_interface *db);
		int get_all(userid_t userid, user_get_ocean_handbook_out_item** pp_list, uint32_t *p_count);
		int insert(userid_t userid, uint32_t fish);
		
};
#endif
