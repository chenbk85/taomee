/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_sbh.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/01/2010 04:41:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tommychen, tommychen@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_sbh.h"
/**
 * @brief 构造函数，分成十个库每个库十个表
 */
Croominfo_sbh::Croominfo_sbh(mysql_interface *db) : CtableRoute10x10(db, "ROOMINFO", "t_roominfo_sbh", "userid")
{

}


int Croominfo_sbh::get_items_value(userid_t userid, uint32_t index1, uint32_t index2, uint32_t &value1, uint32_t &value2)
{
	sprintf(this->sqlstr, "select item_%u, item_%u from %s  where userid = %u",
							index1,
							index2,
						   	this->get_table_name(userid),
						   	userid
		  );

	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(value1);
        INT_CPY_NEXT_FIELD(value2);
	STD_QUERY_ONE_END();
}


int Croominfo_sbh::get_prove_all(userid_t userid, uint32_t&  prove_all, uint32_t& prove)
{
	sprintf(this->sqlstr, "select prove_all, prove from %s  where userid = %u",
						   	this->get_table_name(userid),
						   	userid
		  );

	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(prove_all);
        INT_CPY_NEXT_FIELD(prove);
	STD_QUERY_ONE_END();
}

int Croominfo_sbh::get_prove(userid_t userid, roominfo_user_get_prove_out* p_out )
{
	sprintf(this->sqlstr, "select prove_all, prove from %s  where userid = %u",
						   	this->get_table_name(userid),
						   	userid
		  );

	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(p_out->prove_all);
        INT_CPY_NEXT_FIELD(p_out->prove);
	STD_QUERY_ONE_END();
}




int Croominfo_sbh::insert(userid_t userid)
{
	sprintf(this->sqlstr, "insert into %s (userid) values(%u)",
						   this->get_table_name(userid), 
						   userid
			);

	STD_INSERT_RETURN(this->sqlstr, RECORD_EXIST_ERR);
}

int Croominfo_sbh::update_item_value_inc(userid_t userid, uint32_t index, int value)
{
	sprintf(this->sqlstr, "update %s set item_%u = item_%u + %u  where userid = %u ",
					       this->get_table_name(userid),
						   index,
						   index,
						   value,
					       userid
	       );

	STD_SET_RETURN_EX(this->sqlstr, RECORD_NOT_EXIST_ERR);
}


int Croominfo_sbh::update_prove_inc(userid_t userid, uint32_t prove)
{
	sprintf(this->sqlstr, "update %s set prove_all = prove_all + %u, prove = prove + %u where userid = %u",
					       this->get_table_name(userid),
						   prove,
						   prove,
					       userid
	       );

	STD_SET_RETURN_EX(this->sqlstr, RECORD_NOT_EXIST_ERR);
}

int Croominfo_sbh::update_prove_dec(userid_t userid, uint32_t prove)
{
	sprintf(this->sqlstr, "update %s set prove = prove - %u where userid = %u",
					       this->get_table_name(userid),
						   prove,
					       userid
	       );

	STD_SET_RETURN_EX(this->sqlstr, RECORD_NOT_EXIST_ERR);
}

int Croominfo_sbh::update_prove_all_inc(userid_t userid, uint32_t prove)
{
	sprintf(this->sqlstr, "update %s set prove_all = prove_all + %u  where userid = %u",
					       this->get_table_name(userid),
						   prove,
					       userid
	       );

	STD_SET_RETURN_EX(this->sqlstr, RECORD_NOT_EXIST_ERR);
}




