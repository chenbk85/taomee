/**
 *============================================================
 *  @file      BossLuaAI.cpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */
 
extern "C" {
#include <libtaomee/log.h>
}

#include <libtaomee++/random/random.hpp>
#include <libtaomee++/inet/byteswap.hpp>

#include "MonsterAI.hpp"
#include "HighLvAI.hpp"
#include "LuaAI.hpp"
#include "lua_engine.hpp"
#include "../player.hpp"
#include "../stage.hpp"

using namespace taomee;
using namespace luabind;


/****************************************************************
 * Boss
 ****************************************************************/
bool
BossLuaAI::extra_wait_action(Player * player)
{
	const AIFuncNames* m_func_names = player->i_ai->m_func_names;
	if (m_func_names && m_func_names->func_names[monster_wait_func_idx].size()) {
		try {
			bool ret = call_function<bool>(g_lua_engine.m_lua, m_func_names->func_names[monster_wait_func_idx].c_str(), player);
			return ret;
		} catch(luabind::error& e) {
			ERROR_LOG("lua call error: %s", lua_tostring(g_lua_engine.m_lua, -1));
			return false;
		}
	}
	return false;
}

bool
BossLuaAI::extra_attack_action(Player * player)
{
	
	return false;
}

bool
BossLuaAI::extra_stuck_action(Player * player)
{
	const AIFuncNames* m_func_names = player->i_ai->m_func_names;
	if (m_func_names && m_func_names->func_names[monster_stuck_func_idx].size()) {
		try {
			bool ret = call_function<bool>(g_lua_engine.m_lua, m_func_names->func_names[monster_stuck_func_idx].c_str(), 
					player, reinterpret_cast<MonsterAI*>(this));
			return ret;

		} catch(luabind::error& e) {
			//if any error when lua_pcall, the function will be removed from the stack. so lua_Debug is meanless,becouse
			//debug information cannot be gathered after the return of lua_pcall
			ERROR_LOG("lua call error: %s", lua_tostring(g_lua_engine.m_lua, -1));
			//printf(lua_tostring(g_lua_engine.m_lua, -1));
			return false;
		}
		//return call_function<bool>(g_lua_engine.m_lua, m_func_names->func_names[monster_stuck_func_idx].c_str(), player, this);
	}
	
	return false;
}

