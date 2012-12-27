/*
 * =====================================================================================
 *
 *       Filename:  Cuser_treasure_show.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/09/2011 10:41:48 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_TREASURE_SHOW_INC
#define CUSER_TREASURE_SHOW_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 


class Cuser_treasure_show:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_treasure_show(mysql_interface *db);
		int insert(userid_t userid, uint32_t id, uint32_t pos, uint32_t status);
		int get_all_show(userid_t userid, user_get_treasure_collection_out_item **pp_list, uint32_t *count);
		int del(userid_t userid, uint32_t pos);
		int update(userid_t userid, uint32_t id, uint32_t pos, uint32_t status);
		int update_show_status(userid_t userid, uint32_t pos, uint32_t status);
		int get_pos_show(userid_t userid, uint32_t pos, uint32_t *itemid, uint32_t *status);
		int get_show_count(userid_t userid, uint32_t itemid, uint32_t &show_count);
};
#endif
