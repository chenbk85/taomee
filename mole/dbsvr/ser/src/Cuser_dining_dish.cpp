/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_dining_dish.cpp
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

#include "Cuser_dining_dish.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_dining_dish::Cuser_dining_dish(mysql_interface * db ) 
	:CtableRoute100x10( db, "USER", "t_user_dining_dish", "userid")
{ 
}

int Cuser_dining_dish::add_dish(userid_t userid, user_dining_cook_dish_in* p_in, uint32_t state, uint32_t& id) 
{
	sprintf(this->sqlstr, "insert into %s (id,userid,dish_id,state,location,count,sale_xiaomee) values(NULL,%u,%u,%u,%u,%u,%u)",
			this->get_table_name(userid),
			userid,
			p_in->dish_id,
			state,
			p_in->location,
			p_in->count,
			p_in->sale_xiaomee
			);
	STD_INSERT_GET_ID(this->sqlstr, DB_ERR, id);
}

int Cuser_dining_dish::put_dish_chest(userid_t userid, user_dining_dish_in_chest_in* p_in, uint32_t state) 
{
	sprintf(this->sqlstr, "update %s set state = %u,location = %u where userid = %u and id = %u",
			this->get_table_name(userid),
			state,
			p_in->location,
			userid,
			p_in->id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish::update_dish_dec(userid_t userid, user_dining_eat_dish_in* p_in) 
{
	sprintf(this->sqlstr, "update %s set count = count - 1  where userid = %u and id = %u and state = 6",
			this->get_table_name(userid),
			userid,
			p_in->id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish::update_dish_num_dec(userid_t userid, uint32_t id, uint32_t num) 
{
	sprintf(this->sqlstr, "update %s set count = count - %u  where userid = %u and id = %u and state = 6",
			this->get_table_name(userid),
			num,
			userid,
			id);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}


int Cuser_dining_dish::del_dish(userid_t userid, uint32_t id) 
{
	sprintf(this->sqlstr, "delete  from  %s  where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish::del_all_dish(userid_t userid) 
{
	sprintf(this->sqlstr, "delete from  %s  where userid = %u and state = 6 ",
			this->get_table_name(userid),
			userid
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish::get_user_dish_state(userid_t userid, user_dining_dish_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, dish_id, state, location,count,time,sale_xiaomee\
						   from %s where userid = %u and state = 6",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->dish_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->state);
		INT_CPY_NEXT_FIELD((*pp_list + i)->location);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->sale_xiaomee);
		(*pp_list + i)->time = time(NULL)-(*pp_list + i)->time;
	STD_QUERY_WHILE_END();
}



int Cuser_dining_dish::get_user_dish(userid_t userid, user_dining_dish_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, dish_id, state, location,count,time,sale_xiaomee\
						   from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->dish_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->state);
		INT_CPY_NEXT_FIELD((*pp_list + i)->location);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->sale_xiaomee);
		(*pp_list + i)->time = time(NULL)-(*pp_list + i)->time;
	STD_QUERY_WHILE_END();
}

int Cuser_dining_dish::get_dish_id_count(userid_t userid, uint32_t id, uint32_t& count)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DISH_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cuser_dining_dish::get_dish_id_state_count(userid_t userid, uint32_t id, uint32_t& count)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u and id = %u and state = 6",
			this->get_table_name(userid),
			userid,
			id);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DISH_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}


int Cuser_dining_dish::get_dish_bench_num(userid_t userid, uint32_t max_bench,  uint32_t& count)
{
	sprintf(this->sqlstr, "select count(*) as num  from %s  userid = %u and state < 6 and location > %u",
			this->get_table_name(userid),
			userid,
			max_bench
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DB_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}



int Cuser_dining_dish::get_dish_cooler_num(userid_t userid, uint32_t max_cooler,  uint32_t& count)
{
	sprintf(this->sqlstr, "select count(*) as num  from %s  userid = %u and state = 6 and location > %u",
			this->get_table_name(userid),
			userid,
			max_cooler
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DB_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}




int Cuser_dining_dish::get_dish_count(userid_t userid, uint32_t& count)
{
	sprintf(this->sqlstr, "select count(*) as num  from %s  userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DB_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cuser_dining_dish::fall_dish(userid_t userid, user_dining_fall_dish_in* p_in) 
{
	sprintf(this->sqlstr, "delete from  %s  where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			p_in->id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish::set_dish_id_time(userid_t userid, uint32_t id, uint32_t cur_time) 
{
	sprintf(this->sqlstr, "update %s set time = %u  where userid = %u and id = %u",
			this->get_table_name(userid),
			cur_time,
			userid,
			id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish::set_dish_id_state(userid_t userid, uint32_t id, uint32_t state) 
{
	sprintf(this->sqlstr, "update %s set state = %u  where userid = %u and id = %u",
			this->get_table_name(userid),
			state,
			userid,
			id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}


int Cuser_dining_dish::get_dish_id_state_location(userid_t userid, uint32_t id, uint32_t& state, uint32_t& location)
{
	sprintf(this->sqlstr, "select state, location from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DISH_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(state);
		INT_CPY_NEXT_FIELD(location);
	STD_QUERY_ONE_END();
}

int Cuser_dining_dish::get_dish_all_count(userid_t userid, uint32_t& count)
{
	sprintf(this->sqlstr, "select sum(count) as num  from %s  where userid = %u and state = 6",
			this->get_table_name(userid),
			userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DB_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cuser_dining_dish::get_dish_id_time(userid_t userid, uint32_t id, uint32_t& time)
{
	sprintf(this->sqlstr, "select time from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DISH_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(time);
	STD_QUERY_ONE_END();
}

int Cuser_dining_dish::get_dish_state_location_count(userid_t userid, uint32_t dish_id, uint32_t location, uint32_t& id,  uint32_t& count)
{
	sprintf(this->sqlstr, "select id, count as num  from %s where userid = %u and dish_id = %u and location = %u and state = 6",
			this->get_table_name(userid),
			userid,
			dish_id,
			location);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DISH_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(id);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cuser_dining_dish::update_dish_id_inc(userid_t userid, uint32_t id, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count = count + %u  where userid = %u and id = %u ",
			this->get_table_name(userid),
			count,
			userid,
			id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);

}

int Cuser_dining_dish::del_dish_location_state(userid_t userid, uint32_t dish_id, uint32_t location) 
{
	sprintf(this->sqlstr, "delete  from  %s  where userid = %u and dish_id = %u and location = %u",
			this->get_table_name(userid),
			userid,
			dish_id,
			location
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish::del_dish_id_state(userid_t userid, uint32_t id ) 
{
	sprintf(this->sqlstr, "delete  from  %s  where userid = %u and id = %u and state = 6",
			this->get_table_name(userid),
			userid,
			id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish::update_dish_num_dec(userid_t userid, user_dining_dec_dish_in* p_in) 
{
	sprintf(this->sqlstr, "update %s set count = count - %u  where userid = %u and id = %u and state = 6",
			this->get_table_name(userid),
			p_in->num,
			userid,
			p_in->id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish::get_dish_count_by_dish_id(userid_t userid, uint32_t dish_id, uint32_t& count)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u and dish_id = %u and state = 6",
			this->get_table_name(userid),
			userid,
			dish_id);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DISH_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cuser_dining_dish::update_dish_num_dec_by_dish_id(userid_t userid, uint32_t dish_id, uint32_t num) 
{
	sprintf(this->sqlstr, "update %s set count = count - %u  where userid = %u and dish_id = %u and state = 6",
			this->get_table_name(userid),
			num,
			userid,
			dish_id);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}

int Cuser_dining_dish::get_dish_count_by_location(userid_t userid, uint32_t location,  uint32_t& count)
{
	sprintf(this->sqlstr, "select count(*) as num  from %s  where  userid = %u and  location = %u and state < 6 ",
			this->get_table_name(userid),
			userid,
			location
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DB_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cuser_dining_dish::del_dish_id_state_by_dish_id(userid_t userid, uint32_t dish_id ) 
{
	sprintf(this->sqlstr, "delete  from  %s  where userid = %u and dish_id = %u and state = 6",
			this->get_table_name(userid),
			userid,
			dish_id
			);
	STD_SET_RETURN_EX(this->sqlstr, DISH_NOFIND_ERR);
}
