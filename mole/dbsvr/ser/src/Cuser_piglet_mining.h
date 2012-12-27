/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_mining.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/2012 04:44:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PIGLET_MINING_INC
#define CUSER_PIGLET_MINING_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_piglet_mining:public CtableRoute100x10
{
	private:

	public:
		Cuser_piglet_mining(mysql_interface *db);
		int add(userid_t userid, uint32_t mapid, uint32_t pigletid, uint32_t mining_end_time);
		int drop(userid_t userid, uint32_t in_count, user_finish_some_map_mining_in_item *p_in_list);
		int get_all(userid_t userid, uint32_t* p_count, user_get_piglet_mining_info_out_item **pp_list);
		int get_map_exist(userid_t userid, uint32_t mapid, uint32_t *cnt);
		int get_one(userid_t userid, uint32_t mapid,const char* col, uint32_t *data);
		int drop_one(userid_t userid, uint32_t mapid);
};
#endif
