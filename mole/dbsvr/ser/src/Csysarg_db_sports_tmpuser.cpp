/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_db_sports_tmpuser.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/17/2010 10:01:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_db_sports_tmpuser.h"
/*
 *brief : 拉姆运动会，个人临时信息 20100520
 *
 *
 *
 *
 *
 * */


Csysarg_db_sports_tmpuser::Csysarg_db_sports_tmpuser(mysql_interface * db)
	:Ctable(db, "SYSARG_DB", "t_sysarg_db_sports_tmpuser")
{

}

int Csysarg_db_sports_tmpuser::tmpuser_create()
{
	sprintf(this->sqlstr, "create table %s (userid int(11) NOT NULL DEFAULT '0',\
		score int(11) NOT NULL DEFAULT '0',\
			flag int(11) NOT NULL DEFSULT '0',\
			PRIMARY KEY(userid)) ENGINE=innodb, CHARSET=utf8",
			this->get_table_name()
			);
	STD_INSERT_RETURN(this->sqlstr, SYSARG_DB_SPORTS_TMPUSER_ERR);
}

int Csysarg_db_sports_tmpuser::userid_select(uint32_t userid, uint32_t * flag)
{
	sprintf(this->sqlstr, "select flag from %s where userid=%u",
			this->get_table_name(),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SYSARG_DB_SPORTS_USER_NOFIND_ERR);
	INT_CPY_NEXT_FIELD(*flag);
	STD_QUERY_ONE_END();
}

int Csysarg_db_sports_tmpuser::score_insert(uint32_t userid, uint32_t score)
{
	sprintf(this->sqlstr, "insert into %s (userid, score) values (%u, %u)",
			this->get_table_name(),
			userid,
			score
			);
	STD_SET_RETURN_EX (this->sqlstr, SYSARG_DB_SPORTS_TMPUSER_ERR);
}

int Csysarg_db_sports_tmpuser::score_select(uint32_t userid, uint32_t *score)
{
	sprintf(this->sqlstr, "select score from %s where userid=%u",
			this->get_table_name(),
			userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SYSARG_DB_SPORTS_USER_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*score);
	STD_QUERY_ONE_END();
}

int Csysarg_db_sports_tmpuser::score_update(uint32_t userid, uint32_t score)
{
	sprintf(this->sqlstr, "update %s set score=%u where userid=%u",
			this->get_table_name(),
			score,
			userid
			);
	STD_SET_RETURN_EX (this->sqlstr, SYSARG_DB_SPORTS_TMPUSER_ERR);

}



int Csysarg_db_sports_tmpuser::userid_insert(uint32_t userid, uint32_t flag){
	sprintf(this->sqlstr, "insert into %s (userid, flag) values (%u, %u)",
			this->get_table_name(),
			userid,
			flag	
			);
	STD_SET_RETURN_EX (this->sqlstr, SYSARG_DB_SPORTS_TMPUSER_ERR);
}

int Csysarg_db_sports_tmpuser::userid_update(uint32_t userid, uint32_t flag){
	sprintf(this->sqlstr, "update %s set flag=%u where userid=%u",
			this->get_table_name(),
			flag,
			userid
			);
	STD_SET_RETURN_EX (this->sqlstr, SYSARG_DB_SPORTS_TMPUSER_ERR);

}

int Csysarg_db_sports_tmpuser::tmpuser_del()
{
	sprintf(this->sqlstr, "delete from %s",
			this->get_table_name()
			);
	STD_INSERT_RETURN(this->sqlstr, SYSARG_DB_SPORTS_TMPUSER_ERR);
}
