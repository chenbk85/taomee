/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_consume_money.cpp
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

#include "Croominfo_consume_money.h"

Croominfo_consume_money::Croominfo_consume_money(mysql_interface *db)
	:CtableRoute10x10(db, "ROOMINFO", "t_roominfo_consume_money_quantity", "userid")
{

}

/*
 * @brief  插入记录
 */
int Croominfo_consume_money::insert(userid_t userid, uint32_t value)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u)",
			this->get_table_name(userid), userid, value);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 米币消耗增加
 */ 
int Croominfo_consume_money::update_inc(userid_t userid, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set money = money + %u where userid = %u",
			this->get_table_name(userid), value, userid);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 米币消耗减少
 */ 
int Croominfo_consume_money::update_dec(userid_t userid, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set money = %u where userid = %u",
			this->get_table_name(userid), value, userid);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/*
 * @breif 查询消耗米币数
 */
int Croominfo_consume_money::get(userid_t userid, uint32_t *value)
{
	sprintf(this->sqlstr, "select money from %s where userid = %u",
			this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*value);
	STD_QUERY_ONE_END();
}

