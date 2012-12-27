/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_day_limit.cpp
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

#include "Croominfo_day_limit.h"
/**
 * @brief 构造函数，分成十个库每个库十个表
 */
Croominfo_day_limit::Croominfo_day_limit(mysql_interface *db) : CtableRoute10x10(db, "ROOMINFO", "t_roominfo_day_limit", "userid")
{

}

int Croominfo_day_limit::get(const userid_t userid, const uint32_t petid, uint32_t &change_value)
{
	sprintf(this->sqlstr, "select change_value from %s \
						   where userid = %u and petid = %u",
						   this->get_table_name(userid),
						   userid,
						   petid
		  );

	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(change_value);
	STD_QUERY_ONE_END();
}

int Croominfo_day_limit::get_num(const userid_t userid, const uint32_t petid, uint32_t &num)
{
	sprintf(this->sqlstr, "select num from %s \
						   where userid = %u and petid = %u",
						   this->get_table_name(userid),
						   userid,
						   petid
		  );

	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(num);
	STD_QUERY_ONE_END();
}


int Croominfo_day_limit::get_user_change_value_list(const userid_t userid, uint32_t *p_count, 
		user_su_get_change_value_out_item **pp_item)
{
	sprintf(this->sqlstr, "select petid, change_value from %s \
						   where userid = %u ",
						   this->get_table_name(userid),
						   userid
		  );
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_item, p_count ) ;
		INT_CPY_NEXT_FIELD ((*pp_item+i)->petid);
		INT_CPY_NEXT_FIELD ((*pp_item+i)->change_value);
	STD_QUERY_WHILE_END();
}



int Croominfo_day_limit::insert(const userid_t userid, const uint32_t petid)
{
	sprintf(this->sqlstr, "insert into %s (userid,petid) values(%u, %u)",
						   this->get_table_name(userid), 
						   userid,
						   petid
			);

	STD_INSERT_RETURN(this->sqlstr, RECORD_EXIST_ERR);
}

int Croominfo_day_limit::set(const userid_t userid, const uint32_t petid, const int change_value)
{
	sprintf(this->sqlstr, "update %s set change_value = %u where userid = %u and petid = %u",
					       this->get_table_name(userid),
						   change_value,
					       userid,
						   petid
	       );

	STD_SET_RETURN_EX(this->sqlstr, RECORD_NOT_EXIST_ERR);
}


int Croominfo_day_limit::update_num_inc(const userid_t userid, const uint32_t petid, const int num)
{
	sprintf(this->sqlstr, "update %s set num = num + %u where userid = %u and petid = %u",
					       this->get_table_name(userid),
						   num,
					       userid,
						   petid
	       );

	STD_SET_RETURN_EX(this->sqlstr, RECORD_NOT_EXIST_ERR);
}





