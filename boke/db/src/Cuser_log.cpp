/*
 * =========================================================================
 *
 *        Filename: Cuser_log.cpp
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

#include    "Cuser_log.h"
#include <time.h>
Cuser_log::Cuser_log(mysql_interface * db ) 
	:CtableRoute100x10(db, "POP" , "t_user_log" , "userid" )
{

}
	
int	Cuser_log::insert(userid_t userid, user_log_t * p_in )
{
	sprintf( this->sqlstr, "insert into %s values( %u,%u, %u ,%u)",
		this->get_table_name(userid), userid,p_in->logtime,p_in->v1,p_in->v2 );
	return this->exec_insert_sql( this->sqlstr, USER_LOG_ADD_ERR );
}


int	Cuser_log::get_list(userid_t userid, std::vector<user_log_t> &user_log_list )
{
    GEN_SQLSTR(this->sqlstr, "select logtime,v1,v2 from %s where userid=%u ",
            this->get_table_name(userid),userid);
    STD_QUERY_WHILE_BEGIN(this->sqlstr,user_log_list);
        INT_CPY_NEXT_FIELD(item.logtime);
        INT_CPY_NEXT_FIELD(item.v1);
        INT_CPY_NEXT_FIELD(item.v2);
    STD_QUERY_WHILE_END();
}

