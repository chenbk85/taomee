#include <algorithm>
#include "db_error.h"
#include "Cgf_other_info.h"


Cgf_other_info::Cgf_other_info(mysql_interface * db)
	:CtableRoute( db,"GF" ,"t_gf_other_info","userid")
{

}

int Cgf_other_info::get_other_info_list(userid_t userid, uint32_t role_regtime, gf_get_other_info_list_out_item** pData, uint32_t* count)
{
	GEN_SQLSTR(sqlstr, "select type, value from %s where userid = %u and role_regtime = %u order by type",
			get_table_name(userid), userid,  role_regtime);
	STD_QUERY_WHILE_BEGIN(sqlstr, pData, count);
		INT_CPY_NEXT_FIELD( (*pData+i)->type);
		INT_CPY_NEXT_FIELD( (*pData+i)->value);
	STD_QUERY_WHILE_END();
}

int Cgf_other_info::replace_other_info(userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t value)
{
	time_t now = time (NULL);
	GEN_SQLSTR(sqlstr, "replace into %s (userid, role_regtime, type, value, tm) values(%u, %u, %u, %u, %u)", 
				      get_table_name(userid), 
					  userid,  
					  role_regtime,
					  type,
					  value,
					  (uint32_t)now);
	return exec_update_sql(sqlstr, SUCC);
}

int Cgf_other_info::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}


