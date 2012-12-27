/*
 * =====================================================================================
 *
 *       Filename:  Cuser_christmas_socks.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/12/2011 10:56:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_CHRISTMAS_SOCKS_INCL
#define CUSER_CHRISTMAS_SOCKS_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_christmas_socks:public CtableRoute100x10
{
	private:
		int insert(userid_t userid, uint32_t friendid, uint32_t type, uint32_t count);
		int update(userid_t userid, uint32_t firendid, uint32_t type, uint32_t count);
	public:
		Cuser_christmas_socks(mysql_interface *db);
		int add(userid_t userid, uint32_t friendid, uint32_t type, uint32_t count);
		int query_toal(userid_t userid, uint32_t type, uint32_t *total);
		int select_socks_info(userid_t userid, uint32_t type, uint32_t page, 
				user_query_christmas_socks_out_item **p_out_item, uint32_t *p_count);
		
		int select_page_count(userid_t userid, uint32_t *pages, uint32_t type);
		int query_some_friend(userid_t userid, uint32_t friendid, uint32_t type, uint32_t *total);
		int check_socks_gift(userid_t userid, uint32_t friendid, uint32_t *count, uint32_t *is_get);
		int set_flag(userid_t userid, uint32_t friendid);
};
#endif
