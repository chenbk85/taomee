/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_db_sports_score.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/24/2010 10:08:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_db_sports_gamescore.h"


Csysarg_db_sports_gamescore::Csysarg_db_sports_gamescore(mysql_interface * db) :
		Ctable(db, "SYSARG_DB", "t_sysarg_db_sports_gamescore")
{

}

int Csysarg_db_sports_gamescore::min_score_select(uint32_t gameid, uint32_t* score)
{
	sprintf(this->sqlstr, "select min(score) from %s where gameid = %u limit 1",
			this->get_table_name(),
			gameid	
		);	
	STD_QUERY_ONE_BEGIN(this->sqlstr, SYSARG_DB_SPORTS_GAMESCORE_NO_FIND_ERR);
		INT_CPY_NEXT_FIELD(*score);
	STD_QUERY_ONE_END();

}

int Csysarg_db_sports_gamescore::min_score_delete(uint32_t gameid, uint32_t min_score)
{
	sprintf(this->sqlstr, "delete from %s  where gameid = %u and score = %u limit 1" ,
			        this->get_table_name(),
					gameid,
					min_score
					);
	STD_SET_RETURN_EX(this->sqlstr, SYSARG_DB_SPORTS_GAMESCORE_NO_FIND_ERR);
}

int Csysarg_db_sports_gamescore::score_update(uint32_t userid, uint32_t gameid, uint32_t score)
{
	sprintf(this->sqlstr, "update %s set score = score + %u where gameid = %u and userid = %u " ,
			        this->get_table_name(),
					score,
					gameid,
					userid
					);
	STD_SET_RETURN_EX(this->sqlstr, SYSARG_DB_SPORTS_GAMESCORE_NO_FIND_ERR);

}

int Csysarg_db_sports_gamescore::score_insert(uint32_t userid, uint32_t gameid, uint32_t score, char* p_user_nick)
{
	char user_nick_mysql[mysql_str_len(NICK_LEN)];
	
	set_mysql_string(user_nick_mysql, p_user_nick, NICK_LEN);
	sprintf(this->sqlstr, "insert into %s(userid, gameid, score, nick) values(%u, %u, %u, '%s')",
			this->get_table_name(),
			userid,
			gameid,
			score,
			user_nick_mysql
		);	
	STD_SET_RETURN_EX(this->sqlstr, SYSARG_DB_SPORTS_GAMESCORE_NO_FIND_ERR);
}

int Csysarg_db_sports_gamescore::max_score_select(uint32_t gameid, uint32_t* p_count,
		sysarg_db_sports_gamescore_get_out_item** pp_list)
{
	sprintf(this->sqlstr, "select userid,score,nick from %s where gameid = %u order by score desc limit 10",
			this->get_table_name(),
			gameid	
		);	

    STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->score);
		BIN_CPY_NEXT_FIELD((*pp_list + i)->user_nick, NICK_LEN); 
	STD_QUERY_WHILE_END();

}

int Csysarg_db_sports_gamescore::top9_score_select(uint32_t userid, uint32_t *p_score)
{
	sprintf(this->sqlstr, "select score from %s where gameid = 6 and userid = %u",
			this->get_table_name(),
			userid	
		);	
    STD_QUERY_ONE_BEGIN(this->sqlstr, SYSARG_DB_SPORTS_GAMESCORE_NO_FIND_ERR);
		INT_CPY_NEXT_FIELD(*p_score);
	STD_QUERY_ONE_END();

}
