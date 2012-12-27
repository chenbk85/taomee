#ifndef NET_H
#define NET_H
#include "asyc_main_login_type.h"

extern int mainlogin_dbproxysvr_fd;
extern uint8_t cli_buf[BUFFER_SIZE];
int send_request_to_mainlogin_db(usr_info_t* p, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);
int send_to_self_error(usr_info_t* p, uint32_t error);
int send_to_self(usr_info_t* p, const uint8_t *buf, uint32_t length);
void init_proto_head(void* buf, uint32_t len, uint32_t cmd, userid_t uid, uint32_t result);
int send_warning(usr_info_t* p, int type, int cmdid);
#endif
