/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_game_hall.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/2012 05:00:59 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_game_hall.h"



Csysarg_game_hall::Csysarg_game_hall(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_game_hall")
{

}


int Csysarg_game_hall::insert(uint32_t userid, char* nick, uint32_t exp, uint32_t gameid,
		uint32_t score, uint32_t datetime)
{
	if(gameid != 0){
		char user_name_mysql[mysql_str_len(NICK_LEN)];
		set_mysql_string(user_name_mysql, nick,NICK_LEN);

		sprintf(this->sqlstr, "insert into %s values( %u, '%s', %u, %u, %u, %u)",
				this->get_table_name(),
				userid,
				user_name_mysql,
				exp,
				gameid,
				score,
				datetime
				);

		STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
	}
	else{
		return 0;
	}
}

int Csysarg_game_hall::update(uint32_t userid, uint32_t exp, uint32_t datetime)
{
	sprintf(this->sqlstr, "update %s set exp = exp + %u, datetime = %u where userid = %u",
			this->get_table_name(),
			exp,
			datetime,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Csysarg_game_hall::update_two_col(userid_t userid, const char* col_1, const char* col_2, 
		uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u where userid = %u",
			this->get_table_name(),
			col_1,
			value_1,
			col_2,
			value_2,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_game_hall::update_one_col(userid_t userid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u",
			this->get_table_name(),
			col,
			value,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_game_hall::change_value(userid_t userid, uint32_t exp, uint32_t gameid, uint32_t score,
		uint32_t datetime)
{
	if(gameid != 0){
		sprintf(this->sqlstr, "update %s set exp = %u, gameid = %u, score = %u, \
			datetime = %u where userid = %u",
			this->get_table_name(),
			exp,
			gameid,
			score,
			datetime,
			userid
			);
	}
	else{
		sprintf(this->sqlstr, "update %s set exp = %u where userid = %u",
			this->get_table_name(),
			exp,
			userid
			);

	}
	
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_game_hall::get_recent_ten(sysarg_get_game_top_ten_out_item **pp_list, uint32_t* p_count)
{
	sprintf(this->sqlstr, "select userid, nick, gameid, score, datetime from %s \
			order by datetime desc limit 10",
			this->get_table_name()
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list+i)->userid);
		BIN_CPY_NEXT_FIELD((*pp_list + i)->nick, NICK_LEN);
		INT_CPY_NEXT_FIELD((*pp_list+i)->gameid);
		INT_CPY_NEXT_FIELD((*pp_list+i)->score);
		INT_CPY_NEXT_FIELD((*pp_list+i)->datetime);
	STD_QUERY_WHILE_END();	
}

int Csysarg_game_hall::change_hall_data(userid_t userid, sysarg_game_hall_change_in *p_in)
{	

	uint32_t now = time(0);
	int m_ret = this->change_value(userid, p_in->exp, p_in->gameid, p_in->cur_score, now);

	if(m_ret != SUCC){
		m_ret = insert(userid, p_in->nick, p_in->exp, p_in->gameid, p_in->cur_score, now);
	}
	return m_ret;
}

int Csysarg_game_hall::get_self_exp(userid_t userid, uint32_t *exp)
{
	sprintf(this->sqlstr, "select exp from %s where userid = %u",
			this->get_table_name(),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*exp);
	STD_QUERY_ONE_END();	
}

int Csysarg_game_hall::get_self_rank(userid_t userid, uint32_t *rank)
{
	uint32_t exp = 0;
	get_self_exp(userid, &exp);

	sprintf(this->sqlstr, "select count(*) from %s where exp < %u",
			this->get_table_name(),
			exp
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*rank);
	STD_QUERY_ONE_END();	
}
