/*
 * =====================================================================================
 * 
 *       Filename:  Cdwg_user_msg.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 13时47分01秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CDWG_USER_MSG_INCL
#define    CDWG_USER_MSG_INCL
#include "CtableRoute100x100_ex.h"
#include "proto.h"
#include "benchapi.h"


class Cdwg_user_msg:public CtableRoute100x100_ex {
	private:
	public:
		Cdwg_user_msg(mysql_interface * db ); 

		int insert(userid_t userid ,userid_t sendid, char *  msg,
				uint32_t sendtime  ,uint32_t flag    );

};

#endif   /* ----- #ifndef CDWG_USER_MSG_INCL  ----- */
