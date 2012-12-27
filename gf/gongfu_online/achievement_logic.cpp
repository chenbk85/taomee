#include "achievement_logic.hpp"
#include "achievement.hpp"
#include "kill_boss.hpp"
#include "global_data.hpp"
#include "lua_engine.hpp"
#include "app_log.hpp"
#include "secondary_profession.hpp"
#include "stat_log.hpp"
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

using namespace std;
//using namespace luabind;

bool check_battle_achieve_type_logic(player_t* p, uint32_t id)
{
	int ret = lua_check_achieve_logic(p, id);
	return ret? true : false;
}

bool check_yaoshi_achieve_type_logic(player_t* p, uint32_t id)
{
	//of course you can write c++ code to implement this function instead of calling lua function 
	secondary_profession_data* pdata = get_player_secondary_profession_by_type(p, apothecary_profession_type);
	if(pdata == NULL)return false;
	uint32_t level = pdata->get_profession_level();


	switch(id)
	{
		case 41:
		{
			return level == 1;	
		}
		break;

		case 42:
		{
			return level == 2;
		}
		break;

		case 43:
		{
			return level == 3;
		}
		break;

		case 44:
		{
			return level == 4;
		}
		break;

		case 45:
		{
			return level == 5;
		}	
		break;

		case 46:
		{
			return level == 7;
		}
		break;
	}	
	return false;
}

bool check_player_level_achieve_type_logic(player_t* p, uint32_t id)
{
switch(id)
{
	case 35:
	{
		return p->lv == 10;
	}
	break;

	case 36:
	{
		return p->lv == 20;
	}
	break;

	case 37:
	{
		return p->lv == 30;
	}
	break;

	case 38:
	{
		return p->lv == 40;
	}
	break;

	case 39:
	{
		return p->lv == 50;
	}
	break;

	case 40:
	{
		return p->lv == 60;
	}
	break;
}
return false;
}

bool do_achieve_type_logic(player_t* p, uint32_t id, bool broadcast)
{
	uint32_t get_time = time(NULL);
	uint32_t add_achieve_point = get_achievement_data_mgr()->get_add_achieve_point_by_id(id);

	GF_LOG_WRITE(LOG_ERROR, "achieve_module", "add_achieve type = %u, point = %u", id, add_achieve_point);
	p->add_achieve_point(add_achieve_point, get_time);
	if(broadcast)
	{
		broadcast_add_achievement(p, id);
	}
	set_player_achievement_data(p, id, get_time);
	db_replace_achievement_data(p, id, get_time, add_achieve_point);
	send_add_achievement_data(p, id, get_time, p->achieve_point);	

	achievement_data* pdata = get_achievement_data_mgr()->get_achievement_data_by_id(id);
	if(pdata && strlen(pdata->xiaoba_url_) > 0)
	{
		do_stat_log_add_achievement(p, get_time, pdata->name_, pdata->xiaoba_url_);
	}
	return true;
}


bool check_summon_achieve_type_logic(player_t* p, uint32_t id)
{
		int flag1 = 0;
		int flag2 = 0;
		if(
				(is_summon_role_type_exist(p, 1121) ||
				 is_summon_role_type_exist(p, 1122) ||
				 is_summon_role_type_exist(p, 1123) ||
				 is_summon_role_type_exist(p, 1124) ||
				 is_summon_role_type_exist(p, 1125) ||
				 is_summon_role_type_exist(p, 1126) ) &&

				( is_summon_role_type_exist(p, 1071) ||
				  is_summon_role_type_exist(p, 1072) ||
				  is_summon_role_type_exist(p, 1073) ||
				  is_summon_role_type_exist(p, 1074) ||
				  is_summon_role_type_exist(p, 1075) ||
				  is_summon_role_type_exist(p, 1076)) &&

				(is_summon_role_type_exist(p, 1051) ||
				 is_summon_role_type_exist(p, 1052) ||
				 is_summon_role_type_exist(p, 1053) ||
				 is_summon_role_type_exist(p, 1055) ||
				 is_summon_role_type_exist(p, 1055) ||
				 is_summon_role_type_exist(p, 1056))
		  )
		{
			flag1 = 1;
		}

		if(
				(is_summon_role_type_exist(p, 1041) ||
				 is_summon_role_type_exist(p, 1042) ||
				 is_summon_role_type_exist(p, 1043) ||
				 is_summon_role_type_exist(p, 1044) ||
				 is_summon_role_type_exist(p, 1045) ||
				 is_summon_role_type_exist(p, 1046)) &&

				( is_summon_role_type_exist(p, 1081) ||
				  is_summon_role_type_exist(p, 1082) ||
				  is_summon_role_type_exist(p, 1083) ||
				  is_summon_role_type_exist(p, 1084) ||
				  is_summon_role_type_exist(p, 1085) ||
				  is_summon_role_type_exist(p, 1086)) &&

				(is_summon_role_type_exist(p, 1061) ||
				 is_summon_role_type_exist(p, 1062) ||
				 is_summon_role_type_exist(p, 1063) ||
				 is_summon_role_type_exist(p, 1064) ||
				 is_summon_role_type_exist(p, 1065) ||
				 is_summon_role_type_exist(p, 1066))
		  )
		{
			flag2 = 1;
		}
	if(id == 303 && flag1 == 1)
	{
		return true;
	}
	if(id == 304 && flag2 == 1)
	{
		return true;
	}
	if(id == 305 && flag1 == 1 && flag2 == 1)
	{
		return true;
	}	 
	return false;
}


