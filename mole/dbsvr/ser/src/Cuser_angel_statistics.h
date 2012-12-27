/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angel_statistics.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/24/2011 07:38:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_ANGEL_STATISTICS_INC
#define CUSER_ANGEL_STATISTICS_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 


class Cuser_angel_statistics:public CtableRoute100x10
{
	public:
		Cuser_angel_statistics(mysql_interface *db);
		int insert(userid_t userid, uint32_t angel, uint32_t type, uint32_t count);
		int update(userid_t userid, uint32_t angel, uint32_t count);
		int select(userid_t userid, uint32_t angle, uint32_t &count);
		int get_all(userid_t userid, user_angel_statistic_stru **pp_list, uint32_t *p_count);
		int get_sort(userid_t userid, uint32_t &count);
		int set_angel_type(userid_t userid, uint32_t angelid, uint32_t type);
		int get_vip_angel(userid_t userid, uint32_t *vip_angel, uint32_t count, vip_angel_t **pp_list,
			   	uint32_t *p_count);
};


#endif
