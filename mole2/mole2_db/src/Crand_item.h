
/*
 * =====================================================================================
 * 
 *       Filename:  Cpet.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  g++
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CRAND_ITEM_INC
#define  CRAND_ITEM_INC

#include "CtableRoute100.h"
#include "proto.h"
#include "benchapi.h"
#include <time.h>
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"




class Crand_item : public CtableRoute100
{
public:
	int insert(userid_t userid, uint32_t randid, uint32_t time, uint32_t count);
	int get_rand_info(uint32_t userid, uint32_t randid, uint32_t *p_time, uint32_t *p_count);
	int set_rand_info(uint32_t userid, uint32_t randid, uint32_t time, uint32_t count);
	int set_rand_info(uint32_t userid, uint32_t randid, uint32_t time);
	int get_rand_info_range(userid_t userid, uint32_t min_id, uint32_t max_id,get_rand_info_range_out *p_out);


	Crand_item(mysql_interface * db);
};

#endif /* ----- #ifndef CPET_INC  ----- */
