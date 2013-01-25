/*
 * =====================================================================================
 * 
 *       Filename:  Csync_user_data.h
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

#ifndef  Csync_user_data_INCL
#define  Csync_user_data_INCL
#include "libtaomee++/utils/tcpip.h" 
#include "Ctable.h" 
struct stru_str_with_len{
        uint32_t    len;
        char        str[];
}__attribute__((packed));

//分表分库的类型定义
enum enum_db_table_type{
	type_100x10=1,
	type_100x100
};

class Csync_user_data {
	protected:
		char sql_str[8192];
		mysql_interface * db;
	public:
		Csync_user_data(mysql_interface * db, char * db_old_ip,short db_old_port ); 

		//功能：检查 dbname_pre.tablename_pre  中"useridname"=userid 是不是存在。
		//(其分表分库的类型是:db_table_type)
		//
		//如果不存在，则去同步,(命令号是get_sql_date_cmdid), 
		//如果已经存在，直接返回SUCC.
		int sync_data(const uint32_t userid,uint16_t get_sql_date_cmdid, 
				const char * dbname_pre,const char * tablename_pre ,
				enum_db_table_type db_table_type=type_100x10,
			   const char * useridname="userid"	);
		virtual ~Csync_user_data(void){} 
	private:
		Ctcp tcp;//用于得到用户在原来机器上的所有数据
		Ctable user_table; //用于检查用户数据是否存在
};

#endif   /* ----- #ifndef Csync_user_data_INCL  ----- */

