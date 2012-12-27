/*
 * =====================================================================================
 * 
 *       Filename:  Cregister_serial.h
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

#ifndef  CREGISTER_SERIAL_INCL
#define  CREGISTER_SERIAL_INCL
#include "Ctable.h"
class Cregister_serial :Ctable{
	protected:
	public:
		Cregister_serial(mysql_interface * db ); 
		int get_userid (char * register_serialid , uint32_t *p_userid );
};



#endif   /* ----- #ifndef CREGISTER_SERIAL_INCL  ----- */

