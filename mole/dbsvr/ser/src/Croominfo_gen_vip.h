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

class Croominfo_gen_vip : public CtableRoute10x10 {
	public:
		Croominfo_gen_vip(mysql_interface *db);

		int del(userid_t userid );
		int day_between(const time_t pre_time);

		int get_time(userid_t userid, uint32_t *p_oldtime, uint32_t *p_counttime, uint32_t *p_consume);

		int update_time(userid_t userid, uint32_t count, uint32_t consume);

		int insert(userid_t userid);

		int set_get_time(userid_t userid, uint32_t *p_oldtime, uint32_t *p_count, uint32_t *p_consume);

		int gen_vip(userid_t userid, uint32_t *flag);

		int check_done_today(userid_t userid, uint32_t *p_flag);
};
