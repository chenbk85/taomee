/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_challenge_note.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/23/2011 10:18:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_CHALLENGE_NOTE_INC
#define CUSER_BATTLE_CHALLENGE_NOTE_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_battle_challenge_note:public CtableRoute100x10
{
	public:
		Cuser_battle_challenge_note(mysql_interface *db);
		int insert(userid_t userid, uint32_t rival, uint32_t count, uint32_t threshhold, uint32_t vip_cnt, 
				uint32_t time_cnt, uint32_t pvp_cnt, uint32_t active_cnt);
		int update(userid_t userid, uint32_t rival, const char* col, uint32_t value);
		int update_inc(userid_t userid, uint32_t rival, const char* col, uint32_t value);
		int update_challenge_times(userid_t userid, uint32_t type);
		int update_two_column(userid_t userid, uint32_t rival, const char* col_1, 
				const char* col_2, uint32_t value_1, uint32_t value_2);
		int get(userid_t userid, uint32_t rival, const char* col, uint32_t *value);
		int get_info(userid_t userid, uint32_t rival, uint32_t *cnt, uint32_t *threshold, uint32_t *date);
		int update_info(userid_t userid, uint32_t rival, uint32_t count, uint32_t limit, uint32_t date);
		int get_challenge_cnt(userid_t userid, uint32_t rival, uint32_t *nor_cnt,
				uint32_t *vip_cnt, uint32_t *time_cnt, uint32_t *active_cnt, uint32_t *date);
		int clear_challenge_cnt(userid_t userid, uint32_t rival, uint32_t date);
		int get_two_col(userid_t userid, uint32_t rival, const char* col_1,
			   const char* col_2, uint32_t *value_1, uint32_t *value_2);
		int check_challenge_boss(userid_t userid, uint32_t type, uint32_t *ret);
};
#endif
