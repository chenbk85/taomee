/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_christmas_socks.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/12/2011 02:36:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_christmas_socks.h"

Csysarg_christmas_socks::Csysarg_christmas_socks(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_christmas_socks")
{

}

int Csysarg_christmas_socks::insert(userid_t userid, uint32_t count, uint32_t type)
{
	sprintf(this->sqlstr, "insert %s values( %u, %u, %u, %u)",
			this->get_table_name(),
			userid,
			count,
			(uint32_t)time(0),
			type
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_christmas_socks::update(userid_t userid, uint32_t count, uint32_t type)
{
	sprintf(this->sqlstr, "update %s set userid = %u, count = %u, time = %u  where \
			type = %u and count < %u order by count asc, time desc limit 1",
			this->get_table_name(),
			userid,
			count,
			(uint32_t)time(0),
			type,
			count
			);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

int Csysarg_christmas_socks::get_minority(userid_t *userid, uint32_t *count, uint32_t type)
{
	sprintf(this->sqlstr, "select userid, count from %s where type = %u order by count asc, time desc limit 1",
			this->get_table_name(),
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*userid);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}

int Csysarg_christmas_socks::get_count(uint32_t *count, uint32_t type)
{
	sprintf(this->sqlstr, "select count(*) from %s where type = %u",
			this->get_table_name(),
			type
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}

int Csysarg_christmas_socks::update_old(userid_t userid, uint32_t count, uint32_t type)
{
	sprintf(this->sqlstr, "update %s set count = %u, time = %u where userid = %u and type = %u",
			this->get_table_name(),
			count,
			(uint32_t)time(0),
			userid,
			type
			);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

int Csysarg_christmas_socks::set_count(uint32_t userid, uint32_t count, uint32_t type)
{
	sprintf(this->sqlstr, "update %s set count = %u, time = %u where userid = %u and type = %u",
			this->get_table_name(), count, (uint32_t)time(0), userid, type);
	
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_christmas_socks::renew(userid_t userid, uint32_t count, uint32_t type)
{
	int ret = this->set_count(userid, count, type);
	if (ret != USER_ID_NOFIND_ERR) {
		return ret;
	}
	uint32_t rd_cnt = 0;
	uint32_t max_cnt = 10;
	if (type == 1) {//type == 1为竞拍捐赠排行榜
	//	max_cnt = 8;
	}
	ret = get_count(&rd_cnt, type);
	if(rd_cnt < max_cnt) {
		ret = insert(userid, count, type);
	}
	else {
		ret = update(userid, count, type);
	}

	return ret;
}
int Csysarg_christmas_socks::get_all(user_sysarg_get_christmas_top10_out_item **pp_list,
	   	uint32_t *p_count, uint32_t type)
{
	sprintf(this->sqlstr, "select userid, count from %s where type = %u order by count desc, time asc limit 10",
			this->get_table_name(),
			type
			);

 	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
	STD_QUERY_WHILE_END();
}
