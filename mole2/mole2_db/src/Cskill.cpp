/*
 * =====================================================================================
 *
 *       Filename:  Cskill.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *  common.h
 * =====================================================================================
 */

#include "Cskill.h"
#include <time.h> 
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"

Cskill::Cskill(mysql_interface * db ) 
	:CtableRoute(db , "MOLE2_USER" , "t_skill" , "userid","skillid"),
	str_uid("userid"), str_skillid("skillid"), str_level("level"), str_exp("experience"),str_use_level("use_level")
{ 

}

int Cskill::add_skill(userid_t userid, uint32_t skillid, uint8_t level)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u, %u, %u, %u,-1)", 
			this->get_table_name(userid),
			userid,
			skillid,
			level,
			0 /* experience */);
	return this->exec_insert_sql(this->sqlstr, MOLE2_SKILLID_EXISTED_ERR);
}

int Cskill::get_skill_list(userid_t userid, mole2_user_skill_get_list_out_item** pp_out_item,uint32_t* p_count)
{
   	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s from %s where %s=%u",
		this->str_skillid,	this->str_level, this->str_exp,this->str_use_level,
		this->get_table_name(userid),
		this->str_uid,		userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->skillid);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->level);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->exp);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->use_level);
	STD_QUERY_WHILE_END();
}

int Cskill::add_level(userid_t userid, mole2_user_skill_add_level_in* p_in,mole2_user_skill_add_level_out* p_out)
{
	uint32_t realvalue;
	int ret;
	ret = this->change_int_value(userid, p_in->skillid, this->str_level, p_in->level, MOLE2_SKILL_LEVEL_MAX, &realvalue);
	p_out->level = realvalue;
	p_out->skillid = p_in->skillid;
	return ret;
}

int Cskill::add_exp(userid_t userid, mole2_user_skill_add_exp_in* p_in,mole2_user_skill_add_exp_out* p_out)
{
	uint32_t	realvalue;
	int ret;
	ret = this->change_int_value(userid, p_in->skillid, this->str_exp, p_in->exp, 0x7FFFFFFF, &realvalue);
	p_out->exp = realvalue;
	p_out->skillid = p_in->skillid;
	return ret;
}

int Cskill::del_skill(userid_t userid, uint32_t skillid)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where %s=%u and %s=%u",
			this->get_table_name(userid), 
			this->str_uid,		userid,
			this->str_skillid,	skillid);
	return this->exec_update_sql(this->sqlstr, MOLE2_SKILLID_NOFIND_ERR);
}

int Cskill::skill_up_update(uint32_t userid, user_user_skill_up_set_in *p_in)
{
	sprintf(this->sqlstr, "update %s set experience = experience + %u, level = level + %u where userid = %u and skillid = %u",
		this->get_table_name(userid),
		p_in->experience,
		p_in->level,
		userid,
		p_in->skillid
		);
	return this->exec_update_sql(this->sqlstr, MOLE2_SKILLID_NOFIND_ERR);
}

int Cskill::skill_up_set(uint32_t userid, uint32_t skillid, uint32_t level, uint32_t exp)
{
	sprintf(this->sqlstr, "update %s set  level = %u, experience = %u where userid = %u and skillid = %u",
		this->get_table_name(userid),
		level,
		exp,
		userid,
		skillid
	);
	return this->exec_update_sql(this->sqlstr, MOLE2_SKILLID_NOFIND_ERR);	
}

int Cskill::set_skill_use_level(userid_t userid, uint32_t skillid,int32_t use_level)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%d where %s=%u and %s=%u",
			this->get_table_name(userid),this->str_use_level,use_level,
			this->str_uid,		userid,
			this->str_skillid,	skillid);
	return this->exec_update_sql(this->sqlstr, MOLE2_SKILLID_NOFIND_ERR);
}

int Cskill::get_skills(userid_t userid, std::vector<stru_skill_info> &skills)
{
   	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s from %s where %s=%u",
		this->str_skillid,	this->str_level, this->str_exp,
		this->get_table_name(userid),
		this->str_uid,		userid);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, skills);
		INT_CPY_NEXT_FIELD(item.skillid);
		INT_CPY_NEXT_FIELD(item.level);
		INT_CPY_NEXT_FIELD(item.exp);
	STD_QUERY_WHILE_END_NEW();
}


