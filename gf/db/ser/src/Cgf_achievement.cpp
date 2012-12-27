#include <algorithm>
#include "db_error.h"
#include "Cgf_achievement.h"




Cgf_achievement::Cgf_achievement( mysql_interface * db )
	:CtableRoute( db,"GF" ,"t_gf_achievement","userid")
{

}

int Cgf_achievement::get_achievement_data_list( userid_t userid, uint32_t role_regtime, gf_get_achievement_data_list_out_element** pdata, uint32_t *count)
{
	GEN_SQLSTR(sqlstr, "select achieve_type, get_time from %s where userid = %u and role_regtime = %u",
			            get_table_name(userid), userid,  role_regtime);
	STD_QUERY_WHILE_BEGIN(sqlstr, pdata, count);
		INT_CPY_NEXT_FIELD( (*pdata+i)->achieve_type);
		INT_CPY_NEXT_FIELD( (*pdata+i)->get_time);
	STD_QUERY_WHILE_END();
}


int Cgf_achievement::replace_achievement_data(userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t get_time)
{
	GEN_SQLSTR(sqlstr, "replace into %s values(%u, %u, %u, %u)",
					get_table_name(userid), 
					userid,  
					role_regtime,
					type,
					get_time
				);
	return exec_update_sql(sqlstr, SUCC);
}

int Cgf_achievement::clear_role_achievement(userid_t userid, uint32_t role_regtime)
{
	GEN_SQLSTR(sqlstr, "delete from %s where userid=%u and role_regtime=%u",
					get_table_name(userid), 
					userid,  
					role_regtime);
	return exec_update_sql(sqlstr, SUCC);
}