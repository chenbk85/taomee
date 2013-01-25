/*
 * =====================================================================================
 * 
 *       Filename:  CtableRoute10.h
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

#ifndef  CTABLEROUTE10_INCL
#define  CTABLEROUTE10_INCL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CtableRoute.h"


class CtableRoute10 : public CtableRoute {
	protected:
		virtual char*  get_table_name(uint32_t id);
	public:
		virtual ~CtableRoute10(void){} 
		CtableRoute10(mysql_interface * db, const char * db_name_pre,  
		 const char * table_name_pre,const char* id_name,const char * key2_name=""  );
};

#endif   /* ----- #ifndef CTABLEROUTE_INCL  ----- */

