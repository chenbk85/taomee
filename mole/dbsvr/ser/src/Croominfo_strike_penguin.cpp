/*
 * =====================================================================================
 *
 *       Filename:  Cuser_strike_benguin.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/23/2011 11:01:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_strike_penguin.h"

Croominfo_strike_penguin::Croominfo_strike_penguin(mysql_interface *db):
	CtableRoute10x10(db, "ROOMINFO", "t_roominfo_strike_penguin",
			"userid")
{

}

int Croominfo_strike_penguin::insert(userid_t userid, uint32_t barrier, uint32_t score,
	   	uint32_t is_award, uint32_t is_pass)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			barrier,
			score,
			get_date(time(0)),
			is_award,
			is_pass
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Croominfo_strike_penguin::update_score(userid_t userid, uint32_t barrier, uint32_t score)
{
	sprintf(this->sqlstr, "update %s set score = %u where userid = %u and barrier = %u",
			this->get_table_name(userid),
			score,
			userid,
			barrier
			);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}


int Croominfo_strike_penguin::update_date_award(userid_t userid, uint32_t barrier, uint32_t date, 
		uint32_t award_flag)
{
	sprintf(this->sqlstr, "update %s set date = %u, is_award = %u where userid = %u and barrier = %u",
			this->get_table_name(userid),
			date,
			award_flag,
			userid,
			barrier
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Croominfo_strike_penguin::update_score_date(userid_t userid, uint32_t barrier, uint32_t score, uint32_t date)
{
	sprintf(this->sqlstr, "update %s set score = %u, date = %u where userid = %u and barrier = %u",
			this->get_table_name(userid),
			score,
			date,
			userid,
			barrier
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Croominfo_strike_penguin::update_ispass(userid_t userid, uint32_t barrier, uint32_t pass_flag)
{
	sprintf(this->sqlstr, "update %s set is_pass = %u where userid = %u and barrier = %u",
			this->get_table_name(userid),
			pass_flag,
			userid,
			barrier
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Croominfo_strike_penguin::select(userid_t userid, uint32_t barrier, uint32_t * score, 
		uint32_t *date, uint32_t *pass_flag, uint32_t *award_flag)
{
	sprintf(this->sqlstr, "select score, date, is_pass, is_award from %s where userid = %u and barrier = %u",
			this->get_table_name(userid),
			userid,
			barrier
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*score);
		INT_CPY_NEXT_FIELD(*date);
		INT_CPY_NEXT_FIELD(*pass_flag);
		INT_CPY_NEXT_FIELD(*award_flag);
	STD_QUERY_ONE_END();
}

int Croominfo_strike_penguin::get_max_pass_berrier(userid_t userid, uint32_t *max_pass_tollgate)
{
	sprintf(this->sqlstr, "select barrier from %s where userid = %u and is_pass = 1 order by barrier desc limit 1",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*max_pass_tollgate);
	STD_QUERY_ONE_END();
}

int Croominfo_strike_penguin::select_history(userid_t userid, roominfo_strike_penguin_get_info_out_item 
		**pp_list, uint32_t *p_count)
{
	uint32_t today = get_date(time(0));
	uint32_t date = 0;
	sprintf(this->sqlstr, "select score, date, is_award, is_pass from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i )->score);
		INT_CPY_NEXT_FIELD(date);
		if(date == today){
			INT_CPY_NEXT_FIELD((*pp_list + i )->award_flag);
		}
		else{
			(*pp_list + i )->award_flag = 0;
		}
		INT_CPY_NEXT_FIELD((*pp_list + i )->pass_flag);
	STD_QUERY_WHILE_END();	
}

