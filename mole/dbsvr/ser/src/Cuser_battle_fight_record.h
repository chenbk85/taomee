/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_fight_record.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/13/2011 09:25:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_FIGHT_RECORD_INC
#define CUSER_BATTLE_FIGHT_RECORD_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_battle_fight_record:public CtableRoute100x10
{
	private:

	public:
		Cuser_battle_fight_record(mysql_interface *db);
		int insert(userid_t userid, uint32_t rival_id, uint32_t is_initial, uint32_t is_win, 
				uint32_t exp_inc, uint32_t fight_tim, uint32_t msg_type);
		int get_oldest(userid_t userid, uint32_t *datetime);
		int update(userid_t userid, user_battle_gefight_over_info_out_item *para, uint32_t datetime);
		int get_all_info(userid_t userid, user_battle_gefight_over_info_out_item **pp_list, uint32_t *p_count);
		int update_records(userid_t userid, user_battle_gefight_over_info_out_item *para);
		int get_record_cnt(userid_t userid, uint32_t &cnt);
		int del_friend_info(userid_t userid, uint32_t friend_id);
		int get_rival(userid_t userid, uint32_t **pp_list, uint32_t *p_count);
};

#endif
