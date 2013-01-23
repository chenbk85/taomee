#ifndef PEA_MAIL_H_
#define PEA_MAIL_H_

#include <vector>
#include "CtableRoute100x10.h"
#include "proto.hpp"
#include "benchapi.h"

class pea_mail: public CtableRoute100x10
{
public:
	pea_mail(mysql_interface* db);
	int mail_head_list(Cmessage * c_in, Cmessage * c_out);
	int query_mail_body(Cmessage * c_in, Cmessage * c_out);
	int del_mail(Cmessage * c_in, Cmessage * c_out);
public:
	int send_mail(db_user_id_t* db_user, uint8_t* sender_nick, uint32_t recevie_id, uint32_t recevie_role_tm, uint8_t* title, uint8_t* content, uint32_t time, uint32_t* mail_id);
	int set_mail_read(db_user_id_t* db_user, uint32_t mail_id);
	int clear_mail_enclosure(db_user_id_t* db_user, uint32_t mail_id);
	int get_total_mail_count(uint32_t userid, uint32_t role_tm, uint32_t server_id, uint32_t* count);
};












#endif
