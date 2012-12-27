/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_user_cake.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/27/2010 01:55:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_user_cake.h"

Croominfo_user_cake::Croominfo_user_cake(mysql_interface *db):
	CtableRoute10x10(db, "ROOMINFO", "t_roominfo_user_cake", "userid")
{

}

/*
 * @brief 插入记录
 */
int Croominfo_user_cake::add_cake(userid_t userid, uint32_t date, uint32_t cakeid)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, 0, 0)",
			this->get_table_name(userid), userid, date, cakeid);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief更新
 */
int Croominfo_user_cake::update_state(userid_t userid, uint32_t cur_date, uint32_t cakeid, uint32_t state)
{
	sprintf(this->sqlstr, "update %s set state = %u where userid = %u and date = %u and cakeid = %u ",
			this->get_table_name(userid), state, userid, cur_date, cakeid);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Croominfo_user_cake::update_state_level(userid_t userid, uint32_t cur_date, uint32_t cakeid,  uint32_t state, uint32_t level)
{
	sprintf(this->sqlstr, "update %s set state = %u, level = %u where userid = %u and date = %u and cakeid = %u",
			this->get_table_name(userid), state, level, userid, cur_date, cakeid);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


/*
 * @brief 查询
 */
int Croominfo_user_cake::query_count(userid_t userid, uint32_t t_date, uint32_t* num)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and date = %u",
			this->get_table_name(userid), userid, t_date);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*num);
	STD_QUERY_ONE_END();
}

int Croominfo_user_cake::query_count_state(userid_t userid, uint32_t t_date, uint32_t* num)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and date = %u and state = 2",
			this->get_table_name(userid), userid, t_date);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*num);
	STD_QUERY_ONE_END();
}

int Croominfo_user_cake::get_state_level(userid_t userid, uint32_t t_date, uint32_t cakeid, uint32_t& state, uint32_t& level)
{
	sprintf(this->sqlstr, "select state, level from %s where userid = %u and date = %u and cakeid = %u",
			this->get_table_name(userid), userid, t_date, cakeid);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(state);
		INT_CPY_NEXT_FIELD(level);
	STD_QUERY_ONE_END();
}


int Croominfo_user_cake::get_date_cake_info(uint32_t userid, uint32_t cur_date, roominfo_query_user_cake_info_out_item  **pp_items, uint32_t *p_count)
{									
	sprintf(this->sqlstr, "select date, cakeid, state, level  from %s where userid = %u and date = %u ",
								this->get_table_name(userid), userid, cur_date);
		
		STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_items, p_count);
			INT_CPY_NEXT_FIELD((*pp_items + i)->date);
			INT_CPY_NEXT_FIELD((*pp_items + i)->cakeid);
			INT_CPY_NEXT_FIELD((*pp_items + i)->state);
			INT_CPY_NEXT_FIELD((*pp_items + i)->level);
		STD_QUERY_WHILE_END();
}

int Croominfo_user_cake::get_cake_info(uint32_t userid, roominfo_query_user_cake_info_out_item  **pp_items, uint32_t *p_count)
{									
	sprintf(this->sqlstr, "select date, cakeid, state, level  from %s where userid = %u ",
								this->get_table_name(userid), userid);
		
		STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_items, p_count);
			INT_CPY_NEXT_FIELD((*pp_items + i)->date);
			INT_CPY_NEXT_FIELD((*pp_items + i)->cakeid);
			INT_CPY_NEXT_FIELD((*pp_items + i)->state);
			INT_CPY_NEXT_FIELD((*pp_items + i)->level);
		STD_QUERY_WHILE_END();
}


