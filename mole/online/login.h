#ifndef ONLINE_LOGIN_H_
#define ONLINE_LOGIN_H_

#include "benchapi.h"

//------------------------ Callbacks -------------------------------
int proc_final_login_step(sprite_t* p, uint32_t step);

//------------------ Default Login Maps Related Funcs -----------------
int init_default_login_maps();

//------------------------ CMDs for proto.c -------------------------
int auth_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int login_server_op(uint32_t uid, void* buf, int len);

int check_session_by_session_server(sprite_t *p, int game_id, const uint8_t *body, int flag);

int check_session_by_session_server_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int check_session_by_session_server_tw_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int check_session(sprite_t *p);

int check_userid_exsit_callback(sprite_t *p, uint32_t id, const char *buf, int len);

int get_online_time_cmd(sprite_t* p, const uint8_t* body, int bodylen);

#endif // ONLINE_LOGIN_H_
