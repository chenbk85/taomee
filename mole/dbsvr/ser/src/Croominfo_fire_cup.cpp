/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_gen_vip.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/08/2009 09:21:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_fire_cup.h"

Croominfo_fire_cup :: Croominfo_fire_cup(mysql_interface *db)
	              : CtableRoute10x10(db, "ROOMINFO", "t_roominfo_fire_cup", "userid")
{

}

int Croominfo_fire_cup :: insert(userid_t userid, uint32_t team)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u,0,0,0,0)",
			this->get_table_name(userid),
			userid,
			team
		   );
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}


int Croominfo_fire_cup :: get_team(userid_t userid, roominfo_get_fire_cup_team_out *p_team)
{
	sprintf(this->sqlstr, "select team from %s where userid = %u",
			this->get_table_name(userid),userid);
	 STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_team->teamid);
	 STD_QUERY_ONE_END();
}

int Croominfo_fire_cup :: update_today(userid_t userid, uint32_t today, uint32_t daymax)
{
	sprintf(this->sqlstr, "update %s set today=%u,day_left=%u where userid = %u and today != %u",
		this->get_table_name(userid),today,daymax,userid,today);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Croominfo_fire_cup:: update_team(userid_t userid, uint32_t team_id)
{
	sprintf(this->sqlstr, "update %s set team = %u where userid = %u",
			this->get_table_name(userid),
			team_id,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Croominfo_fire_cup :: getinfo(userid_t userid, roominfo_fire_cup_update_out *p_out)
{
	sprintf(this->sqlstr, "select team,count,today,day_left from %s where userid = %u",
		this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->teamid);
		INT_CPY_NEXT_FIELD(p_out->count);
		INT_CPY_NEXT_FIELD(p_out->today);
		INT_CPY_NEXT_FIELD(p_out->day_left);
	STD_QUERY_ONE_END();
}

int Croominfo_fire_cup :: addnum(userid_t userid, int addnum,uint32_t day_left)
{
	if(addnum > 0) {
		sprintf(this->sqlstr, "update %s set count=count+%d,day_left=%u where userid = %u",
			this->get_table_name(userid),addnum,day_left,userid);
	} else {
		sprintf(this->sqlstr, "update %s set count=count+%d where userid = %u",
			this->get_table_name(userid),addnum,userid);
	}

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Croominfo_fire_cup :: setflag(userid_t userid, roominfo_get_fire_cup_prize_out *p_out)
{
	sprintf(this->sqlstr, "select team, flag from %s where userid = %u",
			this->get_table_name(userid), userid);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->teamid);
		INT_CPY_NEXT_FIELD(p_out->flag);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if (p_out->flag == 0) {
		sprintf(this->sqlstr, "update %s set flag = 1, team = 0 where userid = %u",
			this->get_table_name(userid), userid);
		STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
	} else {
		return YOU_HAVE_GET_FIRECUP_PRIZE_ERR;
	}
}

