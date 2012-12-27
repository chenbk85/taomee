/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_gen_vip.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/08/2009 09:21:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
extern "C" {
#include <time.h>
}

#include "Croominfo_flower.h"


Croominfo_flower:: Croominfo_flower(mysql_interface *db)
	              : CtableRoute10x10(db, "ROOMINFO", "t_roominfo_flower", "userid")
{

}


/* @brief 更新花的信息
 * @param userid 米米号
 * @param index 花的序号
 * @param value 花改变的值 
 */
int Croominfo_flower:: update(userid_t userid, uint32_t flag, uint32_t type, int32_t value)
{
		sprintf(this->sqlstr, "update %s set flag = %u, type = %u, count = %d  where userid = %u",
			this->get_table_name(userid),
			flag,
			type,
			value,
			userid
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}


/* @breif 插入花的记录
 * @param userid 米米号
 * @param index 表示哪种花,用0，1，2表示
 * @param value要改变的值
 */
int Croominfo_flower :: insert(userid_t userid, uint32_t flag, uint32_t type, int32_t value)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %d)",
			this->get_table_name(userid),
			userid,
			flag,
			type,
			value
		   );
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

/* @brief 得到花的记录 
 * @param userid 米米号
 * @param p_flag flag标志
 * @param p_type 花的类型
 * @param p_count 花的数目
 */
int Croominfo_flower:: get_value(userid_t userid, uint32_t *p_flag, uint32_t *p_type, int32_t *p_count) 
{
	sprintf(this->sqlstr, "select flag, type, count from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_flag);
		INT_CPY_NEXT_FIELD(*p_type);
		INT_CPY_NEXT_FIELD(*p_count);
    STD_QUERY_ONE_END();
}

