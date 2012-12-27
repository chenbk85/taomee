#include <algorithm>
#include "db_error.h"
#include "Cgf_home_log.h"

using namespace std;
Cgf_home_log::Cgf_home_log(mysql_interface * db )
	    :CtableRoute( db,"GF" ,"t_gf_home_log","userid")
{

}

int Cgf_home_log::get_home_log(userid_t userid, uint32_t role_regtime, uint32_t* p_count, gf_get_home_log_out_item** pp_list)
{
	GEN_SQLSTR(sqlstr, "select op_uid, op_utm, role_type, nick, type, access_type, tm from %s where userid = %u and role_regtime = %u ", 
				get_table_name(userid),
				userid, 
				role_regtime
				);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->op_uid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->op_utm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->role_type );
		BIN_CPY_NEXT_FIELD ((*pp_list+i)->nick, sizeof ((*pp_list+i)->nick));
		INT_CPY_NEXT_FIELD( (*pp_list+i)->type );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->access_type );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->tm );
	STD_QUERY_WHILE_END();
}

int Cgf_home_log::add_log(userid_t userid, uint32_t role_regtime, gf_add_home_log_in* p_in)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, p_in->nick, NICK_LEN);
	GEN_SQLSTR(sqlstr, "insert into %s (userid,role_regtime,op_uid,op_utm,role_type,nick,type,access_type,tm) values \
		(%u, %u, %u, %u, %u, '%s', %u, %u, %u);", 
				get_table_name(userid),
				userid,
				role_regtime,
				p_in->op_uid,
				p_in->op_utm,
				p_in->role_type,
				nick_mysql,
				p_in->type,
				p_in->access_type,
				p_in->tm
			);
	return exec_update_sql(sqlstr, SUCC);
}

int Cgf_home_log::clear_log(userid_t userid, uint32_t role_regtime)
{
	GEN_SQLSTR(sqlstr, "delete from %s where userid = %u and role_regtime =%u and tm < %u;", 
				get_table_name(userid),
				userid,
				role_regtime, uint32_t(time (NULL)) - 6 * 60 * 60);
	return exec_update_sql(sqlstr, SUCC);
}

int Cgf_home_log::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}


