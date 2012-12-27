/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_db_sports_teaminfo.cpp
 *
 *    Description:  sports teaminfo
 *
 *        Version:  1.0
 *        Created:  05/17/2010 01:28:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_db_sports_teaminfo.h"
/*
 *brief : 拉姆运动会，队伍信息 20100520
 *
 *
 *
 *
 *
 * */


Csysarg_db_sports_teaminfo::Csysarg_db_sports_teaminfo(mysql_interface * db)
	:Ctable(db, "SYSARG_DB", "t_sysarg_db_sports_teaminfo")
{

}

int Csysarg_db_sports_teaminfo::teaminfo_set(sysarg_db_sports_teaminfo_set_in *in,
		sysarg_db_sports_teaminfo_set_out *out)
{
	uint32_t teamid = in->teamid;
	sprintf(this->sqlstr, "select team1_score, team2_score, team3_score, team%u_medal, win_flag from %s",
			teamid,
			this->get_table_name()	
		);	
	STD_QUERY_ONE_BEGIN(this->sqlstr, SYSARG_DB_SPORTS_TEAMINFO_SET_ERR);
		INT_CPY_NEXT_FIELD(out->score[0]);
		INT_CPY_NEXT_FIELD(out->score[1]);
		INT_CPY_NEXT_FIELD(out->score[2]);
		INT_CPY_NEXT_FIELD(this->medal);
		INT_CPY_NEXT_FIELD(out->win_flag);
		
	
	STD_QUERY_ONE_END();
}

int Csysarg_db_sports_teaminfo::update1(uint32_t userid, uint32_t teamid, sysarg_db_sports_teaminfo_set_out *out)
{
	sprintf(this->sqlstr, "update %s a set a.team%u_score = %d",
			this->get_table_name(),
	     	teamid,
	        out->score[teamid - 1]
          );  
	STD_SET_RETURN(this->sqlstr,userid, SYSARG_DB_SPORTS_TEAMINFO_SET_ERR );
}

int Csysarg_db_sports_teaminfo::update2(uint32_t userid, uint32_t teamid, sysarg_db_sports_teaminfo_set_out *out)
{
	sprintf(this->sqlstr, "update %s a set a.team%u_medal = %d,\
			a.team1_score = 0, a.team2_score = 0, a.team3_score = 0, win_flag = %u",
			this->get_table_name(),
			teamid,
			(this->medal + 10),
			teamid);
	STD_SET_RETURN(this->sqlstr,userid, SYSARG_DB_SPORTS_TEAMINFO_SET_ERR );
}


int Csysarg_db_sports_teaminfo::score_get(sysarg_db_sports_teaminfo_score_get_out* out)
{
	sprintf(this->sqlstr, "select team1_score, team2_score, team3_score, win_flag from %s",
			this->get_table_name());
	STD_QUERY_ONE_BEGIN(this->sqlstr, DB_ERR);
	INT_CPY_NEXT_FIELD(out->score[0]);
	INT_CPY_NEXT_FIELD(out->score[1]);
	INT_CPY_NEXT_FIELD(out->score[2]);
	INT_CPY_NEXT_FIELD(out->win_flag);
	STD_QUERY_ONE_END();
}

int Csysarg_db_sports_teaminfo::medal_get(sysarg_db_sports_teaminfo_medal_get_out* out)
{
	sprintf(this->sqlstr, "select team1_medal, team2_medal, team3_medal from %s",
			this->get_table_name());
	STD_QUERY_ONE_BEGIN(this->sqlstr, DB_ERR);
	INT_CPY_NEXT_FIELD(out->medal[0]);
	INT_CPY_NEXT_FIELD(out->medal[1]);
	INT_CPY_NEXT_FIELD(out->medal[2]);
	STD_QUERY_ONE_END();
}

int Csysarg_db_sports_teaminfo::win_flag_get(uint32_t* teamid)
{
	sprintf(this->sqlstr, "select win_flag from %s",
			this->get_table_name());
	STD_QUERY_ONE_BEGIN(this->sqlstr, DB_ERR);
	INT_CPY_NEXT_FIELD(*teamid);
	STD_QUERY_ONE_END();
}

int Csysarg_db_sports_teaminfo::medal_update(uint32_t teamid, uint32_t medal)
{
	sprintf(this->sqlstr, "update %s a set a.team%u_medal = a.team%u_medal + %u",
			this->get_table_name(),
	     	teamid,
			teamid,
	        medal
          );  
	STD_SET_RETURN(this->sqlstr,userid, SYSARG_DB_SPORTS_TEAMINFO_SET_ERR );
}


