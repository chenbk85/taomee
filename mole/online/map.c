#include <assert.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include <glib.h>
#include <libxml/tree.h>

#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>

#include "communicator.h"
#include "proto.h"
#include "util.h"
#include <libtaomee/log.h>
#include "logic.h"
#include "sprite.h"
#include "shmq.h"
#include "map.h"
#include "item.h"
#include "dll.h"
#include "service.h"
#include "mole_doctor.h"
#include "race_car.h"
#include "hallowmas.h"
#include "small_require.h"
#include "snowball_war.h"
#include "mole_dungeon.h"
#include "final_boss_2011.h"
#include "mole_cutepig.h"
#include "year_feast.h"
#include "advanced_class.h"
#include "mole_cake.h"

#define MAX_MAP_ITEM_NUM	1000

list_head_t			map_hash_slots[MAP_HASH_SLOT_NUM];
static map_item_t	all_items_in_map[MAX_MAP_ITEM_NUM];
static int			item_count;
static list_head_t	map_groups_head;

map_t normal_map[NORMAL_MAP_NUM];

static guint
g_int64_hash (gconstpointer v)
{
  return (guint) *(const gint64*) v;
}

static gboolean
g_int64_equal (gconstpointer v1, gconstpointer v2)
{
  return *((const gint64*) v1) == *((const gint64*) v2);
}


#define PET_MAP_STRUCT_LEN  sizeof(pet_map_t)
static GHashTable*	outside_pets;

static inline void
do_free_outside_petmap(void* petmap)
{
	g_slice_free1(PET_MAP_STRUCT_LEN, petmap);
}

void init_outside_petmap()
{
	outside_pets = g_hash_table_new_full(g_int64_hash, g_int64_equal, 0, do_free_outside_petmap);
}

pet_map_t* get_petmap_by_id(map_id_t id)
{
	pet_map_t* petmap = g_hash_table_lookup(outside_pets, &id);
	return petmap;
}

static pet_map_t*
alloc_petmap(map_id_t id)
{
	pet_map_t* petmap = g_slice_alloc(PET_MAP_STRUCT_LEN);
	if(!petmap)
		return NULL;

	petmap->id = id;
	INIT_LIST_HEAD(&petmap->list);
	g_hash_table_insert(outside_pets, &(petmap->id), petmap);

	return petmap;
}

int add_pet_to_map(pet_t* pet, map_id_t map_id)
{
	pet_map_t* petmap = get_petmap_by_id(map_id);
	if(NULL == petmap){
		if(NULL == (petmap = alloc_petmap(map_id)))
			ERROR_RETURN(("alloc pet_map_t error"), -1);
		DEBUG_LOG("ALLOC PETMAP\t[mapid=%lu]",map_id);
	}

	out_pet_t* outpet = g_slice_alloc(sizeof(out_pet_t));
	if(NULL == outpet)
		ERROR_RETURN(("alloc out_pet_t error"), -1);
	DEBUG_LOG("ALLOC OUTSIDE PET\t[uid=%u pid=%u mapid=%lu]",pet->suppl_info.uid, pet->id, map_id);

	outpet->pet = pet;
	list_add_tail(&outpet->list, &petmap->list);
	return 0;
}

int rm_pet_from_map(sprite_t* p, uint32_t pet_id, map_id_t map_id)
{
	pet_map_t* petmap = get_petmap_by_id(map_id);
	out_pet_t* outpet;

	//DEBUG_LOG("REMOVE OUTSIDE PET\t[petid=%u mapid=%llx]", pet_id, map_id);
	if(!petmap)
		return 0;

	list_for_each_entry(outpet, &(petmap->list), list) {
		if (outpet->pet->id == pet_id && outpet->pet->suppl_info.uid == p->id){
			list_del(&outpet->list);
			g_slice_free1(sizeof(out_pet_t), outpet);
			DEBUG_LOG("FREE OUTSIDE PET\t[petid=%u]", pet_id);

			if(list_empty(&petmap->list)){
				g_hash_table_remove(outside_pets, &map_id);
				DEBUG_LOG("FREE PETMAP\t[mapid=%lu]",map_id);
			}
			return 1;
		}
	}
	return 0;
}

void rm_usrpets_when_logout(sprite_t* p)
{
	map_t* map = get_map(p->id);
	pet_t* pets = &map->pets[0];
	if(map != NULL){
		int i;
		for ( i = 0; i != map->pet_cnt; ++i ) {
			pet_t* pet = pets + i;
			if(rm_pet_from_map(p, pet->id, pet->suppl_info.mapid)){
				pet->flag = 0;
				pet->trust_end_tm  = 0;
				db_set_flag_endtime(0, pet->flag, pet->trust_end_tm, pet->id, p->id);
				respond_pet_to_map(pet, PET_GO_HOME);
				pet->suppl_info.mapid = 0;
			}
		}
	}
}

/*
affect client protocol 233
*/

int get_outside_pets_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{

	int k,j;
	uint32_t count = 0;
	uint8_t lvl;
	out_pet_t* outpet;

	CHECK_BODY_LEN(bodylen, 0);
	pet_map_t* petmap = get_petmap_by_id(p->tiles->id);
	k = sizeof (protocol_t) + 4;
	if(petmap){
		list_for_each_entry(outpet, &(petmap->list), list) {
			pet_t *pet = outpet->pet;
			sprite_t* other = get_sprite((pet->suppl_info).uid);
			if(NULL == other){
				//ERROR_LOG("Can't get sprite %u", (pet->suppl_info).uid);
				continue;
			}

			PKG_UINT32(msg, pet->id, k);
			PKG_UINT32(msg, other->id, k);
			PKG_STR(msg, other->nick, k, 16);
			PKG_UINT32(msg, pet->flag, k);
			PKG_UINT32(msg, pet->birth, k);
			PKG_UINT32(msg, now.tv_sec - pet->logic_birthday, k);
			PKG_STR(msg, pet->nick, k, 16);
			PKG_UINT32(msg, pet->color, k);
			PKG_UINT8(msg, pet->hungry, k);
			PKG_UINT8(msg, pet->thirsty, k);
			PKG_UINT8(msg, pet->sanitary, k);
			PKG_UINT8(msg, pet->spirit, k);

			lvl = get_pet_level(pet);
			PKG_UINT8(msg, lvl, k);
//2010-03-03 skill_ex
			PKG_UINT32(msg, pet->skill, k);
			PKG_UINT32(msg, pet->skill_ex[0], k);
			PKG_UINT32(msg, pet->skill_ex[1], k);
			PKG_UINT32(msg, pet->skill_ex[2], k);
			PKG_UINT32(msg, pet->skill_bits, k);
			PKG_UINT32(msg, pet->skill_value, k);
			PKG_UINT8(msg, pet->skill_hot[0], k);
			PKG_UINT8(msg, pet->skill_hot[1], k);
			PKG_UINT8(msg, pet->skill_hot[2], k);

			PKG_UINT32(msg, pet->suppl_info.pet_clothes[0], k);
			PKG_UINT32(msg, pet->suppl_info.pet_honor[0], k);

			count++;
		}
	}
	j = sizeof (protocol_t);
	PKG_UINT32 (msg, count, j);

	init_proto_head(msg, p->waitcmd, k);
	//DEBUG_LOG("send 233 to pid %u, pet num: %u", p->id, count);
	return send_to_self(p, msg, k, 1);
}

/*
affect client protocol 234
*/

int respond_pet_to_map(pet_t* pet, uint32_t action)
{
	uint8_t lvl;

	sprite_t* p = get_sprite((pet->suppl_info).uid);

	uint32_t mapid = (uint32_t)pet->suppl_info.mapid;
	int k = sizeof(protocol_t);

	PKG_UINT32(msg, pet->id, k);
	PKG_UINT32(msg, p->id, k);
	PKG_STR(msg, p->nick, k, 16);
	PKG_UINT32(msg, action, k);
	PKG_UINT32(msg, pet->flag, k);
	PKG_UINT32(msg, pet->birth, k);
	PKG_UINT32(msg, now.tv_sec - pet->logic_birthday, k);
	PKG_STR(msg, pet->nick, k, 16);
	PKG_UINT32(msg, pet->color, k);
	PKG_UINT8(msg, pet->hungry, k);
	PKG_UINT8(msg, pet->thirsty, k);
	PKG_UINT8(msg, pet->sanitary, k);
	PKG_UINT8(msg, pet->spirit, k);
	lvl = get_pet_level(pet);
	PKG_UINT8(msg, lvl, k);
//2010-03-03 skill_ex
	PKG_UINT32(msg, pet->skill, k);
	PKG_UINT32(msg, pet->skill_ex[0], k);
	PKG_UINT32(msg, pet->skill_ex[1], k);
	PKG_UINT32(msg, pet->skill_ex[2], k);
	PKG_UINT32(msg, pet->skill_bits, k);
	PKG_UINT32(msg, pet->skill_value, k);
	PKG_UINT8(msg, pet->skill_hot[0], k);
	PKG_UINT8(msg, pet->skill_hot[1], k);
	PKG_UINT8(msg, pet->skill_hot[2], k);

	PKG_UINT32(msg, pet->suppl_info.pet_clothes[0], k);
	PKG_UINT32(msg, pet->suppl_info.pet_honor[0], k);

	init_proto_head(msg, PROTO_REP_OUTSIDE_PETS, k);
	//DEBUG_LOG("SEND 234\t[mapid=%u]", mapid);
	send_to_map3(mapid, msg, k);
	return 0;
}

static int load_entry_config(xmlNodePtr cur, struct map *p)
{
	int j;

	cur = cur->xmlChildrenNode;
	j = 0;
	while (cur && j < MAX_ENTRY_NUM_PER_MAP) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Entry")) {
			DECODE_XML_PROP_INT(p->u.entries[j].map, cur, "MapID");
			DECODE_XML_PROP_INT(p->u.entries[j].x, cur, "PosX");
			DECODE_XML_PROP_INT(p->u.entries[j].y, cur, "PosY");
			j++;
		}
		cur = cur->next;
	}
	p->entry_num = j;

	return 0;
}

static int load_items_config(xmlNodePtr cur, map_t* p, map_item_group_t* g)
{
	int j, k;

	DECODE_XML_PROP_INT(g->type, cur, "Type");
	DECODE_XML_PROP_INT_DEFAULT(g->flag, cur, "Flag", gamegrp_map_itm_occupy_at_once);
	DECODE_XML_PROP_INT(g->action, cur, "Action");
	DECODE_XML_PROP_INT(j, cur, "GameID");
	g->game = get_game(j);

	DECODE_XML_PROP_INT(g->leavegame_pos_x, cur, "LG_PosX");
	DECODE_XML_PROP_INT(g->leavegame_pos_y, cur, "LG_PosY");

	g->itm_pos = item_count;

	cur = cur->xmlChildrenNode;
	j = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Item")) {
			DECODE_XML_PROP_INT(k, cur, "ID");
			if (k < 1 || k > 120) { // 101 and beyond for watching game
				ERROR_RETURN(("error item id=%d", k), -1);
			}
			all_items_in_map[j + item_count].id = k;
			//direction
			DECODE_XML_PROP_INT(k, cur, "Direction");
			if (k < 0 || k > 8) {
				ERROR_RETURN(("error item direction=%d", k), -1);
			}
			all_items_in_map[j + item_count].direction = k;
			// if is occupied
			DECODE_XML_PROP_INT(k, cur, "Flag");
			if ( k != 0 && k != 1 ) {
				ERROR_RETURN(("error occupation flag=%d", k), -1);
			}
			all_items_in_map[j + item_count].occupied = k;

			list_add_tail(&all_items_in_map[j + item_count].list, &p->item_list_head);
			all_items_in_map[j + item_count].mig = g;
			j++;
		}
		cur = cur->next;
	}

	if (j + item_count >= MAX_MAP_ITEM_NUM) {
		ERROR_RETURN(("too many items in map, count=%d", j + item_count), -1);
	}

	g->itm_count = j;
	g->ggp = alloc_game_group(g);

	if (SCENE_GAME_MAP(p) && SCENE_GAME(g)) {
		p->gamegrp = g->ggp;
	}

	item_count += g->itm_count;
	list_add_tail(&g->list, &map_groups_head);

	return 0;
}

inline void release_map_items (game_group_t *ggp, map_item_group_t *p)
{
	int i;

	for (i = 0; i < p->itm_count; i++)
		all_items_in_map[p->itm_pos + i].occupied = 0;
	for (i = 0; i < ggp->count; i++)
		ggp->players[i]->gitem = NULL;

}

map_item_t *get_map_items (const map_item_group_t *p)
{
	return &all_items_in_map[p->itm_pos];
}

int plan_down(void* owner, void* data);

int plan_up(void* owner, void* data)
{
	map_t * p = owner;
	response_proto_uint32_map(83, PROTO_PLANT_DOWN, 1);

	DEBUG_LOG("PLANT UP");
	ADD_TIMER_EVENT(p, plan_down, 0, (now.tv_sec + 8*60 - 30));
	return 0;
}

int plan_down(void* owner, void* data)
{
	map_t * p = owner;
	response_proto_uint32_map(83, PROTO_PLANT_DOWN, 2);

	DEBUG_LOG("PLANT DOWN");
	ADD_TIMER_EVENT(p, plan_up, 0, now.tv_sec + 30);
	p->next_flight = now.tv_sec + 8*60;
	return 0;
}

int map_open(void* owner, void* data)
{
	map_t * p = owner;
	response_proto_uint32_uint32_map(p->door_map, PROTO_MAP_OPEN_CLOSE, (uint32_t)p->id, 0);

	ADD_TIMER_EVENT(p, map_open, 0, now.tv_sec + 24*60*60);
	return 0;
}

int map_close(void* owner, void* data)
{
	map_t * p = owner;
	response_proto_uint32_uint32_map(p->door_map, PROTO_MAP_OPEN_CLOSE, (uint32_t)p->id, 1);

	ADD_TIMER_EVENT(p, map_close, 0, now.tv_sec + 24*60*60);
	return 0;
}

int load_maps (const char *file)
{
	int i, err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	map_t *p;
	int map_idx = 0;

	INIT_LIST_HEAD (&map_groups_head);
	for (i = 0; i < MAP_HASH_SLOT_NUM; i++)
		INIT_LIST_HEAD (&map_hash_slots[i]);

	doc = xmlParseFile (file);
	if (!doc)
		ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	srand(time(0));
	//Map
	err = 0;
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Map")){
			//p = g_slice_alloc0(sizeof *p);
			DECODE_XML_PROP_INT (map_idx, cur, "ID");
			p = &normal_map[map_idx];
			p->id = map_idx;
			DECODE_XML_PROP_STR (p->name, cur, "Name");
			DECODE_XML_PROP_INT (p->flag, cur, "Type");
			DECODE_XML_PROP_INT_DEFAULT(p->party_period, cur, "PartyPeriod", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->open_hour, cur, "Open_hour", 0);
			DECODE_XML_PROP_INT_DEFAULT(p->close_hour, cur, "Close_hour", 24);
			DECODE_XML_PROP_INT_DEFAULT(p->door_map, cur, "Door_map", 0);

			INIT_LIST_HEAD (&p->item_list_head);
			INIT_LIST_HEAD (&p->sprite_list_head);
			INIT_LIST_HEAD (&p->timer_list);
			INIT_LIST_HEAD (&p->ritem_list);
			INIT_LIST_HEAD (&p->hero_list_head);

			if (p->door_map) {
				struct tm cur_time = *get_now_tm();
				struct tm tm_tmp = cur_time;
				time_t open_time = 0;
				time_t close_time = 0;

				tm_tmp.tm_hour = p->open_hour;
				tm_tmp.tm_min  = 0;
				tm_tmp.tm_sec  = 0;
				open_time	   = mktime(&tm_tmp);
				tm_tmp.tm_hour = p->close_hour;
				close_time	   = mktime(&tm_tmp);
				if (cur_time.tm_hour >= p->open_hour) {
					open_time += 24*60*60;
				}
				if (cur_time.tm_hour >= p->close_hour) {
					close_time += 24*60*60;
				}

				ADD_TIMER_EVENT(p, map_open, 0, open_time);
				ADD_TIMER_EVENT(p, map_close, 0, close_time);
			}

			if (p->id == 83) {
				ADD_TIMER_EVENT(p, plan_down, 0, now.tv_sec + 8*60);
				p->next_flight = now.tv_sec + 8*60;
			}

			chl = cur->xmlChildrenNode;
			while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar *)"Group")) {
					map_item_group_t* g = g_slice_alloc(sizeof *g);
					err = load_items_config (chl, p, g);
				} else if (!xmlStrcmp(chl->name, (const xmlChar*)"Entries")) {
					err = load_entry_config (chl, p);
				} else if (!xmlStrcmp(chl->name, (const xmlChar*)"RandomItems")) {
					err = load_ritem_config(chl, p);
				}

				if (err == -1) goto exit;
				chl = chl->next;
			}

			p->sprite_num = 0;
			p->item_count = 0;
		}
		cur = cur->next;
	}

exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load map file %s", file);
}

void unload_maps ()
{
	int i;
	list_head_t *l, *p;
	map_t *m;
	map_item_group_t *g;

	for (i = 0; i < MAP_HASH_SLOT_NUM; i++) {
		list_for_each_safe (l, p, &map_hash_slots[i]) {
			m = list_entry (l, struct map, hash_list);
			REMOVE_TIMERS(m);
			destroy_ritems(&(m->ritem_list));
			list_del (&m->hash_list);
			g_slice_free1(sizeof *m, m);

		}
	}

	list_for_each_safe (l, p, &map_groups_head) {
		g = list_entry (l, struct map_item_group, list);
		if (g->ggp) free_game_group (g->ggp);
		list_del (&g->list);
		g_slice_free1(sizeof *g, g);
	}
}

void xform_users_at_map(map_id_t mapid, uint32_t itmid, time_t expiretm)
{
	const map_t* tile = get_map(mapid);

	if (tile) {
		list_head_t* p;
		list_for_each(p, &tile->sprite_list_head) {
			sprite_t* l = list_entry(p, sprite_t, map_list);
			if (!IS_ADMIN_ID(l->id) && !IS_NPC_ID(l->id)) {
				l->action = itmid;
				add_event(&(l->timer_list), show_item_use_expired, l, 0, now.tv_sec + expiretm, ADD_EVENT_REPLACE_UNCONDITIONALLY);

				int i = sizeof(protocol_t);
				PKG_UINT32(msg, 0, i);
				PKG_UINT32(msg, l->id, i);
				PKG_UINT32(msg, itmid, i);
				init_proto_head(msg, PROTO_SHOW_ITEM_USE, i);
				send_to_map2(tile, msg, i);
			}
		}
	}
}

static inline int
map_msg_receivable(const sprite_t* p)
{
	return ( !p->group
			|| (!GAME_STARTED(p->group) && !INTERACT_SINGLEPLAYER_GAME(p->group))
			|| MAP_VISIBLE_GAME(p->group) );
}

static inline void
broadcast_pkg(userid_t uid, map_id_t mapid, const void* buf, int len)
{
	if (!IS_NORMAL_MAP(mapid)){
		const protocol_t* proto = (void*)buf;
		if (use_homeserver) {
			send_to_homeserver(MCAST_home_msg, len, buf, uid, mapid, ntohl(proto->cmd));
		} else {
			send_to_broadcaster(MCAST_home_msg, len, buf, uid, mapid, ntohl(proto->cmd));
		}
	}
}

void send_to_map_except_self(sprite_t* s, uint8_t* buffer, int len, int completed)
{
	list_head_t* p;
	map_t* tile = s->tiles;

	if (tile) {
		list_for_each(p, &tile->sprite_list_head) {
			sprite_t* l = list_entry(p, sprite_t, map_list);
			if (!IS_NPC_ID(l->id) && (map_msg_receivable(l) || (s->waitcmd == PROTO_MAP_ENTER)) && l != s) {
				if(s->sub_grid == l->sub_grid) {
					send_to_self(l, buffer, len, 0);
				}
			}
		}
		broadcast_pkg(s->id, tile->id, buffer, len);
	}
	if (completed)
		s->waitcmd = 0;
}

void send_to_all_player_except_self(sprite_t* s, uint8_t* buffer, int len, int completed)
{
	sprite_t* tmp = NULL;
	uint32_t i =0;
	for (i = 0; i < HASH_SLOT_NUM; i++)
	{
		list_for_each_entry (tmp, &idslots[i], hash_list)
		{
		    if (tmp)
		    {
			    if(s->id != tmp->id && !IS_GUEST_ID(tmp->id))
			    {
//			        DEBUG_LOG("send_to_all_player_except_self userid:%d", tmp->id);
                    send_to_self(tmp, buffer, len, 0);
			    }
			}
		}
	}

	if (completed)
	{
		s->waitcmd = 0;
	}
}

void send_to_all_players(sprite_t* s, uint8_t* buffer, int len, int completed)
{
	sprite_t* tmp = NULL;
	uint32_t i =0;
	for (i = 0; i < HASH_SLOT_NUM; i++)
	{
		list_for_each_entry (tmp, &idslots[i], hash_list)
		{
		    if (tmp)
		    {
			    if(!IS_GUEST_ID(tmp->id))
			    {
//			        DEBUG_LOG("send_to_all_player userid:%d", tmp->id);
                    send_to_self(tmp, buffer, len, 0);
			    }
			}
		}
	}

	if (completed)
	{
		s->waitcmd = 0;
	}
}

void send_to_map(sprite_t* s, uint8_t* buffer, int len, int completed)
{
	list_head_t* p;
	map_t* tile = s->tiles;

	if (!tile) {
		if (!IS_NPC_ID(s->id)) {
			send_to_self(s, buffer, len, 0);
		}
	} else {
		list_for_each(p, &tile->sprite_list_head) {
			sprite_t* l = list_entry(p, sprite_t, map_list);
			if ( !IS_NPC_ID(l->id) && ((l == s)
					|| (map_msg_receivable(l)
					|| (s->waitcmd == PROTO_MAP_ENTER)
					|| (s->waitcmd == PROTO_USER_ITEM_USE))) ) {
				if(s->sub_grid ==l->sub_grid) {
					send_to_self(l, buffer, len, 0);
				}
			}
		}
		broadcast_pkg(s->id, tile->id, buffer, len);
	}

	if (completed)
		s->waitcmd = 0;
}

void send_to_map2(const map_t* tile, uint8_t* buffer, int len)
{
	list_head_t* p;
	list_for_each(p, &tile->sprite_list_head) {
		sprite_t* l = list_entry(p, sprite_t, map_list);
		if (map_msg_receivable(l) && !IS_NPC_ID(l->id)) {
			send_to_self(l, buffer, len, 0);
		}
	}
}

void send_to_map3(map_id_t mapid, uint8_t* buffer, int len)
{
	const map_t* tile = get_map(mapid);

	if (tile) {
		send_to_map2(tile, buffer, len);
	}
}

void send_to_group_except_self(game_group_t* ggp, uint32_t id, void* buffer, int len)
{
	int i;

	for (i = 0; i < ggp->count; i++) {
		if (ggp->players[i]->id != id) {
			send_to_self(ggp->players[i], buffer, len, 0);
		}
	}
}
void send_to_group(game_group_t* ggp, void* buffer, int len)
{
	int i;

	for (i = 0; i < ggp->count; i++) {
		send_to_self(ggp->players[i], buffer, len, 0);
	}
}
int send_to_self(sprite_t *p, uint8_t *buffer, int len, int completed)
{
	int fd;
	int send_bytes, cur_len;
	protocol_t *proto;

	struct shm_block mb;

	fd = sprite_fd(p);
	//if (fd == -1) return -1;

	proto = (protocol_t *) buffer;
	proto->id = htonl (p->id);

	fdsession_t* fdsess = p->fdsess;
	mb.id   = fdsess->id;
	mb.fd   = fd;
	mb.type = DATA_BLOCK;

	for (send_bytes = 0; send_bytes < len; send_bytes += cur_len) {
		if (len - send_bytes > PAGESIZE - sizeof (shm_block_t))
			cur_len = PAGESIZE - sizeof (shm_block_t);
		else
			cur_len = len - send_bytes;

		mb.length = cur_len + sizeof (shm_block_t);

		if (shmq_push (&(config_cache.bc_elem->sendq), &mb, (uint8_t*)(buffer + send_bytes)) == -1)
			ERROR_RETURN (("message is lost: id=%u", p->id), -1);
	}
	if (completed && p->waitcmd != ntohl(proto->cmd)) {
		WARN_LOG("p_waitcmd not equal to proto_cmd\t[%u %u %u]", p->id, p->waitcmd, ntohl(proto->cmd));
	}
	if (completed)
		p->waitcmd = 0;

	fdsess->last_sent_tm = now.tv_sec;
	return 0;
}

static inline void
free_map(map_t *m)
{
//	DEBUG_LOG ("free map\t[%d]", m->id);
	list_del_init(&m->hash_list);
	g_slice_free1(sizeof *m, m);
	//free(m);
}

map_t* alloc_map(map_id_t new_map)
{
	map_t* tile = g_slice_alloc0(sizeof *tile);
	//map_t* tile = calloc(1, sizeof *tile);
	tile->id = new_map;
	tile->sprite_num = 0;

	userid_t uid = new_map;
	if (IS_JY_MAP(new_map))
		uid = GET_UID_IN_JY(new_map);

	sprite_t *owner = get_sprite(uid);
	if (owner)
		snprintf (tile->name, sizeof(tile->name), "%s", owner->nick);
	else
		sprintf (tile->name, "%d", uid);
	INIT_LIST_HEAD (&tile->sprite_list_head);
	INIT_LIST_HEAD (&tile->item_list_head);
	INIT_LIST_HEAD (&tile->timer_list);
	INIT_LIST_HEAD (&tile->ritem_list);
	INIT_LIST_HEAD (&tile->hero_list_head);
	INIT_LIST_HEAD (&tile->hash_list);
	list_add_tail (&tile->hash_list, &map_hash_slots[new_map % MAP_HASH_SLOT_NUM]);
	DEBUG_LOG ("alloc map\t[%lu]", new_map);

	return tile;
}

static inline void
free_house_map(userid_t uid)
{
	DEBUG_LOG ("FREE HOUSE\t[%d]", uid);
	map_t* h1 = get_map(uid);
	map_t* h2 = get_map(HOUSE1_MAP(uid));
	map_t* h3 = get_map(HOUSE2_MAP(uid));

	if (h1 && uid > 50000) { // prevent invalid userid
		list_del_init(&h1->hash_list);
		g_slice_free1(sizeof *h1, h1);
	}

	if (h2) {
		list_del_init(&h2->hash_list);
		g_slice_free1(sizeof *h2, h2);
	}

	if (h3) {
		list_del_init(&h3->hash_list);
		g_slice_free1(sizeof *h3, h3);
	}

}

int get_all_house_sprite_num(userid_t uid)
{
	map_t* h1 = get_map(uid);
	map_t* h2 = get_map(HOUSE1_MAP(uid));
	map_t* h3 = get_map(HOUSE2_MAP(uid));

	int sprite_num = 0;
	if (h1)
		sprite_num += h1->sprite_num;

	if (h2)
		sprite_num += h2->sprite_num;

	if (h3)
		sprite_num += h3->sprite_num;

	return sprite_num;
}

static inline void
try_free_map(sprite_t* p, int notify_self)
{
	map_t* m;

	//free self home
	if (!notify_self) {
		if ( (m = get_map(p->id)) && (get_all_house_sprite_num(p->id) == 0) ) {
			free_house_map(p->id);
		}
		if ( (m = get_map(JY_MAP(p->id))) && (m->sprite_num == 0) ) {
			free_map(m);
		}
		if ( (m = get_map(CLASS_MAP(p->id))) && (m->sprite_num == 0) ) {
			free_map(m);
		}
		if ( (m = get_map(PASTURE_MAP(p->id))) && (m->sprite_num == 0) ) {
			free_map(m);
		}
		/*
		if ( (m = get_map(HOUSE1_MAP(p->id))) && (get_all_house_sprite_num(p->id) == 0) ) {
			free_map(m);
		}
		if ( (m = get_map(HOUSE2_MAP(p->id))) && (get_all_house_sprite_num(p->id) == 0) ) {
			free_map(m);
		}
		*/
        //free other home
	}
	else if ( (m = p->tiles) && (IS_HOUSE_MAP(m->id) || IS_HOUSE1_MAP(m->id) || IS_HOUSE2_MAP(m->id))
		&& ((GET_UID_IN_HOUSE(m->id)) != p->id)	&& (get_all_house_sprite_num(GET_UID_IN_HOUSE(m->id)) == 0)
		&& !get_sprite(GET_UID_IN_HOUSE(m->id))) {
		free_house_map(GET_UID_IN_HOUSE(m->id));
	//free  homeland
	} else if ( (m = p->tiles) && IS_JY_MAP(m->id) && (m->sprite_num == 0) ) {
		//DEBUG_LOG("FREE MAP\t[uid=%u mapid=%u]", p->id, m->id);
		free_map(m);
	//free class
	} else if ( (m = p->tiles) && IS_CLASS_MAP(m->id) && (m->sprite_num == 0)) {
		DEBUG_LOG("FREE CLASS MAP\t[uid=%u mapid=%lu]", p->id, m->id);
		free_map(m);
	} else if ( (m = p->tiles) && IS_PASTURE_MAP(m->id) && (m->sprite_num == 0)) {
		DEBUG_LOG("FREE PASTURE MAP\t[uid=%u mapid=%lu]", p->id, m->id);
		free_map(m);
	}

}

void leave_map(sprite_t *p, int notify_self)
{
	DEBUG_LOG( "Player Leave Map ID:[%d]", p->id );
	if ( !p->tiles ) {
		try_free_map(p, notify_self);
		return;
	}

    if(p->tiles->id == 70) {
        clean_all(p, 0, 1);
    }

    if(p->tiles->id == 47)
    {
        cheer_team_clear_user_info(p);
    }
	if(p->tiles->id == CRYSTAL_DANCING_MAP ){
		kick_off_dacing_user(p);
	}
	if(p->tiles->id == PRAY_ACTIVITY_MAP){
		kick_off_pray_user(p);
	}
	if (p->tiles->id == NEW_YEAR_FEAST_MAP) {//离开新年宴会现场
		year_feast_kill_user(p);
	}

	if (p->tiles->id == 3) {
		if (p->group != NULL) {
			if (p->group->game->id == 68) {
				p->group->game->on_game_data(p, PROTO_GAME_LEAVE, NULL, 0);
			}
		}
	}
	if( p->tiles->id == FB_MAP_ID )
	{
		fb_leave_turret_cmd( p, NULL, 0 );
	}

	sprite_t* who;
	//notify the home's owner that nobody is in his home
	if ( IS_JY_MAP(p->tiles->id) && (who = get_sprite(GET_UID_IN_JY(p->tiles->id)))) {
		int home_sprite_num = get_all_house_sprite_num(who->id);
		if (p->tiles->sprite_num + home_sprite_num == 1) {
			notify_home_users_change(who, 0, 0);
		}
	}
	if ( IS_HOUSE_MAP(p->tiles->id) && (who = get_sprite(p->tiles->id))) {
		map_t* mp = get_map(JY_MAP(who->id));
		int jy_sprite_num = mp ? mp->sprite_num : 0;
		int home_sprite_num = get_all_house_sprite_num(who->id);
		if (home_sprite_num + jy_sprite_num == 1) {
			notify_home_users_change(who, 0, 0);
		}
	}

	//先进班级评选活动，离开地图后，清除数据
	if( p->tiles->id == 38 )
	{
		ac_player_leave( p->id );
	}

	map_t* m = p->tiles;
	assert(m->sprite_num > 0);
	response_proto_leave_map(p, notify_self);
	--(m->sprite_num);
	list_del_init(&p->map_list);

	try_free_map(p, notify_self);
	//send_to_all use the tiles variable, don't move it before the function
	p->tiles = NULL;
	p->sub_grid = 0;
}

int send_to_self_error(sprite_t* p, int cmd, int err, int completed)
{
	protocol_t pkg;

	pkg.ver = 1;
	pkg.cmd = htonl(cmd);
	pkg.ret = htonl(err);
	pkg.len = htonl(sizeof(pkg));

	return send_to_self(p, (uint8_t *)&pkg, sizeof(pkg), completed);
}

map_item_t *get_map_item (struct map *p, int id)
{
	map_item_t *m;

	list_for_each_entry (m, &p->item_list_head, list) {
		if (m->id == id) return m;
	}
	return NULL;
}


struct entry_pos *get_entry (struct map *p, int id)
{
	int i;
	for (i = 0; i < p->entry_num; i++) {
		if (p->u.entries[i].map == id)
			return &p->u.entries[i];
	}
	return NULL;
}

int enter_map(sprite_t *p, map_id_t new, map_id_t old,uint32_t gdst)
{
	map_t* tile;

	tile = get_map(new);
	reset_common_action(p);
	p->direction = 0;
	if (!tile && IS_NORMAL_MAP(new))
		ERROR_RETURN (("can't find the map: %lu, id=%d", new, p->id), -1);
	if (tile && IS_NORMAL_MAP (new)) {
		struct entry_pos *en = get_entry (tile, old);
		if (en) {
			p->posX = en->x;
			p->posY = en->y;
		} else {
			ERROR_RETURN (("not found entry path from map 0x%lu to 0x%lu", old, new), -1);
		}
	} else if (!tile && !IS_NORMAL_MAP (new)){
		tile = alloc_map (new);
	}

	if (IS_JY_MAP (new)) {
		p->posX = 180;
		p->posY = 280;
	}else if (IS_HOUSE_MAP(new) || IS_HOUSE1_MAP(new) || IS_HOUSE2_MAP(new)) {
		p->posX = 440;
		p->posY = 440;
	}else if (IS_CLASS_MAP (new)) {
		p->posX = 400;
		p->posY = 400;
	}else if (IS_PASTURE_MAP (new)) {
		p->posX = 180;
		p->posY = 300;
	}else if(IS_SHOP_MAP(new)){
		p->posX = 400;
		p->posY = 400;
	}else if (IS_CLASSROOM_MAP(new)){
		p->posX = 400;
		p->posY = 400;
	}else if(IS_PARADISE_MAP(new)){
		p->posX = 400;
		p->posY = 400;
	}else if(IS_EXHIBIT_MAP(new)){
		p->posX = 200;
		p->posY = 400;
	}else if(IS_CUTEPIG_MAP(new)){
		p->posX = 200;
		p->posY = 400;
	}
	else if(IS_CUTEPIG_SHOW_MAP(new)){
		p->posX = 200;
		p->posY = 400;
	}	
	else if(IS_CUTEPIG_MACHINE_MAP(new)){
        p->posX = 45;
        p->posY = 220;
    }	


	//for dungeon
	if(p->dungeon_on_explore_map_id != 0)
	{
		dungeon_map_info_t* map_info = &dungeon_map_info.map_info[p->dungeon_on_explore_map_id - 1];
		uint32_t screen_count = map_info->screen_count;
		int i;
		uint8_t dungeon_change_map = 1;
		for(i = 0; i < screen_count; ++i)
		{
			if(	map_info->screen_info[i].map_id == new )
			{
				dungeon_change_map = 0;
				break;
			}
		}
		if( 1 == dungeon_change_map )
		{
			p->dungeon_on_explore_map_id = 0;
			uint32_t dewell_time = time(NULL) - p->dungeon_enter_map_time;
			uint32_t msgbuff[2]= {p->id, dewell_time};
			msglog(statistic_logfile, 0x04088325, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}


	//春天花花同乐会 切换地图后不可以领取奖励
	p->fp_can_get_award = 0;

	/* 卡酷异度空间跳转地图非超拉，扣除点点豆 */

	p->tiles = tile;
	p->sub_grid = gdst;
	list_add_tail (&p->map_list, &tile->sprite_list_head);
	p->tiles->sprite_num++;
	return 0;
}

int get_recent_sprite_in_map_cmd (sprite_t *p, const uint8_t *body, int len)
{
	if( !p->tiles )
		ERROR_RETURN(("no tiles %u", p->id), -1);

	list_head_t* cur;
	int count = 0;
	int i, j;
	i = sizeof(protocol_t);
	j = i + 4;
	list_for_each_prev(cur, &(p->tiles->sprite_list_head)){
		sprite_t* sp = list_entry(cur, sprite_t, map_list);
		if(sp->sub_grid == p->sub_grid) {
			PKG_STR(msg, sp->nick, j, 16);
			count++ ;
		}
		if (count >= 5)
			break;
	}
	PKG_UINT32(msg, count, i);

	DEBUG_LOG("GET RECNT SPRITE\t[pid=%u]", p->id);
	init_proto_head(msg, p->waitcmd, j);
	send_to_self(p, msg, j, 1);

	return 0;
}

int get_map_user_cnt_cmd(sprite_t* p, const uint8_t *body, int len)
{
	map_id_t mapid;
	CHECK_BODY_LEN(len,8);

	int i = 0;
	int count = 0;
	unpkg_mapid(body,&mapid,&i);
	map_t *pmap = get_map(mapid);
	if(pmap) {
		count = pmap->sprite_num;
	} else {
		count = 0;
	}

	if(IS_NORMAL_MAP(mapid)) {
		response_proto_uint32(p, p->waitcmd, count, 0);
	} else {
		if (use_homeserver) {
			send_to_homeserver(MCAST_home_msg, 0, NULL, p->id, mapid, SOP_get_map_users_cnt);
		} else {
			send_to_broadcaster(MCAST_home_msg, 0, NULL, p->id, mapid, SOP_get_map_users_cnt);
		}
		p->sess_len = 0;
		PKG_H_UINT32(p->session,count,p->sess_len);
	}

	return 0;
}

int get_map_user_cnt_callback(sprite_t* p,  uint32_t count)
{
	int local_num = 0;
	p->sess_len = 0;
	UNPKG_H_UINT32(p->session,local_num,p->sess_len);
	
	if(count < local_num) count = local_num;
	DEBUG_LOG("get_map_user_cnt_callback uid %u user_cnt=%d",p->id, count);
	response_proto_uint32(p,p->waitcmd,count,0);
	return 0;
}


