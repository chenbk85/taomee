/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_db_sports_petscore.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/24/2010 02:55:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_db_sports_petscore.h"



Csysarg_db_sports_petscore::Csysarg_db_sports_petscore(mysql_interface * db)
	:Ctable(db, "SYSARG_DB", "t_sysarg_db_sports_petscore")
{

}


int Csysarg_db_sports_petscore::score_insert(uint32_t userid, uint32_t petid, uint32_t gameid, uint32_t score,
		char* p_pet_nick)
{
	char pet_nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(pet_nick_mysql, p_pet_nick, NICK_LEN);
	sprintf(this->sqlstr, "insert into %s(userid, petid, gameid, score, nick) values(%u, %u, %u, %u, '%s')",
		   	this->get_table_name(),
			userid,
			petid,
			gameid,
			score,
			pet_nick_mysql
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);

}


int Csysarg_db_sports_petscore::score_update(uint32_t userid, uint32_t petid, uint32_t gameid, uint32_t score)
{
	sprintf(this->sqlstr, "update %s set score = score + %u  where userid = %u\
			and petid = %u and gameid = %u",
		   	this->get_table_name(),
			score,
			userid,
			petid,
			gameid
			);
	STD_SET_RETURN_EX(this->sqlstr, ROOMINFO_SPORTS_PETSCORE_NO_FIND_ERR);

}

int Csysarg_db_sports_petscore::score_get(uint32_t userid, uint32_t gameid,
		sysarg_db_sports_petscore_get_out_item **pp_list, uint32_t* p_count)
{
	sprintf(this->sqlstr, "select petid, score, nick from %s where userid=%u and gameid=%u",
		   this->get_table_name(),
		   userid,
		   gameid
		   );

    STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
        INT_CPY_NEXT_FIELD((*pp_list + i)->petid);
        INT_CPY_NEXT_FIELD((*pp_list + i)->score);
		BIN_CPY_NEXT_FIELD((*pp_list + i)->pet_nick, NICK_LEN);
    STD_QUERY_WHILE_END();
}
