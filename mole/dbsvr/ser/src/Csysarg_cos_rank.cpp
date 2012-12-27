/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_cos_rank.cpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  12/13/2010 03:22:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_cos_rank.h"
Csysarg_cos_rank::Csysarg_cos_rank(mysql_interface * db) :
	Ctable(db, "SYSARG_DB","t_sysarg_cos_rank")
{

}

int Csysarg_cos_rank::insert(uint32_t userid, uint32_t count,  char* nick)
{
	char user_name_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(user_name_mysql, nick,NICK_LEN);
	uint32_t today = get_date(time(0));

	sprintf(this->sqlstr, "insert into %s values(%u, %u, '%s', %u)",
			this->get_table_name(),
			today,
			userid,
			user_name_mysql,
			count
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_cos_rank::update_last_record(uint32_t datetime, uint32_t userid, uint32_t count, char* nick)
{
	char user_name_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(user_name_mysql, nick,NICK_LEN);

	sprintf(this->sqlstr, "update %s set userid = %u, nick = '%s', count = %u where datetime = %u \
		and count < %u order by count asc limit 1",
		this->get_table_name(), userid,  user_name_mysql, count, datetime, count);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);	
}


int Csysarg_cos_rank::get_cos_rank(uint32_t datetime, sysarg_get_cos_rank_out_item **pp_items, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select userid, nick, count from %s where datetime = %u order by count desc  limit 10",
		   			this->get_table_name(), datetime);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_items, p_count);
		INT_CPY_NEXT_FIELD((*pp_items + i)->userid);
		BIN_CPY_NEXT_FIELD(&((*pp_items + i)->nick), NICK_LEN);
		INT_CPY_NEXT_FIELD((*pp_items + i)->count);
	STD_QUERY_WHILE_END();
}

int Csysarg_cos_rank::update_count(uint32_t datetime, uint32_t userid,  uint32_t cnt)
{
	sprintf(this->sqlstr, "update %s set count = %u where userid = %u and datetime = %u",
			this->get_table_name(),
			cnt,
			userid,
			datetime
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_cos_rank::get_min_count(uint32_t datetime,  uint32_t *count)
{
	sprintf(this->sqlstr, "select min(count) from %s where datetime = %u ",
			this->get_table_name(),
			datetime
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Csysarg_cos_rank::get_record_count(uint32_t datetime,  uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where datetime = %u ",
			this->get_table_name(),
			datetime
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Csysarg_cos_rank::get_tenth_count(uint32_t datetime,  uint32_t *count)
{
	sprintf(this->sqlstr, "select count from %s where datetime = %u  order by count desc  limit 10, 1",
			this->get_table_name(),
			datetime
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Csysarg_cos_rank::get_user_record(uint32_t userid,  uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u ",
			this->get_table_name(),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Csysarg_cos_rank::get_user_date_count(uint32_t userid, uint32_t datetime,  uint32_t *count)
{
	sprintf(this->sqlstr, "select count from %s where datetime = %u and userid = %u",
			this->get_table_name(),
			datetime,
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		*count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

