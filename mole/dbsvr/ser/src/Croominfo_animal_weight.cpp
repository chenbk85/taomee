/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_fish_weight.cpp
 *
 *    Description:  举行了一个活动，就是大家可以去钓鱼，此处定义钓鱼用的记录
 *
 *        Version:  1.0
 *        Created:  2009年11月30日 11时44分43秒
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Croominfo_fish_weight.cpp
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#include"Croominfo_animal_weight.h"


int Canimal_weight::sql_insert(const userid_t uid)
{
	sprintf(this->sqlstr, "insert into %s(user_id) values(%u)",
			this->get_table_name(uid),
			uid
		   );
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Canimal_weight::sql_weight(const userid_t uid, uint32_t& weight)
{
	sprintf(this->sqlstr, "select weight from %s where user_id=%u",
			this->get_table_name(uid),
			uid
		   );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(weight);
	STD_QUERY_ONE_END();
}

int Canimal_weight::sql_update(const userid_t uid, const uint32_t weight)
{
	sprintf(this->sqlstr, "update %s set weight = weight + %u where user_id=%u",
			this->get_table_name(uid),
			weight,
			uid
		   );
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

