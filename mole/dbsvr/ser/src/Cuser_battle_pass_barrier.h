/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_pass_barrier.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/24/2011 01:13:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_PASS_BARRIER_INC
#define CUSER_BATTLE_PASS_BARRIER_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_battle_pass_barrier:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_battle_pass_barrier(mysql_interface *db);
		int insert(userid_t userid, uint32_t barrierid, uint32_t count);
		int get(userid_t userid, uint32_t barrierid, uint32_t *count);
		int update(userid_t userid, uint32_t barrierid, uint32_t count);
		int get_barrier_list(userid_t userid,  user_battle_map_info_in_item *p_in_item, 
				uint32_t in_count,  user_battle_map_info_out_item **p_out_item, uint32_t *out_count);
		int update_inc(userid_t userid, uint32_t barrierid, uint32_t count);
};
#endif
