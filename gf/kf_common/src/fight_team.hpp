/*
 * =====================================================================================
 *
 *       Filename:  fight_team.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/01/2012 12:48:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#ifndef FIGHT_TEAM_HPP
#define FIGHT_TEAM_HPP
extern "C" {
#include <stdint.h>
}

int calc_fight_team_lv_by_exp(uint32_t fight_exp);

int get_tax_coin_by_team_lv(uint32_t team_lv);

struct fight_team_attr_add_t {
	uint32_t atk;
	uint32_t def_value;
	uint32_t mp;
	uint32_t hp;
	fight_team_attr_add_t()
	{
		atk = 0;
		def_value = 0;
		mp = 0;
		hp = 0;
	}
};

void calc_player_team_attr_add_by_team_lv(fight_team_attr_add_t * attr_add_info, uint32_t lv);

#endif


