/**
 * =====================================================================================
 *       @file  task.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  12/04/2012 06:24:48 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  henry
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_TASK_H_20120412
#define H_TASK_H_20110412

#include <stdint.h>
#include <benchapi.h>
#include "i_mysql_iface.h"
#include "../memcached/c_memcached.h"

uint32_t get_finished_task_list(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t finished_a_task(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

#endif //H_GAME_CHANGED_H_20110410



