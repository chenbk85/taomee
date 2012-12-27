#include "home.hpp"

static home_exp2level g_home_exp2level[MAX_HOME_LEVEL];

bool init_home_exp2level_data()
{
	add_home_exp2level_data(1,  0,      100);
	add_home_exp2level_data(2,  346,    200);
	add_home_exp2level_data(3,  991,    300);
	add_home_exp2level_data(4,  2288,   400);
	add_home_exp2level_data(5,  4730,   500);
	add_home_exp2level_data(6,  9260,   600);
	add_home_exp2level_data(7,  17565,  700);
	add_home_exp2level_data(8,  32640,  800);
	add_home_exp2level_data(9,  59776,  900);
	add_home_exp2level_data(10, 108264, 1000);
	return true;
}

void add_home_exp2level_data(uint32_t level, uint32_t exp, uint32_t fumo_point)
{
	if(level >= 1 && level <= MAX_HOME_LEVEL)
	{
		g_home_exp2level[level - 1].exp = exp;
		g_home_exp2level[level - 1].level = level;
		g_home_exp2level[level - 1].fumo_point = fumo_point;
	//	g_home_exp2level[level - 1].action_point = action_point;
	}
}

home_exp2level* get_home_exp2level_data(uint32_t level)
{
	if(level >= 1 && level <= MAX_HOME_LEVEL)
	{
		home_exp2level* p_data =  &g_home_exp2level[level - 1];
		if(p_data->level  != level)return 0;
		return p_data;
	}
	return 0;
}
