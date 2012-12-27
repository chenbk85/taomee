/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_cos_rank.h
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
#ifndef CSYSARG_COS_RANK_H
#define CSYSARG_COS_RANK_H

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_cos_rank : public Ctable
{
public:
	Csysarg_cos_rank(mysql_interface *db);

	int insert(uint32_t userid, uint32_t count,  char* nick);
	int update_last_record(uint32_t datetime, uint32_t userid, uint32_t count, char* nick);
	int get_cos_rank(uint32_t datetime, sysarg_get_cos_rank_out_item  **pp_items, uint32_t *p_count);
	int update_count(uint32_t datetime, uint32_t userid,  uint32_t cnt);
	int get_min_count(uint32_t datetime,  uint32_t *count);
	int get_record_count(uint32_t datetime,  uint32_t *count);
	int get_tenth_count(uint32_t datetime,  uint32_t *count);
	int get_user_record(uint32_t userid,  uint32_t *count);
	int get_user_date_count(uint32_t userid, uint32_t datetime,  uint32_t *count);

};

#endif

