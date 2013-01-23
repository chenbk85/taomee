/**
 * =====================================================================================
 *       @file  db_operator.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/13/2009 01:08:34 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <stdint.h>
#include "collector_server.h"

#ifndef _DB_OPERATOR_H_
#define _DB_OPERATOR_H_

int32_t mysql_db_init(); 
int32_t mysql_db_finish();

bool db_process_login_info(const s_login_info &info);
bool db_check_login_info_exist(const s_login_info &info);
bool db_update_login_info(const s_login_info &info);
bool db_insert_login_info(const s_login_info &info);

bool make_table_name(const uint32_t mimi_number, char table_name[], const uint16_t name_len);
bool format_ip(const uint32_t origin_ip, char formatted_ip[]);
bool make_string_time(const time_t login_time, char string_time[]);
#endif
