#include <algorithm>
#include "Cgf_task.h"
#include "db_error.h"

using namespace std;
//¼ÓÈë
Cgf_task::Cgf_task(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_task","userid")
{ 

}

int Cgf_task::get_task_finished(userid_t userid, userid_t role_regtime, uint32_t* p_count,
		gf_get_task_finished_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select taskid from %s \
		where userid=%u and role_regtime=%u and flag=%u;",
		this->get_table_name(userid), userid, role_regtime, SET_TASK_FINISHED);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->taskid );
	STD_QUERY_WHILE_END();
}

int Cgf_task::get_task_in_hand(userid_t userid, userid_t role_regtime, uint32_t* p_count,
		gf_get_task_in_hand_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select taskid,ser_value,as_value from %s \
		where userid=%u and role_regtime=%u and flag=%u;",
		this->get_table_name(userid), userid, role_regtime, SET_TASK_IN_HAND);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->taskid );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->ser_value, sizeof((*pp_list+i)->ser_value));
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->as_value, sizeof((*pp_list+i)->as_value));
	STD_QUERY_WHILE_END();
}

int Cgf_task::get_task_cancel(userid_t userid, userid_t role_regtime, uint32_t* p_count,
		gf_get_task_cancel_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select taskid from %s \
		where userid=%u and role_regtime=%u and flag=%u;",
		this->get_table_name(userid), userid, role_regtime, SET_TASK_CANCEL);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->taskid );
	STD_QUERY_WHILE_END();
}

int Cgf_task::get_task_list_count(userid_t userid, userid_t role_regtime, const char* str_where,
	uint32_t* p_count)
{
	GEN_SQLSTR(this->sqlstr,"select count(*) from %s \
		where userid=%u and role_regtime=%u and flag=3 and taskid in (%s);",
		this->get_table_name(userid), userid, role_regtime, str_where);
		STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(*p_count);
		STD_QUERY_ONE_END();	
}


int Cgf_task::get_all_task(userid_t userid, userid_t role_regtime, uint32_t* p_count,
		gf_get_all_task_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select taskid,flag from %s \
		where userid=%u and role_regtime=%u ;",
		this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->taskid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->task_flg );
		//BIN_CPY_NEXT_FIELD( (*pp_list+i)->ser_value, sizeof((*pp_list+i)->ser_value));
		//BIN_CPY_NEXT_FIELD( (*pp_list+i)->as_value, sizeof((*pp_list+i)->as_value));
	STD_QUERY_WHILE_END();
}

int Cgf_task::set_task_finished(userid_t userid, userid_t role_regtime, uint32_t taskid)
{
	return set_task_flg(userid, role_regtime, taskid, SET_TASK_FINISHED);
}

int Cgf_task::set_task_cancel(userid_t userid, userid_t role_regtime, uint32_t taskid)
{
	return set_task_flg(userid, role_regtime, taskid, SET_TASK_CANCEL);
}

int Cgf_task::del_task(userid_t userid, userid_t role_regtime, uint32_t taskid)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid=%u and role_regtime=%u and taskid=%u;",
		this->get_table_name(userid),userid,role_regtime,taskid);
	return this->exec_update_sql (this->sqlstr, GF_TASKID_NOFIND_ERR);	
}

int Cgf_task::del_daily_loop_task(userid_t userid, userid_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid=%u and role_regtime=%u \
        and flag>1 and taskid=8000;",
		this->get_table_name(userid),userid,role_regtime);
	return this->exec_update_list_sql (this->sqlstr, SUCC);	
}

int Cgf_task::del_old_daily_loop_task(userid_t userid, userid_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid=%u and role_regtime=%u \
        and taskid>10000;",
		this->get_table_name(userid),userid,role_regtime);
	return this->exec_update_list_sql (this->sqlstr, SUCC);	
}

int Cgf_task::set_task_in_hand(userid_t userid, userid_t role_regtime, uint32_t taskid)
{
	GEN_SQLSTR(this->sqlstr,"insert into %s (userid,role_regtime,taskid,optdate,\
		flag,ser_value,as_value) values \
		(%u,%u,%u,0,%u,'','')",
		this->get_table_name(userid),
		userid,role_regtime,taskid,SET_TASK_IN_HAND);
	
	return this->exec_insert_sql  (this->sqlstr, GF_TASK_EXISTED_ERR);		
}

int Cgf_task::set_task_ser_value(userid_t userid, userid_t role_regtime, uint32_t taskid, 
		uint8_t* binary_val,uint32_t binary_len)
{
    char taskvalue_mysql[mysql_str_len(binary_len)];
    set_mysql_string(taskvalue_mysql, (char*)binary_val, binary_len);

    sprintf( this->sqlstr, "update %s  set ser_value='%s' \
             where userid=%u and role_regtime=%u and taskid=%u",
             this->get_table_name(userid), taskvalue_mysql,
             userid, role_regtime, taskid );
	
    return this->exec_update_sql(this->sqlstr,GF_TASKID_NOFIND_ERR);		
}

int Cgf_task::set_task_as_value(userid_t userid, userid_t role_regtime, uint32_t taskid, 
		uint8_t* binary_val,uint32_t binary_len)
{
    char taskvalue_mysql[mysql_str_len(binary_len)];
    set_mysql_string(taskvalue_mysql, (char*)binary_val, binary_len);

    sprintf( this->sqlstr, "update %s  set as_value='%s' \
             where userid=%u and role_regtime=%u and taskid=%u",
             this->get_table_name(userid), taskvalue_mysql,
             userid, role_regtime, taskid );

    return this->exec_update_sql(this->sqlstr,GF_TASKID_NOFIND_ERR);

}

int Cgf_task::set_task_flg(userid_t userid, userid_t role_regtime, uint32_t taskid, int flg)
{
	GEN_SQLSTR(this->sqlstr,"update %s set flag=%u where userid=%u and role_regtime=%u and taskid=%u;",
		this->get_table_name(userid),flg,userid,role_regtime,taskid);
	int ret = this->exec_update_sql (this->sqlstr, GF_TASKID_NOFIND_ERR);		
	if(ret == SUCC && (flg == SET_TASK_CANCEL || flg == SET_TASK_FINISHED))
	{
		GEN_SQLSTR(sqlstr, "update %s set as_value = '', ser_value = '' where userid=%u and role_regtime=%u and taskid=%u;",
			get_table_name(userid), userid,role_regtime,taskid);
		return exec_update_sql (this->sqlstr, GF_TASKID_NOFIND_ERR);
	}
	return ret;
}

int Cgf_task::clear_role_task(userid_t userid,uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), userid, role_regtime);
	
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_task::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

