/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_pet.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 13时47分01秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_PET_INCL
#define  CUSER_PET_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "serlib/db_utility.hpp"


class Cuser_pet:public CtableRoute100x10
{
public:
	Cuser_pet(mysql_interface * db ); 
	void cmd_init(const userid_t uid, const uint32_t petid)
	{
		_userid=uid;
		_petid=petid;
	}

	DB_MACRO_define_value_type(logic_birthday_t, logic_birthday, uint32_t)

	///** bugger
	DB_MACRO_define_value_type(fire_skill_bitflag_t, fire_skill_bitflag, uint32_t)
	DB_MACRO_define_value_type(water_skill_bitflag_t, water_skill_bitflag, uint32_t)
	DB_MACRO_define_value_type(wood_skill_bitflag_t, wood_skill_bitflag, uint32_t)

	template<typename FiledType>
	int template_set(const FiledType& v)
	{
		sprintf(this->sqlstr, " update %s set \
			%s=%s \
			where userid=%u \
			and petid=%u ",
			this->get_table_name(_userid), 
			v.name().c_str(),
			db_utility::to_string(v.value()).c_str(),
			_userid,
			_petid );

		STD_SET_RETURN_EX(this->sqlstr,SUCC);	
	}

	template<typename FiledType>
	int template_get(FiledType& v)
	{
		sprintf(this->sqlstr, "select %s from %s \
				where userid=%u\
				and petid=%u", 
				v.name().c_str(),
				this->get_table_name(_userid),
				_userid,
				_petid); 

		STD_QUERY_ONE_BEGIN(this->sqlstr,DB_ERR);
				v.value(NEXT_FIELD);
		STD_QUERY_ONE_END();
	}

private:
	userid_t _userid;
	uint32_t _petid;

public:
	int set_flag(userid_t userid, uint32_t petid, uint32_t flag);
	int set_flag2(userid_t userid, uint32_t petid, uint32_t flag2);
	int set_nick(userid_t userid, uint32_t petid, char *nickname);
	int set_birthday(userid_t userid, uint32_t petid,  uint32_t birthday);
	int set_color(userid_t userid, uint32_t petid, uint32_t color);
	int set_sicktime(userid_t userid, uint32_t petid, uint32_t sicktime);
	int set_endtime(userid_t userid, uint32_t petid, uint32_t endtime);
	int update_life(userid_t userid, uint32_t petid, user_pet_set_life_in *change, user_pet_set_life_out *life);
	int get_life(userid_t userid, uint32_t petid,  user_pet_set_life_out *out);
	int feed_pet(userid_t userid, user_pet_feed_pet_in *in);
	int set_life(userid_t userid, uint32_t petid, user_pet_set_life_in *change, user_pet_set_life_out *out);
	int set_get_pet(userid_t userid, user_pet_set_life_in *change, user_get_user_pet_out_item *out);
	int get_pet(userid_t userid, user_pet_get_pet_in *in);
	int retrieve_pet(userid_t userid, user_pet_retrieve_pet_in *in);
	int travel_pet(userid_t userid, user_pet_travel_pet_in *in);
	int get_one_pet(userid_t userid, uint32_t petid, stru_user_pet*p_out_item);

	int get(userid_t userid, uint32_t petid, uint32_t *p_count, user_get_user_pet_out_item **pp_out_item );
	int get_count(userid_t userid, uint32_t *p_count);

	int get(userid_t userid, uint32_t petid,  user_get_user_pet_out_item  **p_out );
	int add(userid_t userid,  user_add_user_pet_in *p_in, uint32_t  *p_petid);
	int remove(userid_t userid, uint32_t petid);


	int set_info(userid_t userid, user_set_user_pet_info_in *p_in);
	int set_comm(userid_t userid,  user_set_user_pet_comm_in *p_in);
	int set_pos(userid_t userid,  pet_pos_item *p_in);
	int set_no_vip(userid_t userid);
	int set_all(userid_t userid,  
	user_set_user_pet_all_in *p_in);

	int set_sick_type(userid_t userid, uint32_t pet_id, uint32_t sick_type);
	int set_skill(userid_t userid,user_set_user_pet_skill_in   *p_in);
	int get_flag2(userid_t userid, uint32_t petid, uint32_t *p_out);
	int set_skill_flag(userid_t userid, user_pet_set_skill_flag_in  *p_in);
	int set_change_value(userid_t userid, user_pet_set_change_value_in  *p_in);
	int set_hot_skill(userid_t userid, user_pet_set_hot_skill_in  *p_in);
	int set_pet_em_userid(userid_t userid, uint32_t petid, uint32_t em_userid);
	int get_user_pet_employ_info_list(userid_t userid, user_pet_employ_info_list_out_item **pp_list, uint32_t *p_count);
	int pet_flag_update(uint32_t userid, uint32_t petid, uint32_t pet_flag);
	int is_sulamu(uint32_t userid, uint32_t &is_sulamu);

private:
	int cal(uint8_t *value, int32_t change_value);
};

#endif   /* ----- #ifndef CUSER_PET_INCL  ----- */

