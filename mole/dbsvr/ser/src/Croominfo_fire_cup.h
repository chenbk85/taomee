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

class Croominfo_fire_cup : public CtableRoute10x10 {
	public:
		Croominfo_fire_cup(mysql_interface *db);

		int insert(userid_t userid, uint32_t team);
		int get_team(userid_t userid, roominfo_get_fire_cup_team_out *p_team);
		int update_today(userid_t userid,uint32_t today, uint32_t daymax);
		int getinfo(userid_t userid, roominfo_fire_cup_update_out *p_out);
		int addnum(userid_t userid, int addnum, uint32_t day_left);
		int setflag(userid_t userid, roominfo_get_fire_cup_prize_out *p_out);
		int update_team(userid_t userid, uint32_t team_id);
};
