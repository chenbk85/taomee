/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_tw_day_award.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/17/12 18:16:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_tw_day_award.h"

Csysarg_tw_day_award::Csysarg_tw_day_award(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_everyday_lgward")
{

}
/*
 * @brief 插入记录
 */
int Csysarg_tw_day_award::insert(userid_t userid, uint32_t date)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u)",
			this->get_table_name(), userid, date);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 获取记录
 */
int Csysarg_tw_day_award::get_count(userid_t userid, uint32_t &count, uint32_t day)
{
	sprintf(this->sqlstr, "select count(*) from %s where datetime = %u",
			this->get_table_name(), day);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}


