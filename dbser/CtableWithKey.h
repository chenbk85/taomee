/*
 * =====================================================================================
 * 
 *       Filename:  CtableWithKey.h
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

#ifndef  CTABLEWITHKEY_INCL
#define  CTABLEWITHKEY_INCL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Ctable.h"


class CtableWithKey : public Ctable {
	protected:
		char id_name[25];
	public:
		// id_name : 用于id_is_existed方法中
		virtual int id_is_existed(uint32_t id, bool * existed);
		CtableWithKey (mysql_interface * db,const  char * dbname, 
		const char * tablename,const char* id_name   ); 
		virtual ~CtableWithKey(void){} 
};

#endif   /* ----- #ifndef CTABLEWITHKEY_INCL  ----- */

