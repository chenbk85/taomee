/*
 * =====================================================================================
 *
 *       Filename:  Cuser_response_friend.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/18/2011 04:31:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_response_friend.h"


Cuser_response_friend::Cuser_response_friend(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_check_friend_response","userid")
{

}

int Cuser_response_friend::insert(userid_t userid, uint32_t pre_friend)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(userid),
			userid,
			pre_friend,
			(uint32_t)time(0)
			);

		STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_response_friend::drop(userid_t userid, uint32_t pre_friend)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and pre_friend = %u",
			this->get_table_name(userid),
			userid,
			pre_friend
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_response_friend::select(userid_t userid, uint32_t pre_friend, uint32_t *datetime)
{
	sprintf(this->sqlstr, "select datetime from %s where userid = %u and pre_friend = %u",
			this->get_table_name(userid),
			userid,
			pre_friend
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*datetime);
	STD_QUERY_ONE_END();	
}

int Cuser_response_friend::get_oldest(userid_t userid, uint32_t *uid)
{
	sprintf(this->sqlstr, "select pre_friend from %s where userid = %u order by datetime asc limit 1",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*uid);
	STD_QUERY_ONE_END();	
}

int Cuser_response_friend::get_count(userid_t userid, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}

int Cuser_response_friend::add(userid_t userid, uint32_t pre_friend)
{
	uint32_t total = 0;	
	int ret = get_count(userid, &total);
	if(total < 50){
		ret = insert(userid, pre_friend);
	}
	else{
		uint32_t old_uid = 0;
		ret = get_oldest(userid, &old_uid);
		if(old_uid != 0){
			ret = this->drop(userid, old_uid);
			ret = insert(userid, pre_friend);
		}
		else{
			ret = USER_ID_NOFIND_ERR;
		}
	}
	return ret;
}
