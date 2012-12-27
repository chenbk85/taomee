/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_sock.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/20/2010 02:35:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_sock.h"

Croominfo_sock::Croominfo_sock(mysql_interface *db)
	:CtableRoute10x10(db, "ROOMINFO", "t_roominfo_sock", "userid")
{

}

/*
 * @brief  插入记录
 */
int Croominfo_sock::insert(userid_t userid, uint32_t count, uint32_t date)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(userid), userid, count, date);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 
 */ 
int Croominfo_sock::update_by_date(userid_t userid, uint32_t date, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = count + %u where userid = %u and date = %u",
			this->get_table_name(userid), count, userid, date);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @breif 查询
 */
int Croominfo_sock::get(userid_t userid, uint32_t& count, uint32_t& date)
{
	sprintf(this->sqlstr, "select count, date from %s where userid = %u",
			this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
		INT_CPY_NEXT_FIELD(date);
	STD_QUERY_ONE_END();
}

/*
 * @brief 
 */ 
int Croominfo_sock::del(userid_t userid)
{
	sprintf(this->sqlstr, "delete from %s  where userid = %u",
			this->get_table_name(userid), userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

