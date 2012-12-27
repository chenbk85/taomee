/**
 * =====================================================================================
 *       @file  player.cpp
 *      @brief  玩家信息源文件
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  05/31/2010 11:22:53 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  tommychen
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include "player.hpp"
#include <libtaomee++/random/random.hpp>
#include <algorithm>

Cplayer::Cplayer():roll_point(0),grid_id(-1),num(0),rank(0),status(0),bout_cardid(0),life_value(6),grade(0),bout_result(0),itemid(0),bout_exp(0),cardid(0),bout_xiaomee(0),mole_level(0),high_card(0),game_status(0)
{

}

int Cplayer::roll()
{
	roll_point = taomee::ranged_random(1,6);
    return roll_point;
}

int Cplayer::get_roll_point()
{
    return roll_point;
}


/**
 * @brief 计算玩家等级
 * @param  player_cards_info_t *p_info
 * @return 返回等级0－9
 */
int Cplayer::calculate_rank(player_cards_info_t *p_info)
{
	if (p_info == NULL)
	{
		return 0;
	}

    int exp = p_info->exp;
    if (exp < new_rank1_exp) {
        return 0;
    } else if (exp < new_rank2_exp) {
        return 1;
    } else if (exp < new_rank3_exp) {
        return 2;
    } else if (exp < new_rank4_exp) {
        return 3;
    } else {
        return 4;
    }
}

int Cplayer::create_cards_seq( )
{
//	uint32_t userid = id();
//  srand(time(NULL) + userid);
    srand(time(NULL));
	int id = 0;
    for (uint32_t i = 0; i< p_cards_info->total_cnt; i++)
	{
        id = p_cards_info->card_id[i];
    	cards_id.push_back(id);
//		DEBUG_LOG("create_cards_seq: userid:%d cardid:%d", p->id, id);
    }

    random_shuffle(cards_id.begin(), cards_id.end());

    return 0;
}

int Cplayer::act_bout_card( int pos)
{
	bout_cardid = cards_id[pos];
	int tmpid = bout_cardid;
	cards_id[pos] = cards_id[5];
	cards_id.erase(cards_id.begin() + 5);
	cards_id.push_back(tmpid);

	return 0;

}

int Cplayer::update_life_dec(int value)
{
	life_value = life_value - value;
	return 0;

}

int Cplayer::update_life_inc(int value)
{
	life_value = life_value + value;
	return 0;

}

void Cplayer::set_bout_result( int result)
{
	bout_result = result;

}

void Cplayer::bout_clear()
{
	status = 0;
	bout_cardid = -1;
	bout_result = -1;

}

void Cplayer::set_grid_id(int id)
{
	grid_id = id;
//	DEBUG_LOG("Cplayer: user:%u, set_grid_id  grid_id:%d", p->id, grid_id);
}

int Cplayer::get_grid_id( )
{
//	DEBUG_LOG("Cplayer: user:%u, get_grid_id  grid_id:%d", p->id, grid_id);
	return 	grid_id;
}

int Cplayer::get_new_rank( )
{
    int exp = p_cards_info->exp + bout_exp;
    if (exp < new_rank1_exp) {
        return 0;
    } else if (exp < new_rank2_exp) {
        return 1;
    } else if (exp < new_rank3_exp) {
        return 2;
    } else if (exp < new_rank4_exp) {
        return 3;
    } else {
        return 4;
    }
}

bool Cplayer::is_up_rank( )
{
	int new_rank = get_new_rank( );
	if (new_rank > rank)
	{
		return true;
	}
	else
	{
		return false;
	}

}

uint32_t Cplayer::id()
{
	if (p != NULL)
	{
		return p->id;
	}
	else
	{
		return 0;
	}
}

int Cplayer::cal_mole_level()
{
	int exp = p_cards_info->mole_exp;
	int lv = 1;
	int n = 0;
	int base_exp = 0;
	int js = 108900;

	if (exp < 0)
	{
		lv = 201;
	}
	else if (exp <= js)
	{
		base_exp = 15;
		while (exp >= lv*base_exp)
		{
			exp = exp - lv*base_exp;
			lv++;
		}
	}
	else
	{
		lv = 120;
		n = js;
		while (n <= exp && lv <= 200)
		{
			lv++;
			n = n + (lv - 105)*lv;
		}
	}

	return lv-1;

}


int Cplayer::get_mole_level()
{
	if (mole_level != 0)
	{
		return mole_level;
	}
	else
	{
		return cal_mole_level();
	}
}

int Cplayer::get_day_monster()
{
	if (p_cards_info != NULL)
	{
		return  p_cards_info->day_monster;
	}
	else
	{
		return  0;
	}

}

int Cplayer::get_sl_flag()
{
	if (p_cards_info != NULL)
	{
		return  p_cards_info->sl_flag;
	}
	else
	{
		return  0;
	}

}

int Cplayer::add_v_itemid(int itemid)
{
	itemids.push_back(itemid);

	return 0;
}

int Cplayer::get_card_exp( )
{
	if (p_cards_info == NULL)
	{
		return 0;
	}
	else
	{

    	return  p_cards_info->exp;
	}

}

bool Cplayer::is_in_cards(int cardid)
{

	for (uint32_t i = 0; i < cards_id.size(); i++)
	{
		if (cardid == cards_id[i])
		{
			return true;
		}
	}

	return false;

}

int Cplayer::get_flag( )
{
	if (p_cards_info == NULL)
	{
		return 0;
	}
	else
	{

    	return  p_cards_info->flag;
	}

}

void Cplayer::set_grade(int value)
{
	grade = value;
	DEBUG_LOG("Cplayer: user:%u, set_grade  grade:%d", p->id, grade);
}

int Cplayer::get_grade( )
{
	DEBUG_LOG("Cplayer: user:%u, get_grade  grade:%d", p->id, grade);
	return 	grade;
}



