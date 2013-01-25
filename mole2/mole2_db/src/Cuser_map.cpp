/*
 * =====================================================================================
 *
 *       Filename:  Cuser_map.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/02/2010 06:19:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_map.h"

#define STR_USERID	"userid"
#define STR_MAPID	"mapid"
#define STR_FLIGHT	"flight_path"


Cuser_map::Cuser_map(mysql_interface * db)
	: CtableRoute(db, "MOLE2_USER", "t_user_map", "userid")
{
}

int Cuser_map::insert(uint32_t userid,
	uint32_t mapid,
	uint32_t flight)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u, %u, %u)", 
			this->get_table_name(userid),
			userid,
			mapid,
			flight);
	return this->exec_insert_sql( this->sqlstr, USER_MAP_MAPID_EXISTED_ERR);
}

int Cuser_map::flight_list_get(userid_t userid, 
		user_map_flight_list_get_out_item** pp_out_item, uint32_t* p_count)
{
	GEN_SQLSTR(this->sqlstr, "select %s,%s from %s where %s=%u" ,
		STR_MAPID,STR_FLIGHT,
		this->get_table_name(userid),
		STR_USERID, userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->mapid);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->flight);
	STD_QUERY_WHILE_END();
}

int Cuser_map::flight_get(uint32_t userid, uint32_t mapid, uint32_t* p_flight)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s = %u and %s = %u",
		STR_FLIGHT,
		this->get_table_name(userid),
		STR_USERID, userid,
		STR_MAPID,	mapid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_MAP_MAPID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_flight);
	STD_QUERY_ONE_END();
}

int Cuser_map::flight_set(uint32_t userid, uint32_t mapid, uint32_t flight)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %s | %u where %s = %u and %s=%u", 
			this->get_table_name(userid), 
			STR_FLIGHT, STR_FLIGHT, flight,
			STR_USERID,	userid,
			STR_MAPID,	mapid);
	return this->exec_update_sql(this->sqlstr, USER_MAP_MAPID_NOFIND_ERR);
}

int Cuser_map::get_mapstates(userid_t userid, std::vector<stru_map_state> &states)
{
	GEN_SQLSTR(this->sqlstr, "select mapid,flight_path from %s where userid=%u",
		this->get_table_name(userid),userid);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, states);
		INT_CPY_NEXT_FIELD(item.mapid);
		INT_CPY_NEXT_FIELD(item.state);
	STD_QUERY_WHILE_END_NEW();
}

