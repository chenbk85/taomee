/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_accicent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/12/2011 10:26:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_ACCIDENT_INC
#define CUSER_BATTLE_ACCIDENT_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

class Cuser_battle_accident:public CtableRoute100x10
{
	private:

	public:
		Cuser_battle_accident(mysql_interface *db);
		int insert(userid_t userid, uint32_t itemid, uint32_t fail_cnt, uint32_t fight_cnt, 
				uint32_t today, uint32_t wish_cnt);
		int add_wish_item(userid_t userid, uint32_t wish_id, uint32_t wish_cnt, 
				uint32_t today, uint32_t date);
		int get_cnts(userid_t userid, uint32_t *fail_cnt, uint32_t * fight_cnt);
		int get_two_column(userid_t userid, uint32_t *wish_id, uint32_t *date);
		int update_two_column(userid_t userid, uint32_t wish_id, uint32_t wish_cnt);
		int get_three_column(userid_t userid, uint32_t *wish_id, uint32_t *date, uint32_t *wish_cnt);
		int update_wish_info(userid_t userid, uint32_t wish_id, uint32_t today);
		int get_double_col(uint32_t userid, const char* col_1, const char* col_2, uint32_t *value_1, uint32_t *value_2);
	
};
#endif
