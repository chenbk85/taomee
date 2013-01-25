#include "bt_skill.h"
#include "bt_online.h"

#ifndef MAX_FUNC_NUM
#define MAX_FUNC_NUM	MAX_SKILL_NUM
#endif

// move from bt_online.c
fdsession_t* online[MAX_ONLINE_NUM] = {0};
// holds all the @maps where there are sprites in it
GHashTable* battles = NULL;
int g_log_send_buf_hex_flag = 1;

/*****************************/
