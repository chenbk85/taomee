/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_dining_dish_history.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年03月09日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen, tommychen@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_DINING_DISH_HISTORY_INCL
#define  CUSER_DINING_DISH_HISTORY_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_dining_dish_history: public CtableRoute100x10 {
public:
	Cuser_dining_dish_history(mysql_interface * db); 
	
	
	int insert(userid_t userid, uint32_t dish_id, uint32_t count) ;

	int update_dish_history_inc(userid_t userid, uint32_t dish_id, uint32_t count);

	int get_dish_history_count(userid_t userid, uint32_t& count);

	int dish_stars_get(uint32_t userid, uint32_t dish_id, uint32_t *p_dish_count, uint32_t *p_dish_stars);

	int dish_stars_set(userid_t userid, uint32_t dish_id, uint32_t dish_stars);
	
	int select_count_at_star_5(userid_t userid,uint32_t &count);

	int select_dish_star(userid_t userid, select_dish_stars_out_item **pp_list, uint32_t *p_count);
};

#endif   /* ----- #ifndef CUSER_DINING_DISH_HISTORY_INCL  ----- */

