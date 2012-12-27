#ifndef DBPROXY_H
#define DBPROXY_H
#define DB_GET_UID_BY_EMAIL     0x4004
#define DB_CHECK_UID_PWD        0x000C
#define DB_CHECK_UID_PWD_EX     0x005F


#include "asyc_main_login_type.h"
void handle_db_return(svr_proto_t* dbpkg, uint32_t pkglen);
void statistic_msglog(uint32_t offset, void *data, size_t len);
void clear_usr_info(usr_info_t* p);
#endif
