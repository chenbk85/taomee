#include <assert.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include <glib.h>
#include <libxml/tree.h>

#include <libtaomee/timer.h>

#include <libtaomee/log.h>
#include "logic.h"
#include "map.h"
#include "proto.h"
#include "sprite.h"
#include "util.h"

#include "npc.h"
#include "logic.h"

//for lua
#ifdef LUA_TEST

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#endif


enum NpcCmdConst {
	NPC_CMD_enter_map		= 1,
	NPC_CMD_leave_map		= 2,
	NPC_CMD_take_action		= 3,
	NPC_CMD_walk			= 4,
	NPC_CMD_talk			= 5,

	NPC_CMD_xform_others	= 99999999,
	NPC_CMD_destroy_npc		= 100000000,

	NPC_CMD_talk_msg_size	= 1024
};

typedef struct EnterMapCmd {
	uint32_t cmdtype;
	uint32_t oldmap, newmap;
} __attribute__ ((packed)) enter_map_cmd_t;

typedef struct TakeActionCmd {
	uint32_t cmdtype, action;
	uint8_t  direction;
} __attribute__ ((packed)) take_action_cmd_t;

typedef struct WalkCmd {
	uint32_t cmdtype;
	uint16_t x, y;
} __attribute__ ((packed)) walk_cmd_t;

typedef struct TalkCmd {
	uint32_t cmdtype;
	uint16_t msglen;
	uint8_t  msg[];
} __attribute__ ((packed)) talk_cmd_t;

typedef struct XformOthersCmd {
	map_id_t mapid;
	uint32_t itmid;
	uint32_t   expiretm;
} __attribute__ ((packed)) xform_others_cmd_t;

static size_t   npc_count;
static sprite_t npc_array[NPC_NUMBER_MAX];

#ifdef LUA_TEST

lua_State *L;

static void lua_test();
int lua_wrap_enter_map(lua_State *L);
int lua_wrap_leave_map(lua_State *L);
int lua_wrap_talk(lua_State *L);

#endif

static void parse_npc_cmd(sprite_t* npc, const uint8_t* cmdbuf);

static int proc_npc_cmd(void* npc, void* data)
{
	enter_map_cmd_t*    enter_map;
	take_action_cmd_t*  action;
	walk_cmd_t*         walk;
	talk_cmd_t*         talk;

	switch (*((uint32_t*)data)) {
	case NPC_CMD_enter_map:
		DEBUG_LOG("NPC ENTER MAP");
		enter_map = data;
		do_enter_map(npc, enter_map->newmap, enter_map->oldmap,0,0);

		g_slice_free1(sizeof *enter_map, data);
		break;
	case NPC_CMD_leave_map:
		DEBUG_LOG("NPC LEAVE MAP");
		do_leave_map(npc);

		g_slice_free1(sizeof(uint32_t), data);
		break;
	case NPC_CMD_take_action:
		DEBUG_LOG("NPC TAKE ACTION");
		action = data;
		do_action(npc, action->action, action->direction);

		g_slice_free1(sizeof *action, data);
		break;
	case NPC_CMD_walk:
		DEBUG_LOG("NPC WALK");
		walk = data;
		do_walk(npc, walk->x, walk->y, 0);

		g_slice_free1(sizeof *walk, data);
		break;
	case NPC_CMD_talk:
		DEBUG_LOG("NPC TALK");
		talk = data;
		do_talk(npc, talk->msg, talk->msglen, 0);

		g_slice_free1(NPC_CMD_talk_msg_size, data);
		break;
	case NPC_CMD_destroy_npc:
		DEBUG_LOG("NPC DESTROY");
		free_npc(npc);

		g_slice_free1(sizeof(uint32_t), data);
		break;
	default:
		ERROR_LOG("Unsupported NPC CMD %u", *((uint32_t*)data));
		break;
	}

	return 0;
}

sprite_t* new_npc(const char* nick, uint32_t color, const uint32_t dress[], uint8_t dress_cnt)
{
	if (dress_cnt <= MAX_ITEMS_WITH_BODY) {
		int i = 0;
		sprite_t* p;
		for ( ; i != NPC_NUMBER_MAX; ++i ) {
			p = &(npc_array[i]);
			if (p->id == 0) {
				INIT_LIST_HEAD(&p->map_list);
				INIT_LIST_HEAD(&p->hash_list);
				INIT_LIST_HEAD(&p->timer_list);

				p->login = 1;
				p->id    = i + NPC_BASE_ID;
				p->color = color;
				strncpy(p->nick, nick, sizeof p->nick);
				p->item_cnt = dress_cnt;
				memcpy(p->items, dress, (sizeof *dress) * dress_cnt);

				++npc_count;
				//DEBUG_LOG("NPC %u Created, %u NPC in Total", p->id, npc_count);
				return p;
			}
		}
	}

	ERROR_LOG("Failed to Create NPC: nick=%.16s color=%u dresscnt=%u", nick, color, dress_cnt);
	return 0;
}

void free_npc(sprite_t* npc)
{
	leave_map(npc, 0);
	//DEBUG_LOG("NPC %u Freed, %u NPC Remain", npc->id, npc_count - 1);
	memset(npc, 0, sizeof *npc);
	--npc_count;
}

sprite_t* get_npc(uint32_t id)
{
	assert( IS_NPC_ID(id) );
	if (id >= NPC_BASE_ID + NPC_NUMBER_MAX || id < NPC_BASE_ID)
		return 0;
	sprite_t* p = &npc_array[(id - NPC_BASE_ID)];
	return ((p->login) ? p : 0);
}

int init_npc_system(const char *file)
{
	npc_count = 0;
	return 0;
}

void destroy_npc_system()
{
	npc_count = 0;
}

int create_npc_op(sprite_t* initor, uint32_t rcverid, const void* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 23);

typedef struct NpcBaseInfo {
	char      nick[16];
	uint32_t  color;
	uint8_t   dress_cnt;
	uint32_t  dress[];
} __attribute__ ((packed)) npc_base_info_t;

	DEBUG_LOG("NPC BODYLEN %d", len);

	const npc_base_info_t* npc_info = buf;
	sprite_t* npc = new_npc(npc_info->nick, npc_info->color, npc_info->dress, npc_info->dress_cnt);

	size_t offset = sizeof(npc_base_info_t) + (npc_info->dress_cnt * 4);
	const void* cmdbuf = ((uint8_t*)buf) + offset;
	parse_npc_cmd(npc, cmdbuf);

	return 0;
}

static void parse_npc_cmd(sprite_t* npc, const uint8_t* cmdbuf)
{
	uint32_t* pCmdtype;
	enter_map_cmd_t*    enter_map;
	take_action_cmd_t*  action;
	walk_cmd_t*         walk;
	talk_cmd_t*         talk;
	xform_others_cmd_t* xform;
	time_t exptm = get_now_tv()->tv_sec;

	DEBUG_LOG("TIME NOW %ld %s", exptm, ctime(&exptm));

	int j = 0;
	uint16_t i = 0, cmdcnt;
	UNPKG_H_UINT16(cmdbuf, cmdcnt, j);
	DEBUG_LOG("CMD COUNT IS %d", cmdcnt);

	uint32_t cmdtype;
	time_t   delay;
	for ( ; i != cmdcnt; ++i ) {
		UNPKG_H_UINT32(cmdbuf, cmdtype, j);
		UNPKG_H_UINT32(cmdbuf, delay, j);

		if (delay >= 0) {
			exptm += delay;
		} else {
			exptm += 10;
		}

		DEBUG_LOG("CMDTYPE %u DELAY %ld EXPTIME %ld", cmdtype, delay, exptm);

		switch (cmdtype) {
		case NPC_CMD_enter_map:
			enter_map = g_slice_alloc(sizeof *enter_map);

			enter_map->cmdtype = cmdtype;
			UNPKG_H_UINT32(cmdbuf, enter_map->oldmap, j);
			UNPKG_H_UINT32(cmdbuf, enter_map->newmap, j);

			ADD_TIMER_EVENT(npc, proc_npc_cmd, enter_map, exptm);
			break;
		case NPC_CMD_leave_map:
			pCmdtype  = g_slice_new(uint32_t);

			*pCmdtype = cmdtype;

			ADD_TIMER_EVENT(npc, proc_npc_cmd, pCmdtype, exptm);
			break;
		case NPC_CMD_take_action:
			action = g_slice_alloc(sizeof *action);

			action->cmdtype = cmdtype;
			UNPKG_H_UINT32(cmdbuf, action->action,j);
			UNPKG_H_UINT8(cmdbuf, action->direction, j);

			ADD_TIMER_EVENT(npc, proc_npc_cmd, action, exptm);
			break;
		case NPC_CMD_walk:
			walk = g_slice_alloc(sizeof *walk);

			walk->cmdtype = cmdtype;
			UNPKG_H_UINT16(cmdbuf, walk->x, j);
			UNPKG_H_UINT16(cmdbuf, walk->y, j);

			ADD_TIMER_EVENT(npc, proc_npc_cmd, walk, exptm);
			break;
		case NPC_CMD_talk:
			talk = g_slice_alloc(NPC_CMD_talk_msg_size);

			talk->cmdtype = cmdtype;
			UNPKG_H_UINT16(cmdbuf, talk->msglen, j);
			if (talk->msglen > (NPC_CMD_talk_msg_size - 20)) {
				ERROR_LOG("Talk Msg Too Long %d", talk->msglen);
				return;
			}
			UNPKG_STR(cmdbuf, talk->msg, j, talk->msglen);

			ADD_TIMER_EVENT(npc, proc_npc_cmd, talk, exptm);
			break;
//-----------------------------------------------
		case NPC_CMD_xform_others:
			xform  = (void*)(cmdbuf + j);
			j     += sizeof *xform;
			xform_users_at_map(xform->mapid, xform->itmid, xform->expiretm);
			break;
//-----------------------------------------------
		case NPC_CMD_destroy_npc:
			pCmdtype  = g_slice_new(uint32_t);

			*pCmdtype = cmdtype;

			ADD_TIMER_EVENT(npc, proc_npc_cmd, pCmdtype, exptm);
			//DEBUG_LOG("DESTROY NPC %u AT %s", npc->id, ctime(&exptm));
			break;
		default:
			ERROR_LOG("Unsupported NPC CMD %u", cmdtype);
			return;
		}
	}
}

#ifdef LUA_TEST

void lua_test()
{
	lua_State *L;
	int sum = 0;

	L = lua_open();
	luaL_openlibs(L);
	//luaopen_pack(L);

	if ( luaL_dofile(L, "./conf/npctest.lua") != 0 ) {
		DEBUG_LOG("LUA DO FILE ERROR");
		return;
	}

	lua_getglobal(L, "add"); //function name

	lua_pushnumber(L, 1);
	lua_pushnumber(L, 2);

	lua_call(L, 2, 1);

	sum = (int)lua_tonumber(L, -1); //stack top
	lua_pop(L, 1);

	DEBUG_LOG("lua test add result is %d", sum);

	lua_close(L);
}

void create_npc_test()
{
typedef struct NpcBaseInfo {
	char      nick[16];
	uint32_t  color;
	uint8_t   dress_cnt;
	uint32_t  dress[];
} __attribute__ ((packed)) npc_base_info_t;


	L = lua_open();
	luaL_openlibs(L);

	lua_register(L, "lua_wrap_enter_map", lua_wrap_enter_map);
	lua_register(L, "lua_wrap_leave_map", lua_wrap_leave_map);
	lua_register(L, "lua_wrap_talk", lua_wrap_talk);

	if ( luaL_dofile(L, "/home/kal/project/online/conf/npctest.lua") != 0 ) {
		DEBUG_LOG("LUA DO FILE ERROR");
		return;
	}
	DEBUG_LOG("LUA DO FILE SUCCESS");

	lua_getglobal(L, "get_npc"); //function name

	lua_call(L, 0, 1);

	if ( lua_isstring(L, -1) ) {
		DEBUG_LOG("IS STRING");
	} else if ( lua_isnumber(L, -1) ) {
		DEBUG_LOG("IS NUMBER");
	} else {
		DEBUG_LOG("WHAT TYPE?");
	}
	char* buf = (char*)lua_tostring(L, -1); //stack top
	uint32_t buf_len = *(uint32_t*)buf;
	npc_base_info_t* p_npc = (npc_base_info_t*)(buf + 4);
	DEBUG_LOG("BUF LEN IS %u, NICK IS %s, COLOR IS %u, DRESS CNT IS %d", buf_len,
		p_npc->nick, p_npc->color, (int)p_npc->dress_cnt);

	//create_npc_op(NULL, 0, buf+4, buf_len);


	lua_pop(L, 1);

	lua_close(L);
}


int lua_wrap_enter_map(lua_State *L)
{
	int n = lua_gettop(L); //param count
	DEBUG_LOG("STACK TOP HAS %d PARAM COUNT", n);

	uint32_t id = lua_tonumber(L,1);
	sprite_t *p = get_sprite(id);
	uint32_t newmap = lua_tonumber(L,2);
	uint32_t oldmap = lua_tonumber(L,3);
	DEBUG_LOG("PARAM IS %u, %u %u", id, newmap, oldmap);

	int ret = do_enter_map(p, newmap, oldmap,0,0);
	DEBUG_LOG("ENTER MAP RESULT IS %d", ret);

	return 0;
}

int lua_wrap_leave_map(lua_State *L)
{
	int n = lua_gettop(L); //param count
	DEBUG_LOG("STACK TOP HAS %d PARAM COUNT", n);

	uint32_t id = lua_tonumber(L,1);
	sprite_t *p = get_sprite(id);

	int ret = do_leave_map(p);
	DEBUG_LOG("LEAVE MAP RESULT IS %d", ret);

	return 0;
}

int lua_wrap_talk(lua_State *L)
{
	int n = lua_gettop(L); //param count
	DEBUG_LOG("STACK TOP HAS %d PARAM COUNT", n);

	uint32_t id = lua_tonumber(L,1);
	sprite_t *p = get_sprite(id);
	char *str = (char*)lua_tostring(L,2);
	int len = lua_tonumber(L,3);
	uint32_t recverid = lua_tonumber(L,4);
	DEBUG_LOG("%u, %s, %d, %d", p->id, str, len, recverid);

	int ret = do_talk(p, str, len, recverid);
	DEBUG_LOG("TALK RESULT IS %d", ret);

	return 0;
}

#endif
