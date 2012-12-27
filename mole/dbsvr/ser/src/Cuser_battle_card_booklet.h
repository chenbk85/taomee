/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_card_booklet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/20/2011 05:20:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_CARD_BOOKLET_INC
#define CUSER_BATTLE_CARD_BOOKLET_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_battle_card_booklet:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_battle_card_booklet(mysql_interface *db);
		int add_card(userid_t userid, uint32_t cardid, uint32_t type, uint32_t count, uint32_t limit, 
				uint32_t *if_first);
		int get_all(userid_t userid, uint32_t *p_count, user_battle_get_card_booklet_out_item **pp_list);
		int get_count(userid_t userid, uint32_t cardid, uint32_t &count);
		int get_variety_count(userid_t userid, uint32_t type, uint32_t &count);
};

#endif
