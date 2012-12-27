/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_kakunian.cpp
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

#include "Croominfo_kakunian.h"

Croominfo_kakunian::Croominfo_kakunian(mysql_interface *db)
	:CtableRoute10x10(db, "ROOMINFO", "t_roominfo_kakunian", "userid")
{

}

/*
 * @brief  插入记录
 */
int Croominfo_kakunian::insert(userid_t userid, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u)",
			this->get_table_name(userid), userid, count);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 
 */ 
int Croominfo_kakunian::update(userid_t userid, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = count + %u where userid = %u ",
			this->get_table_name(userid), count, userid);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @breif 查询
 */
int Croominfo_kakunian::get(userid_t userid, uint32_t& count)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u",
			this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

/*
 * @brief 
 */ 
int Croominfo_kakunian::del(userid_t userid)
{
	sprintf(this->sqlstr, "delete from %s  where userid = %u",
			this->get_table_name(userid), userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

