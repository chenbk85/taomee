/*
 * =====================================================================================
 *
 *       Filename:  denote.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年04月17日 15时10分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_MUSEUM_H_20120417
#define H_MUSEUM_H_20120417

#include <stdint.h>
#include <benchapi.h>
#include "i_mysql_iface.h"
#include "../memcached/c_memcached.h"

uint32_t enter_museum(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t commit_museum_game(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t get_museum_reward(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t prize_lottery(i_mysql_iface * p_mysql_conn,c_memcached * p_memcached,uint32_t user_id,uint32_t msg_type,char * p_request_body,uint32_t request_len);

#endif //H_MUSEUM_H_20120417
