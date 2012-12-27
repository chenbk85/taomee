#ifndef _PLAYER_ATTRIBUTE_H
#define _PLAYER_ATTRIBUTE_H

#include "player.hpp"

uint32_t calc_player_attr_by_suit(Player* p);


uint32_t  calc_player_strength_attr(Player* p);
uint32_t  calc_player_agility_attr(Player* p);
uint32_t  calc_player_body_quality_attr(Player* p);
uint32_t  calc_player_stamina_attr(Player* p);
uint32_t  calc_player_hp_attr(Player* p);
uint32_t  calc_player_mp_attr(Player* p);
uint32_t  calc_player_recover_hp_attr(Player* p);
uint32_t  calc_player_recover_mp_attr(Player* p);
uint32_t  calc_player_atk_attr(Player* p);
uint32_t  calc_player_def_value_attr(Player* p);
float     calc_player_def_rate_attr(Player* p);
float     calc_player_crit_attr(Player* p);
float     calc_player_dogde_attr(Player* p);
float     calc_player_hit_attr(Player* p);

uint32_t calc_player_base_mp_ex(Player * p);



//-------------------NEW calculate split line --------------------------------------//
struct base_attr_t {
    uint32_t    agility;
    uint32_t    strength;
    uint32_t    body;
    uint32_t    stamina;
};

struct btl_attr_t {
    uint32_t    maxhp;
    uint32_t    maxmp;
    uint32_t    addhp;
    uint32_t    addmp;
    uint32_t    atk;
    uint32_t    def;
    uint32_t    hit;
    uint32_t    dodge;
    uint32_t    crit;
};

struct add_attr_t {
    uint32_t    addhp;
    uint32_t    addmp;
};
/*
struct weared_suit_t {
    uint32_t    suit_id;
    uint32_t    suit_cnt;
};*/

void calc_player_attr_ex2(Player *p);











#endif
