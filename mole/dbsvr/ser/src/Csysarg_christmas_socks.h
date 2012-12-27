/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_christmas_socks.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/12/2011 02:36:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_CHRISTMAS_SOCKS_INCL
#define CSYSARG_CHRISTMAS_SOCKS_INCL

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_christmas_socks:public Ctable
{
	public:
		Csysarg_christmas_socks(mysql_interface *db);
		int renew(userid_t userid, uint32_t count, uint32_t type = 0);
		int get_minority(userid_t *userid, uint32_t *count, uint32_t type = 0);
		int get_all(user_sysarg_get_christmas_top10_out_item **pp_list, uint32_t *p_count, uint32_t type = 0);
		int update_old(userid_t userid, uint32_t count, uint32_t type = 0);
	private:
		int update(userid_t userid, uint32_t count, uint32_t type = 0);
		int insert(userid_t userid, uint32_t count, uint32_t type = 0);
		int get_count(uint32_t *count, uint32_t type = 0);
		int	set_count(uint32_t userid, uint32_t count, uint32_t type = 0);

};
#endif
