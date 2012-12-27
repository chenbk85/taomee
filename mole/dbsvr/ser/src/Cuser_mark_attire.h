/*
 * =====================================================================================
 *
 *       Filename:  Cuser_mark_attire.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2012 02:11:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_MARK_ATTIRE_INC
#define CUSER_MARK_ATTIRE_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_mark_attire:public CtableRoute100x10

{
	public:
		Cuser_mark_attire(mysql_interface *db);
		int insert(userid_t userid, uint32_t attireid, uint32_t score);
		int update(userid_t userid, uint32_t attireid, uint32_t score);
		int get_all(userid_t userid, uint32_t *p_count, user_get_mark_attire_out_item **pp_list);
};
#endif
