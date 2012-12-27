/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angel_honor.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/12/2011 02:17:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CUSER_ANGEL_HONOR_INC
#define CUSER_ANGEL_HONOR_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_angel_honor:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_angel_honor(mysql_interface *db);
		int insert(userid_t userid, uint32_t honor_type, uint32_t honor_id, uint32_t unlock,
			   	uint32_t recv);
		int update_lock(userid_t userid, uint32_t honor_type, uint32_t honor_id, uint32_t unlock);
		int update_recv(userid_t userid, uint32_t honor_type, uint32_t honor_id, uint32_t recv);
		int update_recv_lock(userid_t userid, uint32_t honor_type, uint32_t honor_id,
			   	uint32_t unlock, uint32_t recv);
		int get_lock(userid_t userid, uint32_t honor_type, uint32_t honor_id, uint32_t *lock);
		int get_recv(userid_t userid, uint32_t honor_type, uint32_t honor_id, uint32_t *recv);
		int get_recv_lock(userid_t userid, uint32_t honor_type, uint32_t honor_id, 
					uint32_t *lock, uint32_t *recv);
		int get_all_locks(userid_t userid, uint32_t honor_type, 
		 		user_get_angel_honors_out_item **pp_list, uint32_t *p_count);
		int get_record_cnt(userid_t userid, uint32_t *cnt);
		int get_honor_ids(userid_t userid, uint32_t honor_type, 
			 uint32_t **pp_list, uint32_t *p_count);
		int get_new_locks_cnt(userid_t userid,  uint32_t *cnt);
};

#endif
