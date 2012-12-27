/*
 * =====================================================================================
 *
 *       Filename:  fight_team.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/11/2012 02:41:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#include "fight_team.hpp"

int calc_fight_team_lv_by_exp(uint32_t fight_exp)
{
	if (fight_exp < 60000) {
		return 1;
	} else if (fight_exp < 180000) {
		return 2;
	} else if (fight_exp < 420000) {
		return 3;
	} else if (fight_exp < 900000) {
		return 4;
	} else if (fight_exp < 1860000) {
		return 5;
	} else if (fight_exp < 3780000) {
		return 6;
	} else {
		return 7;
	}
}

int get_tax_coin_by_team_lv(uint32_t team_lv)
{
	static int tax_coin[7] = {
		50000, 100000, 200000, 500000, 1000000, 2000000, 5000000
	};

	if (team_lv > 0 && team_lv < 8) {
		return tax_coin[team_lv -1];
	}
	return 0;
}

void calc_player_team_attr_add_by_team_lv(fight_team_attr_add_t * attr_add_info, uint32_t lv)
{
	static uint32_t attr_info[7][4] = {
		{15, 30, 0, 0},
		{30, 45, 300, 500},
		{45, 70, 600, 1000},
		{65, 100, 1000, 1700},
		{85, 130, 1500, 2600},
		{110, 170, 2100, 3700},
		{135, 210, 2800, 5000}
	};
	if (lv >0 && lv <= 7) {
		attr_add_info->atk = attr_info[lv-1][0];
		attr_add_info->def_value = attr_info[lv-1][1];
		attr_add_info->mp = attr_info[lv-1][2];
		attr_add_info->hp = attr_info[lv-1][3];
	}
}
