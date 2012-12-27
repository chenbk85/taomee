/*
 * =====================================================================================
 *
 *       Filename:  activity.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年05月30日 13时33分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_ACTIVITY_H_20120530
#define H_ACTIVITY_H_20120530

#include <stdint.h>
#include <benchapi.h>
#include "i_mysql_iface.h"
#include "../memcached/c_memcached.h"

uint32_t get_activity_reward(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);


uint32_t get_open_act_reward(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t update_activity_status(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t activity_get_activity_reward_ex(i_mysql_iface * p_mysql_conn,c_memcached * p_memcached,uint32_t user_id,uint32_t msg_type,char * p_request_body,uint32_t request_len);

uint32_t activity_get_invite_info(i_mysql_iface * p_mysql_conn,c_memcached * p_memcached,uint32_t user_id,uint32_t msg_type,char * p_request_body,uint32_t request_len);

uint32_t activity_update_invite_friend(i_mysql_iface * p_mysql_conn,c_memcached * p_memcached,uint32_t user_id,uint32_t msg_type,char * p_request_body,uint32_t request_len);

uint32_t activity_get_dragon_boat_info(i_mysql_iface * p_mysql_conn,c_memcached * p_memcached,uint32_t user_id,uint32_t msg_type,char * p_request_body,uint32_t request_len);

uint32_t activity_update_dragon_boat_info(i_mysql_iface * p_mysql_conn,c_memcached * p_memcached,uint32_t user_id,uint32_t msg_type,char * p_request_body,uint32_t request_len);
#endif //H_ACTIVITY_H_20120530
