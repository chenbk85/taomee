/*
 * =====================================================================================
 * 
 *       Filename:  CtableRoute.h
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

#ifndef  CTABLEROUTE_INCL
#define  CTABLEROUTE_INCL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "proto.h"
#include "Ctable.h"


class CtableRoute : public Ctable {
	protected:
		char table_name_pre[40];
		char db_name_pre[40];
		char id_name[20];
		enum enum_db_type db_type;
		//通过 id 得到表名(格式： dbname.table, 如: USER_00.t_user_pet_00 )
		// 得到的表名保存在 this->tmp_table_name
		// 返回 表名
		virtual char*  get_table_name(uint32_t id);
	public:
		//判断记录是否存在 通过 ID 
		virtual int id_is_existed(uint32_t id, bool * existed);
		// db_type : 数据库类型 
		// table_name_pre : 表名的前面部分：如： t_user_pet_00 --  t_user_pet_99, 写入的是t_user_pet 
		// id_name : 用于id_is_existed方法中
		CtableRoute(mysql_interface * db, const char * db_name_pre,  
		 const char * table_name_pre,const char* id_name ) ;


		int set_int_value(userid_t userid ,const char * field_type , uint32_t  value);
		int get_int_value(userid_t userid ,const char * field_type ,  uint32_t * p_value);
		int change_int_field(userid_t userid ,const char * field_type ,
			   	int32_t changevalue, uint32_t max_value ,uint32_t *p_value );

		int set_int_value_bit(uint32_t userid ,
				const char * field_type ,uint32_t bitid ,uint32_t  value);

		virtual ~CtableRoute(void){} 

};

#endif   /* ----- #ifndef CTABLEROUTE_INCL  ----- */

