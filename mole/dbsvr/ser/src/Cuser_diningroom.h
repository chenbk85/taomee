/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_diningroom.h
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

#ifndef  CUSER_DININGROOM_INCL
#define  CUSER_DININGROOM_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_diningroom: public CtableRoute100x10 {
public:
	Cuser_diningroom(mysql_interface * db); 

	int insert(userid_t userid, user_diningroom_create_room_in* p_in); 

	int set_room_name(userid_t userid, user_diningroom_set_room_name_in *p_in);

	int set_room_style(userid_t userid, user_diningroom_set_room_style_in *p_in);

	int set_room_inner_style(userid_t userid, user_diningroom_set_room_inner_style_in *p_in);
	
	int get_diningroom_info(userid_t userid, uint32_t type_id, user_get_diningroom_out_header *p_out);

	int get_board_info(userid_t userid, uint32_t type_id, user_dining_get_user_board_info_out& out);
	
	int update_exp_inc(userid_t userid, uint32_t exp);
	int update_xiaomee_inc(userid_t userid, uint32_t xiaomee);
	int update_off_xiaomee_inc(userid_t userid, uint32_t xiaomee);
	int update_xiaomee_dec(userid_t userid, uint32_t xiaomee);
	int update_evaluate_inc(userid_t userid, uint32_t evaluate);
	int update_evaluate_dec(userid_t userid, uint32_t evaluate);
	int update_off_evaluate_inc(userid_t userid, uint32_t evaluate);
	int update_off_evaluate_dec(userid_t userid, uint32_t evaluate);
	int update_time(userid_t userid, uint32_t cur_time);
	int get_time(userid_t userid, uint32_t& eat_time);
	int set_evaluate(userid_t userid, uint32_t evaluate);
	int get_level_evaluate(userid_t userid, uint32_t& level, uint32_t& evaluate);
	int get_user_diningroom_count(userid_t userid, uint32_t type_id, uint32_t& count);
	int set_honor_flag(userid_t userid, uint32_t index);
	int update_evaluate_xiaomee_time_inc(userid_t userid, uint32_t evaluate, uint32_t xiaomee, uint32_t time);

	int event_num_set(userid_t userid);
	int event_num_get(userid_t userid, uint32_t &count);
	int update_exp_dec(userid_t userid, uint32_t exp);
	int get_exp(userid_t userid, uint32_t& exp_out);

};

#endif   /* ----- #ifndef CUSER_DININGROOM_INCL  ----- */

