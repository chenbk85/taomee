/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_battle_exp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/26/2011 04:06:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_BATTLE_EXP_INC
#define CSYSARG_BATTLE_EXP_INC

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Angel_battle:public Ctable
{
	public:
		Angel_battle(mysql_interface *db);
		int add_exp(userid_t userid, uint32_t exp);
		int select_friend_exp(userid_t userid, uint32_t in_count, angel_battle_get_friends_exp_in_item *p_in_item, 
				uint32_t *out_count, angel_battle_get_friends_exp_out_item **p_out_item);
		int insert(userid_t userid, uint32_t exp, bat_arg_event_t *events);
		int update_events(userid_t userid, bat_arg_event_t *events);
		int get_event(userid_t userid, bat_arg_event_t *events);
		int get_wish_count(userid_t userid, uint32_t *wish_count);
		int get_variety_events(userid_t userid, user_battle_get_friend_event_info_in_item* p_in_item, 
				uint32_t in_count, user_battle_get_friend_event_info_out_item **p_out_item,  uint32_t *out_count);
		int set_wish_count(userid_t userid, uint32_t wish_count);
		int get_last_login_time(userid_t userid, uint32_t *datetime);
		int set_last_login_time(userid_t userid, uint32_t datetime);
		int get_one_colum(userid_t userid, const char* col, uint32_t *value);
		int get_two_column(userid_t userid, const char* col_1, const char* col_2, uint32_t* value_1, uint32_t* value_2);
		int update_one_column(userid_t userid, const char* col, uint32_t value);
		int get_prentice_level_and_master(userid_t userid, user_battle_get_prentice_market_in_item* p_in_item,
				uint32_t in_count,  user_battle_get_prentice_market_out_item** p_out_item, uint32_t *out_count);
		int get_master_level(user_battle_get_prentice_market_out_item* p_out_item, uint32_t out_count);
	private:
		int insert(userid_t userid, uint32_t exp);
		int update(userid_t userid, uint32_t exp);
};
#endif
