/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_cake_gift_swap.cpp
 *
 *    Description:  蛋糕与礼品之间可以进行交换，此处正理蛋糕的计数
 *
 *        Version:  1.0
 *        Created:  11/26/2009 01:50:26 PM
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Croominfo_cake_gift_swap.cpp
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#include"Croominfo_cake_gift_swap.h"

int Ccake_gift_swap::sql_insert(const userid_t uid)
{
	sprintf(this->sqlstr, "insert into %s(user_id) values(%u)",
			this->get_table_name(uid),
			uid
		   );
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Ccake_gift_swap::sql_update(const userid_t uid, const uint32_t n)
{
	sprintf(this->sqlstr, "update %s set cake_number = %u where user_id=%u",
			this->get_table_name(uid),
			n,
			uid
		   );
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Ccake_gift_swap::sql_get(const userid_t uid, uint32_t& cake_num)
{
	sprintf(this->sqlstr, "select cake_number from %s where user_id=%u",
			this->get_table_name(uid),
			uid
		   );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(cake_num);
	STD_QUERY_ONE_END();
}

/**
 * @brief 把相应的蛋糕数目添加指定的数目，逻辑上所有数据都合法的
 * @para uid 相应的用户米米号
 * @para n 减去的相应值
 * @return 数据库错误，或者SUCC成功
 */
int Ccake_gift_swap::cake_add(const userid_t uid, const uint32_t n)
{
	uint32_t cv=0;
	DEBUG_LOG("====sql_get");
	int ret=sql_get(uid, cv);
	DEBUG_LOG("====sql_get_after");
	if(USER_ID_NOFIND_ERR==ret)
	{
		ret=sql_insert(uid);
	}

	if(SUCC!=ret)
	{
		return ret;
	}

	uint32_t cake_num=cv + n;

	return sql_update(uid, cake_num);
}

/**
 * @brief 把相应的蛋糕数目减去去指定的数目
 * @dtail 本函数考虑数据合法性，不合法它会拒绝执行
 * @para uid 相应的用户米米号
 * @para n 减去的相应值
 * @return 可能数据库错误，或者一个逻辑错误：ROOMINFO_CAKE_GIFT_SWAP_NOT_ENOUGH_ERR
 */
int Ccake_gift_swap::cake_subtract(const userid_t uid, const uint32_t n)
{
	uint32_t cv=0;
	int ret=sql_get(uid, cv);
	if(USER_ID_NOFIND_ERR==ret)
	{
		ret=sql_insert(uid);
	}

	if(SUCC!=ret)
	{
		return ret;
	}

	if(n>cv) //数目不允许为负值
	{
		return ROOMINFO_CAKE_GIFT_SWAP_NOT_ENOUGH_ERR;
	}

	return sql_update(uid, cv-n);
}

