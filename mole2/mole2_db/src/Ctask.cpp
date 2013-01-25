
#include "Ctask.h"


#define STR_UID		"userid"
#define STR_TASKID	"taskid"
#define STR_NODE	"node"
#define STR_STATE	"state"
#define STR_OPTDATE	"optdate"
#define STR_FINTIME	"fin_time"
#define	STR_FINNUM	"fin_num"
#define	STR_CLIBUF	"cli_buf"
#define STR_SERBUF	"ser_buf"

#define TASK_STATE_ON		1

Ctask::Ctask(mysql_interface* db):
	CtableRoute(db, "MOLE2_USER", "t_task", "userid")
{
}

int Ctask::insert(uint32_t userid, stru_mole2_task_info* p_in)
{
	char cli_mysql[mysql_str_len(TASK_CLI_BUF_LEN)];
	char ser_mysql[mysql_str_len(TASK_SVR_BUF_LEN)];
	set_mysql_string(cli_mysql, p_in->cli_buf, TASK_CLI_BUF_LEN);
	set_mysql_string(ser_mysql, p_in->ser_buf, TASK_SVR_BUF_LEN);
	GEN_SQLSTR(this->sqlstr, "insert into %s(%s,%s,%s,%s,%s,%s,%s,%s,%s) \
			values(%u,%u,%u,%u,%u,%u,%u,'%s','%s')",
			this->get_table_name(userid),
			STR_UID,
			STR_TASKID,
			STR_NODE,
			STR_STATE,
			STR_OPTDATE,
			STR_FINTIME,
			STR_FINNUM,
			STR_CLIBUF,
			STR_SERBUF,
			userid,
			p_in->taskid,
			p_in->node,
			p_in->state,
			p_in->optdate,
			p_in->fin_time,
			p_in->fin_num,
			cli_mysql,
			ser_mysql );
	return this->exec_insert_sql(this->sqlstr, MOLE2_TASK_EXISTED_ERR);
}

int Ctask::task_set(uint32_t userid, stru_mole2_task_info* p_in)
{
	char cli_mysql[mysql_str_len(TASK_CLI_BUF_LEN)];
	char ser_mysql[mysql_str_len(TASK_SVR_BUF_LEN)];
	set_mysql_string(cli_mysql, p_in->cli_buf, TASK_CLI_BUF_LEN);
	set_mysql_string(ser_mysql, p_in->ser_buf, TASK_SVR_BUF_LEN);
	
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s='%s', %s='%s' where %s = %u and %s = %u",
			this->get_table_name(userid),
			STR_NODE,		p_in->node,
			STR_STATE,		p_in->state,
			STR_OPTDATE,	p_in->optdate,
			STR_FINTIME,	p_in->fin_time,
			STR_FINNUM,		p_in->fin_num,
			STR_CLIBUF,		cli_mysql,
			STR_SERBUF,		ser_mysql,
			STR_UID,		userid,
			STR_TASKID,		p_in->taskid);
	return this->exec_update_sql(this->sqlstr, MOLE2_TASK_NOFIND_ERR);
}

int Ctask::task_get(uint32_t userid, uint32_t taskid, stru_mole2_task_info* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s,%s,%s,%s,%s,%s,%s,%s from %s where %s=%u and %s=%u", 
			STR_TASKID,
			STR_NODE,
			STR_STATE,
			STR_OPTDATE,
			STR_FINTIME,
			STR_FINNUM,
			STR_CLIBUF,
			STR_SERBUF,
			this->get_table_name(userid), 
			STR_UID,		userid,
			STR_TASKID,		taskid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_TASK_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->taskid);
		INT_CPY_NEXT_FIELD(p_out->node);
		INT_CPY_NEXT_FIELD(p_out->state);
		INT_CPY_NEXT_FIELD(p_out->optdate);
		INT_CPY_NEXT_FIELD(p_out->fin_time);
		INT_CPY_NEXT_FIELD(p_out->fin_num);
		BIN_CPY_NEXT_FIELD(p_out->cli_buf, TASK_CLI_BUF_LEN);
		BIN_CPY_NEXT_FIELD(p_out->ser_buf, TASK_SVR_BUF_LEN);
	STD_QUERY_ONE_END();
}

int Ctask::on_list_get(userid_t userid, stru_mole2_task_info** pp_out_item, uint32_t* p_count)
{
	GEN_SQLSTR(this->sqlstr, "select %s,%s,%s,%s,%s,%s,%s,%s from %s where %s=%u and %s = %u", 
			STR_TASKID,
			STR_NODE,
			STR_STATE,
			STR_OPTDATE,
			STR_FINTIME,
			STR_FINNUM,
			STR_CLIBUF,
			STR_SERBUF,
			this->get_table_name(userid), 
			STR_UID,		userid,
			STR_STATE,		TASK_STATE_ON);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->taskid);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->node);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->state);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->optdate);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->fin_time);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->fin_num);
		BIN_CPY_NEXT_FIELD((*pp_out_item + i)->cli_buf, TASK_CLI_BUF_LEN);
		BIN_CPY_NEXT_FIELD((*pp_out_item + i)->ser_buf, TASK_SVR_BUF_LEN);
	STD_QUERY_WHILE_END();
}

int Ctask::other_list_get(userid_t userid, stru_mole2_task_info_simple** pp_out_item, uint32_t* p_count)
{
	GEN_SQLSTR(this->sqlstr, "select %s,%s,%s,%s,%s from %s where %s=%u and %s <> %u", 
			STR_TASKID,
			STR_STATE,
			STR_OPTDATE,
			STR_FINTIME,
			STR_FINNUM,
			this->get_table_name(userid),
			STR_UID,		userid,
			STR_STATE,		TASK_STATE_ON);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->taskid);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->state);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->optdate);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->fin_time);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->fin_num);
	STD_QUERY_WHILE_END();
}

int Ctask::svr_buf_set(userid_t userid, uint32_t taskid, char* p_buf)
{
	char data_mysql[mysql_str_len(TASK_SVR_BUF_LEN)];
	set_mysql_string(data_mysql, p_buf, TASK_SVR_BUF_LEN);
	GEN_SQLSTR(this->sqlstr, "update %s set %s='%s' where %s = %u and %s = %u",
			this->get_table_name(userid),
			STR_SERBUF,		data_mysql,
			STR_UID,		userid,
			STR_TASKID,		taskid);
	return this->exec_update_sql(this->sqlstr, MOLE2_TASK_NOT_ONGOING_ERR);
}

int Ctask::task_del(userid_t userid, uint32_t taskid)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where %s=%u and %s=%u",
			this->get_table_name(userid),
			STR_UID,	userid,
			STR_TASKID, taskid);
	return this->exec_update_sql(this->sqlstr, MOLE2_TASK_NOFIND_ERR);
}


int Ctask::task_add(userid_t userid, uint32_t taskid)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s(%s,%s,%s,%s,%s,%s,%s,%s,%s) \
				values(%u,%u,0,1,%u,0,0,0,0)",
				this->get_table_name(userid),
				STR_UID,
				STR_TASKID,
				STR_NODE,
				STR_STATE,
				STR_OPTDATE,
				STR_FINTIME,
				STR_FINNUM,
				STR_CLIBUF,
				STR_SERBUF,
				userid,
				taskid,
				(uint32_t)time(NULL));
	return this->exec_insert_sql(this->sqlstr, MOLE2_TASK_EXISTED_ERR);

}

int Ctask::set_field_value(uint32_t userid, su_mole2_set_field_value_in* p_in){
	char mysql_value[mysql_str_len(sizeof(p_in->value))];
	set_mysql_string(mysql_value, p_in->value, sizeof(p_in->value));

	GEN_SQLSTR(this->sqlstr, "update %s set %s = '%s' where userid = %u and taskid = %u", 
				this->get_table_name(userid), p_in->field,	mysql_value, 
				userid,p_in->opt_id);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Ctask::fin_time_get(uint32_t userid, uint32_t taskid, uint32_t* p_time)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u and %s=%u", 			
			STR_FINTIME,
			this->get_table_name(userid), 
			STR_UID,		userid,
			STR_TASKID,		taskid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_TASK_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_time);
	STD_QUERY_ONE_END();
}

int Ctask::set_state(uint32_t userid, uint32_t taskid, uint32_t state)
{
	uint32_t day = today();
	if(state == 1) {
		day -= 3600 * 24;
	}
	GEN_SQLSTR(this->sqlstr, "update %s set state=%u,fin_time=%u,fin_num=%u where userid=%u and taskid=%u",
		this->get_table_name(userid),state,day,1,userid,taskid);
	return this->exec_update_sql(this->sqlstr, MOLE2_TASK_NOFIND_ERR);
}

int Ctask::get_tasks_done(userid_t userid, std::vector<stru_task_done> & tasks)
{
	GEN_SQLSTR(this->sqlstr, "select "
		"taskid,node,state,optdate,fin_time,fin_num "
		"from %s where userid = %u and state != 1",this->get_table_name(userid), userid);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, tasks);
		INT_CPY_NEXT_FIELD(item.taskid);
		INT_CPY_NEXT_FIELD(item.node);
		INT_CPY_NEXT_FIELD(item.state);
		INT_CPY_NEXT_FIELD(item.optdate);
		INT_CPY_NEXT_FIELD(item.fin_time);
		INT_CPY_NEXT_FIELD(item.fin_num);
	STD_QUERY_WHILE_END_NEW();
}

int Ctask::get_tasks_doing(userid_t userid, std::vector<stru_task_doing> & tasks)
{
	GEN_SQLSTR(this->sqlstr, "select "
		"taskid,node,state,optdate,fin_time,fin_num,cli_buf,ser_buf "
		"from %s where userid = %u and state = 1",this->get_table_name(userid), userid);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, tasks);
		memset(item.cli_buf,0,sizeof(item.cli_buf));
		memset(item.ser_buf,0,sizeof(item.ser_buf));
		INT_CPY_NEXT_FIELD(item.taskid);
		INT_CPY_NEXT_FIELD(item.node);
		INT_CPY_NEXT_FIELD(item.state);
		INT_CPY_NEXT_FIELD(item.optdate);
		INT_CPY_NEXT_FIELD(item.fin_time);
		INT_CPY_NEXT_FIELD(item.fin_num);
		BIN_CPY_NEXT_FIELD(item.cli_buf, sizeof(item.cli_buf));
		BIN_CPY_NEXT_FIELD(item.ser_buf, sizeof(item.ser_buf));
	STD_QUERY_WHILE_END_NEW();
}



