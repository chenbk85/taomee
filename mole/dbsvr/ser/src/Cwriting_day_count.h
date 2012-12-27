/*
 * =====================================================================================
 * 
 *       Filename:  Cwriting_day_count.h
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

#ifndef  Cwriting_day_count_INCL
#define  Cwriting_day_count_INCL
#include "CtableRoute100.h"
#include "proto.h"
class Cwriting_day_count : public CtableRoute100{
	protected:
		uint32_t writing_max_a_day;
	public:
		Cwriting_day_count(mysql_interface * db ); 
		int insert(uint32_t gameid, userid_t userid,uint32_t day,uint32_t count) ;
		int update(uint32_t gameid, userid_t userid,uint32_t day,uint32_t count) ;
		int add_count(uint32_t gameid, userid_t userid) ;
		int get(uint32_t gameid, userid_t userid, uint32_t *p_day, uint32_t * p_count) ;
};
#endif   /* ----- #ifndef CWRITING_INCL  ----- */

