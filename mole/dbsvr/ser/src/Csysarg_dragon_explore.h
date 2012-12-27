/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_dragon_explore.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/26/2011 04:23:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_DRAGON_EXPOLRE_INCL
#define CSYSARG_DRAGON_EXPOLRE_INCL

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_dragon_explore:public Ctable
{
	public:
		Csysarg_dragon_explore(mysql_interface *db);
		int insert(uint32_t itemid, uint32_t count);
		int drop();
		int get_all(user_sysarg_get_map_out_item **pp_list, uint32_t *p_count);
		int get_count(uint32_t *total);
};
#endif
