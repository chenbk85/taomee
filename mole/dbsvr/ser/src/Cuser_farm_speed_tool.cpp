/*
 * =====================================================================================
 *
 *       Filename:  Cuser_fa_speed_tool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/20/2011 01:52:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_farm_speed_tool.h"
#include "proto.h"

Cuser_farm_speed_tool::Cuser_farm_speed_tool(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_animal_speed_tool", "userid")
{

}

/*
 * @brief 插入记录
 */

int Cuser_farm_speed_tool::insert(userid_t userid, uint32_t id, uint32_t animalid,
		uint32_t toolid, uint32_t eat_count, uint32_t date)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			id,
			animalid,
			toolid,
			eat_count,
			date
		   );
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @更新技能使用次数和使用时间
 */
int Cuser_farm_speed_tool::update(userid_t userid, uint32_t id ,uint32_t toolid,
		uint32_t count, uint32_t date)
{
	sprintf(this->sqlstr, "update %s set eat_count = %u , date = %u where userid = %u and id = %u and toolid = %u",
			this->get_table_name(userid),
			count,
			date,
			userid,
			id,
			toolid
			);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
/*
 * @brief 获取日期与使用次数
 */
int Cuser_farm_speed_tool::get_date_count(userid_t userid, uint32_t id, uint32_t toolid,
		uint32_t &count, uint32_t &date)
{
	sprintf(this->sqlstr, "select eat_count, date from %s where userid = %u and id = %u and toolid = %u",
			this->get_table_name(userid),
			userid, 
			id,
			toolid
			);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
		INT_CPY_NEXT_FIELD(date);
    STD_QUERY_ONE_END(); 

}

/*
 * @brief 获取日期
 */
int Cuser_farm_speed_tool::get_date(userid_t userid, uint32_t id, uint32_t toolid,
		uint32_t &date)
{
	sprintf(this->sqlstr, "select  date from %s where userid= %u and id = %u and toolid = %u",
			this->get_table_name(userid),
			userid,
			id,
			toolid
			);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(date);
    STD_QUERY_ONE_END(); 

}

/*
 * 删除记录
 */
int Cuser_farm_speed_tool::del_record(userid_t userid, uint32_t id)
{
	sprintf(this->sqlstr, "delete from  %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

int Cuser_farm_speed_tool::get_date(userid_t userid, uint32_t id, uint32_t toolid1, 
		uint32_t toolid2, uint32_t&date)
{
	sprintf(this->sqlstr, "select  date from %s where userid= %u and id = %u and toolid in(%u, %u)",
			this->get_table_name(userid),
			userid,
			id,
			toolid1,
			toolid2
			);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(date);
    STD_QUERY_ONE_END(); 


}
