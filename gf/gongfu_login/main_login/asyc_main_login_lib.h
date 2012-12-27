#ifndef ASYC_MAIN_LOGIN_LIB_H
#define ASYC_MAIN_LOGIN_LIB_H
#include "asyc_main_login_type.h"
/*
struct usr_info_t {
    uint32_t uid;
    uint32_t waitcmd;
    uint32_t channel;
    uint32_t counter;
    fdsession_t *session;
    timer_head_t tmr;
    uint8_t  tmpinfo[2048];
} __attribute__((packed));
*/

//extern dirser_ini_option_t* get_global_opt();

extern int check_dirty_word(usr_info_t* p, char* msg);
extern int send_warning(usr_info_t* p, int type, int cmdid);

extern void statistic_msglog(uint32_t offset, void *data, size_t len);

extern usr_info_t* get_usr_by_fd(int fd);
extern int clear_usr_info(usr_info_t* p);

extern int init_proto_head(void* buf, uint32_t len, uint32_t cmd, userid_t uid, uint32_t result);
extern int send_to_self(usr_info_t* p, const uint8_t *buf, uint32_t length);
extern int send_to_self_error(usr_info_t* p, uint32_t error);
#endif
