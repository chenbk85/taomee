#ifndef _LUA_ENGINE_H_
#define _LUA_ENGINE_H_


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

bool init_lua();
bool final_lua();
void regist_to_lua();
bool load_lua_file(const char* lua_file);

struct achieve_event;
int lua_achieve_event_handle(player_t * p, achieve_event * event);
#endif
