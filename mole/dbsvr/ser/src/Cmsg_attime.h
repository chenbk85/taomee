/*
 * =====================================================================================
 * 
 *       Filename:  Cmsg_attime.h
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

#ifndef  CMSG_ATTIME_INCL
#define  CMSG_ATTIME_INCL
#include "Ctable.h"
#include "proto.h"
class Cmsg_attime : public Ctable{
	protected:
	public:
		Cmsg_attime(mysql_interface * db ); 

		int insert( uint32_t deal_date, uint32_t deal_hour, uint32_t deal_minute,
	  		uint32_t flag, uint32_t pic_id ,msg_item *msg) ;

		int get(  su_get_msg_attime_all_in *p_in ,su_get_msg_attime_all_out *p_out  );
		int remove(  uint32_t deal_date, uint32_t deal_hour, uint32_t deal_minute );

		int get_by_date(su_get_msg_attime_by_date_in *p_in, 
	   	su_get_msg_attime_all_out *p_out  );
};
#endif   /* ----- #ifndef CMSG_ATTIME_INCL  ----- */

