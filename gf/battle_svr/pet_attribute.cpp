#include "pet_attribute.hpp"
#include "player.hpp"
#include <kf/pet_attr.hpp>

uint32_t  calc_pet_mp_attr(Player* p)
{
	return 100;
}

uint32_t  calc_pet_recover_hp_attr(Player* p)
{
	return 0;
}

uint32_t  calc_pet_recover_mp_attr(Player* p)
{
	return 0;
}

float     calc_pet_def_rate_attr(Player* p)
{
	float def_rate = float(p->def_value)/float(p->def_value + 70*p->lv + 280);
	return def_rate;
}

float     calc_pet_crit_rate_attr(Player* p)
{
	float crit_rate = float(p->crit_value) /  float((1000.0 *  p->lv / 23.0) + 4000.0/23.0) * 100;
	return crit_rate;
}

float     calc_pet_dodge_rate_attr(Player* p)
{
	float dodge_rate = float(p->dodge_value) / float( p->dodge_value + 70*p->lv + 280);
	return dodge_rate;
}


float     calc_pet_hit_rate_attr(Player* p)
{
	float hit_rate = float(p->hit_value)/ (2000.0 * p->lv / 19.0 + 8000.0/19.0) + 0.8;
	return hit_rate;
}









