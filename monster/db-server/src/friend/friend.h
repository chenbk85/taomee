/*
 * =====================================================================================
 *
 *       Filename:  friend.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年08月05日 13时22分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luis, luis@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_FRIEND_H_20110805
#define H_FRIEND_H_20110805

#include <stdint.h>
#include <benchapi.h>
#include "i_mysql_iface.h"
#include "../memcached/c_memcached.h"

/**
 * @brief 拉取所有好友id列表,对应53421(0xD0AD)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t get_friend_id(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

/**
 * @brief 新增一条好友请求,对应53422(0xD0AE)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t apply_for_friend(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

/**
 * @brief 拉取好友申请信息,对应53423(0xD0AF)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t get_friend_apply(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

/**
 * @brief 设置好友状态信息,对应53424(0xD0B0)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t set_friend_status(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

/**
 * @brief 拉取单个好友基本信息,对应53425(0xD0B1)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t get_friend_info(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

/**
 * @brief 删除一个好友,对应53429(0xD0B5)协议
 *
 * @param p_mysql_conn
 * @param user_id
 * @param msg_type
 * @param p_request_body
 * @param request_len
 *
 * @return
 */
uint32_t del_friend(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);




#endif //H_FRIEND_20110805
