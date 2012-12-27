/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_treasure_collection.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2011 04:17:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_TREASURE_INC
#define CSYSARG_TREASURE_INC


#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_treasure_collection:public Ctable
{
	public:
		Csysarg_treasure_collection(mysql_interface *db);
		int insert(userid_t userid, uint32_t exp, uint32_t piglet_level);
		int update(userid_t userid, uint32_t exp);
		int select_visit(uint32_t in_count, sysarg_get_treasure_visit_in_item *p_in_item, 
				uint32_t *out_count, sysarg_get_treasure_visit_out_item **pp_out_item);
		int get_level(uint32_t exp, uint32_t &level);
		int select_friend(userid_t userid, uint32_t in_count, sysarg_get_treasure_friend_in_item *p_in_list, 
				uint32_t *out_count, sysarg_get_treasure_friend_out_item **pp_out_list);


		int update_piglet_level(userid_t userid, uint32_t level);
		int select_piglet_house_friend(userid_t userid, uint32_t in_count, sysarg_get_treasure_friend_in_item *p_in_list, 
				uint32_t *out_count, sysarg_get_treasure_friend_out_item **pp_out_list);

};

#endif
