/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_connect.h
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

#ifndef  CUSER_CONNECT_INCL
#define    CUSER_CONNECT_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#define   STR_USER_CONNECT_CHILDCOUNT "childcount"

class Cuser_connect:public CtableRoute100x10{
	private:
	public:
		Cuser_connect(mysql_interface * db ); 

		int init(userid_t userid, userid_t parentid, userid_t childid);
		int get_info(userid_t userid, user_connect_get_info_out *p_out );
		int get_info_from_db(userid_t userid, user_connect_get_info_out *p_out );
		int set_parentid(userid_t userid, userid_t parentid);

		int add_childid(userid_t userid, userid_t childid,
		uint32_t *  p_childcount  );
};
#endif   /* ----- #ifndef CUSER_CONNECT_INCL  ----- */
