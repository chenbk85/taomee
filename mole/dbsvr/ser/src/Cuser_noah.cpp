/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_noah.cpp
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年02月26日 16时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen, tommychen@taomee.com
 *
 * =====================================================================================
 */

#include "Cuser_noah.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_noah:: Cuser_noah(mysql_interface * db ) 
	:CtableRoute10( db, "NOAH", "t_noah_user", "userid")
{ 
}

int Cuser_noah::insert(userid_t userid, char* sn) 
{
	char sn_mysql[mysql_str_len(NOAH_SN_LEN)];
	set_mysql_string(sn_mysql, sn, NOAH_SN_LEN); 
	sprintf(this->sqlstr, "insert into %s (userid, sn) value(%u, '%s')",
			this->get_table_name(userid),
			userid,
			sn_mysql
			);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}


int Cuser_noah::del(userid_t userid, char* sn)
{
	char sn_mysql[mysql_str_len(NOAH_SN_LEN)];
	set_mysql_string(sn_mysql, sn, NOAH_SN_LEN); 
	sprintf(this->sqlstr, "delete from %s where userid = %u and sn = '%s' ",
		this->get_table_name(userid),
		userid,
		sn_mysql
		);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_noah::get_user_noah_bind_info(userid_t userid, user_noah_get_user_bind_info_out& out)
{
	sprintf(this->sqlstr, "select count(*) as num from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out.count);
	STD_QUERY_ONE_END();
}


int Cuser_noah::get_user_noah_user_bind(userid_t userid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count(*) as num from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

