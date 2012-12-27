/*
 * =====================================================================================
 *
 *       Filename:  Cuser_through_time_item.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/22/2011 03:49:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_through_time_item.h"

Cuser_through_time_item::Cuser_through_time_item(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_through_time_item", "userid")
{

}

int Cuser_through_time_item::insert(userid_t userid, uint32_t itemid)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(userid),
			userid,
			(uint32_t)time(0),
			itemid
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}


int Cuser_through_time_item::get_items(userid_t userid, roominfo_thr_tim_item_t **pp_list, 
		uint32_t *p_count)
{
	uint32_t now = time(0) - 3600*24;
	sprintf(this->sqlstr, "select sendtime, itemid  from %s where userid = %u and sendtime < %u",
			this->get_table_name(userid),
			userid,
			now
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->time);
		INT_CPY_NEXT_FIELD((*pp_list + i )->itemid);
	STD_QUERY_WHILE_END();	
}

int Cuser_through_time_item::get_non_dig_icnt(userid_t userid, uint32_t *count)
{
	uint32_t now = time(0) - 3600*24;
	sprintf(this->sqlstr, "select count(*)  from %s where userid = %u and sendtime < %u",
			this->get_table_name(userid),
			userid,
			now
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
	STD_QUERY_ONE_END();
}

int Cuser_through_time_item::remove(userid_t userid, uint32_t time)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and sendtime = %u",
			this->get_table_name(userid),
			userid,
			time
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

