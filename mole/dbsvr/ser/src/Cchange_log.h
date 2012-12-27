/*
 * =====================================================================================
 * 
 *       Filename:  Cchange_log.h
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

#ifndef  CCHANGE_LOG_INCL
#define  CCHANGE_LOG_INCL
#include "CtableDate.h"
#include "proto.h"
class Cchange_log : public CtableDate{
	protected:
		int add_db(change_log_item  *p_item  );
		int create_table ( uint32_t logtime );
	public:
		Cchange_log(mysql_interface * db ); 
		int add(change_log_item  *p_item  );
		int update_db(change_log_item *p_item );
};

#endif   /* ----- #ifndef cchange_log_incl  ----- */

