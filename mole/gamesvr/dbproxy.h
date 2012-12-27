#ifndef ANDY_DBPROXY_H_
#define ANDY_DBPROXY_H_

#include "sprite.h"

enum
{
	db_obtain_item					= 0x1116,
	db_obtain_items					= 0x1119,
	db_change_game_attr				= 0x1139,
	db_card_getinfo					= 0x1075,
	db_card_add_win_lost			= 0x1177,
	db_get_yuanbao					= 0xD141,
	db_obtain_pet_item				= 0x1165,
	db_card_up_redclothes			= 0x117E,
	db_new_card_get_card_info		= 0x30A3,
	db_new_card_add_card			= 0x31A5,
	db_new_card_add_exp				= 0x31A6,
	db_new_card_add_monster			= 0x31B6,
	db_new_card_dec_item			= 0x1136,
	db_angel_fight_get_user_info	= 0x3245,
	db_angel_fight_game_result		= 0x334A,
	db_angel_fight_updata_exp		= 0x231D,
	db_angel_fight_check_level		= 0x334E,
	db_proto_modify_items			= 0x114D,
	db_proto_get_item_cnt			= 0x1018,
	db_proto_get_item_array			= 0x1040,
	db_proto_modify_team_power		= 0xC168,
	db_proto_get_sport_mvp_team     = 0x3643,
	db_proto_set_sport_team_badge   = 0x3642,
	db_proto_set_sysarg_sport_badge = 0xC077,
	db_proto_get_fire_cup_team      = 0xB421,
};

typedef struct server_proto
{
	uint32_t len; ///< 本次包的长度
	uint32_t seq; ///< 本次包使用的协议号，暂时没有使用
	uint16_t cmd; ///< 本次包的命令号
	uint32_t ret; ///< 返回值，如果本次包是一个返回包
	uint32_t id; ///< 用户米米号，这是摩尔的用户帐号
	char body[0];
} __attribute__((packed)) server_proto_t;

extern int proxy_fd;


int send_request_to_db(int cmd, sprite_t *p, int body_len, void *body_buf, uint32_t id);

#endif // ANDY_DBPROXY_H_
