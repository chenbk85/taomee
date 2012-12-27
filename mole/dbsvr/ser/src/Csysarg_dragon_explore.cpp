/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_dragon_explore.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/26/2011 04:23:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_dragon_explore.h"

Csysarg_dragon_explore::Csysarg_dragon_explore(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_dragon_explore")
{

}

int Csysarg_dragon_explore::insert(uint32_t itemid, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u)",
			this->get_table_name(),
			itemid,
			count
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Csysarg_dragon_explore::drop()
{
	sprintf(this->sqlstr, "delete from %s",
			this->get_table_name()
			);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

int Csysarg_dragon_explore::get_all(user_sysarg_get_map_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select itemid from %s",
			this->get_table_name()
			);

 	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->itemid);
	STD_QUERY_WHILE_END();
}
int Csysarg_dragon_explore::get_count(uint32_t *total)
{
	sprintf(this->sqlstr, "select count(*) from %s",
			this->get_table_name()
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*total);
    STD_QUERY_ONE_END();
}
