/*
 * =====================================================================================
 *
 *       Filename:  service.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年04月09日 15时02分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_SERVICE_H_20120409
#define H_SERVICE_H_20120409

#include <stdint.h>
#include "i_mysql_iface.h"
#include "../memcached/c_memcached.h"



#pragma pack(push)
#pragma pack(1)

typedef struct
{
    uint8_t is_register;
    char user_name[16];
    uint16_t monster_level;
    uint32_t monster_id;
    uint32_t monster_main_color;
    uint32_t monster_exp_color;
    uint32_t monster_eye_color;
}boke_msg_get_role_t;

typedef struct
{
	char monster_name[16];
	char user_name[16];
	uint32_t birthday;
	uint8_t fav_pet;
	char personal_sign[64];
	uint8_t fav_color;
	uint8_t fav_fruit;
	uint8_t mood;
	uint32_t coins;
	uint16_t monster_level;
	uint32_t monster_exp;
	uint32_t monster_health;
	uint32_t monster_happy;
	uint32_t visits;
	uint32_t thumb;
	uint32_t last_login_time;
	uint8_t limit_op;
}service_role_t;

typedef struct
{
	uint8_t is_register;
	service_role_t role_info;
}service_msg_get_role_t;

typedef struct
{
	uint32_t badge_id;
	uint8_t status;
	uint32_t progress;
	uint32_t timestamp;
}service_badge_t;

typedef struct
{
	uint32_t peer_id;
	uint8_t status;
	uint16_t msg_len;
	char msg_content[512];
}service_pinboard_t;

typedef struct
{
	uint32_t stuff_id;
	uint32_t owned_num;
	uint32_t used_num;
}service_stuff_t;

typedef struct
{
	uint32_t level_id;
	uint32_t level_score;
	uint32_t level_star;
}service_level_t;

typedef struct
{
	uint32_t max_score;
	uint32_t total_score;
	uint32_t avg_score;
	uint32_t total_num;
}service_puzzle_t;

#pragma pack(pop)

uint32_t query_role_info_boke(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t query_role_info_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t query_friend_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t query_badge_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t query_pinboard_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t query_stuff_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t query_game_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);

uint32_t query_puzzle_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len);




#endif //H_SERVICE_H_20120409

