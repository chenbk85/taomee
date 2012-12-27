/*
 * =========================================================================
 *
 *        Filename: Cfind_map.cpp
 *
 *        Version:  1.0
 *        Created:  2011-05-13 15:52:03
 *        Description:   
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#include    "Cfind_map.h"
#include <time.h>
Cfind_map::Cfind_map(mysql_interface * db ) 
	:CtableRoute100x10(db, "POP" , "t_find_map" , "userid", "islandid")
{

}
	
int	Cfind_map::insert(userid_t userid,   uint32_t islandid, uint32_t mapid  )
{
	sprintf( this->sqlstr, "insert into %s values( %u,%u, %u )",
		this->get_table_name(userid), userid,islandid, mapid);
	return this->exec_insert_sql( this->sqlstr,MAPID_IS_EXISTED_ERR );
}


int	Cfind_map::get_list(userid_t userid, std::vector<find_map_t> &find_map_list )
{
    GEN_SQLSTR(this->sqlstr, "select islandid, mapid from %s where userid=%u ",
            this->get_table_name(userid),userid);
	find_map_t item;
    STD_QUERY_WHILE_BEGIN(this->sqlstr,find_map_list);
        INT_CPY_NEXT_FIELD(item.islandid);
        INT_CPY_NEXT_FIELD(item.mapid);
    STD_QUERY_WHILE_END();
}
int	Cfind_map::del(userid_t userid, uint32_t islandid, uint32_t mapid)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and islandid =%u and mapid =%u ",
		this->get_table_name(userid),  userid,islandid ,mapid);
	return this->exec_delete_sql( this->sqlstr, ITEM_NOFIND_ERR );
}
