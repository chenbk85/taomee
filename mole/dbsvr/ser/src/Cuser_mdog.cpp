/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_mdog.h
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

#include "Cuser_mdog.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_mdog::Cuser_mdog(mysql_interface * db, Citem_change_log *p_log) 
	:CtableRoute100x10( db, "USER", "t_user_mdog", "userid")
{ 
	p_item_change_log = p_log;
}


int Cuser_mdog::insert(userid_t userid, uint32_t mdog) 
{
	sprintf(this->sqlstr, "insert into %s (userid, mdog) values(%u, %u)",
			this->get_table_name(userid),
			userid,
			mdog
			);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
	//STD_INSERT_WITHOUT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
	
}

int Cuser_mdog::add(userid_t userid, uint32_t mdog, uint32_t is_vip_opt_type)
{
	int ret = insert(userid, mdog);
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, mdog, 1);
	}
	return ret;
}

int Cuser_mdog::get_mdog_info(uint32_t userid, uint32_t mdog, 
		user_mdog_query_mdog_info_out &out)
{
	sprintf(this->sqlstr, "select end_time, plant_water, insent_kill, animal_drink,\
			 animal_catch, flag from %s where userid = %u and mdog = %u",
			this->get_table_name(userid),
			userid,
			mdog
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(out.time);
        INT_CPY_NEXT_FIELD(out.plant_water);
        INT_CPY_NEXT_FIELD(out.insent_kill);
        INT_CPY_NEXT_FIELD(out.animal_drink);
        INT_CPY_NEXT_FIELD(out.animal_catch);
		INT_CPY_NEXT_FIELD(out.flag);
    STD_QUERY_ONE_END();
}

int Cuser_mdog::get_mdog(uint32_t userid, uint32_t mdog, uint32_t &count)
{
    sprintf(this->sqlstr, "select count(*) from %s where userid = %u and mdog = %u",
            this->get_table_name(userid),
            userid,
			mdog
            );
    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(count);
    STD_QUERY_ONE_END();
}

int Cuser_mdog::get_mdog_count(uint32_t userid, uint32_t mdog, user_mdog_query_user_mdog_out& out)
{
	sprintf(this->sqlstr, "select count(*) as icount from %s where userid = %u and mdog = %u",
			this->get_table_name(userid),
			userid,
			mdog
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out.count);
	STD_QUERY_ONE_END();
}



int Cuser_mdog::get_mdog_end_time(uint32_t userid, uint32_t mdog, uint32_t &end_time)
{
	sprintf(this->sqlstr, "select end_time from %s where userid = %u and mdog = %u",
			this->get_table_name(userid),
			userid,
			mdog
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(end_time);
	STD_QUERY_ONE_END();
}


int Cuser_mdog::del(userid_t userid, uint32_t mdog)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and mdog = %u",
		this->get_table_name(userid),
		userid,
		mdog
		);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_mdog::set_end_time(userid_t userid, uint32_t mdog, uint32_t end_time)
{
	sprintf(this->sqlstr, "update %s set end_time = %u where userid = %u and mdog = %u",
		this->get_table_name(userid),
		end_time,
		userid,
		mdog
		);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_mdog::update_kill_water(userid_t userid, uint32_t mdog, uint32_t insent_kill, uint32_t plant_water)
{
	sprintf(this->sqlstr, "update %s set insent_kill = insent_kill + %u, plant_water = plant_water + %u where userid = %u and mdog = %u",
		this->get_table_name(userid),
		insent_kill,
		plant_water,
		userid,
		mdog
		);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_mdog::update_drink_catch(userid_t userid, uint32_t mdog, uint32_t animal_drink, uint32_t animal_catch)
{
	sprintf(this->sqlstr, "update %s set animal_drink = animal_drink + %u ,animal_catch = animal_catch + %u where userid = %u and mdog = %u",
		this->get_table_name(userid),
		animal_drink,
		animal_catch,
		userid,
		mdog
		);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_mdog::update_flag(userid_t userid, uint32_t mdog, uint32_t flag)
{
	sprintf(this->sqlstr, "update %s set flag = %u where userid = %u and mdog = %u",
			this->get_table_name(userid),
			flag,
			userid,
			mdog);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
