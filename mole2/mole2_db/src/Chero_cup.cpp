#include "Chero_cup.h"

Chero_cup::Chero_cup(mysql_interface * db)
	:Ctable(db , "RAND_ITEM" , "t_hero_cup")
{

}

int Chero_cup::set_teamid(userid_t userid,uint32_t teamid)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,%u,0) ON DUPLICATE KEY UPDATE teamid=%u",
		this->get_table_name(),userid,teamid,teamid);
	return this->exec_insert_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR);
}

int Chero_cup::get_teams_users(std::vector<stru_team_rank_t> &teams)
{
	GEN_SQLSTR(this->sqlstr, "select userid,teamid from %s where userid < 10",
		this->get_table_name());
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, teams);
		INT_CPY_NEXT_FIELD(item.teamid);
		INT_CPY_NEXT_FIELD(item.count);
	STD_QUERY_WHILE_END_NEW();
}

int Chero_cup::add_team_users(userid_t teamid)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,1,0) ON DUPLICATE KEY UPDATE teamid=teamid+1",
		this->get_table_name(),teamid);
	return this->exec_insert_sql(this->sqlstr, SUCC);
}

int Chero_cup::add_medals(userid_t userid,uint32_t medals)
{
	GEN_SQLSTR(this->sqlstr, "update %s set medals=medals+%u where userid=%u",
		this->get_table_name(),medals,userid);
	return this->exec_update_sql(this->sqlstr, MOLE2_KEY_NOT_EXIST_ERR);
}

int Chero_cup::get(userid_t userid,uint32_t &teamid,uint32_t &medals)
{
	sprintf(this->sqlstr, "select teamid,medals from %s where userid = %u",
		this->get_table_name(),userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr,MOLE2_KEY_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(teamid);
		INT_CPY_NEXT_FIELD(medals);
	STD_QUERY_ONE_END();
}

int Chero_cup::get_team_rank(std::vector<stru_team_rank_t> &ranks)
{
	GEN_SQLSTR(this->sqlstr, "select userid,teamid,medals from %s where userid < 5 order by medals desc",
		this->get_table_name());
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, ranks);
		INT_CPY_NEXT_FIELD(item.teamid);
		INT_CPY_NEXT_FIELD(item.count);
		INT_CPY_NEXT_FIELD(item.medals);
	STD_QUERY_WHILE_END_NEW();
}

int Chero_cup::get_user_rank(std::vector<stru_user_rank_t> &ranks)
{
	GEN_SQLSTR(this->sqlstr, "select userid,teamid,medals from %s where userid > 50000 order by medals desc limit 100",
		this->get_table_name());
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, ranks);
		INT_CPY_NEXT_FIELD(item.userid);
		INT_CPY_NEXT_FIELD(item.teamid);
		INT_CPY_NEXT_FIELD(item.medals);
	STD_QUERY_WHILE_END_NEW();
}

