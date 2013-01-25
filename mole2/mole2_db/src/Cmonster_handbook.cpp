/*
 * =====================================================================================
 *
 *       Filename:  Cmonster_handbook.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/03/2010 04:23:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cmonster_handbook.h"


Cmonster_handbook::Cmonster_handbook(mysql_interface* db) :
	CtableRoute(db, "MOLE2_USER", "t_monster_handbook", "userid")
{

}




#define STR_USERID	"userid"
#define STR_MONSTERID	"monsterid"
#define STR_STATE	"state"
#define STR_COUNT	"count"

int Cmonster_handbook::insert(uint32_t userid,
	uint32_t monsterid,
	uint32_t state,
	uint32_t count)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			monsterid,
			state,
			count);
	return this->exec_insert_sql( this->sqlstr, MONSTER_HANDBOOK_ID_EXISTED_ERR);
}

int Cmonster_handbook::state_set(uint32_t userid, uint32_t monsterid, uint32_t state)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %u where %s = %u and %s = %u", 
			this->get_table_name(userid),
			STR_STATE,  state,
			STR_USERID, userid,
			STR_MONSTERID, monsterid);
	return this->exec_update_sql(this->sqlstr, MONSTER_HANDBOOK_ID_NOFIND_ERR);
}

int Cmonster_handbook::count_add(uint32_t userid, uint32_t monsterid, uint32_t count)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %s + %u where %s = %u and %s = %u", 
			this->get_table_name(userid), 
			STR_COUNT, STR_COUNT, count,
			STR_USERID, userid, 
			STR_MONSTERID, monsterid);
	return this->exec_update_sql(this->sqlstr, MONSTER_HANDBOOK_ID_NOFIND_ERR);
}

int Cmonster_handbook::list_range_get(userid_t userid, uint32_t min,uint32_t max,
		stru_mole2_monster_handbook_info** pp_out_item, uint32_t* p_count)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s from %s where %s=%u and monsterid >= %u and monsterid <= %u",
		STR_MONSTERID,
		STR_STATE,
		STR_COUNT,
		this->get_table_name(userid),
		STR_USERID, userid,
		min,max);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->monsterid);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->state);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->count);
	STD_QUERY_WHILE_END();
}

int Cmonster_handbook::list_get(userid_t userid, 
		stru_mole2_monster_handbook_info** pp_out_item, uint32_t* p_count)
{
	return this->list_range_get(userid,0,100000,pp_out_item,p_count);
}

int Cmonster_handbook::state_get(uint32_t userid, uint32_t monsterid, uint32_t* p_state)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u and %s=%u", 
			STR_STATE,
			this->get_table_name(userid), 
			STR_USERID,		userid,
			STR_MONSTERID,	monsterid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MONSTER_HANDBOOK_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_state);
	STD_QUERY_ONE_END();
}

int Cmonster_handbook::get_beasts(userid_t userid, std::vector<stru_beast_book> &beasts,uint32_t min,uint32_t max)
{
	GEN_SQLSTR(this->sqlstr, "select "
		"monsterid, state, count "
		"from %s where userid=%u and monsterid >= %u and monsterid <= %u",
		this->get_table_name(userid),userid,min,max);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr,beasts);
		INT_CPY_NEXT_FIELD(item.beastid);
		INT_CPY_NEXT_FIELD(item.state);
		INT_CPY_NEXT_FIELD(item.count);
	STD_QUERY_WHILE_END_NEW();
}


