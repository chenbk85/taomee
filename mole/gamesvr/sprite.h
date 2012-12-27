#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <stdint.h>
#include <time.h>

#include <libtaomee/list.h>

#define GUEST_SEQ_NUM		2000000000
#define MAX_ITEMS_WITH_BODY		12
#define IS_GUEST_ID(x)		((x >= GUEST_SEQ_NUM) || (x <= 10000))

struct game_group;

typedef struct sprite {
	//------------ public interface begins --------------
	uint32_t	id; ///< 用户米米号
	// hold chair/track/... ID of a given player
	uint8_t		pos_id;
	uint16_t	pet_id;
	uint32_t    itemid;

	uint32_t	exp;
	uint32_t	strength;
	uint32_t	iq;
	uint32_t	charm;

	int			score; ///< game score
	time_t		last_act_time; ///< last active time

	uint32_t	oltoday;
	uint32_t 	stamp;
	int			item_cnt;
	uint32_t	items[MAX_ITEMS_WITH_BODY];

	// game group in which the player is
	struct game_group*  group;
	//------------- public interface ends ---------------

	// identify if a user is a game watcher or a game player,
	// and if the user has been offline
	uint16_t	status;
	uint32_t	waitcmd; ///< 指示当前用户正在等待事件
	int			priv; ///< to hold fd and gameid temporarily
	uint8_t		db_data[97]; ///< to hold game score pkg
	uint8_t     session[80];
	//for search
	list_head_t	timer_list;
	list_head_t	hash_list;
} sprite_t;

enum {
	not_in_game		= 0,
	game_player		= 1,
	game_watcher	= 2,
	sprite_offline	= 4,
	sprite_timeout	= 8,
	sprite_escape	= 16,
	super_lamn		= 1<<7,
//	enable_yuanbao  = 1<<8
};

#define NOT_IN_GAME(sp)               ((sp)->status == not_in_game)
#define IS_GAME_PLAYER(sp)            !!((sp)->status & game_player)
#define IS_GAME_WATCHER(sp)           !!((sp)->status & game_watcher)
//#define IS_ENABLE_YUANBAO(sp)           !!((sp)->status & enable_yuanbao)
#define HAS_SUPER_LAMN(sp)           !!((sp)->status & super_lamn)
#define IS_SPRITE_OFFLINE(sp)         !!((sp)->status & sprite_offline)
#define IS_SPRITE_TIMEOUT(sp)         !!((sp)->status & sprite_timeout)
#define IS_SPRITE_ESCAPE(sp)          !!((sp)->status & sprite_escape)
#define SET_GAME_PLAYER(sp)           (sp)->status |= game_player
#define SET_GAME_WATCHER(sp)          (sp)->status |= game_watcher
#define SET_SPRITE_OFFLINE(sp)        (sp)->status |= sprite_offline
#define SET_SPRITE_TIMEOUT(sp)        (sp)->status |= sprite_timeout
#define SET_SPRITE_ESCAPE(sp)         (sp)->status |= sprite_escape
#define SET_HAS_SUPER_LAMN(sp)         (sp)->status |= super_lamn
//#define SET_PLAYEER_ENABLE_YUANBAO(sp) (sp)->status|= enable_yuanbao
#define RESET_GAME_STATUS(sp)         (sp)->status = 0

static inline int
is_item_on_body(sprite_t* p, uint32_t itmid)
{
	int i;
	for (i = 0; i < p->item_cnt; ++i) {
		if (p->items[i] == itmid) {
			return 1;
		}
	}
	return 0;
}

void traverse_sprites (int (*action)(sprite_t *));
void init_sprites();
void fini_sprites();
sprite_t* get_sprite_by_fd(int fd);
sprite_t* get_sprite(uint32_t id);
sprite_t* get_sprite_from_gamegrp(uint32_t id, const struct game_group* grp);

int  del_sprite(sprite_t *p, int update);
int  del_sprite_by_fd(int fd, int update);
void del_sprite_conn(const struct sprite *p);

int  add_sprite(const sprite_t *v);
void free_sprite(sprite_t *p);

int sprite_fd(const sprite_t *p);

int send_to_self(sprite_t* p, void* buffer, int len, int completed);
int send_to_self_error(sprite_t *p, int cmd, int err, int completed);
int response_test_alive(int fd);

#endif
