#include <algorithm>
#include "Cgf_summon_skill.h"
#include "db_error.h"

using namespace std;
//加入
Cgf_summon_skill::Cgf_summon_skill(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_summon_skill","userid")
{ 

}

int Cgf_summon_skill::get_summon_skill_list(userid_t userid, uint32_t usertm, 
    uint32_t mon_tm, uint32_t* p_count, sum_skill_t** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select skillid, skilllv from %s \
        where userid=%u and role_regtime=%u and mon_tm=%u;",
		this->get_table_name(userid),userid, usertm, mon_tm);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->skillid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->skilllv );
	STD_QUERY_WHILE_END();
}

int Cgf_summon_skill::add_summon_skill(userid_t userid, uint32_t usertm, uint32_t mon_tm, uint32_t id, uint32_t lv)
{
	GEN_SQLSTR( this->sqlstr, "replace into %s (userid, role_regtime, mon_tm, skillid, skilllv) \
        values (%u, %u, %u, %u, %u);" ,
		this->get_table_name(userid), userid, usertm, mon_tm, id, lv);
	return this->exec_insert_sql (this->sqlstr, GF_SUMMON_EXISTED_ERR);	
}

int Cgf_summon_skill::del_summon_skill(userid_t userid,uint32_t usertm, uint32_t mon_tm, uint32_t id)
{
    GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and mon_tm=%u and skillid=%u",
        this->get_table_name(userid), userid, usertm, mon_tm, id);

	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);
}

int Cgf_summon_skill::update_summon_skill(userid_t userid, uint32_t usertm, uint32_t mon_tm, uint32_t sid, uint32_t id, uint32_t lv)
{
	GEN_SQLSTR(this->sqlstr,"update %s set skillid=%u, skilllv=%u \
        where userid=%u and role_regtime=%u and mon_tm=%u and skillid=%u;",
			this->get_table_name(userid), id, lv, userid, usertm, mon_tm, sid);
	return this->exec_update_sql( this->sqlstr, GF_SUMMON_NOFIND_ERR);

}

int Cgf_summon_skill::clear_summon_skill(userid_t userid,uint32_t usertm, uint32_t mon_tm)
{
    GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and mon_tm=%u",
        this->get_table_name(userid), userid, usertm, mon_tm);

	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);
}

int Cgf_summon_skill::get_summon_skill_cnt(userid_t userid, uint32_t usertm, uint32_t mon_tm, uint32_t* db_num)
{
    GEN_SQLSTR(this->sqlstr, "select count(*) from %s where userid=%u and role_regtime=%u and mon_tm=%u",
        this->get_table_name(userid), userid, usertm, mon_tm);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, GF_SUMMON_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*db_num );
    STD_QUERY_ONE_END();
}

int Cgf_summon_skill::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}


