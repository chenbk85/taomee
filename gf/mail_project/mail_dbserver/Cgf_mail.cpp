#include <algorithm>
#include "Cgf_mail.h"

using namespace std;



Cgf_mail::Cgf_mail(mysql_interface * db )
	:CtableRoute( db,"GF" ,"t_gf_mail","userid")
{
	
}


int Cgf_mail::get_mail_head_list(userid_t userid, uint32_t role_regtime, gf_mail_head_list_out_element** pData, uint32_t* count )
{
	GEN_SQLSTR(sqlstr,"select mail_id, mail_time, mail_state, mail_templet, mail_type, sender_id, sender_role_time, mail_title  from %s where receive_id = %u and ( receive_role_time = 0 || receive_role_time = %u) order by mail_id desc limit 100", get_table_name(userid), userid,  role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pData, count);
        	INT_CPY_NEXT_FIELD( (*pData+i)->mail_id);
	        INT_CPY_NEXT_FIELD( (*pData+i)->mail_time);
			INT_CPY_NEXT_FIELD( (*pData+i)->mail_state);
		    INT_CPY_NEXT_FIELD( (*pData+i)->mail_templet);
	   		INT_CPY_NEXT_FIELD( (*pData+i)->mail_type);
	 		INT_CPY_NEXT_FIELD( (*pData+i)->sender_id);
			INT_CPY_NEXT_FIELD( (*pData+i)->sender_role_tm);
			BIN_CPY_NEXT_FIELD( (*pData+i)->mail_title,  sizeof((*pData+i)->mail_title));
	STD_QUERY_WHILE_END();	
}

int Cgf_mail::get_mail_count(userid_t userid, uint32_t role_regtime,  uint32_t* count)
{
	*count = 0;
	GEN_SQLSTR(sqlstr, "select count(*) from %s where receive_id = %u and ( receive_role_time = 0 || receive_role_time = %u)",
			 get_table_name(userid), userid,  role_regtime);
	STD_QUERY_ONE_BEGIN(sqlstr, 0);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();
}

int Cgf_mail::get_mail_body(userid_t userid, uint32_t mail_id, gf_mail_body_out* out)

{
	GEN_SQLSTR(sqlstr, "select mail_id, mail_content, numerical_enclosure, item_enclosure, equip_enclosure from %s where mail_id = %u",  get_table_name(userid),  mail_id);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_MAIL_ID_NOT_EXIST);
			INT_CPY_NEXT_FIELD( out->mail_id);
			BIN_CPY_NEXT_FIELD( out->mail_content, sizeof(out->mail_content));
			BIN_CPY_NEXT_FIELD( out->mail_numerical_enclosure,  sizeof(out->mail_numerical_enclosure));
			BIN_CPY_NEXT_FIELD( out->mail_item_enclosure, sizeof(out->mail_item_enclosure));	
			BIN_CPY_NEXT_FIELD( out->mail_equip_enclosure, sizeof(out->mail_equip_enclosure));
	STD_QUERY_ONE_END();
}

int Cgf_mail::set_mail_read_flag(userid_t userid, uint32_t mail_id, uint32_t flag)
{
	GEN_SQLSTR(sqlstr, "update %s set mail_state = %u where mail_id = %u", get_table_name(userid),  flag, mail_id);
	return exec_update_list_sql(sqlstr,SUCC);	
}

int Cgf_mail::delete_mail(userid_t userid, uint32_t mail_id)
{
	GEN_SQLSTR(sqlstr, "delete from %s where mail_id = %u",  get_table_name(userid), mail_id);
	return exec_update_list_sql(sqlstr, SUCC);
}


int Cgf_mail::clear_mail_enclosures(userid_t userid, uint32_t mail_id)
{
	GEN_SQLSTR(sqlstr, "update %s set numerical_enclosure = '', item_enclosure = '', equip_enclosure = '' where mail_id = %u", 
			  get_table_name(userid), mail_id);
	return exec_update_list_sql(sqlstr, SUCC);
}


int Cgf_mail::insert_system_mail(userid_t userid, uint32_t role_regtime, gf_send_system_mail_in* p_in, gf_send_mail_out* out)
{
	out->mail_time = time(NULL);
	out->mail_state = 2;
	out->mail_templet = p_in->mail_templet;
	out->mail_type = 1;
	out->sender_id = p_in->sender_id;
	out->receive_id = p_in->receive_id;
	strcpy(out->mail_title, p_in->mail_title);

	GEN_SQLSTR(sqlstr, "insert into %s (mail_time, mail_state, mail_templet, mail_type, sender_id, sender_role_time, receive_id, receive_role_time, mail_title, mail_content, numerical_enclosure, item_enclosure, equip_enclosure) values(%u,%u,%u,%u,%u,%u,%u,%u,'%s','%s', '%s', '%s', '%s')",
			get_table_name(userid),
			out->mail_time,
			out->mail_state,
			out->mail_templet,
			out->mail_type,
			out->sender_id,
			0,
			out->receive_id,
			role_regtime,
			out->mail_title,
			p_in->mail_body,
			p_in->mail_num_enclosure,
			p_in->mail_item_enclosure,
			p_in->mail_equip_enclosure
			);
	return exec_insert_sql_get_auto_increment_id(sqlstr, SUCC, &out->mail_id);
}

int Cgf_mail::insert_user_mail(userid_t userid, gf_send_mail_in* p_in, gf_send_mail_out* out)
{
	out->mail_time = time(NULL);
	out->mail_state = 2;
	out->mail_templet = p_in->mail_templet;
	out->mail_type = 2;
	out->sender_id = p_in->sender_id;
	out->receive_id = p_in->receive_id;
	strcpy(out->mail_title, p_in->mail_title);

	GEN_SQLSTR(sqlstr, "insert into %s (mail_time, mail_state, mail_templet, mail_type, sender_id, sender_role_time, receive_id, receive_role_time, mail_title, mail_content) values(%u,%u,%u,%u,%u,%u,%u,%u,'%s','%s')",
				get_table_name(userid),		
				out->mail_time,
			    out->mail_state,
			    out->mail_templet,	
				out->mail_type,
				out->sender_id,
				0,
				out->receive_id,
				0,
				out->mail_title,
				p_in->mail_body
			  );
	return exec_insert_sql_get_auto_increment_id(sqlstr, SUCC, &out->mail_id);
}

int Cgf_mail::insert_sys_nor_mail(userid_t userid, uint32_t role_regtime, uint32_t tmpl_id, const char* mail_title, const char* mail_content)
{

	char title_mysql[mysql_str_len(strlen(mail_title))];
	set_mysql_string(title_mysql, mail_title, strlen(mail_title));
	char content_mysql[mysql_str_len(strlen(mail_content)+1)];
	set_mysql_string(content_mysql, mail_content, strlen(mail_content) + 1);


	GEN_SQLSTR(sqlstr, "insert into %s (mail_time, mail_state, mail_templet, mail_type, \
										sender_id, sender_role_time, receive_id, receive_role_time, \
										mail_title, mail_content, numerical_enclosure, item_enclosure, \
										equip_enclosure) values(unix_timestamp(),2,%u,1,\
										0,0,%u,%u,\
										'%s','%s', '', '', \
										'')",
			get_table_name(userid),
			tmpl_id,
			userid, role_regtime,
			title_mysql, content_mysql);
	return exec_update_sql(sqlstr, SUCC);
}










