/*
 * =====================================================================================
 *
 *       Filename:  Csys_arg.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Csys_animal.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include <algorithm>

Csys_animal::Csys_animal(mysql_interface * db):Ctable( db, "SYSARG_DB","t_sys_animal" )
{ 
}

/* @brief 插入一条记录
 * @param sys_animal_type 动物的类型
 * @ param 保存的数值
 */
int Csys_animal::insert(uint32_t sys_animal_type, uint32_t value)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u)",
			this->get_table_name(),
			sys_animal_type,
		   	value
			);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);	
}

/* @brief 在原来的基础上，增加value值
 * @param sys_animal_type 类型
 * @param value 要增加的数值
 */
int Csys_animal::update_add_sql(uint32_t sys_animal_type, uint32_t value)
{
	sprintf( this->sqlstr, "update %s set value=value+(%u) \
			where type=%u ",
			this->get_table_name(),
	   		value,
	   		sys_animal_type
	   	   );
	STD_SET_RETURN_EX(this->sqlstr, KEY_NOFIND_ERR);	
}

/* @breif 在原来的基础上增加数值 
 * @param sys_animal_type 类型值
 * @param value 要增加的数值 
 */
int Csys_animal::add_value(uint32_t sys_animal_type, uint32_t value)
{
	int ret= this->update_add_sql(sys_animal_type, value);
	if (ret!=SUCC) {
		ret= this->insert(sys_animal_type, value); 
	}
	return ret;
}

/* @brief 增加或者减少某个字段的数值
 * @param type 类型
 * @param change_value 要更改的数值 
 * @param maxvalue 字段的最大值
 */
int Csys_animal::change_value(uint32_t type, int32_t change_value, uint32_t maxvalue)
{
	uint32_t count = 0;
	uint32_t ret = this->get_col_value(type, &count);
	if(ret!=SUCC){
		if (ret == KEY_NOFIND_ERR) {
			ret = SUCC;
		} else {
			return ret;
		}
	}
	if (change_value < 0) {
		if ((int32_t)count < -change_value) {
			return HAVE_NOT_ENGOUGH_ANIMAL_ERR;
		}
		count += change_value;
	} else {
		count += change_value;
		if (count > maxvalue) {
			count = maxvalue;
		}
	}
	ret = this->update_value(type, count);
	return ret;
}

/* @breif 得到类型对应的字段的数值
 * @param type 类型
 * @param p_value 对应的类型的数值
 */
int Csys_animal::get_col_value(uint32_t type, uint32_t *p_value)
{
	sprintf( this->sqlstr, "select value from %s where type = %u", 
			this->get_table_name(),
			type
		   );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, KEY_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(*p_value);
	STD_QUERY_ONE_END();
}


/* @breif 得到某个范围内的value值
 * @param start_type 开始的范围
 * @param end_type 结束的范围
 * @param pp_value 返回的值
 * @param p_count 返回的个数
 */
int Csys_animal::get_value_list(uint32_t start_type, uint32_t end_type, uint32_t **pp_value, uint32_t *p_count)
{
	sprintf( this->sqlstr, "select value from %s where type>=%u and type <=%u order by type", 
			this->get_table_name(),
			start_type,
			end_type
		   );
	STD_QUERY_WHILE_BEGIN(this-> sqlstr, pp_value, p_count);
			INT_CPY_NEXT_FIELD(*(*pp_value +i));
	STD_QUERY_WHILE_END();
}

/* @breif 更新数值
 * @param sys_animal_type 要更新的type值
 * @param value 返回的数值
 */
int Csys_animal::update_sql(uint32_t sys_animal_type, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set value=%u where type=%u",
			this->get_table_name(),
   			value,
   			sys_animal_type
   		   );
	STD_SET_RETURN_EX(this->sqlstr, KEY_NOFIND_ERR );	
}

/* @breif 更新数值，如不存在就插入一条记录
 * @param sys_animal_type type值
 * @param value 返回的数值
 */
int Csys_animal::update_value(uint32_t sys_animal_type, uint32_t value)
{
	int ret= this->update_sql(sys_animal_type, value);
	if (ret!=SUCC){
		ret= this->insert(sys_animal_type, value); 
	}
	return ret;
}

/* @brief type为1存储的是最重的鱼的米米号，2存储的是重量。保存最大鱼重量的米米号和鱼的重量
 * @param userid 米米号
 * @param weight 用户鱼的总重量
 * @parma p_max_weight 得到最大的总重量
 * @param p_max_userid 得到最大的总重量的USERID号
 */
int Csys_animal :: set_max_weight(userid_t userid, uint32_t weight, uint32_t *p_max_weight, userid_t *p_max_userid)
{
	uint32_t max_weight = 0;
	userid_t max_userid = 0;
	uint32_t count = 0;
	uint32_t *p_value = NULL;
	uint32_t ret = this->get_value_list(1, 2, &p_value, &count);
	if (ret != SUCC) {
		return ret;
	}
	if (count != 0) {
		max_userid = *p_value;
		max_weight = *(p_value + 1);
	}	
	free(p_value);
	if (max_weight < weight) {
		*p_max_weight = weight;
		*p_max_userid = userid;
		ret = this->update_value(1, userid);
		if (ret != SUCC) {
			return ret;
		}
		ret = this->update_value(2, weight);
		if (ret != SUCC) {
			return ret;
		}
	} else {
		*p_max_weight = max_weight;
		*p_max_userid = max_userid;
	}
	return SUCC;
}
