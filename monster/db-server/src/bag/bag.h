/*
 * =====================================================================================
 *
 *       Filename:  bag.h
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
#ifndef H_BAG_H_20110726
#define H_BAG_H_20110726

#include <stdint.h>
#include <benchapi.h>
#include "i_mysql_iface.h"
#include "../memcached/c_memcached.h"

uint32_t get_bag_stuff(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_led);

uint32_t add_bag_stuff(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t update_bag_stuff(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t factory_op(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

#endif //H_BAG_20110726
