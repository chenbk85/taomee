#include "player_skill.hpp"
#include "player.hpp"

bool init_player_skills(Player* p)
{
	p->skill_maps = new std::map<uint32_t, player_skill*>();
	p->cur_skill_lv_id = 0;
	p->default_skill_lv_id = 0;
	return true;	
}

bool final_player_skills(Player* p)
{
	delete p->skill_maps;
	p->skill_maps = NULL;
	p->cur_skill_lv_id = 0;
	p->default_skill_lv_id = 0;
	return true;
}

player_skill::player_skill(uint32_t skill_id, uint32_t skill_lv)
{
	p_data = skill_data_mgr::get_instance()->get_skill_data_by_id(skill_id, skill_lv);
	skill_cd = 0;
}

player_skill::~player_skill()
{
	p_data= NULL;
	skill_cd = 0;
}


bool player_skill::check_cd(uint32_t round_time)
{
	return skill_cd >= round_time;
}

void player_skill::set_cd(uint32_t round_time)
{
	skill_cd =round_time;
}

uint32_t player_skill::get_skill_id()
{
	return p_data->skill_id;	
}

player_skill* get_player_skill(Player* p, uint32_t skill_id)
{
	std::map<uint32_t, player_skill*>::iterator pItr = p->skill_maps->find(skill_id);
	if(pItr == p->skill_maps->end())return NULL;
	return pItr->second;
}

bool is_player_skill_exist(Player* p, uint32_t skill_id)
{
	std::map<uint32_t, player_skill*>::iterator pItr = p->skill_maps->find(skill_id);
	return pItr != p->skill_maps->end();
}

bool add_player_skill(Player* p, player_skill* skill)
{
	if(is_player_skill_exist(p, skill->get_skill_id()))return false;
	(*p->skill_maps)[skill->get_skill_id()] = skill;
	return true;
}

bool del_player_skill(Player* p, uint32_t skill_id)
{
	std::map<uint32_t, player_skill*>::iterator pItr = p->skill_maps->find(skill_id);
	if(pItr == p->skill_maps->end())return false;
	p->skill_maps->erase(pItr);
	return true;
}

