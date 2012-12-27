/*
 * =====================================================================================
 * 
 *       Filename:  Cdwg_user_friend.h
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

#ifndef  CDWG_USER_FRIEND_INCL
#define    CDWG_USER_FRIEND_INCL
#include "CtableRoute100x100_ex.h"
#include "proto.h"
#include "benchapi.h"


class Cdwg_user_friend:public CtableRoute100x100_ex {
	private:
	public:
		Cdwg_user_friend(mysql_interface * db ); 
		int   get_list(userid_t userid, 
		uint32_t *p_count, stru_id ** pp_list   )	;
};

#endif   /* ----- #ifndef CDWG_USER_FRIEND_INCL  ----- */
