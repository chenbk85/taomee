/*
 * =====================================================================================
 *
 *       Filename:  Cuser_wish_wall.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/09/2012 03:39:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_WISH_WALL_INCL
#define CUSER_WISH_WALL_INCL


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 


class Cuser_wish_wall:public CtableRoute100x10
{
	public:
		Cuser_wish_wall(mysql_interface *db);
		int insert(userid_t userid, char* sp_1, char* sp_2, char* sp_3);
		int get_all(userid_t userid, user_get_wish_wall_out_item** pp_list, uint32_t *p_count);
		int get_count(userid_t userid, uint32_t *count);
};
#endif
