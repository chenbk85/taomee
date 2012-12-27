/*
 * =====================================================================================
 * 
 *       Filename:  Cuser.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_SWAP_CARD_INC
#define  CUSER_SWAP_CARD_INC
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "Citem_change_log.h"






class Cuser_swap_card :public CtableRoute100x10 {
	private:
		Citem_change_log *p_item_change_log;
	public:
		Cuser_swap_card(mysql_interface *db, Citem_change_log* p_log); 
		int insert(userid_t userid, uint32_t card_type);
		int get_card_num(userid_t userid, uint32_t card_type, uint32_t *p_num);
		int update_num(userid_t userid, uint32_t card_type, uint32_t num);
		int add(userid_t userid, uint32_t card_type, uint32_t is_vip_opt_type = 0);
		int change(userid_t userid, uint32_t type, uint32_t card_type, uint32_t swap_id);
		int set_swap_flag(userid_t userid, uint32_t card_type, uint32_t state, uint32_t swap_id, uint8_t *p_in);
		int swap_card(userid_t userid, uint32_t swap_type);
		int swap_super_card(userid_t userid, uint32_t swap_type, uint32_t times);
		int get_swap_flag(userid_t userid, uint32_t card_type, uint32_t *p_flag, uint32_t *p_swapid);
		int update_num_flag(userid_t userid, uint32_t card_type, uint32_t num, uint32_t flag);
		int get_card_all(userid_t userid, card_info_db **p_list, uint32_t *p_count);
		int search_card(userid_t userid, uint32_t swap_id, uint32_t need_id, 
			user_swap_card_search_out_item_1 **pp_list, uint32_t *p_count);
		int get_swap_card(userid_t userid, uint32_t swap_id, uint32_t need_id, char *out, uint32_t *p_count);
		int get_max_num();
		int update_card(userid_t userid, uint32_t card_id, uint32_t num, uint32_t swap_flag, uint32_t need_card);
		int set_card(userid_t userid, uint32_t card_id, uint32_t num, uint32_t swap_flag, uint32_t need_card);
		int get_card_interval(userid_t userid, uint32_t card_start, uint32_t card_end,
			user_get_attire_list_out_item  **pp_list, uint32_t *p_count);
		int del(userid_t userid, uint32_t card_id);
		int set_swap_flag_ex(userid_t userid, uint32_t card_type, uint32_t state, uint32_t swap_id);
		int del_card(userid_t userid, uint32_t card_id, uint32_t is_vip_opt_type = 0);
};


#endif   /* ----- #ifndef CUSER_INC  ----- */

