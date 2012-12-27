/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_shake_rewards_times.cpp
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

#include "Croominfo_shake_rewards_times.h"

Croominfo_shake_rewards_times::Croominfo_shake_rewards_times(mysql_interface *db)
	:CtableRoute10x10(db, "ROOMINFO", "t_roominfo_shake_rewards_times", "userid")
{

}

/*
 * @brief  插入记录
 */
int Croominfo_shake_rewards_times::insert(userid_t userid)
{
	sprintf(this->sqlstr, "insert into %s values(%u, 1)",
			this->get_table_name(userid), userid);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 更新记录
 */ 
int Croominfo_shake_rewards_times::update(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set times = times + 1 where userid =%u",
			this->get_table_name(userid), userid);


	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @breif 查询摇奖次数
 */
int Croominfo_shake_rewards_times::get(userid_t userid, uint32_t *num)
{
	sprintf(this->sqlstr, "select times from %s where userid = %u",
			this->get_table_name(userid),userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*num);
	STD_QUERY_ONE_END();
}

/*
 * @brief 删除记录
 */
//int Croominfo_shake_rewards_times::del_record(userid_t userid)
//{
	//sprintf(this->sqlstr, "delete from %s where userid = %u",
			//this->get_table_name(userid), userid);

	//STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
//}

/*
 *@brief 摇奖次数达到12次后，次数times字段清零 
 */
int Croominfo_shake_rewards_times::clear_times(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set times = 0 where userid = %u",
		this->get_table_name(userid), userid);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
