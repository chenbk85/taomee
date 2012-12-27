/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angel_battle.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/07/2011 07:11:26 PM
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
#include "Cuser_angel_battle.h"

Cuser_angel_battle::Cuser_angel_battle(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_angel_battle", "userid")
{

}


int Cuser_angel_battle::insert(userid_t userid, uint32_t exp, 
		uint32_t vic_count, uint32_t day_count, uint32_t day_exp, uint32_t day_prize)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			exp,
			vic_count,
			day_count,
			day_exp,
			(uint32_t)get_date(time(0)),
			day_prize
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_angel_battle::update_inc_info(userid_t userid, uint32_t inc_exp, 
		uint32_t inc_vic_count, uint32_t play_count, uint32_t day_prize)
{
	sprintf(this->sqlstr, "update %s set exp = exp + %u, \
			victory = victory + %u, day_count = %u, \
			day_exp = day_exp + %u , day_prize = day_prize + %u where userid = %u",
			this->get_table_name(userid),
			inc_exp,
			inc_vic_count,
			play_count,
			inc_exp,
			day_prize,
			userid
			);	

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_angel_battle::update_clear_day(userid_t userid, uint32_t inc_exp, 
		uint32_t inc_vic_count, uint32_t play_count, uint32_t day_prize)
{
	sprintf(this->sqlstr, "update %s set exp = exp + %u, \
			victory = victory + %u, day_count = %u, \
			day_exp = %u, day_prize = %u, date = %u where userid = %u",
			this->get_table_name(userid),
			inc_exp,
			inc_vic_count,
			play_count,
			inc_exp,
			day_prize,
			(uint32_t)get_date(time(0)),
			userid
			);	

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_angel_battle::select_statistic_columns(userid_t userid, uint32_t *victory, 
		uint32_t *exp)
{
	sprintf(this->sqlstr, "select exp, victory from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*exp);
		INT_CPY_NEXT_FIELD (*victory);
	STD_QUERY_ONE_END();

}

int Cuser_angel_battle::select_check_columns(userid_t userid, uint32_t &day_count, 
		uint32_t &day_exp, uint32_t &day_prize, uint32_t &date)
{
	sprintf(this->sqlstr, "select day_count, day_exp, day_prize, date from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (day_count);
		INT_CPY_NEXT_FIELD (day_exp);
		INT_CPY_NEXT_FIELD (day_prize);
		INT_CPY_NEXT_FIELD (date);
	STD_QUERY_ONE_END();

}

int Cuser_angel_battle::get_exp(userid_t userid,  uint32_t *out)
{
	sprintf(this->sqlstr, "select exp from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*out);
	STD_QUERY_ONE_END();

}

int Cuser_angel_battle::cal_level(uint32_t exp, uint32_t *level)
{
	const uint32_t top = 17;
	for(uint32_t k = 1; k <= top; ++k){
		uint32_t cal_exp = 45*k*k*k + 2285*k;
		if(k >= 1 &&  k < 4){
			cal_exp += 270;
		}
		else if(k >= 4 && k < 9){
			cal_exp += 270 + 4000;
		}
		else if(k >= 9 && k < 14){
			cal_exp += 270 + 4000 + 6000;
		}
		else if(k >=14 && k < 17){
			cal_exp += 270 + 4000 + 6000 + 10000;
		}
		else{
			cal_exp = 280201;
		}
		if(exp < cal_exp){
			*level = k;
			break;
		}
	}	
	return SUCC;
}
