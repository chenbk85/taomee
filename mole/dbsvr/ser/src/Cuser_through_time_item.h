/*
 * =====================================================================================
 *
 *       Filename:  Cuser_through_time_item.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/22/2011 03:49:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_THROHGH_SPACE_ITEM_INC
#define CUSER_THROHGH_SPACE_ITEM_INC


#include "proto.h"
#include "benchapi.h"
#include "CtableRoute100x10.h"


class Cuser_through_time_item:public CtableRoute100x10
{
	public:
		Cuser_through_time_item(mysql_interface *db);
		int insert(userid_t userid, uint32_t itemid);
		int get_items(userid_t userid, roominfo_thr_tim_item_t **pp_list, uint32_t *p_count);
		int get_non_dig_icnt(userid_t userid, uint32_t *count);
		int remove(userid_t userid, uint32_t itemid);
	
};
#endif
