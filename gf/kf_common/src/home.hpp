#ifndef _HOME_H_
#define _HOME_H_

#include <map>
#include <stdint.h>

#define MAX_HOME_LEVEL 10

typedef struct home_exp2level
{
	home_exp2level()
	{
		exp = 0;
		level = 0;
		fumo_point = 0;
		//action_point = 0;
	}
	uint32_t exp;
	uint32_t level;
	uint32_t fumo_point;
//	uint32_t action_point;
}home_exp2level;

bool init_home_exp2level_data();

void add_home_exp2level_data(uint32_t level, uint32_t exp, uint32_t fumo_point);

home_exp2level* get_home_exp2level_data(uint32_t level);







#endif
