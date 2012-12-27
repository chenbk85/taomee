/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_dining_employer.cpp
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年03月09日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen, tommychen@taomee.com
 *
 * =====================================================================================
 */

#include "Cuser_dining_employer.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"


static stru_employer_info employer_attribute[] =
{
	{	1,	"小米",	172800,	500,	1,	2},
	{	2,	"赛利",	259200,	1000,	8,	2},
	{	3,	"道格",	345600,	2000,	6,	3},
	{	4,	"瑞林",	432000,	4000,	4,	4},
	{	5,	"米勒",	432000,	6000,	2,	4},
};


/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_dining_employer::Cuser_dining_employer(mysql_interface * db ) 
	:CtableRoute100x10( db, "USER", "t_user_dining_employer", "userid")
{ 
}

int Cuser_dining_employer::add_employer(userid_t userid, user_dining_add_employer_in* p_in) 
{
	uint32_t cur_time = time(NULL);

	sprintf(this->sqlstr, "insert into %s (userid, em_userid, em_user_name, em_petid, pet_color, pet_level, level, time,time_limit,pet_skill) values(%u, %u, '%s', %u, %u, %u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			p_in->em_userid,
			p_in->em_user_name,
			p_in->em_petid,
			p_in->pet_color,
			p_in->pet_level,
			p_in->level,
			cur_time,
			p_in->time,
			p_in->pet_skill
			);
	STD_SET_RETURN_EX (this->sqlstr, DB_ERR);
}


int Cuser_dining_employer::add_employer_work_for_other(userid_t userid, user_dining_user_work_for_other_in* p_in) 
{
	uint32_t cur_time = time(NULL);

	sprintf(this->sqlstr, "insert into %s (userid, em_userid, em_user_name, em_petid, time) values(%u, %u, '%s', %u, %u)",
			this->get_table_name(p_in->userid),
			p_in->userid,
			userid,
			p_in->em_user_name,
			p_in->em_petid,
			cur_time
			);
	STD_SET_RETURN_EX (this->sqlstr, DB_ERR);
}


int Cuser_dining_employer::del_employer(userid_t userid, user_dining_del_employer_in* p_in)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and em_userid = %u and em_petid = %u",
		this->get_table_name(userid),
		userid,
		p_in->em_userid,
		p_in->em_petid
		);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}



int Cuser_dining_employer::get_user_employer(userid_t userid, user_dining_employer_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select em_userid, em_user_name, em_petid, pet_color, pet_level, level, time,time_limit,pet_skill\
						   from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->em_userid);
		BIN_CPY_NEXT_FIELD(&((*pp_list + i)->em_user_name), NICK_LEN);
		INT_CPY_NEXT_FIELD((*pp_list + i)->em_petid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pet_color);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pet_level);
		INT_CPY_NEXT_FIELD((*pp_list + i)->level);
		INT_CPY_NEXT_FIELD((*pp_list + i)->time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->time_limit);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pet_skill);
		(*pp_list + i)->time = time(NULL) - (*pp_list + i)->time;
	STD_QUERY_WHILE_END();
}

int Cuser_dining_employer::get_user_employer_count(userid_t userid, uint32_t& num)
{
	sprintf(this->sqlstr, "select count(*) as num  from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(num);
	STD_QUERY_ONE_END();
}

int Cuser_dining_employer::get_sys_employer_list(userid_t userid, user_dining_get_sys_employer_list_out_item  **pp_list, user_dining_get_sys_employer_list_out_item  **pp,  uint32_t *p_count, uint32_t *count_p)
{	
	sprintf(this->sqlstr, "select  em_petid from %s where userid = %u  and em_userid = 10000 order by level asc",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp, count_p);
		INT_CPY_NEXT_FIELD((*pp + i)->level);
	STD_QUERY_WHILE_END_WITHOUT_RETURN();  
 
	uint32_t num = 5;
	/*
	if (num <= *count_p)
	{
		return USER_DINING_NO_SYS_EMPLOYER_ERR;
	}
	*/
	*p_count = num - *count_p;
	*pp_list = ( typeof(*pp_list))malloc(sizeof(typeof(**pp_list)) * (*p_count)) ;
   	if (*pp_list == NULL){
		 return SYS_ERR;						            
	}
   
	uint32_t n = 0;
	bool is_bool = false;
	for (uint32_t j = 0; j < num; j++)
	{
		is_bool = false;

		for (uint32_t k = 0; k < *count_p; k++)
		{
			if (employer_attribute[j].level == (*pp + k)->level)
			{
				is_bool = true;
			}
		}

		if ((!is_bool) && (n < *p_count))
		{
			(*pp_list + n)->userid = 10000;
			(*pp_list + n)->petid = employer_attribute[j].level;
			(*pp_list + n)->level = employer_attribute[j].level;
			memcpy((*pp_list + n)->nick, employer_attribute[j].nick, NICK_LEN);
			(*pp_list + n)->time = employer_attribute[j].time;
			(*pp_list + n)->xiaomee = employer_attribute[j].xiaomee;
			(*pp_list + n)->pet_color = employer_attribute[j].pet_color;
			(*pp_list + n)->pet_level = employer_attribute[j].pet_level;
			n++;
		}
	}

	return SUCC;
}

int Cuser_dining_employer::get_sys_employer_xiaomee(uint32_t level, uint32_t& xiaomee)
{
	xiaomee = employer_attribute[level-1].xiaomee;

	return SUCC;
}

int Cuser_dining_employer::get_em_user_employer_time(userid_t userid, userid_t em_userid, uint32_t em_petid, uint32_t& time,uint32_t &level, uint32_t &skill)
{
	sprintf(this->sqlstr, "select time, pet_level, pet_skill from %s where userid = %u and em_userid = %u and em_petid = %u",
			this->get_table_name(userid),
			userid,
			em_userid,
			em_petid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(time);
		INT_CPY_NEXT_FIELD(level);
		INT_CPY_NEXT_FIELD(skill);
	STD_QUERY_ONE_END();
}



