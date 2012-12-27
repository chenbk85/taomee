#ifndef MYPROTO_H
#define MYPROTO_H
#include "libtype.h"

int get_role_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
int create_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
int delete_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
int get_role_list_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
int add_game_flag_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
int gf_register_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
int delete_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

#endif
