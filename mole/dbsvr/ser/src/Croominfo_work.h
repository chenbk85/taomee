/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_gen_vip.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/08/2009 09:21:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"


class Croominfo_work : public CtableRoute10x10 {
	public:
		Croominfo_work(mysql_interface *db);
		
		int set_value(userid_t userid, int32_t type);

		int get_value_last(userid_t userid, uint32_t *p_tom, uint32_t *p_nick);

		int get_value_this(userid_t userid, uint32_t *p_tom, uint32_t *p_nick);

	private:
		int get_week_id(time_t now, uint32_t *p_week_id);

		int update(userid_t userid, uint32_t type);

		int insert(userid_t userid, uint32_t type);
};
