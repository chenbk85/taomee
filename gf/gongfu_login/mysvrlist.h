#ifndef MYSVRLIST_H
#define MYSVRLIST_H

#include "libtype.h"

int get_recommeded_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
int get_ranged_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
int login_gf_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret);
int get_recommeded_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret);
int get_ranged_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret);
int get_recommended_svr_list(usr_info_t* p);
int get_friend_time_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret);
int init_timer();
int update_backup_svrlist_callback(uint8_t* body, uint32_t bodylen, int ret);
int get_backup_svr_list(usr_info_t* p);
int unpkg_auth(const uint8_t body[], int len, login_session_t* sess);
int verify_session(const userid_t uid, const login_session_t* sess);

int mydbproxy_timeout(void* owner, void* data);
int mydeluser_timeout(void* owner, void* data);
int timestamp_timeout(void* owner, void* data);
int switch_timeout(void* owner, void* data);

#endif //MYSVRLIST_H

