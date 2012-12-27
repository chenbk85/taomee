/*
 * =====================================================================================
 * 
 *       Filename:  Cemail.h
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

#ifndef  CEMAIL_INCL
#define  CEMAIL_INCL
#include "CtableString.h"
#include "proto.h"
class Cemail :public CtableString{
	protected:
		char email_c[EMAIL_LEN+1];
	 	char email_mysql[mysql_str_len(EMAIL_LEN)];
		void set_email_str(char * email );
	public:
		Cemail(mysql_interface * db ); 
		int insert(char* email,userid_t userid );
		int getuserid(char* email,userid_t * userid );
		int remove(char* email,userid_t userid);
		int update_email(userid_t userid, char* old_email, char* new_email);

		int map_email_userid (char* email,userid_t userid,userid_t * p_old_map_userid );
};

#endif   /* ----- #ifndef CEMAIL_INCL  ----- */

