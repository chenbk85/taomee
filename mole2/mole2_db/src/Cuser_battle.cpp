#include "Cuser_battle.h"


#define STR_USERID		"userid"
#define STR_ALL_SCORE	"all_score"
#define STR_DAY_SCORE	"day_score"
#define STR_UPDATE_TIME	"update_time"
#define STR_PERSON_WIN	"person_win"
#define STR_PERSON_FAIL	"person_fail"
#define	STR_TEAM_WIN	"team_win"
#define STR_TEAM_FAIL	"team_fail"

#define BASE_SCORE	100

Cbattle::Cbattle(mysql_interface * db ) 
	 :CtableRoute100x10(db , "MOLE2_USER" , "t_user_battle" , "userid")
{

}

int Cbattle::insert(userid_t userid)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s(%s, %s) values(%u,%u)",
			this->get_table_name(userid),
			STR_USERID,
			STR_ALL_SCORE,
			userid,
			BASE_SCORE);
	return this->exec_insert_sql(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cbattle::insert(uint32_t userid, stru_mole2_user_battle_info_1* p_in)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s(%s,%s,%s,%s,%s, %s,%s,%s) \
			values(%u,%u,%u,%u,%u, %u,%u,%u)",
			this->get_table_name(userid),
			STR_USERID,
			STR_ALL_SCORE,
			STR_DAY_SCORE,
			STR_UPDATE_TIME,
			STR_PERSON_WIN,
			STR_PERSON_FAIL,
			STR_TEAM_WIN,
			STR_TEAM_FAIL,
			userid,
			p_in->binfo.all_score,
			p_in->binfo.day_score,
			p_in->binfo.update_time,
			p_in->person_win,
			p_in->person_fail,
			p_in->team_win,
			p_in->team_fail);
	return this->exec_insert_sql(this->sqlstr, USER_ID_EXISTED_ERR);
}


int Cbattle::info_set(uint32_t userid, stru_mole2_user_battle_info_1* p_in)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u,%s=%u,%s=%u,%s=%s+%u,%s=%s+%u,%s=%s+%u,%s=%s+%u where %s=%u",
			this->get_table_name(userid),
			STR_ALL_SCORE,		p_in->binfo.all_score,
			STR_DAY_SCORE,		p_in->binfo.day_score,
			STR_UPDATE_TIME,	p_in->binfo.update_time,
			STR_PERSON_WIN,		STR_PERSON_WIN,		p_in->person_win,
			STR_PERSON_FAIL,	STR_PERSON_FAIL,	p_in->person_fail,
			STR_TEAM_WIN,		STR_TEAM_WIN,		p_in->team_win,
			STR_TEAM_FAIL,		STR_TEAM_WIN,		p_in->team_fail,
			STR_USERID,			userid);
	return this->exec_update_sql( this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cbattle::info_set_ex(uint32_t userid, stru_mole2_user_battle_info_1* p_in)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u,%s=%u,%s=%u,%s=%u,%s=%u,%s=%u,%s=%u where %s=%u",
			this->get_table_name(userid),
			STR_ALL_SCORE,		p_in->binfo.all_score,
			STR_DAY_SCORE,		p_in->binfo.day_score,
			STR_UPDATE_TIME,	p_in->binfo.update_time,
			STR_PERSON_WIN,		p_in->person_win,
			STR_PERSON_FAIL,	p_in->person_fail,
			STR_TEAM_WIN,		p_in->team_win,
			STR_TEAM_FAIL,		p_in->team_fail,
			STR_USERID,			userid);
	return this->exec_update_sql( this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cbattle::info_get(uint32_t	userid, stru_mole2_user_battle_info* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s,%s,%s from %s where %s=%u",
			STR_ALL_SCORE,
			STR_DAY_SCORE,
			STR_UPDATE_TIME,
			this->get_table_name(userid),
			STR_USERID,		userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->all_score);
		INT_CPY_NEXT_FIELD(p_out->day_score);
		INT_CPY_NEXT_FIELD(p_out->update_time);
	STD_QUERY_ONE_END();
}

int Cbattle::get_online_login(uint32_t userid, stru_battle_info &p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s,%s,%s from %s where %s=%u",
			STR_ALL_SCORE,
			STR_DAY_SCORE,
			STR_UPDATE_TIME,
			this->get_table_name(userid),
			STR_USERID,		userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out.all_score);
		INT_CPY_NEXT_FIELD(p_out.day_score);
		INT_CPY_NEXT_FIELD(p_out.update_time);
	STD_QUERY_ONE_END();
}


