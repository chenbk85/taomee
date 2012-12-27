/*
 * =====================================================================================
 *
 *       Filename:  Cuser_new_friendship_box.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/13/2012 10:29:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "Cuser_new_friendship_box.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
#include <msglog.h>

Cuser_new_friendship_box::Cuser_new_friendship_box(mysql_interface * db)
	:CtableRoute100x10(db, "USER", "t_user_new_friendship_box", "userid")
{
}

int Cuser_new_friendship_box::init_record(userid_t userid)
{
	get_friend_box_items_out items;
	get_friend_box_history_out history;
	share_src_t	shares;

	char items_mysql[mysql_str_len(sizeof(get_friend_box_items_out))];
	char history_mysql[mysql_str_len(sizeof(get_friend_box_history_out))];
	char share_mysql[mysql_str_len(sizeof(share_src_t))];

	memset(&items,0,sizeof(items));
	memset(&history,0,sizeof(history));
	memset(&shares,0,sizeof(shares));

	set_mysql_string(items_mysql,(char*)&items,sizeof(get_friend_box_items_out));
	set_mysql_string(history_mysql,(char*)&history,sizeof(get_friend_box_history_out));
	set_mysql_string(share_mysql,(char*)&shares,sizeof(share_src_t));

	sprintf(this->sqlstr, "insert into %s values(%u,%u,'%s','%s','%s')",
		this->get_table_name(userid),userid,get_date(time(NULL)),items_mysql,history_mysql,share_mysql);

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_new_friendship_box::get_item_list(userid_t userid,get_friend_box_items_out *p_out)
{
	sprintf(this->sqlstr, "select items from %s where userid=%u",
				        this->get_table_name(userid),userid);
    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
	    BIN_CPY_NEXT_FIELD(p_out,sizeof(*p_out));
	STD_QUERY_ONE_END();
}

int Cuser_new_friendship_box::set_item_list(userid_t userid,get_friend_box_items_out *p_out)
{
	char items_mysql[mysql_str_len(sizeof(*p_out))];
	set_mysql_string(items_mysql,(char*)p_out,sizeof(*p_out));

	sprintf(this->sqlstr, "update %s set items='%s' where userid=%u",
		this->get_table_name(userid),items_mysql,userid);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);
}

int Cuser_new_friendship_box::get_history_list(userid_t userid,get_friend_box_history_out *p_out)
{
	sprintf(this->sqlstr, "select history from %s where userid=%u",
				        this->get_table_name(userid),userid);
    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
	    BIN_CPY_NEXT_FIELD(p_out,sizeof(*p_out));
	STD_QUERY_ONE_END();

}

int Cuser_new_friendship_box::set_history_list(userid_t userid,get_friend_box_history_out *p_out)
{
	char history_mysql[mysql_str_len(sizeof(*p_out))];
	set_mysql_string(history_mysql,(char*)p_out,sizeof(*p_out));

	sprintf(this->sqlstr, "update %s set history='%s' where userid=%u",
		this->get_table_name(userid),history_mysql,userid);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);
}

int Cuser_new_friendship_box::get_share_src_list(userid_t userid,uint32_t &date,share_src_t *p_out)
{
	sprintf(this->sqlstr, "select today,share_src from %s where userid=%u",
				        this->get_table_name(userid),userid);
    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
	    INT_CPY_NEXT_FIELD(date);
	    BIN_CPY_NEXT_FIELD(p_out,sizeof(*p_out));
	STD_QUERY_ONE_END();

}

int Cuser_new_friendship_box::set_share_src_list(userid_t userid,uint32_t date,share_src_t *p_out)
{
	char share_mysql[mysql_str_len(sizeof(*p_out))];
	set_mysql_string(share_mysql,(char*)p_out,sizeof(*p_out));

	sprintf(this->sqlstr, "update %s set today=%u,share_src='%s' where userid=%u",
		this->get_table_name(userid),date,share_mysql,userid);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);
}


