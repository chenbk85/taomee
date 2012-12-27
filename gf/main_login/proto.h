#ifndef PROTO_H
#define PROTO_H
#include "asyc_main_login_type.h"
//extern "C" {
//#include <libtaomee/dirtyword/dirtyword.h>
//}
extern usr_info_t all_players[MAX_CONNECTION];

int dispatch(void* data, fdsession_t* fdsess);
int get_uid_by_mail_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
int check_pwd_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
int check_usr_login(usr_info_t* p);
int login_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
usr_info_t* get_usr_by_fd(int fd);
int check_dirty_word(usr_info_t* p, char* msg);
#endif
