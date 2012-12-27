/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_prob_lahm.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/10/2012 04:16:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_prob_lahm.h"

Csysarg_prob_lahm::Csysarg_prob_lahm(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_prob_super_lahm")
{

}
int Csysarg_prob_lahm::insert(uint32_t userid, char* nick, uint32_t reg_time, uint32_t datetime)
{

	char nick_mysql[mysql_str_len(NICK_LEN)];
	memset(nick_mysql, 0, sizeof(nick_mysql));
	set_mysql_string(nick_mysql,nick, NICK_LEN);

	sprintf(this->sqlstr, "insert into %s values(%u, '%s', %u, %u)",
			this->get_table_name(),
			userid,
			nick_mysql,
			reg_time,
			datetime
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_prob_lahm::update_one_col(const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u",
			this->get_table_name(),
			col,
			value
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Csysarg_prob_lahm::get_one_col(const char* col, uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s",
			col,
			this->get_table_name()
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value);
	STD_QUERY_ONE_END();
}

int Csysarg_prob_lahm::get_all(uint32_t *super, uint32_t *ret_time, char* nick)
{
	sprintf(this->sqlstr, "select super_uid, nick, reg_time from %s",
            this->get_table_name()
            );
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*super);
        BIN_CPY_NEXT_FIELD (nick, NICK_LEN );
		INT_CPY_NEXT_FIELD (*ret_time);
    STD_QUERY_ONE_END();
}

int Csysarg_prob_lahm::update_all(userid_t super, uint32_t reg_time, char* nick)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	memset(nick_mysql, 0, sizeof(nick_mysql));
	set_mysql_string(nick_mysql,nick, NICK_LEN);

	sprintf(this->sqlstr, "update %s set super_uid = %u, nick = '%s', reg_time = %u ",
			this->get_table_name(),
			super,
			nick_mysql,
			reg_time
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
