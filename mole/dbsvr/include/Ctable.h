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

class Ctable
{
protected:
	char sqlstr[8192*4];  ///< 存储暂时的SQL语句
	mysql_interface * db; ///< 完成实际SQL语句请求的地方
	char db_name[50]; ///< 没有分解的数据库前缀，大概像USER
	char table_name[50]; ///< 没有分解的表名前缀，大概像t_user
	char db_table_name[100]; ///< 通过用户米米号路由之后的表名，大概像USER.t_user
public:
	Ctable(mysql_interface * db,const char * dbname,const char * tablename); 

	/**
	 * @brief 生成“数据库.表”的名字
	 * @return 生成的表名，指向一块对象内存
	 */
	virtual char*  get_table_name();

	virtual ~Ctable(void){} 
};

#endif   /* ----- #ifndef CTABLE_INCL  ----- */

