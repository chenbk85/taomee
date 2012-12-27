#ifndef ANDY_GAME_H
#define ANDY_GAME_H

#include <assert.h>
#include <stdint.h>

#include <libtaomee/list.h>
#include "sprite.h"
#include "util.h"

#define GAMES_NUM_MAX			100
#define MAX_PLAYERS_PER_GAME	10
#define MAX_WATCHERS_PER_GAME	  20
#define STATISTIC_TYPE 0x02040000

extern char *statistic_file;

/**
 * @brief 对于游戏的原因进行分类，可能根据分类作不同的处理
 */
typedef enum
{
	GER_end_of_game = 0x01, ///< 正常结束游戏
	GER_player_offline, ///< 游戏参与者离线
	GER_player_request, ///< 游戏参与者请求结束
	GER_player_req_draw, ///< 请求平局
	GER_draw_game = GER_player_req_draw, ///< 请求平局
	GER_player_surrender, ///< 游戏参与者投降
	GER_timer_expire, ///< 定时器超时
	GER_multi_login, ///< 重复登录

	GER_invalid_data = 0xFE, ///< 无效数据
	GER_game_system_err = 0xFF ///< 系统错误
} game_end_reason_t;

typedef enum
{
	REQ_rejected = 0, ///<

	REQ_draw,
	REQ_surrender,

	REQ_max = REQ_surrender,

	REQ_invalid = 0xFF,
	REQ_unconfirmed = 0xFF
} ReqType;

typedef struct
{
	uint32_t uid;
	ReqType req;
} UidReq;

struct game_group;

typedef struct game
{
	uint32_t id;
	uint8_t players;
	char name[32];
	char file[128];
	uint8_t gametype;

	int strong; ///< 力量值
	int IQ; ///< 智商
	int lovely; ///< 亲密度
	int exp; ///< 经验值
	int score; ///< 分数 ???
	int yxb; ///< ???
	int timeout;

	void *handle;

	void *(*create_game) (struct game_group *); ///< 用于创建游戏服务对象C链接函数
	int (*game_init) (); ///< 在加载动态链接库的时候初始化，一般是没有用的
	void (*game_destroy) (); ///<在加载动态链接库的时候清理，一般没有用
} game_t;

enum
{
	multiplayer_game = 1,
	map_visible_game = 2,
	compete_mode_game = 32,
	challenge_game = 64,
	advance_game = 128,
};

#define MULTIPLAYER_GAME(game)      ((game)->gametype & multiplayer_game)
#define ADVANCE_GAME(game)    	  	((game)->gametype & advance_game)
#define MAP_VISIBLE_GAME(game)      ((game)->gametype & map_visible_game)
#define COMPETE_MODE_GAME(game)     ((game)->gametype & compete_mode_game)
#define CHALLENGE_GAME(game)	    ((game)->gametype & challenge_game)
#define GAME_WATCHABLE(game)        MAP_VISIBLE_GAME(game)

typedef uint64_t gamegrpid_t;

/**
 * @class game_group_t
 * @class game_group
 * @brief 游戏组对象，存储一个游戏组相关信息
 * @details 游戏组是游戏进行的基本单位，每个游戏中的用户都是某个游戏组的成员
 *	 更多内容参考：
 *		 @ref doc_how_game_server_work
 *		 @ref doc_how_game_server_work-main_program
 */
typedef struct game_group
{
	gamegrpid_t id; ///< 游戏组编号，对于一个游戏组存在一个全服务器唯一的编号
	uint8_t flag;
	uint8_t count;	///< 游戏参与者的个数
	sprite_t *players[MAX_PLAYERS_PER_GAME];
	uint8_t nwatchers; ///< number of watchers in the game
	sprite_t *watchers[MAX_WATCHERS_PER_GAME];
	game_t *game; ///< 这个游戏组是哪个游戏，因为可能是下棋，也可能是滑雪等等
	void *game_handler;
	uint32_t start;
	uint32_t end;

	uint32_t card_count; ///< counter for card_game request db return
	// for search
	list_head_t timer_list;
	list_head_t hash_list;
} game_group_t;

enum
{
	game_ready = 1,
	game_started = 2,
};

#define GAME_READY(grp)             ((grp)->flag & game_ready)
#define GAME_STARTED(grp)           ((grp)->flag & game_started)
#define SET_GAME_READY(grp)         (grp)->flag |= game_ready
#define SET_GAME_START(grp)         (grp)->flag |= game_started
#define SET_GAME_END(grp)           (grp)->flag &= ~(game_ready | game_started)

typedef struct game_score
{
	uint16_t rank; ///< 名次
	uint32_t pet_id; ///< 宠物编号
	uint32_t itmid;
	uint8_t itmkind;
	uint32_t itm_max;
	int strong, ///< 增加的力量值
	  iq,
	  lovely,
	  exp,
	  score,
	  time,
	  coins; ///< 米币
	uint8_t sess[24]; ///< 密钥
} __attribute__ ((packed)) game_score_t;

typedef struct game_items
{
	uint32_t flag;
	uint32_t newcount;
	uint32_t itm_id[10];
} __attribute__ ((packed)) game_items_t;


int db_get_item (sprite_t * p, const game_score_t * gs);
int db_get_pet_item (sprite_t * p, const game_score_t * gs);
int db_get_items (sprite_t * p, const game_items_t * gs);
int db_get_items_max(sprite_t* p, const game_items_t* gt, uint32_t item_max);
int db_get_items_without_return(sprite_t* p, const game_items_t* gt);
int submit_game_score (sprite_t * p, game_score_t * score);
int get_item_callback (sprite_t * p, uint32_t id, const void *buf, int len, uint32_t ret);
int get_pet_item_callback (sprite_t * p, uint32_t id, const void *buf, int len, uint32_t ret);
int get_items_callback (sprite_t * p, uint32_t id, const void *buf, int len, uint32_t ret);
int upd_game_attr_callback (sprite_t * p, uint32_t id, const void *buf, int len, uint32_t ret);

void send_to_group_except_self (const game_group_t * ggp, uint32_t id, void *buffer, int len);
void pack_score_session (const sprite_t * p, game_score_t * gs, uint32_t gid, uint32_t score);

void add_grp_timers (const game_group_t * grp, const time_t expiretime);
void mod_grp_timers (const game_group_t * grp, const time_t expiretime);
void remove_grp_timers (const game_group_t * grp);

int on_game_begin (sprite_t * p);
int on_game_data (sprite_t * p, int cmd, const uint8_t body[], int len);
int on_db_return (sprite_t * p, uint32_t id, const void *buf, int len, uint32_t ret_code);
int on_timer_expire (void *p, void *data);
int on_game_timer_expire (void *p, void *data);
void on_game_end (game_group_t * gamegrp);

#define send_to_player(p_, buf_, len_) send_to_self((p_), (buf_), (len_), 0)
#define send_to_player_err(p_, cmd_, err_) send_to_self_error((p_), (cmd_), (err_), 1)

/**
 * @brief 把一个缓存区发送给一个游戏组所有的参与者
 * @param ggp 游戏组
 * @param buffer 需要发送的数据
 * @param len 数据的长度
 * @return 无
 */
static inline void
send_to_players (const game_group_t * ggp, void *buffer, int len)
{
	uint8_t i = 0;
	for (; i != ggp->count; ++i)
	{
		send_to_self (ggp->players[i], buffer, len, 0);
	}
}

/**
 * @brief 把一个缓存区发送给一个游戏组所有的观察者
 * @param ggp 游戏组
 * @param buffer 需要发送的数据
 * @param len 数据的长度
 * @return 无
 */
static inline void
send_to_watchers (const game_group_t * ggp, void *buffer, int len)
{
	uint8_t i = 0;
	for (; i != ggp->nwatchers; ++i)
	{
		assert (ggp->watchers[i]);
		send_to_self (ggp->watchers[i], buffer, len, 0);
	}
}

/**
 * @brief 把一个缓存区发送给一个游戏组所有的用户，包括参与者和观察者
 * @param ggp 游戏组
 * @param buffer 需要发送的数据
 * @param len 数据的长度
 * @return 无
 */
static inline void
send_to_group (const game_group_t * ggp, void *buffer, int len)
{
	send_to_players (ggp, buffer, len);
	send_to_watchers (ggp, buffer, len);
}

static inline int
ready_check (const sprite_t * p, int rcvlen, int pkglen)
{
	CHECK_BODY_LEN (rcvlen, pkglen);
	if (GAME_STARTED (p->group) || IS_GAME_WATCHER (p))
	{
		ERROR_RETURN (("GameStarted=%d, Watcher=%d: uid=%u, grpid=%lu",
					   GAME_STARTED (p->group), IS_GAME_WATCHER (p), p->id, p->group->id), -1);
	}
	return 0;
}

static inline int
started_check (const sprite_t * p, int rcvlen, int pkglen)
{
	CHECK_BODY_LEN (rcvlen, pkglen);
	if (!GAME_STARTED (p->group) || IS_GAME_WATCHER (p))
	{
		ERROR_RETURN (("Game Started=%d, Watcher=%d: uid=%u, grpid=%lu",
					   GAME_STARTED (p->group), IS_GAME_WATCHER (p), p->id, p->group->id), -1);
	}
	return 0;
}

static inline int
started_check_ge (const sprite_t * p, int rcvlen, int pkglen)
{
	CHECK_BODY_LEN_GE (rcvlen, pkglen);
	if (!GAME_STARTED (p->group) || IS_GAME_WATCHER (p))
	{
		ERROR_RETURN (("Game Started=%d, Watcher=%d: uid=%u, grpid=%lu",
					   GAME_STARTED (p->group), IS_GAME_WATCHER (p), p->id, p->group->id), -1);
	}
	return 0;
}

int get_yuanbao (sprite_t * p);
int db_add_yuanbao (sprite_t * p, int medal_type);
int add_yuanbao_callback (sprite_t * p, uint32_t id, const void *buf, int len, uint32_t ret);

//#define SVR_PROTO_ADD_FIRE_MEDAL        0xB522
//#define SVR_PROTO_SPORT_ADD_TEAM_MEDAL      0xC117
//int db_add_fire_medal(sprite_t* p, int medal_type);
//int add_fire_medal_callback(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret);
//int get_fire_medal(sprite_t* p, int rank);
//
//
//---------------------------------------------------------------------

/*#define SVR_PROTO_ADD_MEDAL	0xD101

//
int get_medal(sprite_t* p);
int db_add_medal(sprite_t* p, int medal_type);

//--------------- CallBacks ------------------------------------
int add_medal_callback(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret);*/

#endif // ANDY_GAME_H
