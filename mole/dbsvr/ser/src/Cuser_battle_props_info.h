/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_props_info.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/20/2011 10:02:43 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_PROPS_INFO_INC
#define CUSER_BATTLE_PROPS_INFO_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_battle_props_info:public CtableRoute100x10
{
	private:

	public:
		Cuser_battle_props_info(mysql_interface *db);
		int insert(userid_t userid, uint32_t type, uint32_t flag, uint32_t round);
		int get_all(userid_t userid, props_info_t **pp_list, uint32_t *p_count);
		int get_flag(userid_t userid, props_gain_t **pp_list, uint32_t *p_count);
		int select(userid_t userid, uint32_t type , uint32_t flag, uint32_t *count);
		int update_inc(userid_t userid, uint32_t type, uint32_t flag, uint32_t count);
		int update(userid_t userid, uint32_t type, uint32_t flag, uint32_t count);
		int sub_count(userid_t userid, uint32_t flag, uint32_t count);
};

#endif
