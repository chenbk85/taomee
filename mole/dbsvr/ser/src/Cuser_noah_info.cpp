/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_noah_info.cpp
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

#include "Cuser_noah_info.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_noah_info:: Cuser_noah_info(mysql_interface * db ) 
	:CtableRoute10( db, "NOAH", "t_noah_user_info", "userid")
{ 
}

int Cuser_noah_info::insert_user_noah_info(userid_t userid, uint32_t last_time, user_noah_add_user_info_in& st_info) 
{
	char sn_mysql[mysql_str_len(NOAH_SN_LEN)];
	set_mysql_string(sn_mysql, st_info.sn, NOAH_SN_LEN); 
	sprintf(this->sqlstr, "insert into %s (userid, sn, last_time, xiaomee, exp, iq, charm, strong) value(%u, '%s', %u, %u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			st_info.sn,
			last_time,
			st_info.xiaomee,
			st_info.exp,
			st_info.iq,
			st_info.charm,
			st_info.strong
			);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}


int Cuser_noah_info::get_user_noah_info_sum(userid_t userid, char *sn, uint32_t last_time, user_noah_user_info& st_info)
{
	char sn_mysql[mysql_str_len(NOAH_SN_LEN)];
	set_mysql_string(sn_mysql, sn, NOAH_SN_LEN); 
	sprintf(this->sqlstr, "select sum(xiaomee), sum(exp), sum(iq), sum(charm), sum(strong) from %s where userid = %u and sn = '%s' and last_time > %u",
			this->get_table_name(userid),
			userid,
			sn_mysql,
			last_time
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(st_info.xiaomee);
		INT_CPY_NEXT_FIELD(st_info.exp);
		INT_CPY_NEXT_FIELD(st_info.iq);
		INT_CPY_NEXT_FIELD(st_info.charm);
		INT_CPY_NEXT_FIELD(st_info.strong);
	STD_QUERY_ONE_END();
}

int Cuser_noah_info::get_user_xiaomee_sum_week(userid_t userid, char *sn, uint32_t last_week, uint32_t& uixiaomee)
{
	char sn_mysql[mysql_str_len(NOAH_SN_LEN)];
	set_mysql_string(sn_mysql, sn, NOAH_SN_LEN); 
	sprintf(this->sqlstr, "select sum(xiaomee) from %s where userid = %u and sn = '%s' and last_time >= %u",
			this->get_table_name(userid),
			userid,
			sn_mysql,
			last_week
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(uixiaomee);
	STD_QUERY_ONE_END();
}


