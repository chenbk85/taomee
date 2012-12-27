/*
 * =====================================================================================
 *
 *       Filename:  Cuser_mark_attire.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2012 02:11:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "Cuser_mark_attire.h"


Cuser_mark_attire::Cuser_mark_attire(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_mark_attire", "userid")
{

}


int Cuser_mark_attire::insert(userid_t userid, uint32_t attireid, uint32_t score)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(userid),
			userid,
			attireid,
			score
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_mark_attire::update(userid_t userid, uint32_t attireid, uint32_t score)
{
	sprintf(this->sqlstr, "update %s set score = %u where userid = %u and itemid = %u",
			this->get_table_name(userid),
			score,
			userid,
			attireid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_mark_attire::get_all(userid_t userid, uint32_t *p_count, user_get_mark_attire_out_item **pp_list)
{
	sprintf(this->sqlstr, "select itemid, score from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->attireid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->score);
	STD_QUERY_WHILE_END();
}
