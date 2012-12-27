/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_contribute_rank.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/09/2010 07:15:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_contribute_rank.h"
#include "proto.h"
#include "benchapi.h"

Csysarg_contribute_rank::Csysarg_contribute_rank(mysql_interface * db) :
	Ctable(db, "SYSARG_DB","t_sysarg_contribute_rank")
{

}

/*
 * @brief
 */
int Csysarg_contribute_rank::insert(uint32_t userid, uint32_t contri_xiaomee, uint32_t contri_attire,uint32_t guess_right)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u)",
				this->get_table_name(), userid, contri_xiaomee, contri_attire, guess_right);
	
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*  
 * @brief 修改字段的值
 * @param field：字段名
 */
int Csysarg_contribute_rank::update(userid_t userid, const char *field, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u where userid = %u", 
			this->get_table_name(), field, field, value, userid);
	
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 得到排名
 */
int Csysarg_contribute_rank::get_rank(sysarg_contribute_get_rank_out_item **pp_list, const char *field, uint32_t *p_count)
{
 	sprintf(this->sqlstr, "select userid, %s from %s where %s > 0 order by %s desc limit 10", 
			field, this->get_table_name(), field, field);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
	STD_QUERY_WHILE_END();
}

/*
 * @brief 得到捐赠的总人数，捐钱的人数，捐物的人数
int Csysarg_contribute_rank::get_user_count(sysarg_contribute_get_user_count_out *p_out)
{
	sprintf(this->sqlstr, "select count(*) from %s where contri_xiaomee > 0 or guess_sum > 0", this->get_table_name());
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(p_out->user_cnt);
	STD_QUERY_ONE_END();

	this->get_type_count("contri_xiaomee", &(p_out->xiaomee_cnt));
	this->get_type_count("contri_attire", &(p_out->attire_cnt));
	
	return SUCC;
}

 */
/*
 * brief 得到捐赠的总人数
int Csysarg_contribute_rank::get_type_count(const char *filed, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where %s > 0", this->get_table_name(), filed);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}
*/
