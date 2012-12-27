/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_top_rank.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/13/2010 03:14:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CSYSARG_TOP_RANK_H
#define CSYSARG_TOP_RANK_H

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_top_rank : public Ctable
{
public:
	Csysarg_top_rank(mysql_interface *db);

	int update(userid_t userid, uint32_t type, uint32_t value, uint32_t datetime);
	int get_top_rank(uint32_t type, sysarg_get_ranklist_out_item **pp_items, uint32_t *p_count);
	int add(uint32_t type, uint32_t *cur_count);
	int select_count_datetime(uint32_t type, uint32_t *count, uint32_t *datetime);
	int update_count_time(uint32_t type, uint32_t cnt);
	int update_count(uint32_t type, uint32_t cnt);
	int insert(uint32_t type);
private:
	int update_one_user(userid_t userid, uint32_t type, uint32_t value, uint32_t datetime);

};

#endif

