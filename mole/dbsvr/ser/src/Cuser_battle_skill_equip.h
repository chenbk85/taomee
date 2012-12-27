/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_skill_equip.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/16/2011 08:25:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#ifndef CUSER_BATTLE_SKILL_EQUIP_INC
#define CUSER_BATTLE_SKILL_EQUIP_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_battle_skill_equip:public CtableRoute100x10
{
	private:

	public:
		Cuser_battle_skill_equip(mysql_interface *db);
		int insert(userid_t userid, uint32_t skillid, uint32_t type, uint32_t level, uint32_t is_used);
		int update(userid_t userid, uint32_t skillid, const char* col, uint32_t value);
		int update_two_columns(userid_t userid, uint32_t skillid, const char*col_1, const char* col_2, 
				uint32_t value_1, uint32_t value_2);
		int get_one_column(userid_t userid, uint32_t skillid, const char* col, uint32_t *value);
		int get_two_columns(userid_t userid, uint32_t skillid, const char* col_1, const char* col_2, 
				uint32_t *value_1, uint32_t *value_2);
		int get_skill_equip(userid_t userid, skill_equip_t **pp_list, uint32_t *p_count);
		int get_used_ini_skill(userid_t userid, used_skill_info **pp_list,
					uint32_t *p_count, uint32_t type, uint32_t is_used);
		int get_equiped_skill(userid_t userid, used_skill_info **pp_list, uint32_t *p_count);
		int update_inc(userid_t userid, uint32_t skillid, const char* col, uint32_t value);
		int get_used_equip_attire(userid_t userid, uint32_t *equip_id);
		int get_more_level_non_ini_skill(userid_t userid, uint32_t level, uint32_t *p_count);
		int get_equip_attire(userid_t userid, uint32_t *p_count, user_get_battle_bag_out_item_3 **pp_list);
		int put_off(userid_t userid );
		int get_equip_ini_skill_count(userid_t userid, uint32_t *count);
		/* int get_used_equip_attire(userid_t userid, uint32_t *p_count, user_get_battle_bag_out_item_3 **pp_list);*/
		int get_want_skill(userid_t userid, user_battle_get_ini_skill_level_ex_out_item** pp_list,
				uint32_t* p_count, uint32_t type);
};
#endif
