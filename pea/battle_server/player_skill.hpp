#ifndef _PLAYER_SKILL_H_
#define _PLAYER_SKILL_H_

#include "pea_common.hpp"
#include "fwd_decl.hpp"
#include "object.hpp"
#include "skill.hpp"

class Player;

class player_skill
{
	public:    
		player_skill(uint32_t skill_id, uint32_t skill_lv);
		~player_skill();
	public:
		bool check_cd(uint32_t round_time);
		void set_cd(uint32_t round_time);
		uint32_t get_skill_id();
	public:
		skill_data* p_data;
		uint32_t    skill_cd;
};

bool init_player_skills(Player* p);

bool final_player_skills(Player* p);

player_skill* get_player_skill(Player* p, uint32_t skill_id);

bool is_player_skill_exist(uint32_t skill_id);

bool add_player_skill(Player* p, player_skill* skill);

bool del_player_skill(Player* p, uint32_t skill_id);










#endif
