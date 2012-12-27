#ifndef _PET_ATTRIBUTE_H_
#define _PET_ATTRIBUTE_H_

extern "C" {
#include <stdint.h>
}

class Player;

uint32_t  calc_pet_recover_hp_attr(Player* p);
uint32_t  calc_pet_recover_mp_attr(Player* p);

float     calc_pet_def_rate_attr(Player* p);

float     calc_pet_crit_rate_attr(Player* p);

float     calc_pet_dodge_rate_attr(Player* p);

float     calc_pet_hit_rate_attr(Player* p);


#endif
