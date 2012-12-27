/*
 * =====================================================================================
 *
 *       Filename:  Cuser_base_character_attribute.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/15/2011 10:44:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_CHARACTER_ATTRIBUTE_INC
#define CUSER_BATTLE_CHARACTER_ATTRIBUTE_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_battle_character_attribute:public CtableRoute100x10
{
	private:

	public:
		Cuser_battle_character_attribute(mysql_interface *db);
		int insert(userid_t userid);
		int update(userid_t userid, const char* col, uint32_t value);
		int update_two_columns(userid_t userid, const char*col_1, const char* col_2, 
				uint32_t value_1, uint32_t value_2);
		int get_character_attribute(userid_t userid, t_battle_character_info* person);
		int get_one_column(userid_t userid, const char* col, uint32_t *value);
		int get_two_columns(userid_t userid, const char* col_1, const char* col_2, 
				uint32_t *value_1, uint32_t *value_2);
		int cal_battle_level(userid_t userid, uint32_t *level);
		int cal_person_attribute(userid_t userid, t_battle_character_info *person, uint32_t revover_time);
		int update_three_columns(userid_t userid, const char* col_1,const char* col_2,  const char* col_3, 
				uint32_t value_1, uint32_t value_2, uint32_t value_3);
		int get_character(userid_t userid, t_battle_character_info* person,uint32_t *if_first);
		int get_limit(uint32_t level, uint32_t *limit, uint32_t type);
		int use_recover_props(userid_t userid, uint32_t type, uint32_t props_level, uint32_t recover_point,
				uint32_t *value, uint32_t *day_add_point);
		int update_inc(userid_t userid, const char* col, uint32_t value);
		int update_base_attribute(userid_t userid, uint32_t flag, uint32_t value);
		int update_two_columns_inc(userid_t userid, const char* col_1, const char* col_2, 
				uint32_t value_1, uint32_t value_2);
		int update_secondary_attribute(userid_t userid, uint32_t level);
		int cal_level(uint32_t exp, uint32_t *level);
		int update_base_attribute(userid_t userid, user_battle_add_base_attr_t *p_in);
		int update_strong_attribute(userid_t userid, uint32_t old_point, uint32_t add_point);
		int get_level(userid_t userid, uint32_t *level);
};
#endif
