/*
 * =========================================================================
 *
 *        Filename: Ctask.cpp
 *
 *        Version:  1.0
 *        Created:  2011-05-13 15:52:03
 *        Description:   
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#include    "Ctask.h"
#include <time.h>
Ctask::Ctask(mysql_interface * db ) 
	:CtableRoute100x10(db, "POP" , "t_task" , "userid", "taskid")
{

}
	
int	Ctask::insert(userid_t userid,  uint32_t taskid , uint32_t task_nodeid)
{
	sprintf( this->sqlstr, "insert into %s values( %u,%u, %u )",
		this->get_table_name(userid), userid,taskid,task_nodeid );
	return this->exec_insert_sql( this->sqlstr,TASK_IS_EXISTED_ERR );
}


int	Ctask::get_list(userid_t userid, std::vector<task_t> &task_list )
{
    GEN_SQLSTR(this->sqlstr, "select taskid,task_nodeid from %s where userid=%u ",
            this->get_table_name(userid),userid);
    STD_QUERY_WHILE_BEGIN(this->sqlstr,task_list);
        INT_CPY_NEXT_FIELD(item.taskid);
        INT_CPY_NEXT_FIELD(item.task_nodeid);
    STD_QUERY_WHILE_END();
}

int	Ctask::del(userid_t userid, uint32_t taskid , uint32_t task_nodeid)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and taskid=%u and task_nodeid=%u ",
		this->get_table_name(userid),  userid,taskid,task_nodeid);
	return this->exec_delete_sql( this->sqlstr, ITEM_NOFIND_ERR );
}
