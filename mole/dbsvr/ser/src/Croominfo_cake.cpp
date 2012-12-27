/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_cake.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/27/2010 01:55:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_cake.h"

Croominfo_cake::Croominfo_cake(mysql_interface *db):
	CtableRoute10x10(db, "ROOMINFO", "t_roominfo_cake", "userid")
{

}

/*
 * @brief 插入记录
 */
int Croominfo_cake::insert(userid_t userid, uint32_t score)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u)",
			this->get_table_name(userid), userid, score);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief更新蛋糕积分
 */
int Croominfo_cake::update(userid_t userid, uint32_t score)
{
	sprintf(this->sqlstr, "update %s set score = %u where userid = %u",
			this->get_table_name(userid), score, userid);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/*
 * @brief 增加蛋糕积分
 */
int Croominfo_cake::increase(userid_t userid, uint32_t score)
{
	sprintf(this->sqlstr, "update %s set score = score + %u where userid = %u",
			this->get_table_name(userid), score, userid);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 查询蛋糕积分
 */
int Croominfo_cake::query_score(userid_t userid, uint32_t *score)
{
	sprintf(this->sqlstr, "select score from %s where userid = %u",
			this->get_table_name(userid), userid);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*score);
	STD_QUERY_ONE_END();
}

