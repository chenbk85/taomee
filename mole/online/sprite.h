#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "benchapi.h"
#include <libtaomee/log.h>
#include "service.h"

#define ISVIP(flag_)	!!((flag_) & 1)
#define ISVIP_BEFORE(flag_)	(((flag_) & 33) == 32)

#define SPRITE_ACTIVED_FLAG			1
#define SPRITE_HOME_PUBLIC_FLAG		2
#define SPRITE_SMC_FLAG				4
#define SPRITE_MESSENGER_FLAG		8

#define GUEST_SEQ_NUM		2000000000
#define IS_GUEST_ID(x)		(x >= GUEST_SEQ_NUM)
#define IS_NPC_ID(x)		(x < 10000 && x > 0)

#define WITHIN_RANGE(val_, from_, to_) \
		(((val_) >= (from_)) && ((val_) <= (to_)))

#define IS_ADMIN_ID(uid_) \
		( WITHIN_RANGE((uid_), 199990, 200009) || WITHIN_RANGE((uid_), 299990, 300009) \
			|| WITHIN_RANGE((uid_), 399990, 399999) || WITHIN_RANGE((uid_), 20000001, 20000020) )
#define CHECK_VALID_ID(id)	if (!id||IS_NPC_ID(id)||IS_GUEST_ID(id))ERROR_RETURN (("error id=%u", id), -1)

//#define SPRITE_NUM_MAX		500
#define SPRITE_STRUCT_LEN	(4096 * 2)
#define SPRITE_FRIENDS_MAX	200

#define HASH_SLOT_NUM	200

//for cmd queue
#define CMD_QUEUE_MAX_LENGTH 30
#ifndef TW_VER
#define TWO_HOUR_SEC	7200
#define FIVE_HOUR_SEC	18000
#else
#define TWO_HOUR_SEC	100000
#define FIVE_HOUR_SEC	200000
#endif
enum {
	ACTION_IDLE = 0,
	ACTION_DANCE,
	ACTION_WAVE,
	ACTION_SIT,
	ACTION_PAOPAO,
	ACTION_levelup,
	ACTION_gotmedal,
	ACTION_MAX
};

enum {
	SPRITE_STATUS_LOGIN = 0,
	SPRITE_STATUS_IN_GAME,
	SPRITE_STATUS_WAIT_GAME,
	SPRITE_STATUS_NOT_ONLINE
};

extern list_head_t	idslots[HASH_SLOT_NUM];

void traverse_sprites(void (*action)(void* key, void* spri, void* userdata), void* data);
void traverse_sprites_remove(gboolean (*action)(gpointer key, gpointer spri, gpointer userdata), gpointer data);

void init_sprites();
void fini_sprites();

sprite_t* get_sprite_by_fd(int fd);
//sprite_t* get_sprite(uint32_t id);
sprite_t* get_sprite_from_gamegrp(uint32_t id, const struct game_group* grp);

// Change @p's current dress to @dress. Used only by NPC currently
void chg_dress(sprite_t* p, uint32_t dress[], int cnt);

//
void notify_5basic_attr(sprite_t* p);

int sub_sprite_attr(sprite_t* p, int type, int cnt);

int del_sprite(sprite_t* p, int update, int del_node);
int del_sprite_by_fd(int fd, int update, int del_node);

sprite_t* add_sprite(sprite_t* v);

static inline int
sprite_fd(const sprite_t* p)
{
	return p->fd;
}

static inline void
del_sprite_conn(const sprite_t* p, int del_node)
{
	int fd = sprite_fd(p);
	//if (fd >= 0)
		shm_ctl_block_push(&(config_cache.bc_elem->sendq), fd, FIN_BLOCK, del_node);
}

static inline sprite_t *get_sprite (uint32_t id)
{
	sprite_t *p;

	list_for_each_entry (p, &idslots[id % HASH_SLOT_NUM], hash_list) {
		if (p->id == id)
			return p;
	}

	return NULL;
}

typedef struct VarPkg {
	void*   pkg;
	int     pkg_len;
	time_t  idle_time;
} var_pkg_t;

int  kick_all_users_offline(void* owner, void* data);

void send_varpkg(void* key, void* spri, void* userdata);
void send_varpkg_idle(void* key, void* spri, void* userdata);

static inline void
send_to_all(void* buf, int len)
{
	var_pkg_t vpkg = { buf, len, -1 };
	traverse_sprites(send_varpkg, &vpkg);
}

static inline void
send_to_all_idle(void* buf, int len, time_t idle_time)
{
	var_pkg_t vpkg = { buf, len, idle_time };
	traverse_sprites(send_varpkg, &vpkg);
}

extern uint32_t	sprites_count;
extern list_head_t g_busy_sprite_list;


int select_sprite(sprite_t* p, sprite_t** val);
int mole_sprite_op(uint32_t uid, void* buf, int len);

int cal_mole_level(uint32_t mole_exp);

#endif
