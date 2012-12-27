/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_master_prentice_msg.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/26/2011 09:27:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_battle_master_prentice_msg.h"

Cuser_battle_master_prentice_msg::Cuser_battle_master_prentice_msg(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_battle_mater_prentice_info", "userid")
{

}
int Cuser_battle_master_prentice_msg::get_all(userid_t userid, user_battle_get_master_prentice_info_out_item 
		**pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select master, prentice, datetime, train_id, exp, merit from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->master);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->prentice);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->datetime);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->train_id);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->exp);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->merit);
	STD_QUERY_WHILE_END();
}
int Cuser_battle_master_prentice_msg::insert(userid_t userid, uint32_t master, uint32_t prentice, 
		uint32_t train_id, uint32_t exp, uint32_t merit)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			(uint32_t)time(0),
			master,
			prentice,
			train_id,
			exp,
			merit
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_battle_master_prentice_msg::get_oldest(userid_t userid, uint32_t *datetime)
{
	sprintf(this->sqlstr, "select MIN(datetime) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*datetime);
	STD_QUERY_ONE_END();
}
int Cuser_battle_master_prentice_msg::update(userid_t userid, user_battle_get_master_prentice_info_out_item *p_item,
	   	uint32_t datetime)
{
	sprintf(this->sqlstr, "update %s set datetime = %u, master = %u , prentice = %u, train_id = %u, \
			exp = %u, merit = %u where userid = %u and datetime = %u",
			this->get_table_name(userid),
			p_item->datetime,
			p_item->master,
			p_item->prentice,
			p_item->train_id,
			p_item->exp,
			p_item->merit,
			userid,
			datetime
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_master_prentice_msg::get_record_cnt(userid_t userid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (count);
	STD_QUERY_ONE_END();
}

int Cuser_battle_master_prentice_msg::update_record(userid_t userid, user_battle_get_master_prentice_info_out_item 
		*p_item)
{
	uint32_t cnt = 0;
	get_record_cnt(userid, cnt);
	if(cnt < 50){
		insert(userid, p_item->master, p_item->prentice, p_item->train_id, p_item->exp, p_item->merit);

	}
	else{
		uint32_t datetime = 0;
		get_oldest(userid, &datetime);
		update(userid, p_item, datetime);
	}
	return 0;
	
}
