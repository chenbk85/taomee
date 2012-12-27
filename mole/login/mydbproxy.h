#ifndef MYDBPROXY_H
#define MYDBPROXY_H
#define DB_LOGIN_MOLE           0x1008
#define DB_MOLE_REGISTER        0x1109
#define DB_ADD_GAME_FLAG        0x0108


#include "libtype.h"
void m_process_db_return(svr_proto_t* dbpkg, uint32_t pkglen);
void handle_timestamp_return(login_timestamp_pk_header_t* dbpkg, uint32_t pkglen);
void handle_switch_return(svr_proto_t* dbpkg, uint32_t pkglen);
#endif
