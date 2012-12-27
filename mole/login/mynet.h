#ifndef MYNET_H
#define MYNET_H
#include "libtype.h"

extern int mole_proxysvr_fd;
extern int timestamp_fd;
extern int switch_fd;
int send_to_timestamp(usr_info_t* p, const uint8_t *buf, uint32_t length);
int send_request_to_db(usr_info_t* p, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);
void init_timestamp_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd);
int multicast_player_login(uint8_t* body_buf, int body_len);
int multicast_init();
void init_switch_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd);
int send_to_switch(usr_info_t* p, const uint8_t *buf, uint32_t length);
#endif
