/*
 * =====================================================================================
 * 
 *       Filename:  Cmonth_task.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CMONTH_TASK_INCL
#define  CMONTH_TASK_INCL
#include "CtableRoute100.h"
#include "proto.h"
class Cmonth_task :public CtableRoute100{
	protected:
		inline int get(uint32_t type ,userid_t userid ,uint32_t * p_count );
	public:
		Cmonth_task(mysql_interface * db ); 
		int insert(uint32_t type  ,userid_t userid,uint32_t count );
		int update(uint32_t type  ,userid_t userid,uint32_t count );
		int get_ex(uint32_t type ,userid_t userid ,uint32_t * p_count );
		int add( uint32_t type ,userid_t userid, uint32_t maxvalue );
		int remove( userid_t userid );
		int remove_by_type ( userid_t userid, uint32_t type );

		int get_value_list(uint32_t userid, uint32_t * p_count,
			sysarg_day_get_list_by_userid_out_item **pp_item  );
};

#endif   /* ----- #ifndef CMONTH_TASK_INCL  ----- */

