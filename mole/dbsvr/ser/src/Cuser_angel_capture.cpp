/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angel_capture.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/15/2011 10:53:38 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include <time.h> 
#include<cmath>
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "common.h"
#include "Cuser_angel_capture.h"


Cuser_angel_capture::Cuser_angel_capture(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_angel_capture", "userid")
{

}

int Cuser_angel_capture::insert(userid_t userid, uint32_t angelid, uint32_t count, uint32_t date)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			angelid,
			count,
			date
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_angel_capture::update(userid_t userid, uint32_t angelid, uint32_t count, uint32_t date)
{
	uint32_t today = get_date(time(0));
	if(today == date){
		sprintf(this->sqlstr, "update %s set day_count = day_count + %u where userid = %u and angelid = %u",
				this->get_table_name(userid),
				count,
				userid,
				angelid
				);
	}
	else{
		sprintf(this->sqlstr, "update %s set day_count = %u , date =  %u where userid = %u and angelid = %u",
				this->get_table_name(userid),
				count,
				today,
				userid,
				angelid
				);
	}

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
		
int Cuser_angel_capture::select(userid_t userid, uint32_t angelid, uint32_t &count, uint32_t &date)	
{
	sprintf(this->sqlstr, "select day_count, date from %s where userid = %u and angelid = %u",
			this->get_table_name(userid),
			userid,
			angelid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (count);
		INT_CPY_NEXT_FIELD (date);
	STD_QUERY_ONE_END();
}

int Cuser_angel_capture::select_date(userid_t userid, uint32_t angelid, uint32_t &date)	
{
	sprintf(this->sqlstr, "select date from %s where userid = %u and angelid = %u",
			this->get_table_name(userid),
			userid,
			angelid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (date);
	STD_QUERY_ONE_END();
}

int Cuser_angel_capture::select_total(userid_t userid, uint32_t **pp_list, uint32_t *p_count)	
{
	sprintf(this->sqlstr, "select day_count from %s where userid = %u and date = %u",
            this->get_table_name(userid),
            userid,
            (uint32_t)get_date(time(0))
            );

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
        INT_CPY_NEXT_FIELD (*(*pp_list + i));
	STD_QUERY_WHILE_END();	
}

int Cuser_angel_capture::capture_angel_rand(uint32_t total, uint32_t base_point, double beg, 
		uint32_t &partition, uint32_t &dinominator)
{
	uint32_t index = total - base_point;   
	uint32_t num = 0, rands = beg, is_mod = 0; 	
	for(uint32_t i = 0; i < index; ++i){
		if(is_mod == 1 || (rands % 2 != 0)){
			++num;
			is_mod = 1;
		}
		else{
			rands = rands / 2;
		}
		beg = beg / 2;             
	}                              
	if(beg < 1){                   
		return CAPTURE_ANGEL_FAIL_ERR;
	}                              
	else{                          
		uint32_t tmp = pow(10, num);
		dinominator = tmp * 100;
		partition = beg * tmp;
		return SUCC;
	}//else                   

}
