/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_sn.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年03月03日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen, tommychen@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_SN_INCL
#define  CUSER_SN_INCL
#include "Ctable.h"
#include "proto.h"
class Cuser_sn :public Ctable{
	public:
		Cuser_sn(mysql_interface * db ); 
		int insert(char* sn,userid_t userid );
		int getuserid(char* sn,userid_t * userid );
		int remove(char* sn,userid_t userid);
};

#endif   /* ----- #ifndef CUSER_SN_INCL  ----- */

