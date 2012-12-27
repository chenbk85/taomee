/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_msg.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/15/2011 04:23:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PIGLET_MSG_INC
#define CUSER_PIGLET_MSG_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_piglet_msg:public CtableRoute100x10{
	private:

	public:
		Cuser_piglet_msg(mysql_interface *db);	
		int insert(userid_t userid, uint32_t datetime, uint32_t type, uint32_t other_id, 
				int32_t itemid, uint32_t value, char* nick);
		int get_oldest(userid_t userid, uint32_t *datetime);
		int get_all(userid_t userid, user_get_all_msg_out_item **pp_list, uint32_t* p_count);
		int get_record_cnt(userid_t userid, uint32_t &cnt);
		int update_records(userid_t userid, user_get_all_msg_out_item *para);
		int update(userid_t userid, user_get_all_msg_out_item* para, uint32_t datetime);
};

#endif
