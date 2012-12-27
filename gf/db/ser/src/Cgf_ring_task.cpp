#include <algorithm>
#include "db_error.h"
#include "Cgf_ring_task.h"

Cgf_ring_task::Cgf_ring_task(mysql_interface * db)
				:CtableRoute( db,"GF" ,"t_gf_ring_task","userid")
{

}

int Cgf_ring_task::get_ring_task_list(userid_t userid, uint32_t role_regtime, gf_get_ring_task_list_out_element** p_data, uint32_t* count)
{
	GEN_SQLSTR(sqlstr, "select task_id, task_type, master_task_id, task_group_id from %s where userid =  %u and role_regtime = %u",
				get_table_name(userid), userid, role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_data, count);
		INT_CPY_NEXT_FIELD( (*p_data+i)->task_id);
		INT_CPY_NEXT_FIELD( (*p_data+i)->task_type);
		INT_CPY_NEXT_FIELD( (*p_data+i)->master_task_id);
		INT_CPY_NEXT_FIELD( (*p_data+i)->task_group_id);	
	STD_QUERY_WHILE_END();
}

int Cgf_ring_task::replace_ring_task(userid_t userid, uint32_t role_regtime, uint32_t task_id, uint32_t type, uint32_t master_task_id, uint32_t task_group_id)
{
	GEN_SQLSTR(sqlstr, "replace into %s values(%u, %u, %u, %u, %u, %u)",  get_table_name(userid),
					userid,
					role_regtime,
					task_id,
					type,
					master_task_id,
					task_group_id
				);
	return exec_update_list_sql(sqlstr,SUCC);	
}

int Cgf_ring_task::delete_ring_master_task(userid_t userid, uint32_t role_regtime, uint32_t task_id)
{
	GEN_SQLSTR(sqlstr, "delete from %s where userid = %u and role_regtime = %u and task_id = %u", get_table_name(userid),
				userid,
				role_regtime,
				task_id
			);
	int ret =  exec_update_list_sql(sqlstr,SUCC);
	if(ret != SUCC)return ret;

	GEN_SQLSTR(sqlstr, "delete from %s where  userid = %u and role_regtime = %u and master_task_id = %u",  get_table_name(userid),
				userid, 
				role_regtime,
				task_id
			);

	return exec_update_list_sql(sqlstr,SUCC);
}
int Cgf_ring_task::delete_ring_slave_task(userid_t userid, uint32_t role_regtime, uint32_t task_id)
{
	GEN_SQLSTR(sqlstr, "delete from %s where userid = %u and role_regtime = %u and task_id = %u", get_table_name(userid),
				userid,
				role_regtime,
				task_id
			);
	return exec_update_list_sql(sqlstr,SUCC);
}

int Cgf_ring_task::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

Cgf_ring_task_history::Cgf_ring_task_history(mysql_interface * db)
			:CtableRoute( db,"GF" ,"t_gf_ring_task_history","userid")
{

}

int Cgf_ring_task_history::get_ring_task_history_list(userid_t userid, uint32_t role_regtime, gf_get_ring_task_history_list_out_element** p_data, uint32_t* count)
{
	GEN_SQLSTR(sqlstr, "select task_id, day_count, last_tm from %s where userid = %u and role_regtime = %u", 
			get_table_name(userid),
			userid,
			role_regtime
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_data, count);
		INT_CPY_NEXT_FIELD( (*p_data+i)->task_id);
		INT_CPY_NEXT_FIELD( (*p_data+i)->day_count);
		INT_CPY_NEXT_FIELD( (*p_data+i)->last_tm);
	STD_QUERY_WHILE_END();
}

int Cgf_ring_task_history::replace_ring_task_history(userid_t  userid, uint32_t role_regtime, uint32_t task_id, uint32_t day_count, uint32_t cur_time)
{
	GEN_SQLSTR(sqlstr, "replace into %s values(%u, %u, %u, %u, %u)", get_table_name(userid),
				userid,
				role_regtime,
				task_id, 
				day_count,
				cur_time
			  );
	return exec_update_list_sql(sqlstr,SUCC);
}

int Cgf_ring_task_history::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}












