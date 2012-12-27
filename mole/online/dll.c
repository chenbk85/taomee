#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <dlfcn.h>
#include <dirent.h>
#include <sys/mman.h>

#include <libxml/tree.h>

#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>
#include "util.h"
#include "logic.h"

#include "dll.h"
#include "mole_angel.h"

#define GAMES_NUM_MAX	200
#define GAME_HASH_SLOT_NUM	100
static game_t all_games[GAMES_NUM_MAX];
static int games_num;
static list_head_t slots[GAME_HASH_SLOT_NUM];

int char_indx = 6;
char* char_match[68] = {
	"mole",	"thin", "very", "lion", "frog", "black", "white", "glad", "each", "piano",
	"park", "dish", "sail", "duck", "sink", "eight", "plane", "gift", "score", "minus",
	"thank", "stamp", "ship", "sofa", "pour", "soul", "jump", "bump", "mask", "radio",
	"wash", "blue", "red", "flat", "mean", "sing", "lost", "windy", "poem", "song",
	"voice", "form", "head", "third", "even", "prove", "young", "wipe", "width", "beside",
	"title", "tiny", "tale", "family", "soft", "stew", "rule", "rice", "repair", "first",
	"stop", "smile", "eye", "rough", "every", "paint", "easy", "wolf"
};


//----------------------------- load games -----------------------------
int load_dll_conf(const char* file)
{
	int i, k, err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(file);
	if (!doc) {
		ERROR_RETURN(("load items config failed"), -1);
	}

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG("xmlDocGetRootElement error");
		goto exit;
	}

	DECODE_XML_PROP_INT(games_num, cur, "Count");
	if (games_num < 0 || games_num > GAMES_NUM_MAX) {
		ERROR_LOG("error games_num: %d", games_num);
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
			DECODE_XML_PROP_INT(k, cur, "Players");
			if (k > MAX_PLAYERS_PER_GAME && i != 0) {
				ERROR_LOG("too many players in game: id=%d", i);
				goto exit;
			}
			all_games[i].players = k;
			DECODE_XML_PROP_INT_DEFAULT(all_games[i].pk, cur, "PK", 0);
			DECODE_XML_PROP_INT_DEFAULT(all_games[i].vip, cur, "VIP", 0);
			DECODE_XML_PROP_INT_DEFAULT(all_games[i].game_bonus_id, cur, "GameBonusID", 0);
			DECODE_XML_PROP_INT(all_games[i].exp, cur, "Exp");
			DECODE_XML_PROP_INT(all_games[i].strong, cur, "Strong");
			DECODE_XML_PROP_INT(all_games[i].IQ, cur, "IQ");
			DECODE_XML_PROP_INT(all_games[i].lovely, cur, "Lovely");
			DECODE_XML_PROP_INT(all_games[i].yxb, cur, "MaxYXB");
			DECODE_XML_PROP_INT(all_games[i].score, cur, "MaxScore");
			decode_xml_prop_uint32_default(&(all_games[i].aux_item), cur, "AuxItem", 0);
			DECODE_XML_PROP_INT_DEFAULT(all_games[i].aux_yxb, cur, "AuxMaxYXB", 0);

			all_games[i].bonus_cnt = decode_xml_prop_arr_int_default(all_games[i].itembonus, MAX_ITEM_BONUS, cur, "ItemBonus", 0);
			decode_xml_prop_arr_int_default(all_games[i].bonus_rate, MAX_ITEM_BONUS, cur, "BonusRate", 0);
			decode_xml_prop_arr_int_default(all_games[i].bonus_rate_numerator, MAX_ITEM_BONUS, cur, "BonusRateNumerator", 1);
			decode_xml_prop_arr_int_default(all_games[i].bonus_cond, 2, cur, "BonusCond", 0);

			DECODE_XML_PROP_INT_DEFAULT(all_games[i].tm, cur, "MinTime", 0);
			DECODE_XML_PROP_INT_DEFAULT(all_games[i].coins_per_min, cur, "CoinsPerMin", 0);

			//INIT_LIST_HEAD(&all_games[i].group_list);

			i++;
		}
		if (i == GAMES_NUM_MAX)
			break;
		cur = cur->next;
	}

	if (i != games_num) {
		ERROR_LOG ("parse %s failed, games Count=%d, get Count=%d", file, games_num, i);
		goto exit;
	}
	err = 0;
exit:
	xmlFreeDoc (doc);
	return err;
}

static int register_plugin(game_t* p)
{
	char* error;

#define DLFUNC_NO_ERROR(h, v, name) do { \
	v = dlsym(h, name); \
	dlerror(); \
}while (0)

#define DLFUNC(h, v, name) do { \
	v = dlsym(h, name); \
	if ((error = dlerror()) != NULL) { \
		ERROR_LOG("dlsym error, %s", error); \
		dlclose(h); \
		h = NULL; \
		return -1; \
	} \
}while (0)

	// if single-player game
	if ( p->file[0] ) {
		p->handle = dlopen(p->file, RTLD_NOW);
		if ((error = dlerror()) != NULL) {
			ERROR_LOG("dlopen error, %s", error);
			return -1;
		}

		DLFUNC(p->handle, p->game_init, "game_init");
		DLFUNC(p->handle, p->game_destroy, "game_destroy");
		DLFUNC(p->handle, p->on_game_begin, "on_game_begin");
		DLFUNC(p->handle, p->on_game_end, "on_game_end");
		DLFUNC(p->handle, p->on_game_data, "on_game_data");

		return p->game_init();
	}

	return 0;
}

int load_games(const char* conf)
{
	int i;
	for (i = 0; i != GAME_HASH_SLOT_NUM; i++) {
		INIT_LIST_HEAD(&slots[i]);
	}

	int err = load_dll_conf(conf);

	BOOT_LOG(err, "Parse Game File %s", conf);
}

int load_dlls()
{
	int i, err = 0;
	for (i = 0; i != games_num; i++) {
		err = register_plugin(&all_games[i]);
		if (err) break;
		TRACE_LOG("Load Game: %s", all_games[i].file);
	}
	BOOT_LOG(err, "Load Dlls");
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

//----------------------------- load games end -----------------------------


//----------------------------- gamegrp operations -----------------------------
static inline void
all_exit_group(game_group_t *ggp)
{
	int i = 0;
	// all players exit gamegrp
	for ( ; i != ggp->count; ++i ) {
		sprite_t* p = ggp->players[i];
		p->group = NULL;
		if (p->gitem) {
			p->gitem->occupied = 0;
			p->gitem = NULL;
		}
		RESET_GAME_STATUS(p);
	}
	ggp->count = 0;
	// all watchers exit gamegrp
/*	for ( i = 0; i != ggp->nwatchers; ++i ) {
		sprite_t* p = ggp->watchers[i];
		p->group = NULL;
		p->gitem = NULL;
		RESET_GAME_STATUS(p);
	}
	ggp->nwatchers = 0;*/
}

static inline int
player_exit_gamegrp(sprite_t* p)
{
	assert(IS_GAME_PLAYER(p));

	int i = 0;
	game_group_t* ggp = p->group;
	for ( ; i != ggp->count; ++i ) {
		if (ggp->players[i] == p) {
			ggp->players[i] = ggp->players[ggp->count - 1];
			ggp->players[ggp->count - 1] = NULL;
			p->group = NULL;
			if (p->gitem) {
				p->gitem->occupied = 0;
				p->gitem = NULL;
			}
			ggp->count--;
			RESET_GAME_STATUS(p);
			return 0;
		}
	}

	ERROR_RETURN(("can't find id in group, id=%u, group=%d", p->id, ggp->id), -1);
}

static inline int
watcher_exit_gamegrp(sprite_t* p)
{
	assert(IS_GAME_WATCHER(p));

	p->group = 0;
	RESET_GAME_STATUS(p);

	return 0;
/*	int i = 0;
	game_group_t* ggp = p->group;
	for ( ; i != ggp->nwatchers; ++i ) {
		if (ggp->watchers[i] == p) {
			ggp->watchers[i] = ggp->watchers[ggp->nwatchers - 1];
			ggp->watchers[ggp->nwatchers - 1] = NULL;
			p->group = NULL;
			ggp->nwatchers--;
			p->gitem = NULL;
			RESET_GAME_STATUS(p);
			return 0;
		}
	}

	ERROR_RETURN(("can't find id in group, id=%u, group=%d", p->id, ggp->id), -1);*/
}

static inline int
one_exit_group(sprite_t* p)
{
	assert(!NOT_IN_GAME(p));

	return player_exit_gamegrp(p);

/*	if ( IS_GAME_PLAYER(p) ) return player_exit_gamegrp(p);
	else return watcher_exit_gamegrp(p);*/
}

static inline void
set_item_game(const map_item_group_t* mig, int started)
{
	int i = 0;
	map_item_t* itms = get_map_items(mig);
	for ( ; i != mig->itm_count; ++i ) {
		if (itms[i].id > 100) {
			started ? SET_ITEM_GAME_START(&(itms[i])) : SET_ITEM_GAME_END(&(itms[i]));
			break;
		}
	}
}

static uint32_t cur_max_grpid = 0;
game_group_t* alloc_game_group (map_item_group_t *g)
{
	game_group_t* ggp = g_slice_alloc0(sizeof *ggp);

	ggp->id = ++cur_max_grpid;
	ggp->game = g->game;
	ggp->type = g->type;
	ggp->flag = g->flag;

	ggp->mig = g;
	ggp->leavegame_pos_x = g->leavegame_pos_x;
	ggp->leavegame_pos_y = g->leavegame_pos_y;

	INIT_LIST_HEAD(&ggp->timer_list);
	INIT_LIST_HEAD(&ggp->hash_list);
	list_add_tail(&ggp->hash_list, &slots[ggp->id % GAME_HASH_SLOT_NUM]);

	return ggp;
}

game_t* get_game(int id)
{
	if (id < 0 || id >= GAMES_NUM_MAX)
		return NULL;
	return &all_games[id];
}

game_group_t* get_game_group(uint32_t id)
{
	game_group_t *p;

	list_for_each_entry (p, &slots[id % GAME_HASH_SLOT_NUM], hash_list) {
		if (p->id == id)
			return p;
	}

	return NULL;
}
//----------------------------- gamegrp operations end -----------------------------

//----------------------------- functions to send common response -----------------------------
static int redir_to_gameserv(sprite_t* sp)
{
	assert(!NOT_IN_GAME(sp) && GAME_STARTED(sp->group));

typedef struct redir_pkg {
	uint8_t   ip[16];      // ipv4 addr
	in_port_t port;        // port number
	uint8_t   sess[112];    // session id
} __attribute__((packed)) redir_pkg_t;

	game_group_t* gamegrp = sp->group;

	const char*  ip;
	in_port_t    port;
	// get ip address and port number
	ip   = config_cache.bc_elem->gameserv_ip;
	port = config_cache.bc_elem->gameserv_port;

	/*if (gamegrp->game->id == 40 || gamegrp->game->id == 53) {*/
	/*port = config_cache.bc_elem->gameserv_test_port;*/
	/*}*/
	if (gamegrp->game->id == 69) {
		port = config_cache.bc_elem->gameserv_test_port;
	}
	if (gamegrp->game->id == 89 || gamegrp->game->id == 90) {
		ip = config_cache.bc_elem->gameserv_test_ip;
		port = config_cache.bc_elem->gameserv_test_port;
	}
	if ( !ip || !port ) {
		ERROR_RETURN(("gameserver ip or port number unresolvable: grpid=%u", gamegrp->id), -1);
	}

	DEBUG_LOG("SELECT SERVER\t[%u %u %s %d]", sp->id, gamegrp->game->id, ip, port);
	// pack ip address and port number
	redir_pkg_t* rp   = (void*)(msg + sizeof(protocol_t));
	strncpy((void*)(rp->ip), ip, 16);
	rp->port = htons(port);

	int i = 0;
	int len = sizeof(protocol_t) + sizeof(redir_pkg_t);
	PKG_UINT8(msg, sp->group->count, len);
    //DEBUG_LOG("GAME PLAYER CNT [%d %d]", sp->id, sp->group->count);
	for (; i != sp->group->count; ++i) {
		PKG_UINT32(msg, sp->group->players[i]->id, len);
		PKG_UINT8(msg, sp->group->players[i]->gitem->id, len);
	}
	init_proto_head(msg, PROTO_REDIR_TO_GAMESERV_NOTIFY, len);

	// encrypt session id: Time+GameID+OnlineID+GroupID+Flag+Time+UserID+IP
	i = 0;
	char inbuf[8], outbuf[8];
	// pack time and game id
	(*(uint32_t*)&inbuf)    = now.tv_sec;        // time
	(*(uint32_t*)(inbuf+4)) = gamegrp->game->id; // game id
	des_encrypt(DES_KEY, inbuf, outbuf);
	PKG_STR(rp->sess, outbuf, i, sizeof outbuf);
	// pack online id and group id
	uint64_t online_id = config_cache.bc_elem->online_id;
	uint64_t olgrpid   = gamegrp->id;  // group id
	olgrpid |= online_id << 32;        // online id
	des_encrypt(DES_KEY, (void*)&olgrpid, outbuf);
	PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

	const int beg = i;
	uint32_t  tmp;
	if ( IS_GAME_PLAYER(sp) ) {
		// send redirect info to clients
		int idx = 0;
		sprite_t* p;
		for ( ; idx < gamegrp->count; ++idx ) {
			i = beg;
			p = gamegrp->players[idx];
			// pack itemid & flag (identify if is player (1) or watcher (0)) and time
			tmp = p->gitem->id % gamegrp->count; // item id
			if (!tmp) {
				tmp = gamegrp->count;
			}
			tmp <<= 16;
//------------------temp-------------------------------------------
//			tmp  |= (IS_GAME_PLAYER(p) | ((p->flag >> 16) & 0xF800)); // flag
//-----------------------------------------------------------------
			tmp  |= IS_GAME_PLAYER(p);             // flag
			//temp flag
//			DEBUG_LOG("REDIR TO GAMESERVER %d yuanbao FLAG %d", p->id, p->tmpinfo.yuanbao_box);
//			tmp |= p->tmpinfo.yuanbao_box<<8;
			if(p->followed != NULL) {
				if(PET_IS_SUPER_LAHM(p->followed)) {
					tmp |= 1<<7;
				}
			}
			(*(uint32_t*)&inbuf)    = tmp;         // item id & flag
			(*(uint32_t*)(inbuf+4)) = now.tv_sec;  // time
			des_encrypt(DES_KEY, inbuf, outbuf);
			PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

			(*(uint32_t*)&inbuf)    = p->id;      // user id
//			(*(uint32_t*)(inbuf+4)) = get_ip2(p); // ip
			if(p->followed != NULL) {
				(*(uint32_t*)(inbuf+4)) = p->followed->id; // ip
			} else {
				(*(uint32_t*)(inbuf+4)) = 0;
			}
			des_encrypt(DES_KEY, inbuf, outbuf);
			PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

			(*(uint32_t*)&inbuf)    = p->gitem->id;     // item id
			(*(uint32_t*)(inbuf+4)) = 0;  // ip
			des_encrypt(DES_KEY, inbuf, outbuf);
			PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

			(*(uint32_t*)&inbuf)    = p->exp;     //exp
			(*(uint32_t*)(inbuf+4)) = p->strong;  //strong
			des_encrypt(DES_KEY, inbuf, outbuf);
			PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

			// pack 4 basic attr

			(*(uint32_t*)&inbuf)    = p->iq;      // user id
			(*(uint32_t*)(inbuf+4)) = p->lovely;  // ip
			des_encrypt(DES_KEY, inbuf, outbuf);
			PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

			(*(uint32_t*)&inbuf)	= p->oltoday;	// online time today
			(*(uint32_t*)(inbuf+4)) = p->stamp;		// login time
			des_encrypt(DES_KEY, inbuf, outbuf);
			PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

			int k;
			memset(&p->items[p->item_cnt], 0, sizeof(uint32_t) * (MAX_ITEMS_WITH_BODY - p->item_cnt));
			for  ( k = 0; k < MAX_ITEMS_WITH_BODY/2; k++ ) {
				des_encrypt(DES_KEY, (char*)(&p->items[2*k]), outbuf);
				PKG_STR(rp->sess, outbuf, i, sizeof outbuf);
			}

			send_to_self(p, msg, len, 0);
		}
	} else {
		// both itemid & flag for watcher are 0
		(*(uint32_t*)&inbuf)    = 0;   // flag
		(*(uint32_t*)(inbuf+4)) = now.tv_sec; // time
		des_encrypt(DES_KEY, inbuf, outbuf);
		PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

		// pack user id and ip
		(*(uint32_t*)&inbuf)    = sp->id;      // user id
		(*(uint32_t*)(inbuf+4)) = get_ip2(sp); // ip
		des_encrypt(DES_KEY, inbuf, outbuf);
		PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

		// pack 4 basic attr
		(*(uint32_t*)&inbuf)	= sp->exp;	  // user id
		(*(uint32_t*)(inbuf+4)) = sp->strong;  // ip
		des_encrypt(DES_KEY, inbuf, outbuf);
		PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

		(*(uint32_t*)&inbuf)	= sp->iq;	  // user id
		(*(uint32_t*)(inbuf+4)) = sp->lovely;  // ip
		des_encrypt(DES_KEY, inbuf, outbuf);
		PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

		(*(uint32_t*)&inbuf)	= sp->oltoday;	// online time today
		(*(uint32_t*)(inbuf+4)) = sp->stamp;  	// login time
		des_encrypt(DES_KEY, inbuf, outbuf);
		PKG_STR(rp->sess, outbuf, i, sizeof outbuf);

		int k;
		memset(&sp->items[sp->item_cnt], 0, sizeof(uint32_t) * (MAX_ITEMS_WITH_BODY - sp->item_cnt));
		for  ( k = 0; k < MAX_ITEMS_WITH_BODY/2; k++ ) {
			des_encrypt(DES_KEY, (char*)(&sp->items[2 * k]), outbuf);
			PKG_STR(rp->sess, outbuf, i, sizeof outbuf);
		}



		send_to_self(sp, msg, len, 1);
		watcher_exit_gamegrp(sp);
	}

	return 0;
}

static void
response_proto_game_end(game_group_t* ggp, sprite_t* p, uint8_t reason)
{
	assert(GAME_STARTED(ggp));

 	int i = sizeof(protocol_t);
	PKG_UINT32(msg, (p ? p->id : 0), i);
	PKG_UINT8(msg, reason, i);
	init_proto_head(msg, PROTO_GAME_END_NOTIFY, i);

	if (!SCENE_GAME(ggp)) {
		send_to_group(ggp, msg, i);
	} else {
		send_to_map2(p->tiles, msg, i);
	}
}

static void
response_proto_group_leave(map_t *tile, game_group_t *ggp)
{
	int j, i;
	i = sizeof(protocol_t);
	PKG_UINT32(msg, ggp->count, i);
	for (j = 0; j < ggp->count; j++) {
		PKG_UINT32(msg, ggp->players[j]->id, i);
	}
	init_proto_head(msg, PROTO_MAP_LEAVE, i);

	send_to_map2(tile, msg, i);
}

static inline void
notify_gamegrp_info(game_group_t* gamegrp)
{
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, gamegrp->count, j);
	uint8_t i = 0;
	for ( ; i != gamegrp->count; ++i ) {
		sprite_t* p = gamegrp->players[i];
		PKG_UINT32(msg, p->id, j);
		PKG_UINT8(msg, p->gitem->id, j);
	}
	init_proto_head(msg, PROTO_GAMEGRP_INFO, j);
	send_to_group(gamegrp, msg, j);
}

static inline void
notify_game_start(sprite_t* p)
{
	assert(!NOT_IN_GAME(p));

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->group->id, i);
	PKG_UINT32(msg, p->group->game->id, i);
	PKG_UINT16(msg, 1, i); // count
	PKG_UINT32(msg, p->id, i);
	PKG_UINT16(msg, p->session[0], i); // itemid
	init_proto_head(msg, PROTO_GAME_START_NOTIFY, i);
	send_to_group(p->group, msg, i);
/* Not to be deleted
	PKG_UINT32(msg, p->group->count, i);

	int idx = 0;
	for ( ; idx < p->group->count; ++idx )
		PKG_UINT32(msg, p->group->players[idx]->id, i);
	init_proto_head(msg, PROTO_GAME_START_NOTIFY, i);

	if ( IS_GAME_PLAYER(p) ) {
		send_to_group(p->group, msg, i);
	} else {
		send_to_self(p, msg, i, 1);
		watcher_exit_gamegrp(p);
	}*/
}

// this function isn't made `static` only because `mapgame_info_cmd` is made `inline`
void notify_mapgame_info(sprite_t* p, uint8_t istomap)
{
	assert(MAP_WITH_GAME(p->tiles));

	// send game info of the map
	uint8_t cnt = 0;
	int	i   = sizeof(protocol_t) + 1;
	map_item_t* item;
	list_head_t* l;
	list_for_each(l, &(p->tiles->item_list_head)) {
		item = list_entry(l, map_item_t, list);
		if ( item->id > 100 ) {
			PKG_UINT8(msg, item->id, i);
			PKG_UINT8(msg, ITEM_GAME_STARTED(item), i);
			++cnt;
		}
	}
	int len = i;
	i = sizeof(protocol_t);
	PKG_UINT8(msg, cnt, i);
	init_proto_head(msg, PROTO_MAPGAME_INFO, len);
	if (istomap) {
		send_to_map(p, msg, len, 1);
	} else {
		send_to_self(p, msg, len, 1);
	}
	//DEBUG_LOG("SEND MAPGAME INFO\t[uid=%u cnt=%d mapid=%d]", p->id, cnt, p->tiles->id);
}

static inline void
notify_players_status(sprite_t* p)
{
	assert(INTERACT_SINGLEPLAYER_GAME(p->group));

	int           i     = sizeof(protocol_t);
	game_group_t* grp   = p->group;

	PKG_UINT16(msg, grp->count, i); // count
	int        j  = 0;
	sprite_t*  op;
	for (; j != grp->count; ++j) {
		op = grp->players[j];
		PKG_UINT32(msg, op->id, i);
		PKG_STR(msg, op->session, i, op->sess_len);
		DEBUG_LOG("notify_players_status userid:%d, sess_len:%d", op->id,  op->sess_len);
	}
	init_proto_head(msg, PROTO_OTHERS_GAME_STATUS, i);
	send_to_self(p, msg, i, 1);
}

static inline void
notify_player_leave(const sprite_t* p, game_group_t* grp, uint8_t reason)
{
	int i = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT8(msg, reason, i);
	init_proto_head(msg, PROTO_GAME_LEAVE, i);

	if (!SCENE_GAME(grp)) {
		send_to_group(grp, msg, i);
	} else {
		send_to_map2(p->tiles, msg, i);
	}
}

static inline void
enter_game_rsp(sprite_t* p, const map_item_t* m)
{
	assert(!NOT_IN_GAME(p));

	int i = sizeof (protocol_t);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT32(msg, m->id, i);
	PKG_UINT32(msg, p->action, i);
	PKG_UINT8(msg, p->direction, i);
	PKG_UINT16(msg, IS_GAME_PLAYER(p), i);
	PKG_UINT16(msg, p->group->game->id, i);
	PKG_UINT32(msg, p->group->id, i);
	init_proto_head(msg, PROTO_GAME_ENTER, i);

	if ( IS_GAME_PLAYER(p) ) {
		send_to_map(p, msg, i, 1);
		if (MULTIPLAYER_GAME(p->group) && !GAME_STARTED(p->group)) {
			notify_gamegrp_info(p->group);
		}
	} else {
		send_to_self(p, msg, i, 1);
	}
}
//----------------------------- functions to send common response end -----------------------------

//----------------------------- join/leave game logic -----------------------------
static inline int
no_player(const game_group_t* gamegrp)
{
	return (gamegrp->count == 0);
}

static inline void
player_join_game(sprite_t* p, map_item_t* m)
{
	assert(IS_GAME_PLAYER(p));

	game_group_t* ggp = m->mig->ggp;
	// add player to group
	ggp->players[ggp->count++] = p;
	p->group = ggp;
	if ( MAP_ITM_OCCUPY_AT_ONCE(ggp) || (ggp->count == ggp->game->players) ) {
		SET_ITEM_OCCUPIED(m);
	}
	p->gitem       = m;
	p->action      = m->mig->action;
	p->direction   = m->direction;
	p->sess_len    = 17;
	memset(p->session, 0, p->sess_len);
	p->session[0] = m->id % ggp->game->players;
	if ( !(p->session[0]) ) {
		p->session[0] = ggp->game->players;
	}
}

static inline void
watcher_join_game(sprite_t* p, map_item_t* m)
{
	assert(IS_GAME_WATCHER(p));

	game_group_t* ggp = m->mig->ggp;
	// add watcher to group
//	ggp->watchers[ggp->nwatchers++] = p;
	p->group = ggp;
	// TODO - the following fields might not need to be assigned
//	p->gitem = m;
//	p->action = m->mig->action;
//	p->direction = m->direction;
}

static int join_game(sprite_t* p, map_item_t* m)
{
	game_t* g = m->mig->game;
	game_group_t* ggp = m->mig->ggp;
	if ( unlikely(GAME_STARTED(ggp) && !SCENE_GAME(ggp)) ) {
		// game already started but not watchable or max watcher bound met
		if ( !GAME_WATCHABLE(ggp) ) {
			DEBUG_LOG("GAME ROOM FULL\t[id=%u game=%s group=%d]", p->id, g->name, ggp->id);
			return -1;
		}
		SET_GAME_WATCHER(p);
	} else if ( unlikely(ITEM_OCCUPIED(m)) ) {
		// game hasn't started but map item occupied
		DEBUG_LOG("MAP ITEM OCCUPIED\t[id=%u game=%s group=%d]", p->id, g->name, ggp->id);
		return -1;
	}

	// proceed if (game started but still watchable) or
	// (not started yet and map item not yet occupied)
	if ( unlikely(IS_GAME_WATCHER(p)) ) {
		watcher_join_game(p, m);
		DEBUG_LOG("GAME WATCHER\t[uid=%u mid=%u game=%s grpid=%u]", p->id, m->id, p->group->game->name, p->group->id);
	} else {
		SET_GAME_PLAYER(p);
		player_join_game(p, m);
		DEBUG_LOG("GAME PLAYER\t[uid=%u mid=%u game=%s grpid=%u gameid=%u]", p->id, m->id, p->group->game->name, p->group->id, p->group->game->id);
		//更新统计数据
		if(p->group->game->id >= 69 && p->group->game->id <= 73){//森林骑士卡牌
			db_add_hot_intem(p, p->id, knightcard_hot);
		}
		if(p->group->game->id >= 38 && p->group->game->id <= 40){//骑士对战卡牌
			db_add_hot_intem(p, p->id, fightcard_hot);
		}
	}

	// not a game
	if (g->id == 0) return 0;

	//check if game can be started
	if ( !GAME_STARTED(ggp) && ((!SCENE_GAME(ggp) && (ggp->count == g->players)) || ((ggp->count == 1) && SCENE_GAME(ggp))) ) {
		if (MAP_WITH_GAME(p->tiles) && MAP_VISIBLE_GAME(ggp)) {
			// set item which indicates if the game has started
			set_item_game(ggp->mig, 1);
			// send game info of the map
			notify_mapgame_info(p, 1);
		}
		SET_GAME_START(ggp);
		ggp->start = now.tv_sec;
	}

	return 0;
}

void leave_game(sprite_t* p, uint8_t reason)
{
	assert(GAME_STARTED(p->group));

/*
Online需要从游戏组中将对应的用户删除。如果是主动
退出，Online还需要将用户放回场景地图里。然后，
Online判断一下组中是否还有用户，如果没有了，则删
除组。
*/
	// save game grp pointer before exit group
	game_group_t* ggp = p->group;

	if (!MULTIPLAYER_GAME(ggp)) {
		// TODO - Only Single Player Interactive Game Needs This Package Currently
		notify_player_leave(p, ggp, reason);
	}
	// if fail to exit game group
	one_exit_group(p); // TODO - need to judge its return value?
	// put player to map if not leave game coz of offline and not map visible game
	if ( (reason != LEAVE_GAME_BY_OFFLINE) && !MAP_VISIBLE_GAME(ggp) ) {
		p->posX = ggp->leavegame_pos_x;
		p->posY = ggp->leavegame_pos_y;
		p->waitcmd = PROTO_MAP_ENTER;
		response_proto_get_sprite(p, PROTO_MAP_ENTER, 0, NULL);
	}
	// delete game group if no players in it
	if ( no_player(ggp) ) {
		end_game(ggp, p, reason);
	}
	// ggp might have been freed above, so *DO NOT* use it anymore!

	DEBUG_LOG("LEAVE GAME\t[%u %u]", p->id, reason);
}

/*------------------------------------------------------
  *  @ggp:	game to end
  *  @sp:		player who leads to end of game. NULL means
  *			the game ends not because of a given player.
  *  @reason:	reason why the game ends
  *  !!caution:	if @ggp->count == 0, @sp should not be NULL
  *------------------------------------------------------*/
void end_game(game_group_t* ggp, sprite_t* sp, uint8_t reason)
{
	assert(GAME_STARTED(ggp));

	ggp->end = now.tv_sec;
	if (SCENE_GAME(ggp) && ggp->game->on_game_end) {
		ggp->game->on_game_end(ggp);
	}
	REMOVE_TIMERS(ggp);
	response_proto_game_end(ggp, sp, reason);
	SET_GAME_END(ggp);

	sprite_t* p = (sp ? sp : ggp->players[0]);
	if (MAP_WITH_GAME(p->tiles) && MAP_VISIBLE_GAME(ggp)) {
		set_item_game(ggp->mig, 0);
		// send game info of the map
		notify_mapgame_info(p, 1);
	}

	if ( !MAP_VISIBLE_GAME(ggp) ) {
		int i = 0;
		sprite_t* s;
		for (; i != ggp->count; ++i) {
			s = ggp->players[i];
			s->posX = ggp->leavegame_pos_x;
			s->posY = ggp->leavegame_pos_y;
			response_proto_get_sprite(s, PROTO_MAP_ENTER, 0, NULL);
		}
		all_exit_group(ggp);
		free_game_group(ggp);
	} else {
		all_exit_group(ggp);
		// After a given game group is redirected to GameServ and
		// the game begins, if suddenly all the players within this
		// group are disconnected from Online, then this group
		// is ready to accept another group of players while in
		// GameServ this very game group is owned by some other
		// players, thus the newly redirected group of players can't
		// be accepted! To avoid this, we change the group id
		// every time after game ended.
		ggp->id = ++cur_max_grpid;
	}
}
//----------------------------- join/leave game logic end -----------------------------

//----------------------------- protocol related logic -----------------------------
static inline int
unpkg_get_group(const uint8_t body[], int bodylen, uint32_t* grpid)
{
	CHECK_BODY_LEN(bodylen, 4);
	int i = 0;
	UNPKG_UINT32(body, *grpid, i);
	return 0;
}
int get_group_cmd (sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t groupid;
	if (unpkg_get_group(body, bodylen, &groupid) == -1)
		return -1;

	return do_get_group(p, groupid);
}

static inline int
unpkg_game_score(const uint8_t* body, int len, uint32_t* rate, uint32_t* socre, uint32_t *check_num)
{
	CHECK_BODY_LEN(len, 12);

	int i = 0;
	UNPKG_UINT32(body, *rate, i);
	UNPKG_UINT32(body, *socre, i);
	UNPKG_UINT32(body, *check_num, i);

	return 0;
}
int game_score_cmd(sprite_t* p, const uint8_t* body, int len)
{
	uint32_t rate, score, check_num;
	if (unpkg_game_score(body, len, &rate, &score, &check_num) == -1) return -1;

	int day = get_now_tm()->tm_mday;
	if (!p->group
		||(p->group->game->id == 0)
		||(p->group->game->vip && !ISVIP(p->flag))
		||(check_num != (score * score + day * day))) {
		p->waitcmd = 0;
		uint32_t msg_info[] = {1, p->id};
		msglog(statistic_logfile, 0x04020302, get_now_tv()->tv_sec, msg_info, sizeof(msg_info));

		ERROR_RETURN(("not in game, uid=%u rate=%u score=%u vip=%d isvip=%d", p->id, rate, score,p->group ? p->group->game->vip:0,ISVIP(p->flag)), 0);
	}

	if ((p->group->game->id == 36) && (score > 22000)){
	    uint32_t db_buf[] = {0, 1, 0, 0, 99, 1351382, 1, 1};
	    send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);	    
	}
	else if ((p->group->game->id == 41) && (score > 18000)){
	    uint32_t db_buf[] = {0, 1, 0, 0, 99, 1351384, 1, 1};
	    send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);	    
	}
	else if ((p->group->game->id == 46) && (score > 7000)){
	    uint32_t db_buf[] = {0, 1, 0, 0, 99, 1351386, 1, 1};
	    send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);	    
	}

    msglog(statistic_logfile, 0x04040103,get_now_tv()->tv_sec, &(p->id), 4);
	DEBUG_LOG("GAME SCORE CMD\t[uid=%u gameid=%d rate=%u score=%u]", p->id, p->group->game->id, rate, score);

	//20120622
	uint32_t hall_gameid[] = {47, 42, 1, 33, 48, 9, 67, 24, 20, 85, 49, 36, 41,
		95, 59, 46, 56, 26, 86, 25, 55, 8, 19, 16};
	DEBUG_LOG("-----game score---g-id: %u ----", p->group->game->id);
	int k = 0;
	for(; k < sizeof(hall_gameid)/sizeof(uint32_t); ++k){
		if(p->group->game->id == hall_gameid[k]){
			break;
		}
	}
	if(k < sizeof(hall_gameid)/sizeof(uint32_t)){
		uint8_t db_buff[256] = {0};
		int j = 0;
		PKG_STR(db_buff, p->nick, j, 16);
		PKG_H_UINT32(db_buff, p->group->game->id, j);
		PKG_H_UINT32(db_buff, score,j);
		PKG_H_UINT32(db_buff, 0, j);
		send_request_to_db(SVR_PROTO_MOLE_CHANGE_GAME_SCORE, NULL, j, db_buff, p->id);
		DEBUG_LOG("id: %u, g-score: %u, ", p->group->game->id,  score);
		uint32_t db_user_buf[] = {p->group->game->id, score};
		send_request_to_db(SVR_PROTO_MOLE_CHANGE_USER_GAME, NULL, sizeof(db_user_buf), db_user_buf, p->id);
	}

	if(p->group->game->id == 85)
	{
	    rate = (score*100)/(p->group->game->score);
	    if (rate > 100)
	    {
	        rate = 100;
	    }
		if(score >= 100){
			uint32_t db_buff[] = {46, 1};
			send_request_to_db(SVR_PROTO_SET_CHAPTER_STATE, p, sizeof(db_buff), db_buff, p->id);
		}

	    DEBUG_LOG("2 GAME SCORE CMD\t[uid=%u gameid=%d rate=%u score=%u]", p->id, p->group->game->id, rate, score);
	}
	if(p->group->game->id == 14 && p->group->game->score > 4000){
		db_exchange_single_item_op(p, 202, 1351334, 1, 0);
	}

	return do_game_score(p, rate, score);
}

int enter_game_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t i, j;
	map_item_t* m;

	CHECK_BODY_LEN(bodylen, 4);

	i = 0;
	UNPKG_UINT32(body, j, i);
	m = get_map_item(p->tiles, j);
	if (!m) {
		ERROR_LOG("can't find item: id=%u, map=%lu, item=%d",
						p->id, p->tiles->id, j);
		return send_to_self_error(p, PROTO_GAME_ENTER, -ERR_client_not_proc, 1);
	}
	if (p->group || !NOT_IN_GAME(p)) {
		return send_to_self_error(p, PROTO_GAME_ENTER, -ERR_client_not_proc, 1);
		/*ERROR_RETURN(("already in game: id=%u, map=%u, group=%d",
						p->id, p->tiles->id, p->group->id), -1);
						*/
	}
	if (p->gitem) {
		ERROR_RETURN(("already occupied an item: id=%u, map=%lu, item=%d",
						p->id, p->tiles->id, p->gitem->id), -1);
	}

	if(m->mig->game->vip && !ISVIP(p->flag)) {
		return send_to_self_error(p, PROTO_GAME_ENTER, -ERR_client_not_proc, 1);
	}

	if ( join_game(p, m) == -1) {
		return send_to_self_error(p, PROTO_GAME_ENTER, -ERR_occupied_pos, 1);
	}

	enter_game_rsp(p, m);

	DEBUG_LOG("ENTER GAME\t[uid=%u itmid=%d game=%s grpid=%u cnt=%d]", p->id, m->id, p->group->game->name, p->group->id, p->group->count);

	int            err = 0;
	game_group_t*  grp = p->group;
	if ( GAME_STARTED(grp) || INTERACT_SINGLEPLAYER_GAME(grp) ) {
		// redirect multi-player game to game server
		if ( MULTIPLAYER_GAME(grp) ) {
			DEBUG_LOG("REDIRECT TO GAMESERV\t[uid=%u gameid=%d, grpid=%u]", p->id, grp->game->id, grp->id);
			err = redir_to_gameserv(p);
		} else {
			// tell the group that the game has started
			notify_game_start(p);
			if ( INTERACT_SINGLEPLAYER_GAME(grp) ) {
				notify_players_status(p);
			}
		}
		// call on_game_begin if is Scene Game
		if ( SCENE_GAME(grp) && grp->game->on_game_begin ) {
			// TODO - might need to end Scene Game if some critical errors happen
			err = grp->game->on_game_begin(p);
		}
		// tell others the group has disappeared
		if ( GAME_STARTED(grp) && !MAP_VISIBLE_GAME(grp) ) {
			response_proto_group_leave(p->tiles, grp);
			// release item if game group disappear
			release_map_items(grp, m->mig);
			m->mig->ggp = alloc_game_group(m->mig);
		}
		// For games currently processed in Online, it's guaranteed that
		// `game start time` for a given player is `now.tv_sec`
		p->game_start_time = now.tv_sec;
		DEBUG_LOG("%s STARTED\t[uid=%u itmid=%d grpid=%u]", grp->game->name, p->id, m->id, grp->id);
	}

	return err;
}

int leave_game_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	game_group_t* ggp;
	uint8_t reason;

	CHECK_BODY_LEN(bodylen, 1);
	reason = body[0];
	ggp = p->group;
	if (!ggp) {
		p->waitcmd = 0;
		DEBUG_RETURN( ("NOT IN GAME\t[uid=%u]", p->id), 0 );
	}

	int waitcmd = 0;
	if ( GAME_STARTED(ggp) ) {
		// Leave game when playing. No punishment currently.
		if ( SCENE_GAME(ggp) && ggp->game->on_game_data && ggp->game->on_game_data(p, PROTO_GAME_LEAVE, body, bodylen) ) {
			ERROR_RETURN( ("%s: Leave Scene Game\t[uid=%u]", ggp->game->name, p->id), -1);
		}
		waitcmd = p->waitcmd;
		leave_game(p, reason);
	} else {
		if (ggp->game && p->tiles && ggp->game->id == 0 && p->tiles->id == 35) {
			response_proto_head_map(p->tiles->id, PROTO_DAFEN_OVER);
			DEBUG_LOG("DAFEN OVER\t[%u]", p->id);
		}
		notify_player_leave(p, ggp, reason);

		if (one_exit_group(p)) {
			return -1;
		}

		if (MULTIPLAYER_GAME(ggp)) {
			notify_gamegrp_info(ggp);
		}
	}

	DEBUG_LOG("LEAVE GAME CMD\t[%u %u]", p->id, reason);

	if (waitcmd != PROTO_GAME_SCORE) {
		p->waitcmd = 0;
	}
	return 0;
}

int singleplayer_game_msg_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	if (p->group && INTERACT_SINGLEPLAYER_GAME(p->group)) {
		if (bodylen == 16) {
			memcpy(p->session + 1, body, 16);
		}
		int i = sizeof(protocol_t);
		PKG_UINT32(msg, p->id, i);
		PKG_STR(msg, body, i, bodylen);
		init_proto_head(msg, PROTO_SIGPL_GAME_MSG, i);
		send_to_group(p->group, msg, i);
		p->waitcmd = 0;
		return 0;
	}
	ERROR_RETURN( ("%u Not In Interactable SinglePlayer Game", p->id), -1);
}

int get_matched_string_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, strlen(char_match[char_indx]), j);
	PKG_STR(msg, char_match[char_indx], j, strlen(char_match[char_indx]));

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

//----------------------------- protocol related logic end -----------------------------
