#include <time.h> 
#include <algorithm>
#include "db_error.h"
#include "benchapi.h"
#include "Cgf_master.h"



//public:
Cgf_master::Cgf_master(mysql_interface * db)
	:CtableRoute(db,"GF" ,"t_gf_master_apprentice","userid") 
{ 
}

int Cgf_master::get_member_list_by_type(userid_t userid, uint32_t roletm, uint32_t type, mp_member_t **plist, uint32_t *pcount)
{
    GEN_SQLSTR(this->sqlstr,"select uid, roletm, tm, lv, grade from %s \
        where userid=%u and role_regtime=%u and relation=%u;",
        this->get_table_name(userid), userid, roletm, type);
    
    STD_QUERY_WHILE_BEGIN(this->sqlstr, plist, pcount);
		INT_CPY_NEXT_FIELD( (*plist+i)->uid );
		INT_CPY_NEXT_FIELD( (*plist+i)->roletm );
		INT_CPY_NEXT_FIELD( (*plist+i)->tm );
		INT_CPY_NEXT_FIELD( (*plist+i)->lv );
		INT_CPY_NEXT_FIELD( (*plist+i)->grade );
	STD_QUERY_WHILE_END();
}

int Cgf_master::gf_get_master_info(userid_t userid, uint32_t roletm, uint32_t *p_id, uint32_t *p_tm)
{
    GEN_SQLSTR(this->sqlstr,"select uid, roletm from %s \
        where userid=%u and role_regtime=%u and relation=%u;",
        this->get_table_name(userid), userid, roletm, RELATION_TYPE_MASTER);
    
    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *p_id );
		INT_CPY_NEXT_FIELD( *p_tm );
	STD_QUERY_ONE_END();
}

int Cgf_master::add_master(userid_t userid, uint32_t roletm, uint32_t apprentice_id, uint32_t apprentice_tm, uint32_t tm)
{
    GEN_SQLSTR(this->sqlstr,"insert into %s (userid, role_regtime, uid, roletm, relation, tm) values \
        (%u, %u, %u, %u, %u, %u)", this->get_table_name(userid), 
        userid, roletm, apprentice_id, apprentice_tm, RELATION_TYPE_MASTER, tm);
	return this->exec_insert_sql (this->sqlstr, GF_FRIENDID_EXISTED_ERR);
}

int Cgf_master::add_apprentice(userid_t userid, uint32_t roletm, gf_master_add_apprentice_in *p_in)
{
    GEN_SQLSTR(this->sqlstr,"insert into %s (userid, role_regtime, uid, roletm, tm, lv, grade, relation) \
        values (%u, %u, %u, %u, %u, %u, %u, %u)",
		this->get_table_name(userid), userid, roletm, p_in->uid, p_in->roletm, 
        p_in->tm, p_in->prentice_lv, p_in->grade, RELATION_TYPE_PRENTICE);
	return this->exec_insert_sql (this->sqlstr, GF_FRIENDID_EXISTED_ERR);
}

int Cgf_master::del_master(userid_t userid, uint32_t roletm, uint32_t apprentice_id, uint32_t apprentice_tm)
{
    GEN_SQLSTR( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u \
        and uid=%u and roletm=%u and relation=%u",
        this->get_table_name(userid), userid, roletm, apprentice_id, apprentice_tm, RELATION_TYPE_MASTER);
    return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR );	
}

int Cgf_master::del_apprentice(userid_t userid, uint32_t roletm, uint32_t apprentice_id, uint32_t apprentice_tm)
{
    GEN_SQLSTR( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u \
        and uid=%u and roletm=%u and relation=%u",
        this->get_table_name(userid), userid, roletm, apprentice_id, apprentice_tm, RELATION_TYPE_PRENTICE);
    return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR );	
}

int Cgf_master::set_int_value(userid_t userid, uint32_t roletm, 
    uint32_t apprentice_id, uint32_t apprentice_tm, uint32_t value, const char * col_name)
{
    GEN_SQLSTR( this->sqlstr, "update %s set %s=%u where userid=%u and role_regtime=%u \
        and uid=%u and roletm=%u and relation=%u",
        this->get_table_name(userid), col_name, value, userid, roletm, 
        apprentice_id, apprentice_tm, RELATION_TYPE_PRENTICE);
    return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR );	
}

int Cgf_master::get_prentice_grade(userid_t userid, uint32_t roletm, uint32_t p_id, uint32_t p_tm, uint32_t *value)
{
    GEN_SQLSTR(this->sqlstr,"select grade from %s \
        where userid=%u and role_regtime=%u and uid=%u and roletm=%u and relation=%u;",
        this->get_table_name(userid), userid, roletm, p_id, p_tm, RELATION_TYPE_PRENTICE);
    
    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *value );
	STD_QUERY_ONE_END();
}

int Cgf_master::update_grade(userid_t userid, uint32_t roletm, 
    uint32_t apprentice_id, uint32_t apprentice_tm, uint32_t value)
{
    GEN_SQLSTR( this->sqlstr, "update %s set grade=%u where userid=%u and role_regtime=%u \
        and uid=%u and roletm=%u and relation=%u",
        this->get_table_name(userid), value, userid, roletm, 
        apprentice_id, apprentice_tm, RELATION_TYPE_PRENTICE);
    return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR );	
}

int Cgf_master::get_prentice_cnt(userid_t userid, uint32_t roletm, uint32_t *pcount)
{
    GEN_SQLSTR(this->sqlstr,"select count(*) from %s \
        where userid=%u and role_regtime=%u and relation=%u;",
        this->get_table_name(userid), userid, roletm, RELATION_TYPE_PRENTICE);
    
    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *pcount );
	STD_QUERY_ONE_END();
}

int Cgf_master::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}


