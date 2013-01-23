#ifndef _BATTLE_STATISTICS_DATA_HPP_
#define _BATTLE_STATISTICS_DATA_HPP_

#include <cstdio>

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <libtaomee++/random/random.hpp>

extern "C"
{
#include <glib.h>
#include <assert.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee++/random/random.hpp>
}


#include "battle_round.hpp"
#include "battle_impl.hpp"
#include "battle_manager.hpp"
#include "player.hpp"
#include "pea_common.hpp"

using namespace std;
using namespace taomee;

typedef struct target_exp_data
{
	target_exp_data()
	{
		memset(this, 0, sizeof(*this));	
	}	
	
	void calc_pvp_target_exp(uint32_t atker_lv, uint32_t target_lv)
	{
		uint32_t target_exp = target_lv * target_lv	/3 + 25;
		uint32_t cmp_exp    = (atker_lv  * atker_lv /3 + 25)*1.1;
		exp = PEA_MIN(target_exp, cmp_exp);
	}

	uint32_t target_id;
	uint32_t damage;
	uint32_t exp;
}target_exp_data;












#endif
