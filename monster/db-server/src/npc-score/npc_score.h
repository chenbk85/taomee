/**
 * =====================================================================================
 *       @file  pet.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/02/2011 06:24:48 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  henry (韩林), henry@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_NPC_SCORE_H_20110802
#define H_NPC_SCORE_H_20110802

#include <stdint.h>
#include <benchapi.h>
#include "i_mysql_iface.h"
#include "../memcached/c_memcached.h"

uint32_t get_npc_score(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t set_npc_score(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t encourage_guide(i_mysql_iface * p_mysql_conn,c_memcached * p_memcached,uint32_t user_id,uint32_t msg_type,char * p_request_body,uint32_t request_len);

#endif //H_NPC_SCORE_H_20110802



