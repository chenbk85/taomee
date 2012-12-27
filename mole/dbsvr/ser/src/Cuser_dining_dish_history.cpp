/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_dining_dish_history.cpp
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年03月09日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen, tommychen@taomee.com
 *
 * =====================================================================================
 */

#include "Cuser_dining_dish_history.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_dining_dish_history::Cuser_dining_dish_history(mysql_interface * db ) 
	:CtableRoute100x10( db, "USER", "t_user_dining_dish_history", "userid")
{ 
}

int Cuser_dining_dish_history::insert(userid_t userid, uint32_t dish_id, uint32_t count) 
{
	sprintf(this->sqlstr, "insert into %s (userid,dish_id,count) values(%u,%u,%u)",
			this->get_table_name(userid),
			userid,
			dish_id,
			count
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}


int Cuser_dining_dish_history::update_dish_history_inc(userid_t userid, uint32_t dish_id, uint32_t count) 
{
	sprintf(this->sqlstr, "update %s set count = count + 1  where userid = %u and dish_id = %u",
			this->get_table_name(userid),
			userid,
			dish_id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish_history::get_dish_history_count(userid_t userid, uint32_t& count)
{
	sprintf(this->sqlstr, "select sum(count) as num  from %s  where  userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DB_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cuser_dining_dish_history::dish_stars_get(uint32_t userid, uint32_t dish_id, uint32_t *p_dish_count, uint32_t *p_dish_stars)
{
	sprintf(this->sqlstr, "select count, dish_stars from %s where userid = %u and dish_id = %u",
			this->get_table_name(userid),
			userid,
			dish_id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, DISH_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_dish_count);
		INT_CPY_NEXT_FIELD(*p_dish_stars);
	STD_QUERY_ONE_END();

}

int Cuser_dining_dish_history::dish_stars_set(userid_t userid, uint32_t dish_id, uint32_t dish_stars) 
{
	sprintf(this->sqlstr, "update %s set dish_stars = %u  where userid = %u and dish_id = %u",
			this->get_table_name(userid),
			dish_stars,
			userid,
			dish_id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish_history::select_count_at_star_5(userid_t userid,uint32_t &count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and dish_stars = 5",
                        this->get_table_name(userid),
                        userid
                        );
        STD_QUERY_ONE_BEGIN(this->sqlstr, DISH_NOFIND_ERR);
                INT_CPY_NEXT_FIELD(count);
        STD_QUERY_ONE_END();
}

int Cuser_dining_dish_history::select_dish_star(userid_t userid, select_dish_stars_out_item **pp_list, uint32_t *p_count)
{
       sprintf(this->sqlstr, "select dish_id, count, dish_stars from %s where userid = %u",
                        this->get_table_name(userid),
                        userid
                        );
        STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->dish_id);
                INT_CPY_NEXT_FIELD((*pp_list + i)->dish_count);
                INT_CPY_NEXT_FIELD((*pp_list + i)->dish_star);
         STD_QUERY_WHILE_END();
}
