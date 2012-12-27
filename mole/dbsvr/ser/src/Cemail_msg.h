/*
 * =====================================================================================
 * 
 *       Filename:  Cemail_msg.h
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

#ifndef  CEMAIL_MSG_INCL
#define  CEMAIL_MSG_INCL
#include "CtableRoute10x10.h"
#include "proto.h"


class Cemail_msg : public CtableRoute10x10{
	protected:
		int set_add_msg_for_update(userid_t userid);
		int init(userid_t userid, email_msg *p_item);
	public:
		Cemail_msg(mysql_interface * db ) ;

		//增加消息
		//未读加1
		int set_add_msg(userid_t userid);
		//读取消息
		//未读加-1
		int set_read(userid_t userid);
		int get_msg_noread_count(userid_t userid , emailsys_get_email_msg_out *p_out  );
		int init_email_noread(userid_t userid);
};

#endif   /* ----- #ifndef CEMAIL_MSG_INCL  ----- */

