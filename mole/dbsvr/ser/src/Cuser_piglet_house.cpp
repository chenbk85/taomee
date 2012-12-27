/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_house.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/06/2011 04:54:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_piglet_house.h"
#include "common.h"
#include "proto.h"


Cuser_piglet_house::Cuser_piglet_house(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_piglet_house", "userid")
{

}
int Cuser_piglet_house::insert(userid_t userid, uint32_t logintime)
{

	char description[56] = { 0 };
	char des_mysql[mysql_str_len(sizeof(description))];
	set_mysql_string(des_mysql, description, 56);

	sprintf(this->sqlstr, "insert into %s values(%u, 0, 0, 1000, 0, 12, 0, 0, %u, 1, 0, 0, 0, \
		0, 1, '%s', 0, 0, 1613116, 1, 1, 1)",
			this->get_table_name(userid),
			userid,
			logintime,
			des_mysql
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_piglet_house::update_inc_col(userid_t userid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u where userid = %u",
			this->get_table_name(userid),
			col,
			col,
			value,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_house::update_inc_double(userid_t userid, const char* col_1, const char* col_2, 
		uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u, %s = %s + %u where userid = %u",
			this->get_table_name(userid),
			col_1,
			col_1,
			value_1,
			col_2,
			col_2,
			value_2,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_piglet_house::update_double_col(userid_t userid, const char* col_1,  const char* col_2, 
		uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u where userid = %u",
			this->get_table_name(userid),
			col_1,
			value_1,
			col_2,
			value_2,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_house::get_double_col(userid_t userid, const char* col_1, const char* col_2, 
		uint32_t *value_1, uint32_t *value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*value_1);
		INT_CPY_NEXT_FIELD(*value_2);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet_house::get_login_game(userid_t userid, uint32_t *exp, uint32_t *level)
{
	sprintf(this->sqlstr, "select exp, level from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*exp);
		INT_CPY_NEXT_FIELD(*level);
	STD_QUERY_ONE_END();	
}
int Cuser_piglet_house::get_piglet_house(userid_t userid, user_get_piglet_house_out_header* out,
		 uint32_t *logintime)
{
	sprintf(this->sqlstr, "select exp, level, honor, feed_cnt, process_cnt, formation, \
			bath_time, logintime, description, factory_level, beauty_level, background, machine_level from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	uint32_t now = time(0);
	uint32_t datetime = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out->exp);
		INT_CPY_NEXT_FIELD(out->level);
		INT_CPY_NEXT_FIELD(out->honor);
		INT_CPY_NEXT_FIELD(out->feed_cnt);
		INT_CPY_NEXT_FIELD(out->process_cnt);
		INT_CPY_NEXT_FIELD(out->formation);
		INT_CPY_NEXT_FIELD(datetime);
		if(now >= datetime + 6*3600){
			out->bath_time = 0;
		}
		else{
			out->bath_time = 6*3600 - (now - datetime);
		}
		INT_CPY_NEXT_FIELD(*logintime);
		BIN_CPY_NEXT_FIELD(out->description, 56);
		INT_CPY_NEXT_FIELD(out->factory_level);
		INT_CPY_NEXT_FIELD(out->beauty_level);
		INT_CPY_NEXT_FIELD(out->background);
		INT_CPY_NEXT_FIELD(out->machine_level);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet_house::entry_piglet_house(userid_t userid, user_get_piglet_house_out_header* out, 
		uint32_t *logintime, uint32_t *first_flag)
{
	int ret = get_piglet_house(userid, out, logintime); 

	uint32_t now = time(0);
	if(ret != SUCC){
		ret = insert(userid, now);	
		if(ret == SUCC){
			*first_flag = 1;
			//out->factory_level = 1;
			//*logintime = now;
			ret = get_piglet_house(userid, out, logintime);
		}
	}

	return ret;
}

int Cuser_piglet_house::clear(userid_t userid, uint32_t now)
{
	sprintf(this->sqlstr, "update %s set process_cnt = 0, perform_cnt = 0, is_ticket = 1000, \
			rob_cnt = 0, feed_cnt = 0, prob_cnt = 0, sale_cnt = 0, logintime = %u where userid = %u",
			this->get_table_name(userid),
			now,
			userid
			);
	 
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_house::get_level(uint32_t userid, uint32_t* level)
{
	this->get_int_value(userid, "level", level);	
	return 0;
}

int Cuser_piglet_house::cal_factory_level(uint32_t userid, uint32_t *factory_level)
{
	uint32_t lvl = 0;
	get_level(userid, &lvl);
	if(lvl <= 3){
		 *factory_level = 1;
	}
	else if( lvl <= 9){

		 *factory_level = 2;
	}
	else if( lvl <= 19){
		 *factory_level = 3;
	}
	else if( lvl <= 39){
		 *factory_level = 4;

	}
	else{
		 *factory_level = 5;
	}
	return 0;
}
int Cuser_piglet_house::feed_piglets(userid_t userid, uint32_t feed_type, uint32_t exp)
{
	sprintf(this->sqlstr, "update %s set feed_type = %u, feed_cnt = feed_cnt + 1, \
			exp = exp + %u where userid = %u",
			this->get_table_name(userid),
			feed_type,
			exp,
			userid
			);
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_house::change_formation(userid_t userid, user_change_piglet_formation_in *p_in)
{
	DEBUG_LOG("!!!!!!!!formation: %s !!!!!!!!!!!!!!", p_in->des);
	char des_mysql[mysql_str_len(56)];
	set_mysql_string(des_mysql, p_in->des, 56);

	sprintf(this->sqlstr,"update %s set formation = %u, description = '%s' where userid = %u",
			this->get_table_name(userid),
			p_in->forma,
			des_mysql,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_house::set_poll(userid_t userid, uint32_t index)
{

	uint32_t poll = 0;
	this->get_int_value(userid, "poll", &poll);
	
	if(index == 1 || index == 2 || index == 4 || index == 8 || index == 16)
	{
		this->set_int_value(userid, "poll", poll | index);
	}

	if(index == 32)
	{
		this->set_int_value(userid, "poll", 32);
	}

	return 0;
}

int Cuser_piglet_house::get_levels(userid_t userid, uint32_t *exp, uint32_t *level,
		uint32_t *machine_level, uint32_t *warehouse_level )
{
	sprintf(this->sqlstr, "select exp, level, machine_level, warehouse_level from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*exp);
		INT_CPY_NEXT_FIELD(*level);
		INT_CPY_NEXT_FIELD(*machine_level);
		INT_CPY_NEXT_FIELD(*warehouse_level);
	STD_QUERY_ONE_END();	

}
