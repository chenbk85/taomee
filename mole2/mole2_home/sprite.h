#ifndef MOLE2_SPRITE_H
#define MOLE2_SPRITE_H


#include "global.h"
#include "onlinehome.h"
#include "proto.h"

#define ISVIP(flag_)	!!((flag_) & 1)

#define BEGIN_USER_ID	50000

#define IS_GUEST_ID(x)		(x >= GUEST_SEQ_NUM)
#define IS_NPC_ID(x)		(x < BEGIN_USER_ID && x > 20000)

#define SPRITE_OL_STRUCT_LEN	(sizeof(sprite_ol_t))

#define WITHIN_RANGE(val_, from_, to_) \
		(((val_) >= (from_)) && ((val_) <= (to_)))

#define CHECK_VALID_ID(id)	if (!id||IS_NPC_ID(id)||IS_GUEST_ID(id))ERROR_RETURN (("error id=%u", id), -1)

void init_sprites();

void fini_sprites();

static inline sprite_ol_t* get_sprite_from_home(home_t* p_home, userid_t userid)
{
	DEBUG_LOG("get sprite pointer [homeid=(%u,%u), uid=%u]", (uint32_t)(p_home->homeid >> 32), (uint32_t)p_home->homeid, userid);
	return ( sprite_ol_t* ) g_hash_table_lookup(p_home->sprites, &userid);
}

static inline sprite_ol_t* get_sprite_ol(mapid_t homeid, userid_t userid)
{
	home_t* p_home = ( home_t * ) g_hash_table_lookup(all_home, &homeid);
	return (sprite_ol_t* ) g_hash_table_lookup(p_home->sprites, &userid);
}


static inline int send_to_self_ol(sprite_ol_t* p, void* buf, int len)
{
#ifdef __cplusplus
		home_proto_t* p_proto = reinterpret_cast<home_proto_t*>(buf);
#else
		home_proto_t* p_proto = (home_proto_t*)buf;
#endif
    if (send_pkg_to_client(all_fds[p->onlineid], buf, len) == -1) {
		ERROR_RETURN(("failed to send pkg to client[uid=%u cmd=%u]", p->id, p_proto->opid), -1);
	}

	if (p->opid != p_proto->opid) {
		WARN_LOG("P OPID NOT EQUAL TO HOME_PROTO OPID [uid=%u %u %u]", p->id, p->opid, p_proto->opid);
	}
	p->opid = 0;

	return 0;	
}

static inline void log_sprite_ol(sprite_ol_t* p)
{
	DEBUG_LOG("====================sprite_ol info begin====");

	DEBUG_LOG("onlineid cmd userid homeid opid[%u %u %u %u %u]", p->onlineid, p->cmd, p->id, (uint32_t)p->homeid, p->opid);

	DEBUG_LOG("sprite [%u \"%s\" %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u]",
		p->sprite_info.userid,
		p->sprite_info.nick,
		p->sprite_info.flag,
		p->sprite_info.color,
		p->sprite_info.race,
		p->sprite_info.profession,
		p->sprite_info.honor,
		p->sprite_info.level,
		p->sprite_info.hp,
		p->sprite_info.mp,
		p->sprite_info.hp_max,
		p->sprite_info.mp_max,
		p->sprite_info.injury_lv,
		(uint32_t)p->sprite_info.homeid,
		p->sprite_info.pos_x,
		p->sprite_info.pos_y,
		p->sprite_info.base_action,
		p->sprite_info.advance_action,
		p->sprite_info.direction,
		p->sprite_info.teamid,
		p->sprite_info.team_idx,
		p->sprite_info.team_switch,
		p->sprite_info.pk_switch,
		p->sprite_info.base_action,
		p->sprite_info.advance_action,
		p->sprite_info.direction,
		p->sprite_info.teamid,
		p->sprite_info.team_idx,
		p->sprite_info.team_switch,
		p->sprite_info.pk_switch,
		p->sprite_info.busy_state);

	DEBUG_LOG("attire info	: [cnt=%u]", p->sprite_info.attire_cnt);
	DEBUG_LOG("pet followed	: [followed=%u]", p->sprite_info.pet_followed);
	DEBUG_LOG("====================sprite_ol info end====");
}

static inline void update_sprite_head_info(home_proto_t* p_proto, sprite_ol_t* p)
{
	p->cmd		= p_proto->cmd;
	p->id		= p_proto->id;
	p->onlineid	= p_proto->onlineid;
	p->homeid	= p_proto->homeid;
	p->opid		= p_proto->opid;
}

enum {
	hero_cup_game_map =  41201,
};

typedef struct {
	userid_t	userid;
	uint16_t 	status;
	uint8_t		atkpos;
	uint8_t		teamid;
	uint16_t	online;
	uint16_t 	scores;
}	attacker_t;

typedef struct {
	void*			game;
	list_head_t 	glink;
	int				total;
	uint8_t			icount[4];
	attacker_t		player[40];
}	wait_group_t;

typedef struct {
	userid_t			uid;
	int					card;
	uint16_t			level;
	uint8_t				state;
	uint8_t				winner;
	time_t				freezen;
	time_t				lastatk;
	userid_t			atk_uid;
	int					atk_card;
}	position_t;

typedef struct {
	time_t			end_time;
	mapid_t			mapid;
	wait_group_t*	group;
	position_t		weapon[12];
	int				scores[4];
	int				active;
	timer_struct_t*	timerp;
	list_head_t 	timer_list;
}	hero_cup_game_t;

int init_hero_cup();
int free_hero_cup();

int join_hero_cup_op(home_proto_t *pkg);
int cancel_hero_cup_op(home_proto_t *pkg);
int set_position_hero_cup_op(home_t* p_home, sprite_ol_t* p, home_proto_t *pkg);
int play_card_hero_cup_op(home_t* p_home, sprite_ol_t* p, home_proto_t *pkg);
int attack_boss_hero_cup_op(home_t* p_home, sprite_ol_t* p, home_proto_t *pkg);

int get_positions_hero_cup_op(home_t* p_home, sprite_ol_t* p, home_proto_t *pkg);
int leave_position_hero_cup_op(home_t* p_home, sprite_ol_t* p, home_proto_t *pkg);

int del_attacker(wait_group_t *g,userid_t uid);
attacker_t* get_attacker(hero_cup_game_t *game,userid_t uid);
int play_card_result(hero_cup_game_t* game, int position);
gboolean free_game(gpointer key, gpointer obj, gpointer data);

#endif
