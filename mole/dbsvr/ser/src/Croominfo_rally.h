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



class Croominfo_rally : public CtableRoute10x10 {
	public:
		Croominfo_rally(mysql_interface *db);

		int insert(userid_t userid, uint32_t team);

		int sign_up(userid_t userid, uint32_t *p_team);

		int update_score_race(userid_t userid, uint32_t add_score, uint32_t race);

		int get_today_score(userid_t userid, uint32_t *p_day);

		int add_score(userid_t userid, uint32_t add_score, uint32_t race_pos, uint32_t *p_finish);

		int get_races(userid_t userid, uint32_t *p_race1, uint32_t *p_race2,
				      uint32_t *p_race3, uint32_t *p_race4);

		int update_race(userid_t userid);

		int change_race(userid_t userid);

		int get_all_info(userid_t userid, roominfo_rally_get_user_all_info_web_out *p_out);

		int get_team_medal(userid_t userid, uint32_t *p_score, uint32_t *p_team, uint32_t *p_medal);
};
