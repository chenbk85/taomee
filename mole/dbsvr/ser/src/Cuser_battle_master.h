/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_master.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/26/2011 11:44:57 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_MASTER_INC
#define CUSER_BATTLE_MASTER_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_battle_master:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_battle_master(mysql_interface *db);
		int insert(userid_t userid, uint32_t prentice, uint32_t ini_lvl, uint32_t thank_flag);
		int clear_train_info(userid_t userid);
		int update_train_by_id(userid_t userid, uint32_t prentice, uint32_t train_id);
		int recive_prentice_res(userid_t userid, uint32_t prentice);
		int get_train(userid_t userid, uint32_t prentice, uint32_t *train);
		int get_all_prentices(userid_t userid, uint32_t** pp_list, uint32_t* p_count);
		int get_all_prentices_info(userid_t userid, user_battle_get_self_mp_relatrion_out_item** pp_list, 
				uint32_t* p_count);
		int get_train_info(userid_t userid, uint32_t prentice, user_battle_get_train_info_out_item *p_list,
				   uint32_t *p_count);
		int train_prentice(userid_t userid, uint32_t prentice, uint32_t train_id);
		int remove_prentice(userid_t userid, uint32_t prentice_id);
		int update_one_col(userid_t userid, uint32_t prentice, const char* col, uint32_t value);
		int get_prentice_count(userid_t userid, uint32_t *cnt);
		int get_one_col(userid_t userid, uint32_t prentice, const char* col, uint32_t *value);
};
#endif
