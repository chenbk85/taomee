/*
 * =====================================================================================
 * 
 *       Filename:  Ctable.h
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

#ifndef  CTABLE_INCL
#define  CTABLE_INCL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql_iface.h"
#define  GEN_SQLSTR(sqlstr,...)  snprintf( sqlstr, sizeof(sqlstr),__VA_ARGS__ )
class Ctable {
	protected:
		char sqlstr[8192*2]; 
		mysql_interface * db;
		char db_name[50];
		char table_name[50];
		//: db.table 
		char db_table_name[100];

	public:
		virtual char*  get_table_name();
		Ctable(mysql_interface * db,const char * dbname,const char * tablename  ); 
		virtual ~Ctable(void){} 
};

#endif   /* ----- #ifndef CTABLE_INCL  ----- */

