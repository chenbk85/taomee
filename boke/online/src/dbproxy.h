/**
 *============================================================
 *  @file      dbproxy.hpp
 *  @brief    dbproxy related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef POP_DBPROXY_H
#define POP_DBPROXY_H

#include "svr_proto.h"
void init_db_handle_funs();
void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen);

#endif // PAIPAI_DBPROXY_HPP_

