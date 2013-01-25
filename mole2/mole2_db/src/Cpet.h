
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

#ifndef  CPET_INC
#define  CPET_INC
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

#define STORAGE_SPLIT_END	20000

#define MOLE2_MAX_EXPERIENCE	75468400	//经验最大值
#define	MOLE2_MAX_PHYSIQUE		300			//体力最大值
#define	MOLE2_MAX_STRENGTH		300			//力量最大值
#define	MOLE2_MAX_ENDURANCE		300			//耐力最大值
#define	MOLE2_MAX_QUICK			300			//敏捷最大值
#define	MOLE2_MAX_INTELLIGENCE	300			//智力最大值
#define MOLE2_MAX_ATTR_ADDTION	426			//剩余属性点最大值
#define MOLE2_MAX_HP			0x7fffffff	//HP最大值
#define MOLE2_MAX_MP			0x7fffffff	//MP最大值

#define MOLE2_PET_CARRY_FLAG		0x02
#define MOLE2_PET_LOCATION_FLAG		0x04  

#define MOLE2_PET_FOLLOW_FLAG		0x04

#define MOLE2_PET_SKILL_COUNT_MAX	10


//#define MOLE2_PET_ON_BODY_CNT_MAX	6		// max count on body



class Cpet: public CtableRoute
{
private:
	int	get_nick(userid_t userid, uint32_t petid, mole2_user_pet_update_nick_out* p_out);
	int set_nick(userid_t userid, mole2_user_pet_update_nick_in* p_in);
	int get_attr_addition(userid_t userid, uint32_t petid, mole2_user_pet_add_attr_addition_out* p_out = NULL);
//	int set_attr_addition(userid_t userid, mole2_user_pet_add_attr_addition_out* p_out);
	int set_attr_add(userid_t userid, mole2_user_pet_add_attr_add_out* p_out);

	int unset_pet_rdy(uint32_t userid);

private:
	const char* struid;
	const char* strpetid;
	const char* strpettype;
	const char* strrace;
	const char* strflag;
	const char* strnick;
	const char* strlevel;
	const char* strexp;
	const char* strphyinit;
	const char* strstrinit;
	const char* strendinit;
	const char* strquickinit;
	const char* striqinit;
	const char* strphyparam;
	const char* strstrparam;
	const char* strendparam;
	const char* strquickparam;
	const char* striqparam;
	const char* strphyadd;
	const char* strstradd;
	const char* strendadd;
	const char* strquickadd;
	const char* striqadd;
	const char* strattradd;
	const char* strhp;
	const char* strmp;
	const char* strearth;
	const char* strwater;
	const char* strfire;
	const char* strwind;
	const char* strinjury;
	const char* strlocation;
public:
	Cpet(mysql_interface * db);

	int insert(userid_t userid, uint32_t time, mole2_user_pet_add_in* p_in);
	int update(userid_t userid, uint32_t time, mole2_user_pet_add_in* p_in);
	
	int get_pet_info(userid_t userid , uint32_t petid, mole2_user_pet_get_info_out* p_out );

	int	get_pet_list_on_body(userid_t userid, stru_mole2_user_pet_info_1** pp_item, uint32_t* p_count);

	int	get_pet_list_on_body(userid_t userid, stru_mole2_user_pet_info_2** pp_item, uint32_t* p_count);

	int	get_pet_list_in_store(userid_t userid, pet_in_store_list_get_out_item** pp_item, uint32_t* p_count,uint32_t start,uint32_t cnt);

	int	get_pet_list_in_store(userid_t userid, stru_mole2_user_pet_info_1** pp_item, uint32_t* p_count,uint32_t start,uint32_t cnt);
	
	int pet_add(userid_t userid, mole2_user_pet_add_in* p_in, uint32_t* p_gettime = NULL);
	int update_nick(userid_t userid, mole2_user_pet_update_nick_in* p_in, 
			mole2_user_pet_update_nick_out* p_out); 
	int add_exp(userid_t userid, mole2_user_pet_add_exp_in* p_in,
			mole2_user_pet_add_exp_out* p_out);
	int get_base_attr_initial(userid_t userid, 
			mole2_user_pet_get_base_attr_initial_in* p_in,
			mole2_user_pet_get_base_attr_initial_out* p_out);
	int add_base_attr_initial(userid_t userid,
			mole2_user_pet_add_base_attr_initial_in* p_in,
			mole2_user_pet_get_base_attr_initial_out* p_out);
	int get_base_attr_param(userid_t userid,
			mole2_user_pet_get_base_attr_param_in* p_in,
			mole2_user_pet_get_base_attr_param_out* p_out);
	int get_attr_add(userid_t userid, mole2_user_pet_get_attr_add_in* p_in, 
			mole2_user_pet_get_attr_add_out* p_out);
	
	int set_attr_addition(userid_t userid, mole2_user_pet_add_attr_addition_in* p_in);

	int get_four_attr(userid_t userid, mole2_user_pet_get_four_attr_in* p_in,
			mole2_user_pet_get_four_attr_out* p_out);

	int get_hp_mp(userid_t userid, uint32_t petid, mole2_user_pet_add_hp_mp_out* p_out);
	int add_hp_mp(userid_t userid, uint32_t petid, int hp, int mp, uint32_t injury_state);

	int set_hp_mp(userid_t userid, uint32_t petid, uint32_t hp, uint32_t mp);
	int set_hp_mp_injury(userid_t userid, uint32_t petid, int hp, int mp, uint32_t injury);

	
	int set_flag(userid_t userid, uint32_t petid, uint32_t flag);
	
	int unset_flag(userid_t userid, uint32_t petid, uint32_t flag);
	
	int update_carry_flag(userid_t userid, mole2_user_pet_update_carry_flag_in* p_in,
			mole2_user_pet_update_carry_flag_out* p_out);
	
	int get_cur_petid(userid_t userid, mole2_user_pet_get_info_in* p_in);

	int get_flag(userid_t userid, uint32_t petid, mole2_user_pet_get_flag_out* p_out);

	int injury_state_update(userid_t userid, uint32_t petid, uint32_t injury_state);
	
	int injury_state_select(uint32_t userid, uint32_t petid, uint32_t* injury_state);

	int levelup_update(uint32_t userid, user_pet_levelup_set_in *p_in);

	int flag_get(userid_t userid, uint32_t petid, uint32_t* p_flag);

	int location_set(userid_t userid, uint32_t petid, uint32_t location);

	int follow_pet_clear(userid_t userid);

	int follow_pet_set(uint32_t userid, uint32_t petid);

	int set_attributes(userid_t userid, mole2_set_pet_attributes_in* p_in);

	int pet_cnt_get(userid_t userid, uint32_t* p_count,uint32_t location);

	int chk_pet(userid_t userid, uint32_t petid, uint32_t pettype);
	int del_pet(userid_t userid, uint32_t petid);

	int get_all_pet_type(userid_t userid, get_all_pet_type_out* p_out);

	int get_pets(userid_t userid, std::vector<stru_pet_info> &pets,uint32_t location_min,uint32_t location_max);

	int get_type_pet_count(userid_t userid, uint32_t* p_count,uint32_t pettype);
	int get_type_pets(userid_t userid, std::vector<stru_pet_info> &pets,uint32_t start,uint32_t limit,uint32_t pettype);

	int set_field_value(userid_t userid, su_mole2_set_field_value_in *p_in);
	int	get_last_pets(userid_t userid, std::vector<stru_pet_info> &pets);
};

#endif /* ----- #ifndef CPET_INC  ----- */
