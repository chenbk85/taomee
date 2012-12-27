/*
 * =====================================================================================
 * 
 *       Filename:  CtableRoute100x100_ex.h
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

#ifndef  CTABLEROUTE100x100_ex_INCL
#define  CTABLEROUTE100x100_ex_INCL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "CtableRoute.h"


class CtableRoute100x100_ex : public CtableRoute {
	protected:
		virtual char*  get_table_name(uint32_t id);
	public:
		virtual ~CtableRoute100x100_ex(void){} 
		CtableRoute100x100_ex(mysql_interface * db, const char * db_name_pre,  
		 const char * table_name_pre,const char* id_name );
};

#endif   /* ----- #ifndef CTABLEROUTE_INCL  ----- */

