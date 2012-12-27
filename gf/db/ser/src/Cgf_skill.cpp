#include <algorithm>
#include "Cgf_skill.h"
#include "db_error.h"

using namespace std;
//¼ÓÈë
Cgf_skill::Cgf_skill(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_skill","userid")
{ 

}


//int Cgf_skill::add_skill(userid_t userid, uint32_t role_regtime,gf_add_skill_in* p_in)
//{
	//sprintf( this->sqlstr, "insert into %s (userid,role_regtime,skill_id,skill_lv) values \
	//	(%u,%u,%u,%u);" ,
	//	this->get_table_name(userid), userid, role_regtime, p_in->skill_id, p_in->skill_lv );
	//return this->exec_insert_sql (this->sqlstr, GF_SKILL_EXISTED_ERR);	
//	return 0;
//}

int Cgf_skill::add_skill(userid_t userid, uint32_t role_regtime, uint32_t skill_id, uint32_t skill_lv)
{
	sprintf( this->sqlstr, "insert into %s (userid,role_regtime,skill_id,skill_lv) values \
		(%u,%u,%u,%u);" ,
		this->get_table_name(userid), userid, role_regtime, skill_id, skill_lv );
	return this->exec_insert_sql (this->sqlstr, GF_SKILL_EXISTED_ERR);	
}

int Cgf_skill::del_skill(userid_t userid, uint32_t role_regtime,gf_del_skill_in* p_in)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u and skill_id=%u;" ,
		this->get_table_name(userid), userid, role_regtime, p_in->skill_id );
	return this->exec_update_sql(this->sqlstr, GF_SKILL_NOFIND_ERR );	
}

int Cgf_skill::update_skill(userid_t userid, uint32_t role_regtime,gf_upgrade_skill_in* p_in)
{
	sprintf( this->sqlstr, "update %s set skill_lv =%u where userid=%u and role_regtime=%u and skill_id=%u and skill_lv=%u ;" ,
		this->get_table_name(userid), p_in->skill_lv+1, userid, role_regtime, p_in->skill_id, p_in->skill_lv);
	return this->exec_update_sql(this->sqlstr, GF_SKILL_NOFIND_ERR );	
}

int Cgf_skill::upgrade_skill(userid_t userid, uint32_t role_regtime,gf_upgrade_skill_in* p_in)
{
	if (p_in->skill_lv >= max_skill_lv)
		return VALUE_OUT_OF_RANGE_ERR;
	sprintf( this->sqlstr, "update %s set skill_lv =%u where userid=%u and role_regtime=%u and skill_id=%u and skill_lv=%u ;" ,
		this->get_table_name(userid), p_in->skill_lv + 1, userid, role_regtime, p_in->skill_id, p_in->skill_lv);
	return this->exec_update_sql(this->sqlstr, GF_SKILL_NOFIND_ERR );	
}

int Cgf_skill::get_skill_list(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
		gf_get_skill_list_out_item** pp_list)
{
	sprintf( this->sqlstr, "select skill_id,skill_lv,skill_point from %s where userid=%u and role_regtime=%u ;" ,
		this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD ((*pp_list+i)->skill_id);
		INT_CPY_NEXT_FIELD ((*pp_list+i)->skill_lv);
		INT_CPY_NEXT_FIELD ((*pp_list+i)->skill_point);
	STD_QUERY_WHILE_END();
}

int Cgf_skill::get_skill_points(userid_t userid, uint32_t role_regtime, uint32_t* p_count)
{
	sprintf( this->sqlstr, "select sum(skill_point) from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_SKILL_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*p_count);
	STD_QUERY_ONE_END();
}

int Cgf_skill::reset_skills(userid_t userid, uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"update %s set skill_lv =1, skill_point=0 where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), userid, role_regtime);
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_skill::clear_skills(userid_t userid, uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), userid, role_regtime);
	return this->exec_update_list_sql(this->sqlstr, SUCC);
}
#if 0
int Cgf_skill::clear_skill_bind_key(userid_t userid, uint32_t role_regtime)
{
	sprintf( this->sqlstr, "update %s set bind_key =0 where userid=%u and role_regtime=%u ;" ,
		this->get_table_name(userid), userid, role_regtime);
	return this->exec_update_list_sql(sqlstr,SUCC)	;
}

int Cgf_skill::set_bind_key(userid_t userid, uint32_t role_regtime, uint32_t skill_id, 
	uint32_t bind_id)
{
	sprintf( this->sqlstr, "update %s set bind_key =%u where userid=%u and role_regtime=%u and skill_id=%u ;" ,
		this->get_table_name(userid), bind_id, userid, role_regtime, skill_id);
	return this->exec_update_sql(sqlstr, GF_SKILL_NOFIND_ERR)	;

}

int Cgf_skill::get_skill_bind_key(userid_t userid, uint32_t role_regtime, uint32_t *p_count,
	gf_get_skill_bind_key_out_item** pp_list)
{
	sprintf( this->sqlstr, "select skill_id,skill_lv,bind_key from %s where userid=%u and role_regtime=%u and bind_key>0;" ,
		this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD ((*pp_list+i)->skill_id);
		INT_CPY_NEXT_FIELD ((*pp_list+i)->skill_lv);
		INT_CPY_NEXT_FIELD ((*pp_list+i)->bind_key);
	STD_QUERY_WHILE_END();
}
#endif
int Cgf_skill::get_skill_info(userid_t userid, uint32_t role_regtime, uint32_t skill_id,
	skill_info_t* p_out)
{
	sprintf( this->sqlstr, "select skill_id,skill_lv,skill_point from %s where userid=%u and role_regtime=%u and skill_id=%u;" ,
		this->get_table_name(userid), userid, role_regtime, skill_id);
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_SKILL_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (p_out->skill_id);
		INT_CPY_NEXT_FIELD (p_out->skill_lv);
		INT_CPY_NEXT_FIELD (p_out->skill_point);
	STD_QUERY_ONE_END();
}

int Cgf_skill::get_login_skills_info(userid_t userid, uint32_t role_regtime, 
	uint32_t* p_count, gf_get_skill_bind_key_out_item** pp_list)
{
	sprintf( this->sqlstr, "select skill_id,skill_lv from %s where userid=%u and role_regtime=%u ;" ,
		this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD ((*pp_list+i)->skill_id);
		INT_CPY_NEXT_FIELD ((*pp_list+i)->skill_lv);
	STD_QUERY_WHILE_END();

}

int Cgf_skill::get_skill_lv(userid_t userid, uint32_t role_regtime, uint32_t skill_id,
	uint32_t* p_lv)
{
	sprintf( this->sqlstr, "select skill_lv from %s where userid=%u and role_regtime=%u and skill_id=%u;" ,
		this->get_table_name(userid), userid, role_regtime, skill_id);
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_SKILL_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*p_lv);
	STD_QUERY_ONE_END();
}

int Cgf_skill::update_skill_info(userid_t userid, uint32_t role_regtime, uint32_t skill_id,
	skill_info_t* p_in)
{
	if (p_in->skill_lv >= max_skill_lv)
		return VALUE_OUT_OF_RANGE_ERR;
	sprintf( this->sqlstr, "update %s set skill_lv =%u, skill_point=%u where userid=%u and role_regtime=%u and skill_id=%u ;" ,
		this->get_table_name(userid), p_in->skill_lv,p_in->skill_point, userid, role_regtime, skill_id);
	return this->exec_update_sql(sqlstr, GF_SKILL_NOFIND_ERR)	;
}

int Cgf_skill::skill_level_up(userid_t userid, uint32_t role_regtime, uint32_t skill_id)
{
	sprintf( this->sqlstr, "update %s set skill_lv=skill_lv+1 where userid=%u and role_regtime=%u and skill_id=%u ;" ,
		this->get_table_name(userid), userid, role_regtime, skill_id);
	return this->exec_update_sql(this->sqlstr, GF_SKILL_NOFIND_ERR);
}

int Cgf_skill::update_skill_point(userid_t userid, uint32_t role_regtime, uint32_t skill_id,
	uint32_t skill_point)
{
	sprintf( this->sqlstr, "update %s set skill_point=%u where userid=%u and role_regtime=%u and skill_id=%u ;" ,
		this->get_table_name(userid), skill_point, userid, role_regtime, skill_id);
	return this->exec_update_sql(sqlstr, GF_SKILL_NOFIND_ERR)	;
}

int Cgf_skill::clear_role_skill(userid_t userid,uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), userid, role_regtime);
	
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_skill::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}


