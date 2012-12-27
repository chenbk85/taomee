/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_mining.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/2012 04:44:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "Cuser_piglet_mining.h"
#include <sstream>


Cuser_piglet_mining::Cuser_piglet_mining(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_piglet_mining", "userid")
{

}

int Cuser_piglet_mining::add(userid_t userid, uint32_t mapid, uint32_t pigletid, uint32_t mining_end_time)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			mapid,
			pigletid,
			mining_end_time
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_piglet_mining::get_map_exist(userid_t userid, uint32_t mapid, uint32_t *cnt)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and mapid = %u",
			this->get_table_name(userid),
			userid,
			mapid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*cnt);
	STD_QUERY_ONE_END();	
}
int Cuser_piglet_mining::drop(userid_t userid, uint32_t in_count, user_finish_some_map_mining_in_item *p_in_list)
{

	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_list + i)->mapid;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	sprintf(this->sqlstr, "delete from %s where userid = %u and mapid in (%s)",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
			);

    STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}

int Cuser_piglet_mining::get_all(userid_t userid, uint32_t* p_count, user_get_piglet_mining_info_out_item **pp_list)
{
	sprintf(this->sqlstr, "select mapid, pigletid, mine_time from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

 	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->mapid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->piglet_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->mine_end_time);
	STD_QUERY_WHILE_END();
}
int Cuser_piglet_mining::get_one(userid_t userid, uint32_t mapid, const char* col, uint32_t *data)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u and mapid = %u",col,this->get_table_name(userid),userid,mapid);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
	*data=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}
int Cuser_piglet_mining::drop_one(userid_t userid, uint32_t mapid)
{
    sprintf(this->sqlstr, "delete from %s where userid = %u and mapid = %u",this->get_table_name(userid),userid,mapid);
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
