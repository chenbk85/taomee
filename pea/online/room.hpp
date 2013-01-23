#ifndef ROOM_EX_HPP_
#define ROOM_EX_HPP_


extern "C" {
#include <libtaomee/project/types.h>
}

#include <list>
#include <vector>

#include <libtaomee++/utils/strings.hpp>
#include <libtaomee++/random/random.hpp>

using namespace std;
using namespace taomee;

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "room.hpp"
#include "proto.hpp"

class player_t;

enum {
	player_not_in_hall		= 0,
	player_in_hall			= 1,
};


/* online 的 room_t 仅仅起一个记录作用, 甚至有些值都没有赋值或者没有与btlsw同步;
 * 这个结构几乎不影响房间相关的逻辑, eg:
 * (1) player有没有进入房间, 还是到 btlsw 去验证的,
 * (2) 而在 del_player 的时候判断了一下 p->room, 只是出于回收内存的需要 */
struct room_t {
	/* TODO(zog): 标识玩家是否在大厅 (btlsw), 0: 不在大厅, 1: 在大厅 */
	uint8_t		in_hall;
	/* 房间编号: 0表示不在房间, 否则表示房间号 (in_hall==0的时候, 该值没有意义) */
	uint32_t	room_id;
	/* 房间类型 (0: pvp, 1: pve) */
	uint8_t		type;
	/* 房间对战模式 (0: free(svr不干预), 1: compete(svr帮助匹配)) */
	uint8_t		mode;
	/* 队伍编号 */
	uint8_t		team_id;
	/* 座位编号 */
	uint8_t		seat_id;
    /* 房主id */
	uint32_t	owner_id;
	/* 副本地图id */
	uint32_t	map_id;
	/* 副本难度 */
	uint8_t		map_lv;
};


bool is_player_in_hall(player_t *p);
bool is_player_in_room(player_t *p);
int pvp_room_tell_players_start_battle(player_t* p, uint32_t btl_id, uint32_t fd_idx);



#endif
