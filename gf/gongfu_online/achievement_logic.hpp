#ifndef _ACHIEVEMENT_LOGIC_H_
#define _ACHIEVEMENT_LOGIC_H_


#include"fwd_decl.hpp"
#include <vector>
#include <map>
using namespace std;

struct player_t;

bool do_achieve_type_logic(player_t* p, uint32_t id, bool broadcast = false);

bool check_battle_achieve_type_logic(player_t* p, uint32_t id);

bool check_yaoshi_achieve_type_logic(player_t* p, uint32_t id);

bool check_player_level_achieve_type_logic(player_t* p, uint32_t id);

bool check_summon_achieve_type_logic(player_t* p, uint32_t id);

#endif
