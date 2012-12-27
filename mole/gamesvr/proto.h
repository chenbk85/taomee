#ifndef _PROTO_H_
#define _PROTO_H_

// POSIX
#include <netinet/in.h>
// ISO
#include <stdint.h>
// user-defined
#include "game.h"
#include "sprite.h"

enum {
	ERR_INCORRECT_PASSWD	= 10001,
	ERR_TOO_MANY_USER,

	ERR_OCCUPIED_POS		= 10007,
	ERR_SYSTEM_BUSY			= 10009,

	ERR_daily_money_limit_exceeded	= 10021,

	ERR_MAX_CMD
};

typedef struct protocol {
	uint32_t	len;
	uint8_t		ver;
	uint32_t	cmd;
	uint32_t	id;
	uint32_t	ret;
	uint8_t		body[0];
} __attribute__((packed)) protocol_t;

enum {
	proto_version				= 200,
	// for game server
	proto_test_alive			= 30000,
	proto_player_enter			= 30001,
	proto_notify_player_entered	= 30002,
	proto_player_leave			= 30003,
	proto_game_ready			= 30004,
	proto_game_end				= 30005,
	proto_game_score			= 30006,
	proto_player_req			= 30007, // player request (draw, surrender)
	proto_notify_player_req		= 30008, // send player req to the other players
	proto_watcher_number		= 30009,
	proto_player_action			= 30010,
	proto_game_start			= 30011,
	proto_notify_game_winner	= 30012,
	proto_chessboard_info		= 30013,
	proto_game_map_info			= 30013,
	proto_client_ready			= 30014,
	proto_card_getinfo			= 30015,
	proto_card_add_win_lost		= 30016,
	proto_get_items				= 30017,
	proto_set_redclothes		= 30018,
	proto_new_card_get_card_info = 30019,
	proto_new_card_add_exp		= 30020,
	proto_new_card_add_monster	= 30021,
	proto_new_card_dec_item		= 30022,

	//六边棋游戏消息头
	proto_linker_game_cmd		= 30600,
};

struct shm_block;

static inline void
init_proto_head(void* buf, int cmd, int len)
{
#ifdef __cplusplus
	protocol_t* p = reinterpret_cast<protocol_t*>(buf);
#else
	protocol_t* p = buf;
#endif // __cplusplus

	p->len = htonl(len);
	p->cmd = htonl(cmd);
	p->ver = 0x01;
	p->ret = 0;
}

int  parse_protocol(const uint8_t *data, int rcvlen, int fd);

int  leave_game_cmd(sprite_t *p, const uint8_t *body, int bodylen);
void game_end_response(game_group_t *ggp, sprite_t *p, uint8_t reason);

static inline void
handle_invalid_data(sprite_t* p, uint8_t reason)
{
	if (p->group) {
		DEBUG_LOG("HANDLE_INVALID_DATA leave game %d", p->id);
		leave_game_cmd(p, &reason, 1);
	} else {
		DEBUG_LOG("HANDLE_INVALID_DATA delete player %d", p->id);
		del_sprite_conn(p);
	}
}

#endif
