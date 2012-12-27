/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_dining_dish.h
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

#ifndef  CUSER_DINING_DISH_INCL
#define  CUSER_DINING_DISH_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_dining_dish: public CtableRoute100x10 {
public:
	Cuser_dining_dish(mysql_interface * db); 

	int add_dish(userid_t userid, user_dining_cook_dish_in* p_in, uint32_t state, uint32_t& id);
	
	int put_dish_chest(userid_t userid, user_dining_dish_in_chest_in* p_in, uint32_t state); 

	int get_user_dish(userid_t userid, user_dining_dish_item** pp_list, uint32_t* p_count);
	
	int get_user_dish_state(userid_t userid, user_dining_dish_item** pp_list, uint32_t* p_count);

	int get_dish_id_count(userid_t userid, uint32_t id, uint32_t& count);
	
	int get_dish_id_state_location(userid_t userid, uint32_t id, uint32_t& state, uint32_t& location);

	int get_dish_count(userid_t userid, uint32_t& count);

	int update_dish_dec(userid_t userid, user_dining_eat_dish_in* p_in); 
	
	int fall_dish(userid_t userid, user_dining_fall_dish_in* p_in); 
	int set_dish_id_state(userid_t userid, uint32_t id, uint32_t state); 
	int set_dish_id_time(userid_t userid, uint32_t id, uint32_t cur_time);
	int del_dish(userid_t userid, uint32_t id); 
	int del_all_dish(userid_t userid); 
	int get_dish_all_count(userid_t userid, uint32_t& count);
	int get_dish_id_time(userid_t userid, uint32_t id, uint32_t& time);
	int get_dish_state_location_count(userid_t userid, uint32_t dish_id, uint32_t location, uint32_t& id, uint32_t& count);
	int update_dish_id_inc(userid_t userid, uint32_t id, uint32_t count);
	int del_dish_location_state(userid_t userid, uint32_t dish_id, uint32_t location); 
	int get_dish_id_state_count(userid_t userid, uint32_t id, uint32_t& count);
	int del_dish_id_state(userid_t userid, uint32_t id);
	int update_dish_num_dec(userid_t userid, uint32_t id, uint32_t num);
	int get_dish_bench_num(userid_t userid, uint32_t max_bench,  uint32_t& count);
	int get_dish_cooler_num(userid_t userid, uint32_t max_cooler,  uint32_t& count);
	int update_dish_num_dec(userid_t userid, user_dining_dec_dish_in* p_in); 
	int get_dish_count_by_dish_id(userid_t userid, uint32_t dish_id, uint32_t& count);
	int update_dish_num_dec_by_dish_id(userid_t userid, uint32_t dish_id, uint32_t num); 
	int get_dish_count_by_location(userid_t userid, uint32_t location,  uint32_t& count);
	int del_dish_id_state_by_dish_id(userid_t userid, uint32_t dish_id ) ;

};

#endif   /* ----- #ifndef CUSER_DINING_DISH_INCL  ----- */

