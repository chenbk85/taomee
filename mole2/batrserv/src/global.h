#ifndef __BATSERVER_GLOBAL_H__
#define __BATSERVER_GLOBAL_H__
#include "bt_skill.h"
#include "bt_online.h"
#include  "global_not_reload.h" 

// move from bt_online.c
extern fdsession_t* online[MAX_ONLINE_NUM];
// holds all the @maps where there are sprites in it
extern GHashTable* battles;
extern int g_log_send_buf_hex_flag ;


#endif
