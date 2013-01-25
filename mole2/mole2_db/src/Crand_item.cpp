/*
 * =====================================================================================
 *
 *       Filename:  Cpet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *  common.h
 * =====================================================================================
 */

#include "Crand_item.h"


#define STR_USERID	"userid"
#define STR_RANDID	"randid"
#define STR_COUNT	"count"
#define STR_TIME	"time"



Crand_item::Crand_item(mysql_interface * db ) 
	:CtableRoute100(db , "RAND_ITEM" , "t_rand_item" , "userid", "randid")	
{ 

}

int Crand_item::insert(userid_t userid, uint32_t randid, uint32_t time, uint32_t count)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u, %u, %u, %u) ",
			this->get_table_name(userid),
			userid,
			randid, 
			time,
			count
	);
	return this->exec_insert_sql(this->sqlstr, MOLE2_RAND_INFO_EXISTED_ERR);
}

int Crand_item::get_rand_info(uint32_t userid, uint32_t randid, uint32_t *p_time, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select %s, %s from %s where %s = %u and %s = %u",
			STR_TIME,
			STR_COUNT,
			this->get_table_name(userid), 
			STR_USERID,	userid,
			STR_RANDID,	randid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_RAND_INFO_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_time);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Crand_item::set_rand_info(uint32_t userid, uint32_t randid, uint32_t time)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %s + %u where %s = %u and %s = %u",
			this->get_table_name(userid),
			STR_TIME,	time,
			STR_COUNT,	STR_COUNT, 1,
			STR_USERID,	userid,
			STR_RANDID,	randid);
	return this->exec_update_sql(this->sqlstr, MOLE2_RAND_INFO_NOFIND_ERR);
}


int Crand_item::set_rand_info(uint32_t userid, uint32_t randid, uint32_t time, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set time = %u, count = %u where userid = %u and randid = %u",
		this->get_table_name(userid),
		time,
		count,
		userid,
		randid
	);
	return this->exec_update_sql(this->sqlstr, MOLE2_RAND_INFO_NOFIND_ERR);
}

int Crand_item::get_rand_info_range(userid_t userid, uint32_t min_id, uint32_t max_id,get_rand_info_range_out *p_out)
{
	sprintf(this->sqlstr, "select %s, %s, %s from %s where %s = %u and %s >= %u and %s <= %u",
			STR_TIME,
			STR_RANDID,
			STR_COUNT,
			this->get_table_name(userid), 
			STR_USERID,	userid,
			STR_RANDID,	min_id, STR_RANDID,	max_id);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr,p_out->item_list);
		INT_CPY_NEXT_FIELD(item.time);
		INT_CPY_NEXT_FIELD(item.randid);
		INT_CPY_NEXT_FIELD(item.count);
	STD_QUERY_WHILE_END_NEW();
}

