/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_msg.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/15/2011 04:23:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_piglet_msg.h"

Cuser_piglet_msg::Cuser_piglet_msg(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_piglet_msg", "userid")
{

}


int Cuser_piglet_msg::insert(userid_t userid,  uint32_t datetime, uint32_t type, 
		uint32_t other_id, int32_t itemid, uint32_t value, char* nick)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, nick, NICK_LEN);

	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %d, %u, '%s')",
			this->get_table_name(userid),
				userid,
				datetime,
				type,
				other_id,
				itemid,
				value,
				nick
				);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_piglet_msg::get_oldest(userid_t userid, uint32_t *datetime)
{
	sprintf(this->sqlstr, "select MIN(datetime) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*datetime);
	STD_QUERY_ONE_END();
}

int Cuser_piglet_msg::get_all(userid_t userid, user_get_all_msg_out_item **pp_list, uint32_t* p_count)
{
	sprintf(this->sqlstr, "select datetime, type, operator, itemid, value , nick from %s where userid = %u limit 50",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->datetime);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->type);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->other_id);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->itemid);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->value);
		BIN_CPY_NEXT_FIELD ((*pp_list + i)->nick, NICK_LEN);
	STD_QUERY_WHILE_END();
}

int Cuser_piglet_msg::get_record_cnt(userid_t userid, uint32_t &cnt)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (cnt);
	STD_QUERY_ONE_END();
}

int Cuser_piglet_msg::update_records(userid_t userid, user_get_all_msg_out_item *para)
{
	uint32_t r_cnt = 0;
	get_record_cnt(userid, r_cnt);
	DEBUG_LOG("!!!!!!!!!!!!!!!!!!!!!!!!  r_cnt: %u",r_cnt);
	if(r_cnt < 50){
		insert(userid, para->datetime, para->type, para->other_id, para->itemid, para->value, para->nick);
	}
	else{
		uint32_t datetime = 0;
		get_oldest(userid, &datetime);
		update(userid, para, datetime);
	}
	return 0;

}

int Cuser_piglet_msg::update(userid_t userid, user_get_all_msg_out_item* para, uint32_t datetime)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, para->nick, NICK_LEN);

	sprintf(this->sqlstr, "update %s set datetime = %u, type = %u, operator = %u, \
			itemid = %d, value = %u, nick = '%s' where userid = %u and datetime = %u",
			this->get_table_name(userid),
			para->datetime,
			para->type,
			para->other_id,
			para->itemid,
			para->value,
			nick_mysql,
			userid,
			datetime
		   );

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
