#include "home.h"

#include "proto.h"
#include "global.h"
#include "sprite.h"
#include "onlinehome.h"
#include "dispatcher.h"
#include "util.h"

int proto_enter_home_op(home_proto_t* p_proto)
{
	hero_cup_game_t *game = NULL;
	mapid_t gameid = (p_proto->homeid & 0xFFFFFFFF00000000) | hero_cup_game_map;
	CHECK_BODY_LEN_GE(p_proto->len, sizeof(home_proto_t) + sizeof(sprite_base_info_t));

	KDEBUG_LOG(p_proto->id,"ENTER HOME: [homeid=%x %u]", HI32(p_proto->homeid), LO32(p_proto->homeid));

	home_t* p_home	= g_hash_table_lookup(all_home, &p_proto->homeid);
	if((gameid >> 56) == 0xFE) game = g_hash_table_lookup(all_games,&gameid);
	if(!p_home) {
		if(HI32(p_proto->homeid) == 1 || game) p_home	= add_home(p_proto->homeid);
		if (!p_home) {
			KDEBUG_LOG(p_proto->id,"ENTER HOME FAILED [onlineid=%u map=%x %u]",p_proto->onlineid, HI32(p_proto->homeid), LO32(p_proto->homeid));
			return 0;
		}
	}

	sprite_ol_t* p = g_hash_table_lookup(p_home->sprites, &p_proto->id);
	if (!p) {
		p = add_sprite_ol(p_home, p_proto);
		if (!p) {
			KDEBUG_LOG(p_proto->id,"ADD PLAYER FAILED [onlineid=%u map=%x %u]", p_proto->onlineid,HI32(p_proto->homeid), LO32(p_proto->homeid));
			return 0;
		}
		if(game) {
			game->active++;
			KDEBUG_LOG(p_proto->id,"ENTER HOME: have %d players in game(%x)",game->active,HI32(gameid));
		}
	}

	memcpy(&p->sprite_info, p_proto->body, sizeof(sprite_base_info_t));
	if (p->sprite_info.pet_followed) {
		memcpy(&p->pet_sinfo, p_proto->body + sizeof(sprite_base_info_t), sizeof(pet_simple_info_t));
	}

	send_all_users_to_self(p->id, p->onlineid, p_home);
	return 0;
}

static int proto_cli_leave_home_op(home_t* p_home, sprite_ol_t* p, void* data)
{
	hero_cup_game_t* game = NULL;
	home_proto_t* p_proto = data;
	client_proto_t *cli = (client_proto_t*)p_proto->body;
	CHECK_BODY_LEN(p_proto->len, 20 + sizeof(home_proto_t) + sizeof(client_proto_t));

	int i = 12;
	mapid_t newmap = 0;
	unpkg_mapid(cli->body,&newmap,&i);
	mapid_t gameid = (p_home->homeid & 0xFFFFFFFF00000000) | hero_cup_game_map;

	tranfer_to_home(p_home, data, p_proto->len);
	free_sprite_ol(p_home, p);

	if((gameid >> 56) != 0xFE) return 0;
	game = g_hash_table_lookup(all_games,&gameid);
	if(!game) {
		KDEBUG_LOG(p_proto->id,"LEAVE HOME: game(%x.%u) NOT EXSIT!",HI32(gameid),LO32(gameid));
		return 0;
	}

	game->active--;
	if((newmap >> 56) != 0xFE) {
		if(game->active == 0) {
			free_game(NULL,game,NULL);
			return 0;
		}
	}

	attacker_t *atker = get_attacker(game,p_proto->id);
	if(!atker) {
		KDEBUG_LOG(p_proto->id,"LEAVE HOME: uid not in game(%x.%u)",HI32(gameid),LO32(gameid));
		return 0;
	}

	if(atker->atkpos < 12) {
		position_t *weapon = &game->weapon[atker->atkpos];
		if(weapon->uid == atker->userid) {
			weapon->state |= 1;
			weapon->card = 0;
		} else {
			weapon->state |= 2;
			weapon->atk_card = 0;
		}
		play_card_result(game,atker->atkpos);
	}

	if((newmap >> 56) != 0xFE) {
		del_attacker(game->group,atker->userid);
	}

	return 0;
}

static int proto_cli_notify_home_op(home_t* p_home, sprite_ol_t* p, void* data)
{
	home_proto_t* p_proto = data;
	CHECK_BODY_LEN_GE(p_proto->len, sizeof(home_proto_t) + sizeof(client_proto_t));
	tranfer_to_home(p_home, data, p_proto->len);
	return 0;
}

static int proto_cli_action_op(home_t* p_home, sprite_ol_t* p, void* data)
{
	home_proto_t* p_proto = data;
	int i = sizeof(home_proto_t)  + sizeof(client_proto_t) + 4;
	
	CHECK_BODY_LEN(p_proto->len, sizeof(home_proto_t) + sizeof(client_proto_t) + 12);
	
    UNPKG_UINT32(data, p->sprite_info.base_action, i);
	UNPKG_UINT32(data, p->sprite_info.direction, i);
	
	tranfer_to_home(p_home, data, p_proto->len);	
	return 0;
}

static int proto_cli_walk_op(home_t* p_home, sprite_ol_t* p, void* data)
{
	home_proto_t* p_proto = data;
	CHECK_BODY_LEN_GE(p_proto->len, sizeof(home_proto_t) + sizeof(client_proto_t) + 18);

	int i = sizeof(home_proto_t) + sizeof(client_proto_t) + 4;
	UNPKG_UINT32(data, p->sprite_info.pos_x, i);
	UNPKG_UINT32(data, p->sprite_info.pos_y, i);
	i += 2;
	UNPKG_UINT32(data, p->sprite_info.direction, i);

	tranfer_to_home(p_home, data, p_proto->len);
	return 0;
}

static int proto_cli_busy_state_op(home_t* p_home, sprite_ol_t* p, void* data)
{
	uint32_t type, state;
	home_proto_t* p_proto = data;
	int j = sizeof(home_proto_t) + sizeof(client_proto_t) + 4;
	CHECK_BODY_LEN(p_proto->len, sizeof(home_proto_t) + sizeof(client_proto_t) + 12);

	UNPKG_UINT32(data, type, j);
	UNPKG_UINT32(data, state, j);

	switch (type) {
		case team_switch:			
			p->sprite_info.team_switch	= state;
			break;
		case sns_state:
			p->sprite_info.busy_state	= state;
			break;
		case pk_switch:
			p->sprite_info.pk_switch 	= state;
			break;
		default:
			break;
	}

	tranfer_to_home(p_home, data, p_proto->len);
	return 0;
}

static int proto_cli_set_user_nick_op(home_t* p_home, sprite_ol_t* p, void* data)
{
	home_proto_t* p_proto = data;
	int i = sizeof(home_proto_t) + sizeof(client_proto_t) + 4;

	CHECK_BODY_LEN(p_proto->len, sizeof(home_proto_t) + sizeof(client_proto_t) + 20);

	UNPKG_STR(data, p->sprite_info.nick, i, NICK_LEN);

	tranfer_to_home(p_home, data, p_proto->len);
	return 0;
}

inline int handle_online_op(home_t* p_home, sprite_ol_t* p, home_proto_t* p_proto)
{
	switch (p_proto->opid) {
		case proto_challenge_pos:
			return set_position_hero_cup_op(p_home,p,p_proto);
		case proto_play_card:
			return play_card_hero_cup_op(p_home,p,p_proto);
		case proto_attack_boss:
			return attack_boss_hero_cup_op(p_home,p,p_proto);
		case proto_leave_positions:
			return leave_position_hero_cup_op(p_home,p,p_proto);
		case proto_get_positions_info:
			return get_positions_hero_cup_op(p_home,p,p_proto);
		case proto_cli_leave_home:
			return proto_cli_leave_home_op(p_home, p, p_proto);
		case proto_cli_walk:
			return proto_cli_walk_op(p_home, p, p_proto);
		case proto_cli_action:
			return proto_cli_action_op(p_home, p, p_proto);
		case proto_cli_busy_state:
			return proto_cli_busy_state_op(p_home, p, p_proto);
		case proto_cli_set_user_nick:
			return proto_cli_set_user_nick_op(p_home, p, p_proto);
		default:
			return proto_cli_notify_home_op(p_home, p, p_proto);
	}
	return 0;
}

static gboolean free_online(gpointer key, gpointer obj, gpointer userdata)
{
	g_slice_free1(sizeof(online_each_t), obj);
	return TRUE;
}

static gboolean free_sprite(gpointer key, gpointer obj, gpointer userdata)
{
	g_slice_free1(sizeof(sprite_ol_t), obj);
	return TRUE;
}

static gboolean free_homemaps(gpointer key, gpointer obj, gpointer userdata)
{
	home_t* h = (home_t*)obj;

	g_hash_table_foreach_remove(h->onlines, free_online, NULL);
	g_hash_table_foreach_remove(h->sprites, free_sprite, NULL);

	g_hash_table_destroy(h->sprites);
	g_hash_table_destroy(h->onlines);

	g_slice_free1(sizeof(home_t),h);

	return TRUE;
}

home_t* free_home(home_t* h)
{
	g_hash_table_remove(all_home, &h->homeid);
	free_homemaps(NULL, h, NULL);
	return NULL;
}

home_t* add_home(mapid_t homeid)
{
	home_t* p_home = NULL;

	p_home = g_slice_alloc0(sizeof(*p_home));
	p_home->homeid	= homeid;
	p_home->sprites = g_hash_table_new(g_int_hash, g_int_equal);
	p_home->onlines = g_hash_table_new(g_int_hash, g_int_equal);
	g_hash_table_insert(all_home, &(p_home->homeid), p_home);

	return p_home;
}

sprite_ol_t* add_sprite_ol(home_t* p_home, home_proto_t* p_proto)
{
	sprite_ol_t* p = g_slice_alloc0(sizeof(sprite_ol_t));	
	update_sprite_head_info(p_proto, p);
	g_hash_table_insert(p_home->sprites, &p->id, p);
	add_online_user(p_home, p_proto->onlineid);
	return p;	
}

void free_sprite_ol(home_t* h, sprite_ol_t* p)
{
	userid_t uid = p->id;
	del_online_user(h, p->onlineid);	
	g_hash_table_remove(h->sprites, &uid);
	g_slice_free1(SPRITE_OL_STRUCT_LEN, p);
	if (0 == g_hash_table_size(h->sprites)) {
		KDEBUG_LOG(uid,"FREE HOME: [homeid=%x %u]", HI32(h->homeid),LO32(h->homeid));
		free_home(h);
	}
}

void add_online_user(home_t* p_home, uint32_t onlineid)
{
	online_each_t* p_online = g_hash_table_lookup(p_home->onlines, &onlineid);
	if (!p_online) {
		p_online = g_slice_alloc0(sizeof(online_each_t));
		p_online->onlineid	= onlineid;
		p_online->user_cnt	= 0;
		g_hash_table_insert(p_home->onlines, &p_online->onlineid, p_online);
	}
	++p_online->user_cnt;
}

void del_online_user(home_t* p_home, uint32_t onlineid)
{
	online_each_t* p_online = g_hash_table_lookup(p_home->onlines, &onlineid);
	if (p_online) {
		if(--p_online->user_cnt == 0) {
			g_hash_table_remove(p_home->onlines, &onlineid);
			g_slice_free1(sizeof(online_each_t),p_online);
		}
	}
}

typedef struct{
	uint32_t	onlineid;
	home_t*		p_home;
}	clean_user_args_t;

static void notify_leave_home(home_t* h, sprite_ol_t* p)
{
	uint8_t	buf[1024];

	int i = sizeof(home_proto_t) + sizeof(client_proto_t);
	PKG_UINT32(buf, p->id, i);
	PKG_MAP_ID(buf, h->homeid, i);
	PKG_MAP_ID(buf, 0, i);

	init_two_head(buf, i, p->onlineid, p->cmd, p->id, p->homeid, proto_cli_leave_home);

	tranfer_to_home(h, buf, i);
}


static gboolean clean_user(void* key, void* value, void* data)
{
	sprite_ol_t *p = value;
	clean_user_args_t *arg = data;

	if (p->onlineid == arg->onlineid) {
		notify_leave_home(arg->p_home, p);
		del_online_user(arg->p_home, p->onlineid);	
		g_slice_free1(SPRITE_OL_STRUCT_LEN, p);

		return TRUE;
	}

	return FALSE;
}

static gboolean clean_online_users(void* key, void* value, void* data)
{
	home_t* h = value;
	clean_user_args_t arg = {*(uint32_t*)data, h};

	g_hash_table_foreach_steal(h->sprites, clean_user, &arg);
	if (0 == g_hash_table_size(h->sprites)) {
		free_homemaps(NULL, h, NULL);	
		return TRUE;
	}

	return FALSE;
}

void clean_all_users_from_online(int fd)
{
	int i;
	for (i = 0; i < MAX_ONLINE_NUM; ++i) {
		if (all_fds[i] && (all_fds[i]->fd == fd)) {
			all_fds[i] = NULL;
			g_hash_table_foreach_steal(all_home,clean_online_users, &i);
			break;
		}
	}
}

void init_home_maps()
{
	all_home = g_hash_table_new(g_int64_hash, g_int64_equal);
}

void fini_home_maps()
{
	g_hash_table_foreach_remove(all_home, free_homemaps, 0);
	g_hash_table_destroy(all_home);
}

