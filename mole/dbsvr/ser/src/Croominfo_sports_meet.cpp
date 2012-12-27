/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_sports_meet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2010 07:44:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_sports_meet.h"
/*
 * brief: 拉姆运动会20100513
 *
 *
 */
//using namespace std;

Croominfo_sports_meet::Croominfo_sports_meet(mysql_interface * db)
	:CtableRoute10x10(db, "ROOMINFO", "t_roominfo_sports_meet", "userid")
{

}

int Croominfo_sports_meet::teamid_set(uint32_t userid, uint32_t teamid)
{
	sprintf(this->sqlstr, "insert into %s(userid, teamid) values (%u, %u)",
		   	this->get_table_name(userid),
		   	userid, 
			teamid
			);
	STD_SET_RETURN_EX(this->sqlstr, ROOMINFO_SPORTS_MEET_SET_ERR);
}

int Croominfo_sports_meet::teamid_get(uint32_t userid, uint32_t* teamid, uint32_t* p_medal)
{
	sprintf(this->sqlstr, "select teamid,medal from %s where userid=%d",
		   this->get_table_name(userid),
		   userid
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr, ROOMINFO_SPORTS_MEET_GET_ERR);
		INT_CPY_NEXT_FIELD(*teamid);
		INT_CPY_NEXT_FIELD(*p_medal);
	STD_QUERY_ONE_END();
}

int Croominfo_sports_meet::medal_update(uint32_t userid, uint32_t medal_count)
{

    sprintf(this->sqlstr, "update %s set medal = medal + %u, medal_limit = medal_limit + %u\
			,time_limit = %u  where userid = %u",
		this->get_table_name(userid),
		medal_count,
		medal_count,
		(uint32_t)time(NULL),
		userid
		);  
	STD_SET_RETURN_EX(this->sqlstr, ROOMINFO_SPORTS_MEET_SET_ERR);
}


int Croominfo_sports_meet::top_medal_get(uint32_t userid, uint32_t* p_medal, uint32_t* p_time_limit)
{
	sprintf(this->sqlstr, "select medal_limit,time_limit from %s where userid = %d",
		   this->get_table_name(userid),
		   userid
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr, ROOMINFO_SPORTS_MEET_GET_ERR);
		INT_CPY_NEXT_FIELD(*p_medal);
		INT_CPY_NEXT_FIELD(*p_time_limit);
	STD_QUERY_ONE_END();
}

int Croominfo_sports_meet::reward_flag_select(uint32_t userid, roominfo_sports_reward_get_out *p_out)
{
	sprintf(this->sqlstr, "select reward_flag, medal, teamid from %s where userid = %u",
		   this->get_table_name(userid),
		   userid
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr, ROOMINFO_SPORTS_MEET_GET_ERR);
		INT_CPY_NEXT_FIELD(p_out->user_reward_flag);
		INT_CPY_NEXT_FIELD(p_out->user_medal);
		INT_CPY_NEXT_FIELD(p_out->user_teamid);
	STD_QUERY_ONE_END();


}


int Croominfo_sports_meet::reward_flag_update(uint32_t userid)
{
	sprintf(this->sqlstr, "update %s set reward_flag = 1 where userid = %u",
		   this->get_table_name(userid),
		   userid
		   );

	STD_SET_RETURN_EX(this->sqlstr, ROOMINFO_SPORTS_MEET_SET_ERR);
}

int Croominfo_sports_meet::reward_flag_insert(uint32_t userid)
{
	sprintf(this->sqlstr, "insert %s(userid,reward_flag) values(%u, 1)",
		   this->get_table_name(userid),
		  	userid 
		   );

	STD_SET_RETURN_EX(this->sqlstr, ROOMINFO_SPORTS_MEET_SET_ERR);
}
