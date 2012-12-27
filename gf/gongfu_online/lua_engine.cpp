#include"fwd_decl.hpp"
extern "C" {

#include <glib.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <async_serv/service.h>
#include <async_serv/net_if.h>

}
#include "global_data.hpp"
#include "lua_engine.hpp"
#include "player.hpp"
#include "kill_boss.hpp"
#include "fight_team.hpp"
//#include "achievement_logic.hpp"
#include "achievement.hpp"

using namespace taomee;
using namespace std;


void runner_stack_clear(lua_State * l)
{
	if (l) {
		lua_settop(l, -1);
	}
}

int l_check_pass_pve_stage(lua_State * l);
int l_get_event_flag(lua_State * l);
int l_get_event_id(lua_State * l);
int l_player_gain_achieve(lua_State * l);
int l_get_player_stat_info(lua_State * l);
int l_add_player_stat_cnt(lua_State * l);
int l_player_summon_cnt(lua_State * l);
int l_player_super_summon_cnt(lua_State * l);
int l_player_team_coin_donate(lua_State * l);
int l_player_team_exp_donate(lua_State * l);
int l_get_player_team_lv(lua_State *l);
int l_get_player_clothes_quality(lua_State * l);
int l_get_clothes_equippart(lua_State *l);
int l_get_player_clothes_cnt(lua_State *l);

int l_player_has_finish_task(lua_State *l);


bool init_lua()
{
	s_lua = lua_open();
    luaopen_base(s_lua);
//    luabind::open(s_lua);
	return true;	
}
bool final_lua( )
{
	lua_close(s_lua);
	return true;
}

void regist_to_lua( )
{
	lua_pushcfunction(s_lua, l_check_pass_pve_stage);
	lua_setglobal(s_lua, "is_player_pass_pve_stage");

	lua_pushcfunction(s_lua, l_get_event_flag);
	lua_setglobal(s_lua, "event_flag");

	lua_pushcfunction(s_lua, l_get_event_id);
	lua_setglobal(s_lua, "event_type");

	lua_pushcfunction(s_lua, l_player_gain_achieve);
	lua_setglobal(s_lua, "player_gain_achieve");

	lua_pushcfunction(s_lua, l_get_player_stat_info);
	lua_setglobal(s_lua, "get_player_stat");
	
	lua_pushcfunction(s_lua, l_add_player_stat_cnt);
	lua_setglobal(s_lua, "add_player_stat");

	lua_pushcfunction(s_lua, l_get_player_team_lv);
	lua_setglobal(s_lua, "player_team_lv");

	lua_pushcfunction(s_lua, l_player_team_coin_donate);
	lua_setglobal(s_lua, "player_team_t_coin");

	lua_pushcfunction(s_lua, l_player_team_exp_donate);
	lua_setglobal(s_lua, "player_team_t_exp");

	lua_pushcfunction(s_lua, l_player_summon_cnt);
	lua_setglobal(s_lua, "player_summon_cnt");

	lua_pushcfunction(s_lua, l_player_super_summon_cnt);
	lua_setglobal(s_lua, "player_super_summon_cnt");
	
	lua_pushcfunction(s_lua, l_player_has_finish_task);
	lua_setglobal(s_lua, "player_has_finish_task");

	lua_pushcfunction(s_lua, l_get_player_clothes_cnt);
	lua_setglobal(s_lua, "player_clothes_cnt");

	lua_pushcfunction(s_lua, l_get_player_clothes_quality);
	lua_setglobal(s_lua, "player_clothes_quality"); 

	lua_pushcfunction(s_lua, l_get_clothes_equippart);
	lua_setglobal(s_lua, "clothes_equippart");

}

bool load_lua_file(const char* lua_file)
{
	if( luaL_dofile(s_lua, lua_file) != 0){
		printf("--------------load lua fail-----------------------\n");
		return false;
	}

	luaL_openlibs(s_lua);

	printf("--------------load lua sucess-----------------------\n");
	return true;
};

int l_check_pass_pve_stage(lua_State * l) 
{
	player_t * p = NULL;
	int stage = 0;
	int diff = 0;

	diff = lua_tointeger(l, -1);
	lua_remove(l, -1);
	stage = lua_tointeger(l, -1);
	lua_remove(l, -1);

	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
	} else {
		return 0;
	}
	
	runner_stack_clear(l);
	lua_pushboolean(l, is_player_has_stage_fumo_info(p, stage, diff)); 
	return 1;
}

int get_event_flag(achieve_event * event, int flag) {
	switch (flag) {
		case 1:
			return event->flag1_;
		case 2:
			return event->flag2_;
		case 3:
			return event->flag3_;
		case 4:
			return event->flag4_;
		default:
			return 0;
	}
}

int l_get_event_flag(lua_State * l)
{
	achieve_event * event = NULL;
	int	flag_id = 0;
	flag_id = lua_tointeger(l, -1);
	lua_remove(l, -1);
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		event = reinterpret_cast<achieve_event*>(lua_touserdata(l, -1));
	}
	runner_stack_clear(l);
	lua_pushinteger(l, get_event_flag(event, flag_id));
	return 1;
}


int l_get_event_id(lua_State * l)
{
	achieve_event * event = NULL;
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		event = reinterpret_cast<achieve_event*>(lua_touserdata(l, -1));
	}
	runner_stack_clear(l);
	lua_pushinteger(l, event->event_id);
	return 1;
}

int l_player_gain_achieve(lua_State * l)
{
	uint32_t achieve_id = 0;
	player_t * p = NULL;
	achieve_id = lua_tointeger(l, -1);
	lua_remove(l, -1);

	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
	}
	runner_stack_clear(l);
	player_gain_achieve(p, achieve_id);
	return 0;
}

int lua_achieve_event_handle(player_t * p, achieve_event * event) {
	lua_getglobal(s_lua, "handle_achieve_event");
	lua_pushlightuserdata(s_lua, p);
	lua_pushlightuserdata(s_lua, event);
	int val = lua_pcall(s_lua, 2, 0, 0);
	if (val) {
		ERROR_LOG("Player %u achieve_event [%u %u %u %u %u] error", p->id,
				event->event_id, event->flag1_, event->flag2_, event->flag3_, 
				event->flag4_);	   
	}
	return 0;
}


int l_get_player_stat_info(lua_State * l)
{
	int stat_id = 0;
	player_t * p = NULL;
	stat_id = lua_tointeger(l, -1);
	lua_remove(l, -1);
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
		runner_stack_clear(l);
		lua_pushinteger(l, get_player_stat_info(p, stat_id));
	}
	return 1;
}

int l_add_player_stat_cnt(lua_State * l)
{
	int add_cnt = 0;
	int stat_id  = 0;
	player_t * p = NULL;

	add_cnt = lua_tointeger(l, -1);
	lua_remove(l, -1);
	stat_id = lua_tointeger(l, -1);
	lua_remove(l, -1);
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
		runner_stack_clear(l);
		add_player_stat_cnt(p, stat_id, add_cnt);
	}
	return 0;
}

int l_player_summon_cnt(lua_State * l)
{
	player_t * p = NULL;
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
		lua_remove(l, -1); 
		lua_pushinteger(l, p->summon_mon_num);
	}
	return 1;
}

int l_player_super_summon_cnt(lua_State * l)
{
	player_t * p = NULL;
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
		lua_remove(l, -1); 
		int super_cnt = 0;
		for (uint32_t i = 0; i < p->summon_mon_num; i++) {
			if (p->summons[i].mon_type % 10 == 6) {
				super_cnt ++;
			}
		}
		lua_pushinteger(l, super_cnt);
	}
	return 1;
}

int l_player_team_coin_donate(lua_State * l)
{
	player_t * p = NULL;
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
		lua_remove(l, -1); 
		lua_pushinteger(l, get_player_team_coin_donate(p));
	}
	return 1;
}

int l_player_team_exp_donate(lua_State * l)
{
	player_t * p = NULL;
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
		lua_remove(l, -1); 
		lua_pushinteger(l, get_player_team_exp_donate(p));
	}
	return 1;
}

int l_get_player_team_lv(lua_State *l)
{
	player_t * p = NULL;
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
		lua_remove(l, -1); 
		lua_pushinteger(l, p->team_info.team_lv);
	}
	return 1;
}

int l_get_player_clothes_quality(lua_State * l)
{
	player_t * p = NULL;
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
		lua_remove(l, -1); 
		int low_quality  = 0;
		for (uint32_t i = 0; i < p->clothes_num; ++i) {
			player_clothes_info_t * clothes = &(p->clothes[i]);
			const GfItem * item = items->get_item(clothes->clothes_id);
			if (low_quality == 0) {
				low_quality = item->quality_lv;
			}else if (item->quality_lv < low_quality) {
				low_quality = item->quality_lv;
			}
		}
		lua_pushinteger(l, low_quality);
	}
	return 1;
}

int l_get_player_clothes_cnt(lua_State * l)
{
	player_t * p = NULL;
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
		lua_remove(l, -1); 
		lua_pushinteger(l, p->clothes_num);
	}
	return 1;
}

int l_player_has_finish_task(lua_State *l)
{
	player_t * p = NULL;
	int task_id = 0;
	task_id = lua_tointeger(l, -1);
	lua_remove(l, -1);
	if (lua_type(l, -1) == LUA_TLIGHTUSERDATA) {
		p = reinterpret_cast<player_t*>(lua_touserdata(l, -1));
		lua_remove(l, -1); 
		lua_pushboolean(l, is_finished_task(p, task_id)); 
	}
	return 1;
}


int l_get_clothes_equippart(lua_State *l)
{
	int clothes_id = lua_tointeger(l, -1);
	lua_remove(l, -1);
	const GfItem * item = items->get_item(clothes_id);
	if (item) {
		lua_pushinteger(l, item->equip_part());
	}
	return 1;
}






