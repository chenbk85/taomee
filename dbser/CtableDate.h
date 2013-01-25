/*
 * =====================================================================================
 * 
 *       Filename:  CtableDate.h
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

#ifndef  CTABLEDATE_INCL
#define  CTABLEDATE_INCL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CtableRoute.h"


class CtableDate : public CtableRoute {
	protected:
	public:
		virtual char*  get_table_name(time_t logtime );
		CtableDate(mysql_interface * db,const char * db_name_pre,  
			const char * table_name_pre,const  char* id_name ) ;

		virtual ~CtableDate(void){} 
};

#endif   /* ----- #ifndef CTABLEDATE_INCL  ----- */

