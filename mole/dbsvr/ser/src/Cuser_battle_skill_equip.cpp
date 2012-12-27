/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_skill_equip.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/16/2011 08:24:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */



#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "common.h"
#include "Cuser_battle_skill_equip.h"


Cuser_battle_skill_equip::Cuser_battle_skill_equip(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_battle_skill", "userid")
{

}
/*
 * type: 0表示主动技能，1表示被动技能
 */
int Cuser_battle_skill_equip::insert(userid_t userid, uint32_t skillid, uint32_t type, uint32_t level,
		uint32_t is_used)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u )",
			this->get_table_name(userid),
			userid,
			skillid,
			type,
			level,
			is_used
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_battle_skill_equip::update(userid_t userid, uint32_t skillid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid =  %u and skillid = %u", 
			this->get_table_name(userid),
			col,
			value,
			userid,
			skillid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_skill_equip::update_inc(userid_t userid, uint32_t skillid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u where userid =  %u and skillid = %u", 
			this->get_table_name(userid),
			col,
			col,
			value,
			userid,
			skillid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_skill_equip::put_off(userid_t userid )
{
	sprintf(this->sqlstr, "update %s set is_used = 0 where userid =  %u and type = 2", 
			this->get_table_name(userid),
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_battle_skill_equip::update_two_columns(userid_t userid, uint32_t skillid, const char*col_1, 
		const char* col_2, uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u where userid = %u and skillid = %u",
			this->get_table_name(userid),
			col_1,
			value_1,
			col_2,
			value_2,
			userid,
			skillid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_skill_equip::get_one_column(userid_t userid, uint32_t skillid, const char* col, 
		uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u and skillid = %u",
			col,
			this->get_table_name(userid),
			userid,
			skillid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value);
	STD_QUERY_ONE_END();
}

int Cuser_battle_skill_equip::get_equip_attire(userid_t userid, uint32_t *p_count, 
		user_get_battle_bag_out_item_3 **pp_list)
{
	sprintf(this->sqlstr, "select skillid from %s where userid = %u and ((type = 2 or type = 0) and is_used = 0)",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->attireid);
	STD_QUERY_WHILE_END();	
}

int Cuser_battle_skill_equip::get_used_equip_attire(userid_t userid, uint32_t *equip_id)
{
	sprintf(this->sqlstr, "select skillid from %s where userid = %u and type = 2 and is_used = 1 limit 1",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*equip_id);
	STD_QUERY_ONE_END();
}
int Cuser_battle_skill_equip::get_two_columns(userid_t userid, uint32_t skillid, const char* col_1, 
		const char* col_2, uint32_t *value_1, uint32_t *value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u and skillid = %u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid,
			skillid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value_1);
		INT_CPY_NEXT_FIELD (*value_2);
	STD_QUERY_ONE_END();
}
int Cuser_battle_skill_equip::get_skill_equip(userid_t userid, skill_equip_t **pp_list,
	   	uint32_t *p_count)
{
	sprintf(this->sqlstr, "select skillid, type, level, is_used from %s where userid = %u and type = 1",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->skillid);
		INT_CPY_NEXT_FIELD((*pp_list + i )->type);
		INT_CPY_NEXT_FIELD((*pp_list + i )->level);
		INT_CPY_NEXT_FIELD((*pp_list + i )->is_used);
	STD_QUERY_WHILE_END();	
}

int Cuser_battle_skill_equip::get_used_ini_skill(userid_t userid, used_skill_info **pp_list,
	   	uint32_t *p_count, uint32_t type, uint32_t is_used)
{
	sprintf(this->sqlstr, "select skillid, level from %s where userid = %u and type = %u and is_used = %u",
			this->get_table_name(userid),
			userid,
			type,
			is_used
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->skillid);
		INT_CPY_NEXT_FIELD((*pp_list + i )->level);
	STD_QUERY_WHILE_END();	
}

int Cuser_battle_skill_equip::get_equiped_skill(userid_t userid, used_skill_info **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select skillid, level from %s where userid = %u and ((type = 0 and is_used = 1) or type = 1)",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->skillid);
		INT_CPY_NEXT_FIELD((*pp_list + i )->level);
	STD_QUERY_WHILE_END();	
}

int Cuser_battle_skill_equip::get_more_level_non_ini_skill(userid_t userid, uint32_t level, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and type = 1 and level >= %u",
			this->get_table_name(userid),
			userid,
			level
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*p_count);
	STD_QUERY_ONE_END();
}

int Cuser_battle_skill_equip::get_equip_ini_skill_count(userid_t userid, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and type = 0 and is_used = 1",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
	STD_QUERY_ONE_END();

}
int Cuser_battle_skill_equip::get_want_skill(userid_t userid, user_battle_get_ini_skill_level_ex_out_item** pp_list,
		uint32_t* p_count, uint32_t type)
{
	sprintf(this->sqlstr, "select skillid, level from %s where userid = %u and type = %u",
			this->get_table_name(userid),
			userid,
			type
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->skillid);
		INT_CPY_NEXT_FIELD((*pp_list + i )->level);
	STD_QUERY_WHILE_END();	
}
