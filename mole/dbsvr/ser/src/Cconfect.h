/*
 * =====================================================================================
 * 
 *       Filename:  Ccup.h
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

#ifndef  CCONFECT_INCL
#define  CCONFECT_INCL
#include "CtableRoute100.h"
#include "proto.h"
struct confect_stru {
	uint32_t flag;
	uint32_t task;
	uint32_t logdate;
	uint32_t count_today;
	uint32_t count_all;
}__attribute__((packed)) ;


class Cconfect : public CtableRoute100{
	protected:
	public:
	Cconfect(mysql_interface * db );
	int insert(userid_t userid, confect_stru * p_confect );
	int get_value(userid_t userid, confect_stru * p_confect  );
	int update_value(userid_t userid, confect_stru * p_confect );
	int update_task(userid_t userid, uint32_t task );
	int get_value_ex(userid_t userid, confect_get_value_out* p_out);

	int add_count(userid_t userid, confect_change_value_in * p_in,
	 confect_change_value_out * p_out   	);
	int update_count_all(userid_t userid, confect_set_value_in *p_in );
};

#endif   /* ----- #ifndef CCONFECT_INCL  ----- */

