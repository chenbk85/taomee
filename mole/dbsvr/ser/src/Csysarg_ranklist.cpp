/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_ranklist.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/06/2010 02:28:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_ranklist.h"
//#include "proto.h"
//#include "benchapi.h"

Csysarg_ranklist::Csysarg_ranklist(mysql_interface * db) :
	Ctable(db, "SYSARG_DB","t_sysarg_ranklist")
{

}

/*
 * @brief 插入一条记录
 * @param field: 字段名
 */
int Csysarg_ranklist::insert(uint32_t userid, uint32_t type, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u,%u)",
				this->get_table_name(), userid, type, count, count, (uint32_t)time(NULL));
	
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*  
 * @brief 修改字段的值
 */
int Csysarg_ranklist::update(userid_t userid, uint32_t type, uint32_t count, uint32_t dynamic_count, uint32_t datetime)
{
	if(type == 2){
		sprintf(this->sqlstr, "update %s set count = %u, dynamic_count = %u \
				where userid = %u and type = %u", 
			this->get_table_name(), count, dynamic_count, userid, type);
	}else{
		sprintf(this->sqlstr, "update %s set count = %u, dynamic_count= %u,\
			datetime = %u where userid = %u and type = %u", 
			this->get_table_name(), count, dynamic_count, datetime, userid, type);
	}
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 得到排名
 */
int Csysarg_ranklist::get_ranklist(sysarg_get_ranklist_out_item **pp_list, uint32_t type, uint32_t num, uint32_t *p_count)
{
 	sprintf(this->sqlstr, "select userid, count from %s where type= %u order \
		   	by count desc,datetime asc limit %u", this->get_table_name(),type,num);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
	STD_QUERY_WHILE_END();
}

/*  
 * @brief 得到特定用户的指定type下的dynamic数量
 */
int Csysarg_ranklist::get_specify_user_dynamic_count(userid_t userid, uint32_t *p_out, uint32_t type)
{
	sprintf(this->sqlstr, "select dynamic_count from %s where userid = %u and type = %u",
		   	this->get_table_name(),userid,type);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*p_out);
	STD_QUERY_ONE_END();
}

/*  
 * @brief 得到特定用户的指定type下的数量
 */
int Csysarg_ranklist::get_specify_user_count(userid_t userid, uint32_t *p_out, uint32_t type)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u and type = %u",
		   	this->get_table_name(),userid,type);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_out);
	STD_QUERY_ONE_END();
}

/*  
 * @brief 判断用户是否存在于表中,存在out置为1，否则为0
 */
/*
int Csysarg_ranklist::exist_count(uint32_t *out,userid_t userid, uint32_t type)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and type = %u ",
		   	this->get_table_name(),userid,type);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*out);
	STD_QUERY_ONE_END();
}
*/

/*  
 * @brief 获得在count值相同时的用户记录
 */
int Csysarg_ranklist::get_same_count_list(uint32_t count, uint32_t type,tmp_for_rank **out_same_list,uint32_t *num)
{
	sprintf(this->sqlstr, "select count,datetime from %s where type = %u and count = %u",
		   	this->get_table_name(),type, count);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, out_same_list, num);
		INT_CPY_NEXT_FIELD((*out_same_list + i)->count);
		INT_CPY_NEXT_FIELD((*out_same_list + i)->datetime);
	STD_QUERY_WHILE_END();

	return SUCC;
}
/*  
 * @brief 获得在给定type类型下的用户记录数
 */
int Csysarg_ranklist::get_type_count(uint32_t *out, uint32_t type)
{
	sprintf(this->sqlstr, "select count(*) from %s where type = %u",
		   	this->get_table_name(),type);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*out);
	STD_QUERY_ONE_END();
}

/*  
 * @brief 得到用户自己的排名
 */
int Csysarg_ranklist::get_self_ranking(userid_t userid, uint32_t *out, uint32_t type)
{
	uint32_t user_count = 0;
	get_specify_user_count(userid, &user_count, type);

	sprintf(this->sqlstr, "select count(*) from %s where type = %u and count>=%u ",
		   	this->get_table_name(),type,user_count);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*out);
	STD_QUERY_ONE_END();
}

/*  
 * @brief 得到特定用户的排名
 */
/* 
int Csysarg_ranklist::get_specify_user_ranking(uint32_t *out, uint32_t type, uint32_t user_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where type = %u and count>=%u ",
		   	this->get_table_name(),type,user_count);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*out);
	STD_QUERY_ONE_END();
}
*/

/*
 * @brief 兑换物品
 */
int Csysarg_ranklist::exchange_present(userid_t userid,exchange_flower_to_present_in *p_in)
{
	sprintf(this->sqlstr,"update %s set dynamic_count = dynamic_count - %u where \
		   	userid = %u and type = %u ",
			this->get_table_name(), p_in->num, userid, p_in->type);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 获取动态数量与总量
 */
int Csysarg_ranklist::get_double_count(userid_t userid, sysarg_get_double_count_in *p_in,
		sysarg_get_double_count_out *out)
{
	sprintf(this->sqlstr,"select dynamic_count,count from %s where userid = %u and type = %u",
			this->get_table_name(),userid,p_in->type);

	STD_QUERY_ONE_BEGIN(this->sqlstr,  USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out->dynamic_count);
		INT_CPY_NEXT_FIELD(out->count);
	STD_QUERY_ONE_END();
}

