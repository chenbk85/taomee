/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angle_barrier.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/08/2011 09:48:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_ANGEL_BARRIER_INC
#define CUSER_ANGEL_BARRIER_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"



class Cuser_angel_barrier:public CtableRoute100x10
{
	private:
	
	public:
		Cuser_angel_barrier(mysql_interface *db);
		int insert(userid_t userid, uint32_t barrier_id, uint32_t type, uint32_t score, uint32_t succ);
		int update(userid_t userid, uint32_t barrier_id, uint32_t type, uint32_t score);
		int update_succ(userid_t userid, uint32_t barrier_id, uint32_t type, uint32_t pass);
		int select(userid_t userid, uint32_t barrier_id, uint32_t type, uint32_t &score);
		int del(userid_t userid, uint32_t barrier_id, uint32_t type);
		int get_barrier_score(userid_t userid, uint32_t barrier_id, uint32_t *p_list);
		int get_barrier_succss(userid_t userid, uint32_t barrier_id, user_angel_barrier_stru **pp_list,
				uint32_t *p_count);
};
#endif
