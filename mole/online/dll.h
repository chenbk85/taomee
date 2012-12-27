#ifndef ONLINE_DLL_H_
#define ONLINE_DLL_H_

#include <math.h>
#include <stdlib.h>

#include <glib.h>

#include "benchapi.h"
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include "map.h"
#include "proto.h"
#include <libtaomee/timer.h>

typedef struct item_info{
	uint32_t itmid;
	uint32_t userid;
	uint32_t super_lamu;
} __attribute__((packed)) item_info_t;

typedef struct items_info{
	uint32_t count;
	item_info_t iit[6];
} __attribute__((packed)) items_info_t;

#define MULTI_PLAYER_GAME_FLAG		1
#define MAP_VISIBLE_GAME_FLAG		2
#define MAP_GAME_START_FLAG			4
#define INTERACTABLE_SINGLEPLAYER_GAME	8
#define SCENE_GAME_FLAG				16

#define GAME_STARTED(grp)		!!((grp)->type & MAP_GAME_START_FLAG)
#define MULTIPLAYER_GAME(grp)	!!((grp)->type & MULTI_PLAYER_GAME_FLAG)
#define MAP_VISIBLE_GAME(grp)	!!(((grp)->type & MAP_VISIBLE_GAME_FLAG) || ((grp)->type & SCENE_GAME_FLAG))
#define GAME_WATCHABLE(grp)		!!((grp)->type & MAP_VISIBLE_GAME_FLAG)
#define INTERACT_SINGLEPLAYER_GAME(grp)	!!((grp)->type & INTERACTABLE_SINGLEPLAYER_GAME)
#define SCENE_GAME(grp)			!!((grp)->type & SCENE_GAME_FLAG)
#define SET_GAME_START(grp)		(grp)->type |= MAP_GAME_START_FLAG
#define SET_GAME_END(grp)		(grp)->type &= ~MAP_GAME_START_FLAG

enum {
	gamegrp_map_itm_occupy_at_once	= 1
};

#define MAP_ITM_OCCUPY_AT_ONCE(grp_) \
		!!((grp_->flag) & gamegrp_map_itm_occupy_at_once)

enum {
	not_in_game,
	game_player,
	game_watcher
};

#define NOT_IN_GAME(sp)        ((sp)->game_status == not_in_game)
#define IS_GAME_PLAYER(sp)     ((sp)->game_status == game_player)
#define IS_GAME_WATCHER(sp)    ((sp)->game_status == game_watcher)
#define SET_GAME_PLAYER(sp)    (sp)->game_status = game_player
#define SET_GAME_WATCHER(sp)   (sp)->game_status = game_watcher
#define RESET_GAME_STATUS(sp)  (sp)->game_status = not_in_game

enum {
	LEAVE_GAME_BY_OVER = 0,
	LEAVE_GAME_BY_OFFLINE,
	LEAVE_GAME_BY_REQUEST,
	LEAVE_GAME_BY_NOT_PUMPKIN
};

extern int char_indx;
extern char* char_match[68];


int  load_games(const char* file);
int  load_dlls();
void unload_dlls();
int load_dll_conf(const char* file);

game_group_t *alloc_game_group (struct map_item_group *g);
game_t* get_game(int id);
game_group_t *get_game_group (uint32_t id);

void leave_game(sprite_t* p, uint8_t reason);
void end_game(game_group_t* ggp, sprite_t* sp, uint8_t reason);
void notify_mapgame_info(sprite_t* p, uint8_t istomap);

int enter_game_cmd (sprite_t* p, const uint8_t* body, int bodylen);
int game_score_cmd(sprite_t* p, const uint8_t* body, int len);
int get_group_cmd (sprite_t *p, const uint8_t *body, int bodylen);
int leave_game_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int singleplayer_game_msg_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_matched_string_cmd(sprite_t *p, const uint8_t *body, int bodylen);

static inline int
mapgame_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	if ( MAP_WITH_GAME(p->tiles) ) {
		notify_mapgame_info(p, 0);
		return 0;
	}
	ERROR_RETURN(("Wrong Req! Non-game Map: mapid=%lu, uid=%u", p->tiles->id, p->id), -1);
}

static inline int
scenegame_info_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	// call on_game_data if is Scene Game Map
	if (p->tiles && SCENE_GAME_MAP(p->tiles) && !p->group) {
		p->group = p->tiles->gamegrp;
		// Call on_game_begin Only For Football
		if (p->group->game->id == 15) {
			p->group->game->on_game_begin(p);
		}
		p->group->game->on_game_data(p, p->waitcmd, body, bodylen);
		p->group   = 0;
		p->waitcmd = 0;
		return 0;
	}

	ERROR_RETURN( ("Wrong Req! Non-Scene-Game Map Or Already In Game: mapid=%lu uid=%u",
					(p->tiles ? p->tiles->id : 0), p->id), -1 );
}
/*
static inline int
get_cut_sheep_cheer_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	// call on_game_data if is Scene Game Map
	if (p->tiles && SCENE_GAME_MAP(p->tiles) && !p->group) {
		p->group = p->tiles->gamegrp;
		p->group->game->on_game_data(p, p->waitcmd, body, bodylen);
		p->group   = 0;
	}
	return 0;
}
*/
static inline void
free_game_group(game_group_t* ggp)
{
	list_del(&ggp->hash_list);
	g_slice_free1(sizeof *ggp, ggp);
}

/*static inline int
is_game_cheat(const sprite_t* p, int coins)
{
	int mins = (int)ceil((now.tv_sec - p->game_start_time) / 60.0);

	if (coins > (mins + 1) * p->group->game->coins_per_min) {
		ERROR_RETURN(("%s GameCheater\t[Start=%ld End=%ld Mins=%d CoinsPerMin=%d TotalCoins=%d]",
				p->group->game->name, p->game_start_time, now.tv_sec, mins, p->group->game->coins_per_min, coins), 1);
	}
	return 0;
}*/

static inline int
credit_cond_met(const sprite_t* p)
{
	return (((get_now_tv()->tv_sec - p->game_start_time) > p->group->game->tm));
}

#endif // ONLINE_DLL_H_
