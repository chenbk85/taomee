#ifndef MYPROTO_H
#define MYPROTO_H

extern "C" {
#include <libtaomee/tm_dirty/tm_dirty.h>
}
#include "libtype.h"



/*#define CHECK_DIRTYWORD(p_, msg_) \*/
/*do { \*/
/*int r_ = tm_dirty_check(7, (char*)(msg_)); \*/
/*return r_;\*/
/*} while (0)*/



int create_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
int add_mole_flag_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
int mole_register_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
#endif
