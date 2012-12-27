/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_handbook.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/19/2011 05:37:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "Cuser_piglet_handbook.h"
#include "common.h"
#include "proto.h"

Cuser_piglet_handbook::Cuser_piglet_handbook(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_piglet_handbook", "userid")
{

}


int Cuser_piglet_handbook::insert(userid_t userid, uint32_t breed)
{
	sprintf(this->sqlstr,"insert into %s values( %u, %u)",
			this->get_table_name(userid),
			userid,
			breed
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_piglet_handbook::select(userid_t userid, uint32_t breed, uint32_t* count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and breed = %u",
			this->get_table_name(userid),
			userid,
			breed
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}
int Cuser_piglet_handbook::add(userid_t userid, uint32_t breed)
{
	uint32_t count = 0;
	select(userid, breed, &count);
	int ret = 0;
	if(count == 0){
		ret = insert(userid, breed); 
	}
	return ret;
}
int Cuser_piglet_handbook:: get_all(userid_t userid, user_piglet_get_handbook_out_item** pp_list, 
		uint32_t *p_count)
{
	sprintf(this->sqlstr, "select breed from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->breed);
	STD_QUERY_WHILE_END();
}

