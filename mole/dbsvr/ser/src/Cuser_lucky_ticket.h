/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lucky_ticket.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/26/2011 02:26:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alexhe , alexhe@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CUSER_LUCKY_TICKET_INCL
#define CUSER_LUCKY_TICKET_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_lucky_ticket: public CtableRoute100x10 {
public:
	Cuser_lucky_ticket(mysql_interface * db);

	int insert(userid_t userid, uint32_t ticket, uint32_t date, uint32_t prize_level);
	int get_date_ticket(uint32_t userid, uint32_t date, user_check_get_ticket_out* p_list);
	int get_swap_prize_info(userid_t userid, uint32_t date, uint32_t *is_get, uint32_t *prize_level);
	int get_history_ticket_info(userid_t userid, user_get_history_ticket_info_out_item **pp_list, uint32_t *p_count);
	int set_prize_is_get(userid_t, uint32_t date, uint32_t is_get);
};
#endif
