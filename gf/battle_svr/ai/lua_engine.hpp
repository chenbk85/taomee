#ifndef LUA_ENGINE_HPP_
#define LUA_ENGINE_HPP_

#include <map>
#include <string>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <luabind/luabind.hpp>

enum MonsterAIFuncIdx {
	monster_wait_func_idx,
	monster_move_func_idx,
	monster_linger_func_idx,
	monster_evade_func_idx,
	monster_attack_func_idx,
	monster_stuck_func_idx,
	monster_defense_func_idx,
	
	monster_ai_count
};

struct AIFuncNames {
	std::string func_names[monster_ai_count];
};

int pcall_callback_err_fun(lua_State* L);

// Lua Engine
class LuaEngine {
public:
	LuaEngine(const char* dir);
	~LuaEngine()
		{ lua_close(m_lua); }

	void export_core_interfaces_to_lua();
	void load_scripts();

	void reload_lua_scripts()
		{ load_scripts(); }

	void register_monster_ai_func(int mon_type, int ai_func_idx, const char* func_name)
		{ m_ai_funcs[mon_type].func_names[ai_func_idx] = func_name; 
			std::cout<<"lua load mon = ["<<mon_type<<"]:["<<func_name<<"]"<<std::endl;}

	bool check_if_in_lua(int mon_type);
	
	const AIFuncNames* get_ai_func_names(int mon_type) const;

private:
	typedef std::map<int, AIFuncNames> AIFuncMap;

private:
	friend class BossLuaAI;

private:
	std::string m_dir;
	AIFuncMap m_ai_funcs;
	lua_State* m_lua;
};

inline LuaEngine::
LuaEngine(const char* dir) : m_dir(dir)
{
	// For demonstration only! In real world, you should alway check errors return from each function call
	m_lua = lua_open();
	luaopen_base(m_lua);
	luabind::open(m_lua);

	luabind::set_pcall_callback(pcall_callback_err_fun);
}

inline const AIFuncNames* LuaEngine::
get_ai_func_names(int mon_type) const
{
	AIFuncMap::const_iterator it = m_ai_funcs.find(mon_type);
	if (it != m_ai_funcs.end()) {
		return &(it->second);
	}

	return 0;
}

inline bool LuaEngine::
check_if_in_lua(int mon_type)
{
	AIFuncMap::const_iterator it = m_ai_funcs.find(mon_type);
	if (it != m_ai_funcs.end()) {
		return true;
	}

	return false;

}

extern LuaEngine g_lua_engine;

#endif // LUA_ENGINE_HPP_
