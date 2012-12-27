/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_prentice.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/26/2011 01:25:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_PRENTICE_INC
#define CUSER_BATTLE_PRENTICE_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"


class Cuser_battle_prentice:public CtableRoute100x10
{
	private:

	public:
		Cuser_battle_prentice(mysql_interface* db);
		int insert(userid_t userid, uint32_t master, uint32_t rev_cnt);
		int get_self_relation(userid_t userid, user_battle_get_self_mp_relatrion_out_header *out);
		int update_two_col(userid_t userid, const char *col_1, const char * col_2, uint32_t value_1, 
				uint32_t value_2);
		int get_two_col(userid_t userid, const char *col_1, const char *col_2, uint32_t *value_1,
				uint32_t *value_2);
		int update_merit_inc(userid_t userid, uint32_t value);
		int clear(userid_t userid, uint32_t date);
		int get_three_col(userid_t userid, const char* col_1, const char* col_2, const char* col_3,
				uint32_t* value_1, uint32_t* value_2, uint32_t* value_3);

};
#endif
