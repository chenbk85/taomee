/********************************************************
 *
 * @auth saga
 *
 * *****************************************************/
#ifndef __PLAYER_ATTRIBUTE_H__
#define __PLAYER_ATTRIBUTE_H__

#include "player.hpp"

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

uint32_t get_base_hp(player_t *p);
uint32_t get_base_atk(player_t *p);

void calc_player_attr_ex(player_t *p);

#endif
