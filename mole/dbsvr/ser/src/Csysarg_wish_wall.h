/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_wish_wall.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/09/2012 02:31:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_WISH_WALL_INCL
#define CSYSARG_WISH_WALL_INCL


#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_wish_wall: Ctable
{
	public:
		Csysarg_wish_wall(mysql_interface *db);
		int add(uint32_t userid, char* nick);
		int get_some_userid(userid_t userid, user_sysarg_get_wish_wall_in *p_in, 
				user_sysarg_get_wish_wall_out_item** pp_list, uint32_t *p_count);

		int get_total_page(uint32_t *page_total);
	private:
		int insert(userid_t userid, char* nick);
		int update_old(userid_t userid);
		int update_other(userid_t userid, char* nick);
		int get_rd_count(uint32_t *count);
};
#endif
