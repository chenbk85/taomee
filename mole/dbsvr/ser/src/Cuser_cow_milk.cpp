/*
 * =====================================================================================
 *
 *       Filename:  Cuser_cow_milk.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/2011 11:14:33 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include<algorithm>
#include<ctime>
#include "proto.h"
#include "common.h"
#include <map>
#include "Cuser_cow_milk.h"


Cuser_milk_cow::Cuser_milk_cow(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_milk_cow", "userid")
{

}

int Cuser_milk_cow::insert(userid_t userid,  uint32_t cowid)
{
	uint32_t today = time(0);
	sprintf(this->sqlstr, "insert into %s values(NULL, %u, %u, 100, 80, 80, 0, %u)",
			this->get_table_name(userid),
			userid,
			cowid,
			today	
			);	

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_milk_cow::update_attribute(userid_t userid, uint32_t id, const char *col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u  where userid = %u and id = %u",
			this->get_table_name(userid),
			col,
			value,
			userid,
			id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_milk_cow::get_all(userid_t userid, stru_milk_cow_info **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, cowid, hp, mood, clean, milk_date, eat_date from %s \
			where userid = %u",
			this->get_table_name(userid),
			userid
		   );

	uint32_t date = 0;
	uint32_t today = get_date(time(0));
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->cowid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->hp);
		INT_CPY_NEXT_FIELD((*pp_list + i)->mood);
		INT_CPY_NEXT_FIELD((*pp_list + i)->clean);
		INT_CPY_NEXT_FIELD(date);
		(*pp_list + i)->is_milk = (date == today ? 1 : 0);
		INT_CPY_NEXT_FIELD((*pp_list + i)->eat_date);
	STD_QUERY_WHILE_END();	
}

int Cuser_milk_cow::get_milk_cow_by_id(userid_t userid, uint32_t id, stru_milk_cow_info *plist)
{
	sprintf(this->sqlstr, "select cowid, hp, mood, clean, milk_date, eat_date from %s \
			where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);
	uint32_t date = 0;
	uint32_t today = get_date(time(0));
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(plist->cowid);
		INT_CPY_NEXT_FIELD(plist->hp);
		INT_CPY_NEXT_FIELD(plist->mood);
		INT_CPY_NEXT_FIELD(plist->clean);
		INT_CPY_NEXT_FIELD(date);
		plist->is_milk = (date == today ? 1 : 0);
		INT_CPY_NEXT_FIELD(plist->eat_date);
	STD_QUERY_ONE_END();	
}

int Cuser_milk_cow::cal_cow_attribute(userid_t userid, stru_milk_cow_info *cow, stru_grass_info *grass_info, 
		uint32_t today)
{
	uint32_t interval = (today - cow->eat_date) / (3600*24);
	//uint32_t final_eat_date = cow->eat_date, ite = 0;
	for(uint32_t k = 0; k < interval; ++k){
		
		uint32_t old_hp = cow->hp;
		uint32_t old_mood = cow->mood;
		uint32_t old_clean = cow->clean;
		//扣除牧草，增加体力
		if(grass_info->count != 0){
			//if(old_hp < 100 ){
				uint32_t discount_grass = old_mood > 60 ? 2:4;
				for(uint32_t j = 0; j < discount_grass; ++j){
					uint32_t random = rand() % grass_info->count;
					char *offset = ((char*)(grass_info->grass + random));
					uint32_t mov_count = grass_info->count - random - 1;
					memmove(offset, offset + sizeof(t_grass_attri), mov_count * sizeof(t_grass_attri)); 
					grass_info->count -= 1;
					if(grass_info->count == 0)
						break;
				}
			    cow->hp = (cow->hp + 10) < 100 ? (cow->hp + 10):100;
				//++ite;
				//}
		}
		else{
			cow->hp = cow->hp > 5 ? (cow->hp - 5) : 0;	
		}

		//扣除清洁值
		 uint32_t discount_clean = old_hp >= 60 ? 5:10;
		 if(cow->clean >= discount_clean){
			 cow->clean -= discount_clean;
		 }
		 else{
			 cow->clean = 0;
		 }

		 //扣除心情值
		 uint32_t discount_mood = old_clean >= 60 ? 5:10;
		 if(cow->mood >= discount_mood){
			 cow->mood -= discount_mood;
		 }
		 else{
			 cow->mood = 0;
		 }
		 if(cow->hp == 0 && cow->clean == 0 && cow->mood == 0){
			 break;
		 }
	}//for
	//if(ite < interval){
		//final_eat_date += ite * 24 *3600;
	//}
	//else{
		//final_eat_date = today;
	//}

	set_attributes(userid, cow, today);
	return 0;
}

int Cuser_milk_cow::set_attributes(userid_t userid, stru_milk_cow_info *cow, uint32_t datetime)
{
	sprintf(this->sqlstr, "update %s set hp = %u, mood = %u, clean = %u, eat_date = %u where userid =\
			%u and id = %u",
			this->get_table_name(userid),
			cow->hp,
			cow->mood,
			cow->clean,
			datetime,
			userid,
			cow->id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_milk_cow::get_count(userid_t userid, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}
