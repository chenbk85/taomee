/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_hot_play.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/01/2011 02:00:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_HOT_PLAY
#define CSYSARG_HOT_PLAY

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_hot_play:public Ctable
{
	public:
		Csysarg_hot_play(mysql_interface *db);
		int insert(uint32_t  id, uint32_t count);
		int update(uint32_t id, uint32_t value);
		int get_all_items(sysarg_get_hot_play_out_item **pp_list, uint32_t *p_count);
};

#endif
