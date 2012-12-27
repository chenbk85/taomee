/*
 * =====================================================================================
 * 
 *       Filename:  Cdamee_history.h
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

#ifndef  CDAMEE_HISTORY_INCL
#define  CDAMEE_HISTORY_INCL
#include "CtableMonth.h"
#include "proto.h"

class Cdamee_history :public CtableMonth{
	protected:
	public:
		Cdamee_history(mysql_interface * db ); 
		int gen_record( uint32_t date , uint32_t paytype, userid_t userid, 
			int damee, uint32_t leave_damee  , char * pri_msg );
		int get_record(userid_t userid,uint32_t startdate,uint32_t enddate ,
				uint32_t startindex, uint32_t precount , uint32_t*p_count,	
				DAMEE_RECORD**pp_record);

		int get_record(	userid_t userid,uint32_t startdate,uint32_t enddate ,
				uint32_t startindex, uint32_t precount, 
				PAY_GET_DAMEE_LIST_OUT * p_out);

};

#endif   /* ----- #ifndef CDAMEE_HISTORY_INCL  ----- */

