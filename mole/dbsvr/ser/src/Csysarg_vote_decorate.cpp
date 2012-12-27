/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_vote_decorate.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/13/2012 06:11:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_vote_decorate.h"

Csysarg_vote_decorate::Csysarg_vote_decorate(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_vote_decorate")
{

}

int Csysarg_vote_decorate::insert(uint32_t userid, char* nick, uint32_t vote)
{
	
	char nick_mysql[mysql_str_len(NICK_LEN)];
	memset(nick_mysql, 0, sizeof(nick_mysql));
	set_mysql_string(nick_mysql,nick, NICK_LEN);
	sprintf(this->sqlstr, "insert into %s values(%u, '%s', %u, %u)",
			this->get_table_name(),
			userid,
			nick_mysql,
			vote,
			(uint32_t)time(0)
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);

}

int Csysarg_vote_decorate::get_count(uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s",
			this->get_table_name()
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}

int Csysarg_vote_decorate::update(uint32_t userid, char* nick, uint32_t vote)
{
	sprintf(this->sqlstr, "update %s set vote = vote + %u where userid = %u",
			this->get_table_name(),
			vote,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_vote_decorate::update_all(uint32_t userid, char* nick, uint32_t vote)
{

	char nick_mysql[mysql_str_len(NICK_LEN)];
	memset(nick_mysql, 0, sizeof(nick_mysql));
	set_mysql_string(nick_mysql,nick, NICK_LEN);
	sprintf(this->sqlstr, "update %s set userid = %u , vote = %u, nick = '%s', datetime = %u \
			order by datetime asc limit 1",
			this->get_table_name(),
			userid,
			vote,
			nick_mysql,
			(uint32_t)time(0)
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_vote_decorate::add(userid_t userid, char* nick, uint32_t vote)
{
	int ret = update(userid, nick, vote);
	if(ret != SUCC){
		uint32_t count = 0; 
		get_count(&count);
		if(count < 1000){
			ret = insert(userid, nick, vote);
		}
		else{
			ret = update_all(userid, nick, vote);
		}
		
	}
	return ret;
}

int Csysarg_vote_decorate::get(userid_t userid, uint32_t index, 
		sysarg_get_birthday_decorate_out_item** pp_list, uint32_t* p_count)
{
	uint32_t page = 0;	
	if(index >= 1){
		page = index - 1;
	}
	sprintf(this->sqlstr, "select userid, nick, vote from %s order by datetime desc, vote desc limit %u, 6",
			this->get_table_name(),
			page*6
			);
    STD_QUERY_WHILE_BEGIN(this-> sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
		BIN_CPY_NEXT_FIELD((*pp_list +i)->nick, NICK_LEN);
		INT_CPY_NEXT_FIELD((*pp_list + i)->vote);
    STD_QUERY_WHILE_END();
}

int Csysarg_vote_decorate::clear()
{
	sprintf(this->sqlstr, "delete from %s ",
			this->get_table_name()
			);

    STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);	
}
