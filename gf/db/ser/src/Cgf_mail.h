#ifndef _CGF_MAIL_H
#define _CGF_MAIL_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"




class Cgf_mail: public CtableRoute
{
public:
	Cgf_mail(mysql_interface * db);
	int get_mail_head_list(userid_t userid, uint32_t role_regtime, gf_mail_head_list_out_element** pData, uint32_t* count );
	int get_mail_body(userid_t userid, uint32_t mail_id,  gf_mail_body_out* out);
	int set_mail_read_flag(userid_t userid, uint32_t mail_id, uint32_t flag);
	int delete_mail(userid_t userid, uint32_t mail_id);
	int clear_mail_enclosures(userid_t userid, uint32_t mail_id);
	int get_mail_count(userid_t userid, uint32_t role_regtime,  uint32_t* count);
	int insert_user_mail(userid_t userid,  gf_send_mail_in* p_in, gf_send_mail_out* out);
	int insert_system_mail(userid_t userid, uint32_t role_regtime, gf_send_system_mail_in* p_in, gf_send_mail_out* out);
	int insert_sys_nor_mail(userid_t userid, uint32_t role_regtime, uint32_t tmpl_id, const char* mail_title, const char* mail_content);
	int clear_role_info(userid_t userid,uint32_t role_regtime);
};

















#endif
