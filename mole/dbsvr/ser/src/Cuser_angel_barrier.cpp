/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angle_barrier.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/08/2011 09:48:28 AM
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
#include "Cuser_angel_barrier.h"

Cuser_angel_barrier::Cuser_angel_barrier(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_angel_barrier","userid")
{

}

int Cuser_angel_barrier::insert(userid_t userid, uint32_t barrier_id, uint32_t type, uint32_t score,
		uint32_t succ)
{
	sprintf(this->sqlstr, "insert into %s values(%u,%u,%u,%u, %u)",
			this->get_table_name(userid),
			userid,
			barrier_id,
			type,
			score,
			succ
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_angel_barrier::update(userid_t userid, uint32_t barrier_id, uint32_t type, uint32_t score)
{
	
	sprintf(this->sqlstr, "update %s set score = %u where userid = %u and barrier_id = %u and type = %u",
		this->get_table_name(userid),
		score,
		userid,
		barrier_id,
		type
		);	


	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_angel_barrier::update_succ(userid_t userid, uint32_t barrier_id, uint32_t type, uint32_t pass)
{
	sprintf(this->sqlstr, "update %s set success = %u where userid = %u and barrier_id = %u and type = %u",
		this->get_table_name(userid),
		pass,
		userid,
		barrier_id,
		type
		);	


	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}
int Cuser_angel_barrier::select(userid_t userid, uint32_t barrier_id, uint32_t type, uint32_t &score)
{
	sprintf(this->sqlstr, "select score from %s where userid = %u and barrier_id = %u and type = %u",
			this->get_table_name(userid),
			userid,
			barrier_id,
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (score);
	STD_QUERY_ONE_END();
}
int Cuser_angel_barrier::del(userid_t userid, uint32_t barrier_id, uint32_t type)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and barrier_id = %u and type = %u",
			this->get_table_name(userid),
			userid,
			barrier_id,
			type
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_angel_barrier::get_barrier_score(userid_t userid, uint32_t barrier_id,
	   	uint32_t *p_list)
{
	sprintf(this->sqlstr, "select type, score from %s where userid = %u and barrier_id = %u and type != 0",
			this->get_table_name(userid),
			userid,
			barrier_id
			);
	uint32_t type[3] = {0};
	uint32_t count = 0, score = 0;
	uint32_t *temp_list  = 0;

	STD_QUERY_WHILE_BEGIN(this->sqlstr, &temp_list, &count);
		INT_CPY_NEXT_FIELD(type[i]);
		INT_CPY_NEXT_FIELD(score);
		*(p_list + type[i] - 1) = score;
	STD_QUERY_WHILE_END_WITHOUT_RETURN();	

	if(temp_list !=0 ){
		free(temp_list);
	}
	return SUCC;

}
int Cuser_angel_barrier::get_barrier_succss(userid_t userid, uint32_t barrier_id, user_angel_barrier_stru **pp_list,
                uint32_t *p_count)
{
	sprintf(this->sqlstr, "select type, success from %s where userid = %u and barrier_id = %u and type != 0",
			this->get_table_name(userid),
			userid,
			barrier_id
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->type);
		INT_CPY_NEXT_FIELD((*pp_list + i )->succ);
	STD_QUERY_WHILE_END();	
}

