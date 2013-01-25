/*
 * =====================================================================================
 *
 *       Filename:  Citem_handbook.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/03/2010 04:22:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Citem_handbook.h"

#define STR_USERID	"userid"
#define STR_ITEMID	"itemid"
#define STR_STATE	"state"
#define STR_COUNT	"count"



Citem_handbook::Citem_handbook(mysql_interface* db) :
	CtableRoute(db, "MOLE2_USER", "t_item_handbook", "userid")
{

}

int Citem_handbook::insert(uint32_t userid,
	uint32_t itemid,
	uint32_t state,
	uint32_t count)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u, %u, %u, %u)", 
			this->get_table_name(userid),
			userid,
			itemid,
			state,
			count);
	return this->exec_insert_sql( this->sqlstr, ITEM_HANDBOOK_ID_EXISTED_ERR);
}

int Citem_handbook::state_set(uint32_t userid, uint32_t itemid, uint32_t state)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %u where %s = %u and %s = %u", 
			this->get_table_name(userid),
			STR_STATE,	state,
			STR_USERID,	userid,
			STR_ITEMID,	itemid);
	return this->exec_update_sql(this->sqlstr, ITEM_HANDBOOK_ID_NOFIND_ERR);
}

int Citem_handbook::count_add(uint32_t userid, uint32_t itemid, uint32_t count)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %s + %u where %s = %u and %s = %u", 
			this->get_table_name(userid), 
			STR_COUNT, STR_COUNT, count,
			STR_USERID,	userid, 
			STR_ITEMID,	itemid);
	return this->exec_update_sql(this->sqlstr, ITEM_HANDBOOK_ID_NOFIND_ERR);
}


int Citem_handbook::list_get(userid_t userid,
		stru_mole2_item_handbook_info** pp_out_item, uint32_t* p_count)
{
	GEN_SQLSTR(this->sqlstr, "select %s,%s,%s from %s where %s=%u",
		STR_ITEMID,
		STR_STATE,
		STR_COUNT,
		this->get_table_name(userid),
		STR_USERID, userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->itemid);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->state);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->count);
	STD_QUERY_WHILE_END();
}

int Citem_handbook::state_get(uint32_t userid, uint32_t itemid, uint32_t* p_state)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u and %s=%u", 
			STR_STATE,
			this->get_table_name(userid), 
			STR_USERID,	userid,
			STR_ITEMID,	itemid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, ITEM_HANDBOOK_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_state);
	STD_QUERY_ONE_END();
}



