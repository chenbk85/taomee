#ifndef DB_MAIL_HPP_
#define DB_MAIL_HPP_

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "dbproxy.hpp"
#include <vector>

using namespace std;

class player_t;

int db_mail_head_list(player_t* p);

int db_proto_mail_head_list_callback(DEFAULT_ARG);

int db_mail_body(player_t* p, uint32_t mail_id);

int db_proto_mail_body_callback(DEFAULT_ARG);

int db_del_mail(player_t* p, uint32_t mail_id);

int db_take_mail_enclosure(player_t* p, uint32_t mail_id);

int db_proto_take_mail_enclosure_callback(DEFAULT_ARG);

int db_send_mail(player_t* p, uint32_t receive_id, uint32_t receive_role_tm, uint8_t* title, uint8_t* content);

int db_proto_send_mail_callback(DEFAULT_ARG);

#endif
