/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_day_task.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/19/2011 10:29:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "common.h"
#include "Cuser_battle_day_task.h"

Cuser_battle_day_task::Cuser_battle_day_task(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_battle_day_task", "userid")
{

}

int Cuser_battle_day_task::insert(userid_t userid, uint32_t taskid, uint32_t type,
		uint32_t maxcount, uint32_t object_id)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, 1, 0, %u, 0, %u)",
			this->get_table_name(userid),
			userid,
			taskid,
			type,
			maxcount,
			object_id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_battle_day_task::update(userid_t userid, uint32_t taskid, const char* col,
		uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u and taskid = %u",
			this->get_table_name(userid),
			col,
			value,
			userid,
			taskid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_day_task::get_one_column(userid_t userid, uint32_t taskid, const char* col,
	   	uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u and taskid = %u",
			col,
			this->get_table_name(userid),
			userid,
			taskid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		*value=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Cuser_battle_day_task::get_two_columns(userid_t userid, uint32_t taskid, const char* col_1,
	   	const char* col_2, uint32_t *value_1, uint32_t *value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u and taskid = %u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid,
			taskid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		*value_1=atoi_safe(NEXT_FIELD); 
		*value_2=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Cuser_battle_day_task::clear_old_task(userid_t userid)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_day_task::get_day_task(userid_t userid, battle_day_tsk_t **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select taskid,state, count, maxcount from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count ) ;
		(*pp_list+i)->taskid = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->state = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->count = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->maxcount = atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

int Cuser_battle_day_task::get_taskid(userid_t userid, battle_task_id_t **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select taskid from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count ) ;
		(*pp_list+i)->taskid = atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();

}
int Cuser_battle_day_task::del_task_by_id(userid_t userid, uint32_t taskid)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and taskid = %u",
			this->get_table_name(userid),
			userid,
			taskid
			);
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_battle_day_task::update_one_task(userid_t userid, uint32_t taskid, uint32_t type, 
		uint32_t maxcount, uint32_t object_id, uint32_t old_id)
{
	sprintf(this->sqlstr, "update %s set taskid = %u, type = %u, state = 1, count = 0, maxcount = %u, \
			can_comp = 0, object_id = %u where userid = %u and taskid = %u",
			this->get_table_name(userid),
			taskid,
			type,
			maxcount,
			object_id,
			userid,
			old_id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_battle_day_task::update_day_task(userid_t userid, uint32_t taskid)
{
	uint32_t count = 0, maxcount = 0;
	get_two_columns(userid, taskid, "count", "maxcount", &count, &maxcount); 
	
	if(count + 1 > maxcount){
		return SUCC;
	}
	else{
		if(count + 1 == maxcount){
			sprintf(this->sqlstr, "update %s set count = %u, state = 2 where userid = %u and taskid = %u",
					this->get_table_name(userid),
					count+1,
					userid,
					taskid
					);		
		}
		else{
			sprintf(this->sqlstr, "update %s set count = %u where userid = %u \
					and taskid = %u",
					this->get_table_name(userid),
					count+1,
					userid,
					taskid
					);
		}
	}

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}
int Cuser_battle_day_task::get_taskid_by_object_type(userid_t userid, uint32_t type, uint32_t object_id,
		battle_task_id_t **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select taskid from %s where userid = %u and object_id = %u and type = %u",
			this->get_table_name(userid),
			userid,
			object_id,
			type
			);

	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count ) ;
		(*pp_list+i)->taskid = atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}
