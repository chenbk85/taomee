#include <algorithm>
#include "Cgf_decorate.h"
#include "db_error.h"

using namespace std;
//加入
Cgf_decorate::Cgf_decorate(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_decorate","userid")
{ 

}

int Cgf_decorate::get_decorate_list(userid_t userid, uint32_t role_regtime, uint32_t* p_count, gf_get_decorate_list_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select decorate_id, decorate_tm, decorate_lv, status \
        from %s where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), userid, role_regtime);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->decorate_id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->decorate_tm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->decorate_lv );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->status );
	STD_QUERY_WHILE_END();
}

int Cgf_decorate::set_decorate(userid_t userid, uint32_t role_regtime, uint32_t id, uint32_t tm)
{
	GEN_SQLSTR( this->sqlstr, "insert into %s (userid, role_regtime, decorate_id, decorate_tm, decorate_lv) \
        values (%u, %u, %u, %u, 1);" ,
		this->get_table_name(userid), userid, role_regtime, id, tm);
	return this->exec_insert_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);	
}

int Cgf_decorate::del_decorate(userid_t userid,uint32_t role_regtime, uint32_t id)
{
    GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and decorate_id=%u",
        this->get_table_name(userid), userid, role_regtime, id);

	return this->exec_update_sql(this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_decorate::add_decorate_lv(userid_t userid , uint32_t role_regtime, uint32_t id, uint32_t addlv)
{
    GEN_SQLSTR(this->sqlstr,"update %s set decorate_lv=decorate_lv+%u \
        where userid=%u and role_regtime=%u and decorate_id=%u;",
        this->get_table_name(userid), addlv, userid, role_regtime, id);
    return this->exec_update_list_sql( this->sqlstr, SUCC);
}

int Cgf_decorate::set_decorate_status(userid_t userid , uint32_t role_regtime, uint32_t id, uint32_t status)
{
    GEN_SQLSTR(this->sqlstr,"update %s set status=%u \
        where userid=%u and role_regtime=%u and decorate_id=%u;",
        this->get_table_name(userid), status, userid, role_regtime, id);
    return this->exec_update_list_sql( this->sqlstr, SUCC);
}


