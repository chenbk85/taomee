/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_prob_month_lahm.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/11/2012 10:21:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_prob_month_lahm.h"

Csysarg_prob_month_lahm::Csysarg_prob_month_lahm(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_prob_month_lahm")
{

}

int Csysarg_prob_month_lahm::insert(uint32_t userid, uint32_t datetime, char* nick)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	memset(nick_mysql, 0, sizeof(nick_mysql));
	set_mysql_string(nick_mysql,nick, NICK_LEN);

	sprintf(this->sqlstr, "insert into %s values(%u, %u, '%s')",
			this->get_table_name(),
			userid,
			datetime,
			nick_mysql
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_prob_month_lahm::get_count(userid_t userid, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s ",
			this->get_table_name()
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
	STD_QUERY_ONE_END();
}

int Csysarg_prob_month_lahm::get_all(prob_super_lahm_t **pp_list, uint32_t* p_count) 
{
	sprintf(this->sqlstr, "select userid, datetime, nick from %s",
			this->get_table_name()
			);
 	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->datetime);
		BIN_CPY_NEXT_FIELD((*pp_list +i)->nick, NICK_LEN);
	STD_QUERY_WHILE_END();
}
