/*
 * =====================================================================================
 *
 *       Filename:  Cuser_dragon_expolre.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/26/2011 03:23:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_dragon_explore.h"

Cuser_dragon_explore::Cuser_dragon_explore(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_dragon_explore", "userid")
{

}
int  Cuser_dragon_explore::insert(userid_t userid, uint32_t contribute, uint32_t state)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, 0)",
			this->get_table_name(userid),
			userid,
			state,
			contribute
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_dragon_explore::query(userid_t userid, uint32_t *contribute, uint32_t *state)
{
	sprintf(this->sqlstr, "select state, contribute from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*state);
        INT_CPY_NEXT_FIELD(*contribute);
    STD_QUERY_ONE_END();
}
int Cuser_dragon_explore::update_all(userid_t userid, uint32_t contribute, uint32_t state)
{
	sprintf(this->sqlstr,"update %s set state = %u, contribute = %u where userid = %u",
			this->get_table_name(userid),
			state,
			contribute,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_dragon_explore::update_inc_col(userid_t userid, const char *col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u where userid = %u",
			this->get_table_name(userid),
			col,
			col,
			value,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_dragon_explore::update(userid_t userid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u",
			this->get_table_name(userid),
			col,
			value,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
