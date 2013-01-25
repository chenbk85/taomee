
#include <arpa/inet.h>
#include <string.h>

#include <glib.h>

#include "global.h"

#include "sprite.h"
#include "util.h"
#include "home.h"
#include "dispatcher.h"

#define REGIST_TIMER(nbr_, cb_) \
		do { \
			if (register_timer_callback(nbr_, cb_) == -1) \
				ERROR_RETURN(("register timer error\t[%u]", nbr_), -1); \
		} while(0)

enum {
	timer_game_loop		= 1,
};

#define MIN_CMP		5
#define TOTAL_CMP	1
#define ROUND_TIME	600

static void send_to_sprite(void* key, void* value, void* data)
{
	sprite_ol_t* p = value;
	home_proto_t* pkg = data;	
	if (all_fds[p->onlineid]) {
		pkg->id = p->id;
		pkg->homeid = p->homeid;
		pkg->onlineid = p->onlineid;
		send_pkg_to_client(all_fds[p->onlineid], pkg, pkg->len);
	} else {
		KDEBUG_LOG(pkg->id,"send map failed [onlineid=%d]", p->onlineid);
	}
}

static void send_to_home(home_t* p_home, home_proto_t *pkg)
{
	g_hash_table_foreach(p_home->sprites, send_to_sprite, pkg);
}

attacker_t* get_attacker(hero_cup_game_t *game,userid_t uid)
{
	int i;

	for(i = 0; i < game->group->total; i++) {
		if(game->group->player[i].userid == uid) {
			return &game->group->player[i];
		}
	}

	return NULL;
}

void debug_game(hero_cup_game_t *game)
{
	int i;
	position_t *w;
	attacker_t *p;

	for(i = 0; i < 12; i++) {
		w = &game->weapon[i];
		DEBUG_LOG("p[%d]=%u,%u,%u,%u",i,w->uid,w->atk_uid,w->card,w->atk_card);
	}

	for(i = 0; i < game->group->total; i++) {
		p = &game->group->player[i];
		DEBUG_LOG("u[%d]=%u,%u,%u,%u",i,p->userid,p->atkpos,p->teamid,p->scores);
	}
	
	DEBUG_LOG("%u,%u,%u,%u",game->scores[0],game->scores[1],game->scores[2],game->scores[3]);
}

int del_attacker(wait_group_t *g,userid_t uid)
{
	int i;

	for(i = 0; i < g->total; i++) {
		if(g->player[i].userid == uid) {
			g->total--;
			g->icount[g->player[i].teamid - 1] -= 1;
			if(i != g->total) {
				g->player[i] = g->player[g->total];
			} else {
				memset(g->player + i,0,sizeof(attacker_t));
			}
			break;
		}
	}

	return g->total;
}

gboolean free_game(gpointer key, gpointer obj, gpointer userdata)
{
	hero_cup_game_t *game = obj;

	REMOVE_TIMERS(game);
	list_del(&game->group->glink);
	g_slice_free1(sizeof(wait_group_t),game->group);
	g_slice_free1(sizeof(hero_cup_game_t),game);

	return TRUE;
}

static void end_game(hero_cup_game_t *game)
{
	int i,j;
	char buff[4096] = {0};
	int teamid[4] = {0,1,2,3};
	home_proto_t *pkg = (home_proto_t*)buff;
	int len = sizeof(home_proto_t) + sizeof(client_proto_t);

	KDEBUG_LOG(0,"timer(%x) game over active=%u",HI32(game->mapid),game->active);

	for(i = 0; i < 3; i++) {
		for(j = i+1; j < 4; j++) {
			if(game->scores[teamid[i]] < game->scores[teamid[j]]) {
				int temp = teamid[i];
				teamid[i] = teamid[j];
				teamid[j] = temp;
			}
		}
	}

	for(i = 0; i < 4; i++) {
		PKG_UINT32(buff,(teamid[i] + 1),len);
		PKG_UINT32(buff,game->scores[teamid[i]],len);
	}

	init_two_head(buff,len,0,61002,0,game->mapid,proto_game_over);

	home_t *p_home;
	for(i = 0; i < 5; i++) {
		p_home	= g_hash_table_lookup(all_home, &pkg->homeid);
		if(p_home) {
			send_to_home(p_home,pkg);
		}
		pkg->homeid++;
	}

	g_hash_table_remove(all_games,&game->mapid);
	free_game(NULL,game,NULL);
}

static int game_loop(void* owner, void* data)
{
	hero_cup_game_t *game = (hero_cup_game_t*)owner;

	game->timerp = NULL;

	KDEBUG_LOG(0,"timer(%x) left=%d",HI32(game->mapid),(int)(game->end_time - time(NULL)));

	if(time(NULL) >= game->end_time) {
		end_game(game);
		return 0;
	}

	game->timerp = ADD_TIMER(game,timer_game_loop,NULL,10);

	home_t *home = g_hash_table_lookup(all_home,&game->mapid);
	if(!home)  {
		return 0;
	}

	int i;
	int max = 0;
	int count = 0;
	int pos[5] = {0};
	int idx[12] = {0};
	int n = rand() % 3 + 3;
	char buff[2048] = {0};
	position_t *weapon = NULL;
	home_proto_t *pkg = (home_proto_t*)buff;
	int len = sizeof(home_proto_t) + sizeof(client_proto_t);

	for(i = 0; i < 12; i++) {
		idx[max] = i;
		weapon = &game->weapon[i];
		if(!weapon->atk_uid && weapon->uid) max++;
	}

	if(max == 0) return 0;

	for(count = 0; max && count < n; count++) {
		i = rand() % max;
		pos[count] = idx[i];
		idx[i] = idx[--max];
	}

	PKG_UINT32(buff,count,len);
	for(i = 0; i < count; i++) {
		weapon = &game->weapon[pos[i]];
		PKG_UINT32(buff,pos[i],len);
		PKG_UINT32(buff,weapon->uid,len);
		attacker_t *p = get_attacker(game,weapon->uid);
		if(p) p->atkpos = -1;
		memset(weapon,0,sizeof(position_t));
		weapon->level = 1;
	}

	init_two_head(buff,len,0,61002,0,game->mapid,proto_be_attacked);
	send_to_home(home,pkg);

	return 0;
}

int play_card_result(hero_cup_game_t* game, int position)
{
	position_t *weapon = &game->weapon[position];

	if(weapon->state != 0x13) {
		return 0;
	}

	if(weapon->card <= 0 || weapon->card > 4) {
		weapon->winner |= 2;
		weapon->card = rand() % 4 + 1;
	}

	if(weapon->atk_card <= 0 || weapon->atk_card > 4) {
		weapon->winner |= 1;
		weapon->atk_card = rand() % 4 + 1;
	}

	if(weapon->winner == 1) {
		weapon->atk_card = (weapon->card % 4) + 1;
	} else if(weapon->winner == 2) {
		weapon->card = (weapon->atk_card % 4) + 1;
	} else if(weapon->winner == 3) {
		memset(weapon,0,sizeof(position_t));
		weapon->level = 1;
		return 0;
	}

	char buff[2048] = {0};
	home_proto_t *pkg = (home_proto_t*)buff;
	int len = sizeof(home_proto_t) + sizeof(client_proto_t);

	KDEBUG_LOG(HI32(game->mapid),"calc card position(%d) [%u,%u,%u,%d,%u,%d]",position,weapon->state,weapon->winner,weapon->uid,weapon->card,weapon->atk_uid,weapon->atk_card);

	if((weapon->card % 4) + 1 == weapon->atk_card) {
		PKG_UINT32(buff,0,len);
		PKG_UINT32(buff,weapon->uid,len);
		PKG_UINT32(buff,weapon->card,len);
		PKG_UINT32(buff,weapon->atk_uid,len);
		PKG_UINT32(buff,weapon->atk_card,len);
		PKG_UINT32(buff,position,len);

		attacker_t *p = get_attacker(game,weapon->atk_uid);
		if(p) p->atkpos = -1;

		weapon->state = 0;
		weapon->atk_uid = 0;
		weapon->freezen = time(NULL) + 9;
	} else if((weapon->atk_card % 4) + 1 == weapon->card) {
		PKG_UINT32(buff,0,len);
		PKG_UINT32(buff,weapon->atk_uid,len);
		PKG_UINT32(buff,weapon->atk_card,len);
		PKG_UINT32(buff,weapon->uid,len);
		PKG_UINT32(buff,weapon->card,len);
		PKG_UINT32(buff,position,len);

		attacker_t *p = get_attacker(game,weapon->uid);
		if(p) p->atkpos = -1;

		weapon->state = 0;
		weapon->uid = weapon->atk_uid;
		weapon->atk_uid = 0;
		weapon->freezen = time(NULL) + 9;
	} else {
		PKG_UINT32(buff,1,len);
		PKG_UINT32(buff,weapon->uid,len);
		PKG_UINT32(buff,weapon->card,len);
		PKG_UINT32(buff,weapon->atk_uid,len);
		PKG_UINT32(buff,weapon->atk_card,len);
		PKG_UINT32(buff,position,len);
		weapon->state = 0x10;
	}

	weapon->card = 0;
	weapon->level = 1;
	weapon->winner = 0;
	weapon->atk_card = 0;

	home_t *home = g_hash_table_lookup(all_home, &game->mapid);
	if(home) {
		init_two_head(buff,len,0,61002,0,game->mapid,proto_challenge_result);
		send_to_home(home,pkg);
	}
	return 0;
}

static wait_group_t* alloc_group_wait(wait_group_t *gi)
{
	wait_group_t* g = NULL;

	g = g_slice_alloc0(sizeof(wait_group_t));
	memcpy(g,gi,sizeof(wait_group_t));
	list_add_tail(&g->glink, &wait_head);

	int i;
	for(i = 0; i < g->total; i++) {
		g_hash_table_insert(all_player,&g->player[i].userid,g);
	}

	return g;
}

static void merge_group_wait(wait_group_t *g,wait_group_t *tmp)
{
	int i;
	int j;

	g->icount[0] += tmp->icount[0];
	g->icount[1] += tmp->icount[1];
	g->icount[2] += tmp->icount[2];
	g->icount[3] += tmp->icount[3];

	for(i = 0; i < tmp->total; i++) {
		int add_flag = 1;
		for(j = 0; j < g->total; j++) {
			if(g->player[j].userid == tmp->player[i].userid) {
				add_flag = 0;
				g->icount[g->player[j].teamid - 1] -= 1;
				g->player[j] = tmp->player[i];
				g->player[j].status = 0;
				g_hash_table_insert(all_player,&g->player[i].userid,g);
			}
		}
		if(add_flag) {
			g->player[g->total] = tmp->player[i];
			g->player[g->total].status = 0;
			g_hash_table_insert(all_player,&g->player[g->total].userid,g);
			g->total++;
		}
	}

	KDEBUG_LOG(0,"total=%d,icount=(%u,%u,%u,%u)",g->total,g->icount[0],g->icount[1],g->icount[2],g->icount[3]);
}

static int check_group_matched(wait_group_t *g)
{
	int i,j;
	uint8_t temp;
	uint8_t rank[4];

	if(g->total < TOTAL_CMP) return 0;

	return 1;

	memcpy(rank,g->icount,4);
	for(i = 0; i < 3; i++) {
		for(j = i + 1; j < 4; j++) {
			if(rank[i] < rank[j]) {
				temp = rank[i];
				rank[i] = rank[j];
				rank[j] = temp;
			}
		}
	}

	KDEBUG_LOG(0,"rank=(%u,%u,%u,%u)",rank[0],rank[1],rank[2],rank[3]);

	if(rank[3] < MIN_CMP) return 0;
	if(rank[0] - rank[1] > 2) return 0;

	return 1;
}

static hero_cup_game_t* new_game_start(wait_group_t *g)
{
	int i;
	char buff[2048] = {0};
	home_proto_t *pkg = (home_proto_t*)buff;
	client_proto_t *cli = (client_proto_t*)pkg->body;
	int len = sizeof(home_proto_t) + sizeof(client_proto_t);

	hero_cup_game_t* game = g->game;
	if(game == NULL) {
		game = g_slice_alloc0(sizeof(hero_cup_game_t));
		next_dupid++;
		g->game = game;
		game->group = g;
		game->end_time = time(NULL) + ROUND_TIME;
		INIT_LIST_HEAD(&game->timer_list);
		game->mapid = ((next_dupid & 0xFFFFFF) | 0xFE000000);
		game->mapid = (game->mapid << 32) | hero_cup_game_map;
		for(i = 0; i < 12; i++) {
			game->weapon[i].level = 1;
			if(i % 3 == 1) game->weapon[i].level = 30;
		}
		g_hash_table_insert(all_games,&game->mapid,game);
		game->timerp = ADD_TIMER(game,timer_game_loop,NULL,10);
	}

	time_t diff = game->end_time - time(NULL);

	PKG_MAP_ID(buff,game->mapid,len);
	PKG_UINT32(buff,diff,len);
	init_two_head(buff,len,0,61002,0,game->mapid,proto_game_start);

	for(i = 0; i < g->total; i++) {
		if(g->player[i].status) continue;
		g->player[i].status = 1;
		g->player[i].atkpos = -1;
		g_hash_table_remove(all_player,&g->player[i].userid);
		if (all_fds[g->player[i].online]) {
			pkg->id = g->player[i].userid;
			pkg->onlineid = g->player[i].online;
			pkg->homeid = game->mapid + g->player[i].teamid;
			len = 0;
			PKG_MAP_ID(cli->body,pkg->homeid,len);
			send_pkg_to_client(all_fds[pkg->onlineid], pkg, pkg->len);
		} else {
			KDEBUG_LOG(pkg->id,"send group failed [onlineid=%d]", g->player[i].online);
		}
	}

	return game;
}

int join_hero_cup_op(home_proto_t *pkg)
{
	int i;
	wait_group_t temp;
	wait_group_t *g = NULL;
	wait_group_t *gi = &temp;

	int len = sizeof(client_proto_t);
	memset(gi,0,sizeof(wait_group_t));
	CHECK_BODY_LEN_GE(pkg->len, sizeof(home_proto_t) + len + 4);
	UNPKG_UINT32(pkg->body,gi->total,len);
	if(gi->total <= 0 || gi->total > 5) {
		KDEBUG_LOG(pkg->id,"join_hero_cup_op players=%d",gi->total);
		return 0;
	}
	CHECK_BODY_LEN(pkg->len, sizeof(home_proto_t) + sizeof(client_proto_t) + 4 + 8 * gi->total);

	for(i = 0; i < gi->total; i++) {
		UNPKG_UINT32(pkg->body,gi->player[i].userid,len);
		UNPKG_UINT32(pkg->body,gi->player[i].teamid,len);
		gi->player[i].online = pkg->onlineid;
		gi->icount[gi->player[i].teamid-1] += 1;
		if(gi->player[i].teamid < 1 || gi->player[i].teamid > 4) {
			KDEBUG_LOG(pkg->id,"join_hero_cup_op [onlineid=%d,userid=%u,teamid=%u]",
				gi->player[i].online,gi->player[i].userid,gi->player[i].teamid);
			return 0;
		}
	}

	list_head_t *link;
	list_for_each(link, &wait_head) {
		g = list_entry(link, wait_group_t, glink);
		if(g->total + gi->total <= 40){
			merge_group_wait(g,gi);
			if(check_group_matched(g)) {
				new_game_start(g);
			}
			return 0;
		}
	}

	alloc_group_wait(gi);

	return 0;
}

int cancel_hero_cup_op(home_proto_t *pkg)
{
	uint32_t uid = pkg->id;
	
	CHECK_BODY_LEN(pkg->len, sizeof(home_proto_t) + sizeof(client_proto_t));

	wait_group_t *g = g_hash_table_lookup(all_player,&uid);

	if(g) {
		g_hash_table_remove(all_player,&uid);
		if(del_attacker(g,uid) == 0) {
			list_del(&g->glink);
			g_slice_free1(sizeof(wait_group_t),g);
		}
	}

	return 0;
}

int set_position_hero_cup_op(home_t* p_home, sprite_ol_t* p, home_proto_t *pkg)
{
	uint32_t pos = 0;
	int i = sizeof(client_proto_t);
	client_proto_t *cli = (client_proto_t*)pkg->body;

	CHECK_BODY_LEN(pkg->len, sizeof(home_proto_t) + i + 4);

	UNPKG_UINT32(pkg->body, pos ,i);

//	KDEBUG_LOG(pkg->id,"set_position %u",pos);

	attacker_t *atker = NULL;
	position_t *weapon = NULL;
	hero_cup_game_t* game = NULL;

	char buff[2048] = {0};
	home_proto_t *proto = (home_proto_t*)buff;
	int len = sizeof(home_proto_t) + sizeof(client_proto_t);

	if(pos >= 12) {
		cli->ret = htonl(err_invalid_position);
		goto error;
	}

	game = g_hash_table_lookup(all_games,&p_home->homeid);
	if(!game || LO32(p_home->homeid) != hero_cup_game_map) {
		cli->ret = htonl(err_invalid_home_map);
		goto error;
	}

	atker = get_attacker(game,p->id);
	if(!atker)  {
		cli->ret = htonl(err_invalid_home_map);
		goto error;
	}
	if(atker->atkpos < 12)  {
		cli->ret = htonl(err_invalid_position);
		goto error;
	}

	weapon = &game->weapon[pos];

	if(weapon->level > p->sprite_info.level) {
		cli->ret = htonl(err_position_level);
		goto error;
	}
	if(weapon->freezen >= time(NULL)) {
		cli->ret = htonl(err_position_freezen);
		goto error;
	}

	if(weapon->uid == 0) {
		PKG_UINT32(buff,p->id,len);
		PKG_UINT32(buff,0,len);
		PKG_UINT32(buff,pos,len);
		init_two_head(buff,len,0,61002,0,game->mapid,pkg->opid);
		send_to_home(p_home,proto);

		atker->atkpos = pos;
		memset(weapon,0,sizeof(position_t));
		weapon->uid = p->id;
		weapon->level = 1;
		return 0;
	} 

	if(weapon->uid != p->id && !weapon->atk_uid) {
		PKG_UINT32(buff,weapon->uid,len);
		PKG_UINT32(buff,p->id,len);
		PKG_UINT32(buff,pos,len);
		init_two_head(buff,len,0,61002,0,game->mapid,pkg->opid);
		send_to_home(p_home,proto);

		atker->atkpos = pos;
		weapon->state = 0x10;
		weapon->atk_uid = p->id;
		KDEBUG_LOG(pkg->id,"set_position [play card with %u]",weapon->uid);
		return 0;
	}

	cli->ret = htonl(err_position_is_busy);
error:
	pkg->len = len;
	cli->len = htonl(sizeof(client_proto_t));
	KDEBUG_LOG(pkg->id,"set_position [err=%u]",ntohl(cli->ret));
	return send_to_online(pkg,pkg->len);
}

int play_card_hero_cup_op(home_t* p_home, sprite_ol_t* p, home_proto_t *pkg)
{
	int card = 0;
	attacker_t *atker = NULL;
	position_t * weapon = NULL;
	hero_cup_game_t* game = NULL;
	int i = sizeof(client_proto_t);
	
	CHECK_BODY_LEN(pkg->len,  sizeof(home_proto_t) + i + 4);
	
	UNPKG_UINT32(pkg->body, card ,i);

	game = g_hash_table_lookup(all_games,&p_home->homeid);
	if(game && LO32(p_home->homeid) == hero_cup_game_map) {
		atker = get_attacker(game,p->id);
		if(!atker || atker->atkpos >= 12) {
			KDEBUG_LOG(pkg->id,"play card atkpos invalid");
			return 0;
		}
		weapon = &game->weapon[atker->atkpos];
		if(weapon->state == 0) {
			KDEBUG_LOG(pkg->id,"weapon state == 0");
			return 0;
		}

		if(weapon->uid == p->id) {
			weapon->state |= 1;
			weapon->card = card;
		} else {
			weapon->state |= 2;
			weapon->atk_card = card;
		}

		play_card_result(game,atker->atkpos);
	}

	return 0;
}

int leave_position_hero_cup_op(home_t* p_home, sprite_ol_t* p, home_proto_t *pkg)
{
	attacker_t *owner = NULL;
	position_t *weapon = NULL;
	hero_cup_game_t* game = NULL;
	
	char buff[2048] = {0};
	home_proto_t *proto = (home_proto_t*)buff;
	int len = sizeof(home_proto_t) + sizeof(client_proto_t);

	game = g_hash_table_lookup(all_games,&p_home->homeid);
	if(!game || LO32(p_home->homeid) != hero_cup_game_map) {
		goto error;
	}

	owner = get_attacker(game,p->id);
	if(!owner || owner->atkpos >= 12) {
		goto error;
	}

	weapon = &game->weapon[owner->atkpos];

	if(weapon->uid == owner->userid && weapon->atk_uid) {
		return 0;
	}

	if(weapon->atk_uid == owner->userid && weapon->uid) {
		return 0;
	}

	PKG_UINT32(buff,p->id,len);
	PKG_UINT32(buff,owner->atkpos,len);
	init_two_head(buff,len,0,61002,0,game->mapid,pkg->opid);
	send_to_home(p_home,proto);

	owner->atkpos = -1;
	memset(weapon, 0, sizeof(position_t));

	return 0;
error:
	PKG_UINT32(buff,p->id,len);
	PKG_UINT32(buff,100,len);
	init_two_head(buff,len,p->onlineid,61002,p->id,p_home->homeid,pkg->opid);
	return send_to_online(pkg,pkg->len);
}

int attack_boss_hero_cup_op(home_t* p_home, sprite_ol_t* p, home_proto_t *pkg)
{
	CHECK_BODY_LEN(pkg->len, sizeof(home_proto_t) + sizeof(client_proto_t));

	time_t now = time(NULL);
	attacker_t *owner = NULL;
	position_t *weapon = NULL;
	hero_cup_game_t* game = NULL;

	game = g_hash_table_lookup(all_games,&p_home->homeid);
	if(game && LO32(p_home->homeid) == hero_cup_game_map) {
		owner = get_attacker(game,p->id);
		if(!owner || owner->atkpos >= 12) {
			KDEBUG_LOG(pkg->id,"attack boss [pos=%u]",owner->atkpos);
			return 0;
		}

		weapon = &game->weapon[owner->atkpos];

		if(weapon->state == 0 && weapon->uid == p->id && weapon->lastatk + 2 < now) {
			weapon->lastatk = now;
			if(owner->atkpos % 3 != 1) {
				owner->scores++;
				game->scores[owner->teamid - 1]++;
			} else {
				owner->scores += 3;
				game->scores[owner->teamid - 1] += 3;
			}

			if(game->scores[owner->teamid - 1] >= 200) {
				game->scores[owner->teamid - 1] = 200;
				end_game(game);
				return 0;
			}

			int i;
			char buff[2048] = {0};
			mapid_t mapid = game->mapid;
			home_proto_t *proto = (home_proto_t*)buff;
			int len = sizeof(home_proto_t) + sizeof(client_proto_t);
			PKG_UINT32(buff,owner->atkpos,len);
			PKG_UINT32(buff,owner->userid,len);
			PKG_UINT32(buff,owner->scores,len);
			PKG_UINT32(buff,owner->teamid,len);
			PKG_UINT32(buff,game->scores[owner->teamid - 1],len);
			init_two_head(buff,len,0,61002,0,game->mapid,proto_update_score);

			for(i = 0; i < 5; i++) {
				p_home	= g_hash_table_lookup(all_home, &mapid);
				if(p_home) {
					send_to_home(p_home,proto);
				}
				mapid++;
			}
		}
	}

	return 0;
}

int get_positions_hero_cup_op(home_t* p_home, sprite_ol_t* p, home_proto_t *pkg)
{
	int i = sizeof(client_proto_t);
	CHECK_BODY_LEN(pkg->len,  sizeof(home_proto_t) + i);

	char buff[2048] = {0};
	home_proto_t *proto = (home_proto_t*)buff;
	int len = sizeof(home_proto_t) + sizeof(client_proto_t);

	hero_cup_game_t* game = NULL;
	client_proto_t *cli = (client_proto_t*)pkg->body;

	mapid_t mapid = (p_home->homeid & 0xFFFFFFFF00000000) | hero_cup_game_map;

	game = g_hash_table_lookup(all_games,&mapid);
	if(!game) {
		cli->ret = htonl(err_invalid_home_map);
		cli->len = htonl(sizeof(client_proto_t));
		pkg->len = sizeof(client_proto_t) + sizeof(home_proto_t);
		KDEBUG_LOG(pkg->id,"get_positions users [err=%u]",ntohl(cli->ret));
		return send_to_online(pkg,pkg->len);
	}

	for(i = 0; i < 12; i++) {
		PKG_UINT32(buff,game->weapon[i].uid,len);
	}

	uint32_t diff = game->end_time - time(NULL);
	PKG_UINT32(buff,diff,len);
	init_two_head(buff,len,pkg->onlineid,61002,pkg->id,game->mapid,pkg->opid);
	send_to_online(buff,proto->len);

	return 0;
}

int init_hero_cup(void)
{
	INIT_LIST_HEAD(&wait_head);
	all_player = g_hash_table_new(g_int_hash, g_int_equal);
	all_games = g_hash_table_new(g_int64_hash, g_int64_equal);

	REGIST_TIMER(timer_game_loop,		game_loop);
	return 0;
}

int free_hero_cup(void)
{
	g_hash_table_remove_all(all_player);
	g_hash_table_foreach_remove(all_games,free_game,0);
	list_head_t *link;
	list_head_t* next;
	list_for_each_safe(link,next,&wait_head) {
		wait_group_t *g = list_entry(link, wait_group_t, glink);
		list_del(&g->glink);
		g_slice_free1(sizeof(wait_group_t),g);
	}
	return 0;
}

