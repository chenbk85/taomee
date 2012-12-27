/*
 * =====================================================================================
 *
 *       Filename:  pinboard.h
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
#ifndef H_PINBOARD_H_20110808
#define H_PINBOARD_H_20110808

#include <stdint.h>
#include "i_mysql_iface.h"
#include "../memcached/c_memcached.h"


/**
 * @brief 拉取留言板信息,对应53426(0xD0B2)协议
 */
uint32_t get_pinboard_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

/**
 * @brief 增加一条留言信息,对应53427(0xD0B3)协议
 */
uint32_t add_message(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

/**
 * @brief 更新留言的状态,对应53428(0xD0B4)协议
 */
uint32_t update_message_status(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

/**
 * @brief 获得用户实际可见的留言条数,对应32803(0x8023)协议
 */
uint32_t get_real_message_count(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);




#endif //H_PINBOARD_H_20110726
