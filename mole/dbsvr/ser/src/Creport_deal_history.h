/*
 * =====================================================================================
 * 
 *       Filename:  Creport_deal_history.h
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

#ifndef  CREPORT_DEAL_HISTORY_INCL
#define  CREPORT_DEAL_HISTORY_INCL
#include "Ctable.h"
#include "proto.h"
class Creport_deal_history : public Ctable{
	protected:
	public:
		Creport_deal_history(mysql_interface * db ); 
		int insert( uint32_t userid, uint32_t adminid,  
		uint32_t  deal_type,uint32_t reason, char * nick );
		int get_list(userid_t objuserid , 
			usermsg_get_deal_report_list_in *p_in, uint32_t * p_count,  
			 usermsg_get_deal_report_list_item ** pp_item ) ;
};
#endif   /* ----- #ifndef CREPORT_DEAL_HISTORY_INCL  ----- */

