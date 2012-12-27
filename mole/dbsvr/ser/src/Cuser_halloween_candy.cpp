/*
 * =====================================================================================
 *
 *       Filename:  Cuser_halloween_candy.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2011 01:57:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_halloween_candy.h"

Cuser_halloween_candy::Cuser_halloween_candy(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_halloween_candy", "userid")
{

}

int Cuser_halloween_candy::insert(userid_t userid, swap_candy_t *swap_history)
{
	char swap_mysql[mysql_str_len(sizeof(swap_candy_t))]; 
	memset(swap_mysql, 0, sizeof(swap_mysql));
	set_mysql_string(swap_mysql, (char*)(swap_history), sizeof(swap_candy_t));

	sprintf(this->sqlstr, "insert into %s values( %u, %u, 1, '%s')",
			this->get_table_name(userid),
			userid,
			get_date(time(0)),
			swap_mysql
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_halloween_candy::update_date_times(userid_t userid, uint32_t datetime, uint32_t times)
{
	sprintf(this->sqlstr, "update %s set datetime = %u, times = %u where userid = %u",
			this->get_table_name(userid),
			datetime,
			times,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_halloween_candy::get_friends(userid_t userid, swap_candy_t *friends)
{
	sprintf(this->sqlstr, "select friends from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD ((char*)(friends), sizeof(*friends) );
	STD_QUERY_ONE_END();
}

int Cuser_halloween_candy::update_friends_times(userid_t userid, uint32_t datetime,
		uint32_t times, swap_candy_t *friends)
{
	char swap_mysql[mysql_str_len(sizeof(swap_candy_t))]; 
	memset(swap_mysql, 0, sizeof(swap_mysql));
	set_mysql_string(swap_mysql, (char*)(friends), sizeof(swap_candy_t));

	sprintf(this->sqlstr, "update %s set datetime = %u, times = %u, friends = '%s' where userid = %u",
			this->get_table_name(userid),
			datetime,
			times,
			swap_mysql,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_halloween_candy::get_friends_times(userid_t userid, uint32_t *datetime,
	   	uint32_t *times, swap_candy_t *friends)
{
	sprintf(this->sqlstr, "select datetime, times, friends from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*datetime);
		INT_CPY_NEXT_FIELD (*times);
        BIN_CPY_NEXT_FIELD ((char*)(friends), sizeof(*friends) );
	STD_QUERY_ONE_END();

}
