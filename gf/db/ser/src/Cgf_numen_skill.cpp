#include <algorithm>
#include "Cgf_numen_skill.h"
#include "db_error.h"

using namespace std;
//加入
Cgf_numen_skill::Cgf_numen_skill(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_numen_skill","userid")
{ 

}

int Cgf_numen_skill::get_numen_skill_list(userid_t userid, uint32_t usertm, 
    uint32_t numen_id, uint32_t* p_count, numen_skill_t** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select skill_id, skill_lv from %s \
        where userid=%u and role_regtime=%u and numen_id=%u;",
		this->get_table_name(userid),userid, usertm, numen_id);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->lv );
	STD_QUERY_WHILE_END();
}

int Cgf_numen_skill::add_numen_skill(userid_t userid, uint32_t usertm, uint32_t numen_id, uint32_t id, uint32_t lv)
{
	GEN_SQLSTR( this->sqlstr, "replace into %s (userid, role_regtime, numen_id, skill_id, skill_lv) \
        values (%u, %u, %u, %u, %u);" ,
		this->get_table_name(userid), userid, usertm, numen_id, id, lv);
	return this->exec_insert_sql (this->sqlstr, GF_SUMMON_EXISTED_ERR);	
}

int Cgf_numen_skill::del_numen_skill(userid_t userid, uint32_t usertm, uint32_t numen_id, uint32_t id, uint32_t lv)
{
    GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and role_regtime=%u \
        and numen_id=%u and skill_id=%u and skill_lv=%u",
        this->get_table_name(userid), userid, usertm, numen_id, id, lv);

	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);
}

int Cgf_numen_skill::update_numen_skill(userid_t userid, uint32_t usertm, uint32_t numen_id, uint32_t sid, uint32_t id, uint32_t lv)
{
	GEN_SQLSTR(this->sqlstr,"update %s set skill_id=%u, skill_lv=%u \
        where userid=%u and role_regtime=%u and numen_id=%u and skill_id=%u;",
			this->get_table_name(userid), id, lv, userid, usertm, numen_id, sid);
	return this->exec_update_sql( this->sqlstr, GF_SUMMON_NOFIND_ERR);

}

