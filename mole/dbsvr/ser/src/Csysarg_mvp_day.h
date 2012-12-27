/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_mvp_day.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/27/2012 05:43:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_MVP_DAY
#define CSYSARG_MVP_DAY

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_mvp_day:Ctable
{
    public:
        Csysarg_mvp_day(mysql_interface *db);
		int update_one(uint32_t day,const char* col, uint32_t value);
//		int get_two(uint32_t day,uint32_t* data1,uint32_t*data2);
		int get_one(const char* col,uint32_t* data);
		int insert(uint32_t day);
		int remove(uint32_t day);
		int update(const char* col, uint32_t value);
		int insert_newday(uint32_t day,const char* col,uint32_t value);
		int update_oldday(uint32_t day,const char* col, uint32_t value);
		int get_one_by_col(uint32_t day,const char* col, uint32_t* value);
		int get_last_day(uint32_t* oldday);
        int get_current(uint32_t day,uint32_t* data1,uint32_t*data2);
        int get_two(uint32_t day,struct p_list** pp_list,uint32_t *count,uint32_t *data1,uint32_t* data2);
};
#endif
