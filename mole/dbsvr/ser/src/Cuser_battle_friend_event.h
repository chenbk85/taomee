/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_friend_event.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/22/2011 05:01:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_FRIEND_EVENT_INC
#define CUSER_BATTLE_FRIEND_EVENT_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_battle_friend_event:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_battle_friend_event(mysql_interface *db);
		int insert(userid_t userid, uint32_t friend_id, bat_arg_event_t *events);
		int insert(userid_t userid, uint32_t friend_id );
		int update(userid_t userid, uint32_t friend_id, bat_arg_event_t *events);
		int get_friend_event(userid_t userid, uint32_t friend_id, bat_arg_event_t *events);
		int update_fight_count_inc(userid_t userid, uint32_t friend_id, uint32_t fight_count);
		int clear_fight_count(userid_t userid);
		int get_friend_fight_list(userid_t userid, friend_fight_t **pp_list, uint32_t *p_count);
		int get_friend_fight_count(userid_t userid, uint32_t friend_id, uint32_t *fight_count);
		int get_rival_count(userid_t userid,  uint32_t *rival_cnt);
		int get_friend_local_events(userid_t userid, user_get_self_events_info_in_item* p_in_item, 
				uint32_t in_count, user_get_self_events_info_out_item **p_out_item, uint32_t *out_count);
};
#endif
