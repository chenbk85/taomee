/*
 * =====================================================================================
 *
 *       Filename:  Cpet_skill.cpp
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

#include "Cpet_skill.h"
#include <time.h> 
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"

Cpet_skill::Cpet_skill(mysql_interface * db ) 
	:CtableRoute(db , "MOLE2_USER" , "t_petskill" , "userid","petid"),
	str_uid("userid"), str_petid("petid"), str_skillid("skillid"), str_level("level"),str_use_level("use_level")
{ 

}

int Cpet_skill::add_skill(userid_t userid, uint32_t petid, uint32_t skillid, uint8_t level)
{
	if(level == 0) {
		return SUCC;
	}

	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u, %u, %u, %u,-1) ON DUPLICATE KEY UPDATE level=%u,use_level=-1", 
			this->get_table_name(userid),userid,petid,skillid,level,level);
	int ret = this->exec_insert_sql(this->sqlstr, MOLE2_SKILLID_EXISTED_ERR);
	return ret;
}

int Cpet_skill::get_skill_list(userid_t userid, uint32_t petid, 
		stru_mole2_pet_skill_info** pp_out_item, uint32_t* p_count)
{
	return this->get_list(userid,petid,pp_out_item,p_count);
}

int Cpet_skill::get_list(userid_t userid, uint32_t petid, 
		stru_mole2_pet_skill_info** pp_out_item, uint32_t* p_count)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s from %s where %s=%u and %s=%u order by skillid",
		this->str_skillid,	this->str_level, this->str_use_level,
		this->get_table_name(userid), this->str_uid, userid, this->str_petid, petid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->skillid);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->level);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->use_level);
		(*pp_out_item + i)->exp = 0;
	STD_QUERY_WHILE_END();
}

int Cpet_skill::del_skill(userid_t userid, mole2_user_pet_skill_del_in* p_in)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where %s=%u and %s=%u and %s=%u",
			this->get_table_name(userid), 
			this->str_uid,		userid,
			this->str_petid,	p_in->petid,
			this->str_skillid,	p_in->skillid);
	return this->exec_update_sql(this->sqlstr, MOLE2_SKILLID_NOFIND_ERR);
}

int Cpet_skill::set_skill_use_level(userid_t userid, uint32_t petid, uint32_t skillid,int32_t use_level)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%d where %s=%u and %s=%u and %s=%u",
			this->get_table_name(userid),this->str_use_level,use_level,
			this->str_uid,		userid,
			this->str_petid,	petid,
			this->str_skillid,skillid);
	return this->exec_update_sql(this->sqlstr, MOLE2_SKILLID_NOFIND_ERR);
}

int Cpet_skill::get_skills(userid_t userid, uint32_t gettime, std::vector<stru_skill_info> &skills)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s from %s where %s=%u and %s=%u",
		this->str_skillid,	this->str_level,
		this->get_table_name(userid), this->str_uid, userid, this->str_petid, gettime);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, skills);
		INT_CPY_NEXT_FIELD(item.skillid);
		INT_CPY_NEXT_FIELD(item.level);
		item.exp = 0;
	STD_QUERY_WHILE_END_NEW();
}




