/*
 * =====================================================================================
 *
 *       Filename:  Cuser_hide_and_seek.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/02/2011 04:26:06 PM
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
#include "Cuser_hide_and_seek.h"


Cuser_hide_and_seek::Cuser_hide_and_seek(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_hide_and_seek", "userid")
{

}


int Cuser_hide_and_seek::insert(userid_t userid, uint32_t badge_cnt, uint32_t role)
{
	uint32_t angel_badge_cnt = 0, clown_badge_cnt = 0;
	uint32_t angel_date = 0, clown_date = 0;
	if(role == 1){
		angel_badge_cnt = badge_cnt;
		angel_date = (uint32_t)get_date(time(0));

	}
	else{
		clown_badge_cnt = badge_cnt;
		clown_date = (uint32_t)get_date(time(0)); 
	}
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			angel_badge_cnt,
			clown_badge_cnt,
			angel_date,
			clown_date
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}


int Cuser_hide_and_seek::update(userid_t userid, uint32_t count,uint32_t role)
{
	if(role == 1){
		sprintf(this->sqlstr, "update %s set angel_badge_cnt = %u where userid = %u",
				this->get_table_name(userid),
				count,
				userid
				);
	}
	else{
		sprintf(this->sqlstr, "update %s set clown_badge_cnt = %u where userid = %u",
				this->get_table_name(userid),
				count,
				userid
			   );
	
	}

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_hide_and_seek::select(userid_t userid, uint32_t *count, uint32_t *date , uint32_t role)
{
	if(role == 1){
		sprintf(this->sqlstr, "select angel_badge_cnt, angel_date from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	}
	else{
		sprintf(this->sqlstr, "select clown_badge_cnt, clown_date from %s where userid = %u",
		this->get_table_name(userid),
		userid
		);
	}

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
		INT_CPY_NEXT_FIELD (*date);
	STD_QUERY_ONE_END();
}

int Cuser_hide_and_seek::update_date_cnt(userid_t userid, uint32_t count, uint32_t today, uint32_t role)
{
	if(role == 1){
		sprintf(this->sqlstr, "update %s set angel_badge_cnt = %u, angel_date = %u where userid = %u",
				this->get_table_name(userid),
				count,
				today,
				userid
				);
	}
	else{
		sprintf(this->sqlstr, "update %s set clown_badge_cnt = %u, clown_date = %u where userid = %u",
				this->get_table_name(userid),
				count,
				today,
				userid
			   );
	
	}

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}
