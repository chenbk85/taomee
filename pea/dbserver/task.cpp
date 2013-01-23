#include <algorithm>
#include "task.hpp"
#include "pea_common.hpp"

task::task(mysql_interface* db)
	:CtableRoute100x10(db, "pea", "pea_task", "user_id")
{
		
}

int task::add( db_user_id_t db_user_id,  uint32_t taskid, uint32_t buf_len, char *buff)
{
    if (buf_len>TASK_BUF_LEN  ) return   NO_DEFINE_ERR;
    char buf_mysql[mysql_str_len(TASK_BUF_LEN )];
    set_mysql_string(buf_mysql, buff , buf_len );

    GEN_SQLSTR(sqlstr, "insert into %s (user_id,role_tm,server_id  ,tskid, flag , buff)values(%u, %u,%u,%u, 0, '%s')", 
			get_table_name(db_user_id.user_id), db_user_id.user_id,
				db_user_id.role_tm,db_user_id.server_id,
			taskid, buf_mysql);
    return exec_insert_sql(sqlstr, DB_ERR_EXIST );
}

int task::del( db_user_id_t db_user_id,  uint32_t taskid )
{
    GEN_SQLSTR(sqlstr, "delete from %s where user_id=%u and role_tm=%u and server_id =%u  and tskid=%u ", 
			get_table_name(db_user_id.user_id), db_user_id.user_id,db_user_id.role_tm ,db_user_id.server_id,taskid );
    return exec_delete_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int task::update( db_user_id_t db_user_id,  uint32_t taskid, uint32_t buf_len, char *buff, uint32_t flag)
{
    if (buf_len>TASK_BUF_LEN  ) return   NO_DEFINE_ERR;
    char buf_mysql[mysql_str_len(TASK_BUF_LEN )];
    set_mysql_string(buf_mysql, buff , buf_len );

    GEN_SQLSTR(sqlstr, "update %s set flag=%u, buff='%s' where user_id=%u and role_tm=%u and server_id =%u  and tskid=%u ", 
			get_table_name(db_user_id.user_id), flag,buf_mysql, 
            db_user_id.user_id,db_user_id.role_tm,db_user_id.server_id, taskid );
    return exec_delete_sql(sqlstr, DB_ERR_NOT_EXIST);
}


int task::update_buf( db_user_id_t db_user_id,  uint32_t taskid, uint32_t buf_len ,char * buff )
{
	if (buf_len>TASK_BUF_LEN  ) return   NO_DEFINE_ERR;

    char buf_mysql[mysql_str_len(TASK_BUF_LEN )];
    set_mysql_string(buf_mysql, buff , buf_len );
	
    GEN_SQLSTR(sqlstr, "update %s set buff='%s' where user_id=%u and role_tm=%u and server_id =%u  and tskid=%u ", 
			get_table_name(db_user_id.user_id), buf_mysql,db_user_id.user_id,db_user_id.role_tm,db_user_id.server_id, taskid );
    return exec_delete_sql(sqlstr, DB_ERR_NOT_EXIST);
}



int task::get_flag_list( db_user_id_t db_user_id,uint32_t begin,uint32_t end, std::vector<stru_task_flag > &task_list )
{
    GEN_SQLSTR(this->sqlstr, "select tskid,flag from %s where user_id=%u and role_tm=%u and server_id =%u  and tskid>=%u and tskid<= %u ",
            this->get_table_name(db_user_id.user_id),db_user_id.user_id,db_user_id.role_tm,db_user_id.server_id, begin,end );
	stru_task_flag item;
    STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr,task_list);
        INT_CPY_NEXT_FIELD(item.tskid);
        INT_CPY_NEXT_FIELD(item.flag);
    STD_QUERY_WHILE_END_NEW();
}

int task::get_full_list( db_user_id_t db_user_id,uint32_t begin,uint32_t end, std::vector<stru_task_info > &task_list )
{
    GEN_SQLSTR(this->sqlstr, "select tskid,flag,buff from %s where user_id=%u and role_tm=%u and server_id =%u and tskid>=%u and tskid<= %u ",
            this->get_table_name(db_user_id.user_id),db_user_id.user_id, db_user_id.role_tm,db_user_id.server_id, begin,end );
	stru_task_info item;
    STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr,task_list);
        INT_CPY_NEXT_FIELD(item.tskid);
        INT_CPY_NEXT_FIELD(item.flag);
        BIN_CPY_NEXT_FIELD(item.buff, sizeof(item.buff ) );
    STD_QUERY_WHILE_END_NEW();
}

