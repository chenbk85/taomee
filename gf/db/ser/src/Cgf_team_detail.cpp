#include <algorithm>
#include "Cgf_team_detail.h"
#include "db_error.h"

using namespace std;
//加入
Cgf_team_detail::Cgf_team_detail(mysql_interface * db ) 
	 : CtableRoute100(db, "GF_OTHER", "t_gf_team_detail", "team_id")
{ 

}

int Cgf_team_detail::add_team_member(uint32_t userid, uint32_t role_tm, uint32_t team_id)
{
	GEN_SQLSTR(sqlstr, "insert into %s (team_id, userid, role_tm, tm) \
		values(%u, %u, %u, unix_timestamp(now()))",
						get_table_name(team_id),team_id, userid, role_tm);
	return exec_update_sql(sqlstr, GF_TEAM_MEMBER_FULL_ERR);
}

int Cgf_team_detail::del_team(uint32_t team_id)
{
	GEN_SQLSTR(sqlstr,"delete from %s where team_id=%u",
		this->get_table_name(team_id), team_id);
	
	return this->exec_update_list_sql(this->sqlstr, SUCC);
}


int Cgf_team_detail::del_team_member(uint32_t userid, uint32_t role_tm, uint32_t team_id)
{
	GEN_SQLSTR(sqlstr, "delete from %s  where team_id=%u and userid=%u and role_tm=%u",
						get_table_name(team_id),team_id, userid, role_tm);
	return exec_update_sql(sqlstr, GF_TEAM_MEMBER_FULL_ERR);
}


int Cgf_team_detail::get_team_member_list(uint32_t team_id, uint32_t* p_count, gf_get_team_info_out_item** pp_list)
{
	GEN_SQLSTR(sqlstr, "select userid,role_tm,tm, level, team_coins, team_exp \
        from %s where team_id=%u limit 25", get_table_name(team_id),team_id);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->tm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->join_tm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->level);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->team_coins);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->team_exp);
	STD_QUERY_WHILE_END();

}



int Cgf_team_detail::get_team_member_list(uint32_t team_id, uint32_t* p_count, gf_add_fight_team_member_out_item** pp_list)
{
	GEN_SQLSTR(sqlstr, "select userid,role_tm,tm, level, team_coins, team_exp from %s where team_id=%u limit 25",
						get_table_name(team_id),team_id);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->uid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->role_tm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->join_tm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->level);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->team_coins);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->team_exp);
	STD_QUERY_WHILE_END();

}

int Cgf_team_detail::set_team_member_level(gf_set_team_member_level_in * p_in)
{
	GEN_SQLSTR(sqlstr, "update %s set level=%u where team_id=%u and userid=%u and role_tm=%u",
				get_table_name(p_in->team_id), p_in->level, p_in->team_id, p_in->userid, p_in->role_tm);
	return exec_update_sql(sqlstr, GF_TEAM_MEMBER_NOT_FOUND);
}

int Cgf_team_detail::increase_team_member_int_value(uint32_t team_id, uint32_t uid, uint32_t rtm, uint32_t value, const char *char_name)
{
	GEN_SQLSTR(sqlstr, "update %s set %s=%s+%u where team_id=%u and userid=%u and role_tm=%u",
				get_table_name(team_id), char_name, char_name, value, team_id, uid, rtm);
	return exec_update_sql(sqlstr, GF_TEAM_MEMBER_NOT_FOUND);
}

