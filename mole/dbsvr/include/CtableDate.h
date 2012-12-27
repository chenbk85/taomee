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
#include "common.h"
#include "CtableRoute.h"


class CtableDate : public CtableRoute
{
public:
	CtableDate(mysql_interface * db,const char * db_name_pre,  const char * table_name_pre,const  char* id_name ) ;

	/**
	 * @brief 通过指定的时间来映射出一个表的名字
	 * @param time_t logtime 指定的标量时间
	 * @return 对象内容的一个成员结构
	 */
	virtual char*  get_table_name(time_t logtime);

	virtual ~CtableDate(void){} 
protected:
};

#endif   /* ----- #ifndef CTABLEDATE_INCL  ----- */

