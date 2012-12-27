/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_dragon_egg.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年12月22日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  zheng, zheng@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_DRAGON_EGG_INCL
#define  CUSER_DRAGON_EGG_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_dragon_egg : public CtableRoute100x10 {
public:
	Cuser_dragon_egg(mysql_interface * db); 

	uint32_t get_item_growth(uint32_t itemid);
	uint32_t get_egg_time_limit(uint32_t dragon_egg_id);
	int check_egg_is_hatched(uint32_t dragon_id,uint32_t start);	

	int add(userid_t userid, uint32_t dragon_egg_id); 
	int del(userid_t userid, uint32_t dragon_egg_id); 
	int get_egg_hatch(userid_t userid, uint32_t &dragon_egg_id, uint32_t &time); 
	int add_egg_growth(userid_t userid, uint32_t dragon_egg_id, uint32_t growth); 
};

#endif   /* ----- #ifndef CUSER_CARD_INCL  ----- */

