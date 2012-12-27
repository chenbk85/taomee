/*
 * =====================================================================================
 *
 *       Filename:  Cuser_ocean_adventure.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/11/2012 11:18:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_ocean_adventure.h"

Cuser_ocean_adventure::Cuser_ocean_adventure(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_ocean_adventure", "userid")
{

}

int Cuser_ocean_adventure::add(uint32_t userid, uint32_t prime, uint32_t secondary)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(userid),
			userid,
			prime,
			secondary
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_ocean_adventure::get(userid_t userid, uint32_t *p_count, 
		user_get_mole_adventure_pass_out_item_1** pp_list)

{
	sprintf(this->sqlstr, "select prime, secondary from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->prime);
		INT_CPY_NEXT_FIELD((*pp_list + i )->secondary);
	STD_QUERY_WHILE_END();	
}

int Cuser_ocean_adventure::select_max(userid_t userid, uint32_t &prime, uint32_t &secondary)
{
	sprintf(this->sqlstr, "select prime, secondary from %s where userid = %u order by prime desc, secondary desc limit 1",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (prime);
		INT_CPY_NEXT_FIELD (secondary);
	STD_QUERY_ONE_END();
}

int Cuser_ocean_adventure::clear(userid_t userid, uint32_t prime)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and prime = %u",
			this->get_table_name(userid),
			userid,
			prime
			);

    STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}

int Cuser_ocean_adventure::get_secondary(userid_t userid, uint32_t prime, uint32_t** pp_list, uint32_t* p_count)
{
	sprintf(this->sqlstr, "select secondary from %s where userid = %u and prime = %u",
			this->get_table_name(userid),
			userid,
			prime
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD(*(*pp_list + i ));
	STD_QUERY_WHILE_END();	

}
