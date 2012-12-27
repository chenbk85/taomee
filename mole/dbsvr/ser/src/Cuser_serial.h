/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_serial.h
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

#ifndef  CUSER_SERIAL_INCL
#define  CUSER_SERIAL_INCL
#include "Ctable.h"
#include "proto.h"
class Cuser_serial :Ctable{
	protected:
		int gen_serial(char * user_serialid );
		int insert(userid_t userid,uint32_t type,  char * user_serialid);
		int update(userid_t userid,uint32_t type,  char * user_serialid);
	public:
		Cuser_serial(mysql_interface * db ); 
		int add_serial (userid_t userid,uint32_t type,  char * user_serialid);
		int del(userid_t userid,uint32_t type);
		int get_serialid(userid_t userid ,uint32_t type,  
		userserial_get_serial_out * p_out );

};



#endif   /* ----- #ifndef CUSER_SERIAL_INCL  ----- */

