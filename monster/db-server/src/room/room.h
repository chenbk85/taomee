/*
 * =====================================================================================
 *
 *       Filename:  room.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月21日 13时22分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luis, luis@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_ROOM_H_20110726
#define H_ROOM_H_20110726

#include <stdint.h>
#include <benchapi.h>
#include "i_mysql_iface.h"
#include "../memcached/c_memcached.h"

uint32_t get_room_num(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t get_room_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t add_room(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t update_room(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

#endif //H_ROOM_H_20110726
