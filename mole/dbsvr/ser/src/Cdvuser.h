/*
 * =====================================================================================
 * 
 *       Filename:  Cdvuser.h
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

#ifndef  CDVUSER_INCL
#define  CDVUSER_INCL
#include "CtableWithKey.h"
#include "proto.h"


class Cdvuser : public CtableWithKey{
	protected:
		
	public:
		Cdvuser(mysql_interface * db ); 
		int insert( userid_t  userid , char *passwd ,uint32_t sex,char * nick, char * email  ) ;
		int change_passwd(userid_t userid ,  char *newpasswd   );
		int change_nick(userid_t userid ,  char *newnick);
};
#endif   /* ----- #ifndef CDVUSER_INCL  ----- */

