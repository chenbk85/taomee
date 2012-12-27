/*
 * =====================================================================================
 * 
 *       Filename:  Cusermap_day.h
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

#ifndef  CUSERMAP_DAY_INCL
#define  CUSERMAP_DAY_INCL
#include "Ctable.h"
#include "proto.h"
class Cusermap_day :public Ctable{
public:
		int get(uint32_t type ,userid_t userid ,uint32_t * p_count );
		Cusermap_day(mysql_interface * db ); 
		int insert(uint32_t type  ,userid_t userid,uint32_t count );
		int update(uint32_t type  ,userid_t userid,uint32_t count );
		int get_ex(uint32_t type ,userid_t userid ,uint32_t * p_count );
		int add( uint32_t type ,userid_t userid, uint32_t maxvalue );
		int add_cnt( uint32_t type ,userid_t userid, uint32_t maxvalue,uint32_t cnt, uint32_t *count);
		int remove( userid_t userid );
		int remove_by_type ( userid_t userid, uint32_t type );

		int get_value_list(uint32_t userid, uint32_t * p_count,
			sysarg_day_get_list_by_userid_out_item **pp_item  );

		int get_value_list_by_type(uint32_t userid, uint32_t start_type, uint32_t end_type, uint32_t * p_count,
	 sysarg_day_get_list_out_item **pp_item  );
		int get_value_list_by_some_type(userid_t userid, uint32_t in_count, 
			sysarg_day_get_value_in_item* p_in_item, sysarg_day_get_value_out_item **pp_out_item, uint32_t *out_count);

};

#endif   /* ----- #ifndef CUSERMAP_DAY_INCL  ----- */

