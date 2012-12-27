extern "C" {
#include <dirent.h>
}

#include <string>


#include "AI_interface.hpp"
#include "MonsterAI.hpp"
#include "HighLvAI.hpp"
#include "LuaAI.hpp"
#include "lua_engine.hpp"
#include "../player.hpp"

using namespace std;
using namespace luabind;

LuaEngine g_lua_engine("ai/lua");

static void register_monster_ai_func(int mon_type, int ai_func_idx, const char* func_name)
{
	g_lua_engine.register_monster_ai_func(mon_type, ai_func_idx, func_name);
}

void LuaEngine::
export_core_interfaces_to_lua()
{
	module(m_lua)
	[
		def("register_monster_ai_func", &::register_monster_ai_func),

		class_<AIInterface>("AIInterface")
			.def("set_event_tm", &AIInterface::set_event_tm)
			.def("check_event_tm", &AIInterface::check_event_tm)
			.def("change_state", &AIInterface::change_state)
			.def_readwrite("flag", &AIInterface::common_flag_)
			.def_readwrite("flag1", &AIInterface::common_flag1_)
			.def_readwrite("flag2", &AIInterface::common_flag2_)
			.def_readwrite("flag3", &AIInterface::common_flag3_)
			.def_readwrite("ready_skill_id", &AIInterface::ready_skill_id_),

		class_<MonsterAI>("MonsterAI")
			.def("monster_attack", &MonsterAI::monster_attack),

		class_<player_skill_t>("player_skill_t")
			.def_readonly("skill_id", &player_skill_t::skill_id),

		class_<Player>("Player")
			.def("select_super_armor_skill", &Player::select_super_armor_skill)
			.def_readwrite("i_ai", &Player::i_ai)
			.def_readwrite("super_armor", &Player::super_armor)
	];
}

void LuaEngine::
load_scripts()
{
	// load all lua scripts under directory 'm_dir'
	/*
	DIR* dir = opendir("ai/lua");
	dirent* dentry;
	while ((dentry = readdir(dir))) {
		string filename(dentry->d_name);
		if ((filename != ".") && (filename != "..")) {

			if( filename.find(".lua") == string::npos){
				continue;
			}

			string pathname = m_dir + "/" + filename;
			if (luaL_loadfile(m_lua, pathname.c_str()) != 0) {
				std::cout<<"loadfile error[file:"<<pathname.c_str()<<"]: "<<lua_tostring(m_lua, -1)<<endl;
			}
			if (lua_pcall(m_lua, 0, LUA_MULTRET, 0) != 0) {
				std::cout<<"pcall error[file:"<<pathname.c_str()<<"]: "<<lua_tostring(m_lua, -1)<<endl;
			}
		}
	}*/
	//all lua file include in gongfu_ai.lua you should only load this file
	string pathname = "./ai/lua/gongfu_ai.lua";
	if( luaL_loadfile(m_lua, pathname.c_str()) != 0){
		std::cout<<"loadfile error[file:"<<pathname.c_str()<<"]: "<<lua_tostring(m_lua, -1)<<endl;
	}
	if (lua_pcall(m_lua, 0, LUA_MULTRET, 0) != 0) {
		std::cout<<"pcall error[file:"<<pathname.c_str()<<"]: "<<lua_tostring(m_lua, -1)<<endl;
	}

}

int pcall_callback_err_fun(lua_State* L) 
{
	lua_Debug debug;
	uint32_t level = 0;
	while (lua_getstack(L, level, &debug)) {
		level++;
	}

	if (!level) {
		return 0;
	}

	lua_getstack(L, level - 1, &debug);
	lua_getinfo(L, "Sln", &debug);

	std::string err = lua_tostring(L, -1);
	lua_pop(L, 1);
	std::stringstream msg;
	msg << debug.short_src << ":line " << debug.currentline;
	if (debug.name != 0) {
		msg << "(" << debug.namewhat << " " << debug.name << ")";
	}

	msg << " [" << err << "]";
	lua_pushstring(L, msg.str().c_str());
	return 1;
}

