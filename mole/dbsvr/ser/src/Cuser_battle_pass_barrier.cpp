/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_pass_barrier.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/24/2011 01:13:12 AM
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
#include <sstream>
#include "Cuser_battle_pass_barrier.h"


Cuser_battle_pass_barrier::Cuser_battle_pass_barrier(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_battle_pass_barrier", "userid")
{

}
int Cuser_battle_pass_barrier::insert(userid_t userid, uint32_t barrierid, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(userid),
			userid,
			barrierid,
			count
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_battle_pass_barrier::get(userid_t userid, uint32_t barrierid, uint32_t *count)
{
	sprintf(this->sqlstr, "select win_count from %s where userid = %u and barrerid = %u",
			this->get_table_name(userid),
			userid,
			barrierid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
	STD_QUERY_ONE_END();
}

int Cuser_battle_pass_barrier::update(userid_t userid, uint32_t barrierid, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set win_count = %u where userid = %u and barrerid = %u",
			this->get_table_name(userid),
			count,
			userid,
			barrierid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_pass_barrier::update_inc(userid_t userid, uint32_t barrierid, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set win_count = if(win_count + %u > 100, 100, win_count + %u) \
			where userid = %u and barrerid = %u",
			this->get_table_name(userid),
			count,
			count,
			userid,
			barrierid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_battle_pass_barrier::get_barrier_list(userid_t userid,  user_battle_map_info_in_item *p_in_item, 
		uint32_t in_count,  user_battle_map_info_out_item **p_out_item, uint32_t *out_count)
{
   std::ostringstream in_str;
    for(uint32_t i = 0; i < in_count; ++i){
        in_str << (p_in_item + i)->barrier_id;
        if(i < in_count - 1){
            in_str << ',';
        }
    }
    sprintf(this->sqlstr, "select barrerid, win_count from %s where userid = %u and barrerid  in (%s)",
            this->get_table_name(userid),
			userid,
            in_str.str().c_str()
           );

    STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out_item, out_count);
        INT_CPY_NEXT_FIELD((*p_out_item + i)->barrier_id);
        INT_CPY_NEXT_FIELD((*p_out_item + i)->comp_degree);
    STD_QUERY_WHILE_END();

}
