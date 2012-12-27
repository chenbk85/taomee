/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angel_statistics.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/24/2011 07:38:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_angel_statistics.h"
#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "msglog.h"
#include "common.h"
#include <sstream>

Cuser_angel_statistics::Cuser_angel_statistics(mysql_interface *db): 
	CtableRoute100x10(db, "USER", "t_user_angel_statistic", "userid")
{

}
int Cuser_angel_statistics::insert(userid_t userid, uint32_t angel, uint32_t type, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			angel,
			type,
			count
			);
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_angel_statistics::update(userid_t userid, uint32_t angel, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = count + %u where userid = %u and angelid = %u",
			this->get_table_name(userid),
			count,
			userid,
			angel
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_angel_statistics::select(userid_t userid, uint32_t angel, uint32_t &count)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u and angelid = %u",
			this->get_table_name(userid),
			userid,
			angel
			);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (count);
	STD_QUERY_ONE_END();

}
int Cuser_angel_statistics::get_sort(userid_t userid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (count);
	STD_QUERY_ONE_END();

}
int Cuser_angel_statistics::get_all(userid_t userid, user_angel_statistic_stru **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select angelid, count from %s where userid = %u and angelid != 0",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->angelid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
	STD_QUERY_WHILE_END();	
}

int Cuser_angel_statistics::set_angel_type(userid_t userid, uint32_t angelid, uint32_t type)
{
	sprintf(this->sqlstr, "update %s set type = %u where userid = %u and angelid = %u",
			this->get_table_name(userid),
			type,
			userid,
			angelid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_angel_statistics::get_vip_angel(userid_t userid, uint32_t *vip_angel, uint32_t count,
		vip_angel_t **pp_list, uint32_t *p_count)
{
	std::ostringstream in_str;
    for(uint32_t i = 0; i < count; ++i) {
		if( i < count-1){
			in_str << vip_angel[i] << ",";
		}
		else{
			in_str << vip_angel[i];
		}
    }

	sprintf(this->sqlstr, "select angelid, count from %s where userid = %u and angelid in (%s)",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->angelid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
	STD_QUERY_WHILE_END();

}
