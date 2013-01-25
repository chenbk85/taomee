
/*
 * =====================================================================================
 * 
 *       Filename:  Cpet.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  g++
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CSKILL_INC
#define  CSKILL_INC
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

#define MOLE2_SKILL_LEVEL_MAX	10

class Cskill: public CtableRoute
{
public:
	const char* str_uid;
	const char* str_skillid;
	const char* str_level;
	const char* str_exp;
	const char* str_use_level;
public:
	Cskill(mysql_interface * db);
	int del_skill(userid_t userid, uint32_t skillid);
	int add_skill(userid_t userid, uint32_t skillid, uint8_t level);
	int get_skill_list(userid_t userid, mole2_user_skill_get_list_out_item** pp_out_item,uint32_t* p_count);
	int add_level(userid_t userid, mole2_user_skill_add_level_in* p_in,mole2_user_skill_add_level_out* p_out);
	int add_exp(userid_t userid, mole2_user_skill_add_exp_in* p_in,mole2_user_skill_add_exp_out* p_out);
	int skill_up_update(uint32_t userid, user_user_skill_up_set_in *p_in);
	int skill_up_set(uint32_t userid, uint32_t skillid, uint32_t level, uint32_t exp);
	int set_skill_use_level(userid_t userid, uint32_t skillid,int32_t use_level);
	int get_skills(userid_t userid, std::vector<stru_skill_info> &skills);
};

#endif /* ----- #ifndef CSKILL_INC  ----- */
