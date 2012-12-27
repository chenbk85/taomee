#include <algorithm>
#include "db_error.h"
#include "Cgf_secondary_pro.h"

using namespace std;

Cgf_secondary_pro::Cgf_secondary_pro(mysql_interface * db)
	:CtableRoute( db,"GF" ,"t_gf_secondary_pro","userid")
{

}


int Cgf_secondary_pro::get_player_secondary_pro_info(userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t* exp)
{
	*exp = 0;
	GEN_SQLSTR(sqlstr, "select exp from %s where userid = %u and role_regtime = %u and type = %u", 
			get_table_name(userid), userid, role_regtime, type);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_SECONDARY_PRO_NOT_EXIST);
            INT_CPY_NEXT_FIELD( *exp );
	STD_QUERY_ONE_END();	
}


int Cgf_secondary_pro::add_player_secondary_pro_exp(userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t exp, uint32_t max_exp_limit)
{
	uint32_t cur_exp = 0;
	int ret = get_player_secondary_pro_info(userid, role_regtime, type, &cur_exp);
	if(ret != SUCC){
		return ret;
	}
	if( cur_exp > max_exp_limit)
	{
		cur_exp = max_exp_limit;
	}

	if(exp + cur_exp > max_exp_limit)
	{
		exp = max_exp_limit - cur_exp;
	}
	GEN_SQLSTR(sqlstr, "update %s set exp = exp + %u where userid = %u and role_regtime = %u and type = %u",
			get_table_name(userid), exp, userid, role_regtime, type);
	return exec_update_sql(sqlstr, GF_SECONDARY_PRO_NOT_EXIST);
}

int Cgf_secondary_pro::insert_player_secondary_pro(userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t exp)
{
	GEN_SQLSTR(sqlstr, "insert into %s values(%u, %u, %u, %u)",
			 get_table_name(userid), userid, role_regtime, type, exp);
	return exec_insert_sql(sqlstr, GF_SECONDARY_PRO_EXIST);
}

int Cgf_secondary_pro::get_player_secondary_pro_list(userid_t userid, uint32_t role_regtime, gf_get_secondary_pro_list_out_type** pData, uint32_t* count)
{
	GEN_SQLSTR(sqlstr, "select type, exp from %s where userid = %u and role_regtime = %u",
			 get_table_name(userid), userid, role_regtime);	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pData, count);
		INT_CPY_NEXT_FIELD( (*pData+i)->pro );
		INT_CPY_NEXT_FIELD( (*pData+i)->exp );
	STD_QUERY_WHILE_END();
}

int Cgf_secondary_pro::clear_player_secondary_pro(userid_t userid, uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), userid, role_regtime);
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_secondary_pro::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}


int Cgf_secondary_pro::update_player_secondary_pro(userid_t userid, uint32_t role_regtime, uint32_t type, uint32_t exp)
{
	GEN_SQLSTR(sqlstr, "update %s set exp = %u where userid = %u and role_regtime = %u and type = %u",
			get_table_name(userid), exp, userid, role_regtime, type);		
	return exec_update_sql(sqlstr, GF_SECONDARY_PRO_NOT_EXIST);	
}
