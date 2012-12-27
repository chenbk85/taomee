#include "Cgf_role_deleted.h"
#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "msglog.h"
#include <set>
//dudu 
Cgf_role_deleted::Cgf_role_deleted(mysql_interface * db)
	:CtableRoute10( db , "DELUSER" , "t_gf_role_deleted" , "userid") 
{ 

}

int Cgf_role_deleted::add_role_deleted(userid_t userid, uint32_t role_regtime)
{
	GEN_SQLSTR( this->sqlstr, "insert into %s (userid,role_regtime,del_time ) \
		values (%u,%u,UNIX_TIMESTAMP(NOW()));",
		this->get_table_name(userid),userid,role_regtime);
	STD_INSERT_RETURN(this->sqlstr, DB_ERR);

}

int Cgf_role_deleted::del_role_deleted(userid_t userid, uint32_t role_regtime)
{
	GEN_SQLSTR( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;",
		this->get_table_name(userid),userid,role_regtime);
	STD_SET_RETURN(this->sqlstr, ROLE_ID_NOFIND_ERR);	
}

int Cgf_role_deleted::get_list_role_deleted(userid_t userid, uint32_t* p_count,gf_role_deleted_stru** pp_list)
{
	GEN_SQLSTR( this->sqlstr, "select role_regtime,del_time from %s where userid=%u  order by del_time;",
		this->get_table_name(userid),userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->role_regtime );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->del_time );
	STD_QUERY_WHILE_END();
}

int Cgf_role_deleted::get_list_all_role_deleted(char* dbname,uint32_t del_time, uint32_t* p_count,gf_role_deleted_stru** pp_list)
{
	GEN_SQLSTR( this->sqlstr, "select userid,role_regtime,del_time from %s where del_time<%u order by del_time;",
		dbname,del_time);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->userid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->role_regtime );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->del_time );
	STD_QUERY_WHILE_END();
}



