/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_ocean_info.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/2012 07:18:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#ifndef CSYSARG_OCEAN_INFO_INC
#define CSYSARG_OCEAN_INFO_INC


#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_ocean_info:public Ctable
{
	public:
		Csysarg_ocean_info(mysql_interface *db);
		
		int add(userid_t userid, uint32_t exp);
		int select_ocean_friend(userid_t userid, uint32_t in_count, sysarg_get_ocean_friend_in_item *p_in_list, 
				uint32_t *out_count, sysarg_get_ocean_friend_out_item **pp_out_list);

		uint32_t get_level(uint32_t exp);
	private:
		int insert(userid_t userid, uint32_t exp);
		int update(userid_t userid, uint32_t exp);

};

#endif
