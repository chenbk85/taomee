/*
 * =====================================================================================
 *
 *       Filename:  Cuser_base_character_attribute.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/15/2011 10:43:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "common.h"
#include "Cuser_battle_character_attribute.h"

uint32_t level_threshold[] = {
	50, 110, 210, 330, 480, 680, 910, 1160, 1460, 1810, 2210, 2660, 3210, 3810, 4510,
	    5360, 6360, 7560, 9060, 10860, 12960, 15460, 18360, 21660, 25660, 30060, 34860,
		      40060, 45660, 51660, 58060, 64860, 72060, 79660, 88260, 97260, 106660,
			         116460, 126660, 137260
};

Cuser_battle_character_attribute::Cuser_battle_character_attribute(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_battle_character_attribute", "userid")
{

}

int Cuser_battle_character_attribute::insert(userid_t userid)
{
	sprintf(this->sqlstr, "insert into %s values(%u, 0,6,6,6,6,50,50,0,0,50,50,0,0,0,0,150,500,0,0)",
			this->get_table_name(userid),
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_battle_character_attribute::update(userid_t userid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u",
			this->get_table_name(userid),
			col,
			value,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_character_attribute::update_inc(userid_t userid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u where userid = %u",
			this->get_table_name(userid),
			col,
			col,
			value,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_character_attribute::update_two_columns(userid_t userid, const char* col_1,
	   	const char* col_2,  uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u where userid = %u",
			this->get_table_name(userid),
			col_1,
			value_1,
			col_2,
			value_2,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_character_attribute::update_two_columns_inc(userid_t userid, const char* col_1,
	   	const char* col_2,  uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u, %s = %s + %u where userid = %u",
			this->get_table_name(userid),
			col_1,
			col_1,
			value_1,
			col_2,
			col_2,
			value_2,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_battle_character_attribute::update_three_columns(userid_t userid, const char* col_1,
	   	const char* col_2,  const char* col_3, uint32_t value_1, uint32_t value_2, uint32_t
		value_3)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u, %s = %u where userid = %u",
			this->get_table_name(userid),
			col_1,
			value_1,
			col_2,
			value_2,
			col_3,
			value_3,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_character_attribute::get_one_column(userid_t userid, const char* col, 
		uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u",
			col,
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value);
	STD_QUERY_ONE_END();
}

int Cuser_battle_character_attribute::get_two_columns(userid_t userid, const char* col_1,
	   	const char* col_2, uint32_t *value_1, uint32_t *value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value_1);
		INT_CPY_NEXT_FIELD (*value_2);
	STD_QUERY_ONE_END();
}


int Cuser_battle_character_attribute::get_character_attribute(userid_t userid, t_battle_character_info* 
		person)
{
	sprintf(this->sqlstr, "select exp, wisdom, flexibility, power, strong, energy, vigour, \
			collect_points, energy_limit, vigour_limit, hp, mp from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (person->cur_exp);
		person->exp = person->cur_exp;
		cal_level(person->exp, &person->level);
		if(person->level > 1){
			person->exp = person->exp - level_threshold[person->level - 2];
			person->exp_max = level_threshold[person->level - 1] - level_threshold[person->level - 2];
		}
		else{
			person->exp_max = level_threshold[person->level - 1];
		}
		INT_CPY_NEXT_FIELD (person->wisdom);
		INT_CPY_NEXT_FIELD (person->flexibility);
		INT_CPY_NEXT_FIELD (person->power);
		INT_CPY_NEXT_FIELD (person->strong);
		INT_CPY_NEXT_FIELD (person->energy);
		INT_CPY_NEXT_FIELD (person->vigour);
		INT_CPY_NEXT_FIELD (person->collect_points);
		INT_CPY_NEXT_FIELD (person->energy_limit);
		person->energy_limit += 50;
		INT_CPY_NEXT_FIELD (person->vigour_limit);
		person->vigour_limit += 50;
		INT_CPY_NEXT_FIELD (person->hp);
		INT_CPY_NEXT_FIELD (person->mp);
	STD_QUERY_ONE_END();

}

int Cuser_battle_character_attribute::get_character(userid_t userid, t_battle_character_info* person, 
		uint32_t *if_first)
{
	int ret = get_character_attribute(userid, person);
	if(ret == USER_ID_NOFIND_ERR){
		ret = insert(userid);
		*if_first = 1;
		ret = get_character_attribute(userid, person);
	}
	return ret;
}

int Cuser_battle_character_attribute::cal_person_attribute(userid_t userid, t_battle_character_info *person,
	   	uint32_t recover_time)
{
		uint32_t energy_limit = 0;
		uint32_t vigour_limit = 0;
		int ret = this->get_two_columns(userid, "energy_limit", "vigour_limit", &energy_limit, &vigour_limit);

	    uint32_t now = time(0);
		uint32_t interval = (now - recover_time)/(7*60);
		uint32_t remainder = (now - recover_time)%(7*60);		

		person->energy = (person->energy + interval) > (energy_limit + 50) ? (energy_limit+50):(person->energy + interval);
		person->vigour = (person->vigour + interval) > (vigour_limit + 50)? (vigour_limit+50):(person->vigour + interval); 
		recover_time = now - remainder;
		update_three_columns(userid, "energy", "vigour", "recover_time", person->energy, person->vigour, recover_time);

		return ret;
}

int Cuser_battle_character_attribute::cal_battle_level(userid_t userid, uint32_t *level)
{
	uint32_t exp = 0;
	get_one_column(userid, "exp", &exp);
	cal_level(exp, level);
	return 0;
}

int Cuser_battle_character_attribute::cal_level(uint32_t exp, uint32_t *level)
{
	uint32_t k = 0;
    for(; k < sizeof(level_threshold)/sizeof(uint32_t); ++k){
        if(exp < level_threshold[k]){
            *level = k + 1;
			return 0;
        }
    }
    if(k >= sizeof(level_threshold)/sizeof(uint32_t)){
        *level = 40;
    }
    return 0;
}
int Cuser_battle_character_attribute::get_limit(uint32_t level, uint32_t *limit, uint32_t type)
{
	*limit = 100;
	return 0;
}
int Cuser_battle_character_attribute::update_secondary_attribute(userid_t userid, uint32_t level)
{
	if(level % 2 == 0){
		this->update_two_columns_inc(userid, "power", "strong", level/2, level/2);
	}
	else{
		this->update_two_columns_inc(userid, "wisdom", "flexibility", level/2, level/2);
	}
	return 0;
}

int Cuser_battle_character_attribute::use_recover_props(userid_t userid, uint32_t type, uint32_t props_level,
		uint32_t recover_point, uint32_t *value, uint32_t *day_add_point)
{
	uint32_t level = 0;
	cal_battle_level(userid, &level);
	if(props_level > level){
		return THE_LEVEL_NOT_REACHED_UP_LIMIT_ERR;
	}
	uint32_t today = get_date(time(0));
	if(type == 1){//能量                      
		uint32_t day_energy = 0, date = 0;
		this->get_two_columns(userid, "day_energy", "date", &day_energy, &date);
		*day_add_point = day_energy;
		if(date == today){
			if(day_energy + recover_point > 300){
				*value = 2;	
				return 0;
			}
			day_energy += recover_point;
			*day_add_point = day_energy;
			DEBUG_LOG("&&&&&&&&&&&&&&&&&&& day_energy: %u, recover_point: %u --------", day_energy, recover_point);
			this->update(userid, "day_energy", day_energy);
		}
		else{
			update_three_columns(userid, "day_energy", "day_vigour", "date", recover_point, 0, today);
		}
		uint32_t energy_limit = 0;                  
		this->get_one_column(userid, "energy_limit", &energy_limit);
		uint32_t old_energy  = 0;                   
		get_one_column(userid, "energy", &old_energy);
		energy_limit += 50;
		uint32_t sum = 0;
		if((old_energy + recover_point) > energy_limit){
			sum = energy_limit;
		}                                           
		else{
			sum = old_energy + recover_point;
		}
		DEBUG_LOG( "energy: %u, recover_point: %u --------",sum, recover_point);
		update(userid, "energy", sum);
	}                                               
	else{//活力                                     
		uint32_t day_vigour = 0, date = 0;
		this->get_two_columns(userid, "day_vigour", "date", &day_vigour, &date);
		 *day_add_point = day_vigour;
		if(date == today){
			if(day_vigour + recover_point > 300){
				*value = 2;
				return 0;
			}
            day_vigour += recover_point;
            this->update(userid, "day_vigour", day_vigour);
			*day_add_point = day_vigour;
		}
		else{
		   update_three_columns(userid, "day_energy", "day_vigour", "date", 0, recover_point, today);
		}
		uint32_t vigour_limit = 0;                  
		this->get_one_column(userid, "vigour_limit", &vigour_limit);
		uint32_t old_vigour = 0;                    
		get_one_column(userid, "vigour", &old_vigour);
		vigour_limit += 50;
		uint32_t sum = 0;
		if((old_vigour + recover_point) > vigour_limit){
			sum = vigour_limit;
		}                                           
		else{
			sum  = old_vigour + recover_point;
		}
		update(userid, "vigour", sum);
	}                                               

	return 0;
}

int Cuser_battle_character_attribute::update_base_attribute(userid_t userid, uint32_t flag, uint32_t value)
{
		char *column;
		if(flag == 1){//大力
			column = (char*)"power";   
		}
		else if(flag == 2){//智慧
			column = (char*)"wisdom";
		}   
		else if(flag == 3){//灵活
			column = (char*)"flexibility";
		}   
		else if(flag == 4){//强壮
			column = (char*)"strong";
		}
		else if(flag == 5){//能量
			column = (char*)"energy_limit";
		}
		else if(flag == 6){//活力
			column = (char*)"vigour_limit";
		}
		int32_t ret = update_inc(userid, column, value);

		return ret; 
}

int Cuser_battle_character_attribute::update_base_attribute(userid_t userid, user_battle_add_base_attr_t *p_in)
{
	sprintf(this->sqlstr, "update %s set wisdom = wisdom + %u, flexibility = flexibility + %u, power = power + %u, \
			strong = strong + %u, hp = hp + %u where userid = %u",
			this->get_table_name(userid),
			p_in->wisdom_inc,
			p_in->flexbility_inc,
			p_in->power_inc,
			p_in->strong_inc,
			p_in->hp_inc,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_character_attribute::update_strong_attribute(userid_t userid, uint32_t old_point, uint32_t add_point)
{
	/*
	if(collect_point <= 2000){
		uint32_t index = 1;
		if((collect_point != 100) && (collect_point % 100 == 0)){
			index = collect_point / 100 - 1;
		}
		else{
			index = collect_point / 100;
		}
		if(index != 0){
			this->update_inc(userid, "strong", (index - 1)*3 + 6);
		}
	}//if
	*/
	if (old_point > 1900) {
		return SUCC;
	}

	uint32_t add_val = 0;
	if (old_point / 100 != (old_point + add_point) / 100) {
		add_val = 3;
	}
	if (add_val == 3 && old_point < 100) {
		add_val = 6;
	}
	if (add_val != 0) {
		return this->update_inc(userid, "strong", add_val);
	}
		
	return SUCC;
}

int Cuser_battle_character_attribute::get_level(userid_t userid, uint32_t *level)
{
	*level = 0;
	uint32_t exp;
	int ret = this->get_int_value(userid, "exp", &exp);
	if (ret == SUCC) {
		this->cal_level(exp, level);
	}
	return ret;
}
