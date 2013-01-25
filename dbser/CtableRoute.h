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
#include "Ctable.h"
#include <string> 



class CtableRoute : public Ctable {
	protected:
		char table_name_pre[40];
		char db_name_pre[40];
		char id_name[20];
		char key2_name[20];
		//通过 id 得到表名(格式： dbname.table, 如: USER_00.t_user_pet_00 )
		// 得到的表名保存在 this->tmp_table_name
		// 返回 表名
		virtual char*  get_table_name(uint32_t id);
	public:
		//判断记录是否存在 通过 ID 
		virtual int id_is_existed(uint32_t id, bool * existed);
		virtual int id_is_existed(uint32_t id, uint32_t key2, bool * existed);
		// table_name_pre : 表名的前面部分：如： t_user_pet_00 --  t_user_pet_99, 写入的是t_user_pet 
		// id_name : 用于id_is_existed方法中
		CtableRoute(mysql_interface * db, const char * db_name_pre,  
		 const char * table_name_pre,const char* id_name ,const char* key2_name="") ;


    //使用userid 为主键

        //设置int字段
        int set_int_value(userid_t userid ,const char * field_type , uint32_t  value);
        //得到int字段的值
        int get_int_value(userid_t userid ,const char * field_type ,  uint32_t * p_value);
        //设置int字段中的某一位, bitid  在[1..32]
        int set_int_value_bit(uint32_t userid ,
                const char * field_type ,uint32_t bitid ,uint32_t  value);

        //修改int字段的的值
        // changevalue : 修改多少
        // max_value 最大值
        // p_cur_value 返回当前值 
        // p_real_change_value 返回实际修改多少 
        // is_change_to_max_min  如果计算出的值大于最大值(或小于0)，是否设置为最大值(或0)
        int change_int_value(userid_t userid ,const char * field_type , 
                int32_t changevalue, uint32_t max_value , uint32_t *p_cur_value=NULL,
                int32_t *p_real_change_value=NULL, bool is_change_to_max_min=false);




	//使用userid ,和另一个key  为主键, key 如：装扮表中的 attireid (装扮ID )	
		int set_int_value(userid_t userid ,uint32_t key2 ,const char * field_type , uint32_t  value);
		int get_int_value(userid_t userid,uint32_t key2 ,const char * field_type ,  uint32_t * p_value);
		int set_int_value_bit(uint32_t userid ,uint32_t key2, 
				const char * field_type ,uint32_t bitid ,uint32_t  value);


		int change_int_value(userid_t userid ,uint32_t key2 ,const char * field_type ,
			   	int32_t changevalue, uint32_t max_value , uint32_t *p_cur_value=NULL, 
				int32_t *p_real_change_value=NULL, bool is_change_to_max_min=false);
		virtual ~CtableRoute(void){} 

		int get_insert_sql_by_userid( userid_t userid, std::string & sql_str,
				const char * userid_field_name="userid" ,uint32_t obj_userid=0);
};

#endif   /* ----- #ifndef CTABLEROUTE_INCL  ----- */

