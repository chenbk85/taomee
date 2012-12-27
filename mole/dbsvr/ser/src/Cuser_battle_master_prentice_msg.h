/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_master_prentice_msg.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/26/2011 09:27:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#ifndef CUSER_BATTLE_MASTER_PRENTICE_MSG_INC
#define CUSER_BATTLE_MASTER_PRENTICE_MSG_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_battle_master_prentice_msg:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_battle_master_prentice_msg(mysql_interface *db);
		int insert(userid_t userid, uint32_t master, uint32_t prentice, uint32_t train_id, uint32_t exp, uint32_t merit);
		int get_all(userid_t userid, user_battle_get_master_prentice_info_out_item **pp_list, uint32_t *p_count);
		int get_oldest(userid_t userid, uint32_t *datetime);
		int update(userid_t userid, user_battle_get_master_prentice_info_out_item *p_item, uint32_t datetime);
		int update_record(userid_t userid, user_battle_get_master_prentice_info_out_item *p_item);
		int get_record_cnt(userid_t userid, uint32_t &count);

};
#endif
