/*
 * =====================================================================================
 *
 *       Filename:  Cuser_christmas_socks.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/12/2011 10:55:53 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_christmas_socks.h"

Cuser_christmas_socks::Cuser_christmas_socks(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_christmas_socks", "userid")
{

}


int Cuser_christmas_socks::insert(userid_t userid, uint32_t friendid, uint32_t type, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values( %u, %u, %u, %u, 0)",
			this->get_table_name(userid),
			userid,
			friendid,
			type,
			count
			);

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_EXISTED_ERR);	
}


int Cuser_christmas_socks::update(userid_t userid, uint32_t friendid, uint32_t type, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = count + %u where userid = %u and friend = %u and type = %u",
			this->get_table_name(userid),
			count,
			userid,
			friendid,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	
}


int Cuser_christmas_socks::add(userid_t userid, uint32_t friendid, uint32_t type, uint32_t count)
{
	int ret = update(userid, friendid, type, count);
	if(ret == USER_ID_NOFIND_ERR){
		ret = insert(userid, friendid, type, count);
	}
	return ret;
}

int Cuser_christmas_socks::query_toal(userid_t userid, uint32_t type, uint32_t *total)
{
	sprintf(this->sqlstr, "select sum(count) from %s where userid = %u and type = %u",
			this->get_table_name(userid),
			userid,
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*total = atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Cuser_christmas_socks::query_some_friend(userid_t userid, uint32_t friendid, uint32_t type, uint32_t *total)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u and friend = %u and type = %u",
			this->get_table_name(userid),
			userid,
			friendid,
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*total = atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}
int Cuser_christmas_socks::select_page_count(userid_t userid, uint32_t *pages, uint32_t type)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and type = %u",
			this->get_table_name(userid),
			userid,
			type
			);

	uint32_t tmp = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		tmp = atoi_safe(NEXT_FIELD); 
		*pages = (tmp % 10 == 0)? tmp/10 : (tmp/10 + 1);
	STD_QUERY_ONE_END();

}
int Cuser_christmas_socks::select_socks_info(userid_t userid, uint32_t type,  
		uint32_t page, user_query_christmas_socks_out_item **p_out_item, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select friend, count from %s where userid = %u and type = %u limit %u, 10",
			this->get_table_name(userid),
			userid,
			type,
			(page-1)*10
			);

 	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out_item, p_count);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->friendid);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->count);
	STD_QUERY_WHILE_END();
}
int Cuser_christmas_socks::check_socks_gift(userid_t userid, uint32_t friendid, uint32_t *count,
		uint32_t *is_get)
{
	sprintf(this->sqlstr, "select count, is_get from %s where userid = %u and friend = %u and type = 1",
			this->get_table_name(userid),
			userid,
			friendid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		INT_CPY_NEXT_FIELD(*count);
		INT_CPY_NEXT_FIELD(*is_get);
	STD_QUERY_ONE_END();
}

int Cuser_christmas_socks::set_flag(userid_t userid, uint32_t friendid)
{
	sprintf(this->sqlstr, "update %s set is_get = 1 where userid = %u and friend = %u and type = 1",
			this->get_table_name(userid),
			userid,
			friendid
			);

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	
}
