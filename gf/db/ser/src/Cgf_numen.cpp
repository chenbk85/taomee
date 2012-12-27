#include <algorithm>
#include "Cgf_numen.h"
#include "db_error.h"

using namespace std;
//加入
Cgf_numen::Cgf_numen(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_numen","userid")
{ 

}

int Cgf_numen::get_numen_list(userid_t userid, uint32_t usertm, uint32_t* p_count, gf_get_numen_list_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select numen_id, name, status \
        from %s where userid=%u and role_regtime=%u;",
		this->get_table_name(userid),userid, usertm);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->numen_type );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->name, sizeof((*pp_list+i)->name) );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->status );
	STD_QUERY_WHILE_END();
}

int Cgf_numen::add_numen(userid_t userid, uint32_t usertm, uint32_t numen_type, char* name)
{
    char nick_mysql[mysql_str_len(NICK_LEN)];
    set_mysql_string(nick_mysql, name, NICK_LEN);
	GEN_SQLSTR( this->sqlstr, "insert into %s (userid, role_regtime, numen_id, name) \
        values ( %u, %u, %u, '%s');" ,
		this->get_table_name(userid), userid, usertm, numen_type, nick_mysql);
	return this->exec_insert_sql (this->sqlstr, GF_SUMMON_EXISTED_ERR);	
}

int Cgf_numen::update_numen_nick(userid_t userid , uint32_t usertm, uint32_t numen_type, char* nick)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, nick, NICK_LEN);
	GEN_SQLSTR(this->sqlstr,"update %s set name='%s' where userid=%u and role_regtime=%u and numen_id=%u;",
			this->get_table_name(userid), nick_mysql, userid, usertm, numen_type);
	return this->exec_update_sql( this->sqlstr, GF_SUMMON_NOFIND_ERR);

}


int Cgf_numen::set_int_value(userid_t userid, uint32_t usertm, uint32_t numen_type, const char* column_type, uint32_t value)
{
	GEN_SQLSTR( this->sqlstr, "update %s set %s=%u \
        where userid=%u and usertm=%u and numen_id=%u" ,
		this->get_table_name(userid), column_type, value, userid, usertm, numen_type);
	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);	
}


int Cgf_numen::set_numen_status(userid_t userid, uint32_t usertm, uint32_t numen_type, uint32_t status)
{
    GEN_SQLSTR(this->sqlstr, "update %s set status=%u where userid=%u and role_regtime=%u and numen_id=%u",
        this->get_table_name(userid), status, userid, usertm, numen_type);

	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);
}

int Cgf_numen::set_role_numen_status(userid_t userid, uint32_t usertm, uint32_t status)
{
    GEN_SQLSTR(this->sqlstr, "update %s set status=%u where userid=%u and role_regtime=%u",
        this->get_table_name(userid), status, userid, usertm);

	return this->exec_update_list_sql(this->sqlstr, ROLE_ID_NOFIND_ERR);
}

