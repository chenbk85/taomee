
/**
 * @file dll.c
 * @brief 游戏种类相关的处理
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <dirent.h>
#include <dlfcn.h>
#include <sys/mman.h>

#include <glib.h>
#include <libxml/tree.h>

#include <libtaomee/log.h>
#include "proto.h"
#include "util.h"
#include "config.h"
#include "service.h"
#include "timer.h"

#include "dll.h"

#define HASH_SLOT_NUM	100
static game_t all_games[GAMES_NUM_MAX];
static int games_num;
static list_head_t slots[HASH_SLOT_NUM];

static int load_dll_conf (const char* file)
{
	int i, err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile (file);
	if (!doc)
		ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	DECODE_XML_PROP_INT (games_num, cur, "Count");
	if (games_num < 0 || games_num > GAMES_NUM_MAX) {
		ERROR_LOG ("error games_num: %d", games_num);
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	i = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Game"))){
			DECODE_XML_PROP_INT(all_games[i].id, cur, "ID");
			if (all_games[i].id != i)
				ERROR_RETURN(("%s parse error: id=%u, num=%d",
					config_get_strval("games_conf"), all_games[i].id, games_num), -1);

			DECODE_XML_PROP_STR(all_games[i].name, cur, "Name");
			DECODE_XML_PROP_STR(all_games[i].file, cur, "File");
			DECODE_XML_PROP_INT(all_games[i].players, cur, "Players");
			DECODE_XML_PROP_INT(all_games[i].gametype, cur, "Type");
			if ( (all_games[i].players > MAX_PLAYERS_PER_GAME && i != 0)
				|| (all_games[i].players < 2 && MULTIPLAYER_GAME(&all_games[i])) ) {
				ERROR_LOG("Invalid Number of Players: id=%d", i);
				goto exit;
			}
			if ( !(all_games[i].file[0]) && MULTIPLAYER_GAME(&all_games[i]) ) {
				ERROR_LOG("File Path Unspecified: id=%d", i);
				goto exit;
			}
			DECODE_XML_PROP_INT(all_games[i].exp, cur, "Exp");
			DECODE_XML_PROP_INT(all_games[i].strong, cur, "Strong");
			DECODE_XML_PROP_INT(all_games[i].IQ, cur, "IQ");
			DECODE_XML_PROP_INT(all_games[i].lovely, cur, "Lovely");
			DECODE_XML_PROP_INT(all_games[i].yxb, cur, "MaxYXB");
			DECODE_XML_PROP_INT(all_games[i].score, cur, "MaxScore");
			DECODE_XML_PROP_INT(all_games[i].timeout, cur, "TimeOut");

			++i;
		}
		if (i == GAMES_NUM_MAX)
			break;
		cur = cur->next;
	}

	if (i != games_num) {
		ERROR_LOG ("parse %s failed, games Count=%d, get Count=%d",
				file, games_num, i);
		goto exit;
	}
	err = 0;
exit:
	xmlFreeDoc (doc);
	return err;
}

static int register_plugin (game_t *p)
{
	char *error;

#define DLFUNC_NO_ERROR(h, v, name) do { \
	v = dlsym (h, name); \
	dlerror (); \
}while (0)

#define DLFUNC(h, v, name) do { \
	v = dlsym (h, name); \
	if ((error = dlerror ()) != NULL) { \
		ERROR_LOG ("dlsym error, %s", error); \
		dlclose (h); \
		h = NULL; \
		return -1; \
	} \
}while (0)

	// if multiplayer game
	if ( p->file[0] ) {
		p->handle = dlopen (p->file, RTLD_NOW);
		if ((error = dlerror()) != NULL) {
			ERROR_LOG("dlopen error, %s", error);
			return -1;
		}

		DLFUNC(p->handle, p->create_game, "create_game");
		DLFUNC(p->handle, p->game_init, "game_init");
		DLFUNC(p->handle, p->game_destroy, "game_destroy");

		return p->game_init();
	}

	return 0;
}

void unload_dlls ()
{
	while (games_num-- > 0) {
		if (all_games[games_num].handle) {
			all_games[games_num].game_destroy ();
			dlclose (&all_games[games_num].handle);
			all_games[games_num].handle = NULL;
		}
	}
}

int load_dlls (const char *conf)
{
	int err, i;

	for (i = 0; i < HASH_SLOT_NUM; i++)
		INIT_LIST_HEAD (&slots[i]);

	load_dll_conf(conf);

	err = 0;
	for (i = 0; i < games_num; i++) {
		err = register_plugin(&all_games[i]);
		if (err) break;
		TRACE_LOG ("load game: %s", all_games[i].file);
	}
	BOOT_LOG (err, "Load game file %s", conf);
}

inline void
free_game_group (game_group_t *ggp)
{
	DEBUG_LOG("%lu\tFREE GROUP", ggp->id);
	REMOVE_TIMERS(ggp);
// 	if( ggp->hash_list.next == NULL || ggp->hash_list.prev == NULL )
// 	{
// 		ERROR_LOG( "free_game_group Invalid hash_list data next:[%p], prev:[%p]", ggp->hash_list.next, ggp->hash_list.prev );
// 	}
// 	else
	{
		list_del(&ggp->hash_list);
	}
	g_slice_free1(sizeof *ggp, ggp);
}

static inline void
all_exit_group (game_group_t* ggp)
{
	int i = 0;
	sprite_t* p;
	// players exit game group
	for ( ; i < ggp->count; ++i ) {
		p = ggp->players[i];
		p->group = 0;
		remove_timers(&(p->timer_list));
		if ( IS_SPRITE_OFFLINE(p) || (p->waitcmd != proto_game_score && p->waitcmd != proto_get_items && p->waitcmd != proto_card_add_win_lost && p->waitcmd != proto_set_redclothes)) {
			DEBUG_LOG("ALL_EXIT_GROUP delete player %d %d", p->id, p->waitcmd);
			del_sprite_conn(p);
		}
	}
	ggp->count = 0;
	// watchers exit game group
	for ( i = 0; i != ggp->nwatchers; ++i ) {
		p = ggp->watchers[i];
		p->group = 0;
		del_sprite_conn(p);
	}
	ggp->nwatchers = 0;
}

/**
 * @brief 从指定游戏组删除指定用户
 * @param sprite_t* p 指定用户，其中包含的组的信息
 * @return 0:成功，其它失败（目前不可能）
 */
static inline int
player_exit_gamegrp(sprite_t* p)
{
	assert(p->group && IS_GAME_PLAYER(p));

	int i = 0;
	game_group_t *ggp = p->group;
	for ( ; i != ggp->count; ++i ) {
		if (ggp->players[i] == p) {
			ggp->players[i] = ggp->players[ggp->count - 1];
			ggp->players[ggp->count - 1] = 0;
			ggp->count--;
			p->group = 0;
			remove_timers(&(p->timer_list));
			return 0;
		}
	}

	ERROR_RETURN(("can't find id in group, id=%u, group=%lu", p->id, ggp->id), -1);
}

static inline int
watcher_exit_gamegrp(sprite_t* p)
{
	assert(p->group && IS_GAME_WATCHER(p));

	int i = 0;
	game_group_t* ggp = p->group;
	for ( ; i != ggp->nwatchers; ++i ) {
		if (ggp->watchers[i] == p) {
			ggp->watchers[i] = ggp->watchers[ggp->nwatchers - 1];
			ggp->watchers[ggp->nwatchers - 1] = 0;
			ggp->nwatchers--;
			p->group = 0;
			return 0;
		}
	}

	ERROR_RETURN(("can't find id in group, id=%u, group=%lu", p->id, ggp->id), -1);
}

// TODO - modify to return void later
int one_exit_group(sprite_t* p)
{
	assert(!NOT_IN_GAME(p) && p->group);

	if ( IS_GAME_PLAYER(p) ) player_exit_gamegrp(p);
	else watcher_exit_gamegrp(p);
// watcher owns no timer
//	remove_timer_event(p->id);
	//DEBUG_LOG("ONE_EXIT_GROUP delete player %d", p->id);
	del_sprite_conn(p);
	return 0;
}

/**
  * all sprites in the same game group are deleted and their connections are
  * also close when multiplayer game is ended
  *
  */
void end_multiplayer_game(game_group_t* ggp, sprite_t* p, uint8_t reason)
{
	//DEBUG_LOG("END MULTIPLAYER GAME\t[grp=%lu uid=%u reason=%d]", ggp->id, p->id, reason);
	// TODO - end needed or not?
	ggp->end = now.tv_sec;
	game_end_response(ggp, p, reason);

	SET_GAME_END(ggp);
	on_game_end(ggp);
	// delete sprites and close connections
	all_exit_group(ggp);
	free_game_group(ggp);
}

static inline game_group_t*
get_game_group (gamegrpid_t id)
{
	DEBUG_LOG("groupid %lu\t get_game_group enter ", id);
	game_group_t* p;

	list_for_each_entry (p, &slots[id % HASH_SLOT_NUM], hash_list) {
		if (p->id == id)
		{
			DEBUG_LOG("groupid %lu\t get_game_group group exist", id);
			return p;

		}

	}

	return NULL;
}

/**
 * @brief 创建一个游戏组对象
 * @param grpid 该组的全局唯一编号，即组ID
 * @param game 哪一种游戏
 * @return 创建的游戏组
 */
static inline game_group_t*
alloc_game_group(gamegrpid_t grpid, game_t* game)
{
	DEBUG_LOG("NEW GAME GROUP ID 3 %lu", grpid);
	game_group_t* ggp = g_slice_alloc0(sizeof *ggp);
	if ( ggp ) {
		ggp->id = grpid;
		ggp->game = game;
		INIT_LIST_HEAD (&ggp->timer_list);
		ADD_TIMER_EVENT(ggp, on_game_timer_expire, 0, now.tv_sec + game->timeout);
		INIT_LIST_HEAD (&ggp->hash_list);
		list_add_tail (&ggp->hash_list, &slots[ggp->id % HASH_SLOT_NUM]);
		DEBUG_LOG("%lu\tNEW GAME GROUP", grpid);
	}
	return ggp;
}

/**
 * @brief 把一个用户加入一个游戏组
 * @param p 相应的用户
 * @param gameid 加入哪种游戏，跳绳或棋牌或其它
 * @param grpid 加入哪个游戏组，这是一个编号
 * @return 0, 表示成功
 */
int join_game(sprite_t* p, int gameid, gamegrpid_t grpid)
{
	assert( p && !(p->group) && (gameid > 0) && (gameid < GAMES_NUM_MAX) );

	game_t* game = &(all_games[gameid]);
#ifndef GAME_TEST
#ifndef STANDALONE_GAMESERVER
	if ( ((p->pos_id == 0) || (p->pos_id > game->players)) && !IS_GAME_WATCHER(p) ) {
		ERROR_RETURN( ("Invalid PosID=%d: uid=%u, game=%s", p->pos_id, p->id, game->name), -1 );
	}
#endif
#endif
	//查看不是已经存在这个组

	game_group_t* gamegrp = get_game_group(grpid);
	if ( !gamegrp ) {
		gamegrp = alloc_game_group( grpid, game );
		if ( !gamegrp ) {
			ERROR_RETURN( ("Fail to allocate game group! id=%u, game=%d, group=%lu",
					p->id, game->id, gamegrp->id), -1 );
		}
	}
#ifndef GAME_TEST
#ifndef STANDALONE_GAMESERVER
	// check if the player's position id is right
	if ( IS_GAME_PLAYER(p) &&
		((p->pos_id == 0) || (p->pos_id > gamegrp->game->players)) ) {
		ERROR_RETURN( ("Wrong PosID: uid=%u posid=%d game=%u group=%lu",
				p->id, p->pos_id, game->id, gamegrp->id), -1 );
	}
#endif
	// check if game room is full
	if ( GAME_READY(gamegrp) &&
		(!GAME_WATCHABLE(gamegrp->game)	|| IS_GAME_PLAYER(p) || (gamegrp->nwatchers == MAX_WATCHERS_PER_GAME)) ) {
		ERROR_RETURN( ("Game Room Full or Unwatchable! id=%u, game=%d, group=%lu, status:%d, gamegrp->flag:%d, gamegrp->nwatchers:%d, gamegrp->game->gametype:%d",
				p->id, game->id, gamegrp->id, p->status, gamegrp->flag, gamegrp->nwatchers, gamegrp->game->gametype), -1 );
	}
#endif

	// proceed if (game ready but still watchable) or not ready yet
	// add sprite to group
	if ( IS_GAME_PLAYER(p) ) gamegrp->players[gamegrp->count++] = p;
	else gamegrp->watchers[gamegrp->nwatchers++] = p;

	p->group = gamegrp;

	// check if game can be ready
	// all games handled here should be multiuser games. gamegrp->flag is set at map.c
	// If flag needed here, could be set at load_dll_conf of this file
//	if ( ( (gamegrp->flag & MULTI_PLAYER_GAME_FLAG) &&
//		   (gamegrp->count == game->players) ) ||
//		 !(gamegrp->flag & MULTI_PLAYER_GAME_FLAG) ) {
	if ( !GAME_READY(gamegrp) && (gamegrp->count == game->players) ) {
		SET_GAME_READY(gamegrp);
		// TODO - start time needed or not?
		gamegrp->start = now.tv_sec;
	}

	return 0;
}

/**
 * @brief 加入边界处理的获取游戏类型，通过编号取得类型
 * @param id 哪一类型的游戏，这只是一个编号
 * @return 游戏类型
 */
static inline game_t*
get_game (int id)
{
	if (id < 0 || id >= GAMES_NUM_MAX)
		return NULL;
	return &all_games[id];
}

/*
static inline int
find_game(const char* name)
{
	int i = 0;
	for ( ; i != games_num; ++i ) {
		if ( !strncmp(all_games[i].name, name, sizeof all_games[i].name) ) {
			return i;
		}
	}

	return -1;
}
*/
