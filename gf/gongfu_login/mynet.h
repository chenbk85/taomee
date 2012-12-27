#ifndef MYNET_H
#define MYNET_H
#include "libtype.h"

#define DB_LOGIN_GF           	0x06E4
#define DB_GET_ROLE_LIST        0x06E3
#define DB_GF_REGISTER        	0x07E1
#define DB_ADD_GAME_FLAG        0x0108

#define DELUSER_DELETE_ROLE     0x07F0
#define DELUSER_PHY_DEL_ROLE    0x07F1

extern int gf_proxysvr_fd;
extern int gf_deluser_fd;
extern int timestamp_fd;
extern int switch_fd;

int send_request_to_db(usr_info_t* p, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);
int send_request_to_deluser(usr_info_t* p, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);
int send_to_timestamp(usr_info_t* p, const uint8_t *buf, uint32_t length);
int send_to_switch(usr_info_t* p, const uint8_t *buf, uint32_t length);

void init_timestamp_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd);
void init_switch_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd);

int multicast_player_login(uint8_t* body_buf, int body_len);
int multicast_init();

void m_process_db_return(svr_proto_t* dbpkg, uint32_t pkglen);
void handle_timestamp_return(login_timestamp_pk_header_t* dbpkg, uint32_t pkglen);
void handle_switch_return(svr_proto_t* dbpkg, uint32_t pkglen);
void handle_deluser_return(svr_proto_t* dbpkg, uint32_t pkglen);

#endif //MYNET_H
