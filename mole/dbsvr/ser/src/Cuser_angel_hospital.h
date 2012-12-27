/*
 * =====================================================================================
 *
 *       Filename:  Cuser_angel_hospital.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/22/2011 10:21:50 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_ANGEL_HOSPITAL_INC
#define CUSER_ANGEL_HOSPITAL_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_angel_hospital:public CtableRoute100x10
{
	public:
		Cuser_angel_hospital(mysql_interface *db);
		int insert(userid_t userid, uint32_t angelid);
		int select(userid_t userid, user_get_angel_in_hospital_out_item **pp_list, uint32_t *p_count);
		int go_out_hospital(userid_t userid, uint32_t id);
		int recover(userid_t userid, uint32_t id, uint32_t angelid);
		int get_angelid(userid_t userid, uint32_t id, uint32_t &angelid, uint32_t &state);
		int get_recover_time(uint32_t angelid, int32_t &time);
		int get_state(userid_t userid, uint32_t id, uint32_t &state);

};


#endif
