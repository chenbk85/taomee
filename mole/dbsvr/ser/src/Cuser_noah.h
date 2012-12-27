/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_noah.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年02月26日 15时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen, tommychen@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_NOAH_INCL
#define  CUSER_NOAH_INCL
#include "CtableRoute10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_noah : public CtableRoute10 {
public:
	Cuser_noah(mysql_interface * db); 

	int insert(userid_t userid, char* sn); 

	int del(userid_t userid, char* sn);

	int get_user_noah_bind_info(userid_t userid,  user_noah_get_user_bind_info_out & out);
	
	int get_user_noah_user_bind(userid_t userid, uint32_t & count);

};

#endif   /* ----- #ifndef CUSER_NOAH_INCL  ----- */
