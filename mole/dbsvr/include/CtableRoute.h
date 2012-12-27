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
#include "Ctable.h"



class CtableRoute : public Ctable
{
	protected:
		char table_name_pre[40];
		char db_name_pre[40];
		char id_name[20];
		enum enum_db_type db_type;

		/**
		 * @brief 通过 id 得到表名(格式： dbname.table, 如: USER_00.t_user_pet_00 )
		 * 	得到的表名保存在 this->tmp_table_name
		 * @return  表名
		 */
		virtual char*  get_table_name(uint32_t id);
	public:
		/**
		 * @brief 判断记录是否存在 通过 ID 
		 */
		virtual int id_is_existed(uint32_t id, bool * existed);

		/**
		 * @param table_name_pre : 表名的前面部分，如： t_user_pet_00 --  t_user_pet_99, 写入的是t_user_pet 
		 * @param id_name : 用于id_is_existed方法中
		 */
		CtableRoute(mysql_interface * db, const char * db_name_pre,  const char * table_name_pre,const char* id_name ) ;

		/**
		 * @brief 改变一个整型的值，会访问数据库，保证改变后的值不大于max_value
		 * @param uint32_t userid 用于表名路由的用户米米号
		 * @param const char* field_type 数据库表中的字段名
		 * @param int32_t changevalue 改变的值，可以是负值
		 * @param uint32_t max_value 允许的最大值
		 * @param uint32_t* p_value 改变之后的值
		 */
		int change_int_field(uint32_t userid ,const char * field_type ,
			   	int32_t changevalue, uint32_t max_value ,uint32_t *p_value );

		/**
		 * @brief 改变一个整型的值，会访问数据库，不保证改变后的值不大于max_value
		 * @param uint32_t userid 用于表名路由的用户米米号
		 * @param const char* field_type 数据库表中的字段名
		 * @param int32_t changevalue 改变的值，可以是负值
		 * @param uint32_t* p_value 改变之后的值
		 */
		int change_int_field_without_max_check(uint32_t userid ,const char * field_type ,
				                int32_t changevalue, uint32_t *p_value );


		/**
		 * @brief 取得一个整型字段的值
		 * @param uint32_t userid 用于表名路由的用户米米号
		 * @param const char* field_type 数据库表中的字段名
		 * @param uint32_t* p_value 取得的值
		 */
		int get_int_value(uint32_t userid ,const char * field_type ,  uint32_t * p_value);

		/**
		 * @brief 设置一个整型字段的值
		 * @param uint32_t userid 用于表名路由的用户米米号
		 * @param const char* field_type 数据库表中的字段名
		 * @param uint32_t p_value 设置的值
		 */
		int set_int_value(uint32_t userid ,const char * field_type , uint32_t  value);

		/**
		 * @brief 设置一个整型字段中指定位的值
		 * @param uint32_t userid 用于表名路由的用户米米号
		 * @param const char* field_type 数据库表中的字段名
		 * @param uint32_t bitid 第多少位被设置，只能是0-31
		 * @param uint32_t value 设置的值，只能是1或0
		 */
		int set_int_value_bit(uint32_t userid ,const char * field_type ,uint32_t bitid ,uint32_t  value);

		virtual ~CtableRoute(void){} 
};

#endif   /* ----- #ifndef CTABLEROUTE_INCL  ----- */

