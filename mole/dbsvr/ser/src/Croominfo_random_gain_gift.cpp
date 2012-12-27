/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_random_gain_gift.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/29/2011 02:29:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_random_gain_gift.h"

Croominfo_random_gain_gift::Croominfo_random_gain_gift(mysql_interface *db)
	:CtableRoute10x10(db, "ROOMINFO", "t_roominfo_random_gain_gift", "userid")
{

}

int Croominfo_random_gain_gift::insert(userid_t userid, uint32_t type, uint32_t times)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(userid),
			userid,
			type,
			times
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Croominfo_random_gain_gift::update_times(userid_t userid, uint32_t type, uint32_t times)
{
	sprintf(this->sqlstr, "update %s set times = %u where userid = %u and type = %u",
			this->get_table_name(userid),
			times,
			userid,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Croominfo_random_gain_gift::get_times(userid_t userid, uint32_t type, uint32_t *times)
{
	sprintf(this->sqlstr, "select times from %s where userid = %u and type = %u",
			this->get_table_name(userid),
			userid,
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*times);
	STD_QUERY_ONE_END();
}

int Croominfo_random_gain_gift::add_times(userid_t userid, uint32_t type, uint32_t times)
{
	uint32_t old_times = 0;
	int ret = get_times(userid, type, &old_times);
	if(ret != SUCC){
		ret = insert(userid, type, times);
		DEBUG_LOG("===========type: %u, time: %u", type, times);
	}
	else{
		ret = update_times(userid, type, times + old_times);
		DEBUG_LOG("===========type: %u, time: %u", type, times+old_times);
	}
	return ret;
}
