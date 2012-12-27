/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angel_honor.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/12/2011 02:17:09 PM
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
#include "Cuser_angel_honor.h"

Cuser_angel_honor::Cuser_angel_honor(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_angel_honor", "userid")
{

}

int Cuser_angel_honor::insert(userid_t userid, uint32_t honor_type, uint32_t honor_id, 
		uint32_t unlock, uint32_t recv)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			honor_type,
			honor_id,
			unlock,
			recv
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_angel_honor::update_lock(userid_t userid, uint32_t honor_type, uint32_t honor_id,
		uint32_t unlock)
{
	sprintf(this->sqlstr, "update %s set if_lock = %u where userid = %u and honor_type = %u \
			and honor_id = %u",
			this->get_table_name(userid),
			unlock,
			userid,
			honor_type,
			honor_id
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}

int Cuser_angel_honor::update_recv(userid_t userid, uint32_t honor_type, uint32_t honor_id, uint32_t recv)
{
	sprintf(this->sqlstr, "update %s set receive = %u where userid = %u and honor_type = %u \
			and honor_id = %u",
			this->get_table_name(userid),
			recv,
			userid,
			honor_type,
			honor_id
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}

int Cuser_angel_honor::update_recv_lock(userid_t userid, uint32_t honor_type, uint32_t honor_id,
	   	uint32_t unlock, uint32_t recv)
{
	sprintf(this->sqlstr, "update %s set if_lock = %u, receive = %u where userid = %u \
			and honor_type = %u and honor_id = %u",
			this->get_table_name(userid),
			unlock,
			recv,
			userid,
			honor_type,
			honor_id
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}
int Cuser_angel_honor::get_lock(userid_t userid, uint32_t honor_type, uint32_t honor_id, uint32_t *lock)
{
	sprintf(this->sqlstr, "select if_lock from %s where userid = %u and honor_type = %u and honor_id = %u",
			this->get_table_name(userid),
			userid,
			honor_type,
			honor_id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*lock);
	STD_QUERY_ONE_END();
}

int Cuser_angel_honor::get_record_cnt(userid_t userid, uint32_t *cnt)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u ",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*cnt);
	STD_QUERY_ONE_END();
}

int Cuser_angel_honor::get_recv(userid_t userid, uint32_t honor_type, uint32_t honor_id, uint32_t *recv)
{
	sprintf(this->sqlstr, "select receive from %s where userid = %u and honor_type = %u and honor_id = %u",
			this->get_table_name(userid),
			userid,
			honor_type,
			honor_id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*recv);
	STD_QUERY_ONE_END();
}
int Cuser_angel_honor::get_recv_lock(userid_t userid, uint32_t honor_type, uint32_t honor_id, 
		uint32_t *lock, uint32_t *recv)
{
	sprintf(this->sqlstr, "select if_lock, receive from %s where userid = %u and honor_type = %u and honor_id = %u",
			this->get_table_name(userid),
			userid,
			honor_type,
			honor_id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*lock);
		INT_CPY_NEXT_FIELD (*recv);
	STD_QUERY_ONE_END();
}
int Cuser_angel_honor::get_new_locks_cnt(userid_t userid,  uint32_t *cnt)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and honor_type = 1 and honor_id > 6",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*cnt);
	STD_QUERY_ONE_END();
}
 int Cuser_angel_honor::get_all_locks(userid_t userid, uint32_t honor_type, 
		 user_get_angel_honors_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select honor_id, receive from %s where userid = %u and honor_type = %u",
			this->get_table_name(userid),
			userid,
			honor_type
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->honor_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->recv);
	STD_QUERY_WHILE_END();	
}
int Cuser_angel_honor::get_honor_ids(userid_t userid, uint32_t honor_type, 
		 uint32_t **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select honor_id from %s where userid = %u and honor_type = %u",
			this->get_table_name(userid),
			userid,
			honor_type
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD(*(*pp_list + i));
	STD_QUERY_WHILE_END();	
}
