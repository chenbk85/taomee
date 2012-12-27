/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_user_rank.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/15/2010 06:00:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_user_rank.h"

Csysarg_user_rank::Csysarg_user_rank(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_user_rank")
{

}
/*
 * @brief 插入记录
 */
int Csysarg_user_rank::insert(userid_t userid, uint32_t type, uint32_t rank, uint32_t date)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u)",
			this->get_table_name(), userid, type, rank, date);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 更新记录
 */

int Csysarg_user_rank::update(userid_t userid, uint32_t type, uint32_t rank, uint32_t date)
{
	sprintf(this->sqlstr, "update %s set rank = %u, date = %u where userid = %u and type = %u",
			this->get_table_name(), rank, date, userid, type);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 获取记录
 */
int Csysarg_user_rank::get_user_rank(userid_t userid, uint32_t type, uint32_t &rank, uint32_t &date)
{
	sprintf(this->sqlstr, "select rank, date from %s where userid = %u and type = %u",
			this->get_table_name(), userid, type);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(rank);
		INT_CPY_NEXT_FIELD(date);
	STD_QUERY_ONE_END();
}


