/*
 * =====================================================================================
 *
 *       Filename:  Cuser_note.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010-07-21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#include "Cuser_note.h"


#define STR_USERID	"userid"
#define STR_JOB		"job_dream"
#define STR_BRAVE	"brave_dream"


Cuser_note::Cuser_note(mysql_interface * db ) 
	 :CtableRoute100x10(db , "MOLE2_USER" , "t_user_note" , "userid")
{

}

int Cuser_note::insert(uint32_t userid, char* job_dream, char* brave_dream)
{
	char job_dream_mysql[mysql_str_len(NOTE_LEN)];
	char brave_dream_mysql[mysql_str_len(NOTE_LEN)];
	set_mysql_string(job_dream_mysql, job_dream, NOTE_LEN);
	set_mysql_string(brave_dream_mysql, brave_dream, NOTE_LEN);

	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u, '%s', '%s')",
		this->get_table_name(userid),
		userid,
		job_dream_mysql,
		brave_dream_mysql
	);
	return this->exec_insert_sql( this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_note::brave_dream_set(uint32_t userid, char* job_dream, char* brave_dream)
{
//	log_buf("job_dream_set", (uint8_t*)job_dream, NOTE_LEN);
//	log_buf("brave_dream_set", (uint8_t*)brave_dream, NOTE_LEN);
	
	char job_dream_mysql[mysql_str_len(NOTE_LEN)];
	char brave_dream_mysql[mysql_str_len(NOTE_LEN)];
	set_mysql_string(job_dream_mysql, job_dream, NOTE_LEN);
	set_mysql_string(brave_dream_mysql, brave_dream, NOTE_LEN);

	GEN_SQLSTR(this->sqlstr, "update %s set %s='%s', %s='%s' where %s=%u",
		this->get_table_name(userid),
		STR_JOB,		job_dream_mysql,
		STR_BRAVE,		brave_dream_mysql,
		STR_USERID,		userid
	);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}
	

 

