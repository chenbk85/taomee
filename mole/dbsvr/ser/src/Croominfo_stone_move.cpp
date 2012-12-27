/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_stone_move.cpp
 *
 *    Description:  摩尔搬石头，需要记录每天搬的石头和总共搬的石头
 *
 *        Version:  1.0
 *        Created:  01/25/2010 02:54:59 PM
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Croominf_stone_move.cpp
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#include"Croominfo_stone_move.h"
#include "logproto.h"
#include "benchapi.h"


int Croominfo_stone_move::insert()
{
	sprintf(this->sqlstr, "insert into %s(userid) values(%u)",
		this->get_table_name(_userid),
		_userid
	);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Croominfo_stone_move::stone_today_total_get(uint32_t& today, uint32_t& total)
{
	sprintf( this->sqlstr, "select today, total from %s where userid=%d", 
		this->get_table_name(_userid),
		_userid
	);

	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(today);
		INT_CPY_NEXT_FIELD(total);
	STD_QUERY_ONE_END();
}

int Croominfo_stone_move::stone_today_total_set(uint32_t today, uint32_t total)
{
	sprintf(this->sqlstr, "update %s set today=%u, total=%u where userid=%d", 
		this->get_table_name(_userid),
		today,
		total,
		_userid
	);

	STD_SET_RETURN(this->sqlstr,_userid,USER_ID_NOFIND_ERR);	
}

int Croominfo_stone_move::stone_today_total_replace(uint32_t today, uint32_t total)
{
	int ret=stone_today_total_set(today, total);
	if(USER_ID_NOFIND_ERR==ret)
	{
		ret=insert();
		ret=stone_today_total_set(today, total);
	}

	return ret;
}

