/*
 * =====================================================================================
 *
 *       Filename:  Cuser_ocean_handbook.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/2012 06:56:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "Cuser_ocean_handbook.h"
#include "common.h"
#include "proto.h"

Cuser_ocean_handbook::Cuser_ocean_handbook(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_ocean_handbook", "userid")
{

}


int Cuser_ocean_handbook::insert(userid_t userid, uint32_t fish)
{
	sprintf(this->sqlstr,"insert into %s values( %u, %u)",
			this->get_table_name(userid),
			userid,
			fish
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_ocean_handbook:: get_all(userid_t userid, user_get_ocean_handbook_out_item** pp_list, 
		uint32_t *p_count)
{
	sprintf(this->sqlstr, "select itemid from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->fishid);
	STD_QUERY_WHILE_END();
}

