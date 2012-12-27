/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_prentice.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/26/2011 01:25:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_battle_prentice.h"


Cuser_battle_prentice::Cuser_battle_prentice(mysql_interface *db):
	CtableRoute100x10(db,"USER", "t_user_battle_prentice", "userid")
{

}
int Cuser_battle_prentice::insert(userid_t userid, uint32_t master, uint32_t rev_cnt)
{
	sprintf(this->sqlstr, "insert into %s values(%u, 0, %u, 0, 0, 0, 0, 0, 0, %u)",
			this->get_table_name(userid),
			userid,
			master,
			(uint32_t)get_date(time(0))
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_battle_prentice::get_self_relation(userid_t userid, user_battle_get_self_mp_relatrion_out_header *out)
{
	sprintf(this->sqlstr, "select merit, succ_master, initial_level, master, respect from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	uint32_t respect = 0;	
	out->userid = userid;
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (out->merit);
		INT_CPY_NEXT_FIELD (out->finish_cnt);
		INT_CPY_NEXT_FIELD (out->level);
		INT_CPY_NEXT_FIELD (out->master);
		INT_CPY_NEXT_FIELD (respect);
		if(respect != 0){
			out->is_respect = 0;//表示已经送过
		}
		else{
			out->is_respect = 1;
		}
	STD_QUERY_ONE_END();
}
int Cuser_battle_prentice::get_two_col(userid_t userid, const char *col_1, const char *col_2, uint32_t *value_1,
		uint32_t *value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value_1);
		INT_CPY_NEXT_FIELD (*value_2);
	STD_QUERY_ONE_END();
}
int Cuser_battle_prentice::update_two_col(userid_t userid, const char *col_1, const char * col_2, uint32_t value_1, 
		uint32_t value_2)
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
int Cuser_battle_prentice::update_merit_inc(userid_t userid, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set merit = if(merit + %u > 20000, 20000, merit + %u) where userid = %u",
			this->get_table_name(userid),
			value,
			value,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_battle_prentice::clear(userid_t userid, uint32_t date)
{
	sprintf(this->sqlstr, "update %s set respect = 0, recv_cnt = 0, date = %u where userid = %u",
			this->get_table_name(userid),
			date,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_battle_prentice::get_three_col(userid_t userid, const char* col_1, const char* col_2, const char* col_3,
		uint32_t* value_1, uint32_t* value_2, uint32_t* value_3)
{
	sprintf(this->sqlstr, "select %s, %s, %s from %s where userid = %u",
			col_1,
			col_2,
			col_3,
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value_1);
		INT_CPY_NEXT_FIELD (*value_2);
		INT_CPY_NEXT_FIELD (*value_3);
	STD_QUERY_ONE_END();
}
