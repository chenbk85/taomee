/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_wish_wall.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/09/2012 02:31:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_wish_wall.h"

#define PAGESIZE 9
Csysarg_wish_wall::Csysarg_wish_wall(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_wish_wall")
{

}


int Csysarg_wish_wall::add(uint32_t userid, char* nick)
{
	uint32_t count = 0;	
	get_rd_count(&count);
	if(count < 900){
		insert(userid, nick);
	}
	else{
		int ret = update_old(userid);
		if(ret == USER_ID_NOFIND_ERR){
			ret = update_other(userid, nick);
		}
	}
	return 0;
}

int Csysarg_wish_wall::insert(userid_t userid, char* nick)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	memset(nick_mysql, 0, sizeof(nick_mysql));
	set_mysql_string(nick_mysql,nick, NICK_LEN);

	sprintf(this->sqlstr, "insert into %s values(%u, '%s', %u)",
			this->get_table_name(),
			userid,
			nick_mysql,
			(uint32_t)time(0)
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_wish_wall::update_old(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set datetime = %u where userid = %u",
			this->get_table_name(),
			(uint32_t)time(0),
			userid
		   );

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_wish_wall::update_other(userid_t userid, char* nick)
{

	char nick_mysql[mysql_str_len(NICK_LEN)];
	memset(nick_mysql, 0, sizeof(nick_mysql));
	set_mysql_string(nick_mysql,nick, NICK_LEN);

	sprintf(this->sqlstr, "update %s set userid = %u, nick = '%s', datetime = %u order by datetime asc limit 1",
			this->get_table_name(),
			userid,
			nick_mysql,
			(uint32_t)time(0)
		   );

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Csysarg_wish_wall::get_rd_count(uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s for update",
			this->get_table_name()
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();
}

int Csysarg_wish_wall::get_total_page(uint32_t *page_total)
{
	uint32_t total = 0;
	get_rd_count(&total);
	if(total % PAGESIZE == 0){
		*page_total = total / PAGESIZE;
	}
	else{
		*page_total = total / PAGESIZE + 1;
	}
	return 0;
}

int Csysarg_wish_wall::get_some_userid(userid_t userid, user_sysarg_get_wish_wall_in *p_in, 
		user_sysarg_get_wish_wall_out_item** pp_list, uint32_t *p_count)
{
	uint32_t page = 0;
	if(p_in->page >= 1){
		page = p_in->page - 1;
	}
	sprintf(this->sqlstr, "select userid, nick from %s limit %u, 9",
			this->get_table_name(),
			page*PAGESIZE
			);

    STD_QUERY_WHILE_BEGIN(this-> sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD ((*pp_list+i)->userid);
        BIN_CPY_NEXT_FIELD ((*pp_list+i)->nick, NICK_LEN );
    STD_QUERY_WHILE_END();

}
