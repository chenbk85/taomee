/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_fish_weight.h
 *
 *    Description:  举行了一个活动，就是大家可以去钓鱼，此处定义钓鱼用的记录
 *
 *        Version:  1.0
 *        Created:  2009年11月30日 11时44分43秒
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Croominfo_fish_weight.h
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef CROOMINFO_FISH_WEIGHT_INCL
#define CROOMINFO_FISH_WEIGHT_INCL

#include "CtableRoute10x10.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"

class Canimal_weight: public CtableRoute10x10
{
public:
    Canimal_weight(mysql_interface * db):CtableRoute10x10(db, "ROOMINFO","t_roominfo_animal_weight","user_id"){};

	int fish_get_weight(const userid_t uid, uint32_t& weight)
	{
		return sql_weight(uid, weight);
	}

	int fish_set_weight(const userid_t uid, const uint32_t ,  const uint32_t weight)
	{
		int ret=sql_update(uid, weight);
		if(ret==USER_ID_EXISTED_ERR)
		{
			ret=sql_insert(uid);
			return sql_update(uid, weight);
		}
		return ret;
	}

private:
	int sql_insert(const userid_t uid);
	int sql_weight(const userid_t uid, uint32_t& weight);
	int sql_update(const userid_t uid, const uint32_t weight);
};


#endif //CROOMINFO_FISH_WEIGHT_INCL

