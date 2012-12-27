/*
 * =====================================================================================
 *
 *       Filename:  Cuser_temp_active.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/27/2012 05:00:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_TEMP_ACTIVE_INC
#define CUSER_TEMP_ACTIVE_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_temp_active:public CtableRoute100x10
{
	private:
		int insert(userid_t userid, uint32_t type, uint32_t value_1, uint32_t value_2);
	public:
		Cuser_temp_active(mysql_interface *db);
		int add(userid_t userid, uint32_t type, uint32_t value_1, uint32_t value_2);
		int get_one(userid_t userid, uint32_t type, const char* col, uint32_t *data);
		int get_two(userid_t userid, uint32_t type, const char* col_1, const char* col_2, uint32_t *data_1, uint32_t *data_2);
		int update(userid_t userid, uint32_t type, uint32_t value_1, uint32_t value_2);
        int get_pig_guess_info(userid_t userid,user_get_pig_guess_out_item** list,uint32_t* count);
		int drop_record(userid_t userid, uint32_t type);
		int  get_chapter_states(userid_t userid,user_get_story_chapter_state_out_item** list,uint32_t* count);
		int get_merman_kingdom(userid_t userid, user_get_merman_kingdom_out_item** pp_list, uint32_t *p_count);
		int update_one(userid_t userid, uint32_t type, const char* col, uint32_t value);
		int get_day_ocean_list(userid_t userid, uint32_t* p_count, user_get_mole_adventure_pass_out_item_2** pp_list);
};
#endif
