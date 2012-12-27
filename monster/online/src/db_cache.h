/**
 * =====================================================================================
 *       @file  ucount.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/30/2011 01:38:05 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef H_DB_CACHE_H_20120605
#define H_DB_CACHE_H_20120605

#include "data_structure.h"
#include "user_manager.h"

void handle_cache_svr_return(svr_msg_header_t *p_ucount_pkg, int len);

int handle_get_pinboard_return(usr_info_t *p_user, svr_msg_header_t *p_ucount_pkg);

int handle_enter_show_return(usr_info_t *p_user, svr_msg_header_t *p_pkg);
int handle_join_show_return(usr_info_t *p_user, svr_msg_header_t *p_pkg);
int handle_vote_show_return(usr_info_t *p_user, svr_msg_header_t *p_pkg);
int handle_history_show_return(usr_info_t *p_user, svr_msg_header_t *p_pkg);



#endif //H_DB_CACHE_H_20120605

