/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_footprint.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/28/2010 03:03:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_footprint.h"

Croominfo_footprint::Croominfo_footprint(mysql_interface *db):
	CtableRoute10x10(db, "ROOMINFO", "t_roominfo_footprint", "userid")
{

}

/*
 * @brief 插入脚印
 */
int Croominfo_footprint::insert(userid_t userid, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(userid), userid, count, count);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 累积脚印个数
 */
int Croominfo_footprint::update_inc(userid_t userid, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set total_count = total_count + %u,\
			 variable_count = variable_count + %u where userid = %u",
			this->get_table_name(userid), count, count, userid);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 通过交换，减少脚印个数
 */
int Croominfo_footprint::update(userid_t userid, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set variable_count = %u where userid = %u",
			this->get_table_name(userid), count, userid);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 获取脚印数(包括总的，和当前的)
 */
int Croominfo_footprint::get_count(userid_t userid, roominfo_get_footprint_count_out &curr)
{
	sprintf(this->sqlstr, "select total_count,variable_count from %s where userid = %u",
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(curr.total_count);
		INT_CPY_NEXT_FIELD(curr.variable_count);
	STD_QUERY_ONE_END();

}

/*
 * @brief 获取当前脚印数
 */
int Croominfo_footprint::get_curr_count(userid_t userid, uint32_t &curr)
{
    sprintf(this->sqlstr, "select variable_count from %s where userid = %u",
            this->get_table_name(userid), userid);
    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(curr);
    STD_QUERY_ONE_END();

}
