
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

#ifndef  CPET_SKILL_INC
#define  CPET_SKILL_INC
#include "CtableRoute.h"
#include "proto.h"
#include "db_error.h"
#include "benchapi.h"

#define MOLE2_SKILL_LEVEL_MAX	10

class Cpet_skill: public CtableRoute
{
private:
	const char* str_uid;
	const char* str_petid;
	const char* str_skillid;
	const char* str_level;
	const char* str_use_level;
public:
	Cpet_skill(mysql_interface * db);
	int add_skill(userid_t userid, uint32_t petid, uint32_t skillid, uint8_t level);
	int del_skill(userid_t userid, mole2_user_pet_skill_del_in* p_in);
	int get_skill_list(userid_t userid, uint32_t petid, 
			stru_mole2_pet_skill_info** pp_out_item, uint32_t* p_count);
	int get_list(userid_t userid, uint32_t petid, 
			stru_mole2_pet_skill_info** pp_out_item, uint32_t* p_count);
	int set_skill_use_level(userid_t userid, uint32_t petid, uint32_t skillid,int32_t use_level);

	int get_skills(userid_t userid, uint32_t gettime, std::vector<stru_skill_info> &skills);
};

#endif /* ----- #ifndef CPET_SKILL_INC  ----- */
