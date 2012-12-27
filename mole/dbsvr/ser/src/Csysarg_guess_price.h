/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_guess_price.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/14/2012 10:20:53 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSAEG_GUESS_PRICE_INCL
#define CSYSAEG_GUESS_PRICE_INCL

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_guess_price:Ctable
{
	public:
		Csysarg_guess_price(mysql_interface *db);
		int insert(uint32_t userid, char* nick, uint32_t itemid,
				uint32_t price);
		int get_records_by_date(userid_t userid, uint32_t date, 
				sysarg_get_guess_price_out_item **pp_list, uint32_t *p_count);
		int get_exist(uint32_t userid, uint32_t itemid,  uint32_t &price);
		int update(userid_t userid, uint32_t itemid, uint32_t price);
		int get_records_by_itemid(sysarg_get_free_vip_player_out_item **pp_list, uint32_t *p_count);
};
#endif
