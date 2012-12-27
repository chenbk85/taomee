/*
 * =====================================================================================
 * 
 *       Filename:  Cchat.h
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

#ifndef  CCHAT_INCL
#define  CCHAT_INCL
#include "CtableDate.h"
#include "proto.h"
struct chat_log_stru 
{
    int32_t chat_count;
    int32_t userid;
}__attribute__((packed)) ;
class Cchat : public CtableDate{
	protected:
		char *msglog_file;
		struct chat_log_stru  chat_log;
			
		int add_db( chat_item *p_item  );
		int create_table ( uint32_t logtime );
	public:
		Cchat(mysql_interface * db );
		int add( chat_item *p_item  );
};

#endif   /* ----- #ifndef CCHAT_INCL  ----- */

