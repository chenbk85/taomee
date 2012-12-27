/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_mvp_team.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/21/2012 02:53:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_mvp_team.h"

Csysarg_mvp_team::Csysarg_mvp_team(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_mvp_team")
{

}

int Csysarg_mvp_team::insert(uint32_t mvp_team, char *nick, uint32_t logo, uint32_t teamid)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	memset(nick_mysql, 0, sizeof(nick_mysql));
	set_mysql_string(nick_mysql,nick, NICK_LEN);

	sprintf(this->sqlstr, "insert into %s values(%u,'%s', %u, 0, %u, 1)",
			this->get_table_name(),
			mvp_team,
			nick_mysql,
			logo,
			teamid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);

}

int Csysarg_mvp_team::update(uint32_t mvp_team, uint32_t badge)
{
	sprintf(this->sqlstr, "update %s set badge = badge + %u where mvp_team = %u",
			this->get_table_name(),
			badge,
			mvp_team
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_mvp_team::change_member_count(uint32_t mvp_team, int32_t cnt)
{
	sprintf(this->sqlstr, "update %s set count = count + %d where mvp_team = %u",
			this->get_table_name(),
			cnt,
			mvp_team
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Csysarg_mvp_team::change_member_counts(uint32_t mvp_team, int32_t cnt)
{
	sprintf(this->sqlstr, "update %s set count = %u where mvp_team = %u",
			this->get_table_name(),
			cnt,
			mvp_team
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_mvp_team::drop(uint32_t mvp_team)
{
	sprintf(this->sqlstr, "delete from %s where mvp_team = %u",
			this->get_table_name(),
			mvp_team
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_mvp_team::get_page(uint32_t index, sysarg_get_mvp_team_out_item **pp_list, uint32_t *p_count)
{
	uint32_t page = 0;	
	if(index >= 1){
		page = index - 1;
	}
	DEBUG_LOG("page: %u", page);
	sprintf(this->sqlstr, "select mvp_team, nick, logo, badge, teamid, count from %s order by badge desc limit %u, 8",
			this->get_table_name(),
			page*8
			);
    STD_QUERY_WHILE_BEGIN(this-> sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->mvp_team);
		BIN_CPY_NEXT_FIELD((*pp_list +i)->mvp_team_name, NICK_LEN);
		INT_CPY_NEXT_FIELD((*pp_list + i)->logo);
		INT_CPY_NEXT_FIELD((*pp_list + i)->badge);
		INT_CPY_NEXT_FIELD((*pp_list + i)->sport_team);
		INT_CPY_NEXT_FIELD((*pp_list + i)->member_count);
    STD_QUERY_WHILE_END();

}

int Csysarg_mvp_team::get_count(uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s",
			this->get_table_name()
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}

int Csysarg_mvp_team::get_max_not_full(uint32_t sport_team, uint32_t *team_id)
{
	sprintf(this->sqlstr, "select mvp_team from %s where teamid = %u and count < 99 order by count desc limit 1",
			this->get_table_name(),
			sport_team
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*team_id);
	STD_QUERY_ONE_END();	
}
int Csysarg_mvp_team::get_one(uint32_t teamid,const char* col,uint32_t* data)
{
    sprintf(this->sqlstr, "select %s from  %s where teamid = %u",col,this->get_table_name(),teamid);
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
    *data=atoi_safe(NEXT_FIELD);
    STD_QUERY_ONE_END();
}

int Csysarg_mvp_team::select_mvp_team(uint32_t mvp_team, user_get_sysarg_mvp_teaminfo_out* out)
{
	sprintf(this->sqlstr, "select mvp_team, nick, logo, badge, teamid, count from %s where mvp_team = %u",
			this->get_table_name(),
			mvp_team
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out->mvp_team);
		BIN_CPY_NEXT_FIELD(out->name, NICK_LEN);
		INT_CPY_NEXT_FIELD(out->logo);
		INT_CPY_NEXT_FIELD(out->badge);
		INT_CPY_NEXT_FIELD(out->teamid);
		INT_CPY_NEXT_FIELD(out->count);
	STD_QUERY_ONE_END();	

}
