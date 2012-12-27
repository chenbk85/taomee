#ifndef _TITLE_ATTRIBUTE_HPP_
#define _TITLE_ATTRIBUTE_HPP_

#include <kf/title_attr.hpp>

#include "player.hpp"

title_attr_data_mgr* get_title_attr_data_mgr();

/*
#include <map>

using namespace std;

typedef struct title_attr_data
{
	title_attr_data()
	{
		id = 0;
        strength = 0;
        agility  = 0;
        body     = 0;
        //stamina  = 0;
        hit      = 0;
        dodge    = 0;
        atk      = 0;
        def      = 0;
        hp       = 0;
        mp       = 0;
	}
	uint32_t id;
	uint32_t strength;
	uint32_t agility;
	uint32_t body;
	//uint32_t stamina;
	uint32_t hit;
	uint32_t dodge;
	uint32_t atk;
	uint32_t def;
    uint32_t hp;
    uint32_t mp;
}title_attr_data;

class title_attr_data_mgr
{
public:
    title_attr_data_mgr(){}
    ~title_attr_data_mgr(){}
public:
	bool init(const char* xml);
	bool final();

    bool is_title_id_exist(uint32_t id);
    bool add_title_attr_data(title_attr_data* p_data);

    title_attr_data * get_title_attr_data(uint32_t id);
private:
	std::map<uint32_t, title_attr_data*> datas;
};
*/

uint32_t calc_title_strength_attr(Player* p);
uint32_t calc_title_agility_attr(Player* p);
uint32_t calc_title_body_quality_attr(Player* p);
uint32_t calc_title_stamina_attr(Player* p);

uint32_t calc_title_recover_hp_attr(Player* p);

uint32_t calc_title_recover_mp_attr(Player* p);

uint32_t calc_title_def_value_attr(Player* p);
float calc_title_crit_attr(Player* p);
float calc_title_hit_attr(Player* p);

uint32_t calc_title_hp_attr(Player* p);
uint32_t calc_title_mp_attr(Player* p);
uint32_t calc_title_atk_attr(Player* p);
float calc_title_dodge_attr(Player* p);

#endif
