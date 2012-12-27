/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lucky_ticket.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/26/2011 02:31:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alexhe, alexhe@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "Cuser_lucky_ticket.h"
#include "common.h"

Cuser_lucky_ticket::Cuser_lucky_ticket(mysql_interface * db)
	 :CtableRoute100x10(db, "USER", "t_user_lucky_ticket", "userid")
{
}

int Cuser_lucky_ticket::insert(userid_t userid, uint32_t date, uint32_t ticket, uint32_t prize_level)
{
	sprintf(this->sqlstr, "insert into %s (userid, date, ticket, prize_level, is_get) values(%u, %u, %u, %u, 0)",
			this->get_table_name(userid), userid, date, ticket, prize_level);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_lucky_ticket::get_date_ticket(uint32_t userid, uint32_t date, user_check_get_ticket_out* p_list)
{
	sprintf(this->sqlstr, "select ticket, prize_level, is_get from %s where userid = %u and date = %u",
			this->get_table_name(userid), userid, date);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_list->ticket);
		INT_CPY_NEXT_FIELD(p_list->prize_level);
		INT_CPY_NEXT_FIELD(p_list->is_get);
	STD_QUERY_ONE_END();
}

int Cuser_lucky_ticket::get_swap_prize_info(userid_t userid, uint32_t date, uint32_t *is_get, uint32_t *prize_level)
{
	sprintf(this->sqlstr, "select is_get, prize_level from %s where userid = %u and date = %u",
			this->get_table_name(userid), userid, date);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*is_get);
		INT_CPY_NEXT_FIELD(*prize_level);
	STD_QUERY_ONE_END();
}

int Cuser_lucky_ticket::set_prize_is_get(userid_t userid, uint32_t date, uint32_t is_get)
{
	sprintf(this->sqlstr, "update %s set is_get = %u where userid = %u and date = %u",
			this->get_table_name(userid), is_get, userid, date);
	
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_lucky_ticket::get_history_ticket_info(userid_t userid, 
		user_get_history_ticket_info_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select ticket, date, prize_level, is_get from %s where userid = %u",
			this->get_table_name(userid), userid);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->ticket);
		INT_CPY_NEXT_FIELD((*pp_list + i)->date);
		INT_CPY_NEXT_FIELD((*pp_list + i)->prize_level);
		INT_CPY_NEXT_FIELD((*pp_list + i)->is_get);
	STD_QUERY_WHILE_END();
}
