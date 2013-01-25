/*
 * =====================================================================================
 * 
 *       Filename:  CtableString.h
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

#ifndef  CTABLESTRING_INCL
#define  CTABLESTRING_INCL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Ctable.h"


class CtableString : public Ctable {
	protected:
		char table_name_pre[25];
		char db_name_pre[25];
		//通过 id 得到表名(格式： dbname.table, 如: USER_00.t_user_pet_00 )
		// 得到的表名保存在 this->tmp_table_name
		// 返回 表名
		virtual char *get_table_name(const char *  str  );
	public:
		// table_name_pre : 表名的前面部分：如： t_user_pet_00 --  t_user_pet_99, 写入的是t_user_pet 
		// id_name : 用于id_is_existed方法中
		CtableString(mysql_interface * db, const char * db_name_pre,  
		 const char * table_name_pre) ;

		virtual ~CtableString(void){} 
};

#endif   /* ----- #ifndef CTABLESTRING_INCL  ----- */

