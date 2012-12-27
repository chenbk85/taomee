/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_master.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/26/2011 11:44:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_battle_master.h"

Cuser_battle_master::Cuser_battle_master(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_battle_master", "userid")
{

}
int Cuser_battle_master::insert(userid_t userid, uint32_t prentice, uint32_t ini_lvl, uint32_t thank_flag)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, 0, %u, %u)",
			this->get_table_name(userid),
			userid,
			prentice,
			ini_lvl,
			thank_flag
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}


int Cuser_battle_master::clear_train_info(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set train = 0 where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}
int Cuser_battle_master::get_train(userid_t userid, uint32_t prentice, uint32_t *train)
{
	sprintf(this->sqlstr, "select train from %s where userid = %u and prentice = %u",
			this->get_table_name(userid),
			userid,
			prentice
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*train);
	STD_QUERY_ONE_END();
}

int Cuser_battle_master::update_train_by_id(userid_t userid, uint32_t prentice, uint32_t train_id)
{
	uint32_t train  = 0;
	int ret = get_train(userid, prentice, &train);
	if (ret != SUCC) {
		return ret;
	}

	uint32_t mask =  1 << (train_id - 1);
	if (mask & train) {
		return USER_RESPECT_MASTER_ERR;
	}

	train = train | mask; 
	sprintf(this->sqlstr, "update %s set train = %u where userid = %u and prentice = %u",
			this->get_table_name(userid),
			train,
			userid,
			prentice
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_master::recive_prentice_res(userid_t userid, uint32_t prentice)
{
	sprintf(this->sqlstr, "update %s set thank_flag = 0 where userid = %u and prentice = %u",
			this->get_table_name(userid),
			userid,
			prentice
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_master::get_all_prentices(userid_t userid, uint32_t** pp_list, uint32_t* p_count)
{
	*p_count = 0;
	sprintf(this->sqlstr, "select prentice from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD(*(*pp_list + i));
	STD_QUERY_WHILE_END();
}
int Cuser_battle_master::get_all_prentices_info(userid_t userid, user_battle_get_self_mp_relatrion_out_item** pp_list, 
		uint32_t* p_count)
{
	sprintf(this->sqlstr, "select prentice, initial_level, thank_flag from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	*p_count = 0;
	uint32_t thank_flag = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->prentice);
		INT_CPY_NEXT_FIELD((*pp_list + i)->ini_level);
		INT_CPY_NEXT_FIELD(thank_flag);
		if(thank_flag == 1){	//如果可领，设置为1
			(*pp_list + i)->is_recv_resp = 1;
		}
		else{
			(*pp_list + i)->is_recv_resp = 0;
		}
	STD_QUERY_WHILE_END();
}
int Cuser_battle_master::get_train_info(userid_t userid, uint32_t prentice, user_battle_get_train_info_out_item *p_list,
	   uint32_t *p_count)
{
	uint32_t train = 0;
	int ret = get_train(userid, prentice, &train);
	if (ret != SUCC) {
		return ret;
	}
	for(uint32_t k = 0; k < 6; ++k){
		if (train & (1 << k)) {
			(p_list + *(p_count))->train_id = k + 1;
			++(*p_count);
		}
	}

	return ret; 
}
int Cuser_battle_master::remove_prentice(userid_t userid, uint32_t prentice_id)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and prentice = %u",
			this->get_table_name(userid),
			userid,
			prentice_id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_battle_master::update_one_col(userid_t userid, uint32_t prentice, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u and prentice = %u",
			this->get_table_name(userid),
			col,
			value,
			userid,
			prentice
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_battle_master::get_one_col(userid_t userid, uint32_t prentice, const char* col, uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u and prentice = %u",
			col,
			this->get_table_name(userid),
			userid,
			prentice
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value);
	STD_QUERY_ONE_END();
}
int Cuser_battle_master::get_prentice_count(userid_t userid, uint32_t *cnt)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*cnt);
	STD_QUERY_ONE_END();
}
