/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_task.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/14/2011 05:19:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_piglet_task.h"


Cuser_piglet_task::Cuser_piglet_task(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_piglet_day_task","userid")
{

}
int Cuser_piglet_task::insert(userid_t userid, uint32_t taskid, uint32_t type, uint32_t maxcount)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, 1, 0, %u)",
			this->get_table_name(userid),
			userid,
			taskid,
			type,
			maxcount
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}


int Cuser_piglet_task::update(userid_t userid, uint32_t taskid, 
		const char* col, uint32_t value)
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

int Cuser_piglet_task::change_task_state(userid_t userid, uint32_t taskid)
{
	uint32_t state = 0, count = 0;
	int ret = get_two_col(userid, taskid, "state", "count", &state, &count);
	if(ret == SUCC && state == 1){
		uint32_t maxcount = 0;
		get_one_col(userid, taskid, "maxcount", &maxcount);
		if(taskid > 10){//大于10表示每日任务, 高十六位表示每日次数，每天做多领取5次;低十六位表示完成任务目标
			maxcount = 0xFFFF & maxcount;
		}
		if(count + 1 >= maxcount){
			state = 2;
		}
		update_two_col(userid, taskid, "state", "count", state, count+1);
	}	
	return ret;
}

int Cuser_piglet_task::update_two_col(userid_t userid, uint32_t taskid,  
		const char* col_1, const char* col_2, uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u where userid = %u and taskid = %u",
			this->get_table_name(userid),
			col_1,
			value_1,
			col_2,
			value_2,
			userid,
			taskid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_task::update_state_count(userid_t userid, uint32_t taskid, uint32_t state,
		uint32_t count, uint32_t maxcount)
{
	sprintf(this->sqlstr, "update %s set state = %u, count = %u, maxcount = %u where \
			userid = %u and taskid = %u",
			this->get_table_name(userid),
			state,
			count,
			maxcount,
			userid,
			taskid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_task::get_one_col(userid_t userid, uint32_t taskid, 
		const char *col, uint32_t *value)
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
int Cuser_piglet_task::get_two_col(userid_t userid, uint32_t taskid, 
		const char *col_1, const char* col_2, uint32_t *value_1, uint32_t *value_2)
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
int Cuser_piglet_task::get_all_task(userid_t userid, user_get_piglet_day_task_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select taskid, state, count from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count ) ;
		(*pp_list+i)->taskid = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->state = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->count = atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}
int Cuser_piglet_task::get_day_task_count(userid_t userid, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and type > 10000",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		*count = atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Cuser_piglet_task::get_finish_day_task_count(userid_t userid, cp_task_info_t** pp_list, uint32_t* p_count)
{
	sprintf(this->sqlstr, "select taskid, maxcount from %s where userid = %u and taskid > 10",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count ) ;
		(*pp_list+i)->taskid = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->maxcount = atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

int Cuser_piglet_task::get_guide_task_count(userid_t userid, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and type < 10000 and state = 3",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		*p_count = atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}
int Cuser_piglet_task::clear_task(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set state = 1, count = 0, maxcount =  maxcount & 0xFFFF \
			where userid = %u and taskid > 10",
			this->get_table_name(userid),
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
