
#include "Cmail.h"


#define STR_UID			"userid"
#define STR_MAILID		"mailid"
#define STR_FLAG		"flag"
#define STR_TYPE		"type"
#define STR_THEMEID		"themeid"
#define STR_SENDERID	"senderid"
#define STR_SENDERNICK	"sender_nick"
#define STR_TITLE		"title"
#define STR_MESSAGE		"message"
#define STR_ITEMLIST	"item_list"
#define STR_IDLIST		"id_list"


Cmail::Cmail(mysql_interface* db) : CtableRoute(db, "MOLE2_USER", "t_mail", "userid", "mailid")
{

}

int Cmail::insert(userid_t userid, uint32_t mailid, stru_mole2_mail_info* p_in)
{
	char nick_mysql[mysql_str_len(sizeof(p_in->sender_nick))];
	char title_mysql[mysql_str_len(sizeof(p_in->title))];
	char message_mysql[mysql_str_len(sizeof(p_in->message))];
	char item_mysql[mysql_str_len(MAIL_ITEM_LIST_MAX)];
	char id_mysql[mysql_str_len(MAIL_ID_LIST_LEN)];
	DEBUG_LOG("XXXXX mailid:%d",mailid );

	p_in->title[sizeof(p_in->title) -1] = 0;
	p_in->message[sizeof(p_in->message) -1] = 0;
	p_in->sender_nick[sizeof(p_in->sender_nick) -1] = 0;

	set_mysql_string(nick_mysql, (char*)p_in->sender_nick, strlen(p_in->sender_nick));
	set_mysql_string(title_mysql, (char*)p_in->title, strlen(p_in->title));
	set_mysql_string(message_mysql, (char*)p_in->message, strlen(p_in->message));
	set_mysql_string(item_mysql, (char*)p_in->items, MAIL_ITEM_LIST_MAX);
	set_mysql_string(id_mysql, (char*)p_in->ids, MAIL_ID_LIST_LEN);

	GEN_SQLSTR(this->sqlstr, "insert into %s(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s) \
			values(%u, %u, %u, %u, %u, %u, '%s', '%s', '%s', '%s', '%s')",
			this->get_table_name(userid),
			STR_UID,
			STR_MAILID,
			STR_FLAG,
			STR_TYPE,
			STR_THEMEID,
			STR_SENDERID,
			STR_SENDERNICK,
			STR_TITLE,
			STR_MESSAGE,
			STR_ITEMLIST,
			STR_IDLIST,
			userid,
			mailid,
			p_in->flag,
			p_in->type,
			p_in->themeid,
			p_in->senderid,
			nick_mysql,
			title_mysql,
			message_mysql,
			item_mysql,
			id_mysql);

	DEBUG_LOG("XXXXX mailid:%s",this->sqlstr );
	return this->exec_insert_sql(this->sqlstr, MAILID_EXISTED_ERR);
}

int Cmail::del(userid_t userid, uint32_t mailid)
{
	GEN_SQLSTR(this->sqlstr, "update %s set flag = 9999 where %s=%u and %s=%u and themeid = 10004",
			this->get_table_name(userid), 
			STR_UID,	userid,
			STR_MAILID,	mailid);
	this->exec_update_sql(this->sqlstr, SUCC);

	GEN_SQLSTR(this->sqlstr, "delete from %s where %s=%u and %s=%u and themeid != 10004",
			this->get_table_name(userid), 
			STR_UID,	userid,
			STR_MAILID,	mailid);
	return this->exec_update_sql(this->sqlstr, SUCC);
}

int Cmail::del_all(userid_t userid)
{
	GEN_SQLSTR(this->sqlstr, "update %s set flag = 9999 where %s=%u and themeid = 10004",
			this->get_table_name(userid), 
			STR_UID,	userid);
	this->exec_update_sql(this->sqlstr, SUCC);

	GEN_SQLSTR(this->sqlstr, "delete from %s where %s=%u and themeid != 10004",
			this->get_table_name(userid), 
			STR_UID,	userid);
	return this->exec_update_list_sql(this->sqlstr, SUCC);
}

int Cmail::get_theme_count(userid_t userid, uint32_t themeid,uint32_t* p_count)
{
	GEN_SQLSTR(this->sqlstr, "select count(*) from %s where %s=%u and themeid=%u",
			this->get_table_name(userid),STR_UID,userid,themeid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MAILID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Cmail::count_get(userid_t userid, uint32_t* p_count)
{
	GEN_SQLSTR(this->sqlstr, "select count(*) from %s where %s=%u",
			this->get_table_name(userid),
			STR_UID,	userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MAILID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Cmail::simple_list_get(userid_t userid, user_mail_simple_list_get_out_item** pp_out_item, uint32_t* p_count)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s, %s, %s, %s from %s where %s=%u and flag != 9999 limit 100",
		STR_MAILID,
		STR_FLAG,
		STR_TYPE,
		STR_THEMEID,
		STR_SENDERID,
		STR_SENDERNICK,
		STR_TITLE,
		this->get_table_name(userid),
		STR_UID, userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->mailid);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->flag);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->type);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->themeid);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->senderid);
		BIN_CPY_NEXT_FIELD((*pp_out_item + i)->sender_nick, sizeof((*pp_out_item + i)->sender_nick));
		BIN_CPY_NEXT_FIELD((*pp_out_item + i)->title, sizeof((*pp_out_item + i)->title));		
	STD_QUERY_WHILE_END();
}


int Cmail::content_get(userid_t userid, uint32_t mailid, char* p_msg, char* p_item_list, uint32_t* p_flag, char* p_id_list)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s from %s where %s=%u and %s=%u",
		STR_MESSAGE,
		STR_ITEMLIST,
		STR_FLAG,
		STR_IDLIST,
		this->get_table_name(userid), 
		STR_UID,	userid,
		STR_MAILID,	mailid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MAILID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_msg, MAIL_MESSAGE_LEN);
		BIN_CPY_NEXT_FIELD(p_item_list, MAIL_ITEM_LIST_MAX);
		INT_CPY_NEXT_FIELD(*p_flag);
		BIN_CPY_NEXT_FIELD(p_id_list, MAIL_ID_LIST_LEN);
	STD_QUERY_ONE_END();
}

int Cmail::flag_set(userid_t userid, uint32_t mailid, uint32_t flag)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%s|%u where %s=%u and %s=%u",
			this->get_table_name(userid),
			STR_FLAG,	STR_FLAG,	flag,
			STR_UID,	userid,
			STR_MAILID,	mailid);
	return this->exec_update_sql(this->sqlstr, MAILID_NOFIND_ERR);
}

int Cmail::get_mail_item_list(userid_t userid, uint32_t mailid, item_list_t* p_list)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u and %s=%u",
		STR_ITEMLIST,
		this->get_table_name(userid), 
		STR_UID,	userid,
		STR_MAILID,	mailid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MAILID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_list, MAIL_ITEM_LIST_MAX);
	STD_QUERY_ONE_END();
}

int Cmail::set_mail_item_list(userid_t userid, uint32_t mailid, item_list_t* p_list)
{
	char item_mysql[mysql_str_len(MAIL_ITEM_LIST_MAX)] = {};
	set_mysql_string(item_mysql, (char*)p_list, sizeof(item_list_t) + p_list->count * sizeof(p_list->item[0]));

	GEN_SQLSTR(this->sqlstr, "update %s set %s='%s' where %s=%u and %s=%u",
		this->get_table_name(userid),
		STR_ITEMLIST,	item_mysql,
		STR_UID,		userid,
		STR_MAILID, 	mailid);
	return this->exec_update_sql(this->sqlstr, MAILID_NOFIND_ERR);
}


