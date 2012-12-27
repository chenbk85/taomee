/*
 * =====================================================================================
 *
 *       Filename:  Cgf_hunter_top.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/01/2011 01:37:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Saga (), saga@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef __GF_CGF_HUNTER_TOP_H__
#define __GF_CGF_HUNTER_TOP_H__

#include "CtableRoute.h"
#include "proto.h" 
#include "benchapi.h"

class Cgf_hunter_top:public Ctable
{
	public:
		Cgf_hunter_top(mysql_interface *db);

        int get_over_tower_info(uint32_t userid, uint32_t role_tm, uint32_t* layer, uint32_t* interval, uint32_t* place_tm, uint32_t reward_flg = 0);
		int set_hunter_top_info(uint32_t userid, uint32_t role_tm, gf_set_hunter_top_info_in* p_in);
        int get_hunter_order(uint32_t role_type, uint32_t layer, uint32_t interval, uint32_t place_tm, uint32_t* order, uint32_t reward_flg = 0);
		int get_hunter_top_info(uint32_t role_type, uint32_t top_num, uint32_t* count, gf_get_hunter_top_info_out_item** pout);
		int get_hunter_top_info(uint32_t role_type, uint32_t top_num, uint32_t* count, gf_get_hunter_top_info_for_web_out_item** pout);
		int get_top_cnt(uint32_t* p_count, uint32_t role_type);
		int get_one_over_tower_info(uint32_t role_type, uint32_t* p_userid, uint32_t* p_role_tm);
		int del_overflowed_info(uint32_t userid, uint32_t role_tm);
		int cut_hunter_top(uint32_t role_type);
		int get_last_info(uint32_t* p_layer, uint32_t* p_tm, uint32_t* p_place_tm, uint32_t role_type);

		int get_hunter_old_topXX(uint32_t top_num, uint32_t role_type, uint32_t* count, gf_get_hunter_old_top100_out_item** pout);
		int get_hunter_old_place(uint32_t userid, uint32_t role_tm, uint32_t role_type);
		int del_player_place_in_old_top(uint32_t userid, uint32_t role_tm);
		int get_role_type(uint32_t userid, uint32_t role_tm, uint32_t* p_role_type);
	private:

};
#endif


