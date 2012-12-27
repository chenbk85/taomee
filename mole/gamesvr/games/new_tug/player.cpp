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

Cplayer::Cplayer():bout_cardid(0),life_value(6),result(0),exp(0),medal(0),xiaomee(0)
{

}

int Cplayer::create_cards_seq( )
{
	uint32_t userid = id();
    srand(time(NULL) + userid);
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

void Cplayer::set_result( int value)
{
	result = value;

}

void Cplayer::bout_clear()
{
	bout_cardid = -1;
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



