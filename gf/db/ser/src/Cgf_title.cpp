#include <algorithm>
#include "db_error.h"
#include "Cgf_title.h"




Cgf_title::Cgf_title( mysql_interface * db )
	:CtableRoute( db,"GF" ,"t_gf_title","userid")
{

}

int Cgf_title::get_achieve_title(userid_t userid, uint32_t role_regtime, gf_get_achieve_title_out_item** pdata, uint32_t *count)
{
	GEN_SQLSTR(sqlstr, "select title_type, subid, get_time from %s where userid = %u and role_regtime = %u",
			            get_table_name(userid), userid,  role_regtime);
	STD_QUERY_WHILE_BEGIN(sqlstr, pdata, count);
		INT_CPY_NEXT_FIELD( (*pdata+i)->type);
		INT_CPY_NEXT_FIELD( (*pdata+i)->subid);
		INT_CPY_NEXT_FIELD( (*pdata+i)->get_time);
	STD_QUERY_WHILE_END();
}
/*
int Cgf_title::get_achieve_title_by_subid(userid_t userid, uint32_t role_regtime, uint32_t subid)
{

}*/

int Cgf_title::add_achieve_title(userid_t userid, uint32_t role_regtime, uint8_t type, uint32_t subid, uint32_t get_time)
{
	GEN_SQLSTR(sqlstr, "insert into %s values(%u, %u, %u, %u, %u)",
					get_table_name(userid), userid, role_regtime, type, subid, get_time);
	return exec_update_sql(sqlstr, SUCC);
}

int Cgf_title::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}


