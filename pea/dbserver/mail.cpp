#include <algorithm>
#include "mail.hpp"
#include "pea_common.hpp"

pea_mail::pea_mail(mysql_interface* db)
    :CtableRoute100x10(db, "pea", "pea_mail", "user_id")
{
		
}

int pea_mail::mail_head_list(Cmessage * c_in, Cmessage * c_out)
{
	db_proto_mail_head_list_in * p_in = P_IN;
	db_proto_mail_head_list_out * p_out = P_OUT;

	uint32_t user_id = p_in->db_user_id.user_id;
	uint32_t role_tm = p_in->db_user_id.role_tm;
	uint32_t server_id = p_in->db_user_id.server_id;


	GEN_SQLSTR(sqlstr, "select mail_id, mail_time, mail_state, mail_templet, mail_type, sender_id, sender_role_tm, sender_nick, mail_title from %s where receive_id = %u and receive_role_tm = %u and server_id = %u order by mail_time desc limit %u", 
			get_table_name(user_id),
			user_id,
			role_tm,
			server_id,
			MAX_MAIL_COUNT
			);

	STD_QUERY_WHILE_BEGIN_NEW(sqlstr, p_out->head_infos);
	INT_CPY_NEXT_FIELD(item.mail_id);
	INT_CPY_NEXT_FIELD(item.mail_time);
	INT_CPY_NEXT_FIELD(item.mail_state);
	INT_CPY_NEXT_FIELD(item.mail_templet);
	INT_CPY_NEXT_FIELD(item.mail_type);
	INT_CPY_NEXT_FIELD(item.sender_id);
	INT_CPY_NEXT_FIELD(item.sender_role_tm);
	BIN_CPY_NEXT_FIELD(item.sender_nick, sizeof(item.sender_nick));
	BIN_CPY_NEXT_FIELD(item.mail_title, sizeof(item.mail_title));
	STD_QUERY_WHILE_END_NEW();
}

int pea_mail::query_mail_body(Cmessage * c_in, Cmessage * c_out)
{
	db_proto_mail_body_in* p_in = P_IN;
	db_proto_mail_body_out* p_out = P_OUT;

	uint32_t user_id = p_in->db_user_id.user_id;

	GEN_SQLSTR(sqlstr, "select mail_id, mail_content, numerical_enclosure, item_enclosure, equip_enclosure from %s where mail_id = %u limit 1", get_table_name(user_id), p_in->mail_id);
	
	STD_QUERY_ONE_BEGIN(sqlstr, DB_ERR_NOT_EXIST);
		INT_CPY_NEXT_FIELD(p_out->mail_id);
		BIN_CPY_NEXT_FIELD(p_out->mail_content, sizeof(p_out->mail_content));
		BIN_CPY_NEXT_FIELD(p_out->numerical_enclosure, sizeof(p_out->numerical_enclosure));
		BIN_CPY_NEXT_FIELD(p_out->item_enclosure, sizeof(p_out->item_enclosure));
		BIN_CPY_NEXT_FIELD(p_out->equip_enclosure, sizeof(p_out->equip_enclosure));
	STD_QUERY_ONE_END();
}

int pea_mail::set_mail_read(db_user_id_t* db_user, uint32_t mail_id)
{
	GEN_SQLSTR(sqlstr, "update %s set mail_state = 1 where mail_id = %u", 
				get_table_name(db_user->user_id),
				mail_id
				);
	return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);	
}

int pea_mail::clear_mail_enclosure(db_user_id_t* db_user, uint32_t mail_id)
{
	GEN_SQLSTR(sqlstr, "update %s set numerical_enclosure = '', item_enclosure = '', equip_enclosure = '' where mail_id = %u", 
			get_table_name(db_user->user_id),
			mail_id
			);
	return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_mail::get_total_mail_count(uint32_t userid, uint32_t role_tm, uint32_t server_id, uint32_t* count)
{
	*count = 0;
	GEN_SQLSTR(sqlstr, "select count(*) from %s where receive_id = %u and receive_role_tm = %u and server_id = %u limit 1", 
			get_table_name(userid),
			userid,
			role_tm,
			server_id
			);	
	STD_QUERY_ONE_BEGIN(sqlstr, DB_ERR_NOERROR)
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();
}

int pea_mail::del_mail(Cmessage * c_in, Cmessage * c_out)
{
	db_proto_del_mail_in* p_in = P_IN;
	db_proto_del_mail_out* p_out = P_OUT;
	
	uint32_t user_id = p_in->db_user_id.user_id;

	GEN_SQLSTR(sqlstr, "delete from %s where mail_id = %u", 
			get_table_name(user_id),
			p_in->mail_id
			);
	p_out->mail_id = p_in->mail_id;
	return exec_update_sql(sqlstr, DB_ERR_NOT_EXIST);
}

int pea_mail::send_mail(db_user_id_t* db_user, uint8_t* sender_nick, uint32_t recevie_id, uint32_t recevie_role_tm, uint8_t* title, uint8_t* content, uint32_t time,  uint32_t* mail_id)
{
	GEN_SQLSTR(sqlstr, "insert into %s set mail_time = %u, mail_state = 2, mail_type = 2, server_id = %u, sender_id = %u, sender_role_tm = %u, sender_nick = '%s', receive_id = %u, receive_role_tm = %u, mail_title = '%s', mail_content = '%s'",
			get_table_name(recevie_id),
			time,
			db_user->server_id,
			db_user->user_id,
			db_user->role_tm,
			sender_nick,
			recevie_id,
			recevie_role_tm,
			title,
			content
		);	
	return exec_insert_sql_get_auto_increment_id(sqlstr, DB_ERR_NOERROR, mail_id);
}

