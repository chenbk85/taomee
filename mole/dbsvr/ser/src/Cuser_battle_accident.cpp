/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_accicent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/12/2011 10:26:31 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_battle_accident.h"

Cuser_battle_accident::Cuser_battle_accident(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_battle_accident","userid")
{

}

int Cuser_battle_accident::insert(userid_t userid, uint32_t itemid, uint32_t fail_cnt, 
		uint32_t fight_cnt, uint32_t today, uint32_t wish_cnt)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u, 0, 0)",
			this->get_table_name(userid),
			userid,
			itemid,
			fail_cnt,
			fight_cnt,
			today,
			wish_cnt
			);


    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_battle_accident::get_cnts(userid_t userid, uint32_t *fail_cnt, uint32_t * fight_cnt)
{
	sprintf(this->sqlstr, "select fail_cnt, fight_cnt from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*fail_cnt);
		INT_CPY_NEXT_FIELD (*fight_cnt);
	STD_QUERY_ONE_END();
}
int Cuser_battle_accident::update_two_column(userid_t userid, uint32_t wish_id, uint32_t wish_cnt)
{
	sprintf(this->sqlstr, "update %s set itemid = %u, wish_cnt = %u where userid = %u",
			this->get_table_name(userid),
			wish_id,
			wish_cnt,
			userid
			);
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_battle_accident::update_wish_info(userid_t userid, uint32_t wish_id, uint32_t today)
{
	sprintf(this->sqlstr, "update %s set itemid = %u, date = %u, wish_cnt = 1 where userid = %u",
			this->get_table_name(userid),
			wish_id,
			today,
			userid
			);
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_accident::add_wish_item(userid_t userid, uint32_t wish_id, uint32_t wish_cnt, 
		uint32_t today, uint32_t date)
{
	int ret = 0;
	if(date == today){
		 ret = update_two_column(userid, wish_id, wish_cnt+1);	
	}
	else{
		ret = update_wish_info(userid, wish_id, today);
	}
	if(ret == USER_ID_NOFIND_ERR){
		ret = insert(userid, wish_id, 0, 0, today, 1);
	}

	return ret;
}
int Cuser_battle_accident::get_two_column(userid_t userid, uint32_t *wish_id, uint32_t *date)
{
	    sprintf(this->sqlstr, "select itemid, date from %s where userid = %u",
				this->get_table_name(userid),
				userid
				);
		STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD (*wish_id);
			INT_CPY_NEXT_FIELD (*date);
		STD_QUERY_ONE_END();
}
int Cuser_battle_accident::get_three_column(userid_t userid, uint32_t *wish_id, uint32_t *date,
		uint32_t *wish_cnt)
{
	    sprintf(this->sqlstr, "select itemid, date, wish_cnt from %s where userid = %u",
				this->get_table_name(userid),
				userid
				);
		STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD (*wish_id);
			INT_CPY_NEXT_FIELD (*date);
			INT_CPY_NEXT_FIELD (*wish_cnt);
		STD_QUERY_ONE_END();
}
int Cuser_battle_accident::get_double_col(uint32_t userid, const char* col_1, const char* col_2,
	   	uint32_t *value_1, uint32_t *value_2)
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
