/*
 * =====================================================================================
 *
 *       Filename:  Cmonster_handbook.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/03/2010 04:23:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Ctask_handbook.h"


#define STR_USERID	"userid"
#define STR_TASKID	"taskid"
#define STR_CONTENT		"content"



Ctask_handbook::Ctask_handbook(mysql_interface* db) :
	CtableRoute(db, "MOLE2_USER", "t_task_handbook", "userid")
{

}

int Ctask_handbook::insert(uint32_t userid,
	uint32_t taskid,
	const char* content)
{
	char content_mysql[mysql_str_len(NOTE_LEN)];
	set_mysql_string(content_mysql, content, NOTE_LEN);
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u, %u, '%s')",
			this->get_table_name(userid),
			userid,
			taskid,
			content_mysql);
	return this->exec_insert_sql( this->sqlstr, MOLE2_TASK_EXISTED_ERR);
}

int Ctask_handbook::content_set(uint32_t userid, uint32_t taskid, const char* content)
{
	char content_mysql[mysql_str_len(NOTE_LEN)];
	set_mysql_string(content_mysql, content, NOTE_LEN);
	
	GEN_SQLSTR(this->sqlstr, "update %s set %s = '%s' where %s = %u and %s = %u", 
			this->get_table_name(userid),
			STR_CONTENT,	content_mysql,
			STR_USERID, userid,
			STR_TASKID, taskid);
	return this->exec_update_sql(this->sqlstr, MOLE2_TASK_NOFIND_ERR);
}


int Ctask_handbook::content_get(uint32_t userid, uint32_t taskid, char* sz_content)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u and %s=%u", 
			STR_CONTENT,
			this->get_table_name(userid), 
			STR_USERID,		userid,
			STR_TASKID,	taskid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_TASK_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(sz_content, NOTE_LEN);
	STD_QUERY_ONE_END();
}


