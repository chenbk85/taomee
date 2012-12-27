/*
 * =====================================================================================
 *
 *       Filename:  badge.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年02月20日 16时43分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_DB_BADGE_H_20120220_H
#define H_DB_BADGE_H_20120220_H


#include <stdint.h>
#include <benchapi.h>
#include "i_mysql_iface.h"
#include "../memcached/c_memcached.h"

uint32_t get_all_badge(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t update_a_badge(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);


#endif//H_DB_BADGE_H_20120220_H
