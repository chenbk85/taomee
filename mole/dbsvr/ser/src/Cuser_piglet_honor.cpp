/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_honor.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2012 01:19:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_piglet_honor.h"

Cuser_piglet_honor::Cuser_piglet_honor(mysql_interface *db)
	:CtableRoute100x10(db, "USER","t_user_piglet_honor", "userid")
{

}

int Cuser_piglet_honor::insert(userid_t userid, uint32_t type, uint32_t honorid, uint32_t light, 
		uint32_t timestamp, uint32_t cur_times, uint32_t max_times, uint32_t mul_value)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			type,
			honorid,
			light,
			timestamp,
			cur_times,
			max_times,
			mul_value
			);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_piglet_honor::add_honor(userid_t userid, uint32_t breed, uint32_t *res)
{

	uint32_t piglet_breed[] = {0, 1, 5, 2, 12, 7, 9, 10, 6, 3, 4, 13, 8, 11, 17, 23, 14, 10001, 
		10003, 10004, 10000, 10005, 10007, 10002};	

	uint32_t *iter = std::find(piglet_breed, piglet_breed+sizeof(piglet_breed)/sizeof(uint32_t), breed);
	if(iter != piglet_breed+sizeof(piglet_breed)/sizeof(uint32_t)){
		uint32_t mul_value = 0;
		if(breed >= 0 && breed < 10000){
			mul_value = 24*2*3600;	
		}
		else{
			mul_value = 24*6*3600;
		}
		int ret = insert(userid, 0, (iter-piglet_breed)+1, 0, (uint32_t)time(0), 0, 1, mul_value);	
		if(ret == SUCC){
			*res = 1;
			//*medal = (iter - piglet_breed)+1351243;
		}//if
	}
	return 0;
}

int Cuser_piglet_honor::update_two_cols(uint32_t userid, uint32_t type, uint32_t honorid, const char* col_1,
	   	const char* col_2, uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u where userid = %u and type = %u and honorid = %u",
			this->get_table_name(userid),
			col_1,
			value_1,
			col_2,
			value_2,
			userid,
			type,
			honorid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_honor::update(userid_t userid, uint32_t type, uint32_t honorid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u and type = %u and honorid = %u",
			this->get_table_name(userid),
			col,
			value,
			userid,
			type,
			honorid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_honor::get_all(userid_t userid, user_piglet_honor_t** pp_tmp, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select type, honorid, light, timestamp, cur_times, max_times, \
			mul_value from %s where userid = %u ",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_tmp, p_count);
		INT_CPY_NEXT_FIELD((*pp_tmp + i)->type);
		INT_CPY_NEXT_FIELD((*pp_tmp + i)->honorid);
		INT_CPY_NEXT_FIELD((*pp_tmp + i)->light);
		INT_CPY_NEXT_FIELD((*pp_tmp + i)->timestamp);
		INT_CPY_NEXT_FIELD((*pp_tmp + i)->cur_times);
		INT_CPY_NEXT_FIELD((*pp_tmp + i)->max_times);
		INT_CPY_NEXT_FIELD((*pp_tmp + i)->mul_value);
	STD_QUERY_WHILE_END();

}

int Cuser_piglet_honor::get_honor_by_id(userid_t userid, uint32_t type, uint32_t honorid, uint32_t* light,
		uint32_t *cur_times, uint32_t *max_times, uint32_t *mul_values)
{
	sprintf(this->sqlstr, "select light, cur_times, max_times, mul_value from %s \
			where userid = %u and type = %u and honorid = %u",
			this->get_table_name(userid),
			userid,
			type,
			honorid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*light);
		INT_CPY_NEXT_FIELD(*cur_times);
		INT_CPY_NEXT_FIELD(*max_times);
		INT_CPY_NEXT_FIELD(*mul_values);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet_honor::set_honor(userid_t userid, user_set_piglet_honor_in *p_in, uint32_t *flag)
{
	if(p_in->honorid < 26 || p_in->honorid > 40){
		return 0;
	}	
	uint32_t  challenge_honor_max[] = {3, 20, 0, 1, 10, 5, 20, 10, 5, 10, 30, 0, 50, 10, 3};

	uint32_t cur_times = 0, max_times = 0, mul_value = 0, light = 0;
	int ret = this->get_honor_by_id(userid, 1, p_in->honorid, &light, &cur_times, &max_times, &mul_value);
	if(ret != SUCC){
		if(p_in->honorid == 26){
			ret = this->insert(userid, 1, p_in->honorid, 0, 0, 1, challenge_honor_max[p_in->honorid-26], (uint32_t)time(0));
		}
		if(p_in->honorid == 29){
			ret = this->insert(userid, 1, p_in->honorid, 1, 0, 1, challenge_honor_max[p_in->honorid-26], p_in->extra);
			if(ret == SUCC){
				*flag = 1;
			}
		}
		else{
			ret = this->insert(userid, 1, p_in->honorid, 0, 0, 1, challenge_honor_max[p_in->honorid-26], p_in->extra);
		}
	}
	else{
		  if(light == 0){
		  	if(p_in->honorid == 26){//登陆喂养三次
			  uint32_t now = time(0);
			  uint32_t today = get_date(now);
			  if(today != (uint32_t)get_date(mul_value)){
				++cur_times;
				this->update(userid, 1, p_in->honorid, "mul_value", now);
			  }//if
		  	}
			else if(p_in->honorid == 31){//连胜30次
				if(p_in->extra == 0){
					cur_times = 0;
				}
				else{
					++cur_times;
				}
			}
			else{
				++cur_times;
			}
			if(cur_times == max_times){
				*flag = 1;
				this->update_two_cols(userid, 1, p_in->honorid, "light", "cur_times", 1, max_times);
			}
			else{
				this->update(userid, 1, p_in->honorid, "cur_times", cur_times);
			}
		  }//if(light==0)

	}//else	

	return 0;

}

