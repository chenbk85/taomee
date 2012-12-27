/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_mark_attire.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2012 11:39:37 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_MARK_ATTIRE_INC
#define CSYSARG_MARK_ATTIRE_INC


#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_mark_attire:public Ctable
{
	public:
		Csysarg_mark_attire(mysql_interface *db);
		int insert(uint32_t attireid, uint32_t score, uint32_t count);
		int update(uint32_t attireid, uint32_t score, uint32_t count);
		int select_all(uint32_t *p_count, sysarg_get_mark_attire_out_item **pp_list);
		int add(uint32_t attireid, uint32_t score, uint32_t count);
};
#endif
