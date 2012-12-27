/*
 * =====================================================================================
 *
 *       Filename:  Cuser_ocean_info.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/03/2012 10:07:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_ocean_info.h"


Cuser_ocean_info::Cuser_ocean_info(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_ocean_info", "userid")
{

}


int Cuser_ocean_info::insert(userid_t userid, uint32_t exp)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, 0, 0)",
			this->get_table_name(userid),
			userid,
			exp
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}


int Cuser_ocean_info::update(userid_t userid, uint32_t exp)
{
	sprintf(this->sqlstr, "update %s set exp = if(exp + %u >=16000, 15999, exp+%u) where userid = %u",
			this->get_table_name(userid),
			exp,
			exp,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_ocean_info::add(userid_t userid, uint32_t exp)
{
	int ret = update(userid, exp);
	if(ret != SUCC){
		ret = insert(userid, exp);
	}

	return ret;
}


int Cuser_ocean_info::set_exp(userid_t userid, uint32_t exp)
{
	if(exp >= 16000){
		exp = 15999;
	}
	sprintf(this->sqlstr, "update %s set exp = %u where userid = %u",
			this->get_table_name(userid),
			exp,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_ocean_info::get_exp(userid_t userid, uint32_t *exp)
{
	sprintf(this->sqlstr, "select exp from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*exp);
	STD_QUERY_ONE_END();	
}

int Cuser_ocean_info::get(userid_t userid ,uint32_t* exp, uint32_t* datetime)
{
	sprintf(this->sqlstr, "select exp, datetime from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*exp);
		INT_CPY_NEXT_FIELD(*datetime);
	STD_QUERY_ONE_END();	
}

int Cuser_ocean_info::set(userid_t userid, uint32_t exp, uint32_t datetime)
{
	if(exp >= 16000){
		exp = 15999;
	}
	sprintf(this->sqlstr, "update %s set exp = %u, datetime = %u where userid = %u",
			this->get_table_name(userid),
			exp,
			datetime,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_ocean_info::update_coin(userid_t userid, uint32_t coin)
{
	sprintf(this->sqlstr, "update %s set coin = coin + %u where userid = %u",
			this->get_table_name(userid),
			coin,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_ocean_info::set_coin(userid_t userid, uint32_t coin)
{
	sprintf(this->sqlstr, "update %s set coin =  %u where userid = %u",
			this->get_table_name(userid),
			coin,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_ocean_info::get_coin(userid_t userid, uint32_t *coin)
{
	sprintf(this->sqlstr, "select coin from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*coin);
	STD_QUERY_ONE_END();	
}
